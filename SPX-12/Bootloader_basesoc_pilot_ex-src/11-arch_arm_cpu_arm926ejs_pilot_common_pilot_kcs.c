--- u-boot/arch/arm/cpu/pilotcommon/pilotkcs.c	1970-01-01 05:30:00.000000000 +0530
+++ u-boot.new/arch/arm/cpu/pilotcommon/pilotkcs.c	2014-07-28 22:17:08.869926840 +0530
@@ -0,0 +1,37 @@
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
+ ***                                                            **
+ ***            All Rights Reserved.                            **
+ ***                                                            **
+ ***        6145-F, Northbelt Parkway, Norcross,                **
+ ***                                                            **
+ ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ ***                                                            **
+ *****************************************************************
+ *****************************************************************
+
+ ******************************************************************
+ * 
+ * KCS.c
+ * KCS Functions.
+ *
+ *  Author: Suresh V <sureshv@amiindia.com>
+ ******************************************************************/
+#include <common.h>
+#include <config.h>
+#include "soc_hw.h"
+
+#define MAX_KCS_CHANNELS 2
+
+volatile unsigned char *lpc_base[MAX_KCS_CHANNELS];
+
+void InitializeKCSHardware(void)
+{
+	lpc_base[0] = (unsigned char *)SE_KCS4_BASE;
+	lpc_base[1] = (unsigned char *)SE_KCS3_BASE;
+
+	return;
+}
+
