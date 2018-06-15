--- linux/drivers/usb/host/pilot/pilot4_host.h	1970-01-01 08:00:00.000000000 +0800
+++ linux.new/drivers/usb/host/pilot/pilot4_host.h	2016-12-19 16:06:00.763414544 +0800
@@ -0,0 +1,279 @@
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
+#ifndef __PILOT4_HOST_H__
+#define __PILOT4_HOST_H__
+
+#define SW_WORKAROUND 1
+#define DEBUG_START_FINISH 1
+#define HOST_DEBUG_STATS 1
+extern int pilot4_hub_status_data(struct usb_hcd *hcd, char *buf);
+extern int pilot4_hub_control(struct usb_hcd *hcd, u16 typeReq,
+	u16 wValue, u16 wIndex, char *buf, u16 wLength);
+
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
+
+#define HCENDPRST	0x1A2
+#define HOST_FUNC_ADDR 0x1A6
+
+enum pilot4_h_ep0_state {
+	PILOT4_EP0_IDLE,
+	PILOT4_EP0_START,			/* expect ack of setup */
+	PILOT4_EP0_IN,			/* expect IN DATA */
+	PILOT4_EP0_OUT,			/* expect ack of OUT DATA */
+	PILOT4_EP0_STATUS,		/* expect ack of STATUS */
+} __attribute__((packed));
+
+struct pilot4_hw_ep {
+	struct pilot_host_hcd	*pilot_host;
+	unsigned	bytes_pending;
+	unsigned int	ep_num;
+	unsigned int	ep_data_buf_reg;
+	unsigned int	ep_byte_cnt_reg;
+	unsigned int	ep_cs_reg;
+	unsigned int	ep_data_buf_reg1;
+	unsigned int	ep_byte_cnt_reg1;
+	unsigned int	ep_cs_reg1;
+	unsigned int    func_addr_reg;
+	unsigned int	hc_in_con;
+	unsigned int	hc_out_con;
+	unsigned int	hcinerr;//hcinxerr
+	unsigned int	hcouterr;//hcoutxerr
+	unsigned int	hcoutctrl;//hcoutxtrl
+	unsigned int	hcinctrl;//hcinxctrl
+	unsigned int	hcinmaxpck;
+	unsigned int	hcoutmaxpck;	
+	char	name[14];
+	u16	maxpacket;
+	u8	bEndpointAddress;
+	u8	bmAttributes;
+	unsigned	double_buf:1;
+	unsigned	stopped:1;
+	unsigned	timer_armed:1;
+ 	unsigned	has_dma:1;
+	struct list_head queue;
+	int last_dev_addr;
+	int reinit;
+	struct pilot4_qh * last_freed_qh;
+	struct urb * last_freed_urb;
+#ifdef SW_WORKAROUND
+	struct timer_list intr_ep_timer;
+#endif
+	struct pilot4_qh		*cur_qh;
+	struct urb *cur_urb;
+#ifdef DEBUG_START_FINISH
+	u64 start_time;
+	struct timer_list debug_timer;
+	u32 wait_time;
+	u32 cur_cpu;
+	u64 last_complete_time;
+#endif
+#ifdef HOST_DEBUG_STATS
+	u8 dev_addr;
+#ifndef DEBUG_START_FINISH
+	u64 start_time;
+#endif
+	u32 total_urbs;
+	u64 total_transfer_buffer_length;
+	u64 completed_length;
+	u32 errors;
+	u32 success;
+	u32 total_fail_duration;
+	u32 total_success_duration;
+	u32 max_success_duration;
+	u32 max_failure_duration;
+#endif
+};
+
+struct pilot4_qh {
+        struct usb_host_endpoint *hep;          /* usbcore info */
+        struct usb_device       *dev;
+        struct pilot4_hw_ep       *hw_ep;         /* current binding */
+
+        struct list_head        ring;           /* of pilot4_qh */
+        /* struct pilot4_qh               *next; */       /* for periodic tree */
+        u8                      mux;            /* qh multiplexed to hw_ep */
+
+        unsigned                offset;         /* in urb->transfer_buffer */
+        unsigned                segsize;        /* current xfer fragment */
+
+        u8                      type_reg;       /* {rx,tx} type register */
+        u8                      intv_reg;       /* {rx,tx} interval register */
+        u8                      addr_reg;       /* device address register */
+        u8                      h_addr_reg;     /* hub address register */
+        u8                      h_port_reg;     /* hub port register */
+
+        u8                      is_ready;       /* safe to modify hw_ep */
+        volatile u8     programmed;
+        volatile u8     abort_pending;
+
+        u8                      type;           /* XFERTYPE_* */
+        u8                      epnum;
+	 u8			      free_recursion;
+        u16                      retries;
+        u16                     maxpacket;
+        u16                     frame;          /* for periodic schedule */
+};
+
+struct pilot_host_hcd {
+	void __iomem *base_addr;//virtual membase addr
+	struct pilot4_hw_ep endpoints[32];
+	struct pilot4_hw_ep *intr_in[8];
+	struct pilot4_hw_ep *intr_out[8];
+	struct pilot4_hw_ep *bulk_in[8];
+	struct pilot4_hw_ep *bulk_out[8];
+	spinlock_t                      lock;
+	enum pilot4_h_ep0_state	ep0_stage;
+	int state;
+	struct pilot4_qh          *cur_qh;
+#define PILOT_PORT_STAT_RESUME	(1 << 31)
+	u32                     port1_status;
+	u32 speed;
+	unsigned                is_active:1;
+	void                    (*board_set_vbus)(struct pilot_host_hcd *, int is_on);
+#define VBUSERR_RETRY_COUNT     3
+	u16                     vbuserr_retry;
+	unsigned long           rh_timer;
+	unsigned int debug_level;
+	struct device *dev;
+};
+
+static inline struct usb_hcd *pilot_pilot4_to_hcd(struct pilot_host_hcd *p4_hcd)
+{
+        return container_of((void *) p4_hcd, struct usb_hcd, hcd_priv);
+}
+
+static inline struct pilot_host_hcd *hcd_to_pilot_hcd(struct usb_hcd *hcd)
+{
+        return (struct pilot_host_hcd *) (hcd->hcd_priv);
+}
+
+
+static inline struct pilot4_qh *first_qh(struct list_head *q)
+{
+        if (list_empty(q))
+                return NULL;
+        return list_entry(q->next, struct pilot4_qh, ring);
+}
+
+static inline struct urb *next_urb(struct pilot4_qh *qh)
+{
+        struct list_head        *queue;
+
+        if (!qh)
+                return NULL;
+        queue = &qh->hep->urb_list;
+        if (list_empty(queue))
+                return NULL;
+        return list_entry(queue->next, struct urb, urb_list);
+}
+#define CONFIG_P4_USB_DEBUG 1 //For now #define it here
+#define ENDPOINT_SHARING_DEBUG 0x1
+#define DETAILED_ENDPOINT_SHARING_DEBUG 0x2
+#define CLEANUP_PRINT	0x4
+#define DETAILED_CLEANUP_PRINT	0x8
+#define ENQUEUE_PRINT		0x10
+#define DETAILED_ENQUEUE_PRINT		0x20
+#define ERROR_PRINT			0x40
+#define DETAILED_ERROR_PRINT			0x80
+#define GIVEBACK_PRINT		0x100
+#define DETAILED_GIVEBACK_PRINT		0x100
+#define HUB_CONTROL	0x200
+#define DETAILED_HUB_CONTROL		0x400
+#define DETAILED_RECURSION_PRINT	0x800
+#define ENABLE_CLEANUP_PRINTS_DURING_ERROR	0x1000
+#define DETAILED_RECURSION_PRINT_DURING_GIVEBACK	0x2000
+
+#ifdef CONFIG_P4_USB_DEBUG
+#define DBG_ENDPOINT_SHARING(fmt, args...)		do  { \
+			if(pilot_host_udc->debug_level & ENDPOINT_SHARING_DEBUG)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_DETAILED_ENDPOINT_SHARING(fmt, args...)	do  { \
+			if(pilot_host_udc->debug_level & DETAILED_ENDPOINT_SHARING_DEBUG)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_CLEANUP_PRINT(fmt, args...)	do  { \
+			if(pilot_host_udc->debug_level & CLEANUP_PRINT)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_DETAILED_CLEANUP_PRINT(fmt, args...)do  { \
+			if(pilot_host_udc->debug_level & DETAILED_CLEANUP_PRINT)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_ENQUEUE_PRINT(fmt, args...)do  { \
+			if(pilot_host_udc->debug_level & ENQUEUE_PRINT)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_DETAILED_ENQUEUE_PRINT(fmt, args...)do  { \
+			if(pilot_host_udc->debug_level & DETAILED_ENQUEUE_PRINT)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_ERROR_PRINT(fmt, args...) do  { \
+			if(pilot_host_udc->debug_level & ERROR_PRINT)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_DETAILED_ERROR_PRINT(fmt, args...)do  { \
+			if(pilot_host_udc->debug_level & DETAILED_ERROR_PRINT)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_GIVEBACK_PRINT(fmt, args...) do  { \
+			if(pilot_host_udc->debug_level & GIVEBACK_PRINT)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_DETAILED_GIVEBACK_PRINT(fmt, args...)do  { \
+			if(pilot_host_udc->debug_level & DETAILED_GIVEBACK_PRINT)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_HUB_CONTROL(fmt, args...) do  { \
+			if(pilot_host_udc->debug_level & HUB_CONTROL)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_DETAILED_HUB_CONTROL(fmt, args...) do  { \
+			if(pilot_host_udc->debug_level & DETAILED_HUB_CONTROL)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#define DBG_RECURSION_PRINT(fmt, args...) do  { \
+			if(pilot_host_udc->debug_level & DETAILED_RECURSION_PRINT)	\
+				printk(KERN_CRIT fmt, ## args);	\
+		} while(0)
+#else
+#define DBG_ENDPOINT_SHARING(fmt, args...)	do { }while(0)
+#define DBG_DETAILED_ENDPOINT_SHARING(fmt, args...)	do { }while(0)
+#define DBG_CLEANUP_PRINT(fmt, args...)	do { }while(0)
+#define DBG_DETAILED_CLEANUP_PRINT(fmt, args...)	do { }while(0)
+#define DBG_ENQUEUE_PRINT(fmt, args...)	do { }while(0)
+#define DBG_DETAILED_ENQUEUE_PRINT(fmt, args...)	do { }while(0)
+#define DBG_ERROR_PRINT(fmt, args...)	do { }while(0)
+#define DBG_DETAILED_ERROR_PRINT(fmt, args...)	do { }while(0)
+#define DBG_GIVEBACK_PRINT(fmt, args...)	do { }while(0)
+#define DBG_DETAILED_GIVEBACK_PRINT(fmt, args...)	do { }while(0)
+#define DBG_HUB_CONTROL(fmt, args...)	do { }while(0)
+#define DBG_DETAILED_HUB_CONTROL(fmt, args...)	do { }while(0)
+#endif
+
+#endif
