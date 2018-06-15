diff -Naur uboot/arch/arm/cpu/pilotcommon/reset.c uboot_new/arch/arm/cpu/pilotcommon/reset.c
--- uboot/arch/arm/cpu/pilotcommon/reset.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot_new/arch/arm/cpu/pilotcommon/reset.c	2016-09-30 13:55:34.316252409 -0400
@@ -0,0 +1,122 @@
+/* Pilot-III  WDT Routines for u-boot reboot */
+
+#include <common.h>
+#include <linux/types.h>
+#include <spiflash.h>
+#include "soc_hw.h"
+
+#ifdef CONFIG_PILOT4
+/*
+ * OK, and resets too.
+ */
+#define SW_RESET_BASE	0x40100000	
+#define SYSSRERL2		0x854
+#define SYSSRERH2		0x858
+#define SYSRCR2		0x850
+void reset_cpu(ulong addr)
+{
+//	volatile unsigned int *base = SW_RESET_BASE;
+
+	int spi_bank = 0;
+
+	// Restore the Boot SPI device to default state  (In uboot code, SPI bank (ractrends.c) starts with index 0)
+  	restore_spidevice_to_default_state(spi_bank);
+
+    // Reset the SPI controller along with ARM CPU reset 
+	*(volatile unsigned int *)(SW_RESET_BASE  + SYSSRERL2) = 0x83;
+	*(volatile unsigned int *)(SW_RESET_BASE  + SYSSRERH2) = 0;
+	*(volatile unsigned int *)(SW_RESET_BASE + SYSRCR2) = 0xD01;
+
+	printf("resetting CPI\n");
+
+	while(1) {;}
+}
+
+#else
+
+#define Pilot2_Combined_Irq_Ctrl_Reg    (SE_SYS_CLK_BASE + 0x38)
+#define Pilot2_Combined_Fiq_Ctrl_Reg    (SE_SYS_CLK_BASE + 0x3C)
+#define Pilot2_Irq_Ctrl_Hi_Reg          (SE_SYS_CLK_BASE + 0x60)
+#define Pilot2_Fiq_Ctrl_Hi_Reg          (SE_SYS_CLK_BASE + 0x64)
+#define SPI_CLK_MAX     0x111
+#define SPI_CLK_DEFAULT 0x444
+void
+reset_cpu  (ulong addr)
+{
+
+  int spi_bank = 0;
+
+  // Restore the Boot SPI device to default state  (In uboot code, SPI bank (ractrends.c) starts with index 0)
+  restore_spidevice_to_default_state(spi_bank);
+
+  /* Disable all interrupts */
+  *((volatile unsigned long*) Pilot2_Irq_intmask_Reg) = 0xFFFFFFFF;
+  *((volatile unsigned long*) Pilot2_Irq_inten_Reg)   = 0;
+  *((volatile unsigned long*) Pilot2_Combined_Irq_Ctrl_Reg) = 0x0;
+
+  *((volatile unsigned long*) Pilot2_Fiq_intmask_Reg) = 0xFFFFFFFF;
+ *((volatile unsigned long*) Pilot2_Fiq_inten_Reg)   = 0;
+  *((volatile unsigned long*) Pilot2_Combined_Fiq_Ctrl_Reg) = 0x0;
+
+  *((volatile unsigned long*) Pilot2_Irq_Ctrl_Hi_Reg) = 0x0;
+  *((volatile unsigned long*) Pilot2_Fiq_Ctrl_Hi_Reg) = 0x0;
+
+  /* Bring down SPI clock */
+  *(volatile unsigned long*)(SE_SYS_CLK_BASE+0x20) &= ~SPI_CLK_MAX;
+  *(volatile unsigned long*)(SE_SYS_CLK_BASE+0x20) |= SPI_CLK_DEFAULT;
+
+  /* Bring down SPI clock */
+  *(volatile unsigned long*)(SE_SYS_CLK_BASE+0x20) &= ~SPI_CLK_MAX;
+  *(volatile unsigned long*)(SE_SYS_CLK_BASE+0x20) |= SPI_CLK_DEFAULT;
+
+
+  // clear the cold reset bit
+  *((volatile unsigned long *)(SE_SYS_CLK_BASE + 0x00)) &= 0xFFFFFFFC;
+
+  //Reset using software reset control registers
+  *(volatile unsigned long*)(SE_RES_DEB_BASE+0x04) &= 0xFFFFFFFD;
+  *(volatile unsigned long*)(SE_RES_DEB_BASE+0x08) = 0;
+  *(volatile unsigned long*)(SE_RES_DEB_BASE+0x0C) |= 0x00081; // Reset the SPI controller along with ARM CPU reset 
+  *(volatile unsigned long*)(SE_RES_DEB_BASE+0x00) = 0x00003002;
+
+ #if 0
+	void (*resetloc)(void);
+	volatile unsigned long resetspi;
+
+	/* Bring down SPI Clock */
+	*(volatile unsigned long*)(0x40100120) &= ~SPI_CLK_MAX;
+	*(volatile unsigned long*)(0x40100120) |= SPI_CLK_DEFAULT;
+
+	/* After SPI write/erase, SPI goes to a wierd state and first read
+	   after this returns all 0xff. Furthur read are ok. So in this wierd
+	   state, reset (Jump to 0) will lead to fault. So do a dummy read here 
+	   to clear the wierd state 
+	*/
+	resetspi =*((volatile unsigned long *)0);
+	if (resetspi == 0xFFFFFFFF)
+		printf("Setting SPI to Read Mode \n");
+
+	
+	printf("Resetting ...\n");
+
+	/* clear the cold reset bit and address remap bit */
+	*((volatile unsigned long *)(SE_SYS_CLK_VA_BASE + 0x00)) &= 0xFFFFFFFC;
+
+	/* Watch dog reset does power on reset which will reinitialize DDR thus
+	   blanking out video output. So cannot use WDT. Instead do warn reset */
+	resetloc = 0;
+	(*resetloc)();
+
+	/* Needs the WDT output line to be connected to reset */
+	/*
+	*(unsigned long *)(WDT_CONTROL_REG) = 0x1 ; 		// set watchdog run
+	*(unsigned char *)(WDT_LOAD_REG0) = 0x1;		// load the count
+	*(unsigned char *)(WDT_LOAD_REG1) = 0x0;		// load the count
+	*(unsigned long *)(WDT_CONTROL_REG) = 0x81 ; 		// set watchdog trigger
+	while (1);
+	*/
+#endif 
+	
+}
+
+#endif
