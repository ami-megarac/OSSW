--- uboot_old/arch/arm/cpu/pilotcommon/wdt.c	1970-01-01 05:30:00.000000000 +0530
+++ uboot/arch/arm/cpu/pilotcommon/wdt.c	2017-05-03 12:03:18.555068442 +0530
@@ -0,0 +1,130 @@
+#include <pilot_hw.h>
+#include <common.h>
+#define WATCHDOG_WDCTL_RUN 0x01
+#define WATCHDOG_SYSRCR_ENABLE 0x01
+#define WATCHDOG_SYSWRER_VALUE 0x00
+
+#define WATCHDOG_RESET_ARM 0x01
+#define WATCHDOG_RESET_I2C 0xFF00
+
+#define WATCHDOG_TRIGGER 0x80
+
+
+#define BOOT_SRC_MASK 0x00004000
+#define RST_STAT_CLEAR 0x00000000
+#define SYS_WDT_CTRL_ENABLE 0x00000001
+#define WDT_MAX_TIMEOUT 0x0000FFFF
+#define SYS_WDT_RUN     0x00010000
+#define SYS_WDT_RELOAD  0x00800000
+
+#define BOOTSPI_INTERFACE_MASK 0x00000080
+#define ARM_CPU_MASK           0x00000001
+
+
+#define SYS_WDT_RST_STATUS_REG_SCRACHPAD 0x4010027C // since P4 is not able to retrive the SYS_WDT_RST_STATUS_REG values, a work around has been impemented to store it in scratchpad.
+
+
+/**
+ * @fn Enable_watchdog
+ * @brief Enables watchdog reset for modules ARM and I2C devices.
+ * @param[in] wdt_timeout_status - reset status.
+ * @retval  0 - on success.
+ */
+int Enable_watchdog(unsigned long *watchdog_reset_status)
+{
+
+#ifdef CONFIG_PILOT4	
+	/* Disable Watchdg 1 */
+    (*((volatile unsigned long *)(SYS_WDT1_RESET_CTRL_REG))) &= (~SYS_WDT_CTRL_ENABLE);
+
+	/* Check the reset status */
+#if 0
+	*watchdog_reset_status = (((*((volatile unsigned long *)(SYS_WDT_RST_STATUS_REG)))) & 0x00000001);
+#endif
+	/*
+	 *P4 has issues in retriving SYS_WDT_RST_STATUS_REG upon reset. implemeted work around to store the values in scratcpad reg
+	 */
+	*watchdog_reset_status = (((*((volatile unsigned long *)(SYS_WDT_RST_STATUS_REG_SCRACHPAD))) ) & 0x00000001);
+
+	/* clear system reset status */
+    (*((volatile unsigned long *)(SYS_WDT_RST_STATUS_REG))) &= RST_STAT_CLEAR;	
+
+    /*increase watch dog timeout*/
+    (*((volatile unsigned long *)(SYS_WDT1_CONF_REG))) |= WDT_MAX_TIMEOUT;
+
+    /*set arm processor and SPI controllor routines to reset*/
+    (*((volatile unsigned long *)(SYS_WDT1_RESET_ENABLE_LOW_REG))) = (BOOTSPI_INTERFACE_MASK || ARM_CPU_MASK);
+
+
+    /* Enable watch dog */
+    (*((volatile unsigned long *)(SYS_WDT1_RESET_CTRL_REG))) |= (SYS_WDT_CTRL_ENABLE); 
+
+    /*start watchdog */
+    (*((volatile unsigned long *)(SYS_WDT1_RESET_CTRL_REG))) |= (SYS_WDT_RUN);
+
+    /*REload max value*/
+    (*((volatile unsigned long *)(SYS_WDT1_RESET_CTRL_REG))) |= (SYS_WDT_RELOAD);
+
+#else
+       int wdt_count=300; // 5 minutes
+
+       *((volatile unsigned long *)(WDT_CONTROL_REG)) &= ~WATCHDOG_WDCTL_RUN; 
+       *((volatile unsigned long *)(SYS_RST_CTRL_REG)) |= WATCHDOG_SYSRCR_ENABLE;
+
+       /*pilot spec  recommends to reset only ARM processor */
+       *((volatile unsigned long *)(SYS_WDT_RST_EN)) = WATCHDOG_RESET_ARM; //Watchdod reset to ARM
+       *((volatile unsigned long *)(SYS_WDT_RST_EN)) |= WATCHDOG_RESET_I2C; //Watchdog reset to I2C
+
+       *((volatile unsigned long *)(SE_SYS_CLK_VA_BASE  + 0x00)) &= 0xFFFFFFFC;
+      
+       /* Check whether reset caused by watchdog, bit-0 indicates reset cause */
+       *watchdog_reset_status = *((volatile unsigned long *)(SYS_RST_STAT_REG)) & 0x01;
+       /* Clearing the Watdog Reset Status bit*/
+       *((volatile unsigned long *)(SYS_RST_STAT_REG)) |= 0x01;
+       /* Setting Timeout value to 5 minutes */
+
+
+       //To configure watchdog Timer count
+       /*  
+       char *wdt_test = 0;
+       wdt_test = getenv("wdtcnt");
+       if (wdt_test != 0)
+       {
+                wdt_count = (int)simple_strtoul(wdt_test,0,0);
+       }
+       */
+ 
+       *((volatile unsigned long *)(WDT_LOAD_REG0)) = wdt_count & 0x000000FF;
+       *((volatile unsigned long *)(WDT_LOAD_REG1)) = (wdt_count & 0x0000FF00)>>8;
+
+       // *((volatile unsigned long *)(WDT_LOAD_REG0)) = 0x2c;
+       // *((volatile unsigned long *)(WDT_LOAD_REG1)) = 0x1;
+
+       *((volatile unsigned long *)(WDT_CONTROL_REG)) |= WATCHDOG_WDCTL_RUN; 
+       /* Trigger Watchdog Timer */
+       *((volatile unsigned long *)(WDT_CONTROL_REG)) |= WATCHDOG_TRIGGER;
+ #endif
+return 0;
+}
+
+/**
+ * @fn Disable_watchdog
+ * @brief Disables watchdog reset.
+ * @param[in] void.
+ * @retval  0 - on success.
+ */
+int Disable_watchdog(void)
+{
+#ifdef CONFIG_PILOT4	
+    /* Disable system watchdog 1 which is used for software failsafe*/
+    (*((volatile unsigned long *)(SYS_WDT1_RESET_CTRL_REG))) &= (~SYS_WDT_CTRL_ENABLE);
+    
+    /* system watchdog 2 will be enabled in kernel.. for BMC cpu reset workaround . so disable it too */
+    (*((volatile unsigned long *)(SYS_WDT2_RESET_CTRL_REG))) &= (~SYS_WDT_CTRL_ENABLE);
+#else
+     *((volatile unsigned long *)(WDT_CONTROL_REG)) &= ~WATCHDOG_WDCTL_RUN;
+#endif
+return 0;
+}
+
+
