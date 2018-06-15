--- u-boot/include/pilot_hw.h	1970-01-01 05:30:00.000000000 +0530
+++ u-boot.new/include/pilot_hw.h	2014-07-22 11:34:47.844954821 +0530
@@ -0,0 +1,17 @@
+#ifndef _PILOTIII_HW_H_
+#define _PILOTIII_HW_H_
+
+#include <config.h>
+
+/* U-Boot does not use MMU. So no mapping */
+#define IO_ADDRESS(x)	(x)
+#define MEM_ADDRESS(x)  (x)
+
+#include <pilot/hwmap.h>
+#include <pilot/hwreg.h>
+#include <pilot/hwdef.h>
+#include <pilot/serreg.h>
+#include <pilot/macreg.h>
+
+
+#endif
