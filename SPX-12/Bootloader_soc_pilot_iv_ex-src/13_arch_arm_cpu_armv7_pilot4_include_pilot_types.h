--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/include/pilot_types.h	1969-12-31 19:00:00.000000000 -0500
+++ mywork/arch/arm/cpu/armv7/pilot4/include/pilot_types.h	2015-02-06 05:06:37.000000000 -0500
@@ -0,0 +1,41 @@
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
+#ifndef __PILOTTYPES
+#define __PILOTTYPES
+typedef unsigned int              tU32;
+typedef unsigned short            tU16;
+typedef unsigned char             tU8;
+
+typedef unsigned int *            tPU32;
+typedef unsigned short *          tPU16;
+typedef unsigned char *           tPU8;
+
+typedef volatile unsigned int     tVU32;
+typedef volatile unsigned short   tVU16;
+typedef volatile unsigned char    tVU8;
+
+typedef volatile unsigned int *   tPVU32;
+typedef volatile unsigned short * tPVU16;
+typedef volatile unsigned char *  tPVU8;
+
+#define NULL          0
+  
+#endif
