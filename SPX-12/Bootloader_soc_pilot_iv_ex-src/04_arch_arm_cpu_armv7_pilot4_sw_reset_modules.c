--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/sw_reset_modules.c 1969-12-31 19:00:00.000000000 -0500
+++ mywork/arch/arm/cpu/armv7/pilot4/sw_reset_modules.c 2015-04-22 05:38:35.901900128 -0400
@@ -0,0 +1,310 @@
+/*******************************************************************************
+ *
+ * Copyright (C) 2004-2014 Emulex. All rights reserved.
+ * EMULEX is a trademark of Emulex.
+ * www.emulex.com
+ *
+ * This program is free software; you can redistribute it and/or modify it under
+ * the terms of version 2 of the GNU General Public License as published by the
+ * Free Software Foundation.
+ * This program is distributed in the hope that it will be useful. ALL EXPRESS
+ * OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY IMPLIED
+ * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
+ * NON-INFRINGEMENT, ARE DISCLAIMED, EXCEPT TO THE EXTENT THAT SUCH DISCLAIMERS
+ * ARE HELD TO BE LEGALLY INVALID. See the GNU General Public License for more
+ * details, a copy of which can be found in the file COPYING included
+ * with this package.
+ *
+ ********************************************************************************/
+
+#define tU8 unsigned char
+#define tU32 unsigned long
+#define tPVU8 volatile unsigned char *
+#define tPVU32 volatile unsigned long *
+
+#define VIC 0x40000000
+
+/*BMC CPU Reset-SDK Update*/
+#define SSP_DEV_LOCK			0x40800718
+#define SDRAID_ENABLED			0x1
+#define SE_SYS_CLK_VA_BASE 0x40100100
+
+//mac defines
+#define MAC0BASE			0x40500000
+#define MAC1BASE			0x40600000
+
+#define CSR0BMR				0x0
+#define CSR5SR				0x28
+#define CSR6OMR				0x30
+
+#define CSR0SWR				(0x1<<0)  		//;S/W reset
+#define CSR6SR				(0x1<<1)		//;start/stop receive command
+#define CSR6ST				(0x1<<13)		//;start/stop transmit command
+
+//sdhc
+#define SDHC0_BASE			0x40C00000
+#define SDHC1_BASE			0x40C01000
+#define EMMC_BASE			0x40C02000
+#define PRSNTST_REG			0x024
+
+#define READ_REG8(a)		(*((tPVU8)  (a)))
+#define READ_REG16(a)		(*((tPVU16) (a)))
+#define READ_REG32(a)		(*((tPVU32) (a)))
+
+#define WRITE_REG8(a,d)		(*((tPVU8) (a)) = d)
+#define WRITE_REG16(a,d)	(*((tPVU16) (a)) = d)
+#define WRITE_REG32(a,d)	(*((tPVU32) (a)) = d)
+
+#define SET_BIT8(a,b)		(WRITE_REG8 (a, (READ_REG8(a)  | (tU8) (1<<b))))
+#define SET_BIT16(a,b)		(WRITE_REG16(a, (READ_REG16(a) | (tU16)(1<<b))))
+#define SET_BIT32(a,b)		(WRITE_REG32(a, (READ_REG32(a) | (tU32) (1<<b))))
+#define SET_BIT(a,b)		(SET_BIT32(a,b))
+
+#define CLEAR_BIT8(a,b)		(WRITE_REG8 (a, (READ_REG8(a) & (tU8)(~(1<<b)))))
+#define CLEAR_BIT16(a,b)	(WRITE_REG16(a, (READ_REG16(a) & (tU16)(~(1<<b)))))
+#define CLEAR_BIT32(a,b)	(WRITE_REG32(a, (READ_REG32(a) & (tU32)(~(1<<b)))))
+#define CLEAR_BIT(a,b)		(CLEAR_BIT32(a,b))
+
+void stop_mac_tx_rx(unsigned int mac_base)
+{
+	*(tPVU32)(mac_base + CSR6OMR)&=~( (CSR6SR)|(CSR6ST));
+}
+
+//Disable MAC transmit and Receive port_num = 0 for MAC0 and 1 for MAC1
+int Wait_4_Tx_Rx_Stopped(unsigned int mac_base)
+{
+	//  *(tPVU32)(mac_base + CSR6OMR)&=~( (CSR6SR)|(CSR6ST));
+	tU32 temp=0;
+	tU32 wait_to=0;
+	volatile unsigned int timeout = 0xffff;//BMC CPU Reset-SDK Update
+
+	//Wait for TX to goto stopped state
+	temp = 0xffffffff;
+	wait_to = 0xffff;
+	while(temp)
+	{
+		temp = *(tPVU32)(mac_base + CSR5SR);
+		temp = (temp >> 20)&0x7;
+		wait_to--;
+		if(wait_to==0x0)
+		{
+			wait_to=0xffff;
+
+			break;
+		}
+	}
+
+	//Wait for RX to goto stopped state
+	temp = 0xffffffff;
+	wait_to = 0xffff;
+	while(temp)
+	{
+		temp = *(tPVU32)(mac_base + CSR5SR);
+		temp = (temp >> 17)&0x7;
+		wait_to--;
+		if(wait_to==0x0)
+		{
+			wait_to=0xffff;
+			//nc_printf("wait for mac1  rx to be in IDLE state\n");
+			break;
+		}
+
+		*(tPVU32)(mac_base+CSR0BMR)=CSR0SWR;
+		while((( *(tPVU32)(mac_base+CSR0BMR)) &CSR0SWR) && --timeout);//BMC CPU Reset-SDK Update
+	}
+	return 0;
+}
+
+
+#define FGBBASE 0x40900000
+
+//Disable TFE, BSE & SSP
+void fgb_disable(void)
+{
+  	*(tPVU32)(FGBBASE+0x100)&=(~0x7); //Clear bit 0,1,2 of TFE
+	*(tPVU32)(FGBBASE+0x200)&=(~0x7); //Clear bit 0,1,2 of BSE
+}
+
+/*BMC CPU Reset-SDK Update*/
+#define P4_PILOT_SPEC_BASE 0x40426500
+void reset_psr_interrupts(void)
+{
+	tU8 temp;
+
+	temp = *(tPVU8)(P4_PILOT_SPEC_BASE + 0x45);
+	// clear everything except bit 0 in PCCT register to disable all the
+	// port capture interrupts(not doing RMW to avoid writting '1' to status bit)
+	temp &= 0x1;
+	*(tPVU8)(P4_PILOT_SPEC_BASE + 0x45) = temp;
+	// Disable DMA interrupt in PC
+	*(tPVU8)(P4_PILOT_SPEC_BASE + 0x50) &= 0xFE;
+
+	// disable MessageCapture DMA interrupt(reset bit 0)
+	*(tPVU8)(P4_PILOT_SPEC_BASE + 0x78) &= 0xFE;
+
+	// disable SIO reset interrupt. It will anyway get enabled in the module init
+	// (not doing RMW to avoid writting '1' to SIOCFG lock bit)
+	temp = *(tPVU8)(P4_PILOT_SPEC_BASE + 0x23);
+	temp &= 0xF1;
+	*(tPVU8)(P4_PILOT_SPEC_BASE + 0x23) = temp;
+}
+
+
+void usb_dma_disable(void)
+{
+	//TO-DO
+}
+
+
+//check if SDHC is busy
+void sdhc_not_busy(void)
+{
+	tU32 i;
+	tU32 data;
+	tU32 SDHC_BASE;
+      volatile unsigned int timeout = 0xffff;//BMC CPU Reset-SDK Update
+
+	for(i=0; i<3; i++)
+	{
+		SDHC_BASE = SDHC0_BASE + (i*0x1000);
+
+		data = READ_REG32(SDHC_BASE+0x110);
+		while(((data & 0xf) != 0) && --timeout)//BMC CPU Reset-SDK Update
+			data = READ_REG32(SDHC_BASE+0x110);
+		data = READ_REG32(SDHC_BASE+0x118);
+		timeout = 0xffff;//BMC CPU Reset-SDK Update
+
+		while(((data & 0xff000000) != 0) && --timeout)//BMC CPU Reset-SDK Update
+			data = READ_REG32(SDHC_BASE+0x118);
+		data = READ_REG32(SDHC_BASE+0x11C);
+		timeout = 0xffff;//BMC CPU Reset-SDK Update
+
+		while(((data & 0xf0000000) != 0) && --timeout)//BMC CPU Reset-SDK Update
+			data = READ_REG32(SDHC_BASE+0x11C);
+		data = READ_REG32(SDHC_BASE+0x120);
+		timeout = 0xffff;//BMC CPU Reset-SDK Update
+
+		while(((data & 0xff000000) != 0) && --timeout)//BMC CPU Reset-SDK Update
+			data = READ_REG32(SDHC_BASE+0x120);
+		data = READ_REG32(SDHC_BASE+0x124);
+		timeout = 0xffff;//BMC CPU Reset-SDK Update
+
+		while (((data & 0xf0000000) != 0)&& --timeout)//BMC CPU Reset-SDK Update
+			data = READ_REG32(SDHC_BASE+0x124);
+		data = *(tPVU32)(SDHC_BASE + PRSNTST_REG);
+		timeout = 0xffff;//BMC CPU Reset-SDK Update
+
+		while (((data & 0x00f00000) != 0x00f00000)&& --timeout)//BMC CPU Reset-SDK Update
+			data = *(tPVU32)(SDHC_BASE + PRSNTST_REG);
+		//  DELAY(1000);
+	}
+}
+
+#define Bomber 0x40417000
+void disable_bomber(void)
+{
+	WRITE_REG32(Bomber, 0x00);
+}
+
+#define FGB_SECBASE 0x40A00000
+void gdma_disable(void)
+{
+	while(*(tPVU32)(FGB_SECBASE+0x084)&0x1)
+	{
+		//   nc_printf("Gdma is busy \n");
+		;
+	}
+	*(tPVU32)(FGB_SECBASE+0x080)=0x0;
+}
+
+#define NAND_BASE 0x40D00000
+//Checks if nand is busy.
+void nand_not_busy(void) 
+{
+	tU32 data;
+	tU32 time_out=0xffffff;//BMC CPU Reset-SDK Update
+	data = READ_REG32(NAND_BASE+0x18);
+	while(data & 0x700) 
+	{
+		data = READ_REG32(NAND_BASE+0x18);
+		//  nc_printf("NAND BUSY\n");
+		time_out--;
+		if(time_out==0x0)
+			break;
+	}
+
+
+	time_out=0xffffff;//BMC CPU Reset-SDK Update
+	data = READ_REG32(NAND_BASE+0x30);
+	while(data & 0x7) 
+	{
+		data = READ_REG32(NAND_BASE+0x30);
+		//  nc_printf("NAND BUSY\n");
+		time_out--;
+		if(time_out==0x0)
+			break;
+
+	}
+}
+
+#define SYSRCR3         	0x40100860
+#define SYSSRERL3			0x40100864
+#define SYSSRERH3			0x40100868
+
+void do_soft_reset(void) 
+{
+/*BMC CPU Reset-SDK Update*/
+unsigned int modules_reset_rerl = 0xCB58FF3C;
+	unsigned int modules_reset_rerh = 0x3F;
+
+	modules_reset_rerl = 0xCB58FF3C;	//excluding ARM,FGB, DMA AHB,V1,V2, 8051 & BOOT SPI
+
+	if((*(tPVU32)SSP_DEV_LOCK & SDRAID_ENABLED) == SDRAID_ENABLED)
+	{
+		modules_reset_rerl &= ~((1<<5) | (1<<30));	//Dont reset SD0 and USB	
+		modules_reset_rerh &= ~(1<<0);	//Dont reset SD1
+	}
+
+#ifndef CONFIG_PILOT_MMC
+	modules_reset_rerl &= ~(1<<30);	//Dont reset SD0
+	modules_reset_rerh &= ~((1<<0) | (1<<5));	//Dont reset SD1 and EMMC
+#endif
+
+	WRITE_REG32(SYSSRERL3, modules_reset_rerl);
+	WRITE_REG32(SYSSRERH3, modules_reset_rerh);	//Reset eMMC,PCIE RC,I2c9,I2C8,MCTP,SD1 excluding BMC SPI
+	WRITE_REG32(SYSRCR3, 0xC01);	//Issue soft reset to modules selected above
+
+	while((READ_REG32(SYSRCR3) & 0x01) == 0x01);	//Wait till Reset happens
+}
+
+void sw_reset_modules(void)
+{
+	*(unsigned int *)(VIC)=0x0;
+	stop_mac_tx_rx(MAC0BASE);
+	stop_mac_tx_rx(MAC1BASE);
+
+	sdhc_not_busy();
+	usb_dma_disable();
+#ifndef CONFIG_BOMBER_PWM_WORKAROUND//BMC CPU Reset-SDK Update
+	/*Dont touch the bomber, leave it the one who uses it*/
+	//disable_bomber();
+#endif
+	gdma_disable();
+
+	nand_not_busy();
+	fgb_disable();
+
+	Wait_4_Tx_Rx_Stopped(MAC0BASE);
+	Wait_4_Tx_Rx_Stopped(MAC1BASE);
+
+
+	// disable PSR module related interrupts to avoid any driver load dependencies.
+	// These interrupts anyways gets enabled when the respective driver gets loaded
+	// (or when the application enables them)
+	reset_psr_interrupts();
+	/*BMC CPU Reset-SDK Update_Winbod SPI need set SPI clock divder to default value*/
+	*(volatile unsigned long*)(SE_SYS_CLK_VA_BASE+0x20) &= 0xFFFFF000;
+	*(volatile unsigned long*)(SE_SYS_CLK_VA_BASE+0x20) |= 0x223;
+	do_soft_reset();
+	return;  
+}
