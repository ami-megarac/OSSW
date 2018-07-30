--- uboot_old/arch/arm/cpu/astcommon/reset.c	1970-01-01 05:30:00.000000000 +0530
+++ uboot/arch/arm/cpu/astcommon/reset.c	2017-01-14 04:50:43.572880418 +0530
@@ -0,0 +1,14 @@
+/* AST2X00 WDT Routines for u-boot reboot */
+
+#include <common.h>
+#include <linux/types.h>
+#include "soc_hw.h"
+
+void reset_cpu(ulong addr)
+{
+    printf("Resetting ...\n");
+    *(unsigned long *)(WDT_RELOAD_REG) = 0x10;
+    *(unsigned long *)(WDT_CNT_RESTART_REG) = 0x4755;
+    *(unsigned long *)(WDT_CONTROL_REG) = WDT_TIMER_EN | WDT_RESET_EN;
+	while (1);
+}
