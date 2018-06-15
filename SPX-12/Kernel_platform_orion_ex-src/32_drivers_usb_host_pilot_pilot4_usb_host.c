--- linux/drivers/usb/host/pilot/pilot4_usb_host.c	1970-01-01 08:00:00.000000000 +0800
+++ linux.new/drivers/usb/host/pilot/pilot4_usb_host.c	2016-12-19 16:09:36.703414544 +0800
@@ -0,0 +1,3472 @@
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
+#include <linux/module.h>
+#include <linux/kernel.h>
+#include <linux/delay.h>
+#include <linux/sched.h>
+#include <linux/slab.h>
+#include <linux/errno.h>
+#include <linux/init.h>
+#include <linux/list.h>
+#include <linux/kallsyms.h>
+
+#include "pilot4_core.h"
+#include "pilot4_host.h"
+
+extern struct pilot_host_hcd *pilot_host_udc;
+#if 0
+#define DBG(level, fmt, args...) printk(KERN_CRIT fmt, ## args)
+#else
+#define DBG(level, fmt, args...) //printk(KERN_CRIT fmt, ## args)
+#endif
+
+#define HOST_DEBUG
+#define CLEANUP_DEBUG 1
+static volatile int print_giveback = 0;
+static volatile int in_giveback = 0;
+static volatile int giveback_recursion = 0;
+static struct pilot4_qh *giveback_qh;
+static volatile int start_intr_print = 0;
+//#define HAVE_WORKAROUND
+//static int host_ep3_in_data_valid = 0;
+//#define HW_BUG 1 //Endpoint reset H/W bug
+static void pilot4_ep0_continue(struct pilot_host_hcd *pilot_host, u16 len, struct urb *urb);
+static inline int get_toggle(u8 epnum, unsigned int is_out);
+
+static int no_endp_handshake;
+
+
+static void pilot4_ep_program(struct pilot_host_hcd *pilot_host, u8 epnum,
+			struct urb *urb, unsigned int is_out,
+			u8 *buf, u32 len, struct pilot4_hw_ep *);
+
+
+#ifdef CONFIG_SYSFS
+
+static inline struct pilot_host_hcd *dev_to_p4_host(struct device *dev)
+{
+	return dev_get_drvdata(dev);
+}
+
+
+static char * show_debug_level(unsigned int val, int kernel_print, char *buf)
+{
+	if(kernel_print) {
+		if(val & ENDPOINT_SHARING_DEBUG)
+			printk("ENDPOINT_SHARING_DEBUG\n");
+		if(val & DETAILED_ENDPOINT_SHARING_DEBUG)
+			printk("DETAILED_ENDPOINT_SHARING_DEBUG\n");
+		if(val & CLEANUP_PRINT)
+			printk("CLEANUP_PRINT\n");
+		if(val & DETAILED_CLEANUP_PRINT)
+			printk("DETAILED_CLEANUP_PRINT\n");
+		if(val & ENQUEUE_PRINT)
+			printk("ENQUEUE_PRINT\n");
+		if(val & DETAILED_ENQUEUE_PRINT)
+			printk("DETAILED_ENQUEUE_PRINT\n");
+		if(val & ERROR_PRINT)
+			printk("ERROR_PRINT\n");
+		if(val & DETAILED_ERROR_PRINT)
+			printk("DETAILED_ERROR_PRINT\n");
+		if(val & GIVEBACK_PRINT)
+			printk("GIVEBACK_PRINT\n");
+		if(val & DETAILED_GIVEBACK_PRINT)
+			printk("DETAILED_GIVEBACK_PRINT\n");
+		if(val & DETAILED_RECURSION_PRINT)
+			printk("DETAILED_RECURSION_PRINT\n");
+		if(val & DETAILED_RECURSION_PRINT_DURING_GIVEBACK)
+			printk("DETAILED_RECURSION_PRINT_DURING_GIVEBACK\n");
+		return NULL;
+	}else {
+		if(val & ENDPOINT_SHARING_DEBUG)
+			buf += sprintf(buf, "%s", "ENDPOINT_SHARING_DEBUG\n");
+		if(val & DETAILED_ENDPOINT_SHARING_DEBUG)
+			buf += sprintf(buf, "%s", "DETAILED_ENDPOINT_SHARING_DEBUG\n");
+		if(val & CLEANUP_PRINT)
+			buf += sprintf(buf, "%s", "CLEANUP_PRINT\n");
+		if(val & DETAILED_CLEANUP_PRINT)
+			buf += sprintf(buf, "%s", "DETAILED_CLEANUP_PRINT\n");
+		if(val & ENQUEUE_PRINT)
+			buf += sprintf(buf, "%s", "ENQUEUE_PRINT\n");
+		if(val & DETAILED_ENQUEUE_PRINT)
+			buf += sprintf(buf, "%s", "DETAILED_ENQUEUE_PRINT\n");
+		if(val & ERROR_PRINT)
+			buf += sprintf(buf, "%s", "ERROR_PRINT\n");
+		if(val & DETAILED_ERROR_PRINT)
+			buf += sprintf(buf, "%s", "DETAILED_ERROR_PRINT\n");
+		if(val & GIVEBACK_PRINT)
+			buf += sprintf(buf, "%s", "GIVEBACK_PRINT\n");
+		if(val & DETAILED_GIVEBACK_PRINT)
+			buf += sprintf(buf, "%s", "DETAILED_GIVEBACK_PRINT\n");
+		if(val & DETAILED_RECURSION_PRINT)
+			buf += sprintf(buf, "%s", "DETAILED_RECURSION_PRINT\n");
+		if(val & DETAILED_RECURSION_PRINT_DURING_GIVEBACK)
+			buf += sprintf(buf, "%s", "DETAILED_RECURSION_PRINT_DURING_GIVEBACK\n");
+		buf += sprintf(buf, "%s-%x\n","raw_debug_level", val);
+		return buf;
+	}
+}
+
+static ssize_t
+p4_usb_debug_store(struct device *dev, struct device_attribute *attr,
+		const char *buf, size_t n)
+{
+	struct pilot_host_hcd	*p4_udc = dev_to_p4_host(dev);
+	unsigned long	flags;
+	unsigned int	val;
+
+	if (sscanf(buf, "%x", &val) < 1) {
+		dev_err(dev, "Invalid value %x\n", val);
+		return -EINVAL;
+	}
+
+	spin_lock_irqsave(&p4_udc->lock, flags);
+	p4_udc->debug_level = val;
+	if(val) {
+		printk("Debug prints for %s device\n", dev_name(dev));
+		(void)show_debug_level(val, 1, NULL);
+	}else {
+		printk("Debug prints Disabled for %s device\n", dev_name(dev));
+	}
+	spin_unlock_irqrestore(&p4_udc->lock, flags);
+
+	return n;
+}
+
+static ssize_t
+p4_usb_debug_show(struct device *dev, struct device_attribute *attr, char *buf)
+{
+	struct pilot_host_hcd	*p4_udc = dev_to_p4_host(dev);
+	unsigned long	flags;
+	unsigned long	val;
+	char *temp = buf;
+
+	spin_lock_irqsave(&p4_udc->lock, flags);
+	val = p4_udc->debug_level;
+	spin_unlock_irqrestore(&p4_udc->lock, flags);
+	if(val) {
+		sprintf(buf, "Debug prints for %s device\n", dev_name(dev));
+		temp = show_debug_level(val, 0, buf);
+		return (temp - buf);
+	}else {
+		return sprintf(buf, "Debug prints Disabled for %s device\n", dev_name(dev));
+	}
+}
+
+static DEVICE_ATTR(p4_usb_debug, 0644, p4_usb_debug_show, p4_usb_debug_store);
+
+static struct attribute *p4_usb_host_attributes[] = {
+	&dev_attr_p4_usb_debug.attr,
+	NULL
+};
+
+static const struct attribute_group p4_usb_host_attr_group = {
+	.attrs = p4_usb_host_attributes,
+};
+#endif
+
+/*
+ * Clear TX fifo. Needed to avoid BABBLE errors.
+ */
+
+static void print_control_msg(struct usb_ctrlrequest *dr)
+{
+	return ;
+	printk("dr->bRequestType %x\n", dr->bRequestType);
+	printk("dr->bRequest %x\n", dr->bRequest);
+	printk("dr->wValue %x\n", dr->wValue);
+	printk("dr->wIndex %x\n", dr->wIndex);
+	printk("dr->wLength %x\n", dr->wLength);
+}
+
+#ifdef DEBUG_START_FINISH
+static void start_ep0_timer(unsigned long data)
+{
+	unsigned long flags;
+	struct pilot4_hw_ep *hw_ep = (struct pilot4_hw_ep *)data;
+	spin_lock_irqsave(&hw_ep->pilot_host->lock, flags);
+	if(hw_ep->cur_urb) {
+		printk("EP0 urb timer expired error urb %p status %d jiffies %lu starttime %llu curwait_time %u\n",
+			hw_ep->cur_urb, hw_ep->cur_urb->status, jiffies, hw_ep->start_time, hw_ep->wait_time);
+		printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_qh);
+		printk("epnum %d busy bit set i ask y ??? epcs 0x%x addr %x programmed %d\n",
+			hw_ep->ep_num, *((volatile unsigned char*)(hw_ep->ep_cs_reg)), 
+			(hw_ep->cur_qh)?hw_ep->cur_qh->addr_reg:-1, (hw_ep->cur_qh)?hw_ep->cur_qh->programmed:-1);
+	} else {
+		printk("%s hw_ep->cur_urb is NULL\n", __FUNCTION__);
+	}
+	hw_ep->wait_time = hw_ep->wait_time;
+	mod_timer(&hw_ep->debug_timer, jiffies + hw_ep->wait_time);
+	spin_unlock_irqrestore(&hw_ep->pilot_host->lock, flags);
+}
+#endif
+
+
+/*
+ * Start the URB at the front of an endpoint's queue
+ * end must be claimed from the caller.
+ *
+ * Context: controller locked, irqs blocked
+ */
+static void
+pilot4_start_urb(struct pilot_host_hcd *pilot_host, int is_in, struct pilot4_qh *qh)
+{
+	u32			len;
+	void			*buf;
+	struct urb		*urb = NULL;
+	struct pilot4_hw_ep	*hw_ep = NULL;
+	unsigned		pipe;
+	int			epnum;
+	struct pilot4_qh *temp_qh = NULL;
+	int total_qh = 0;
+	struct usb_endpoint_descriptor	*epd = NULL;
+	register unsigned long sp asm ("sp");
+
+        if(pilot_host == NULL)
+                panic("pilot_host == NULL");
+
+        if(qh == NULL) {
+		unsigned long this_size;
+		this_size = ((unsigned long)sp) & (THREAD_SIZE-1);
+		this_size = THREAD_SIZE - this_size;
+		printk("pilot4_start_urb sp %lx current %p %s task_stack_page %p this_size %ld giveback_recursion %d thrinfo %p\n",
+			sp, current, current->comm, task_stack_page(current),this_size, giveback_recursion,current_thread_info());
+		panic("qh==null\n");
+        }
+
+	urb = next_urb(qh);
+	if(urb == NULL) {
+		unsigned long this_size;
+		hw_ep = qh->hw_ep;
+		this_size = ((unsigned long)sp) & (THREAD_SIZE-1);
+		this_size = THREAD_SIZE - this_size;
+		printk("pilot4_start_urb sp %lx current %p %s task_stack_page %p this_size %ld giveback_recursion %d thrinfo %p\n",
+			sp, current, current->comm, task_stack_page(current),this_size, giveback_recursion, current_thread_info());
+		if(qh->hep) {
+			epd = &qh->hep->desc;
+			printk("qh %p hep %p empty %d>? bEndpointAddress %d\n", qh, qh->hep, list_empty(&qh->hep->urb_list), epd->bEndpointAddress);
+		}
+		else
+			printk("qh %p\n", qh);
+		if(hw_ep)
+			printk("epnum %d is_in %d\n", hw_ep->ep_num, is_in);
+		panic("urb==null\n");
+	}
+	hw_ep = qh->hw_ep;
+	pipe = urb->pipe;
+	if(hw_ep ==NULL) {
+		unsigned long this_size;
+		this_size = ((unsigned long)sp) & (THREAD_SIZE-1);
+		this_size = THREAD_SIZE - this_size;
+		printk("pilot4_start_urb sp %lx current %p %s task_stack_page %p this_size %ld giveback_recursion %d thrinfo %p\n",
+			sp, current, current->comm, task_stack_page(current),this_size, giveback_recursion, current_thread_info());
+		panic("hw_ep ==NULL\n");
+	}
+
+	if(qh->free_recursion) {
+		printk("Recursion observed qh->free_recursion %d urblistempty  %d? giveback_recursion %d\n", qh->free_recursion, list_empty(&qh->hep->urb_list), giveback_recursion);
+		printk("hw_ep->cur_qh %p qh %p\n", hw_ep->cur_qh, qh);
+		printk("qh %p urb %p free_recursion %d giveback_qh %p free_recursion %d\n", qh, urb, qh->free_recursion, giveback_qh, (giveback_qh)?giveback_qh->free_recursion:-1);
+		printk("givebackQH:addr %d ep %d type %d curQH:addr %d ep %d type %d\n", 
+			giveback_qh->addr_reg,qh->epnum, qh->type,  qh->addr_reg, qh->epnum, qh->type);
+		print_symbol("caller is %s\n", (long)__builtin_return_address(0));
+	}
+
+	epnum = hw_ep->ep_num;
+
+	if(unlikely(pilot_host_udc->debug_level & DETAILED_RECURSION_PRINT && giveback_recursion)) {
+		unsigned long this_size;
+
+		this_size = ((unsigned long)sp) & (THREAD_SIZE-1);
+		this_size = THREAD_SIZE - this_size;
+		DBG_RECURSION_PRINT("pilot4_start_urb sp %lx current %p %s task_stack_page %p this_size %ld giveback_recursion %d thrinfo %p\n",
+			sp, current, current->comm, task_stack_page(current),this_size, giveback_recursion, current_thread_info());
+		DBG_RECURSION_PRINT("qh %p urb %p free_recursion %d giveback_qh %p free_recursion %d\n", qh, urb, qh->free_recursion, giveback_qh, (giveback_qh)?giveback_qh->free_recursion:-1);
+		DBG_RECURSION_PRINT("givebackQH:addr %d ep %d type %d curQH:addr %d ep %d type %d\n", 
+			giveback_qh->addr_reg,qh->epnum, qh->type,  qh->addr_reg, qh->epnum, qh->type);
+		print_symbol("caller is %s\n", (long)__builtin_return_address(0));
+	}
+
+	/* initialize software qh state */
+	qh->offset = 0;
+	qh->segsize = 0;
+	qh->retries = 0;
+	list_for_each_entry (temp_qh, &hw_ep->queue, ring) {
+		total_qh++;
+	}
+	if(0 && total_qh > 1 ){
+		printk("More than 1 qh is queued on hw epnum %d!!\n", epnum);
+		list_for_each_entry (temp_qh, &hw_ep->queue, ring) {
+			printk("qh->type %x qh->addr %x qh->epnum %d\n", temp_qh->type,temp_qh->addr_reg, temp_qh->epnum);
+		}
+	}
+#if 0
+	if(qh->type == USB_ENDPOINT_XFER_BULK)
+		printk("pilot4_start_urb qh->type %x epnum %d dev addr %x is_in 0x%x\n", qh->type, epnum, qh->addr_reg, is_in);
+#endif
+	/* gather right source of data */
+	switch (qh->type) {
+	case USB_ENDPOINT_XFER_CONTROL:
+		/* control transfers always start with SETUP */
+		is_in = 0;
+		pilot_host->ep0_stage = PILOT4_EP0_START;
+		buf = urb->setup_packet;
+		len = 8;
+#ifdef DEBUG_START_FINISH
+#ifdef CONFIG_SMP
+		if(!spin_is_locked(&pilot_host->lock)) {
+			printk("Spin not locked?\n");
+			dump_stack();
+		}
+#endif
+		if(hw_ep->cur_urb) {
+			printk("Previous urb still not finished actual length %d transferbuflength %d urb %p status %d\n",
+				hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_urb->status);
+			printk("start_control epnum %d busy bit set i ask y ??? epcs 0x%x addr %x programmed %d\n",
+				hw_ep->ep_num, *((volatile unsigned char*)(hw_ep->ep_cs_reg)), qh->addr_reg, qh->programmed);
+			printk("qh %p cur_qh %p empty %d? urb %p\n", qh, hw_ep->cur_qh, list_empty(&qh->hep->urb_list), urb);
+			dump_stack();
+		}
+		hw_ep->cur_urb = urb;
+		hw_ep->wait_time = msecs_to_jiffies(5000);
+		hw_ep->start_time = jiffies;
+		hw_ep->debug_timer.function = start_ep0_timer;
+		hw_ep->debug_timer.data = (unsigned long) hw_ep;
+		mod_timer(&hw_ep->debug_timer, jiffies + msecs_to_jiffies(5000));
+#endif
+
+		print_control_msg(buf);
+		break;
+	default:		/* bulk, interrupt */
+		buf = urb->transfer_buffer;
+		len = urb->transfer_buffer_length;
+		if(is_in != usb_pipein(pipe)){
+			printk("is_in %d != usb_pipein(%d) %d\n", is_in, pipe, usb_pipein(pipe));
+			is_in = usb_pipein(pipe);
+		}
+	}
+#if 0
+	DBG(4, "qh %p urb %p dev%d ep%d%s%s, hw_ep %d, %p/%d\n",
+			qh, urb, address, qh->epnum,
+			is_in ? "in" : "out",
+			({char *s; switch (qh->type) {
+			case USB_ENDPOINT_XFER_CONTROL:	s = ""; break;
+			case USB_ENDPOINT_XFER_BULK:	s = "-bulk"; break;
+			case USB_ENDPOINT_XFER_ISOC:	s = "-iso"; break;
+			default:			s = "-intr"; break;
+			}; s; }),
+			epnum, buf, len);
+#endif
+//	spin_lock_irqsave(&pilot_host->lock, flags);
+	//For now assign pilot_host->cur_qh =qh so that only 1 transaction can be active at a time.
+	hw_ep->cur_qh = qh;
+	hw_ep->pilot_host = pilot_host;
+//	spin_unlock_irqrestore(&pilot_host->lock, flags);
+
+	pilot4_ep_program(pilot_host, epnum, urb, !is_in, buf, len, hw_ep);
+
+	return;
+}
+
+/* caller owns controller lock, irqs are blocked */
+static void
+__pilot4_giveback(struct pilot_host_hcd *pilot_host, struct urb *urb, int status)
+__releases(pilot_host->lock)
+__acquires(pilot_host->lock)
+{
+	DBG(({ int level; switch (urb->status) {
+				case 0:
+					level = 4;
+					break;
+				/* common/boring faults */
+				case -EREMOTEIO:
+				case -ESHUTDOWN:
+				case -ECONNRESET:
+				case -EPIPE:
+					level = 3;
+					break;
+				default:
+					level = 2;
+					break;
+				}; level; }),
+			"complete %p (%d), dev%d ep%d%s, %d/%d\n",
+			urb, urb->status,
+			usb_pipedevice(urb->pipe),
+			usb_pipeendpoint(urb->pipe),
+			usb_pipein(urb->pipe) ? "in" : "out",
+			urb->actual_length, urb->transfer_buffer_length
+			);
+
+	usb_hcd_unlink_urb_from_ep(pilot_pilot4_to_hcd(pilot_host), urb);
+	if(print_giveback)
+		DBG_DETAILED_CLEANUP_PRINT("%s %d status %d\n", __FUNCTION__, __LINE__,urb->status);
+	spin_unlock(&pilot_host->lock);
+	usb_hcd_giveback_urb(pilot_pilot4_to_hcd(pilot_host), urb, status);
+	if(print_giveback)
+		DBG_DETAILED_CLEANUP_PRINT("%s %d\n", __FUNCTION__, __LINE__);
+	spin_lock(&pilot_host->lock);
+	if(print_giveback)
+		DBG_DETAILED_CLEANUP_PRINT("%s %d\n", __FUNCTION__, __LINE__);
+}
+
+/* for bulk/interrupt endpoints only */
+static inline void
+pilot4_save_toggle(struct pilot4_hw_ep *hw_ep, int is_in, struct urb *urb)
+{
+	int val = -1;
+	val = get_toggle(hw_ep->ep_num, !is_in);
+	usb_settoggle(urb->dev, hw_ep->cur_qh->epnum, !is_in, val);
+}
+
+static struct pilot4_qh * pick_next_transaction(struct pilot_host_hcd *pilot_host, int which,
+			int is_in,struct pilot4_hw_ep *hw_ep)
+{
+	struct pilot4_qh *qh = NULL;
+	if(!list_empty(&hw_ep->queue)) {
+		u8 addr;
+		//Move the current qh to tail
+		qh = container_of(hw_ep->queue.next, struct pilot4_qh, ring);
+		addr = qh->addr_reg;
+		return qh;
+	}
+	//TODO move head of QH to tail of ep queue and VERIFY
+	return NULL;
+}
+
+/* caller owns controller lock, irqs are blocked */
+static struct pilot4_qh *
+pilot4_giveback(struct pilot4_qh *qh, struct urb *urb, int status)
+{
+	int			is_in;
+	struct pilot4_hw_ep	*hw_ep = qh->hw_ep;
+	struct pilot_host_hcd	*pilot_host = hw_ep->pilot_host;
+	int			ready = qh->is_ready;
+	u8 type = qh->type;
+	u8 addr = qh->addr_reg;
+	u8 pick_next = 1;
+	u8 dont_free = 0;
+	register unsigned long sp asm ("sp");
+
+	if(unlikely(pilot_host_udc->debug_level & DETAILED_RECURSION_PRINT_DURING_GIVEBACK && giveback_recursion)) {
+		unsigned long this_size;
+		this_size = ((unsigned long)sp) & (THREAD_SIZE-1);
+		this_size = THREAD_SIZE - this_size;
+		DBG_RECURSION_PRINT("pilot4_giveback sp %lx current %p %s task_stack_page %p this_size %ld giveback_recursion %d thrinfo %p\n",
+			sp, current, current->comm, task_stack_page(current),this_size, giveback_recursion, current_thread_info());
+		DBG_RECURSION_PRINT("qh %p urb %p free_recursion %d giveback_qh %p free_recursion %d\n", qh, urb, qh->free_recursion, giveback_qh, (giveback_qh)?giveback_qh->free_recursion:-1);
+		DBG_RECURSION_PRINT("givebackQH:addr %d ep %d type %d curQH:addr %d ep %d type %d\n", 
+			giveback_qh->addr_reg,qh->epnum, qh->type,  qh->addr_reg, qh->epnum, qh->type);
+		print_symbol("caller is %s\n", (long)__builtin_return_address(0));
+	}
+
+	is_in = usb_pipein(urb->pipe);
+	in_giveback = 1;
+	giveback_recursion++;
+	giveback_qh = qh;
+	/* save toggle?*/
+//	if(type != USB_ENDPOINT_XFER_INT)
+	DBG(4, "pilot4_giveback type %u ready %d\n", type, ready);
+	if(print_giveback) {
+		DBG_DETAILED_CLEANUP_PRINT("%s %d pilot_host %p urb %p next_urb %p qh %p\n", __FUNCTION__, __LINE__, pilot_host, urb, next_urb(qh), qh);
+                if(pilot_host_udc->debug_level & DETAILED_CLEANUP_PRINT) {
+                        print_symbol("caller is %s\n", (long)__builtin_return_address(0));
+                }
+	}
+	qh->is_ready = 0;
+	hw_ep->last_freed_urb = urb;
+	if(urb != next_urb(qh)) {
+		printk("\n!!urb != next_urb(qh %p type %d urb %p nexturb %p\n",
+		qh, qh->type, urb, next_urb(qh));
+		pick_next = 0;
+	}
+	qh->free_recursion++;
+	if(hw_ep->cur_urb == urb) {
+#ifdef DEBUG_START_FINISH
+		if(hw_ep->bmAttributes != USB_ENDPOINT_XFER_INT)
+			del_timer(&hw_ep->debug_timer);
+		if(status) {
+			DBG_DETAILED_ERROR_PRINT("urb status finishing with error urb %p status %d jiffies %lu starttime %llu\n", urb, urb->status, jiffies, hw_ep->start_time);
+			DBG_DETAILED_ERROR_PRINT(" urb finished with error actual length %d transferbuflength %d urb %p\n",
+				hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb);
+			DBG_DETAILED_ERROR_PRINT("epnum %d epcs 0x%x addr %x programmed %d hw_ep->cur_qh %p\n",
+				hw_ep->ep_num, *((volatile unsigned char*)(hw_ep->ep_cs_reg)), qh->addr_reg, qh->programmed, hw_ep->cur_qh);
+		}
+#endif
+#ifdef HOST_DEBUG_STATS
+		hw_ep->completed_length += urb->actual_length;
+		hw_ep->last_complete_time = jiffies;
+		if(status) {
+			hw_ep->errors++;
+			hw_ep->total_fail_duration += (jiffies - hw_ep->start_time) & 0xFFFFFFFF;
+			if(hw_ep->max_failure_duration < ((jiffies - hw_ep->start_time) & 0xFFFFFFFF)) {
+				hw_ep->max_failure_duration = (jiffies - hw_ep->start_time) & 0xFFFFFFFF;
+			}
+		} else {
+			hw_ep->success++;
+			hw_ep->total_success_duration += (jiffies - hw_ep->start_time) & 0xFFFFFFFF;
+			if(hw_ep->max_success_duration < ((jiffies - hw_ep->start_time) & 0xFFFFFFFF)) {
+				hw_ep->max_success_duration = (jiffies - hw_ep->start_time) & 0xFFFFFFFF;
+			}
+		}
+#endif
+		hw_ep->cur_urb = NULL;
+		smp_wmb();
+	} else {
+#ifdef DEBUG_START_FINISH
+		if(type == USB_ENDPOINT_XFER_INT || type == USB_ENDPOINT_XFER_BULK) {
+			printk("giveback:hw_ep->cur_urb %p urb %p bmAttributes %d\n", hw_ep->cur_urb, urb, hw_ep->bmAttributes);
+			printk("%s error urb %p status %d jiffies %lu starttime %llu curwait_time %u\n", __FUNCTION__,
+				hw_ep->cur_urb, hw_ep->cur_urb->status, jiffies, hw_ep->start_time, hw_ep->wait_time);
+			printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+				hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_qh);
+			printk("current urb actual length %d transferbuflength %d urb %p cur_qh %p status %d\n",
+				urb->actual_length, urb->transfer_buffer_length, urb, qh, status);
+			dump_stack();
+		}
+#endif
+	}
+	__pilot4_giveback(pilot_host, urb, status);
+	qh->free_recursion--;
+	if(qh->free_recursion) {
+		printk("Recursion observed qh->free_recursion %d urblistempty  %d?\n", qh->free_recursion, list_empty(&qh->hep->urb_list));
+		printk("hw_ep->cur_qh %p qh %p\n", hw_ep->cur_qh, qh);
+	}
+	if(qh->ring.next == LIST_POISON1 || qh->ring.prev == LIST_POISON2) {
+		printk("qh %p qh->hep %p\n", qh, qh->hep);
+		if(qh->hep)
+			printk("urb_listempty %d?\n", list_empty(&qh->hep->urb_list));
+		panic("qh is already freed List is already empty!\n");
+		dont_free = 1;
+	}
+	if(print_giveback)
+		DBG_DETAILED_CLEANUP_PRINT("%s %d qh %p hw_ep %p last_freed_qh %p\n", __FUNCTION__, __LINE__,qh,hw_ep, hw_ep->last_freed_qh);
+	qh->is_ready = ready;
+	if(qh != hw_ep->cur_qh || qh->free_recursion)
+		pick_next = 0;
+	if(print_giveback)
+		DBG_DETAILED_CLEANUP_PRINT("%s %d pick_next %d\n", __FUNCTION__, __LINE__, pick_next);
+	if(pick_next)
+		qh->programmed = 0;//Not programmed any more
+	/* reclaim resources (and bandwidth) ASAP; deschedule it, and
+	 * invalidate qh as soon as list_empty(&hep->urb_list)
+	 */
+	 /*Can there be race conditions where in callback handler during giveback does dequeue and frees up qh*/
+	/*Free only on the last instance */
+	if (list_empty(&qh->hep->urb_list) && !qh->free_recursion) {
+		if(print_giveback)
+			DBG_DETAILED_CLEANUP_PRINT("%s %d\n", __FUNCTION__, __LINE__);
+		if(1 && qh->type == USB_ENDPOINT_XFER_INT)
+			printk("qhaddr %x qh list is empty\n", qh->addr_reg);
+		if(pick_next)
+			hw_ep->cur_qh = (void*)0xDEADBEEF;
+		hw_ep->last_freed_qh = qh;
+		if(print_giveback)
+			DBG_DETAILED_CLEANUP_PRINT("%s %d\n", __FUNCTION__, __LINE__);
+		qh->hep->hcpriv = NULL;
+		if(print_giveback)
+			DBG_DETAILED_CLEANUP_PRINT("%s %d\n", __FUNCTION__, __LINE__);
+		if(type != USB_ENDPOINT_XFER_INT)
+			DBG(4, "Deleting list type %u is_in %d\n", type, is_in);
+		if(pick_next && timer_pending(&qh->hw_ep->intr_ep_timer)){
+			del_timer(&qh->hw_ep->intr_ep_timer);
+			printk("Timer deleted 1 for epnum %d in %d\n", qh->hw_ep->ep_num, is_in);
+		}
+		if(print_giveback)
+			DBG_DETAILED_CLEANUP_PRINT("%s %d\n", __FUNCTION__, __LINE__);
+		list_del(&qh->ring);
+		if(print_giveback)
+			DBG_DETAILED_CLEANUP_PRINT("%s %d\n", __FUNCTION__, __LINE__);
+		kfree(qh);
+		if(print_giveback)
+			DBG_DETAILED_CLEANUP_PRINT("%s %d\n", __FUNCTION__, __LINE__);
+		qh = NULL;
+		switch (type) {
+
+		case USB_ENDPOINT_XFER_CONTROL:
+	 		//TODO Check if this is correct
+			if (!list_empty(&hw_ep->queue)) {
+				qh = container_of(hw_ep->queue.next, struct pilot4_qh, ring);
+				return qh;
+			}
+		case USB_ENDPOINT_XFER_BULK:
+			break;
+		case USB_ENDPOINT_XFER_INT:
+			break;
+		}
+	}else{
+			if(print_giveback)
+				DBG_DETAILED_CLEANUP_PRINT("%s %d pick_next %d\n", __FUNCTION__, __LINE__, pick_next);
+			if(pick_next) {
+				list_move_tail(&qh->ring, &hw_ep->queue);
+			if(timer_pending(&qh->hw_ep->intr_ep_timer)){
+				del_timer(&qh->hw_ep->intr_ep_timer);
+				printk("Timer deleted 2 for epnum %d in %d\n", qh->hw_ep->ep_num, is_in);
+			}		
+		}
+	}
+	
+	if(pick_next) {
+		if(print_giveback)
+			DBG_DETAILED_CLEANUP_PRINT("%s %d hw_ep %p \n", __FUNCTION__, __LINE__, hw_ep);
+
+		if(start_intr_print && hw_ep->bmAttributes == USB_ENDPOINT_XFER_INT)
+			printk("hw_ep->cur_qh %p addr_reg %x\n", hw_ep->cur_qh, (qh)?qh->addr_reg:-1);
+		qh = pick_next_transaction(pilot_host, type, is_in, hw_ep);
+		if(print_giveback)
+			DBG_DETAILED_CLEANUP_PRINT("%s %d hw_ep %p qh %p pilot_host %p\n", __FUNCTION__, __LINE__, hw_ep, qh, pilot_host);
+		if(qh == NULL) {
+			if(0 && type == USB_ENDPOINT_XFER_BULK)
+				printk("initiaing to NULL addr %x ep_num %d\n", addr,hw_ep->ep_num);
+			hw_ep->cur_qh = NULL;
+			pilot_host->cur_qh = NULL;//Redundant 
+		} else {
+			if(start_intr_print && hw_ep->bmAttributes == USB_ENDPOINT_XFER_INT)
+				printk("addr %x nextaddr %x type %d\n", addr, qh->addr_reg, qh->type);
+		}
+	}
+	if(print_giveback)
+		DBG_DETAILED_CLEANUP_PRINT("%s %d hw_ep %p \n", __FUNCTION__, __LINE__, hw_ep);
+	in_giveback = 0;
+	giveback_recursion--;
+	return qh;
+}
+
+/*
+ * Advance this hardware endpoint's queue, completing the specified urb and
+ * advancing to either the next urb queued to that qh, or else invalidating
+ * that qh and advancing to the next qh scheduled after the current one.
+ *
+ * Context: caller owns controller lock, irqs are blocked
+ */
+static void
+advance_schedule(struct pilot_host_hcd *pilot_host, struct urb *urb,
+		struct pilot4_hw_ep *hw_ep, int is_in)
+{
+#if 1
+	struct pilot4_qh	*qh;
+	if(hw_ep == NULL){
+		printk("\nhw_ep == NULL!!\n");
+#ifndef CONFIG_USB_PILOT4_HCD_MODULE
+		show_stack(NULL, NULL);
+#endif
+	}
+	qh = hw_ep->cur_qh;
+	if((unsigned int)qh == 0xDEADBEEF || qh == NULL) {
+		printk("\nqh not proper!! qh %p last_freed_qh %p hw_ep->last_freed_urb %p\n", qh, hw_ep->last_freed_qh, hw_ep->last_freed_urb);
+		print_symbol("caller is %s\n", (long)__builtin_return_address(0));
+	}
+	if(urb == NULL)
+		printk("\nurb is NULL!!\n");
+	if(print_giveback) {
+		DBG_DETAILED_CLEANUP_PRINT("%s %d hw_ep->cur_qh %p\n", __FUNCTION__, __LINE__, hw_ep->cur_qh);
+                if(pilot_host_udc->debug_level & DETAILED_CLEANUP_PRINT) {
+                        print_symbol("caller is %s\n", (long)__builtin_return_address(0));
+                }
+	}
+	switch (qh->type) {
+	case USB_ENDPOINT_XFER_BULK:
+	case USB_ENDPOINT_XFER_INT:
+	if(print_giveback)
+		DBG_DETAILED_CLEANUP_PRINT("%s %d qh->type %d\n", __FUNCTION__, __LINE__, qh->type);
+
+		pilot4_save_toggle(hw_ep, is_in, urb);
+	if(print_giveback)
+		DBG_DETAILED_CLEANUP_PRINT("%s %d\n", __FUNCTION__, __LINE__);
+
+		break;
+	case USB_ENDPOINT_XFER_ISOC:
+		printk("USB_ENDPOINT_XFER_ISOC!\n");
+		break;
+	}
+	if(print_giveback)
+		DBG_DETAILED_CLEANUP_PRINT("%s %d urb->status %d\n", __FUNCTION__, __LINE__, urb->status);
+//	if(qh->type != USB_ENDPOINT_XFER_INT)
+		DBG(4, "advance_schedule qh %p urb %p is_in %d\n", qh, urb, is_in);
+	if (urb->status == -EINPROGRESS)
+		qh = pilot4_giveback(qh, urb, 0);
+	else
+		qh = pilot4_giveback(qh, urb, urb->status);
+	if(print_giveback)
+		DBG_DETAILED_CLEANUP_PRINT("%s %d qh %p hw_ep %p\n", __FUNCTION__, __LINE__, qh, hw_ep);
+
+	if (qh && qh->is_ready && !list_empty(&qh->hep->urb_list)) {
+		if(start_intr_print && hw_ep->bmAttributes == USB_ENDPOINT_XFER_INT)
+			printk("... next ep%d %cX urb %p\n",
+				hw_ep->ep_num, is_in ? 'R' : 'T',
+				next_urb(qh));
+
+		DBG(4, "... next ep%d %cX urb %p\n",
+				hw_ep->ep_num, is_in ? 'R' : 'T',
+				next_urb(qh));
+		pilot4_start_urb(pilot_host, is_in, qh);
+	} else {
+		if(1 && hw_ep->bmAttributes == USB_ENDPOINT_XFER_INT) {
+			if(qh)
+				printk("qh %p qh->is_ready %d list_empty(&qh->hep->urb_list) %d\n",
+					qh, qh->is_ready, list_empty(&qh->hep->urb_list));
+			else
+				printk("qh == null \n");
+		}
+	}
+#endif
+}
+
+
+static char * inerr[] = {"no error", "CRC error", "data toggle mismatch", "endpoint sent STALL handshake", 
+			"no endpoint handshake", "PID error", "Data Overrun", "Data Underrun"};
+static int check_hcin_error_details(u8 hcinerr, int epnum)
+{
+	int error_status = 0;
+//	DBG(4, "IN ep %d in error hw_retries %d hcinerr %x\n", epnum, hcinerr & 0x3, hcinerr);
+	hcinerr = hcinerr >> 2;
+	error_status = (hcinerr & 0x7);
+	if(error_status == 0)
+		return 1;
+	printk("Which error: %s\n", inerr[error_status]);
+#if 0
+	if(error_status == 1 || error_status == 2)
+		return 1;
+#endif
+	if(error_status == 3)//stall
+		return -EPIPE;
+	else if(error_status == 6)
+		return -EOVERFLOW;
+	else if(error_status == 4)
+		return -ECONNRESET;
+	else
+		return -EPROTO;
+}
+static int check_hcout_error_details(u8 hcouterr, int epnum)
+{
+	int error_status = 0;
+	printk("OUT ep %d in error hw_retries %d\n", epnum, hcouterr & 0x3);
+	hcouterr = hcouterr >> 2;
+	error_status = (hcouterr & 0x7);
+	printk("Which error: %s\n", inerr[error_status]);
+	if(error_status == 0)
+		return 1;
+	if(error_status == 3)//stall
+		return -EPIPE;
+	else if(error_status == 4)
+		return -ECONNRESET;
+	else
+		return -EPROTO;
+
+}
+int hcin0err_handler(struct pilot_host_hcd *pilot_host)
+{
+#if 1
+	struct urb		*urb = NULL;
+	int			status = 0, worthretry  = 0;
+	struct pilot4_hw_ep	*hw_ep = &pilot_host->endpoints[0];	
+	struct pilot4_qh		*qh = hw_ep->cur_qh;
+	bool			complete = false;
+	irqreturn_t		retval = IRQ_NONE;
+	u8	hcinerr = 0;
+	unsigned long flags;
+
+	*((volatile unsigned short*)(pilot_host->base_addr + hcin07errirq)) = (1 << 0);//ack interrupt
+
+	spin_lock_irqsave(&pilot_host->lock, flags);
+	if(qh == NULL || ((unsigned int)qh == 0xDEADBEEF)) {
+		(void)check_hcin_error_details(*((volatile u8*)(hw_ep->hcinerr)), 0);
+		printk("qh== %p enjoy! get out of the system err hcin%d\n",qh, 0);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+//		endpoint_reset(x, 0);
+		return -1;
+	}
+
+ 	hcinerr = *((volatile unsigned char*)(hw_ep->hcinerr));
+	worthretry = check_hcin_error_details(hcinerr, 0);
+	printk("\n\n! hcin0err_handler hcinerr %x worthretry %d qh %p\n\n\n",hcinerr, worthretry, qh);
+	if(worthretry > 0) {
+		qh->retries++;
+		if(qh->retries < 10) {
+			status = 0;
+			*((volatile unsigned char*)(hw_ep->hcinerr)) = (1 << 5);//retry set resend bit
+		}
+		else {
+			*((volatile unsigned char*)(hw_ep->hcinerr)) = 0;//no retry
+			status = -ETIME;
+		}
+	} else {
+			*((volatile unsigned char*)(hw_ep->hcinerr)) = 0;//no retry
+			//Now the endpoint is halted and we cannot reset EP0 
+			//need to check what will be done.
+			status = worthretry;
+	}
+	urb = next_urb(qh);
+
+	if(status){
+		complete = true;
+		retval = IRQ_HANDLED;
+		if (urb)
+			urb->status = status;
+	}
+	
+	if(complete)
+		advance_schedule(pilot_host, urb, hw_ep, USB_DIR_IN);
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+
+	return retval;
+#endif
+}
+
+int hcout0err_handler(struct pilot_host_hcd *pilot_host)
+{
+#if 0
+	struct urb		*urb;
+	u16			csr, len;
+	int			status = 0;
+//	struct pilot4_qh		*qh = hw_ep->cur_qh;
+	bool			complete = false;
+	irqreturn_t		retval = IRQ_NONE;
+#else
+	struct pilot4_hw_ep	*hw_ep = &pilot_host->endpoints[0];
+#endif
+	*((volatile unsigned short*)(pilot_host->base_addr + hcout07errirq)) = (1 << 0);//ack interrupt
+	printk("\n\nep0 hcouterr 0x%x\n\n\n",*((volatile unsigned char*)(hw_ep->hcouterr)));
+	printk("\n\nhcout0err_handler Not Implemented\n\n\n!!!");
+	return -1;
+}
+
+int hcep0in_handler(struct pilot_host_hcd *pilot_host)
+{
+#if 1
+	struct urb		*urb;
+	u16	len;
+	struct pilot4_hw_ep	*hw_ep = &pilot_host->endpoints[0];
+	struct pilot4_qh		*qh = hw_ep->cur_qh;
+	bool			complete = false;
+	irqreturn_t		retval = IRQ_NONE;
+	unsigned long flags;
+
+	*((volatile unsigned char*)(pilot_host->base_addr + hcin07irq)) = 1;//ack interrupt
+
+	spin_lock_irqsave(&pilot_host->lock, flags);
+	len = *((volatile unsigned char*)(hw_ep->ep_byte_cnt_reg));
+	urb = next_urb(qh);
+	
+	if (PILOT4_EP0_STATUS == pilot_host->ep0_stage) {
+		retval = IRQ_HANDLED;
+		complete = true;
+	}
+//	printk("hcep0in_handler len %d urb %p\n", len, urb);
+	if (!complete) {
+		pilot4_ep0_continue(pilot_host, len, urb);
+	} else
+		advance_schedule(pilot_host, urb, hw_ep, USB_DIR_IN);
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+	return retval;
+#endif
+}
+int hcep0out_handler(struct pilot_host_hcd *pilot_host)
+{
+	struct urb		*urb;
+	struct pilot4_hw_ep	*hw_ep = &pilot_host->endpoints[0];	
+	struct pilot4_qh		*qh = hw_ep->cur_qh;
+	bool			complete = false;
+	irqreturn_t		retval = IRQ_NONE;
+	unsigned long flags;
+
+	*((volatile unsigned char*)(pilot_host->base_addr + hcout07irq)) = 1;//ack interrupt
+	spin_lock_irqsave(&pilot_host->lock, flags);
+	if(qh == NULL || ((unsigned int)qh == 0xDEADBEEF)) {
+		printk("hcep0out_handler qh %p error\n", qh);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return -1;
+	}
+	urb = next_urb(qh);
+	if(urb == NULL) {
+		printk("qh %p addr_reg %x urb is NULL!!\n", qh, qh->addr_reg);
+		return -1;
+	}
+#if 0
+	DBG(4, "<== csr0 %04x, qh %p, count %d, urb %p, stage %d\n",
+		csr, qh, len, urb, pilot_host->ep0_stage);
+#endif
+	/* if we just did status stage, we are done */
+	if (PILOT4_EP0_STATUS == pilot_host->ep0_stage) {
+		retval = IRQ_HANDLED;
+		complete = true;
+	}
+	if (!complete) {
+		pilot4_ep0_continue(pilot_host, 0, urb);
+	} else
+		advance_schedule(pilot_host, urb, hw_ep, USB_DIR_OUT);
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+	return retval;
+}
+
+static int program_ep0(struct pilot4_hw_ep	*hw_ep, struct pilot_host_hcd *pilot_host, 
+	u8 *buf, u32 len)
+{
+#if 1
+	int i =0;
+#define START_SETUP_TOKEN	(1<<4)
+#ifdef DEBUG_START_FINISH
+#ifdef CONFIG_SMP
+	if(!spin_is_locked(&pilot_host->lock)) {
+		dump_stack();
+	}
+#endif
+#endif
+	switch(pilot_host->ep0_stage) {
+		case PILOT4_EP0_START:
+			*((volatile unsigned char*)(pilot_host->base_addr + HOST_FUNC_ADDR)) = hw_ep->cur_qh->addr_reg & 0x7F;
+ 			*((volatile unsigned char*)(hw_ep->ep_cs_reg)) = START_SETUP_TOKEN;
+#ifdef HOST_DEBUG
+			if(len!= 8)
+				printk("setup pkt len!= 8 len %d buf %p\n", len , buf);
+#endif
+			for(i = 0;i < 8;i++)
+				*((volatile unsigned char*)(hw_ep->ep_data_buf_reg1 + i)) = buf[i];
+			hw_ep->cur_qh->programmed = 1;
+			*((volatile unsigned char*)(hw_ep->ep_byte_cnt_reg1)) = 8;
+			break;
+		case PILOT4_EP0_IN:
+			printk("EP0 start with PILOT4_EP0_IN Is it possible\n");
+			break;
+		case PILOT4_EP0_OUT:
+			printk("EP0 start with PILOT4_EP0_OUT Is it possible\n");
+			break;
+		default:
+			break;			
+	}
+	return 0;
+#endif
+}
+static inline int get_toggle(u8 epnum, unsigned int is_out)
+{
+	int val = -1;
+	*((volatile unsigned char*)(pilot_host_udc->base_addr + HCENDPRST)) = ((is_out << 4) | epnum);
+	val = *((volatile unsigned char*)(pilot_host_udc->base_addr + HCENDPRST)) & 0x80;
+	DBG(4, "get_toggle epnum %d is %d\n", epnum, val?1:0);
+	if(val)
+		return 1;
+	return 0;
+}
+
+static inline void set_toggle(u8 epnum, unsigned int is_out,u8 one)
+{
+	DBG(4, "Setting toggle bit %d for %d %s Endpoint \n", one, epnum, (is_out)?"OUT":"IN");
+	*((volatile unsigned char*)(pilot_host_udc->base_addr + HCENDPRST)) = ((is_out << 4) | epnum);
+	
+	if(one) {
+		//set toggle
+		*((volatile unsigned char*)(pilot_host_udc->base_addr + HCENDPRST)) = 
+			((1 << 7) | (is_out << 4) | epnum);
+	} else {
+		//clear toggle
+		*((volatile unsigned char*)(pilot_host_udc->base_addr + HCENDPRST)) = 
+			((1 << 5) | (is_out << 4) | epnum);
+	}
+	return;
+}
+#if 0
+inline void endpoint_reset(struct pilot4_hw_ep *hw_ep, unsigned int is_out)
+{
+	printk("endpoint_reset for %d %s EP\n", hw_ep->ep_num, (is_out)?"OUT":"IN");
+	/*H/w Bug for IN Endpoints during reset */
+#ifdef HW_BUG
+	if(!is_out)
+		return;
+#endif
+#if 0
+	if(is_out)
+		*((volatile unsigned char*)(hw_ep->hc_out_con)) &= ~(1<<7);
+	else
+		*((volatile unsigned char*)(hw_ep->hc_in_con)) &= ~(1<<7);
+#endif
+	//Do FIFO Reset
+	*((volatile unsigned char*)(pilot_host_udc->base_addr + HCENDPRST)) = ((is_out << 4) | hw_ep->ep_num);
+	*((volatile unsigned char*)(pilot_host_udc->base_addr + HCENDPRST)) = 
+		((1 << 6) | (1 << 5) | (is_out << 4) | hw_ep->ep_num);
+#if 0
+	if(is_out)
+		*((volatile unsigned char*)(hw_ep->hc_out_con)) |= (1<<7);
+	else
+		*((volatile unsigned char*)(hw_ep->hc_in_con)) |= (1<<7);
+#endif
+	printk("after reset: ep_cs_reg %x\n", *((volatile unsigned char*)(hw_ep->ep_cs_reg)));
+	return;
+}
+#else
+int clear_intr_status(struct pilot4_hw_ep *hw_ep, unsigned int is_out)
+{
+	struct pilot4_qh *qh = NULL;
+	struct urb *urb = NULL;
+	ushort curr_status, curr_err_status;
+	int timeout = 0;
+	int pass = 1;
+
+	qh = hw_ep->cur_qh;
+	if(qh)
+		urb = next_urb(qh);
+
+	if(is_out) {
+		curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07irq));
+		curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07errirq));
+		if((curr_status & (1 << hw_ep->ep_num)) && (curr_err_status & (1 << hw_ep->ep_num))) {
+			printk("Both normal and err intr status is set THis can't be possible\n");
+		}
+		printk("hcout07irq %x status set  %d? curr_err_status %x cur_urb %p urb %p\n",
+			curr_status, (curr_status & (1 << hw_ep->ep_num)), curr_err_status, hw_ep->cur_urb, urb);
+		printk("hcouterr %x\n",*((volatile unsigned char*)(hw_ep->hcouterr)));
+		if((curr_err_status & (1 << hw_ep->ep_num))) {
+			printk("hcouterr %x\n",*((volatile unsigned char*)(hw_ep->hcouterr)));
+//			*((volatile unsigned char*)(hw_ep->hcouterr)) = 0;
+		}
+		if(hw_ep->cur_urb && hw_ep->cur_urb == urb) {
+			curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07irq));
+			curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07errirq));
+			while(!(curr_status & (1 << hw_ep->ep_num)) && !(curr_err_status & (1 << hw_ep->ep_num))) {
+				udelay(1);
+				curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07irq));
+				curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07errirq));
+				timeout++;
+				if(timeout >= 15000)
+					break;
+			}
+			*((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07irq)) = (1 << hw_ep->ep_num);//Clear status
+			*((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07errirq)) = (1 << hw_ep->ep_num);//Clear status
+		}
+		curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07irq));
+		curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07errirq));
+
+		if((curr_status & (1 << hw_ep->ep_num)) || (curr_err_status & (1 << hw_ep->ep_num)) || timeout) {
+			printk("hcout07irq %x status still set  %d? curr_err_status %x errset %d timeout %d\n",
+				curr_status, (curr_status & (1 << hw_ep->ep_num)), curr_err_status, (curr_err_status & (1 << hw_ep->ep_num)), timeout);
+			pass = 0;
+		}
+	} else {
+		curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07irq));
+		curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07errirq));
+		if((curr_status & (1 << hw_ep->ep_num)) && (curr_err_status & (1 << hw_ep->ep_num))) {
+			printk("Both normal and err intr status is set THis can't be possible\n");
+		}
+		printk("hcin07irq %x status set  %d? curr_err_status %x cur_urb %p urb %p\n",
+			curr_status, (curr_status & (1 << hw_ep->ep_num)), curr_err_status, hw_ep->cur_urb, urb);
+		printk("hcinerr %x\n",*((volatile unsigned char*)(hw_ep->hcinerr)));
+		if((curr_err_status & (1 << hw_ep->ep_num))) {
+			printk("hcinerr %x\n",*((volatile unsigned char*)(hw_ep->hcinerr)));
+	//		*((volatile unsigned char*)(hw_ep->hcinerr)) = 0;
+		}
+		if(hw_ep->cur_urb && hw_ep->cur_urb == urb) {
+			curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07irq));
+			curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07errirq));
+			while(!(curr_status & (1 << hw_ep->ep_num)) && !(curr_err_status & (1 << hw_ep->ep_num))) {
+				udelay(1);
+				curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07irq));
+				curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07errirq));
+				timeout++;
+				if(timeout >= 15000)
+					break;
+			}
+			*((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07irq)) = (1 << hw_ep->ep_num);//Clear status
+			*((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07errirq)) = (1 << hw_ep->ep_num);//Clear status
+		}
+		curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07irq));
+		curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07errirq));
+
+		if((curr_status & (1 << hw_ep->ep_num)) || (curr_err_status & (1 << hw_ep->ep_num)) || timeout) {
+			printk("hcin07irq %x status still set  %d? curr_err_status %x errset %d timeout %d\n",
+				curr_status, (curr_status & (1 << hw_ep->ep_num)), curr_err_status, (curr_err_status & (1 << hw_ep->ep_num)), timeout);
+			pass = 0;
+		}
+	}
+	return pass;
+}
+
+
+inline int endpoint_reset(struct pilot4_hw_ep *hw_ep, unsigned int is_out)
+{
+	struct pilot4_qh *qh = NULL;
+	struct urb *urb = NULL;
+	ushort prev_status, prev_err_status;
+	ushort curr_status, curr_err_status;
+	u8 prev_cs_reg, cur_cs_reg;
+	u8 ep_errstatus, ep_errstatus2;
+	ushort curr_intr_enable, curr_err_intr_enable;
+
+	//check if lock is aquired TODO
+	if(hw_ep->ep_num == 0) {
+		panic("endpoint_reset for EP 0!!\n");
+	}
+	qh = hw_ep->cur_qh;
+	if(qh)
+		urb = next_urb(qh);
+	if(is_out) {
+		prev_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07irq));
+		prev_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07errirq));
+		curr_intr_enable = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07ien));
+		curr_err_intr_enable = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07errien));
+
+		ep_errstatus = *((volatile unsigned char*)(hw_ep->hcouterr));
+		if((prev_err_status & (1 << hw_ep->ep_num))) {
+			*((volatile unsigned char*)(hw_ep->hcouterr)) = 0;
+		}
+		ep_errstatus2 = *((volatile unsigned char*)(hw_ep->hcouterr));
+	} else {
+		prev_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07irq));
+		prev_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07errirq));
+		curr_intr_enable = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07ien));
+		curr_err_intr_enable = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07errien));
+
+		ep_errstatus = *((volatile unsigned char*)(hw_ep->hcinerr));
+		if((prev_err_status & (1 << hw_ep->ep_num))) {
+			*((volatile unsigned char*)(hw_ep->hcinerr)) = 0;
+		}
+		ep_errstatus2 = *((volatile unsigned char*)(hw_ep->hcinerr));
+	}
+	prev_cs_reg = *((volatile unsigned char*)(hw_ep->ep_cs_reg));
+	DBG_DETAILED_ERROR_PRINT("EP reset for %d %s EP epcs %x prev_status %x prev_err_status %x ep_errstatus %x\n",
+		hw_ep->ep_num, (is_out)?"OUT":"IN", *((volatile unsigned char*)(hw_ep->ep_cs_reg)), prev_status, prev_err_status, ep_errstatus);
+	DBG_DETAILED_ERROR_PRINT("curr_intr_enable %x curr_err_intr_enable %x ep_errstatus2 %x ctrlreg %x\n",
+		curr_intr_enable, curr_err_intr_enable, ep_errstatus2, (is_out)?hw_ep->hc_out_con:hw_ep->hc_in_con);
+	/*H/w Bug for IN Endpoints during reset */
+#ifdef HW_BUG
+	if(!is_out)
+		return;
+#endif
+	//Do FIFO Reset
+	*((volatile unsigned char*)(pilot_host_udc->base_addr + HCENDPRST)) = ((is_out << 4) | hw_ep->ep_num);
+	*((volatile unsigned char*)(pilot_host_udc->base_addr + HCENDPRST)) = 
+		((1 << 6) | (1 << 5) | (is_out << 4) | hw_ep->ep_num);
+	udelay(1);//some small delay TODO check if 100ns is ok
+	cur_cs_reg = *((volatile unsigned char*)(hw_ep->ep_cs_reg));
+	if(is_out) {
+		curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07irq));
+		curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07errirq));
+		ep_errstatus = *((volatile unsigned char*)(hw_ep->hcouterr));
+	} else {
+		curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07irq));
+		curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07errirq));
+		ep_errstatus = *((volatile unsigned char*)(hw_ep->hcinerr));
+	}
+	DBG_DETAILED_ERROR_PRINT("after reset: epcs %x curr_status %x curr_err_status %x ep_errstatus  %x\n",
+		cur_cs_reg, curr_status, curr_err_status, ep_errstatus);
+	if((prev_cs_reg & 0x2) && !(cur_cs_reg & 0x2)) {
+		return 1;
+	}
+	return 0;
+}
+
+#endif
+
+static int ep_read_fifo(struct pilot_host_hcd *pilot_host, struct urb *urb, struct pilot4_hw_ep *hw_ep)
+{
+	int i;
+	struct pilot4_qh		*qh = hw_ep->cur_qh;
+	unsigned max = qh->hw_ep->maxpacket;
+	char	 *ubuf = NULL;
+	int byte_cnt = 0;
+	int err = 0;
+	byte_cnt = *((volatile unsigned short*)(hw_ep->ep_byte_cnt_reg));
+#ifdef HOST_DEBUG
+	if(byte_cnt > max || (urb->actual_length + byte_cnt) > urb->transfer_buffer_length) {
+		printk("ep_read_fifo byte_cnt > max !! %d > %d actual_length %d transfer_buffer_length %d\n",
+			byte_cnt, max, urb->actual_length, urb->transfer_buffer_length);
+	}
+#endif
+	if((urb->actual_length + byte_cnt) > urb->transfer_buffer_length) {
+		byte_cnt = (urb->transfer_buffer_length - urb->actual_length);
+		err = -EOVERFLOW;
+	}
+	ubuf = urb->transfer_buffer + urb->actual_length;
+//	if(qh->type != USB_ENDPOINT_XFER_INT)
+		DBG(4, "actual_length %d max %d byte_cnt %d\n", urb->actual_length, max, byte_cnt);
+	for(i = 0 ; i < byte_cnt; i++) {
+		*(ubuf) = *(u8 *)(hw_ep->ep_data_buf_reg);
+		ubuf++;
+	}
+	qh->offset += byte_cnt;
+	if(err)
+		return err;
+	if(byte_cnt < max)
+		return 1;
+	return 0;
+}
+
+static int ep_write_fifo(struct pilot_host_hcd *pilot_host, struct urb *urb, struct pilot4_hw_ep *hw_ep)
+{
+#if 1
+	int i;
+	int is_last = 0;
+	int byte_cnt;
+	struct pilot4_qh		*qh = hw_ep->cur_qh;
+	unsigned max = qh->hw_ep->maxpacket;
+	unsigned char * temp_buf = NULL;
+
+	temp_buf = (unsigned char *)(((unsigned char *)(urb->transfer_buffer)) + qh->offset);
+	byte_cnt = min((unsigned)(urb->transfer_buffer_length- urb->actual_length) , max);
+	DBG(4, "qh->offset 0x%x max %d byte_cnt %d\n", qh->offset, max, byte_cnt);
+	while(*((volatile unsigned char*) (qh->hw_ep->ep_cs_reg)) & 0x2) {
+		printk("ep_write_fifo:busy not clear epnum %d addr_reg %d\n", hw_ep->ep_num, qh->addr_reg);
+		printk("qh %p cur_qh %p empty %d?\n", qh, hw_ep->cur_qh, list_empty(&qh->hep->urb_list));
+	}
+	for(i = 0 ; i < byte_cnt; i++) {
+		*(u8 *)(qh->hw_ep->ep_data_buf_reg) = *(temp_buf);
+		temp_buf++;
+	}
+	qh->offset += byte_cnt;
+	qh->programmed	= 1;
+	//trigger h/w to start sending this packet to host
+	*((volatile unsigned short*) (qh->hw_ep->ep_byte_cnt_reg)) = byte_cnt;
+	*((volatile unsigned char*) (qh->hw_ep->ep_cs_reg)) = 0x00;
+	if((*((volatile unsigned char*)(hw_ep->ep_cs_reg)) & 0x2) != 0x2) {
+		printk("writefifo:Busy bit not set? epnum %d epcs %x byte_cnt %d\n",
+			hw_ep->ep_num, *((volatile unsigned char*)(hw_ep->ep_cs_reg)), byte_cnt);
+	}
+	/* last packet is often short (sometimes a zlp) */
+	if (byte_cnt < qh->hw_ep->maxpacket)
+		is_last = 1;
+	else
+		is_last = 0;
+
+	return is_last;
+#endif
+}
+#ifdef HOST_DEBUG
+static struct urb *out_urb = NULL, *in_urb = NULL;
+#endif
+static int hcoutxirq_handler(struct pilot_host_hcd *pilot_host, int x, struct pilot4_hw_ep *hw_ep)
+{
+	struct pilot4_qh *qh;
+	struct urb *urb = NULL;
+	unsigned long flags;
+	*((volatile unsigned short*)(pilot_host->base_addr + hcout07irq)) = (1 << x);//ack interrupt
+
+	spin_lock_irqsave(&pilot_host->lock, flags);
+	if(hw_ep == NULL) {
+		panic("hcoutxirq_handler ep num %d is NULL!\n", x);
+	}
+	qh = hw_ep->cur_qh;
+#ifdef HOST_DEBUG
+	if(qh == NULL) {
+		printk("qh==NULL enjoy! get out of the system hcout%d\n",x);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return IRQ_HANDLED;
+	}
+#endif
+	smp_rmb();
+	urb = next_urb(qh);
+	if(urb == NULL) {
+		printk("%s urb is NULL hw_ep->cur_urb %p ep_num %d\n", __FUNCTION__, hw_ep->cur_urb, hw_ep->ep_num);
+		printk("hw_ep %p bulkin %p bulk out %p interin %p\n", hw_ep, pilot_host->bulk_in, pilot_host->bulk_out, pilot_host->intr_in);
+		printk("qh %p\n", qh);
+		printk("%s hw_ep free_recursion %d epnum %d %d\n", __FUNCTION__, qh->free_recursion, hw_ep->ep_num, qh->hw_ep->ep_num);
+
+		WARN(1, "urb ==NULL enjoy! get out of the system hcout%d\n",x);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return IRQ_HANDLED;
+	}
+
+	if(hw_ep->cur_urb && hw_ep->cur_urb != urb) {
+#ifdef DEBUG_START_FINISH
+		printk("%s error urb %p status %d jiffies %lu starttime %llu curwait_time %u\n", __FUNCTION__,
+			hw_ep->cur_urb, hw_ep->cur_urb->status, jiffies, hw_ep->start_time, hw_ep->wait_time);
+#endif
+		printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_qh);
+		printk("current urb actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			urb->actual_length, urb->transfer_buffer_length, urb, qh);
+	} else if(hw_ep->cur_urb == NULL && urb) {
+		printk("hcoutx act length %d transferbuflength %d urb %p cur_qh %p addr %x cururb %p ep_num %d\n",
+			urb->actual_length, urb->transfer_buffer_length, urb, qh, qh->addr_reg, hw_ep->cur_urb, hw_ep->ep_num);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return IRQ_HANDLED;
+	}
+	if(unlikely(qh->abort_pending)) {
+		printk("hcoutxirq_handler x %dqh->abort_pending is set status %d\n", x, urb->status);
+		goto done;
+	}
+	
+	DBG(4 , "hc out %d finished offset %d urb->actual_length %d\n",
+		x , qh->offset, urb->actual_length);
+	urb->actual_length = qh->offset;//offset contain actual bytes written?
+#ifdef HOST_DEBUG
+	if(urb->actual_length > urb->transfer_buffer_length	)
+		printk("!!actual_length %d if gr8 than transfer_buffer_length %d!!\n",
+		urb->actual_length, urb->transfer_buffer_length);
+#endif
+done:
+	if((urb->actual_length == urb->transfer_buffer_length) || qh->abort_pending)
+		advance_schedule(pilot_host, urb, qh->hw_ep, USB_DIR_OUT);
+	else
+		ep_write_fifo(pilot_host, urb, qh->hw_ep);
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+	return IRQ_HANDLED;
+}
+
+int hcout1_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcoutxirq_handler(pilot_host, 1, pilot_host->bulk_out[1]);
+#else
+	if(pilot_host->bulk_out[1] == NULL && pilot_host->intr_out[1] == NULL)
+		panic("OUTerr 1 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[1] && pilot_host->bulk_out[1])
+		panic("pilot_host->intr_out[1] && pilot_host->bulk_out[1] errset\n");
+	if(pilot_host->intr_out[1])
+		return hcoutxirq_handler(pilot_host, 1, pilot_host->intr_out[1]);
+	else
+		return hcoutxirq_handler(pilot_host, 1, pilot_host->bulk_out[1]);
+#endif
+}
+
+int hcout2_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcoutxirq_handler(pilot_host, 2, pilot_host->bulk_out[2]);
+#else
+	if(pilot_host->bulk_out[2] == NULL && pilot_host->intr_out[2] == NULL)
+		panic("OUTerr 2 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[2] && pilot_host->bulk_out[2])
+		panic("pilot_host->intr_out[2] && pilot_host->bulk_out[2] errset\n");
+	if(pilot_host->intr_out[2])
+		return hcoutxirq_handler(pilot_host, 2, pilot_host->intr_out[2]);
+	else
+		return hcoutxirq_handler(pilot_host, 2, pilot_host->bulk_out[2]);
+#endif
+}
+
+
+int hcout3_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcoutxirq_handler(pilot_host, 3, pilot_host->bulk_out[3]);
+#else
+	if(pilot_host->bulk_out[3] == NULL && pilot_host->intr_out[3] == NULL)
+		panic("OUTerr 3 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[3] && pilot_host->bulk_out[3])
+		panic("pilot_host->intr_out[3] && pilot_host->bulk_out[3] errset\n");
+	if(pilot_host->intr_out[3])
+		return hcoutxirq_handler(pilot_host, 3, pilot_host->intr_out[3]);
+	else
+		return hcoutxirq_handler(pilot_host, 3, pilot_host->bulk_out[3]);
+#endif
+}
+
+int hcout4_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcoutxirq_handler(pilot_host, 4, pilot_host->bulk_out[4]);
+#else
+	if(pilot_host->bulk_out[4] == NULL && pilot_host->intr_out[4] == NULL)
+		panic("OUTerr 4 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[4] && pilot_host->bulk_out[4])
+		panic("pilot_host->intr_out[4] && pilot_host->bulk_out[4] errset\n");
+	if(pilot_host->intr_out[4])
+		return hcoutxirq_handler(pilot_host, 4, pilot_host->intr_out[4]);
+	else
+		return hcoutxirq_handler(pilot_host, 4, pilot_host->bulk_out[4]);
+#endif
+}
+
+int hcout5_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcoutxirq_handler(pilot_host, 5, pilot_host->bulk_out[5]);
+#else
+	if(pilot_host->bulk_out[5] == NULL && pilot_host->intr_out[5] == NULL)
+		panic("OUTerr 5 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[5] && pilot_host->bulk_out[5])
+		panic("pilot_host->intr_out[5] && pilot_host->bulk_out[5] errset\n");
+	if(pilot_host->intr_out[5])
+		return hcoutxirq_handler(pilot_host, 5, pilot_host->intr_out[5]);
+	else
+		return hcoutxirq_handler(pilot_host, 5, pilot_host->bulk_out[5]);
+#endif
+}
+
+int hcout6_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifdef DYNAMIC_CONFIGURE_EP
+	if(pilot_host->bulk_out[6] == NULL && pilot_host->intr_out[6] == NULL)
+		panic("OUTerr 6 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[6] && pilot_host->bulk_out[6])
+		panic("pilot_host->intr_out[6] && pilot_host->bulk_out[6] errset\n");
+	if(pilot_host->intr_out[6])
+		return hcoutxirq_handler(pilot_host, 6, pilot_host->intr_out[6]);
+	else
+		return hcoutxirq_handler(pilot_host, 6, pilot_host->bulk_out[6]);
+#else
+#error "Please add a handler manually"
+#endif
+}
+
+int hcout7_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifdef DYNAMIC_CONFIGURE_EP
+	if(pilot_host->bulk_out[7] == NULL && pilot_host->intr_out[7] == NULL)
+		panic("OUTerr 7 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[7] && pilot_host->bulk_out[7])
+		panic("pilot_host->intr_out[7] && pilot_host->bulk_out[7] errset\n");
+	if(pilot_host->intr_out[7])
+		return hcoutxirq_handler(pilot_host, 7, pilot_host->intr_out[7]);
+	else
+		return hcoutxirq_handler(pilot_host, 7, pilot_host->bulk_out[7]);
+#else
+#error "Please add a handler manually"
+#endif
+}
+
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+extern struct pilot4_hw_ep *bulk_hw_ep;
+static int hc_ep0_out_bulk_handler(struct pilot_host_hcd *pilot_host, struct pilot4_hw_ep* bulk_hw_ep);
+int hcout8_bulk_ep0_handler(struct pilot_host_hcd *pilot_host)
+{
+        return hc_ep0_out_bulk_handler(pilot_host, bulk_hw_ep);
+}
+#endif
+
+static int hcoutxerrirq_handler(struct pilot_host_hcd *pilot_host, int x, struct pilot4_hw_ep *hw_ep)
+{
+#if 1
+	struct pilot4_qh *qh = NULL;
+	struct urb *urb = NULL;
+	int worthretry  = 0;
+	bool complete = false;
+	int status = -EPROTO;
+	unsigned long flags;
+	int reset_ret = 0;
+
+	*((volatile unsigned short*)(pilot_host->base_addr + hcout07errirq)) = (1 << x);//ack interrupt
+	spin_lock_irqsave(&pilot_host->lock, flags);
+	qh = hw_ep->cur_qh;
+#ifdef HOST_DEBUG
+	if(qh == NULL)
+ {
+		printk("qh==NULL enjoy! get out of the system hcouerr%d\n",x);
+	        spin_unlock_irqrestore(&pilot_host->lock, flags);
+        	return IRQ_HANDLED;
+	}
+
+#endif
+	urb = next_urb(qh);
+	if(urb == NULL) {
+		printk("%s urb is NULL hw_ep->cur_urb %p ep_num %d\n", __FUNCTION__, hw_ep->cur_urb, hw_ep->ep_num);
+		printk("hw_ep %p bulkin %p bulk out %p interin %p\n", hw_ep, pilot_host->bulk_in, pilot_host->bulk_out, pilot_host->intr_in);
+		printk("qh %p\n", qh);
+		printk("%s hw_ep free_recursion %d epnum %d %d\n", __FUNCTION__, qh->free_recursion, hw_ep->ep_num, qh->hw_ep->ep_num);
+
+		WARN(1, "urb ==NULL enjoy! get out of the system hcout%d\n",x);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return IRQ_HANDLED;
+	}
+	if(unlikely(qh->abort_pending)) {
+		printk("hcoutxerrirq_handler x %dqh->abort_pending is set status %d\n", x, urb->status);
+	}
+#ifdef HOST_DEBUG
+	if(hw_ep->cur_urb && hw_ep->cur_urb != urb) {
+		printk("%s  cur_urb %p urb %p\n", __FUNCTION__, hw_ep->cur_urb, urb);
+#ifdef DEBUG_START_FINISH
+		printk("%s error urb %p status %d jiffies %lu starttime %llu curwait_time %u\n", __FUNCTION__,
+			hw_ep->cur_urb, hw_ep->cur_urb->status, jiffies, hw_ep->start_time, hw_ep->wait_time);
+#endif
+		printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_qh);
+		printk("current urb actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			urb->actual_length, urb->transfer_buffer_length, urb, qh);
+	} else if(hw_ep->cur_urb == NULL && urb) {
+		printk("hcoutxerr act length %d transferbuflength %d urb %p cur_qh %p addr %x cururb %p  ep_num %d\n",
+			urb->actual_length, urb->transfer_buffer_length, urb, qh, qh->addr_reg, hw_ep->cur_urb, hw_ep->ep_num);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return IRQ_HANDLED;
+	}
+#endif
+	printk("hc out %d error offset %d urb->actual_length %d hcouterr 0x%x ep_num %d\n",
+		x , qh->offset, urb->actual_length, *((volatile u8*)(qh->hw_ep->hcouterr)), qh->hw_ep->ep_num);
+	
+	worthretry = check_hcout_error_details(*((volatile u8*)(qh->hw_ep->hcouterr)), qh->hw_ep->ep_num);
+	if(unlikely(qh->abort_pending)) {
+		worthretry = -ESHUTDOWN;
+	}
+	/* if we cannot retry should we reset endpoint , store toggle and restart from same point?
+	  * as of now not much clarity need to discuss further */
+	if(worthretry > 0) {
+		qh->retries++;
+		if(qh->retries < 100) {
+			if(pilot_host->speed == USB_SPEED_HIGH) {
+				//retry set resend bit and ping, should we set the ping ? or does HC takes care automatically???
+				*((volatile unsigned char*)(qh->hw_ep->hcouterr)) = ((1 << 5) | (1 << 6));
+			} else {
+				//No ping.
+				*((volatile unsigned char*)(qh->hw_ep->hcouterr)) = ((1 << 5));
+			}
+			status = 0;
+		} else {
+			*((volatile unsigned char*)(qh->hw_ep->hcouterr)) = 0;//no retry
+			reset_ret = endpoint_reset(qh->hw_ep, 1);
+			if(reset_ret) {
+				printk("%s interrupt status still set? hw_ep->cur_urb %p ep_num %d urb %p\n", __FUNCTION__, hw_ep->cur_urb, hw_ep->ep_num, urb);
+			}
+			status = -ETIME;
+		}
+	} else {
+			*((volatile unsigned char*)(qh->hw_ep->hcouterr)) = 0;//no retry
+			reset_ret = endpoint_reset(qh->hw_ep, 1);
+			if(reset_ret) {
+				printk("%s interrupt status2 still set? hw_ep->cur_urb %p ep_num %d urb %p\n", __FUNCTION__, hw_ep->cur_urb, hw_ep->ep_num, urb);
+			}
+			//Now the endpoint is halted and we cannot reset EP0 
+			//need to check what will be done.
+			status = worthretry;
+	}
+	if(status){
+		complete = true;
+		if (urb)
+			urb->status = status;
+	}
+	
+	if(complete)
+		advance_schedule(pilot_host, urb, qh->hw_ep, USB_DIR_OUT);
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+	
+	return IRQ_HANDLED;	
+#endif
+}
+#ifdef SW_WORKAROUND
+struct timer_list intr_ep_delay;
+static void intr_nak_retry(unsigned long param)
+{
+	struct pilot4_qh *qh = (struct pilot4_qh *)param;
+	unsigned long flags;
+	if(qh == NULL) {
+		panic("intr_start qh == NULL!!\n");
+		while(1);
+	}
+	if(qh->hw_ep == NULL)
+		printk("qh->hw_ep == NULL\n");
+	spin_lock_irqsave(&qh->hw_ep->pilot_host->lock, flags);
+	qh->hw_ep->timer_armed = 0;	
+	qh->programmed = 1;
+	*((volatile unsigned char*)(qh->hw_ep->hcinerr)) = ((1 << 5));
+	spin_unlock_irqrestore(&qh->hw_ep->pilot_host->lock, flags);
+ 	return;
+}
+struct timer_list intr_ep_start;
+static void intr_start(unsigned long param)
+{
+	struct pilot4_qh *qh = (struct pilot4_qh *)param;
+	unsigned long flags;
+	if(qh == NULL) {
+		panic("intr_start qh == NULL!!\n");
+		while(1);
+	}
+	if(qh->hw_ep == NULL)
+		printk("qh->hw_ep == NULL\n");
+	spin_lock_irqsave(&qh->hw_ep->pilot_host->lock, flags);
+	qh->hw_ep->timer_armed = 0;	
+	qh->programmed = 1;
+	*((volatile unsigned char*)(qh->hw_ep->ep_cs_reg)) = 0;//Arm Intr in EP?
+	spin_unlock_irqrestore(&qh->hw_ep->pilot_host->lock, flags);
+	return;
+}
+
+#endif
+static int hcinxerrirq_handler(struct pilot_host_hcd *pilot_host, int x, struct pilot4_hw_ep *hw_ep)
+{
+	struct pilot4_qh *qh = NULL, *temp_qh= NULL;
+	struct urb *urb = NULL;
+	int status = -EPROTO;
+	unsigned long flags;
+	int worthretry  = 0;
+	u8 addr = 0, total_qh = 0;
+	int reset_ret = 0;
+	*((volatile unsigned short*)(pilot_host->base_addr + hcin07errirq)) = (1 << x);//ack interrupt
+	spin_lock_irqsave(&pilot_host->lock, flags);
+	qh = hw_ep->cur_qh;
+
+#ifdef HOST_DEBUG
+	if(qh == NULL || ((unsigned int)qh == 0xDEADBEEF)) {
+		(void)check_hcin_error_details(*((volatile u8*)(hw_ep->hcinerr)), x);
+		printk("qh== %p enjoy! get out of the system err hcin%d\n",qh, x);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return -1;
+	}
+#endif
+	smp_rmb();
+	urb = next_urb(qh);
+	if(urb == NULL) {
+		printk("%s urb is NULL hw_ep->cur_urb %p ep_num %d\n", __FUNCTION__, hw_ep->cur_urb, hw_ep->ep_num);
+		printk("hw_ep %p bulkin %p bulk out %p interin %p\n", hw_ep, pilot_host->bulk_in, pilot_host->bulk_out, pilot_host->intr_in);
+		printk("qh %p\n", qh);
+		printk("%s hw_ep free_recursion %d epnum %d %d\n", __FUNCTION__, qh->free_recursion, hw_ep->ep_num, qh->hw_ep->ep_num);
+		WARN(1, "urb ==NULL enjoy! get out of the system hcin%d\n",x);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return IRQ_HANDLED;
+	}
+
+#ifdef HOST_DEBUG
+
+	if(hw_ep->cur_urb && hw_ep->cur_urb != urb) {
+		printk("%s cur_urb %p ep_num %d urb %p bmAttributes %d\n", __FUNCTION__, hw_ep->cur_urb, hw_ep->ep_num, urb, hw_ep->bmAttributes);
+#ifdef DEBUG_START_FINISH
+		printk("%s error urb %p status %d jiffies %lu starttime %llu curwait_time %u\n", __FUNCTION__,
+			hw_ep->cur_urb, hw_ep->cur_urb->status, jiffies, hw_ep->start_time, hw_ep->wait_time);
+#endif
+		printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_qh);
+		printk("current urb actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			urb->actual_length, urb->transfer_buffer_length, urb, qh);
+	} else if(hw_ep->cur_urb == NULL && urb) {
+		printk("hcinxerr act length %d transferbuflength %d urb %p cur_qh %p addr %x cururb %p ep_num %d bmAttributes %d\n",
+			urb->actual_length, urb->transfer_buffer_length, urb, qh, qh->addr_reg, hw_ep->cur_urb, hw_ep->ep_num, hw_ep->bmAttributes);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return IRQ_HANDLED;
+	}
+#endif
+	if(unlikely(qh->abort_pending)) {
+		printk("hcinxerrirq_handler x %dqh->abort_pending is set status %d\n", x, urb->status);
+		status = -ESHUTDOWN;
+	}
+	addr = qh->addr_reg;
+	DBG(3, "hcin %d error offset %d urb->actual_length %d hcouterr 0x%x\n",
+		x , qh->offset, urb->actual_length, *((volatile u8*)(qh->hw_ep->hcinerr)));
+	DBG(3, "hcin %d error offset %d urb->actual_length %d hcouterr 0x%x addr %x func_addr_reg %x\n",
+		x , qh->offset, urb->actual_length, *((volatile u8*)(qh->hw_ep->hcinerr)), addr, *((volatile unsigned char*)hw_ep->func_addr_reg));
+	DBG(3, "Not retrying on HCINERR hw_ep %p name %s epnum %d \n", hw_ep, hw_ep->name, qh->hw_ep->ep_num);
+	//printk("pilot_host->bulk_in %p epnum %d hw_ep %p epnum %d\n", qh->hw_ep, qh->hw_ep->ep_num,hw_ep,hw_ep->ep_num);
+	worthretry = check_hcin_error_details(*((volatile u8*)(qh->hw_ep->hcinerr)), x);
+	if(worthretry == -ECONNRESET) {
+		print_giveback = 1;
+		no_endp_handshake++;
+		if(pilot_host->debug_level & ENABLE_CLEANUP_PRINTS_DURING_ERROR)
+			pilot_host->debug_level = DETAILED_CLEANUP_PRINT | CLEANUP_PRINT;
+	}
+	if(worthretry > 0) {
+no_other_qh_pending:
+		qh->retries++;
+		if(0||qh->retries < 4) {
+			//retry set resend bit and ping, should we set the ping ? or does HC takes care automatically???
+			if(qh->type != USB_ENDPOINT_XFER_INT)
+				printk("NAK? for endpoin type %d\n", qh->type);
+#ifdef SW_WORKAROUND
+			qh->hw_ep->timer_armed = 1;
+			qh->hw_ep->intr_ep_timer.function = intr_nak_retry;
+			qh->hw_ep->intr_ep_timer.data = (unsigned long)qh;
+			mod_timer(&qh->hw_ep->intr_ep_timer, jiffies + msecs_to_jiffies(200));
+#else
+			*((volatile unsigned char*)(qh->hw_ep->hcinerr)) = ((1 << 5));
+#endif
+			status = 0;
+			spin_unlock_irqrestore(&pilot_host->lock, flags);
+			return IRQ_HANDLED;
+		}
+		else {
+			status = -ETIME;
+			if(hw_ep->bmAttributes == USB_ENDPOINT_XFER_INT) {
+				status = 0;
+				qh->retries = 0;
+				list_for_each_entry (temp_qh, &hw_ep->queue, ring) {
+					total_qh++;
+					if(start_intr_print) {
+						printk("qh %p qh->type %x qh->addr %x qh->epnum %d next_urb %p hw_ep.ep_num %d\n",
+							temp_qh, temp_qh->type,temp_qh->addr_reg, temp_qh->epnum, next_urb(temp_qh), hw_ep->ep_num);
+						printk("bEndpointAddress %d addr_reg %x\n", temp_qh->hep->desc.bEndpointAddress, temp_qh->addr_reg);
+					}
+				}
+				if(total_qh == 1) {
+					goto no_other_qh_pending;
+				}
+			}
+#ifndef HW_BUG
+			*((volatile unsigned char*)(qh->hw_ep->hcinerr)) = 0;//no retry
+			reset_ret = endpoint_reset(qh->hw_ep, 0);
+			if(reset_ret) {
+				printk("%s interrupt status still set? hw_ep->cur_urb %p ep_num %d urb %p\n", __FUNCTION__, hw_ep->cur_urb, hw_ep->ep_num, urb);
+			}
+#endif
+		}
+	} else {
+			*((volatile unsigned char*)(qh->hw_ep->hcinerr)) = 0;//no retry
+#ifndef HW_BUG
+			reset_ret = endpoint_reset(qh->hw_ep, 0);
+			if(reset_ret) {
+				printk("%s interrupt status2 still set? hw_ep->cur_urb %p ep_num %d urb %p\n", __FUNCTION__, hw_ep->cur_urb, hw_ep->ep_num, urb);
+			}
+#endif
+			//Now the endpoint is halted and we cannot reset EP0 
+			//need to check what will be done.
+			status = worthretry;
+	}
+
+ 	if(status != 0) {
+		urb->status = status;
+		//print_giveback = 1;
+		advance_schedule(pilot_host, urb, qh->hw_ep, USB_DIR_IN);
+	} 
+#if 1//ndef HW_BUG
+	else {
+		if((1 || start_intr_print )&& hw_ep->bmAttributes == USB_ENDPOINT_XFER_INT)
+			printk("total_qh %d qh->hw_ep->timer_armed %d\n", total_qh, qh->hw_ep->timer_armed);
+		list_move_tail(&qh->ring, &hw_ep->queue);
+		//If its a NAK we will simply retry later until there is a higher level timeout and we get a dequeue.
+		qh = pick_next_transaction(pilot_host,qh->type, USB_DIR_IN, hw_ep);
+
+		if (qh && !list_empty(&qh->hep->urb_list)) {
+			if((1 || start_intr_print) && hw_ep->bmAttributes == USB_ENDPOINT_XFER_INT) {
+				printk("Advancing intr in  qh error addr %x new adr %x\n", addr, qh->addr_reg);
+				printk("error moving to next addr %x qh type %x addr %x\n",addr, qh->type, qh->addr_reg);
+			}
+			pilot4_start_urb(pilot_host, USB_DIR_IN, qh);
+		}else {
+			if(1 && hw_ep->bmAttributes == USB_ENDPOINT_XFER_INT)
+				printk("line %d qh == nULL\n", __LINE__);
+		}
+	}
+#endif
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+
+	return IRQ_HANDLED;	
+}
+int hcout1err_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcoutxerrirq_handler(pilot_host, 1, pilot_host->bulk_out[1]);
+#else
+	if(pilot_host->bulk_out[1] == NULL && pilot_host->intr_out[1] == NULL)
+		panic("OUTerr 1 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[1] && pilot_host->bulk_out[1])
+		panic("pilot_host->intr_out[1] && pilot_host->bulk_out[1] errset\n");
+	if(pilot_host->intr_out[1])
+		return hcoutxerrirq_handler(pilot_host, 1, pilot_host->intr_out[1]);
+	else
+		return hcoutxerrirq_handler(pilot_host, 1, pilot_host->bulk_out[1]);
+#endif
+}
+
+int hcout2err_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcoutxerrirq_handler(pilot_host, 2, pilot_host->bulk_out[2]);
+#else
+	if(pilot_host->bulk_out[2] == NULL && pilot_host->intr_out[2] == NULL)
+		panic("OUTerr 2 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[2] && pilot_host->bulk_out[2])
+		panic("pilot_host->intr_out[2] && pilot_host->bulk_out[2] errset\n");
+	if(pilot_host->intr_out[2])
+		return hcoutxerrirq_handler(pilot_host, 2, pilot_host->intr_out[2]);
+	else
+		return hcoutxerrirq_handler(pilot_host, 2, pilot_host->bulk_out[2]);
+#endif
+}
+
+int hcout3err_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcoutxerrirq_handler(pilot_host, 3, pilot_host->bulk_out[3]);
+#else
+	if(pilot_host->bulk_out[3] == NULL && pilot_host->intr_out[3] == NULL)
+		panic("OUTerr 3 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[3] && pilot_host->bulk_out[3])
+		panic("pilot_host->intr_out[3] && pilot_host->bulk_out[3] errset\n");
+	if(pilot_host->intr_out[3])
+		return hcoutxerrirq_handler(pilot_host, 3, pilot_host->intr_out[3]);
+	else
+		return hcoutxerrirq_handler(pilot_host, 3, pilot_host->bulk_out[3]);
+#endif
+}
+
+int hcout4err_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcoutxerrirq_handler(pilot_host, 4, pilot_host->bulk_out[4]);
+#else
+	if(pilot_host->bulk_out[4] == NULL && pilot_host->intr_out[4] == NULL)
+		panic("OUTerr 4 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[4] && pilot_host->bulk_out[4])
+		panic("pilot_host->intr_out[4] && pilot_host->bulk_out[4] errset\n");
+	if(pilot_host->intr_out[4])
+		return hcoutxerrirq_handler(pilot_host, 4, pilot_host->intr_out[4]);
+	else
+		return hcoutxerrirq_handler(pilot_host, 4, pilot_host->bulk_out[4]);
+#endif
+}
+
+int hcout5err_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcoutxerrirq_handler(pilot_host, 5, pilot_host->bulk_out[5]);
+#else
+	if(pilot_host->bulk_out[5] == NULL && pilot_host->intr_out[5] == NULL)
+		panic("OUTerr 5 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[5] && pilot_host->bulk_out[5])
+		panic("pilot_host->intr_out[5] && pilot_host->bulk_out[5] errset\n");
+	if(pilot_host->intr_out[5])
+		return hcoutxerrirq_handler(pilot_host, 5, pilot_host->intr_out[5]);
+	else
+		return hcoutxerrirq_handler(pilot_host, 5, pilot_host->bulk_out[5]);
+#endif
+}
+
+int hcout6err_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifdef DYNAMIC_CONFIGURE_EP
+	if(pilot_host->bulk_out[6] == NULL && pilot_host->intr_out[6] == NULL)
+		panic("OUTerr 6 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[6] && pilot_host->bulk_out[6])
+		panic("pilot_host->intr_out[6] && pilot_host->bulk_out[6] errset\n");
+	if(pilot_host->intr_out[6])
+		return hcoutxerrirq_handler(pilot_host, 6, pilot_host->intr_out[6]);
+	else
+		return hcoutxerrirq_handler(pilot_host, 6, pilot_host->bulk_out[6]);
+#else
+#error "Please add handler manually"
+#endif
+}
+
+int hcout7err_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifdef DYNAMIC_CONFIGURE_EP
+	if(pilot_host->bulk_out[7] == NULL && pilot_host->intr_out[7] == NULL)
+		panic("OUTerr 7 both ptrs NULL\n");
+	
+	if(pilot_host->intr_out[7] && pilot_host->bulk_out[7])
+		panic("pilot_host->intr_out[7] && pilot_host->bulk_out[7] errset\n");
+	if(pilot_host->intr_out[7])
+		return hcoutxerrirq_handler(pilot_host, 7, pilot_host->intr_out[7]);
+	else
+		return hcoutxerrirq_handler(pilot_host, 7, pilot_host->bulk_out[7]);
+#else
+#error "Please add handler manually"
+#endif
+}
+
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+int hcout8err_bulk_ep0_handler(struct pilot_host_hcd *pilot_host)
+{
+        *((volatile unsigned short*)(pilot_host->base_addr + hcout07errirq)) = (1 << bulk_hw_ep->ep_num);
+        printk("hcout8err_bulk_ep0_handler Not implemented\n");
+        return -1;
+}
+#endif
+
+static int hcinxirq_handler(struct pilot_host_hcd *pilot_host, int x, struct pilot4_hw_ep *hw_ep)
+{
+	struct pilot4_qh *qh = NULL;
+	struct urb *urb = NULL;
+	int ret = -1;
+	unsigned long flags;
+
+	*((volatile unsigned short*)(pilot_host->base_addr + hcin07irq)) = (1 << x);//ack interrupt
+	spin_lock_irqsave(&pilot_host->lock, flags);
+	qh = hw_ep->cur_qh;
+#ifdef HOST_DEBUG
+	if(qh == NULL || ((unsigned int)qh == 0xDEADBEEF)) {
+		printk("qh==%p enjoy! get out of the system hcin%d ep_num %d\n",qh, x, hw_ep->ep_num);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return -1;
+	}
+#endif
+	hw_ep = qh->hw_ep;
+	smp_rmb();
+	urb = next_urb(qh);
+#ifdef HOST_DEBUG
+	if (unlikely(urb == NULL)) {
+		printk("hw_ep %p bulkin %p bulk out %p interin %p\n", hw_ep, pilot_host->bulk_in, pilot_host->bulk_out, pilot_host->intr_in);
+		printk("qh %p hw_ep->cur_urb %p\n", qh, hw_ep->cur_urb);
+		printk("%s hw_ep free_recursion %d epnum %d %d\n", __FUNCTION__, qh->free_recursion, hw_ep->ep_num, qh->hw_ep->ep_num);
+		WARN(1, "urb ==NULL enjoy! get out of the system hcin%d\n",x);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return -1;
+	}
+	if (unlikely(hw_ep == NULL))
+		panic("hw_ep == NULL!!\n");
+	if(hw_ep->cur_urb && hw_ep->cur_urb != urb) {
+		printk("%s cur_urb %p ep_num %d urb %p bmAttributes %d\n", __FUNCTION__,
+			hw_ep->cur_urb, hw_ep->ep_num, urb, hw_ep->bmAttributes);
+#ifdef DEBUG_START_FINISH
+		printk("%s error urb %p status %d jiffies %lu starttime %llu curwait_time %u\n", __FUNCTION__,
+			hw_ep->cur_urb, hw_ep->cur_urb->status, jiffies, hw_ep->start_time, hw_ep->wait_time);
+#endif
+		printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_qh);
+		printk("current urb actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			urb->actual_length, urb->transfer_buffer_length, urb, qh);
+	} else if(hw_ep->cur_urb == NULL) {
+		printk("hcinxirq act length %d transferbuflength %d urb %p cur_qh %p addr %x cururb %p ep_num %d bmAttributes %d\n",
+			urb->actual_length, urb->transfer_buffer_length, urb, qh, qh->addr_reg, hw_ep->cur_urb, hw_ep->ep_num, hw_ep->bmAttributes);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return IRQ_HANDLED;
+}
+#endif
+	if(unlikely(qh->abort_pending)) {
+		printk("hcinxirq_handler x %dqh->abort_pending is set status %d\n", x, urb->status);
+		goto done;
+	}
+//	if(qh->type != USB_ENDPOINT_XFER_INT)
+		DBG(4, "qh->offset %d actual_length %d transfer_buffer_length %d\n", qh->offset, urb->actual_length, urb->transfer_buffer_length);
+	DBG(4, "hcin:qh->offset %d actual_length %d transfer_buffer_length %d addr_reg %x func_addr_reg %x\n", qh->offset, urb->actual_length, urb->transfer_buffer_length, qh->addr_reg, *((volatile unsigned char*)hw_ep->func_addr_reg));
+	ret = ep_read_fifo(pilot_host, urb, hw_ep);
+	urb->actual_length = qh->offset;
+#ifdef HOST_DEBUG
+	if(urb->actual_length > urb->transfer_buffer_length || (ret < 0)) {
+               printk("read urb->actual_length %d > urb->transfer_buffer_length %d!!\n",
+                       urb->actual_length, urb->transfer_buffer_length);
+	}
+#endif
+	if(ret < 0) {
+		urb->status = ret;
+	}
+done:
+	if((urb->actual_length >= urb->transfer_buffer_length) || (ret == 1) || qh->abort_pending) {
+		advance_schedule(pilot_host, urb, qh->hw_ep, USB_DIR_IN);
+	}
+	else {
+		*((volatile unsigned char*)(hw_ep->ep_cs_reg)) = 0;//Arm Bulk in EP?
+	}
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+	return IRQ_HANDLED;
+}
+//Interrupt In
+int hcin1err_handler(struct pilot_host_hcd *pilot_host)
+{
+	if(pilot_host->bulk_in[1])
+		DBG(3, "Err pilot_host->bulk_in %p epnum %d\n", pilot_host->bulk_in[1],pilot_host->bulk_in[1]->ep_num);
+	else
+		DBG(3, "Err pilot_host->intr_in %p epnum %d\n", pilot_host->intr_in[1],pilot_host->intr_in[1]->ep_num);
+
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxerrirq_handler(pilot_host, 1, pilot_host->intr_in[1]);
+#else
+	if(pilot_host->intr_in[1] == NULL && pilot_host->bulk_in[1] == NULL)
+		panic("INerr 1 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[1] && pilot_host->bulk_in[1])
+		panic("pilot_host->intr_in[1] && pilot_host->bulk_in[1] errset\n");
+	if(pilot_host->intr_in[1])
+		return hcinxerrirq_handler(pilot_host, 1, pilot_host->intr_in[1]);
+	else
+		return hcinxerrirq_handler(pilot_host, 1, pilot_host->bulk_in[1]);
+#endif
+}
+
+//Bulk In
+int hcin2err_handler(struct pilot_host_hcd *pilot_host)
+{
+	if(pilot_host->bulk_in[2])
+		DBG(3, "Err pilot_host->bulk_in %p epnum %d\n", pilot_host->bulk_in[2],pilot_host->bulk_in[2]->ep_num);
+	else
+		DBG(3, "Err pilot_host->intr_in %p epnum %d\n", pilot_host->intr_in[2],pilot_host->intr_in[2]->ep_num);
+
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxerrirq_handler(pilot_host, 2, pilot_host->bulk_in[2]);
+#else
+	if(pilot_host->intr_in[2] == NULL && pilot_host->bulk_in[2] == NULL)
+		panic("INerr 2 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[2] && pilot_host->bulk_in[2])
+		panic("pilot_host->intr_in[2] && pilot_host->bulk_in[2] errset\n");
+	if(pilot_host->intr_in[2])
+		return hcinxerrirq_handler(pilot_host, 2, pilot_host->intr_in[2]);
+	else
+		return hcinxerrirq_handler(pilot_host, 2, pilot_host->bulk_in[2]);
+#endif
+}
+
+//Bulk In
+int hcin3err_handler(struct pilot_host_hcd *pilot_host)
+{
+	if(pilot_host->bulk_in[3])
+		DBG(3, "Err pilot_host->bulk_in %p epnum %d\n", pilot_host->bulk_in[3],pilot_host->bulk_in[3]->ep_num);
+	else
+		DBG(3, "Err pilot_host->intr_in %p epnum %d\n", pilot_host->intr_in[3],pilot_host->intr_in[3]->ep_num);
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxerrirq_handler(pilot_host, 3, pilot_host->bulk_in[3]);
+#else
+	if(pilot_host->intr_in[3] == NULL && pilot_host->bulk_in[3] == NULL)
+		panic("INerr 3 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[3] && pilot_host->bulk_in[3])
+		panic("pilot_host->intr_in[3] && pilot_host->bulk_in[3] errset\n");
+	if(pilot_host->intr_in[3])
+		return hcinxerrirq_handler(pilot_host, 3, pilot_host->intr_in[3]);
+	else
+		return hcinxerrirq_handler(pilot_host, 3, pilot_host->bulk_in[3]);
+#endif
+
+}
+
+//Interrupt In
+int hcin4err_handler(struct pilot_host_hcd *pilot_host)
+{
+	if(pilot_host->intr_in[4])
+		DBG(3, "Err pilot_host->intr_in %p epnum %d\n", pilot_host->intr_in[4],pilot_host->intr_in[4]->ep_num);
+	else
+		DBG(3, "Err pilot_host->bulk_in %p epnum %d\n", pilot_host->bulk_in[4],pilot_host->bulk_in[4]->ep_num);
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxerrirq_handler(pilot_host, 4, pilot_host->intr_in[4]);
+#else
+	if(pilot_host->intr_in[4] == NULL && pilot_host->bulk_in[4] == NULL)
+		panic("INerr 4 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[4] && pilot_host->bulk_in[4])
+		panic("pilot_host->intr_in[4] && pilot_host->bulk_in[4] errset\n");
+	if(pilot_host->intr_in[4])
+		return hcinxerrirq_handler(pilot_host, 4, pilot_host->intr_in[4]);
+	else
+		return hcinxerrirq_handler(pilot_host, 4, pilot_host->bulk_in[4]);
+#endif
+}
+
+//Bulk In Hardcoded
+int hcin5err_handler(struct pilot_host_hcd *pilot_host)
+{
+	if(pilot_host->bulk_in[5])
+		DBG(3, "Err pilot_host->bulk_in %p epnum %d\n", pilot_host->bulk_in[5],pilot_host->bulk_in[5]->ep_num);
+	else
+		DBG(3, "Err pilot_host->intr_in %p epnum %d\n", pilot_host->intr_in[5],pilot_host->intr_in[5]->ep_num);
+
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxerrirq_handler(pilot_host, 5, pilot_host->bulk_in[5]);
+#else
+	if(pilot_host->intr_in[5] == NULL && pilot_host->bulk_in[5] == NULL)
+		panic("INerr 5 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[5] && pilot_host->bulk_in[5])
+		panic("pilot_host->intr_in[5] && pilot_host->bulk_in[5] errset\n");
+	if(pilot_host->intr_in[5])
+		return hcinxerrirq_handler(pilot_host, 5, pilot_host->intr_in[5]);
+	else
+		return hcinxerrirq_handler(pilot_host, 5, pilot_host->bulk_in[5]);
+#endif
+}
+
+//Bulk In Hardcoded
+int hcin6err_handler(struct pilot_host_hcd *pilot_host)
+{
+	if(pilot_host->bulk_in[6])
+		DBG(3, "Err pilot_host->bulk_in %p epnum %d\n", pilot_host->bulk_in[6],pilot_host->bulk_in[6]->ep_num);
+	else
+		DBG(3, "Err pilot_host->intr_in %p epnum %d\n", pilot_host->intr_in[6],pilot_host->intr_in[6]->ep_num);
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxerrirq_handler(pilot_host, 6, pilot_host->bulk_in[6]);
+#else
+	if(pilot_host->intr_in[6] == NULL && pilot_host->bulk_in[6] == NULL)
+		panic("INerr 6 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[6] && pilot_host->bulk_in[6])
+		panic("pilot_host->intr_in[6] && pilot_host->bulk_in[6] errset\n");
+	if(pilot_host->intr_in[6])
+		return hcinxerrirq_handler(pilot_host, 6, pilot_host->intr_in[6]);
+	else
+		return hcinxerrirq_handler(pilot_host, 6, pilot_host->bulk_in[6]);
+#endif
+
+}
+
+//Bulk In Hardcoded
+int hcin7err_handler(struct pilot_host_hcd *pilot_host)
+{
+	if(pilot_host->intr_in[7])
+		DBG(3, "Err pilot_host->intr_in %p epnum %d\n", pilot_host->intr_in[7],pilot_host->intr_in[7]->ep_num);
+	else
+		DBG(3, "Err pilot_host->bulk_in %p epnum %d\n", pilot_host->bulk_in[7],pilot_host->bulk_in[7]->ep_num);
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxerrirq_handler(pilot_host, 7, pilot_host->intr_in[7]);
+#else
+	if(pilot_host->intr_in[7] == NULL && pilot_host->bulk_in[7] == NULL)
+		panic("INerr 7 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[7] && pilot_host->bulk_in[7])
+		panic("pilot_host->intr_in[7] && pilot_host->bulk_in[7] errset\n");
+	if(pilot_host->intr_in[7])
+		return hcinxerrirq_handler(pilot_host, 7, pilot_host->intr_in[7]);
+	else
+		return hcinxerrirq_handler(pilot_host, 7, pilot_host->bulk_in[7]);
+#endif
+}
+
+//Interrupt In
+int hcin1_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxirq_handler(pilot_host, 1, pilot_host->intr_in[1]);
+#else
+	if(pilot_host->intr_in[1] == NULL && pilot_host->bulk_in[1] == NULL)
+		panic("IN 1 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[1] && pilot_host->bulk_in[1])
+		panic("pilot_host->intr_in[1] && pilot_host->bulk_in[1] set\n");
+	if(pilot_host->intr_in[1])
+		return hcinxirq_handler(pilot_host, 1, pilot_host->intr_in[1]);
+	else
+		return hcinxirq_handler(pilot_host, 1, pilot_host->bulk_in[1]);
+#endif
+
+}
+
+//Bulk In
+int hcin2_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxirq_handler(pilot_host, 2, pilot_host->bulk_in[2]);
+#else
+	if(pilot_host->intr_in[2] == NULL && pilot_host->bulk_in[2] == NULL)
+		panic("IN 2 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[2] && pilot_host->bulk_in[2])
+		panic("pilot_host->intr_in[2] && pilot_host->bulk_in[2] set\n");
+	if(pilot_host->intr_in[2])
+		return hcinxirq_handler(pilot_host, 2, pilot_host->intr_in[2]);
+	else
+		return hcinxirq_handler(pilot_host, 2, pilot_host->bulk_in[2]);
+#endif
+
+}
+
+//Bulk In
+int hcin3_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxirq_handler(pilot_host, 3, pilot_host->bulk_in[3]);
+#else
+	if(pilot_host->intr_in[3] == NULL && pilot_host->bulk_in[3] == NULL)
+		panic("IN 3 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[3] && pilot_host->bulk_in[3])
+		panic("pilot_host->intr_in[3] && pilot_host->bulk_in[3] set\n");
+	if(pilot_host->intr_in[3])
+		return hcinxirq_handler(pilot_host, 3, pilot_host->intr_in[3]);
+	else
+		return hcinxirq_handler(pilot_host, 3, pilot_host->bulk_in[3]);
+#endif
+}
+
+//Interrupt In
+int hcin4_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxirq_handler(pilot_host, 4, pilot_host->intr_in[4]);
+#else
+	if(pilot_host->intr_in[4] == NULL && pilot_host->bulk_in[4] == NULL)
+		panic("IN 4 both ptrs NULL\n");
+	
+	if(pilot_host->intr_in[4] && pilot_host->bulk_in[4])
+		panic("pilot_host->intr_in[4] && pilot_host->bulk_in[4] set\n");
+	if(pilot_host->intr_in[4])
+		return hcinxirq_handler(pilot_host, 4, pilot_host->intr_in[4]);
+	else
+		return hcinxirq_handler(pilot_host, 4, pilot_host->bulk_in[4]);
+#endif
+}
+
+//Bulk In
+int hcin5_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxirq_handler(pilot_host, 5, pilot_host->bulk_in[5]);
+#else
+	if(pilot_host->intr_in[5] == NULL && pilot_host->bulk_in[5] == NULL)
+		panic("IN 5 both ptrs NULL\n");
+
+	if(pilot_host->intr_in[5] && pilot_host->bulk_in[5])
+		panic("pilot_host->intr_in[5] && pilot_host->bulk_in[5] set\n");
+	if(pilot_host->intr_in[5])
+		return hcinxirq_handler(pilot_host, 5, pilot_host->intr_in[5]);
+	else
+		return hcinxirq_handler(pilot_host, 5, pilot_host->bulk_in[5]);
+#endif
+}
+
+//Bulk In
+int hcin6_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxirq_handler(pilot_host, 6, pilot_host->bulk_in[6]);
+#else
+	if(pilot_host->intr_in[6] == NULL && pilot_host->bulk_in[6] == NULL)
+		panic("IN 6 both ptrs NULL\n");
+
+	if(pilot_host->intr_in[6] && pilot_host->bulk_in[6])
+		panic("pilot_host->intr_in[6] && pilot_host->bulk_in[6] set\n");
+	if(pilot_host->intr_in[6])
+		return hcinxirq_handler(pilot_host, 6, pilot_host->intr_in[6]);
+	else
+		return hcinxirq_handler(pilot_host, 6, pilot_host->bulk_in[6]);
+#endif
+}
+
+//Bulk In
+int hcin7_handler(struct pilot_host_hcd *pilot_host)
+{
+#ifndef DYNAMIC_CONFIGURE_EP
+	return hcinxirq_handler(pilot_host, 7, pilot_host->intr_in[7]);
+#else
+	if(pilot_host->intr_in[7] == NULL && pilot_host->bulk_in[7] == NULL)
+		panic("IN 7 both ptrs NULL\n");
+
+	if(pilot_host->intr_in[7] && pilot_host->bulk_in[7])
+		panic("pilot_host->intr_in[7] && pilot_host->bulk_in[7] set\n");
+	if(pilot_host->intr_in[7])
+		return hcinxirq_handler(pilot_host, 7, pilot_host->intr_in[7]);
+	else
+		return hcinxirq_handler(pilot_host, 7, pilot_host->bulk_in[7]);
+#endif
+}
+
+//start_bulk_in and start_intr_in can be merged into 1 will do later.
+static void start_intr_in(struct pilot4_hw_ep *hw_ep, struct pilot_host_hcd *pilot_host, 
+	struct urb *urb, u8 *buf, u32 len, unsigned int is_out)
+{
+	struct pilot4_qh *qh = NULL;
+	u8 ctrl_reg;
+	qh = hw_ep->cur_qh;
+#ifdef HOST_DEBUG
+	in_urb = urb;
+	if(qh == NULL)
+		panic("start_intr_in qh == NULL getout \n"); 
+#endif
+	ctrl_reg = *((volatile unsigned char*)(hw_ep->hc_in_con));
+	if((ctrl_reg & 0xCF) != ((USB_ENDPOINT_XFER_INT << 2) | (1 <<7))) {
+		printk("%s unexpected ctrlreg %x %p ep_num %d\n",
+			__FUNCTION__, ctrl_reg, ((volatile unsigned char*)(hw_ep->hc_in_con)), hw_ep->ep_num);
+	}
+	DBG(4,"intr in host_ep3_in_data_valid %d ep_cs_reg %x\n",host_ep3_in_data_valid,
+		*((volatile unsigned char*)(hw_ep->ep_cs_reg)));
+
+	//Reset just for safety?
+	*((volatile unsigned char*)(hw_ep->hcinerr)) = 0;
+        *((volatile unsigned char*)hw_ep->func_addr_reg) = qh->addr_reg & 0x7F;
+	smp_rmb();
+	if(hw_ep->cur_urb) {
+		printk("Previous urb still not finished actual length %d transferbuflength %d urb %p status %d\n",
+			hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_urb->status);
+		printk("start_bulk_in epnum %d busy bit set i ask y ??? epcs 0x%x addr %x programmed %d\n",
+			hw_ep->ep_num, *((volatile unsigned char*)(hw_ep->ep_cs_reg)), qh->addr_reg, qh->programmed);
+		printk("qh %p cur_qh %p empty %d? urb %p\n", qh, hw_ep->cur_qh, list_empty(&qh->hep->urb_list), urb);
+#ifdef DEBUG_START_FINISH
+		printk("hw_ep->cur_cpu %d curcpu %d last_complete_time %llu jiffies %lu\n",
+			hw_ep->cur_cpu, raw_smp_processor_id(), hw_ep->last_complete_time, jiffies);
+#endif
+		dump_stack();
+	}
+	hw_ep->cur_urb = urb;
+	smp_wmb();
+#ifdef DEBUG_START_FINISH
+	hw_ep->cur_cpu = raw_smp_processor_id();
+	hw_ep->start_time = jiffies;
+#ifdef CONFIG_SMP
+	if(!spin_is_locked(&pilot_host->lock)) {
+		printk("Spin not locked?\n");
+		dump_stack();
+	}
+#endif
+#endif
+#ifdef HOST_DEBUG_STATS
+//#ifndef DEBUG_START_FINISH
+	hw_ep->start_time = jiffies;
+//#endif
+	hw_ep->total_urbs++;
+	hw_ep->total_transfer_buffer_length += urb->transfer_buffer_length;
+	if(hw_ep->dev_addr != (qh->addr_reg & 0x7F)) {
+		printk("Address Issue? %s epnum %d addr %d hw_ep->dev_addr %d\n", __FUNCTION__, hw_ep->ep_num, qh->addr_reg,hw_ep->dev_addr);
+	}
+#endif
+	if (usb_gettoggle(urb->dev, qh->epnum, 0))
+		set_toggle(hw_ep->ep_num, is_out, 1);
+	else
+		set_toggle(hw_ep->ep_num, is_out, 0);
+
+	if((*((volatile unsigned char*)(hw_ep->ep_cs_reg)) & 0x2) == 0x2) {
+		printk("Is there already intr in data present!? resuming hcinerr %x ep %d\n",
+			*((volatile unsigned char*)(hw_ep->hcinerr)), hw_ep->ep_num);
+#ifdef SW_WORKAROUND
+			qh->hw_ep->intr_ep_timer.data = (unsigned long)qh;
+			qh->hw_ep->intr_ep_timer.function = intr_nak_retry;
+			qh->hw_ep->timer_armed = 1;
+			mod_timer(&qh->hw_ep->intr_ep_timer, jiffies + msecs_to_jiffies(200));
+#else
+		qh->programmed = 1;
+		*((volatile unsigned char*)(hw_ep->hcinerr)) = (1 << 5);
+#endif
+	}else {
+#ifdef SW_WORKAROUND
+			qh->hw_ep->intr_ep_timer.data = (unsigned long)qh;
+			qh->hw_ep->timer_armed = 1;
+			qh->hw_ep->intr_ep_timer.function = intr_start;
+			mod_timer(&qh->hw_ep->intr_ep_timer, jiffies + msecs_to_jiffies(200));
+#else
+ 		qh->programmed = 1;
+		*((volatile unsigned char*)(hw_ep->ep_cs_reg)) = 0;//Arm Intr in EP?
+#endif
+	}
+}
+
+#ifdef DEBUG_START_FINISH
+#include <linux/sched/sysctl.h>
+
+static unsigned long timeout_jiffies(unsigned long timeout)
+{
+	/* timeout of 0 will disable the watchdog */
+	return timeout ? timeout * HZ : MAX_SCHEDULE_TIMEOUT;
+}
+
+static void start_bulk_in_timer(unsigned long data)
+{
+	unsigned long flags;
+	struct pilot4_hw_ep *hw_ep = (struct pilot4_hw_ep *)data;
+	ushort curr_status, curr_err_status;
+	ushort curr_intr_enable, curr_err_intr_enable;
+	unsigned long hang_check = sysctl_hung_task_timeout_secs;
+
+	spin_lock_irqsave(&hw_ep->pilot_host->lock, flags);
+	curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07irq));
+	curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07errirq));
+	curr_intr_enable = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07ien));
+	curr_err_intr_enable = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcin07errien));
+
+	if(hw_ep->cur_urb) {
+		printk("IN urb timer expired error urb %p status %d jiffies %lu starttime %llu curwait_time %u\n",
+			hw_ep->cur_urb, hw_ep->cur_urb->status, jiffies, hw_ep->start_time, hw_ep->wait_time);
+		printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_qh);
+		printk("epnum %d busy bit set i ask y ??? epcs 0x%x addr %x programmed %d\n",
+			hw_ep->ep_num, *((volatile unsigned char*)(hw_ep->ep_cs_reg)), 
+			(hw_ep->cur_qh)?hw_ep->cur_qh->addr_reg:-1, (hw_ep->cur_qh)?hw_ep->cur_qh->programmed:-1);
+		printk("curr_status %x curr_err_status %x curr_intr_enable %x curr_err_intr_enable %x\n",
+			curr_status, curr_err_status, curr_intr_enable, curr_err_intr_enable);
+		printk("inxcon %x hcinerr %x hang_check %lu\n",
+			*((volatile unsigned char*)(hw_ep->hc_in_con)), *((volatile unsigned char*)(hw_ep->hcinerr)), hang_check);
+	} else {
+		printk("%s hw_ep->cur_urb is NULL\n", __FUNCTION__);
+		printk("curr_status %x curr_err_status %x curr_intr_enable %x curr_err_intr_enable %x\n",
+			curr_status, curr_err_status, curr_intr_enable, curr_err_intr_enable);
+		printk("inxcon %x hcinerr %x hang_check %lu\n",
+			*((volatile unsigned char*)(hw_ep->hc_in_con)), *((volatile unsigned char*)(hw_ep->hcinerr)), hang_check);
+	}
+	hw_ep->wait_time = hw_ep->wait_time + hw_ep->wait_time;
+	if(hw_ep->wait_time > timeout_jiffies(hang_check))
+		hw_ep->wait_time = timeout_jiffies(hang_check);
+	mod_timer(&hw_ep->debug_timer, jiffies + hw_ep->wait_time);
+	spin_unlock_irqrestore(&hw_ep->pilot_host->lock, flags);
+}
+#endif
+
+
+/*In host mode,we issue Tokens i think we dont need to bother about maintaining a list
+  * we get In completion interrupt only on issuing a token and never before that?
+  */
+static void start_bulk_in(struct pilot4_hw_ep *hw_ep, struct pilot_host_hcd *pilot_host, 
+	struct urb *urb, u8 *buf, u32 len, unsigned int is_out)
+{
+	struct pilot4_qh *qh = NULL;
+	u8 ctrl_reg;
+
+	qh = hw_ep->cur_qh;
+#ifdef HOST_DEBUG
+	in_urb = urb;
+	if(qh == NULL)
+		panic("start_bulk_in qh == NULL getout \n"); 
+#endif
+	ctrl_reg = *((volatile unsigned char*)(hw_ep->hc_in_con));
+	if((ctrl_reg & 0xCF) != ((USB_ENDPOINT_XFER_BULK << 2) | (1 <<7))) {
+		printk("%s unexpected ctrlreg %x %p ep_num %d\n",
+			__FUNCTION__, ctrl_reg, ((volatile unsigned char*)(hw_ep->hc_in_con)), hw_ep->ep_num);
+	}
+
+#ifdef CONFIG_SMP
+	if(!spin_is_locked(&pilot_host->lock)) {
+		printk("Spin not locked?\n");
+		dump_stack();
+	}
+#endif
+	smp_rmb();
+	if(hw_ep->cur_urb) {
+		printk("Previous urb still not finished actual length %d transferbuflength %d urb %p status %d\n",
+			hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_urb->status);
+		printk("start_bulk_in epnum %d busy bit set i ask y ??? epcs 0x%x addr %x programmed %d\n",
+			hw_ep->ep_num, *((volatile unsigned char*)(hw_ep->ep_cs_reg)), qh->addr_reg, qh->programmed);
+		printk("qh %p cur_qh %p empty %d? urb %p\n", qh, hw_ep->cur_qh, list_empty(&qh->hep->urb_list), urb);
+#ifdef DEBUG_START_FINISH
+		printk("hw_ep->cur_cpu %d curcpu %d last_complete_time %llu jiffies %lu\n",
+			hw_ep->cur_cpu, raw_smp_processor_id(), hw_ep->last_complete_time, jiffies);
+#endif
+		dump_stack();
+	}
+	hw_ep->cur_urb = urb;
+#ifdef DEBUG_START_FINISH
+	hw_ep->cur_cpu = raw_smp_processor_id();
+	hw_ep->wait_time = msecs_to_jiffies(5000);
+	hw_ep->start_time = jiffies;
+	hw_ep->debug_timer.function = start_bulk_in_timer;
+	hw_ep->debug_timer.data = (unsigned long) hw_ep;
+	//1.5 second
+	mod_timer(&hw_ep->debug_timer, jiffies + msecs_to_jiffies(5000));
+#endif
+	smp_wmb();
+#ifdef HOST_DEBUG_STATS
+#ifndef DEBUG_START_FINISH
+	hw_ep->start_time = jiffies;
+#endif
+	hw_ep->total_urbs++;
+	hw_ep->total_transfer_buffer_length += urb->transfer_buffer_length;
+	if(hw_ep->dev_addr != (qh->addr_reg & 0x7F)) {
+		printk("Address Issue? %s epnum %d addr %d hw_ep->dev_addr %d\n", __FUNCTION__, hw_ep->ep_num, qh->addr_reg,hw_ep->dev_addr);
+	}
+#endif
+	//Reset just for safety?
+	*((volatile unsigned char*)(hw_ep->hcinerr)) = 0;
+        *((volatile unsigned char*)hw_ep->func_addr_reg) = qh->addr_reg & 0x7F;
+	if (usb_gettoggle(urb->dev, qh->epnum, 0))
+		set_toggle(hw_ep->ep_num, is_out, 1);
+	else
+		set_toggle(hw_ep->ep_num, is_out, 0);
+	if((*((volatile unsigned char*)(hw_ep->ep_cs_reg)) & 0x2) == 0x2) {
+		printk("Is there already Bulk in data present!? resuming hcinerr %x ep_num %d\n",
+			*((volatile unsigned char*)(hw_ep->hcinerr)), hw_ep->ep_num);
+#if 0//def SW_WORKAROUND
+			qh->hw_ep->intr_ep_timer.data = (unsigned long)qh;
+			qh->hw_ep->intr_ep_timer.function = intr_nak_retry;
+			mod_timer(&qh->hw_ep->intr_ep_timer, jiffies + msecs_to_jiffies(200));
+#else
+		qh->programmed = 1;
+		*((volatile unsigned char*)(hw_ep->hcinerr)) = (1 << 5);
+#endif
+	}else {
+		qh->programmed = 1;
+		*((volatile unsigned char*)(hw_ep->ep_cs_reg)) = 0;//Arm Bulk in EP?
+	}
+}
+
+#ifdef DEBUG_START_FINISH
+static void start_bulk_out_timer(unsigned long data)
+{
+	unsigned long flags;
+	struct pilot4_hw_ep *hw_ep = (struct pilot4_hw_ep *)data;
+	ushort curr_status, curr_err_status;
+	ushort curr_intr_enable, curr_err_intr_enable;
+	unsigned long hang_check = sysctl_hung_task_timeout_secs;
+
+	spin_lock_irqsave(&hw_ep->pilot_host->lock, flags);
+	curr_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07irq));
+	curr_err_status = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07errirq));
+	curr_intr_enable = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07ien));
+	curr_err_intr_enable = *((volatile unsigned short*)(pilot_host_udc->base_addr + hcout07errien));
+
+	if(hw_ep->cur_urb) {
+		printk("OUT urb timer expired error urb %p status %d jiffies %lu starttime %llu curwait_time %u\n",
+			hw_ep->cur_urb, hw_ep->cur_urb->status, jiffies, hw_ep->start_time, hw_ep->wait_time);
+		printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+			hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_qh);
+		printk("epnum %d epcs 0x%x addr %x programmed %d\n",
+			hw_ep->ep_num, *((volatile unsigned char*)(hw_ep->ep_cs_reg)), 
+			(hw_ep->cur_qh)?hw_ep->cur_qh->addr_reg:-1, (hw_ep->cur_qh)?hw_ep->cur_qh->programmed:-1);
+		printk("curr_status %x curr_err_status %x curr_intr_enable %x curr_err_intr_enable %x\n",
+			curr_status, curr_err_status, curr_intr_enable, curr_err_intr_enable);
+		printk("outxcon %x hcouterr %x hang_check %lu\n",
+			*((volatile unsigned char*)(hw_ep->hc_out_con)), *((volatile unsigned char*)(hw_ep->hcouterr)), hang_check);
+	} else {
+		printk("%s hw_ep->cur_urb is NULL\n", __FUNCTION__);
+		printk("curr_status %x curr_err_status %x curr_intr_enable %x curr_err_intr_enable %x\n",
+			curr_status, curr_err_status, curr_intr_enable, curr_err_intr_enable);
+		printk("outxcon %x hcouterr %x hang_check %lu\n",
+			*((volatile unsigned char*)(hw_ep->hc_out_con)), *((volatile unsigned char*)(hw_ep->hcouterr)), hang_check);
+	}
+	hw_ep->wait_time = hw_ep->wait_time + hw_ep->wait_time;
+	if(hw_ep->wait_time > timeout_jiffies(hang_check))
+		hw_ep->wait_time = timeout_jiffies(hang_check);
+	mod_timer(&hw_ep->debug_timer, jiffies + hw_ep->wait_time);
+	spin_unlock_irqrestore(&hw_ep->pilot_host->lock, flags);
+}
+#endif
+
+static void start_bulk_out(struct pilot4_hw_ep *hw_ep, struct pilot_host_hcd *pilot_host, 
+	struct urb *urb, u8 *buf, u32 len, unsigned int is_out)
+{
+#if 1
+	struct pilot4_qh *qh = NULL;
+	int reset_ret = 0;
+	u8 ctrl_reg;
+	qh = hw_ep->cur_qh;
+#ifdef HOST_DEBUG
+	out_urb = urb;
+	if(qh == NULL)
+		panic("start_bulk_out qh == NULL getout \n");
+#endif
+	ctrl_reg = *((volatile unsigned char*)(hw_ep->hc_out_con));
+	if((ctrl_reg & 0xCF) != ((USB_ENDPOINT_XFER_BULK << 2) | (1 <<7))) {
+		printk("%s unexpected ctrlreg %x %p ep_num %d\n",
+			__FUNCTION__, ctrl_reg, ((volatile unsigned char*)(hw_ep->hc_out_con)), hw_ep->ep_num);
+	}
+	//Make sure ping and resend is not set just for safety :)
+	*((volatile unsigned char*)(hw_ep->hcouterr)) = 0;
+        *((volatile unsigned char*)hw_ep->func_addr_reg) = qh->addr_reg & 0x7F;
+
+#ifdef CONFIG_SMP
+	if(!spin_is_locked(&pilot_host->lock)) {
+		printk("Spin not locked?\n");
+		dump_stack();
+	}
+#endif
+	/*Not sure but I beleive sometimes auto cache coherency using SCU is not maintained by ARM(i dont know what can be the issue)
+	  *TODO check if Shareable bit is set in pagetables
+	  *Ideally i feel SCU should take care of it after spinlock/unlock
+	  *safe side put memory barrier
+	  */
+	smp_rmb();
+	if(hw_ep->cur_urb) {
+		printk("Previous urb still not finished actual length %d transferbuflength %d urb %p status %d\n",
+			hw_ep->cur_urb->actual_length, hw_ep->cur_urb->transfer_buffer_length, hw_ep->cur_urb, hw_ep->cur_urb->status);
+		printk("start_bulk_out epnum %d busy bit set i ask y ??? epcs 0x%x addr %x programmed %d\n",
+			hw_ep->ep_num, *((volatile unsigned char*)(hw_ep->ep_cs_reg)), qh->addr_reg, qh->programmed);
+		printk("qh %p cur_qh %p empty %d? urb %p\n", qh, hw_ep->cur_qh, list_empty(&qh->hep->urb_list), urb);
+#ifdef DEBUG_START_FINISH
+		printk("hw_ep->cur_cpu %d curcpu %d last_complete_time %llu jiffies %lu\n",
+			hw_ep->cur_cpu, raw_smp_processor_id(), hw_ep->last_complete_time, jiffies);
+#endif
+		dump_stack();
+	}
+	hw_ep->cur_urb = urb;
+#ifdef DEBUG_START_FINISH
+	hw_ep->cur_cpu = raw_smp_processor_id();
+	hw_ep->wait_time = msecs_to_jiffies(3000);
+	hw_ep->start_time = jiffies;
+	hw_ep->debug_timer.function = start_bulk_out_timer;
+	hw_ep->debug_timer.data = (unsigned long) hw_ep;
+	mod_timer(&hw_ep->debug_timer, jiffies + msecs_to_jiffies(3000));
+#endif
+	smp_wmb();	
+
+#ifdef HOST_DEBUG_STATS
+#ifndef DEBUG_START_FINISH
+	hw_ep->start_time = jiffies;
+#endif
+	hw_ep->total_urbs++;
+	hw_ep->total_transfer_buffer_length += urb->transfer_buffer_length;
+	if(hw_ep->dev_addr != (qh->addr_reg & 0x7F)) {
+		printk("Address Issue? %s epnum %d addr %d hw_ep->dev_addr %d\n", __FUNCTION__, hw_ep->ep_num, qh->addr_reg,hw_ep->dev_addr);
+	}
+#endif
+	//Check for Busy Bit
+	if((*((volatile unsigned char*)(hw_ep->ep_cs_reg)) & 0x2) == 0x2) {
+		int wait_time = 0;
+		/* Basically with the current implementation we start a new USB
+		  * transaction only if previous is completed with or without error
+		  * Busy should not be ideally set
+		  * TODO add some counters to see if we are programming twice by error
+		  * programming second time while first is going on is highly unlikely
+		  * for ex:- program somehting during giveback we release lock and see if 
+		  * have programmed twice.
+		  */
+#if 1
+		printk("start_bulk_out epnum %d busy bit set i ask y ??? epcs 0x%x addr %x programmed %d\n",
+			hw_ep->ep_num, *((volatile unsigned char*)(hw_ep->ep_cs_reg)), qh->addr_reg, qh->programmed);
+		printk("qh %p cur_qh %p empty %d? urb %p\n", qh, hw_ep->cur_qh, list_empty(&qh->hep->urb_list), urb);
+		printk("qh->free_recursion %d giveback_recursion %d urb->use_count %d\n", qh->free_recursion, giveback_recursion, atomic_read(&urb->use_count));
+		if(giveback_recursion) {
+			printk("qh %p urb %p free_recursion %d giveback_qh %p free_recursion %d\n", qh, urb, qh->free_recursion, giveback_qh, (giveback_qh)?giveback_qh->free_recursion:-1);
+			printk("givebackQH:addr %d ep %d type %d curQH:addr %d ep %d type %d\n", 
+				giveback_qh->addr_reg,qh->epnum, qh->type,  qh->addr_reg, qh->epnum, qh->type);
+		}
+#endif
+		while((*((volatile unsigned char*)(hw_ep->ep_cs_reg)) & 0x2)) {
+#if 0
+			schedule_timeout(1);
+#else
+			mdelay(10);
+#endif
+			wait_time++;
+			if(wait_time > 25) {//250msecs
+				reset_ret = endpoint_reset(hw_ep, is_out);
+				if(reset_ret) {
+					printk("start_B_OUT:reset_ret interrupt status is set clearning it\n");
+					reset_ret = clear_intr_status(qh->hw_ep, 0);
+					if(!reset_ret)
+						printk("Cleanup:INtr status could not be cleared for IN\n");
+				}		
+			}
+		}
+	}
+	if (usb_gettoggle(urb->dev, qh->epnum, 1))
+		set_toggle(hw_ep->ep_num, is_out, 1);
+	else
+		set_toggle(hw_ep->ep_num, is_out, 0);
+	ep_write_fifo(pilot_host, urb, hw_ep);
+	
+#endif
+}
+/*
+ * Program an HDRC endpoint as per the given URB
+ * Context: irqs blocked, controller lock held
+ */
+static void pilot4_ep_program(struct pilot_host_hcd *pilot_host, u8 epnum,
+			struct urb *urb, unsigned int is_out,
+			u8 *buf, u32 len, struct pilot4_hw_ep *hw_ep1)
+{
+#if 1
+	struct pilot4_hw_ep	*hw_ep = NULL;
+	struct pilot4_qh		*qh;
+	u16			packet_sz;
+
+	if(is_out)
+		hw_ep = pilot_host->endpoints + epnum;
+	else
+		hw_ep = pilot_host->endpoints + (0x10 |epnum);
+#ifdef HAVE_WORKAROUND
+	if(hw_ep1 != hw_ep) {
+		printk("Some issue ith hw_ep epnum %d name %s epnumd %d name %s\n",
+			hw_ep->ep_num, hw_ep->name, hw_ep1->ep_num, hw_ep1->name);
+		hw_ep = hw_ep1;
+	}
+#endif
+	qh = hw_ep->cur_qh;
+	packet_sz = qh->maxpacket;
+	if(is_out)
+		*((volatile unsigned char*)(hw_ep->hcoutctrl)) = qh->epnum;
+	else
+		*((volatile unsigned char*)(hw_ep->hcinctrl)) = qh->epnum;
+	if(hw_ep->last_dev_addr != (qh->addr_reg & 0x7F)) {
+		//TODO Do we need to reset this endpoint 
+		//take care of toggle here?
+		hw_ep->reinit = 0;
+	}
+	hw_ep->last_dev_addr = qh->addr_reg & 0x7F;
+	if(no_endp_handshake) {
+		DBG_ERROR_PRINT("%s hw%d urb %p spd%d dev%d ep%d%s "
+				"h_addr%02x h_port%02x bytes %d\n",
+			is_out ? "-->" : "<--",
+			epnum, urb, urb->dev->speed,
+			qh->addr_reg, qh->epnum, is_out ? "out" : "in",
+			qh->h_addr_reg, qh->h_port_reg,
+			len);
+	}
+	DBG(3, "%s hw%d urb %p spd%d dev%d ep%d%s "
+				"h_addr%02x h_port%02x bytes %d\n",
+			is_out ? "-->" : "<--",
+			epnum, urb, urb->dev->speed,
+			qh->addr_reg, qh->epnum, is_out ? "out" : "in",
+			qh->h_addr_reg, qh->h_port_reg,
+			len);
+	switch (qh->type) {
+		case USB_ENDPOINT_XFER_CONTROL:
+			program_ep0(hw_ep, pilot_host, buf, len);
+			break;
+		case USB_ENDPOINT_XFER_INT:
+			if(is_out){
+			} else {
+				start_intr_in(hw_ep, pilot_host, urb, buf, len, is_out);
+			}
+			break;
+		case USB_ENDPOINT_XFER_BULK:
+#ifdef HOST_DEBUG
+	//VERify if hw_ep == pilot_host->bulk_out/in
+#endif
+			if(is_out){
+				start_bulk_out(hw_ep, pilot_host, urb, buf, len, is_out);
+			} else {
+				start_bulk_in(hw_ep, pilot_host, urb, buf, len, is_out);
+			}
+			break;
+	}
+#endif
+}
+
+void inline pilot_hc_write_fifo(struct pilot4_hw_ep *hw_ep, u16 len, const u8 *buf)
+{
+	int i = 0;
+	for(i = 0;i < len; i++)
+		*((volatile unsigned char*)(hw_ep->ep_data_buf_reg)) = buf[i];
+}
+
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+static int hc_ep0_out_bulk_handler(struct pilot_host_hcd *pilot_host, struct pilot4_hw_ep* bulk_hw_ep)
+{
+        struct urb              *urb;
+        struct pilot4_hw_ep     *hw_ep = &pilot_host->endpoints[0];
+        struct pilot4_qh                *qh = bulk_hw_ep->cur_qh;
+        bool                    complete = false;
+        irqreturn_t             retval = IRQ_NONE;
+        unsigned long flags;
+
+        *((volatile unsigned short*)(pilot_host->base_addr + hcout07irq)) = (1 << bulk_hw_ep->ep_num);//ack interrupt
+        spin_lock_irqsave(&pilot_host->lock, flags);
+        if(qh == NULL || ((unsigned int)qh == 0xDEADBEEF)) {
+                printk("hcep0out_handler qh %p error\n", qh);
+                spin_unlock_irqrestore(&pilot_host->lock, flags);
+                return -1;
+        }
+        urb = next_urb(qh);
+        if(urb == NULL) {
+                printk("qh %p addr_reg %x urb is NULL!!\n", qh, qh->addr_reg);
+                return -1;
+        }
+
+        if (PILOT4_EP0_STATUS == pilot_host->ep0_stage) {
+                retval = IRQ_HANDLED;
+                complete = true;
+        }
+        if (!complete) {
+                pilot4_ep0_continue(pilot_host, 0, urb);
+        } else
+                advance_schedule(pilot_host, urb, hw_ep, USB_DIR_OUT);
+        spin_unlock_irqrestore(&pilot_host->lock, flags);
+        return retval;
+
+}
+#endif
+/*
+ * Service the default endpoint (ep0) as host.
+ * Return true until it's time to start the status stage.
+ */
+static void pilot4_ep0_continue(struct pilot_host_hcd *pilot_host, u16 len, struct urb *urb)
+{
+#if 1
+	u8			*fifo_dest = NULL;
+	u16			fifo_count = 0;
+	int i  = 0, ctrl_write = -1;
+	struct pilot4_hw_ep	*hw_ep = &pilot_host->endpoints[0];
+	struct pilot4_qh		*qh = hw_ep->cur_qh;
+	struct usb_ctrlrequest	*request;
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+	int set_tgle = 0;
+#endif
+#define hcsettoggle (1<<6)
+#if 0
+	printk("pilot4_ep0_continue pilot_host->ep0_stage %d\n", pilot_host->ep0_stage);
+	printk("urb->transfer_buffer_length %d urb->actual_length %d len %d\n",
+		urb->transfer_buffer_length, urb->actual_length, len);
+#endif
+	switch (pilot_host->ep0_stage) {
+	case PILOT4_EP0_IN:
+		fifo_dest = urb->transfer_buffer + urb->actual_length;
+		fifo_count = min(len, ((u16) (urb->transfer_buffer_length
+					- urb->actual_length)));
+		if (fifo_count < len) {
+			urb->status = -EOVERFLOW;
+			printk("PILOT4_EP0_IN overflow urb->transfer_buffer_length %d len %d fifo_count %d\n",
+				urb->transfer_buffer_length, len, fifo_count);
+		}
+
+		for(i = 0 ;i < fifo_count; i++)
+			fifo_dest[i] = *((volatile unsigned char*)(hw_ep->ep_data_buf_reg + i));
+		urb->actual_length += fifo_count;
+		if (len < qh->maxpacket) {
+			/* always terminate on short read; it's
+			 * rarely reported as an error.
+			 */
+			ctrl_write = 1;//Issue OUT status token?
+ 			pilot_host->ep0_stage = PILOT4_EP0_STATUS;
+		} else if (urb->actual_length <
+				urb->transfer_buffer_length) {
+			*((volatile unsigned char*)(hw_ep->ep_byte_cnt_reg)) = 0;//arm HCIN0 EP
+			return;
+		} else {
+			//TODO Check if this can happen
+			ctrl_write = 1;
+ 			pilot_host->ep0_stage = PILOT4_EP0_STATUS;
+		}
+			
+		
+		break;
+	case PILOT4_EP0_START:
+		request = (struct usb_ctrlrequest *) urb->setup_packet;
+#if 0
+		printk("PILOT4_EP0_START wLength 0x%x bRequestType 0x%x\n", request->wLength,
+			request->bRequestType);
+#endif
+		//TODO Do we need to clear hcset in ep0cs?
+		if (!request->wLength) {
+			DBG(4, "start no-DATA\n");
+			ctrl_write = 0;
+ 			pilot_host->ep0_stage = PILOT4_EP0_STATUS;
+ 			break;
+		} else if (request->bRequestType & USB_DIR_IN) {
+			DBG(4, "start IN-DATA\n");
+			pilot_host->ep0_stage = PILOT4_EP0_IN;
+			*((volatile unsigned char*)(hw_ep->ep_byte_cnt_reg)) = 0;//arm HCIN0 EP
+			return;
+
+		} else {
+			DBG(4, "start OUT-DATA\n");
+			pilot_host->ep0_stage = PILOT4_EP0_OUT;
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+			set_tgle = 1;
+#endif
+		}
+		/* FALLTHROUGH */
+	case PILOT4_EP0_OUT:
+		fifo_count = min(qh->maxpacket, ((u16)
+				(urb->transfer_buffer_length
+				- urb->actual_length)));
+		ctrl_write = 0;//Always issue IN Token?
+		if (fifo_count) {
+			fifo_dest = (u8 *) (urb->transfer_buffer
+					+ urb->actual_length);
+			DBG(3, "Sending %d bytes to %p\n",
+					fifo_count, fifo_dest);
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+			bulk_hw_ep->cur_qh = (void*)qh;//EP0 usage
+                        *((volatile unsigned char*)(bulk_hw_ep->hcoutctrl)) = 0;
+                        //Make sure ping and resend is not set just for safety :)
+                        *((volatile unsigned char*)(bulk_hw_ep->hcouterr)) = 0;
+                        *((volatile unsigned char*)bulk_hw_ep->func_addr_reg) = qh->addr_reg & 0x7F;
+                        if(set_tgle) {
+                                set_toggle(bulk_hw_ep->ep_num, 1, 1);
+                                usb_settoggle(urb->dev, 0, 1, 1);
+                        } else {
+                                set_tgle = usb_gettoggle(urb->dev, 0, 1);
+                                set_toggle(bulk_hw_ep->ep_num, 1, (1 ^ set_tgle));
+                                usb_settoggle(urb->dev, 0, 1, (1 ^ set_tgle));
+                        }
+                        for(i = 0;i < fifo_count;i++)
+                                *(u8 *)(bulk_hw_ep->ep_data_buf_reg) = fifo_dest[i];
+                        urb->actual_length += fifo_count;
+                        *((volatile unsigned short*) (bulk_hw_ep->ep_byte_cnt_reg)) = fifo_count;
+                        *((volatile unsigned char*) (bulk_hw_ep->ep_cs_reg)) = 0x00;
+#else
+			for(i = 0;i < fifo_count;i++)
+				*((volatile unsigned char*)(hw_ep->ep_data_buf_reg1 + i)) = fifo_dest[i];
+
+			urb->actual_length += fifo_count;
+			*((volatile unsigned char*)(hw_ep->ep_byte_cnt_reg1)) = fifo_count;
+#endif
+			return;
+		} else {
+			//done with data start status?
+//			ctrl_write = 1;
+			pilot_host->ep0_stage = PILOT4_EP0_STATUS;
+		}
+		break;
+	default:
+		printk("bogus ep0 stage %d!!\n", pilot_host->ep0_stage);
+		return;
+//break;
+	}
+	if(pilot_host->ep0_stage == PILOT4_EP0_STATUS) {
+		*((volatile unsigned char*)(hw_ep->ep_cs_reg)) = hcsettoggle;
+		if(ctrl_write == 0)
+			*((volatile unsigned char*)(hw_ep->ep_byte_cnt_reg)) = 0;//status IN token HCIN0BC
+		else
+			*((volatile unsigned char*)(hw_ep->ep_byte_cnt_reg1)) = 0;//OUT status token HCOUT0BC
+	}
+	return;
+#endif
+}
+#ifndef DYNAMIC_CONFIGURE_EP
+/* Currently Bulk cannot be shared as we dont get NAK error interrupt */
+static struct pilot4_hw_ep* chose_best_bulk_hw_ep(int is_in, struct pilot_host_hcd	*pilot_host)
+{
+	int i = 0;
+	struct pilot4_hw_ep *hw_ep = NULL;
+	struct pilot4_hw_ep **bulk_hw_ep = NULL;
+	struct pilot4_hw_ep *chosen_hw_ep = NULL;
+
+	if(is_in)
+		bulk_hw_ep = pilot_host->bulk_in;
+	else
+		bulk_hw_ep = pilot_host->bulk_out;
+
+	for(i = 0;i < 8;i++) {
+		hw_ep = bulk_hw_ep[i];
+		if(hw_ep != NULL){
+			if(hw_ep->cur_qh == NULL)//This is free
+				chosen_hw_ep = hw_ep;
+#if 0
+			else
+				printk("Assigned hw_ep->epnum %p type %x addr %x cur_qh %p\n",
+					hw_ep->ep_num, hw_ep->cur_qh->type, hw_ep->cur_qh->addr_reg, hw_ep->cur_qh);
+#endif
+		}
+	}
+	//TODO Add more logic to select a best EP.
+	if(chosen_hw_ep == NULL)
+		printk("No free H/W BULK IN EP's available!\n");
+	return chosen_hw_ep;
+}
+
+static struct pilot4_hw_ep * chose_best_intr_hw_ep(int is_in, struct pilot_host_hcd	*pilot_host)
+{
+	int i = 0;
+	struct pilot4_hw_ep *hw_ep = NULL;
+	struct pilot4_hw_ep **intr_hw_ep = NULL;
+	struct pilot4_hw_ep *chosen_hw_ep = NULL;
+
+	if(is_in)
+		intr_hw_ep = pilot_host->intr_in;
+	else
+		intr_hw_ep = pilot_host->intr_out;
+
+	for(i = 0;i < 8;i++) {
+		hw_ep = intr_hw_ep[i];
+		if(hw_ep != NULL){
+			if(hw_ep->cur_qh == NULL)//This is free
+				chosen_hw_ep = hw_ep;
+		}
+	}
+	if(chosen_hw_ep == NULL) {
+ 		for(i = 0;i < 8;i++) {
+			hw_ep = intr_hw_ep[i];
+			if(hw_ep != NULL){
+				printk("hw_ep %p epnum %d curqh %p\n",
+					hw_ep,hw_ep->ep_num, hw_ep->cur_qh);
+				if(hw_ep->cur_qh != NULL && ((unsigned int)hw_ep->cur_qh != 0xDEADBEEF))
+					printk("hw_ep->cur_qh->addr_reg %x\n", hw_ep->cur_qh->addr_reg);
+			}
+		}
+	}
+	//TODO Add more logic to select a best EP.
+	if(chosen_hw_ep == NULL) {
+		for(i = 0;i < 8;i++) {
+			hw_ep = intr_hw_ep[i];
+			if(hw_ep != NULL){
+				chosen_hw_ep = hw_ep;//For now choose the first one available 
+				printk("More than 1 qh shared chosen_hw_ep %p epnum %d\n", chosen_hw_ep, chosen_hw_ep->ep_num);
+				break;
+			}
+		}
+	}
+	return chosen_hw_ep;
+}
+#endif
+/* Each hardware Endpoint has a queue USB requests are executed 
+  * with following priority
+  * EP0in/out, INTR IN, BULK in, BULK OUT
+  */
+  static int pilot4_usb_schedule(
+	struct pilot_host_hcd	*pilot_host,
+	struct pilot4_qh		*qh,
+	int			is_in)
+{
+#if 1
+	int idle = 0;
+	struct pilot4_hw_ep	*hw_ep = NULL;
+	struct list_head	*head = NULL;
+#ifdef DYNAMIC_CONFIGURE_EP
+	extern struct pilot4_hw_ep* chose_bulk_hw_ep(int , struct pilot_host_hcd *, unsigned char );
+	extern struct pilot4_hw_ep* chose_int_hw_ep(int , struct pilot_host_hcd *, unsigned char );
+#endif
+	switch(qh->type) {
+		case USB_ENDPOINT_XFER_CONTROL:
+			head = &pilot_host->endpoints[0].queue;
+			hw_ep = &pilot_host->endpoints[0];
+			break;
+		case USB_ENDPOINT_XFER_BULK:
+#ifndef DYNAMIC_CONFIGURE_EP
+			hw_ep = chose_best_bulk_hw_ep(is_in, pilot_host);
+#else
+			hw_ep = chose_bulk_hw_ep(is_in, pilot_host, qh->addr_reg);
+#endif
+//			printk("Choose hw_ep->epnum %d for bulkd dev addr %x\n",hw_ep->ep_num, qh->addr_reg);
+			head = &hw_ep->queue;
+			break;
+		case USB_ENDPOINT_XFER_INT:
+#ifndef DYNAMIC_CONFIGURE_EP
+			hw_ep = chose_best_intr_hw_ep(is_in, pilot_host);
+#else
+			hw_ep = chose_int_hw_ep(is_in, pilot_host, qh->addr_reg);
+#endif
+			head = &hw_ep->queue;
+			break;
+		default:
+			printk("Unsupported Endpoint Type %d\n", qh->type);
+			break;
+	}
+	qh->hw_ep = hw_ep;
+	qh->hep->hcpriv = qh;//I dont know y this is required lets keep for now
+	if (head) {
+		idle = list_empty(head);
+		if(qh->type == USB_ENDPOINT_XFER_CONTROL && idle == 0)
+			printk("Control EP list not empty pilot_host->cur_qh %p addr_reg 0x%x\n", hw_ep->cur_qh, qh->addr_reg);
+		list_add_tail(&qh->ring, head);
+		qh->mux = 1;
+	}
+	if (idle)
+		pilot4_start_urb(pilot_host, is_in, qh);
+	else {
+		start_intr_print = 0;
+		printk("More than 1 req present ? hw_ep->cur_qh %p qh %p hw_ep->ep_num %d type %d\n",
+			hw_ep->cur_qh, qh, hw_ep->ep_num, qh->type);
+		if(hw_ep->cur_qh != NULL &&  ((unsigned int)hw_ep->cur_qh != 0xDEADBEEF))
+			printk("hw_ep->cur_qh->addr %x qh->addr %x programmed %d\n",
+				hw_ep->cur_qh->addr_reg, qh->addr_reg, hw_ep->cur_qh->programmed);
+	}
+	return 0;
+#endif
+}
+
+static int pilot4_urb_enqueue(
+	struct usb_hcd			*hcd,
+	struct urb			*urb,
+	gfp_t				mem_flags)
+{
+	unsigned long			flags;
+	struct pilot_host_hcd *pilot_host = hcd_to_pilot_hcd(hcd);
+	struct usb_host_endpoint	*hep = urb->ep;
+	struct pilot4_qh			*qh = NULL;
+	struct usb_endpoint_descriptor	*epd = &hep->desc;
+	int				ret;
+	unsigned			interval;
+	register unsigned long sp asm ("sp");
+
+	/* host role must be active */
+	if (!HC_IS_RUNNING(hcd->state)){
+		printk("HC Not running!\n");
+		return -ENODEV;
+	}
+	/* Aquire the lock and make sure to not release atleast till qh->hep->hcpriv is initialized.
+	  * Basic Thumb rule we need to allocate a distinct QH for each distinct usb_host_endpoint 
+	  * if for same hep a new urb wants to be enqueued then just link (usb_hcd_link_urb_to_ep) 
+	  * and return(hep->hcpriv should not be NULL in this case). There will be lots of issues if
+	  * 2 different qh's are allocated for same hep, This is possible if for same device and for same EP
+	  * 2 different urb's are enqueued from different cpu's almost at the same time. Basically when we link
+	  * urb it will be linked to hep->urb_list, so even though qh is unique hep is not and so is not
+	  * urb_list.
+	  */
+	spin_lock_irqsave(&pilot_host->lock, flags);
+	ret = usb_hcd_link_urb_to_ep(hcd, urb);
+	qh = ret ? NULL : hep->hcpriv;
+	if (qh)
+		urb->hcpriv = qh;
+	if (qh || ret) {
+		if(ret)
+			printk("error %s %d qh %p\n", __FUNCTION__, __LINE__, qh);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return ret;
+	}
+
+
+	qh = kzalloc(sizeof *qh, GFP_ATOMIC);//TODO Later we can use mempool(see mempool_create_kmalloc_pool)
+	if (!qh) {
+		usb_hcd_unlink_urb_from_ep(hcd, urb);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		printk("error %s %d\n", __FUNCTION__, __LINE__);		
+		return -ENOMEM;
+	}
+
+	if(unlikely(pilot_host_udc->debug_level & DETAILED_RECURSION_PRINT && giveback_recursion)) {
+		unsigned long this_size;
+		this_size = ((unsigned long)sp) & (THREAD_SIZE-1);
+		this_size = THREAD_SIZE - this_size;
+		DBG_RECURSION_PRINT("pilot4_urb_enqueue sp %lx current %p %s task_stack_page %p this_size %ld giveback_recursion %d thrinfo %p\n",
+			sp, current, current->comm, task_stack_page(current),this_size, giveback_recursion, current_thread_info());
+		DBG_RECURSION_PRINT("qh %p urb %p free_recursion %d giveback_qh %p free_recursion %d\n", qh, urb, qh->free_recursion, giveback_qh, (giveback_qh)?giveback_qh->free_recursion:-1);
+		DBG_RECURSION_PRINT("givebackQH:addr %d ep %d type %d curQH:addr %d ep %d type %d\n", 
+			giveback_qh->addr_reg,qh->epnum, qh->type,  qh->addr_reg, epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK,
+			epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK);
+		print_symbol("caller is %s\n", (long)__builtin_return_address(0));
+	}
+
+	qh->hep = hep;
+	qh->dev = urb->dev;
+	INIT_LIST_HEAD(&qh->ring);
+	qh->is_ready = 1;
+
+	qh->maxpacket = le16_to_cpu(epd->wMaxPacketSize);
+
+	/* no high bandwidth support yet */
+	if (qh->maxpacket & ~0x7ff) {
+		ret = -EMSGSIZE;
+		printk("error %s %d\n", __FUNCTION__, __LINE__);		
+		usb_hcd_unlink_urb_from_ep(hcd, urb);
+		kfree(qh);
+		spin_unlock_irqrestore(&pilot_host->lock, flags);	
+		return ret;
+	}
+
+	qh->epnum = epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
+	qh->type = epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;
+
+	/* NOTE: urb->dev->devnum is wrong during SET_ADDRESS */
+	qh->addr_reg = (u8) usb_pipedevice(urb->pipe);
+
+	interval = min((u8)16, epd->bInterval);
+	switch (qh->type) {
+	case USB_ENDPOINT_XFER_INT:
+		if (USB_SPEED_FULL == urb->dev->speed) {
+			interval = epd->bInterval;
+			if (!interval)
+				interval = 1;
+		}
+		break;
+		/* FALLTHROUGH */
+	case USB_ENDPOINT_XFER_ISOC:
+		printk("Error USB_ENDPOINT_XFER_ISOC\n");
+		return -EINVAL;
+	default:
+		interval = 0;
+	}
+	qh->intv_reg = interval;
+
+	if (hep->hcpriv) {
+		kfree(qh);
+		printk("concurrent activity!!\n");
+		qh = NULL;
+		ret = 0;
+	} else
+		ret = pilot4_usb_schedule(pilot_host, qh,
+				epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK);
+
+	if (ret == 0) {
+		urb->hcpriv = qh;
+	}
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+
+	if (ret != 0) {
+		printk("%s ret %d\n", __FUNCTION__, ret);
+	}
+	return ret;
+}
+
+
+/*
+ * abort a transfer that's at the head of a hardware queue.
+ * called with controller locked, irqs blocked
+ * that hardware queue advances to the next transfer, unless prevented
+ */
+static int pilot4_cleanup_urb(struct urb *urb, struct pilot4_qh *qh, int is_in, struct pilot_host_hcd *pilot_host)
+{
+#if 1
+	struct pilot4_hw_ep	*ep = qh->hw_ep;
+	int reset_ret = 1;
+
+#ifdef HAVE_WORKAROUND	
+	unsigned		hw_end = ep->ep_num;
+	printk("pilot4_cleanup_urb hw_end %d urb->status %d pilot_host %p ep %p hw_end %d\n", hw_end, urb->status, pilot_host, ep, hw_end);
+	printk("intr_in %p bulk_in %p bulk_out %p EP0 %p\n", pilot_host->intr_in, pilot_host->bulk_in, pilot_host->bulk_out, &pilot_host->endpoints[0]);
+	if(ep == pilot_host->bulk_in[3] || ep == pilot_host->bulk_out[3])
+		hw_end = 3;
+	else if(ep == pilot_host->intr_in[1])
+		hw_end = 1;
+#endif
+	printk("pilot4_cleanup_urb type %d is_in %d urb %p addr 0x%x programmed %d urb %p\n",
+		qh->type, is_in, urb, qh->addr_reg, qh->programmed, urb);
+	if(qh->type != USB_ENDPOINT_XFER_CONTROL) {
+		if(is_in) {
+			reset_ret = endpoint_reset(qh->hw_ep, 0);
+			if(!reset_ret) {
+				printk("Cleanupurb:IN INtr Status is not set?\n");
+			} else {
+				reset_ret = clear_intr_status(qh->hw_ep, 0);
+				if(reset_ret)
+					printk("Cleanup:INtr status could not be cleared for IN\n");
+			}
+			if((*((volatile unsigned char*)(qh->hw_ep->ep_cs_reg)) & 0x2) == 0x2) {			
+				printk("1:busy bit set even after endpoint reset? epcsreg %x\n",
+					*((volatile unsigned char*)(qh->hw_ep->ep_cs_reg)));
+			}
+		}
+		else {
+			reset_ret = endpoint_reset(qh->hw_ep, 1);
+			if(!reset_ret) {
+				printk("Cleanupurb:OUT INtr Status is not set?\n");
+			} else {
+				reset_ret = clear_intr_status(qh->hw_ep, 1);
+				if(reset_ret)
+					printk("Cleanup:INtr status could not be cleared for OUT\n");
+			}
+			if((*((volatile unsigned char*)(qh->hw_ep->ep_cs_reg)) & 0x2) == 0x2) {
+				printk("busy bit set even after endpoint reset? epcsreg %x\n",
+					*((volatile unsigned char*)(qh->hw_ep->ep_cs_reg)));
+			}
+		}
+	}
+	/* Todo shd we do anything with Toggle value? i hope not... */
+	advance_schedule(pilot_host, urb, ep, is_in);
+	return 0;
+#endif
+}
+
+static int pilot4_urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status)
+{
+	struct pilot_host_hcd	*pilot_host;
+	struct pilot4_qh		*qh = NULL;
+	struct list_head	*sched = NULL;
+	unsigned long		flags;
+	int			ret = -1;
+	struct urb * current_urb = NULL;
+	struct pilot4_qh *temp_qh = NULL;
+	int total_qh = 0;
+	pilot_host = hcd_to_pilot_hcd(hcd);
+
+	print_giveback = 1;
+	spin_lock_irqsave(&pilot_host->lock, flags);
+	ret = usb_hcd_check_unlink_urb(hcd, urb, status);
+	if (ret)
+		goto done;
+
+	qh = urb->hcpriv;
+	if (!qh)
+		goto done;
+
+	DBG_CLEANUP_PRINT("pilot4_urb_dequeue cur_qh %p qh %p type %x hep %p urb %p\n", pilot_host->cur_qh, qh, qh->type, qh->hep, urb);
+	DBG_CLEANUP_PRINT("qh->is_ready%d programmed %d abort_pending %d hw_ep %p qh->hw_ep->epnum %d\n",
+		qh->is_ready,qh->programmed, qh->abort_pending, qh->hw_ep, (qh->hw_ep)?qh->hw_ep->ep_num:-1);
+	DBG_CLEANUP_PRINT("next_urb(qh) %p hw_ep->cur_qh %p last_freed_qh %p last_freed_urb %p\n",
+		next_urb(qh), qh->hw_ep->cur_qh, qh->hw_ep->last_freed_qh, qh->hw_ep->last_freed_urb);
+	if(in_giveback)  {
+#ifndef CONFIG_USB_PILOT4_HCD_MODULE
+		show_stack(NULL, NULL);
+#endif
+	}
+
+	if (!qh->is_ready || urb->urb_list.prev != &qh->hep->urb_list)
+		ret = -EINPROGRESS;
+	else {
+		switch (qh->type) {
+		case USB_ENDPOINT_XFER_CONTROL:
+			sched = &pilot_host->endpoints[0].queue;
+			break;
+		case USB_ENDPOINT_XFER_BULK:
+				if (usb_pipein(urb->pipe))
+					sched = &qh->hw_ep->queue;
+				else
+					sched = &qh->hw_ep->queue;
+				break;
+		case USB_ENDPOINT_XFER_INT:
+				if (usb_pipein(urb->pipe))
+					sched = &qh->hw_ep->queue;
+				else
+					sched = &qh->hw_ep->queue;
+				break;
+
+		default:
+			sched = NULL;
+			break;
+		}
+	}
+	list_for_each_entry (temp_qh, &qh->hw_ep->queue, ring) {
+		total_qh++;
+	}
+	if(total_qh > 1 ){
+		DBG_DETAILED_CLEANUP_PRINT("More than 1 qh is queued on hw epnum %d!!\n", qh->hw_ep->ep_num);
+		list_for_each_entry (temp_qh, &qh->hw_ep->queue, ring) {
+			DBG_DETAILED_CLEANUP_PRINT("qh %p qh->type %x qh->addr %x qh->epnum %d next_urb %p\n",
+				temp_qh, temp_qh->type,temp_qh->addr_reg, temp_qh->epnum, next_urb(temp_qh));
+			DBG_DETAILED_CLEANUP_PRINT("bEndpointAddress %d addr_reg %x programmed %d\n",
+				temp_qh->hep->desc.bEndpointAddress, temp_qh->addr_reg, temp_qh->programmed);
+		}
+	}
+
+	/* NOTE:  qh is invalid unless !list_empty(&hep->urb_list) */
+	if (ret < 0 || (sched && qh != first_qh(sched))) {
+		int	ready = qh->is_ready;
+
+		ret = 0;
+		qh->is_ready = 0;
+		if(qh->hw_ep->cur_urb && qh->hw_ep->cur_urb == urb) {
+			printk("active urb is being unlinked with out reset? can this be an error programmed %d line %d\n", qh->programmed, __LINE__);
+		}
+		pilot4_giveback(qh, urb, status);
+		qh->is_ready = ready;
+	} else {
+		current_urb = next_urb(qh);
+#ifdef CLEANUP_DEBUG
+		DBG_CLEANUP_PRINT("Calling pilot4_cleanup_urb programmed %d urb %p current_urb %p\n",
+			qh->programmed, urb, current_urb);
+#endif
+		if(current_urb != urb) {
+			if(qh->hw_ep->cur_urb && qh->hw_ep->cur_urb == urb) {
+				printk("active urb is being unlinked with out reset? can this be an error programmed %d line %d\n", qh->programmed, __LINE__);
+			}
+			pilot4_giveback(qh, urb, status);
+			goto done;
+		}
+
+		//spin_unlock_irqrestore(&pilot_host->lock, flags);
+		if(1 || qh->programmed) {
+			//spin_lock_irqsave(&pilot_host->lock, flags);		
+			urb->status = status;
+			ret = pilot4_cleanup_urb(urb, qh, urb->pipe & USB_DIR_IN, pilot_host);
+		}
+		else
+			ret = 0;
+		print_giveback = 0;
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return ret;
+	}
+done:
+	if(print_giveback) {
+		DBG_CLEANUP_PRINT("%s %d\n", __FUNCTION__, __LINE__);
+	}
+	if(in_giveback == 0)
+		print_giveback = 0;
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+	return ret;
+}
+
+/* disable an endpoint */
+static void
+pilot4_ep_disable(struct usb_hcd *hcd, struct usb_host_endpoint *hep)
+{
+	unsigned long		flags;
+	struct pilot_host_hcd		*pilot_host = hcd_to_pilot_hcd(hcd);
+	struct pilot4_qh		*qh = hep->hcpriv;
+	struct urb		*urb = NULL, *tmp = NULL;
+	struct list_head	*sched;
+	int i  =0;
+	DBG_CLEANUP_PRINT("pilot4_h_disable pilot_host->cur_qh %p qh %p bEndpointAddress %x\n", pilot_host->cur_qh, qh, hep->desc.bEndpointAddress);
+	if (!qh)
+		return;
+	print_giveback = 1;
+	DBG_CLEANUP_PRINT("qh->programmed %d abort_pending %d type %d qh->hw_ep %p\n", qh->programmed, qh->abort_pending, qh->type, qh->hw_ep);
+	if(qh->hw_ep)
+		DBG_CLEANUP_PRINT("qh->hw_ep->epnum %d bmAttributes %x timer_armed %d\n", qh->hw_ep->ep_num, qh->hw_ep->bmAttributes, qh->hw_ep->timer_armed);
+	spin_lock_irqsave(&pilot_host->lock, flags);
+
+	switch (qh->type) {
+	case USB_ENDPOINT_XFER_CONTROL:
+		sched = &pilot_host->endpoints[0].queue;
+		printk("qh->programmed %d is_control!!\n", qh->programmed);
+		break;
+	default:
+		break;
+	}
+
+	/* NOTE:  qh is invalid unless !list_empty(&hep->urb_list) */
+
+	/* kick first urb off the hardware, if needed */
+	qh->is_ready = 0;
+	if(qh->programmed) {
+		urb = next_urb(qh);
+		if(urb == NULL)
+			printk("urb is NULL!!\n");
+		DBG_CLEANUP_PRINT("urb->unlinked %d status %d qh->type %d\n", urb->unlinked, urb->status, qh->type);
+		if (!urb->unlinked)
+			urb->status = -ESHUTDOWN;
+	}
+	qh->abort_pending = 1;
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+	/* Wait for 50ms for the interrupt to finish, if we get interrupt then its good.
+	  * else we will reset the Non Control Endpoint
+	*/
+	while(i < 50) {
+		msleep(1);
+		if(qh->programmed == 0) {
+			msleep(1);
+			break;
+		}
+		i++;
+	}
+	spin_lock_irqsave(&pilot_host->lock, flags);
+	/* It is still programmed something must be wrong we will do h/w cleanup as well?*/
+	if(qh->programmed) {
+		//TODO I dont know wat to do with Control Endpoint ?
+		pilot4_cleanup_urb(urb, qh, urb->pipe & USB_DIR_IN, pilot_host);
+	}
+	urb = next_urb(qh);
+	if(urb != NULL) {
+		DBG_CLEANUP_PRINT("urb->unlinked %d status %d qh->type %d\n", urb->unlinked, urb->status, qh->type);
+		list_for_each_entry_safe_from(urb, tmp, &hep->urb_list, urb_list) {
+			if(qh->hw_ep->cur_urb && qh->hw_ep->cur_urb == urb) {
+				printk("active urb is being unlinked with out reset? can this be an error programmed %d line %d\n", qh->programmed, __LINE__);
+			}
+			pilot4_giveback(qh, urb, -ESHUTDOWN);
+		}
+	} else
+		DBG_CLEANUP_PRINT("No more pending urb's for this qh?\n");
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+#ifdef SAMPL
+		list_for_each_entry_safe_from(urb, tmp, &hep->urb_list, urb_list)\
+		for(tmp = list_entry(urb->urb_list.next, typeof(*urb), urb_list); &urb->urb_list != &hep->urb_list;
+			urb = tmp, tmp = list_entry(tmp->urb_list.next, typeof(*tmp), urb_list))
+#endif
+	print_giveback = 0;
+}
+
+static int pilot4_get_frame_number(struct usb_hcd *hcd)
+{
+	printk("%s\n", __FUNCTION__);
+	return -1;
+}
+
+static int pilot4_hc_start(struct usb_hcd *hcd)
+{
+	struct pilot_host_hcd	*pilot_host = hcd_to_pilot_hcd(hcd);
+	int status = - 1;
+	extern void pilot4_usb_enable_interrupts(struct pilot_host_hcd *);
+	printk("%s\n", __FUNCTION__);
+	no_endp_handshake = 0;
+	/* NOTE: pilot4_start() is called when the hub driver turns
+	 * on port power, or when (OTG) peripheral starts.
+	 */
+#ifdef CONFIG_SYSFS
+	status = sysfs_create_group(&pilot_host->dev->kobj, &p4_usb_host_attr_group);
+	if (status)
+		printk("sysfs_create_group failed with status %d\n", status);
+#endif
+	pilot4_usb_enable_interrupts(pilot_host);
+	hcd->state = HC_STATE_RUNNING;
+	pilot_host->port1_status = 0;
+	return 0;
+}
+
+static void pilot4_hc_stop(struct usb_hcd *hcd)
+{
+	printk("%s\n", __FUNCTION__);
+	hcd->state = HC_STATE_HALT;
+}
+
+static int pilot4_bus_suspend(struct usb_hcd *hcd)
+{
+	struct pilot_host_hcd	*pilot_host = hcd_to_pilot_hcd(hcd);
+	printk("%s\n", __FUNCTION__);
+
+	if (pilot_host->state == OTG_STATE_A_SUSPEND)
+		return 0;
+
+	if (is_host_active(pilot_host) && pilot_host->is_active) {
+		printk("trying to suspend as state %x is_active=%i\n",
+			pilot_host->state, pilot_host->is_active);
+		return -EBUSY;
+	} else
+		return 0;
+}
+
+static int pilot4_bus_resume(struct usb_hcd *hcd)
+{
+	printk("%s\n", __FUNCTION__);
+	/* resuming child port does the work */
+	return -1;
+}
+
+const struct hc_driver pilot4_hc_driver = {
+	.description		= "pilot4-hcd",
+	.product_desc		= "Pilot4 Usb host driver",
+	.hcd_priv_size		= sizeof(struct pilot_host_hcd),
+	.flags			= HCD_USB2 | HCD_MEMORY,
+
+	.start			= pilot4_hc_start,
+	.stop			= pilot4_hc_stop,
+
+	.get_frame_number	= pilot4_get_frame_number,
+
+	.urb_enqueue		= pilot4_urb_enqueue,
+	.urb_dequeue		= pilot4_urb_dequeue,
+	.endpoint_disable	= pilot4_ep_disable,
+
+	.hub_status_data	= pilot4_hub_status_data,
+	.hub_control		= pilot4_hub_control,
+	.bus_suspend		= pilot4_bus_suspend,
+	.bus_resume		= pilot4_bus_resume,
+};
+
+
