--- linux/drivers/usb/host/pilot/pilot4_core.h	1970-01-01 08:00:00.000000000 +0800
+++ linux.new/drivers/usb/host/pilot/pilot4_core.h	2016-12-19 16:03:23.671414544 +0800
@@ -0,0 +1,97 @@
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
+#ifndef __PILOT4_CORE_H__
+#define __PILOT4_CORE_H__
+
+#include <linux/slab.h>
+#include <linux/list.h>
+#include <linux/interrupt.h>
+#include <linux/errno.h>
+#include <linux/clk.h>
+#include <linux/device.h>
+#include <linux/usb/ch9.h>
+#include <linux/usb/gadget.h>
+#include <linux/usb.h>
+#include <linux/usb/otg.h>
+#include <linux/usb/hcd.h>
+
+struct pilot_host_hcd;
+struct pilot4_hw_ep;
+struct pilot4_ep;
+
+#include "pilot4_io.h"
+#include "pilot4_host.h"
+extern const struct hc_driver pilot4_hc_driver;
+//#define TEST_PORT_CONNECT //for testing only
+#define USB_MULTI_DEV_SUPPORT 
+#define DYNAMIC_CONFIGURE_EP 1
+#define USE_BULK_EP_FOR_EP0_OUT 1
+#define is_host_capable()       (1)
+#define	is_host_active(p4_hcd)		is_host_capable()
+
+
+
+#ifdef CONFIG_PROC_FS
+#include <linux/fs.h>
+#define PILOT4_CONFIG_PROC_FS
+#endif
+
+/* peripheral side ep0 states */
+enum pilot4_g_ep0_state {
+	PILOT4_EP0_STAGE_SETUP,		/* idle, waiting for setup */
+	PILOT4_EP0_STAGE_TX,		/* IN data */
+	PILOT4_EP0_STAGE_RX,		/* OUT data */
+	PILOT4_EP0_STAGE_STATUSIN,	/* (after OUT data) */
+	PILOT4_EP0_STAGE_STATUSOUT,	/* (after IN data) */
+	PILOT4_EP0_STAGE_ACKWAIT,		/* after zlp, before statusin */
+} __attribute__ ((packed));
+
+/* OTG protocol constants */
+#define OTG_TIME_A_WAIT_VRISE	100		/* msec (max) */
+#define OTG_TIME_A_WAIT_BCON	0		/* 0=infinite; min 1000 msec */
+#define OTG_TIME_A_IDLE_BDIS	200		/* msec (min) */
+
+static inline void pilot4_set_vbus(struct pilot_host_hcd *p4_hcd, int is_on)
+{
+	p4_hcd->board_set_vbus(p4_hcd, is_on);
+}
+
+
+/***************************** Glue it together *****************************/
+
+extern const char pilot4_driver_name[];
+
+extern void pilot4_start(struct pilot_host_hcd *);
+extern void pilot4_stop(struct pilot_host_hcd *);
+
+extern void pilot4_write_fifo(struct pilot4_hw_ep *ep, u16 len, const u8 *src);
+extern void pilot4_read_fifo(struct pilot4_hw_ep *ep, u16 len, u8 *dst);
+
+extern void pilot4_load_testpacket(struct pilot_host_hcd *);
+
+extern irqreturn_t pilot4_interrupt(struct pilot_host_hcd *);
+
+extern void pilot4_platform_enable(struct pilot_host_hcd *);
+extern void pilot4_platform_disable(struct pilot_host_hcd *);
+
+extern void pilot4_hnp_stop(struct pilot_host_hcd *);
+
+extern void pilot4_platform_set_mode(struct pilot_host_hcd *, u8 pilot4_mode);
+
+#define pilot4_platform_try_idle(x, y)		do {} while (0)
+
+//extern int __init pilot4_platform_init(struct pilot_host_hcd *);
+//extern int pilot4_platform_exit(struct pilot_host_hcd *);
+
+#endif	/* __PILOT4_CORE_H__ */
