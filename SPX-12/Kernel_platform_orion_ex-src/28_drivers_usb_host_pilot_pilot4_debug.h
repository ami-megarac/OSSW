--- linux/drivers/usb/host/pilot/pilot4_debug.h	1970-01-01 08:00:00.000000000 +0800
+++ linux.new/drivers/usb/host/pilot/pilot4_debug.h	2016-12-19 16:04:51.287414544 +0800
@@ -0,0 +1,39 @@
+/*
+ * Copyright (C) 2003-2015 Emulex. All rights reserved.
+ * EMULEX is a trademark of Emulex.
+ * www.emulex.com
+ * This program is free software; you can redistribute it and/or modify it under the terms of version 2
+ * of the GNU General Public License as published by the Free Software Foundation.
+ * This program is distributed in the hope that it will be useful. ALL EXPRESS OR IMPLIED CONDITIONS,
+ * REPRESENTATIONS AND WARRANTIES, INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
+ * PARTICULAR PURPOSE, OR NON-INFRINGEMENT, ARE DISCLAIMED, EXCEPT TO THE EXTENT THAT SUCH DISCLAIMERS
+ * ARE HELD TO BE LEGALLY INVALID. See the GNU General Public License for more details, a copy of which
+ * can be found in the file COPYING included with this package.
+ */
+
+#ifndef __PILOT4_LINUX_DEBUG_H__
+#define __PILOT4_LINUX_DEBUG_H__
+
+#define yprintk(facility, format, args...) \
+	do { printk(facility "%s %d: " format , \
+	__func__, __LINE__ , ## args); } while (0)
+#define WARNING(fmt, args...) yprintk(KERN_WARNING, fmt, ## args)
+#define INFO(fmt, args...) yprintk(KERN_INFO, fmt, ## args)
+#define ERR(fmt, args...) yprintk(KERN_ERR, fmt, ## args)
+
+#define xprintk(level, facility, format, args...) do { \
+	if (_dbg_level(level)) { \
+		printk(facility "%s %d: " format , \
+				__func__, __LINE__ , ## args); \
+	} } while (0)
+
+extern unsigned pilot4_debug;
+
+static inline int _dbg_level(unsigned l)
+{
+	return pilot4_debug >= l;
+}
+
+#define DBG(level, fmt, args...) xprintk(level, KERN_DEBUG, fmt, ## args)
+
+#endif				/*  __PILOT4_LINUX_DEBUG_H__ */
