--- linux-3.14.17/arch/arm/include/asm/soc-pilot/hwmap.h	2015-08-11 12:32:00.250249353 +0530
+++ linux/arch/arm/include/asm/soc-pilot/hwmap.h	2015-08-11 11:51:20.519104640 +0530
@@ -1,7 +1,4 @@
 /*
- *  This file contains the SE PILOT3 Register Base, IRQ and DMA  mappings
- *
- *  Copyright (C) 2009 ServerEngines
  *  Based on AMI's similar file
  *
  *  Copyright (C) 2005 American Megatrends Inc
@@ -20,9 +17,16 @@
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  */
+/*
+ * Copyright (c) 2010-2015, Emulex Corporation.
+ * Modifications made by Emulex Corporation under the terms of the
+ * GNU General Public License as published by the Free Software
+ * Foundation; either version 2 of the License, or (at your option)
+ * any later version.
+ */
 
-#ifndef     _HWREG_PILOT3_H_
-#define     _HWMAP_PILOT3_H_
+#ifndef     _HWREG_PILOT4_H_
+#define     _HWMAP_PILOT4_H_
 
 
 /********************************************************************
@@ -34,7 +38,7 @@
 #define SE_LPC_ROM_BASE                	(SE_REGISTER_BASE + 0x0100000)	 	/* 256  */
 #define SE_SYS_CLK_BASE                	(SE_REGISTER_BASE + 0x0100100)	 	/* 256  */
 #define SE_SCRATCH_128_BASE            	(SE_REGISTER_BASE + 0x0100200)    	/* 256  */ 
-#define SE_TOP_LVL_PIN_BASE             (SE_REGISTER_BASE + 0x0100900)    	/* 256  */ 
+#define SE_TOP_LVL_PIN_BASE               (SE_REGISTER_BASE + 0x0100900)    	/* 256  */ 
 #define SE_SCRATCH_RAM_BASE            	(SE_REGISTER_BASE + 0x0101000)	 	/* 24 K */ 	
 #define SE_BOOT_SPI_BASE		       	(SE_REGISTER_BASE + 0x0200000)  	/* 1 MB */ 
 #define SE_DDR2_CTRL_BASE              	(SE_REGISTER_BASE + 0x0300000) 	 	/* 2 MB */
@@ -51,6 +55,7 @@
 #define SE_GPIO_BASE				   	(SE_REGISTER_BASE + 0x0420000)	 	/*  4 K */	
 #define SE_SIO_UART_0_BASE			   	(SE_REGISTER_BASE + 0x0421000)	 	/*  4 K */
 #define SE_SIO_UART_1_BASE			   	(SE_REGISTER_BASE + 0x0422000)    	/*  4 K */
+#define SE_PILOT_ESPI   			   	(SE_REGISTER_BASE + 0x0423000)    	/*  4 K */
 #define SE_SYS_WAKEUP_BASE			   	(SE_REGISTER_BASE + 0x0425000)    	/*  4 K */
 #define SE_KCS1_KCS2_BASE			   	(SE_REGISTER_BASE + 0x0426000)    	/*  512 */
 #define SE_KCS3_BASE				   	(SE_REGISTER_BASE + 0x0426200)    	/*  256 */
@@ -81,8 +86,11 @@
 #define SE_I2C_5_BASE	                (SE_REGISTER_BASE + 0x0438000)		/*  4 K */
 #define SE_I2C_6_BASE	                (SE_REGISTER_BASE + 0x0439000)		/*  4 K */
 #define SE_I2C_7_BASE	                (SE_REGISTER_BASE + 0x043A000)		/*  4 K */
+#define SE_I2C_8_BASE	                (SE_REGISTER_BASE + 0x043C000)		/*  4 K */
+#define SE_I2C_9_BASE	                (SE_REGISTER_BASE + 0x043D000)		/*  4 K */
 #define SE_BACKUP_SPI_BASE	            (SE_REGISTER_BASE + 0x0280000)		/*  4 K */
-#define SE_RES_DEB_BASE                 (SE_REGISTER_BASE + 0x0100800)     /* 256  */
+#define SE_RES_DEB_BASE                 (SE_REGISTER_BASE + 0x0100800)
+
 
 /*--------------- Virtual address of the IO Registers Region  ------------*/
 #define SE_REGISTER_VA_BASE				IO_ADDRESS(SE_REGISTER_BASE)
@@ -90,7 +98,7 @@
 #define SE_LPC_ROM_VA_BASE				IO_ADDRESS(SE_LPC_ROM_BASE)
 #define SE_SYS_CLK_VA_BASE				IO_ADDRESS(SE_SYS_CLK_BASE)
 #define SE_SCRATCH_128_VA_BASE			IO_ADDRESS(SE_SCRATCH_128_BASE)
-#define SE_TOP_LVL_PIN_VA_BASE          IO_ADDRESS(SE_TOP_LVL_PIN_BASE) 
+#define SE_TOP_LVL_PIN_VA_BASE                   IO_ADDRESS(SE_TOP_LVL_PIN_BASE) 
 #define SE_SCRATCH_RAM_VA_BASE			IO_ADDRESS(SE_SCRATCH_RAM_BASE)
 #define SE_BOOT_SPI_VA_BASE				IO_ADDRESS(SE_BOOT_SPI_BASE)
 #define SE_DDR2_CTRL_VA_BASE			IO_ADDRESS(SE_DDR2_CTRL_BASE)
@@ -134,152 +142,91 @@
 #define SE_I2C_5_VA_BASE				IO_ADDRESS(SE_I2C_5_BASE)
 #define SE_I2C_6_VA_BASE				IO_ADDRESS(SE_I2C_6_BASE)
 #define SE_I2C_7_VA_BASE				IO_ADDRESS(SE_I2C_7_BASE)
+#define SE_I2C_8_VA_BASE				IO_ADDRESS(SE_I2C_8_BASE)
+#define SE_I2C_9_VA_BASE				IO_ADDRESS(SE_I2C_9_BASE)
 #define SE_BACKUP_SPI_VA_BASE			IO_ADDRESS(SE_BACKUP_SPI_BASE)
 #define SE_RES_DEB_VA_BASE             IO_ADDRESS(SE_RES_DEB_BASE)
 
 /*****************************************************************
 					  IRQ Assignment
 *****************************************************************/
+#define GIC_IRQS	256
+#define NR_IRQS				GIC_IRQS
+#define IRQ_MAC_A		  32
+#define IRQ_MAC_B    	          33
+#define IRQ_FGE                   34
+#define IRQ_USB_1_0               35 //usb host
+#define IRQ_USB2_INT     	  62
+#define IRQ_USB_2_0		  62
+#define IRQ_TIMER_0               43
+#define IRQ_TIMER_1               44
+#define IRQ_GPIO		  40
+#define IRQ_SYS_WAKEUP		  41
+#define IRQ_KCS_BT_SMIC_MAIL	  42
+#define IRQ_UART_2                46
+#define IRQ_UART_3		  47
+#define IRQ_UART_4                48
+#define IRQ_ICMB		  55
+#define IRQ_I2C_0                 49
+#define IRQ_I2C_1                 50
+#define IRQ_I2C_2                 51
+#define IRQ_I2C_3                 52
+#define IRQ_I2C_4                 53
+#define IRQ_I2C_5		          54
+#define IRQ_I2C_6		          56
+#define IRQ_I2C_7		          57
+#define IRQ_I2C_8		          58
+#define IRQ_I2C_9		          59
+#define IRQ_SYS_CLK_SWITCH        36
+#define IRQ_RTC	                  92
+#define IRQ_SECURITY			  37
+#define IRQ_BOMBER			  45
+#define IRQ_BMC_SSP_DOORBELL           	  39
+#define IRQ_SSP_BMC_DOORBELL              38
+#define IRQ_ESPI_VWIRE          76
+#define IRQ_ESPI_OOB            77
+#define IRQ_ESPI_SAFS           78
+#define IRQ_SIO_WDT				79
+#define IRQ_SIO_PSR				80
+#define IRQ_SIO_SWC				81
+#define IRQ_V_UART_0				82
+#define IRQ_V_UART_1				83
+#define IRQ_BMC_WDT				91
+#define IRQ_SDHC1_INT					101
+#define IRQ_EMMC_INT					102
+#define IRQ_PS2					93
+#define IRQ_PWM					94
+#define IRQ_PECI				95
+#define IRQ_DDR2				96
+#define IRQ_SPI					97
+
+#define IRQ_CRYPTO_ENGINE				37
+#define IRQ_BOMBER_INT					45
+#define IRQ_SSP_TO_ARM					38
+#define IRQ_ARM_TO_SSP					39
+#define IRQ_SDHC_INT					100
+#define IRQ_NAND_INT					103
+#define IRQ_AHB2PCI_INT					104
+#define IRQ_TIMER4_INT					87
+#define IRQ_TIMER5_INT					88
+#define IRQ_TIMER6_INT					89
+#define IRQ_I2C7_INT					57
+#define IRQ_I2C8_INT					58
+#define IRQ_PCIE_TEST				107
+#define IRQ_HW_DBG_INT					108
+
+#define IRQ_USB_HUB					62
+#define IRQ_USB_DEV0					63
+#define IRQ_USB_DEV1					64
+#define IRQ_USB_DEV2					65
+#define IRQ_USB_DEV3					66
+#define IRQ_USB_DEV4					67
+#define IRQ_USB_DEV5					68
+#define IRQ_USB_DEV6					69
+#define MAX_SE_ETH		2
 
-#define NR_IRQS							96  
-#define SE_NR_IRQS                     	96  
-#define MAXIRQNUM                       95  
-#define COMBINED_IRQ_START				32
-#define COMBINED2_IRQ_START				64  
-
-#define IRQ_MAC_A		           		0
-#define IRQ_MAC_B    	                1
-#define IRQ_FGE                     	2
-#define IRQ_USB_1_0                     3
-#define IRQ_I2C_4                       4
-#define IRQ_I2C_5						5
-#define IRQ_TIMER_0                     6
-#define IRQ_TIMER_1                     7
-
-#define IRQ_COMBINED_8					8			//Don't Use
-#define IRQ_COMBINED_9					9			//Don't Use
-#define IRQ_USB_2_0						10			
-#define IRQ_COMBINED_11					11			//Don't Use
-#define IRQ_COMBINED_12					12			//Don't Use
-#define IRQ_COMBINED_13					13			//Don't Use
-#define IRQ_COMBINED_14					14			//Don't Use
-#define IRQ_COMBINED_15					15			//Don't Use
-
-#define IRQ_GPIO						16
-#define IRQ_SYS_WAKEUP					17
-#define IRQ_KCS_BT_SMIC_MAIL			18
-#define IRQ_UART_2                      19
-#define IRQ_UART_3						20
-#define IRQ_UART_4                      21
-#define IRQ_ICMB						22	
-#define IRQ_I2C_0                       23
-#define IRQ_I2C_1                       24
-#define IRQ_I2C_2                       25	
-#define IRQ_I2C_3                       26	
-#define IRQ_SYS_CLK_SWITCH            	27
-#define IRQ_SECURITY					28
-#define IRQ_BOMBER						29
-#define IRQ_BMC_SSP_DOORBELL           	30 
-#define IRQ_SSP_BMC_DOORBELL            31
-
-#define IRQ_COMBINED_BIT0				(32+0)		// Don't Use
-#define IRQ_COMBINED_BIT1				(32+1)		// Don't Use
-#define IRQ_COMBINED_BIT2				(32+2)		// Don't Use
-#define IRQ_COMBINED_BIT3_RESERVED		(32+3)		// Don't Use
-#define IRQ_COMBINED_BIT4				(32+4)		// Don't Use
-#define IRQ_SIO_WDT						(32+5)
-#define IRQ_SIO_PSR						(32+6)
-#define IRQ_SIO_SWC						(32+7)
-#define IRQ_V_UART_0					(32+8)
-#define IRQ_V_UART_1					(32+9)
-#define IRQ_COMBINED_BIT10				(32+10)		// Don't Use
-#define IRQ_COMBINED_BIT11				(32+11)		// Don't Use
-#define IRQ_TIMER_2						(32+12)		
-#define IRQ_TIMER_3						(32+13)		
-#define IRQ_BMC_WDT						(32+14)
-#define IRQ_RTC							(32+15)
-#define IRQ_PS2							(32+16)
-#define IRQ_PWM							(32+17)
-#define IRQ_PECI						(32+18)
-#define IRQ_DDR2						(32+19)
-#define IRQ_SPI							(32+20)
-#define IRQ_COMBINED_BIT21				(32+21)		// Don't Use
-#define IRQ_COMBINED_BIT22				(32+22)		// Don't Use
-#define IRQ_COMBINED_BIT23				(32+23)		// Don't Use
-#define IRQ_COMBINED_BIT24				(32+24)		// Don't Use
-#define IRQ_COMBINED_BIT25				(32+25)		// Don't Use
-#define IRQ_COMBINED_BIT26				(32+26)		// Don't Use
-#define IRQ_COMBINED_BIT27				(32+27)		// Don't Use
-#define IRQ_COMBINED_BIT28				(32+28)		// Don't Use
-#define IRQ_COMBINED_BIT29				(32+29)		// Don't Use
-#define IRQ_COMBINED_BIT30				(32+30)		// Don't Use
-#define IRQ_CLOCK_COMTROL	   			(32+31)		
-
-//All these below interrupts represent the combined interrupts in CIRQCTLHI register of Pilot3 spec
-#define IRQ_CRYPTO_ENGINE				(64+0)
-#define IRQ_BOMBER_INT					(64+1)
-#define IRQ_SSP_TO_ARM					(64+2)
-#define IRQ_ARM_TO_SSP					(64+3)
-#define IRQ_USB_COMBIRQ					(64+4)  	
-#define IRQ_SDHC						(64+5)
-#define IRQ_NAND						(64+6)
-#define IRQ_AHB2PCI						(64+7)
-#define IRQ_TIMER_4						(64+8)
-#define IRQ_TIMER_5						(64+8)
-#define IRQ_TIMER_6						(64+10)
-#define IRQ_I2C_6						(64+11)
-#define IRQ_I2C_7						(64+12)
-#define IRQ_PCIE_TEST					(64+13)
-#define IRQ_HW_DBG						(64+14)
-#define IRQ_RESERVED					(64+15) 	//Don't use
-
-/*The below interrupts are SSP interrupts so "Don't use" */
-#define IRQ_RESERVED_16					(64+16)
-#define IRQ_RESERVED_17					(64+17)
-#define IRQ_RESERVED_18             	(64+18)
-#define IRQ_RESERVED_19                 (64+19)
-#define IRQ_RESERVED_20					(64+20)
-#define IRQ_RESERVED_21                 (64+21)
-#define IRQ_RESERVED_22                 (64+22)
-#define IRQ_RESERVED_23                 (64+23)
-#define IRQ_RESERVED_24                 (64+24)
-#define IRQ_RESERVED_25                 (64+25)
-#define IRQ_RESERVED_26                 (64+26)
-#define IRQ_RESERVED_27                 (64+27)
-#define IRQ_RESERVED_28                 (64+28)
-#define IRQ_RESERVED_29                 (64+29)
-#define IRQ_RESERVED_30					(64+30)
-#define IRQ_RESERVED_31					(64+31)
-
-
-/* List of invalid Irq - Using this will fail in Request IRQ */
-#define INVALID_IRQ_MAP                                 \
-{                                                                               \
-8,9,11,12,13,14,15,32,33,34,35,36,42,43,\
-44,45,53,54,55,56,57,58,59,60,61,62,63,  \
-79,80,81,82, \
-83,84,85,86,87,88,89,90,91,92,93,94,95  \
-}
-
-/* Mapping of Actual IRQ (8 to 15) to the Bits in Combined IRQ Control/Status Register */
-#define COMBINED_IRQ_MAP  							\
-{ 													\
-  0x00000003,0x00000004,0x00000000,0x000007F0,		\
-  0x00003800,0x001FC000,0x01E00000,0xFE000000,		\
-}							
-  	
-/* Mapping of Actual IRQ (8 to 15) to the Bits in Combined IRQ Control Register High & Combined IRQ Status Register High  */
-/*CIRQCTLHI & CIRQSTSHI*/
-#define COMBINED2_IRQ_MAP				\
-{							\
-  0x00000000,0x00000000,0x00000010,0x00000000,		\
-  0x00000700,0x000000EF,0x00006000,0x00001800,		\
-}
-/*****************************************************************
-				    APB DMA channel assignment
-*****************************************************************/
-/* TODO */
+#define P4SMPBOOT_BASE_ADDR_F0  0x40100D04
+#define P4SMPBOOT_COMM_ADDR_F0  0x40100D0C
 
-#define MAX_SE_ETH		2
 
 #endif
