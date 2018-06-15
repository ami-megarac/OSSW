--- uboot_old/arch/arm/cpu/pilotcommon/misc.c	2017-02-23 17:33:01.697520923 +0500
+++ uboot/arch/arm/cpu/pilotcommon/misc.c	2017-02-23 17:31:38.063877653 +0500
@@ -0,0 +1,451 @@
+#include <common.h>
+#include <command.h>
+#include <linux/types.h>
+#include "soc_hw.h"
+#include <linux/ctype.h>
+#include <environment.h>
+
+extern int rconsole(void);
+
+#define MAX_UBOOT_VAR_COUNT          64
+#define MAX_UBOOT_VAR_NAME_LENGTH    32
+#define MAX_UBOOT_VAR_VALUE_LENGTH   64
+#define HWREG_ENVVAR_FORMAT          "pilotreg"
+#define HWREG_ENVVAL_DELIMITER       ","
+#define I2C_INTERFACE_RESET 0x0000FF00
+#define ENABLE_SOFTWARE_RESET 0x00003002
+
+/* List of gpio pins which are going to activate in uboot of particular Pilot-III SOC*/
+GPIO GpioTable[] = {
+
+	/* { GPIO_00_0,    GPIO_TYPE_IN,    GPIO_DATA_LOW,    GPIO_PULL_NO } */
+	{ 0xFF,    0xFF,    0xFF,    0xFF }
+};
+
+/* List of Pilot-III SOC Hardware Registers to initialize in uboot*/
+HWREG RegTable[] = {
+
+	/* { 0x40100188, 0x0, 0x220, REGISTER_TYPE_BIT32 } */
+	{ 0xFF,    0xFF,    0xFF,    0xFF }
+};
+
+#define IS_INPUT_PIN(port, pin) (!( READ_REG8( GPIO_BASE_OFFS + pin + port * 0x10) & GPIO_OUTPUT_ENB))
+
+int Readgpio (u8 pindata)
+{
+	unsigned int Addr;
+	unsigned int Value;
+        unsigned char port,pin;
+
+	port = pindata / 8;
+	pin = pindata % 8;
+
+	if(pindata <= 31) 
+	{
+		/* check is it input pin */
+		if (IS_INPUT_PIN(port, pin))
+                {
+			Addr = GPIO_BASE_OFFS + GPIO_GPDI_OFFSET + port * 0x10;
+                }
+		else
+                {
+			Addr = GPIO_BASE_OFFS + GPIO_GPDO_OFFSET + port * 0x10;
+                }
+	}
+	else 
+	{
+		Addr = GPIO_BASE_OFFS + GPIO_GPDI_OFFSET + port * 0x10;
+	}
+
+	/* Get the particular bit */
+      	Value = READ_REG8( Addr );
+
+	Value &= ( 0x01 << pin );
+	Value >>= pin ;
+
+return Value;
+}
+
+void SetGpio(u8 pindata, u8 data)
+{
+        unsigned int Addr;
+        unsigned int Value;
+        unsigned char port,pin;
+
+	port = pindata / 8;
+	pin = pindata % 8;
+
+	Addr = GPIO_BASE_OFFS + GPIO_GPDO_OFFSET + port * 0x10;
+	Value = READ_REG8( Addr );
+
+	if ( data )
+		Value = Value | ( 0x01 << pin );
+	else
+		Value = Value & ~ ( 0x01 << pin);
+
+	WRITE_REG8( Addr, Value );
+
+}
+
+void SetGpioDir(u8 pindata, u8 data, u8 pull)
+{
+        unsigned int Addr;
+        unsigned int Value;
+        unsigned char port,pin; 
+
+	port = pindata / 8;
+	pin = pindata % 8;
+
+	Addr = GPIO_BASE_OFFS + pin + port * 0x10;
+
+	Value = READ_REG8( Addr );
+
+	if ( data == GPIO_DIR_OUT )
+		Value = Value | GPIO_OUTPUT_ENB;
+	else if ( data == GPIO_DIR_IN )
+		Value = Value & ~GPIO_OUTPUT_ENB;
+
+	// add pull-down pull-up pull-no
+	// 0b00: Internal Pulldown enabled
+	// 0b10: Internal Pullup enabled
+	// 0b11: No Pullup/Pulldown
+	if ( pull == GPIO_PULL_DOWN ) {
+		Value = Value & ~GPIO_PULL_NO; // clear bit 2 and 1
+	}
+	else if ( pull == GPIO_PULL_UP ) {
+		Value = Value & ~GPIO_PULL_NO; // clear bit 2 and 1
+		Value = Value |  GPIO_PULL_UP; // set bit 2
+	}
+	else if ( pull == GPIO_PULL_NO ) {
+		Value = Value |  GPIO_PULL_NO; // set bit 2 and 1
+	}
+
+	WRITE_REG8( Addr, Value );
+
+}
+
+
+void InitGPIO(void)
+{
+     unsigned char  index = 0;
+
+     for(index =0 ; index< sizeof(GpioTable)/sizeof(GPIO);index++)
+     {
+
+         if(GpioTable[index].type != GPIO_TYPE_INVALID)
+         {
+
+              if(GpioTable[index].type == GPIO_TYPE_IN)
+              {
+                  SetGpioDir(GpioTable[index].num, GPIO_DIR_IN, GpioTable[index].pull);
+              }
+	 	
+              if(GpioTable[index].type == GPIO_TYPE_OUT)
+              {
+                  SetGpioDir(GpioTable[index].num, GPIO_DIR_OUT, GpioTable[index].pull);
+
+                  if( GpioTable[index].initVal )
+                     SetGpio( GpioTable[index].num, GPIO_DATA_HIGH);
+                  else
+                    SetGpio( GpioTable[index].num, GPIO_DATA_LOW);
+              }
+         }
+    }
+}
+
+
+int WriteRegister(u32 addrs,u32 mask,u32 value,u8 regType)
+{
+    unsigned int regVal;
+
+    if(regType == REGISTER_TYPE_BIT8)
+    {
+        regVal = READ_REG8(addrs);
+        regVal = ( (regVal & mask) | value);
+        WRITE_REG8(addrs,regVal);
+    }
+    else if(regType == REGISTER_TYPE_BIT16)
+    {
+        regVal = READ_REG16(addrs);
+        regVal = ( (regVal & mask) | value);
+        WRITE_REG16(addrs,regVal);
+    }
+    else if(regType == REGISTER_TYPE_BIT32)
+    {
+        regVal = READ_REG32(addrs);
+        regVal = ( (regVal & mask) | value);
+        WRITE_REG32(addrs,regVal);
+    }
+    else
+    {
+       printf("Invalid register type of HW register:%x for given value:%x\n",addrs,value);
+       return -1;
+    }
+
+return 0;
+}
+
+void CheckForEmergencyMode(void)
+{
+    /* OEM's can overwrite this condition accordingly to enter BMC recovery/emergency mode dynamically */
+    /* Eg: considering GPIO value to move BMC into recovery mode for every reboot */
+    //if()
+    //rconsole();  /* Remote Recovery Console */
+
+return;
+}
+
+int IsNumberStr(char *str)
+{
+    u8 i=0;
+    if (*str == '0')
+    {
+        if ((str[1] == 'x') && isxdigit(str[2]))
+            i=2;
+    }
+	 
+    while(*(str+i))
+    {
+        if( !isxdigit(*(str+i)))
+            return -1;
+        i++;
+    }
+    if(i==0)
+      return -1;
+
+return 0;
+}
+
+int ParseDataWriteRegister(char *data)
+{
+    unsigned char regType=0;
+    unsigned int addrs=0,mask=0,value=0;
+    //unsigned int regVal;
+    char str[MAX_UBOOT_VAR_VALUE_LENGTH]={0};
+    char *ptr = NULL;
+ 
+    strcpy(str, data);
+
+    ptr =strtok(str,HWREG_ENVVAL_DELIMITER);
+    if(ptr != NULL)
+    {
+        if( IsNumberStr(ptr) == -1)
+            return -1;
+        regType = simple_strtol(ptr,NULL,0);
+    }
+
+    ptr =strtok(NULL,HWREG_ENVVAL_DELIMITER);
+    if(ptr != NULL)
+    {
+        if( IsNumberStr(ptr) == -1)
+            return -1;
+        addrs = simple_strtoul(ptr,NULL,0);
+    }
+
+    ptr =strtok(NULL,HWREG_ENVVAL_DELIMITER);
+    if(ptr != NULL)
+    {
+        if( IsNumberStr(ptr) == -1)
+            return -1;
+        mask = simple_strtoul(ptr,NULL,0);
+    }
+ 
+    ptr =strtok(NULL,HWREG_ENVVAL_DELIMITER);
+    if(ptr != NULL)
+    {
+        if( IsNumberStr(ptr) == -1)
+            return -1;	 	
+        value = simple_strtoul(ptr,NULL,0);
+    } 
+
+    if( WriteRegister(addrs,mask,value,regType) != 0)
+       return -1;
+
+return 0;
+}
+
+/************************************************************************
+*Gets all the environment variable names
+*/
+static
+int Getenvvar(unsigned char *count,unsigned char *VarName,int *BootVarlen)
+{
+     int i=0,Counter=0;
+     unsigned char *Nextenv;
+     uchar *env,*nxt = NULL;
+
+     uchar *env_data = (uchar *)env_get_addr(0);
+     Nextenv = VarName;
+     for (env=env_data; *env; env=nxt+1)
+     {
+        i=0;
+        for (nxt=env; *nxt; ++nxt)
+        {
+            if(*nxt != '\0')
+            {
+                  if(i==0)
+                  {
+                      *count += 1;
+                   while((*Nextenv++ = *nxt++) != '=' )
+                   {
+                        Counter ++;
+                   };
+                   Nextenv --;
+                   *Nextenv++ = '\0';
+                       i =1;
+                  }
+            }
+              }
+      }
+     *BootVarlen = Counter + *count;
+     return 0;
+}
+
+void  ReadEnvVarsSetHWRegs(void)
+{
+    unsigned char EnvVarsName[MAX_UBOOT_VAR_COUNT * MAX_UBOOT_VAR_NAME_LENGTH];
+    char EnvVar[MAX_UBOOT_VAR_NAME_LENGTH];
+    char EnvVal[MAX_UBOOT_VAR_VALUE_LENGTH];
+    int EnvVarlen =0,len=0;
+    u8 EnvVarCount = 0, Count=0;
+
+    memset(EnvVarsName,0,sizeof(EnvVarsName));
+    Getenvvar (&EnvVarCount,EnvVarsName,&EnvVarlen);
+
+    for(Count = 0;Count<EnvVarCount;Count++)
+    {
+        strcpy(EnvVar,(char *)(EnvVarsName + len));
+        len+=(strlen(EnvVar) + 1);
+        if( !memcmp(EnvVar,HWREG_ENVVAR_FORMAT,strlen(HWREG_ENVVAR_FORMAT)) )
+        {
+            if ( IsNumberStr(EnvVar + strlen(HWREG_ENVVAR_FORMAT)) == -1)
+            {
+                printf("Not a valid env var:%s to set HW register\n",EnvVar);
+                continue;//will continue to set for next register.
+            }
+            
+            if( getenv_f(EnvVar,EnvVal,sizeof(EnvVal)) != -1)
+            {
+                if(ParseDataWriteRegister(EnvVal) == -1)
+                    printf("Can't set HW register for given data %s=%s\n",EnvVar,EnvVal);
+            }
+        }
+    }
+
+return;
+}
+
+void ResetConfigReg(void)
+{
+    WRITE_REG32(SYS_SOFT_RST_EN, I2C_INTERFACE_RESET);
+    WRITE_REG32(SYS_WDT_RST_EN,0x00);
+    WRITE_REG32(SYS_RST_CTRL_REG, ENABLE_SOFTWARE_RESET);
+    WRITE_REG8(PCCT_REG, 0x00); //Disabling Port Capture Ctrl Reg
+    return;
+}
+
+void platformSpecificInit(void)
+{
+    printf("Platform Specific Init happened\n");
+    ResetConfigReg();
+    CheckForEmergencyMode();
+    ReadEnvVarsSetHWRegs();
+
+return;
+}
+
+void
+soc_init(void)
+{
+    /* Do any PILOT-II SOC Initialization here */
+    u8 index = 0;
+    u32 softreset = 0;
+    u32 sys_reset_status_reg = 0;
+
+    for(index = 0;index < ( sizeof(RegTable) / sizeof(HWREG) );index++)
+    {
+        if(RegTable[index].RegType != REGISTER_TYPE_INVALID)
+            WriteRegister(RegTable[index].Address,RegTable[index].Mask,RegTable[index].Val,RegTable[index].RegType);
+    }
+
+    /* Enable Backup SPI */
+    *((volatile u32 *)(SE_TOP_LEVEL_PIN_CTRL_BASE)) = 0;
+
+    softreset = *(volatile unsigned long*)(SE_RES_DEB_BASE+0x04);
+    printf("Soft Reset Value is %x\n",softreset);
+    if((softreset & 0x02) != 0)
+    {
+     /*Soft reset performed clear of the status bits first to avoid
+     unwanted reset when we write in SYSSRER Register */
+     *(volatile unsigned long*)(SE_RES_DEB_BASE+0x04)|=0x2;
+     *(volatile unsigned long*)(SE_RES_DEB_BASE+0x0C)&=0xFFFFFFFE;
+     *(volatile unsigned long*)(SE_RES_DEB_BASE+0x00) = 0x3000;
+    }
+
+    sys_reset_status_reg = *(volatile unsigned long*)(SE_WDT_BMC_VA_BASE+0x00);
+    if((sys_reset_status_reg & 0x02) != 0) 
+    {
+        *(volatile unsigned long*)(SE_WDT_BMC_VA_BASE+0x00) &= 0x02;
+    }
+#ifdef CONFIG_PILOT4
+    sys_reset_status_reg = *(volatile unsigned long*)(SE_RES_DEB_BASE+0xB0);
+    /*Check if system reset was due to any of the three watchdog timeout  */
+    if((sys_reset_status_reg & 0x07) != 0)
+    {
+        if((sys_reset_status_reg & 0x01) != 0)
+        {
+            *(volatile unsigned long*)(SE_RES_DEB_BASE+0x80) &= 0x00060c00;
+        }
+        if((sys_reset_status_reg & 0x02) != 0)
+        {
+            *(volatile unsigned long*)(SE_RES_DEB_BASE+0x90) &= 0x00060c00;
+        }
+        if((sys_reset_status_reg & 0x04) != 0)
+        {
+            *(volatile unsigned long*)(SE_RES_DEB_BASE+0xA0) &= 0x00060c00;
+        }
+    }
+#endif	
+   /* Change CPU freq to max if possible */
+#ifdef  CONFIG_SPX_FEATURE_GLOBAL_PCIE_FUNCTION1_SUPPORT
+    /* Init PCIe Function 1 */
+    WRITE_REG32(0x40440F00, 0x080019A2);
+#ifdef CONFIG_PILOT4
+    WRITE_REG32(0x40440F04, 0x00000001);	  
+#else
+    WRITE_REG32(0x40440F04, 0x00000002);
+#endif     
+    //WRITE_REG32(0x40440F04, 0x00000002);
+    WRITE_REG32(0x40440F08, 0x0B400000); //To Change PCIe Device name from Co-Processor to Management Device
+#ifdef CONFIG_PILOT4   
+    WRITE_REG32(0x40440F0C, 0x00000000);
+#else    
+    WRITE_REG32(0x40440F0C, 0x00000001);
+#endif    
+    WRITE_REG32(0x40440F10, CONFIG_SPX_FEATURE_GLOBAL_PCIE_FUNCTION1_SHARED_MEM_SIZE - 0x10);
+#ifdef CONFIG_PILOT4    
+    WRITE_REG32(0x40440F14, 0x0000FFF0);
+#else    
+    WRITE_REG32(0x40440F14, 0x000000F0);
+#endif    
+    WRITE_REG32(0x40440F18, CONFIG_SPX_FEATURE_GLOBAL_MEMORY_START+CONFIG_SPX_FEATURE_GLOBAL_MEMORY_SIZE-CONFIG_SPX_FEATURE_GLOBAL_PCIE_FUNCTION1_SHARED_MEM_SIZE);
+    WRITE_REG32(0x40440F1C, 0x00000000);
+    WRITE_REG32(0x40440F20, 0x00000000);
+    WRITE_REG32(0x40440F28, 0x00000000);
+    WRITE_REG32(0x40440F2C, 0x080019A2);
+#ifdef CONFIG_PILOT4    
+    WRITE_REG32(0x40440F30, 0xFFFFFFFF);
+#else    
+    WRITE_REG32(0x40440F30, 0x00000000);
+#endif    
+    WRITE_REG32(0x40440F34, 0x00000000);
+    WRITE_REG32(0x40440F38, 0x00000000);
+    WRITE_REG32(0x40440F3C, 0x000002FF);
+    WRITE_REG32(0x4010091C, 0x00000001);
+    WRITE_REG32(0x4010091C, 0x00000007);
+#endif
+
+    /* Inialize all PILOT-III SOC GPIO's */
+    InitGPIO();
+return;
+}
