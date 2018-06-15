--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/seccode.c	1969-12-31 19:00:00.000000000 -0500
+++ mywork/arch/arm/cpu/armv7/pilot4/seccode.c	2015-04-22 04:33:54.953802392 -0400
@@ -0,0 +1,39 @@
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
+/*This is the binary of the Hover code that is copied and executed by the Core1*/
+unsigned long dat[17]={
+0xe59f001c,
+0xe5901000, 
+0xe59f2018, 
+0xe1520001, 
+0x0a000001, 
+0xe320f002, 
+0xeafffff8, 
+0xe59f0008, 
+0xe590f000, 
+0x40100d0c, 
+0xabbaadda, 
+0x40100d04, 
+0x00001341, 
+0x61656100, 
+0x01006962, 
+0x00000009, 
+0x01080906 
+};
