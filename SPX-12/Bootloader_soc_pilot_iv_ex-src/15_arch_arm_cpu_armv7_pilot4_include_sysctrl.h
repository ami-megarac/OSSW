--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/include/sysctrl.h	1969-12-31 19:00:00.000000000 -0500
+++ mywork/arch/arm/cpu/armv7/pilot4/include/sysctrl.h	2015-02-06 05:06:37.000000000 -0500
@@ -0,0 +1,166 @@
+/*******************************************************************************
+ *
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
+#define SYSCONFIG               (SYSCONTROL)
+#define SCLKSTS			(SYSCONTROL + 0x4)              
+#define SCLKCTL			(SYSCONTROL + 0x8)              
+#define PLLSTS			(SYSCONTROL + 0x10)    //do not exist          
+#define DDR_PLLCTL              (SYSCONTROL + 0x10)
+#define MISCCLKCTL              (SYSCONTROL + 0x20)
+#define I2CPCT0		        (SYSCONTROL + 0x24)
+#define I2CPCT1		        (SYSCONTROL + 0x28)
+#define UART_CTRL               (SYSCONTROL + 0x2C)
+#define IO_EN_CTRL		(SYSCONTROL + 0x32)
+#define	SYS_IRQ_STS		(SYSCONTROL + 0x40)
+#define	PIDBG0  		(SYSCONTROL + 0x54)
+#define SYS_SIO_CTRL            (SYSCONTROL + 0x58)
+#define SYS_MCSS_CLK_CTRL       (SYSCONTROL + 0x84)
+#define MAC_RGMII_CTRL  	(SYSCONTROL + 0x88)
+#define ECO_CONTROL             (SYSCONTROL + 0x90)
+#define FGB_INTR_SWAP_REG       SYSCONTROL
+#define F1_GPIOEN               (SYSCONTROL + 0x800)
+#define PINMUX_CTL              (SYSCONTROL + 0x804)
+#define F2_GPIOEN               (SYSCONTROL + 0x808)
+#define SW_STRAP_CTL            (SYSCONTROL + 0x818)
+#define PCIE_FUNC_TEST          (SYSCONTROL + 0x81C)
+#define I2C_SMB_CTRL_2          (SYSCONTROL + 0x908)
+#define I2CPCT2		        (SYSCONTROL + 0x910)
+
+#define SYSRCR                  (SYSCONTROL + 0x700)
+#define SYSRSTS                 (SYSCONTROL + 0x704)
+#define SYSWRER                 (SYSCONTROL + 0x708)
+#define SYSSRER                 (SYSCONTROL + 0x70C)
+#define SYSBDCR                 (SYSCONTROL + 0x720)
+#define SYSBDSR1                (SYSCONTROL + 0x724)
+#define SYSBDSR2                (SYSCONTROL + 0x728)
+
+
+// MAC DELAY LINE CONTROL BITS
+#define MAC0_TXCLK_DELAYLINE_EN (1<<9)
+#define MAC0_RXCLK_DELAYLINE_EN (1<<4)
+#define MAC1_TXCLK_DELAYLINE_EN (1<<25)
+#define MAC1_RXCLK_DELAYLINE_EN (1<<20)
+
+// ECO Control Register bits
+#define DIS_MAC0_INT_ARB_ECO    (1<<8)
+#define DIS_MAC0_RLSM_ECO       (1<<9)
+#define DIS_MAC1_INT_ARB_ECO    (1<<10)
+#define DIS_MAC1_RLSM_ECO       (1<<11)
+//syscontrol register offsets
+#define  SYSCTRL_REVID          0x50
+
+// PLL Control and Status register bits
+#define SYS_PLLLOCK		(1<<14)
+#define DDR_PLLLOCK		(1<<29)
+#define	PLLLOCK     		(SYS_PLLLOCK + DDR_PLLLOCK)
+
+/// IRQ Swap register in SYSCONFIG
+#define  ARMF2I                 (1<<8)
+#define  SSPF2I                 (1<<9)
+#define  ARMI2F                 (1<<10)
+#define  SSPI2F                 (1<<11)
+#define  SW_STRAP_EN            (1<<31)
+
+
+// GPIO EN registers for Muxed pins
+#define GPIO16_EN               (F1_GPIOEN + 0x0)
+#define GPIO17_EN               (F1_GPIOEN + 0x1)
+#define GPIO18_EN               (F1_GPIOEN + 0x2)
+
+#define GPIO21_EN               (F2_GPIOEN + 0x0)
+#define GPIO22_EN               (F2_GPIOEN + 0x1)
+#define GPIO23_EN               (F2_GPIOEN + 0x2)
+#define GPIO24_EN               (F2_GPIOEN + 0x3)
+#define GPIO25_EN               (F2_GPIOEN + 0x4)
+#define GPIO26_EN               (F2_GPIOEN + 0x5)
+#define GPIO27_EN               (F2_GPIOEN + 0x6)
+#define GPIO28_EN               (F2_GPIOEN + 0x7)
+#define GPIO29_EN               (F2_GPIOEN + 0x8)
+
+#define PINMUXCTRL              (SYSCONTROL + 0x804)
+
+// PINMUX CTRL Bit Defines
+#define PWM4_ONGPIO_EN          (1<<0)
+#define PWM5_ONGPIO_EN          (1<<1)
+#define PWM6_ONGPIO_EN          (1<<2)
+#define PWM7_ONGPIO_EN          (1<<3)
+#define FAN8_ONGPIO_EN          (1<<4)
+#define FAN9_ONGPIO_EN          (1<<5)
+#define FAN10_ONGPIO_EN         (1<<6)
+#define FAN11_ONGPIO_EN         (1<<7)
+#define FAN12_ONGPIO_EN         (1<<8)
+#define FAN13_ONGPIO_EN         (1<<9)
+#define FAN14_ONGPIO_EN         (1<<10)
+#define FAN15_ONGPIO_EN         (1<<11)
+#define FAN4_ONUART2_EN         (1<<12)
+#define FAN5_ONUART2_EN         (1<<13)
+#define FAN6_ONUART2_EN         (1<<14)
+#define FAN7_ONUART2_EN         (1<<15)
+#define PWM2_ONUART2_EN         (1<<16)
+#define PWM3_ONUART2_EN         (1<<17)
+#define KBDDATA_ONI2C_EN        (1<<18)
+#define KBDBCLK_ONI2C_EN        (1<<19)
+#define MOUSEDATA_ONI2C_EN      (1<<20)
+#define MOUSECLK_ONI2C_EN       (1<<21)
+#define BMCWDO_ONUART3_EN       (1<<22)
+#define SIOWDO_ONUART3_EN       (1<<23)
+#define SIO_LDRQ_ONSCI_EN       (1<<24)
+
+
+// Sys SIO CONTROL bit defines
+#define spi_host_wr_disable_bit (1<<24)
+#define spi_host_rd_disable_bit (1<<25)
+
+// MiscClock Control bit defines
+#define BOOTSPI_SCLK_40MHZ      (4<<0)      
+#define BOOTSPI_SCLK_100MHZ     (1<<0)      
+
+#define BKUPSPI_SCLK_40MHZ      (4<<4)      
+#define BKUPSPI_SCLK_100MHZ     (1<<4)      
+
+#define HSPI_SCLK_40MHZ         (4<<8)      
+#define HSPI_SCLK_100MHZ        (1<<8)   
+   
+#define ARM_SFT_RST_FUNCTION_INSPI (0x4c)
+
+// Software Strap control bits
+#define RGMII1_MODE_STRAP_BIT           (1<<20)
+#define CRTM_EN_STRAP_BIT               (1<<19)
+#define CRTM_SIZE_STRAP_1_BIT           (1<<18)
+#define CRTM_SIZE_STRAP_0_BIT           (1<<17)
+#define HSPI_SWAP_SIZE_STRAP_1_BIT      (1<<16)
+#define HSPI_SWAP_SIZE_STRAP_0_BIT      (1<<15)
+#define HSPI_SWAP_EN_STRAP_BIT          (1<<14)
+#define HSPI_EN_STRAP_BIT               (1<<13)
+#define BAR_STRAP_BIT                   (1<<12)         
+#define INTACPI_STRAP_BIT               (1<<11)
+#define EEPROM_STRAP_BIT                (1<<10
+#define RSTBYP_STRAP_BIT                (1<<9)
+#define SSPJTAG_STRAP_BIT               (1<<8)
+#define UART4_STRAP_BIT                 (1<<7)
+#define SD_STRAP_BIT                    (1<<6)
+#define EXT_BUS_STRAP_BIT               (1<<5)
+#define RGMII_MODE_STRAP_BIT            (1<<4)
+#define MAC1_STRAP_BIT                  (1<<3)
+#define LPC_BOOT_STRAP_BIT              (1<<2)
+#define ROMBOOT_STRAP_BIT               (1<<1)
+#define DDR2_MODE_STRAP_BIT             (1<<0)        
+
+
