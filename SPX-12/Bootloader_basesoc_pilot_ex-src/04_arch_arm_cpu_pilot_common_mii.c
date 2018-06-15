--- u-boot-2013.07/arch/arm/cpu/pilotcommon/pilotmii.c	1016-09-21 16:56:01.867970127 +0800
+++ mywork/arch/arm/cpu/pilotcommon/pilotmii.c	2016-09-21 16:45:08.627970127 +0800
@@ -0,0 +1,425 @@
+#include <common.h>
+#if (CONFIG_CMD_NET)
+#include <asm/processor.h>
+#include <miiphy.h>
+#include "soc_hw.h"
+#include "pilotmac.h"
+
+#define inl(addr) 		(*((volatile u32 *)(addr)))
+#define outl(addr,val) 	(*((volatile u32 *)(addr)) = (val))
+
+#define PILOT_RT_PHYSR	0x11 
+//Reg 0x11 defines - PHY specific Status register
+#define REG0x11_FULL_DUPLX_MODE         0x00002000
+#define REG0x11_10Mbps                  0x00000000
+#define REG0x11_100Mbps                 0x00004000
+#define REG0x11_1000Mbps                0x00008000
+#define Mircel_Phy_Control              0x1F
+
+/***************************** Tulip  MII read/write access macros *************************/
+#define	AST_MII_BIT_READ(Addr, pBData)                           		\
+   	{                                                           			\
+    	outl((Addr)+AST_MAC_CSR9,CSR9_MII_RD);								\
+        udelay(2);                                            				\
+        outl((Addr)+AST_MAC_CSR9,CSR9_MII_RD | CSR9_MDC);					\
+        udelay(2);                                                     		\
+        *(pBData) |= CSR9_MII_DBIT_RD (inl ((Addr)+ AST_MAC_CSR9));  		\
+   	}
+
+#define	AST_MII_BIT_WRITE(Addr, data)                                 		\
+    {                                                               		\
+    	outl((Addr)+AST_MAC_CSR9, CSR9_MII_DBIT_WR(data) | CSR9_MII_WR);	\
+        udelay(2);                                                     		\
+    	outl((Addr)+AST_MAC_CSR9, CSR9_MII_DBIT_WR(data) | CSR9_MII_WR | CSR9_MDC);				\
+        udelay(2);                                                     		\
+    }
+
+#define	AST_MII_RTRISTATE(Addr)                                       		\
+	{                                                               		\
+        int retBData;                                                     	\
+        AST_MII_BIT_READ ((Addr), &retBData);                          		\
+    }
+
+#define	AST_MII_WTRISTATE(Addr)                                       		\
+	{                                                               		\
+        AST_MII_BIT_WRITE((Addr), 0x1);                               		\
+        AST_MII_BIT_WRITE((Addr), 0x0);                               		\
+	}
+
+#define AST_MII_WRITE(Addr, data, bitCount)                           		\
+   	{                                                               		\
+        int i=(bitCount);                                               	\
+                                                                        	\
+        while (i--)                                                    	 	\
+            AST_MII_BIT_WRITE ((Addr), ((data) >> i) & 0x1);          		\
+    }
+
+#define	AST_MII_READ(Addr, pData, bitCount)                           		\
+   	{                                                               		\
+        int i=(bitCount);                                               	\
+                                                                        	\
+        while (i--)                                                     	\
+        {                                                           		\
+            *(pData) <<= 1;                                             	\
+	        AST_MII_BIT_READ ((Addr), (pData));                      		\
+        }                                                           		\
+    }
+
+int
+GetDevNum(const char *devname)
+{
+	int num = -1;
+
+	/* Assumption: single digits 0 to 9 only and occurs at last char */
+	while (*devname)
+	{
+		num= *devname;
+		devname++;
+	}
+	if ((num < '0') || ( num  > '9'))
+		return -1;
+
+	/* Convert char to integer */
+	num-='0';
+
+	return num;
+}
+
+
+static
+u32
+getmacaddr(const char *devname)
+{
+	int num;
+
+	if (devname == NULL)
+		return 0;
+	num = GetDevNum(devname);
+	if (num == -1)
+		return 0;
+	switch (num)
+	{
+		case 1:
+	// Pilot-II does not have MDIO/MDC signals for the second MAC
+	// So normally, we set the two PHY to different address and connect 
+	// both the PHY to the same MDIO/MDC signals from the first MAC
+	//		return SE_MAC_B_BASE;
+		case 0:
+			return SE_MAC_A_BASE;
+		default:
+			break;
+	}
+	return 0;
+}
+
+
+int
+ast_miiphy_read (const char *devname, unsigned char addr, unsigned char reg,  unsigned short *value)
+{
+	u32 macaddr;
+
+    	macaddr = getmacaddr(devname);
+    	if (macaddr == 0)
+		return -1;
+
+    /* Write 34-bit preamble */
+    AST_MII_WRITE (macaddr, MII_PREAMBLE, 32);
+
+    /* start of frame + op-code nibble */
+    AST_MII_WRITE (macaddr, MII_SOF | MII_RD, 4);
+
+    /* device address */
+    AST_MII_WRITE (macaddr, addr, 5);
+    AST_MII_WRITE (macaddr, reg, 5);
+
+    /* turn around */
+    AST_MII_RTRISTATE (macaddr);
+
+#ifdef CONFIG_PHY_CMD_DELAY
+	udelay (CONFIG_PHY_CMD_DELAY);		/* Intel LXT971A needs this */
+#endif
+
+    /* read data */
+    AST_MII_READ (macaddr, value, 16);
+
+	/* turn around */
+    AST_MII_RTRISTATE (macaddr);
+
+//	printf("Phy Read Value = %x Phy Address = %x Reg = %x\n",*value,addr,reg);
+	return 0;
+}
+
+int
+ast_miiphy_write (const char *devname, unsigned char addr, unsigned char reg, unsigned short value)
+{
+	u32 macaddr;
+
+	macaddr = getmacaddr(devname);
+	if (macaddr == 0)
+		return -1;
+
+    /* write 34-bit preamble */
+    AST_MII_WRITE (macaddr, MII_PREAMBLE, 32);
+
+    /* start of frame + op-code nibble */
+    AST_MII_WRITE (macaddr, MII_SOF | MII_WR, 4);
+
+    /* device address */
+    AST_MII_WRITE (macaddr, addr, 5);
+    AST_MII_WRITE (macaddr, reg, 5);
+
+    /* turn around */
+    AST_MII_WTRISTATE (macaddr);
+
+#ifdef CONFIG_PHY_CMD_DELAY
+	udelay (CONFIG_PHY_CMD_DELAY);		/* Intel LXT971A needs this */
+#endif
+    /* write data */
+    AST_MII_WRITE (macaddr,value, 16);
+
+	return 0;
+
+}
+
+int
+GetLinkStatus(char *devname)
+{
+	u16 status;
+
+	unsigned char addr;
+	int devnum;
+	u16 phyid=0;
+	
+	devnum = GetDevNum(devname);
+	switch (devnum)
+	{
+		case 0:
+			addr = CONFIG_PHY_ADDR0;
+			break;
+		case 1:
+			addr = CONFIG_PHY_ADDR1;
+			break;
+		default:
+			return 0;
+	}
+	
+	/* Check if PHY is preset. If not treat as NC-SI */
+     	ast_miiphy_read(devname, addr, MII_PHY_ID1,&phyid);
+//	printf("Get Link Status : PHYID = %x\n",phyid);
+	if (phyid == 0xffff)
+	{
+		printf("(%s): No PHY Found. Setting Link Up\n",devname);
+		return 1;			/* Link is up */
+	}
+
+	/* Needs two reads of Status reg to get correct link status */
+	ast_miiphy_read (devname,addr,MII_PHY_SR,&status);
+	ast_miiphy_read (devname,addr,MII_PHY_SR,&status);
+	if (!(status & MII_PHY_SR_LNK))	/* Link Status */
+		return 0;
+//	if (!(status & MII_PHY_SR_AN))		/* Auto Nego Capability */
+	if (!(status & MII_PHY_SR_ASS))		/* Auto Nego Complete */		
+		return 0;
+	return 1;
+}
+
+int
+GetPhySpeed(char *devname)
+{
+	unsigned char addr;
+	int devnum;
+	
+	devnum = GetDevNum(devname);
+	switch (devnum)
+	{
+		case 0:
+			addr = CONFIG_PHY_ADDR0;
+			break;
+		case 1:
+			addr = CONFIG_PHY_ADDR1;
+			break;
+		default:
+			return _100BASET;
+	}
+	
+#ifdef CONFIG_PILOT4
+	u16 phyid=0;
+	u16 status;
+    ast_miiphy_read(devname, addr, MII_PHY_ID1, &phyid);
+    
+    if(((phyid&0x3f0)>>0x4)==0x11)
+    {//if realtek
+	if(( phyid & 0xf ) == 0x4) {
+	    return miiphy_speed(devname, addr);
+	}
+	else {
+            ast_miiphy_read (devname,addr,PILOT_RT_PHYSR,&status);
+            
+            if((status & (REG0x11_1000Mbps | REG0x11_100Mbps | REG0x11_10Mbps)) == REG0x11_100Mbps){
+  	           return _100BASET;	/* 100 Mbps */
+            } else if ((status & (REG0x11_1000Mbps | REG0x11_100Mbps | REG0x11_10Mbps)) == REG0x11_1000Mbps){
+  	           return _1000BASET;	/* 1000 Mbps */
+            } else {
+  	           return _10BASET;	/* 10Mbps */
+            }     
+	}
+     }
+    
+     else if((((phyid&0x3f0)>>0x4)==0x21) || (((phyid&0x3f0)>>0x4) == 0x22)) //Shivah Added
+     {
+        //if mircel
+        ast_miiphy_read (devname,addr,Mircel_Phy_Control,&status);
+        printf("Mircel Phy Speed Sts is %x\n",((status>>3)&(0xf)));
+        switch( ( (status>>3)&(0xf) ) )
+  	    {
+  	        case 2:
+  	        case 3:
+  	        	return _10BASET;
+  	        case 4:
+  	        case 5:
+  	        	return _100BASET;
+  	        case 8:
+  	        case 9:
+  	        	return _1000BASET;
+  	        default:
+  	        	printf("Mircel Phy Speed Unknown Sts is %x\n",status);	  
+  	    }
+     }
+    
+     else if( (((phyid&0x3f0)>>0x4)==0x0f) || (((phyid&0x3f0)>>0x4)==0x0c) )
+     {
+        ast_miiphy_read (devname,addr,31,&status);
+        if( (status&0xc)==0x4)
+        {
+        	return _10BASET;
+        }
+        else if( (status&0xc)==0x8)
+        {
+        	return _100BASET;
+        }
+     }
+    
+     else if( ((phyid&0x3f0)>>0x4) == 0x1D )
+     {
+        ast_miiphy_read (devname,addr,17,&status);
+        printf("Marvell GetPhySpeed status is 0x%x\n", ((status >> 14) & 0x3));
+        if( status & 0x800 )   /* Speed & Duplex Resolved bit is set */
+        {
+        	status = ((status >> 14) & 0x3);
+        	switch(status) 
+        	{
+        		case 2:
+        			return _1000BASET;
+        		case 1:
+        			return _100BASET;
+        		case 0:
+        			return _10BASET;
+        		default:
+        			printf("Phy speed is wrong \n");
+        			return 0;
+        	} 
+        }
+     }
+    
+     else
+     {
+    	 printf("GetPhySpeed Phy Id is not matching %x\n", phyid);
+     }
+#endif
+	return miiphy_speed (devname,addr);
+}
+
+int
+GetPhyDuplex(char *devname)
+{
+	unsigned char addr;
+	int devnum;
+	
+	devnum = GetDevNum(devname);
+	switch (devnum)
+	{
+		case 0:
+			addr = CONFIG_PHY_ADDR0;
+			break;
+		case 1:
+			addr = CONFIG_PHY_ADDR1;
+			break;
+		default:
+			return FULL;
+	}
+#ifdef CONFIG_PILOT4
+	u16 phyid=0;
+	u16 status;
+    ast_miiphy_read(devname, addr, MII_PHY_ID1, &phyid);
+	if(((phyid&0x3f0)>>0x4)==0x11)
+	{//if realtek
+		if(( phyid & 0xf ) == 0x4) {
+			return miiphy_duplex(devname,addr);
+		}
+		else {
+			ast_miiphy_read (devname,addr,PILOT_RT_PHYSR,&status);
+			if(status & REG0x11_FULL_DUPLX_MODE)
+			{
+				return FULL;  /* FD */
+			}
+			else 
+			{
+				return HALF;	 /* HD */
+			}
+		}
+	}
+
+	else if((((phyid&0x3f0)>>0x4)==0x21) || (((phyid&0x3f0)>>0x4) == 0x22)) //Shivah  Changed
+	{
+		//if mircel
+		ast_miiphy_read (devname,addr,Mircel_Phy_Control,&status);
+		printf("Mircel Phy Duplex Sts is %x\n",((status>>3)&(0xf)));
+		switch( ( (status>>3)&(0xf) ) )
+		{
+			case 3:
+			case 5:
+			case 9:
+			return FULL;
+			case 2:
+			case 4:
+			case 8:
+			return HALF;
+			default:
+			printf("Mircel Phy Speed/duplex Unknown Sts is %x\n",status);
+		}
+	}
+	
+	else if( (((phyid&0x3f0)>>0x4)==0x0f)|| (((phyid&0x3f0)>>0x4)==0x0c) )
+	{
+		ast_miiphy_read (devname,addr,31,&status);
+		if(status & 0x10)
+		{
+			return FULL;  /* FD */
+		}
+		else 
+		{
+			return HALF;	 /* HD */
+		}
+	}
+	
+	else if( ((phyid&0x3f0)>>0x4) == 0x1D )
+	{
+		ast_miiphy_read (devname,addr,17,&status);
+		printf("Marvell Phy Duplex Sts is %x\n", status);
+		if(status & 0x800)
+		{
+			if(status & 0x2000)	
+				return FULL;  /* Full Duplex */
+			else
+				return HALF;  /* Half Duplex */
+		}
+	}
+	
+	else{
+		printf("GetPhyDuplex Phy Id is not matching %x\n", phyid);
+	}
+	
+#endif
+	return miiphy_duplex(devname,addr);
+}
+#endif
