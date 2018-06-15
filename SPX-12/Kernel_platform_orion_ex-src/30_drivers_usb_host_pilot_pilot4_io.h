--- linux/drivers/usb/host/pilot/pilot4_io.h	1970-01-01 08:00:00.000000000 +0800
+++ linux.new/drivers/usb/host/pilot/pilot4_io.h	2016-12-19 16:06:59.067414544 +0800
@@ -0,0 +1,84 @@
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
+#ifndef __PILOT_LINUX_PLATFORM_ARCH_H__
+#define __PILOT_LINUX_PLATFORM_ARCH_H__
+
+#include <linux/io.h>
+#include <asm/soc-pilot/hwmap.h>
+/* Hw register definitions */
+#define OTGCTRL	0x1BE
+	#define OTG_BUS_REQ (1<<0)
+	#define OTG_ABUS_DROP (1<<1)
+
+#define HCIVECT	0x1A0
+#define OTGIRQ	0x1BC
+#define OTGSTATUS	0x1BF
+#define OTGSTATE	0x1BD
+#define HCIN0BC	0x0
+#define HCOUT0BC	0x1
+#define HCEP0CS	0x2
+#define HCEP0OUTDAT	0x100
+#define HCEP0INDAT	0x140
+#define HCEP0CTRL	0xC0
+#define HCIN0MAXPCK	0x1E0
+#define SE_PILOT_USBIRQ_REG_OFFSET	0x18C
+
+#define HCENDPRST	0x1A2
+#define HCUSBCS		0x1A3
+#define HOST_FUNC_ADDR 0x1A6
+
+#define HCPORTCTRL	0x1AB
+	#define PORT_RESET (1<<5)
+
+#define hcout07ien	0x194
+#define hcout815ien	0x195
+#define hcin07ien		0x196
+#define hcin07errien    0x1B8
+#define hcout07errien 0x1BA
+#define hcout815errien 0x1BB
+
+#define  BIT_USB2_INT_USB       (1<<0)
+#define  BIT_USB2_INT_DMA       (1<<1)
+#define  BIT_USB2_INT_WAKUP     (1<<2)
+#define  BIT_USB2_INT_FIFO      (1<<3)
+#define  BIT_USB2_INT_IN_TOKEN    (1<<6)
+
+#define USB2_INT_ENABLE_OFFSET	0x1B1
+
+#define hcout07irq 0x188
+#define hcin07irq 0x18A
+#define hcout07errirq 0x1B6
+#define hcin07errirq 0x1B4
+
+static inline u16 pilot4_readw(const void __iomem *addr, unsigned offset)
+	{ return __raw_readw(addr + offset); }
+
+static inline u32 pilot4_readl(const void __iomem *addr, unsigned offset)
+	{ return __raw_readl(addr + offset); }
+
+
+static inline void pilot4_writew(void __iomem *addr, unsigned offset, u16 data)
+	{ __raw_writew(data, addr + offset); }
+
+static inline void pilot4_writel(void __iomem *addr, unsigned offset, u32 data)
+	{ __raw_writel(data, addr + offset); }
+
+
+static inline u8 pilot4_readb(const void __iomem *addr, unsigned offset)
+	{ return __raw_readb(addr + offset); }
+
+static inline void pilot4_writeb(void __iomem *addr, unsigned offset, u8 data)
+	{ __raw_writeb(data, addr + offset); }
+
+#endif
