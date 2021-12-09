--- uboot/board/ast2500evb/ast2500evb.c	2019-07-02 14:54:03.803076661 +0800
+++ uboot.new/board/ast2500evb/ast2500evb.c	2019-07-02 14:57:47.227076661 +0800
@@ -32,7 +32,7 @@
 #include <common.h>
 #include "ast_hw.h"
 #include "i2c.h"
-
+#include <asm/io.h>
 #define AST_FMC_REG_BASE	0x1e620000
 #define AST_FMC_FLASH_CTRL_REG	0x00
 #define AST_CE0_CTRL_REGISTER	0x30
@@ -66,10 +66,76 @@
 #define AST_PCIE_KEY_CONTROL	0x7C
 #define AST_PCIE_L1_ENTRY_TIMER	0x68
 
+#define BIT(x)		(1 << (x))
+#define ESPI_CTRL                 0x00
+#define ESPI_CTRL_SW_RESET             GENMASK(31, 24)
+#define ESPI_CTRL_OOB_CHRDY            BIT(4)
+#define ESPI_ISR                  0x08
+#define ESPI_ISR_HW_RESET              BIT(31)
+#define ESPI_ISR_VW_SYS_EVT1           BIT(22)
+#define ESPI_ISR_VW_SYS_EVT            BIT(8)
+#define ESPI_IER                  0x0C
+#define ESPI_SYS_IER              0x94
+#define ESPI_SYS_EVENT            0x98
+#define ESPI_SYS_INT_T0           0x110
+#define ESPI_SYS_INT_T1           0x114
+#define ESPI_SYS_INT_T2           0x118
+#define ESPI_SYS_ISR              0x11C
+#define ESPI_SYSEVT_HOST_RST_ACK       BIT(27)
+#define ESPI_SYSEVT_SLAVE_BOOT_STATUS  BIT(23)
+#define ESPI_SYSEVT_SLAVE_BOOT_DONE    BIT(20)
+#define ESPI_SYSEVT_OOB_RST_ACK        BIT(16)
+#define ESPI_SYSEVT_HOST_RST_WARN      BIT(8)
+#define ESPI_SYSEVT_OOB_RST_WARN       BIT(6)
+#define ESPI_SYSEVT_PLT_RST_N          BIT(5)
+#define ESPI_SYS1_IER             0x100
+#define ESPI_SYS1_EVENT           0x104
+#define ESPI_SYS1_INT_T0          0x120
+#define ESPI_SYS1_INT_T1          0x124
+#define ESPI_SYS1_INT_T2          0x128
+#define ESPI_SYS1_ISR             0x12C
+#define ESPI_SYSEVT1_SUS_ACK           BIT(20)
+#define ESPI_SYSEVT1_SUS_WARN          BIT(0)
+#define AST_ESPI_REG_BASE               0x1E6EE000
+#define AST_LPC_REG_BASE                0x1E789000
+
+#define SCROSIO                         0x170
+
+/* Setup Interrupt Type/Enable of System Event from Master
+ *                                 T2 T1 T0
+ *  1). HOST_RST_WARN : Dual Edge   1  0  0
+ *  2). OOB_RST_WARN  : Dual Edge   1  0  0
+ *  3). PLTRST_N      : Dual Edge   1  0  0
+ */
+#define ESPI_SYS_INT_T0_SET        0x00000000
+#define ESPI_SYS_INT_T1_SET        0x00000000
+#define ESPI_SYS_INT_T2_SET \
+(ESPI_SYSEVT_HOST_RST_WARN | ESPI_SYSEVT_OOB_RST_WARN)
+#define ESPI_SYS_INT_SET \
+(ESPI_SYSEVT_HOST_RST_WARN | ESPI_SYSEVT_OOB_RST_WARN)
+
+/* Setup Interrupt Type/Enable of System Event 1 from Master
+ *                                 T2 T1 T0
+ *  1). SUS_WARN    : Rising Edge   0  0  1
+ */
+#define ESPI_SYS1_INT_T0_SET        ESPI_SYSEVT1_SUS_WARN
+#define ESPI_SYS1_INT_T1_SET        0x00000000
+#define ESPI_SYS1_INT_T2_SET        0x00000000
+#define ESPI_SYS1_INT_SET           ESPI_SYSEVT1_SUS_WARN
+
+
 extern void soc_init(void);
 
 DECLARE_GLOBAL_DATA_PTR;
 
+#if defined(CONFIG_EARLYBOOT_ESPI_HANDSHAKE)
+#define ESPI_HANDSHAKE_TIMEOUT      CONFIG_EARLYBOOT_ESPI_MONITOR_TIMEOUT
+#define ESPI_WAIT_TIMEOUT           CONFIG_EARLYBOOT_ESPI_HANDSHAKE_TIMEOUT
+bool oob_evt = 0, sys_evt = 0, sys_evt1 = 0, host_evt = 0;
+int count = 0;
+#endif
+
+
 #if defined(CONFIG_SHOW_BOOT_PROGRESS)
 void show_boot_progress(int progress)
 {
@@ -77,16 +143,142 @@
 }
 #endif
 
-/*
- * Miscellaneous platform dependent initialisations
- */
-
 int board_early_init_f (void)
 {
 
 	return 0;
 }
+#if defined(CONFIG_EARLYBOOT_ESPI_HANDSHAKE)
+void aspeed_espi_slave_sys_event(void)
+{
+        u32 sts, evt;
+
+        sts = readl(AST_ESPI_REG_BASE + ESPI_SYS_ISR);
+        evt = readl(AST_ESPI_REG_BASE + ESPI_SYS_EVENT);
+
+		// ACK the Host_Rst_Warn
+        if (sts & ESPI_SYSEVT_HOST_RST_WARN)
+        {
+			if (evt & ESPI_SYSEVT_HOST_RST_WARN)
+			{
+				writel(ESPI_SYSEVT_HOST_RST_ACK, AST_ESPI_REG_BASE + ESPI_SYS_EVENT);
+				host_evt = 1;
+			}
+        }
+
+		// ACK the OOB_Rst_Warn
+        if (sts & ESPI_SYSEVT_OOB_RST_WARN)
+        {
+	    if (evt & ESPI_SYSEVT_OOB_RST_WARN)
+            {
+                writel(ESPI_SYSEVT_OOB_RST_ACK, AST_ESPI_REG_BASE + ESPI_SYS_EVENT);
+                oob_evt = 1;
+            }
+        }
+
+		// clear status, if any
+        writel(sts, AST_ESPI_REG_BASE + ESPI_SYS_ISR);
+}
+
+void aspeed_espi_slave_sys1_event(void)
+{
+        u32 sts, evt;
+ 
+        sts = readl(AST_ESPI_REG_BASE + ESPI_SYS1_ISR);
+
+        if (sts & ESPI_SYSEVT1_SUS_WARN)
+		{
+			evt = readl(AST_ESPI_REG_BASE + ESPI_SYS1_EVENT);
+
+			// ACK the Suspend Warn
+			if (evt & ESPI_SYSEVT1_SUS_ACK)
+			{
+				writel(ESPI_SYSEVT1_SUS_ACK, AST_ESPI_REG_BASE + ESPI_SYS1_EVENT);
+				host_evt = 1;
+			}
+		}
+
+		// clear the System Event 1 interrupt status, if any
+        writel(sts, AST_ESPI_REG_BASE + ESPI_SYS1_ISR);
+}
+
+void aspeed_espi_event_handling(void)
+{
+	u32 sts;
+
+	sts = readl(AST_ESPI_REG_BASE + ESPI_ISR);
+
+	if (sts & (ESPI_ISR_VW_SYS_EVT | ESPI_ISR_VW_SYS_EVT1))
+	{
+		if (sts & ESPI_ISR_VW_SYS_EVT)
+			aspeed_espi_slave_sys_event();
+
+		if (sts & ESPI_ISR_VW_SYS_EVT1)
+			aspeed_espi_slave_sys1_event();
+
+		// clear the interrupt status
+		writel(sts, AST_ESPI_REG_BASE + ESPI_IER);
+	}
+
+}
+
+void aspeed_espi_slave_boot_ack(void)
+{
+	u32 evt;
+	evt = readl(AST_ESPI_REG_BASE + ESPI_SYS_EVENT);
+	if((evt & ESPI_SYSEVT_SLAVE_BOOT_STATUS) == 0)
+	{
+		evt = evt | ESPI_SYSEVT_SLAVE_BOOT_STATUS | ESPI_SYSEVT_SLAVE_BOOT_DONE;
+		writel(evt, AST_ESPI_REG_BASE + ESPI_SYS_EVENT);
+		host_evt = 1;
+	}
+	evt = readl(AST_ESPI_REG_BASE + ESPI_SYS1_EVENT);
+	if((evt & ESPI_SYSEVT1_SUS_WARN) != 0)
+	{
+		evt |= ESPI_SYSEVT1_SUS_ACK;
+		writel(evt, AST_ESPI_REG_BASE + ESPI_SYS1_EVENT);
+		host_evt = 1;
+	}
+}
+void eSPI_config_handshake(void)
+{
+	u32 reg;
+
+	// setting up the eSPI ID
+	reg = readl(AST_LPC_REG_BASE + SCROSIO);
+
+	reg &= ~0x000000ff;
+	writel(reg, AST_LPC_REG_BASE + SCROSIO);
+	reg = readl(AST_LPC_REG_BASE + SCROSIO);
+	reg |= 0xa8;
+	writel(reg, AST_LPC_REG_BASE + SCROSIO);
+
+	// set System Event interrupts for Host Reset Warn and OOB Reset Warn
+    writel(ESPI_SYS_INT_T0_SET, AST_ESPI_REG_BASE + ESPI_SYS_INT_T0);
+    writel(ESPI_SYS_INT_T1_SET, AST_ESPI_REG_BASE + ESPI_SYS_INT_T1);
+    writel(ESPI_SYS_INT_T2_SET, AST_ESPI_REG_BASE + ESPI_SYS_INT_T2);
+    writel(ESPI_SYS_INT_SET, AST_ESPI_REG_BASE + ESPI_SYS_IER);
+
+	// set System Event 1 interrupts for Suspend Warn
+	writel(ESPI_SYS1_INT_T0_SET, AST_ESPI_REG_BASE + ESPI_SYS1_INT_T0);
+    writel(ESPI_SYS1_INT_T1_SET, AST_ESPI_REG_BASE + ESPI_SYS1_INT_T1);
+    writel(ESPI_SYS1_INT_T2_SET, AST_ESPI_REG_BASE + ESPI_SYS1_INT_T2);
+    writel(ESPI_SYS1_INT_SET, AST_ESPI_REG_BASE + ESPI_SYS1_IER);
+
+	// enable VW System Event interrupt
+	writel(ESPI_ISR_VW_SYS_EVT, AST_ESPI_REG_BASE + ESPI_IER);
+	while ((count < ESPI_HANDSHAKE_TIMEOUT && oob_evt == 0 && host_evt == 0 )
+		|| (count < ESPI_WAIT_TIMEOUT && oob_evt == 0 && host_evt == 1) )
+	{
+		aspeed_espi_slave_boot_ack();
+		aspeed_espi_event_handling();
+		count++;
+		udelay(1000000);
+	}
 
+
+}
+#endif
 int board_init (void)
 {
     
@@ -100,6 +292,13 @@
 
 	icache_enable ();
 	
+#if defined(CONFIG_EARLYBOOT_ESPI_HANDSHAKE) 
+	
+	eSPI_config_handshake();
+	printf("eSPI Handshake complete\r\n");
+#endif
+
+	
 #ifdef CONFIG_AST2500	
 	/* Set I2C Bus Number */
 	i2c_set_bus_num(CONFIG_I2C_CHANNEL_ID);
