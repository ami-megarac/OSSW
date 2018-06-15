--- u-boot/arch/arm/cpu/pilotcommon/cache.c	1970-01-01 05:30:00.000000000 +0530
+++ u-boot.new/arch/arm/cpu/pilotcommon/cache.c	2014-07-28 17:17:18.074458596 +0530
@@ -0,0 +1,14 @@
+#include <common.h>
+#include <asm/errno.h>
+#include <linux/compiler.h>
+
+void enable_caches(void)
+{
+#ifndef CONFIG_SYS_ICACHE_OFF
+	icache_enable();
+#endif
+#ifndef CONFIG_SYS_DCACHE_OFF
+	dcache_enable();		
+#endif
+}
+
