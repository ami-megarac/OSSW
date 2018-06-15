--- linux/drivers/usb/host/pilot/pilot4_usb_hc.c	1970-01-01 08:00:00.000000000 +0800
+++ linux.new/drivers/usb/host/pilot/pilot4_usb_hc.c	2016-12-19 16:08:10.963414544 +0800
@@ -0,0 +1,1155 @@
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
+#include <linux/platform_device.h>
+#include <linux/module.h>
+#include "pilot4_core.h"
+#ifdef DEBUG_START_FINISH
+#include <linux/proc_fs.h>
+#include <linux/seq_file.h>
+#endif
+
+extern int high_speed_device;
+static void pilot3_set_vbus(struct pilot_host_hcd *pilot_host, int is_on)
+{
+	uint8_t		devctl;
+	if (is_on) {
+		pilot_host->is_active = 1;
+		pilot_host->state = OTG_STATE_A_WAIT_VRISE;
+		devctl = pilot4_readb(pilot_host->base_addr, OTGCTRL);
+#ifdef EXTRA_DEBUG
+		printk("pilot3_set_vbus on devctl %x OTGSTATE %x\n", devctl, pilot4_readb(pilot_host->base_addr,OTGSTATE));
+#endif
+		devctl |= OTG_BUS_REQ;
+		devctl &= ~(OTG_ABUS_DROP);
+		pilot4_writeb(pilot_host->base_addr, OTGCTRL, 	devctl);
+	} else {
+		pilot_host->is_active = 0;
+		pilot_host->state = OTG_STATE_A_IDLE;
+		devctl = pilot4_readb(pilot_host->base_addr, OTGCTRL);
+#ifdef EXTRA_DEBUG
+		printk("pilot3_set_vbus off devctl %x OTGSTATE %x\n", devctl, pilot4_readb(pilot_host->base_addr,OTGSTATE));
+#endif
+		devctl |= OTG_ABUS_DROP;
+		devctl &= ~(OTG_BUS_REQ);
+		pilot4_writeb(pilot_host->base_addr, OTGCTRL, 	devctl);
+	}
+
+#ifdef EXTRA_DEBUG
+	printk("devctl %02x pilot_host->base_addr %p\n",pilot4_readb(pilot_host->base_addr, OTGCTRL), pilot_host->base_addr);
+	printk("pilot3_set_vbus is_on %d devctl %x OTGSTATE %x\n", is_on, devctl, pilot4_readb(pilot_host->base_addr,OTGSTATE));
+#endif
+}
+
+#define SE_PILOT_MAX_SUPPORTED_VECT 89
+extern int hcep0out_handler(struct pilot_host_hcd *);
+static int se_pilot_dummy_handler(void  * udc_context)
+{
+	printk(" Entered Unwanted %s \n", __FUNCTION__);
+	return 0;
+}
+int host_otgirq(void  *pilot_host);
+int (* se_pilot_host_vect_handler[256]) (void *);
+#define OTG_STATUS_CONN (1 << 1)
+int host_otgirq(void  *p4_host)
+{
+	#define idchangeirq (1<<5)
+	#define locsofirq (1<<2)
+	#define vbuserrirq (1<<3)
+	#define idleirq (1<<0)
+
+	uint8_t otg_irq = 0, otg_status = 0, otg_state = 0, otg_ctrl = 0;
+	struct pilot_host_hcd *pilot_host = (struct pilot_host_hcd*)p4_host;
+
+	otg_irq = pilot4_readb(pilot_host->base_addr, OTGIRQ);
+	otg_status = pilot4_readb(pilot_host->base_addr, OTGSTATUS);
+	otg_ctrl  = pilot4_readb(pilot_host->base_addr, OTGCTRL);
+	
+#ifdef EXTRA_DEBUG
+	printk("pilot_host->state %d otg_irq %x otg_status %x otg_ctrl %x\n", pilot_host->state, otg_irq, otg_status, otg_ctrl);
+#endif
+	pilot4_writeb(pilot_host->base_addr, OTGIRQ, otg_irq);//clear all interrupt's at once
+	if(otg_irq & idchangeirq) {
+		otg_status = pilot4_readb(pilot_host->base_addr, OTGSTATUS);
+		otg_state = pilot4_readb(pilot_host->base_addr, OTGSTATE);
+#ifdef EXTRA_DEBUG
+		printk("id is changed otg_status %x otg_state %x\n", otg_status, otg_state);
+#endif
+		//TODO Need to check if its disconnected or can we wait for a_idle interrupt?
+		switch(pilot_host->state) {
+			case OTG_STATE_A_IDLE:
+				printk("Unexpected tfor id to change from OTG_STATE_A_IDLE\n");
+				break;
+			case OTG_STATE_A_WAIT_VRISE:
+				pilot_host->state = OTG_STATE_A_WAIT_BCON;
+				break;
+			case OTG_STATE_A_WAIT_BCON:
+				pilot_host->state = OTG_STATE_A_WAIT_VFALL;//???
+				break;
+			/* If we are in host mode and we get a id change irq then as per otg spec i assumed that 
+			  * device is disconnected 
+			  */
+			case OTG_STATE_A_HOST:
+				if(otg_status & OTG_STATUS_CONN) {
+					printk("Device is connected to Host!?\n");
+				}else
+ {
+					printk("Device is not connected to Host!?\n");
+					printk("id is changed otg_status %x otg_state %x\n", otg_status, otg_state);
+				}
+				pilot_host->state = OTG_STATE_A_IDLE;
+				pilot_host->is_active = 0;
+			break;
+			default:
+				printk("Unhandled state %d %d\n",pilot_host->state, __LINE__);
+			break;
+		}
+	}
+
+	if(otg_irq & locsofirq) {//Can we assume this as a connect?
+		struct usb_hcd *hcd = pilot_pilot4_to_hcd(pilot_host);
+		otg_status = pilot4_readb(pilot_host->base_addr, OTGSTATUS);
+		otg_state = pilot4_readb(pilot_host->base_addr, OTGSTATE);
+#ifdef EXTRA_DEBUG
+		printk("Entered A_HOST otg_status %x otg_state %x\n", otg_status, otg_state);
+#endif
+		if(otg_status & 1){
+#ifdef EXTRA_DEBUG
+			printk("mini-A plug was connected and the core becomes an A-device\n");
+#endif
+		}
+		if(otg_status & OTG_STATUS_CONN) {
+			printk("Device is connected to Host!?\n");
+		}else {
+			printk("Device is not connected to Host!?\n");
+			printk("Entered A_HOST otg_status %x otg_state %x\n", otg_status, otg_state);
+			pilot3_set_vbus(pilot_host, 0);
+			pilot_host->port1_status &=
+				~(USB_PORT_STAT_CONNECTION
+				| USB_PORT_STAT_ENABLE
+				| USB_PORT_STAT_LOW_SPEED
+				| USB_PORT_STAT_HIGH_SPEED
+				| USB_PORT_STAT_TEST
+				);
+			pilot_host->speed = USB_SPEED_UNKNOWN;
+			pilot_host->port1_status |= (USB_PORT_STAT_C_CONNECTION << 16);
+			usb_hcd_poll_rh_status(hcd);
+			pilot3_set_vbus(pilot_host, 1);
+			goto chknext;
+		}
+		switch(pilot_host->state) {
+			case OTG_STATE_A_IDLE:
+				printk("Unexpected to goto A_HOST from OTG_STATE_A_IDLE?\n");
+				break;
+			case OTG_STATE_A_WAIT_VRISE:
+				printk("Entering A_HOST from OTG_STATE_A_WAIT_VRISE\n");
+				break;
+			case OTG_STATE_A_WAIT_BCON:
+				printk("Entering A_HOST from OTG_STATE_A_WAIT_BCON\n");
+				break;
+			default:
+				printk("Unhandled state %d %d\n",pilot_host->state, __LINE__);
+			break;
+		}
+		pilot_host->state = OTG_STATE_A_HOST;
+		pilot_host->is_active = 1;
+		pilot_host->port1_status &= ~(USB_PORT_STAT_LOW_SPEED
+					|USB_PORT_STAT_HIGH_SPEED
+					|USB_PORT_STAT_ENABLE
+					);
+		pilot_host->port1_status |= USB_PORT_STAT_CONNECTION
+					|(USB_PORT_STAT_C_CONNECTION << 16);
+		usb_hcd_poll_rh_status(hcd);
+	}
+chknext:
+	
+	if(otg_irq & idleirq) {
+		printk("OTG Has entered a_idle or b_idle state?no idea wat to do pilot_host->state %d\n", pilot_host->state);
+		otg_state = pilot4_readb(pilot_host->base_addr, OTGSTATE);
+#ifdef EXTRA_DEBUG
+		printk("OTG IRQ End otg_state %x\n", otg_state);
+		printk("Switch on (not off for now )bus\n");
+#endif
+		pilot3_set_vbus(pilot_host, 1);
+	}
+	if(otg_irq & vbuserrirq) {
+		printk("vbuserrirq error!!\n");
+		pilot3_set_vbus(pilot_host, 0);
+		udelay(200);
+		pilot3_set_vbus(pilot_host, 1);
+	}
+	otg_state = pilot4_readb(pilot_host->base_addr, OTGSTATE);
+#ifdef EXTRA_DEBUG
+	printk("OTG IRQ End otg_state %x\n", otg_state);
+#endif
+	return IRQ_HANDLED;
+}
+
+static irqreturn_t se_pilot_udc_host_interrupt(int irq, void *p4_host)
+{
+	unsigned short io_vect_val;
+	unsigned char int_sts;
+	int 	retval;
+	unsigned int int_en;
+	struct pilot_host_hcd *pilot_host = p4_host;
+	int_sts = *(uint8_t *)IO_ADDRESS(SE_SYS_CLK_BASE+ 0x6b);
+	int_en = *(uint8_t *)(pilot_host->base_addr + USB2_INT_ENABLE_OFFSET);
+	if(1 || int_sts & BIT_USB2_INT_USB) {
+		io_vect_val = *(u16 *)(pilot_host->base_addr+ HCIVECT);
+		retval = se_pilot_host_vect_handler[io_vect_val](((unsigned int *)pilot_host));
+	}
+
+	return IRQ_HANDLED;
+}
+struct pilot_host_hcd *pilot_host_udc = NULL, *pilot_host = NULL;
+static unsigned char host_out_ctrl_reg[16 + 1] = {0, 0xE, 0x16, 0x1E, 0x26, 0x2E, 0x36, 0x3E, 0x46,
+									0x4E, 0x56, 0x5E, 0x66, 0x6E, 0x76, 0x7E};
+static unsigned char host_in_ctrl_reg[16 + 1] = {0, 0xA, 0x12, 0x1A, 0x22, 0x2A, 0x32, 0x3A, 0x42,
+									0x4A, 0x52, 0x5A, 0x62, 0x6A, 0x72, 0x7A};
+static unsigned char host_in_cs_reg[16 + 1] = {0, 0xB, 0x13, 0x1B, 0x23, 0x2B, 0x33, 0x3B, 0x43,
+									0x4B, 0x53, 0x5B, 0x63, 0x6B, 0x73, 0x7B};
+static unsigned char host_out_cs_reg[16 + 1] = {0, 0xF, 0x17, 0x1F, 0x27, 0x2F, 0x37, 0x3F, 0x47,
+									0x4F, 0x57, 0x5F, 0x67, 0x6F, 0x77, 0x7F};
+static unsigned char host_fifo_reg[16 + 1] = {0, 0x84, 0x88, 0x8C, 0x90, 0x94, 0x98, 0x9C, 0xA0,
+								0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC};
+static unsigned char host_out_byte_cnt_reg[16 + 1] = {0, 0xC, 0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C, 0x44,
+									0x4C, 0x54, 0x5C, 0x64, 0x6C, 0x74, 0x7C};
+static unsigned char host_in_byte_cnt_reg[16 + 1] = {0, 0x8, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 
+									0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78};
+static unsigned char host_hcinxctrl[16 + 1] = {0, 0xC6 , 0xCA, 0xCE, 0xD2, 0xD6 , 0xDA, 0xDE, 0xE2, 0xE6, 0xEA,
+								0xEE, 0xF2, 0xF6, 0xFA, 0xFE};
+static unsigned char host_hcoutxctrl[16 + 1] = {0, 0xC4 , 0xC8, 0xCC, 0xD0, 0xD4 , 0xD8, 0xDC, 0xE0, 0xE4, 0xE8,
+								0xEC, 0xF0, 0xF4, 0xF8, 0xFC};
+static unsigned int hcoutxerr[] = {0x0C1, 0x0C5, 0x0C9 ,0x0CD, 0x0D1, 0x0D5, 0x0D9, 0x0DD, 0x0E1 ,0x0E5 ,0x0E9,0x0ED ,0x0F1,0x0F5,0x0F9,0x0FD};
+static unsigned int hcinxerr[] = {0x0C3, 0x0C7, 0x0CB, 0x0CF, 0x0D3, 0x0D7, 0x0Db, 0x0Df, 0x0E3, 0x0E7, 0x0Eb, 0x0EF, 0x0F3, 0x0F7, 0x0FB, 0x0FF};
+static unsigned short hcinxmaxpck[16 + 1] = {0};
+static unsigned short hcoutxmaxpck[16 + 1] = {0};
+
+#ifndef DYNAMIC_CONFIGURE_EP
+void configure_endpoints(int speed)
+{
+	struct pilot4_hw_ep * ep;
+	int i = 0, bulk_ep_size = 0, intr_ep_size = 0, ep0_size;
+	extern int endpoint_reset(struct pilot4_hw_ep * , unsigned int is_out);
+#define LOW_SPEED 0
+#define FULL_SPEED 1
+#define HIGH_SPEED 2
+#ifdef EXTRA_DEBUG
+	printk("configure_endpoints speed %d\n", speed);
+#endif
+	switch(speed) {
+		case USB_SPEED_LOW:
+			intr_ep_size  = ep0_size = 8;
+			break;
+		case USB_SPEED_FULL:
+			ep0_size = 64;
+			intr_ep_size  = 64;
+			bulk_ep_size = 64;
+			break;
+		case USB_SPEED_HIGH:
+			ep0_size = 64;
+			intr_ep_size  = 64;
+			bulk_ep_size = 512;
+			break;
+	}
+	/* Out EP's */
+	for(i = 0;i < 16;i++) {
+		ep = &pilot_host_udc->endpoints[i];
+#if 0
+		printk("i %d bmAttributes %x maxpacket %d hcoutmaxpck %x\n",
+			i , ep->bmAttributes, ep->maxpacket, ep->hcoutmaxpck);
+#endif
+		if(ep->maxpacket && ep->hcoutmaxpck) {
+			if(ep->bmAttributes == USB_ENDPOINT_XFER_INT) {
+				printk("OUT INTRIN!!!\n");
+				*((volatile unsigned short*)(ep->hcoutmaxpck)) = intr_ep_size;
+				ep->maxpacket = intr_ep_size;
+				(void)endpoint_reset(ep, 1);
+			}
+			if(ep->bmAttributes == USB_ENDPOINT_XFER_BULK && speed >= USB_SPEED_FULL) {
+				*((volatile unsigned short*)(ep->hcoutmaxpck)) = bulk_ep_size;
+				(void)endpoint_reset(ep, 1);
+				ep->maxpacket = bulk_ep_size;
+			}
+			if(ep->bmAttributes == USB_ENDPOINT_XFER_CONTROL) {
+				*((volatile unsigned short*)(ep->hcoutmaxpck)) = ep0_size;
+				ep->maxpacket = ep0_size;
+			}
+		}
+	}
+	
+	/* In EP's */
+	for(i = 16;i < 32;i++) {
+		ep = &pilot_host_udc->endpoints[i];
+		if(ep->maxpacket && ep->hcinmaxpck) {
+			if(ep->bmAttributes == USB_ENDPOINT_XFER_INT) {
+				*((volatile unsigned short*)(ep->hcinmaxpck)) = intr_ep_size;
+				(void)endpoint_reset(ep, 0);
+				ep->maxpacket = intr_ep_size;
+			}
+			if(ep->bmAttributes == USB_ENDPOINT_XFER_BULK && speed >= USB_SPEED_FULL) {
+				*((volatile unsigned short*)(ep->hcinmaxpck)) = bulk_ep_size;
+				(void)endpoint_reset(ep, 0);
+				ep->maxpacket = bulk_ep_size;
+			}
+			if(ep->bmAttributes == USB_ENDPOINT_XFER_CONTROL) {
+				*((volatile unsigned short*)(ep->hcinmaxpck)) = ep0_size;
+				ep->maxpacket = ep0_size;
+			}
+		}
+	}
+}
+#else
+void configure_ep0(int speed)
+{
+	struct pilot4_hw_ep * ep;
+	int ep0_size = 0;
+#define LOW_SPEED 0
+#define FULL_SPEED 1
+#define HIGH_SPEED 2
+#ifdef EXTRA_DEBUG
+	printk("configure_ep0 speed %d\n", speed);
+#endif
+	switch(speed) {
+		case USB_SPEED_LOW:
+			ep0_size = 8;
+			break;
+		case USB_SPEED_FULL:
+			ep0_size = 64;
+			break;
+		case USB_SPEED_HIGH:
+			ep0_size = 64;
+			break;
+	}
+	ep = &pilot_host_udc->endpoints[0];
+	if(ep->bmAttributes == USB_ENDPOINT_XFER_CONTROL) {
+		*((volatile unsigned short*)(ep->hcoutmaxpck)) = ep0_size;
+		ep->maxpacket = ep0_size;
+	} else
+		printk("\nNon EP0?!!!\n");
+
+}
+EXPORT_SYMBOL(configure_ep0);
+#endif
+#ifdef DYNAMIC_CONFIGURE_EP
+struct ep_details {
+	int dev_addr;//EP associated with dev_addr
+	unsigned char free;
+	unsigned char type;
+	__le16 size;
+};
+struct ep_details usb_host_free_in[8];
+struct ep_details usb_host_free_out[8];
+#endif
+extern int total_bulk_in_eps, total_bulk_out_eps, total_intr_in_eps;;
+extern int (*pilot_require_ep)(__le16 size, __u8 type, int dir, int dev_addr, char*);
+extern void (*pilot_free_ep)(int dev_addr, char*);
+
+static unsigned
+se_pilot_host_ep_setup(char *name, uint8_t addr, uint8_t type, unsigned maxp)
+{
+	int ind = 0;
+	struct pilot4_hw_ep * ep;
+	extern int endpoint_reset(struct pilot4_hw_ep * , unsigned int is_out);
+	if((addr & USB_DIR_IN)) {
+	    ind = 0x10 | (addr & 0x7f);
+	}else {
+	    ind = (addr & 0x7f);
+	}
+
+	ep = &pilot_host_udc->endpoints[ind];
+	if(addr == 0) {// EP-0
+		ep->ep_data_buf_reg = (unsigned int) pilot_host_udc->base_addr + HCEP0INDAT;
+		ep->ep_byte_cnt_reg = (unsigned int) pilot_host_udc->base_addr + HCIN0BC;
+		ep->ep_cs_reg = (unsigned int) pilot_host_udc->base_addr + HCEP0CS;
+		ep->ep_num = 0;
+		ep->ep_data_buf_reg1 = (unsigned int) pilot_host_udc->base_addr + HCEP0OUTDAT;
+		ep->ep_byte_cnt_reg1 = (unsigned int) pilot_host_udc->base_addr + HCOUT0BC;
+		ep->ep_cs_reg1 = ep->ep_cs_reg;
+		ep->hcinctrl = (unsigned int) pilot_host_udc->base_addr + HCEP0CTRL;
+		ep->hcoutctrl = (unsigned int) pilot_host_udc->base_addr + HCEP0CTRL;
+		ep->hcouterr = (unsigned int) pilot_host_udc->base_addr + hcoutxerr[(addr & 0x7f)];
+		ep->hcinerr = (unsigned int) pilot_host_udc->base_addr + hcinxerr[(addr & 0x7f)];
+		*((volatile unsigned char*)(ep->hcinctrl)) = (unsigned char)0;
+		ep->hcinmaxpck  = (unsigned int) pilot_host_udc->base_addr + HCIN0MAXPCK;
+		ep->hcoutmaxpck = ep->hcinmaxpck;
+		*((volatile unsigned char*)(ep->hcinmaxpck)) = maxp;
+		ep->func_addr_reg = (unsigned int) pilot_host_udc->base_addr + HOST_FUNC_ADDR;
+#ifdef EXTRA_DEBUG
+		printk("epsetup 0 hw_ep %p type %d ep_num %d\n", ep, type, ep->ep_num);
+#endif
+	} else {
+		if(addr & USB_DIR_IN) {
+			ep->ep_data_buf_reg = (unsigned int) pilot_host_udc->base_addr + host_fifo_reg[(addr & 0x7f)];
+			ep->ep_cs_reg = (unsigned int) pilot_host_udc->base_addr + host_in_cs_reg[(addr & 0x7f)];
+			ep->ep_byte_cnt_reg = (unsigned int) pilot_host_udc->base_addr + host_in_byte_cnt_reg[(addr & 0x7f)];;
+			ep->ep_num = (addr & 0x7f);
+			ep->hcinctrl = (unsigned int) pilot_host_udc->base_addr + host_hcinxctrl[(addr & 0x7f)];
+			ep->hc_in_con = (unsigned int) pilot_host_udc->base_addr + host_in_ctrl_reg[(addr & 0x7f)];
+			ep->hcinmaxpck  = (unsigned int) pilot_host_udc->base_addr + hcinxmaxpck[(addr & 0x7f)];
+			*((volatile unsigned short*)(ep->hcinmaxpck)) = maxp;//initialize max packet size for this EP
+			ep->hcinerr = (unsigned int) pilot_host_udc->base_addr + hcinxerr[(addr & 0x7f)];
+			ep->func_addr_reg = (unsigned int) (pilot_host_udc->base_addr + 0x600 + (addr & 0x7f) - 1);
+			//Single Buffering, IN, type, endpoint is valid
+			*((volatile unsigned char*)(ep->hc_in_con)) = (0 |(type << 2) | (1<<7));
+			(void)endpoint_reset(ep, 0);
+			if(type == USB_ENDPOINT_XFER_BULK) {
+				pilot_host_udc->bulk_in[(addr & 0x7f)] = ep;
+				total_bulk_in_eps++;
+			}
+			if(type == USB_ENDPOINT_XFER_INT) {
+				pilot_host_udc->intr_in[(addr & 0x7f)] = ep;
+				total_intr_in_eps++;
+			}
+#ifdef EXTRA_DEBUG
+			printk("epsetup in hw_ep %p type %d ep_num %d\n", ep, type, ep->ep_num);			
+#endif
+#ifdef DYNAMIC_CONFIGURE_EP
+			usb_host_free_in[ep->ep_num].free = 0;
+			usb_host_free_in[ep->ep_num].type = type;
+			usb_host_free_in[ep->ep_num].size = maxp;
+#endif
+
+		} else {
+			ep->ep_data_buf_reg = (unsigned int) pilot_host_udc->base_addr +  host_fifo_reg[(addr & 0x7f)];
+			ep->ep_cs_reg = (unsigned int) pilot_host_udc->base_addr + host_out_cs_reg[(addr & 0x7f)];
+			ep->ep_byte_cnt_reg = (unsigned int) pilot_host_udc->base_addr + host_out_byte_cnt_reg[(addr & 0x7f)];;
+			ep->ep_num = (addr & 0x7f);
+			ep->hcoutctrl = (unsigned int) pilot_host_udc->base_addr + host_hcoutxctrl[(addr & 0x7f)];
+			ep->hc_out_con = (unsigned int) pilot_host_udc->base_addr + host_out_ctrl_reg[(addr & 0x7f)];
+			ep->hcoutmaxpck  = (unsigned int) pilot_host_udc->base_addr + hcoutxmaxpck[(addr & 0x7f)];
+			*((volatile unsigned short*)(ep->hcoutmaxpck)) = maxp;//initialize max packet size for this EP
+			ep->hcouterr = (unsigned int) pilot_host_udc->base_addr + hcoutxerr[(addr & 0x7f)];
+			ep->func_addr_reg = (unsigned int) (pilot_host_udc->base_addr + 0x608 + (addr & 0x7f) - 1);
+			//Single Buffering, OUT, type, endpoint is valid
+			*((volatile unsigned char*)(ep->hc_out_con)) = (0 |(type << 2) | (1<<7));
+			(void)endpoint_reset(ep, 1);
+			if(type == USB_ENDPOINT_XFER_BULK) {
+				pilot_host_udc->bulk_out[(addr & 0x7f)] = ep;
+				total_bulk_out_eps++;
+			}
+			if(type == USB_ENDPOINT_XFER_INT)
+				pilot_host_udc->intr_out[(addr & 0x7f)] = ep;
+#ifdef EXTRA_DEBUG
+			printk("epsetup OUT hw_ep %p type %d ep_num %d\n", ep, type, ep->ep_num);
+#endif
+#ifdef DYNAMIC_CONFIGURE_EP
+			usb_host_free_out[ep->ep_num].free = 0;
+			usb_host_free_out[ep->ep_num].type = type;
+			usb_host_free_out[ep->ep_num].size = maxp;
+#endif
+		}
+	}
+	BUG_ON(strlen(name) >= sizeof ep->name);
+	strlcpy(ep->name, name, sizeof ep->name);
+	INIT_LIST_HEAD(&ep->queue);
+	ep->bEndpointAddress = addr;
+	ep->bmAttributes = type;
+	ep->double_buf = 0;
+	ep->maxpacket = maxp;
+	ep->last_dev_addr = -1;//Currently Not in use
+#ifdef SW_WORKAROUND
+	init_timer(&ep->intr_ep_timer);
+#endif
+#ifdef DEBUG_START_FINISH
+	init_timer(&ep->debug_timer);
+#endif
+	return 0;
+}
+
+static int host_ubs_reset(void * udc_context)
+{
+	struct pilot_host_hcd *pilot_host = udc_context;
+	*(u8 *)(pilot_host->base_addr + SE_PILOT_USBIRQ_REG_OFFSET) = (u8)(1<<4);
+#ifdef EXTRA_DEBUG
+	printk("host_ubs_reset intr\n");
+#endif
+	return 0;
+}
+
+static int host_high_speed(void  * udc_context)
+{
+	struct pilot_host_hcd *pilot_host = udc_context;
+	*(u8 *)(pilot_host->base_addr + SE_PILOT_USBIRQ_REG_OFFSET) = (u8)(1<<5);
+	printk("host_high_speed intr\n");
+	high_speed_device = 1;
+	return 0;
+}
+
+
+static void install_vector(void)
+{
+	int i = 0;
+	extern int hcep0in_handler(struct pilot_host_hcd *);
+	extern int hcep0out_handler(struct pilot_host_hcd *);
+	extern int hcin0err_handler(struct pilot_host_hcd *);
+	extern int hcout0err_handler(struct pilot_host_hcd *);
+
+	extern int hcout1_handler(struct pilot_host_hcd *);
+	extern int hcout2_handler(struct pilot_host_hcd *);
+	extern int hcout3_handler(struct pilot_host_hcd *);
+	extern int hcout4_handler(struct pilot_host_hcd *);
+	extern int hcout5_handler(struct pilot_host_hcd *);
+	extern int hcout6_handler(struct pilot_host_hcd *);
+	extern int hcout7_handler(struct pilot_host_hcd *);
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+	extern int hcout8_bulk_ep0_handler(struct pilot_host_hcd *);
+#endif
+	extern int hcout1err_handler(struct pilot_host_hcd *);
+	extern int hcout2err_handler(struct pilot_host_hcd *);
+	extern int hcout3err_handler(struct pilot_host_hcd *);
+	extern int hcout4err_handler(struct pilot_host_hcd *);
+	extern int hcout5err_handler(struct pilot_host_hcd *);
+	extern int hcout6err_handler(struct pilot_host_hcd *);
+	extern int hcout7err_handler(struct pilot_host_hcd *);
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+	extern int hcout8err_bulk_ep0_handler(struct pilot_host_hcd *);
+#endif
+	extern int hcin1err_handler(struct pilot_host_hcd *);
+	extern int hcin2err_handler(struct pilot_host_hcd *);
+	extern int hcin3err_handler(struct pilot_host_hcd *);
+	extern int hcin4err_handler(struct pilot_host_hcd *);
+	extern int hcin5err_handler(struct pilot_host_hcd *);
+	extern int hcin6err_handler(struct pilot_host_hcd *);
+	extern int hcin7err_handler(struct pilot_host_hcd *);
+
+	extern int hcin1_handler(struct pilot_host_hcd *);
+	extern int hcin3_handler(struct pilot_host_hcd *);
+	extern int hcin2_handler(struct pilot_host_hcd *);
+	extern int hcin4_handler(struct pilot_host_hcd *);
+	extern int hcin5_handler(struct pilot_host_hcd *);
+	extern int hcin6_handler(struct pilot_host_hcd *);
+	extern int hcin7_handler(struct pilot_host_hcd *);
+	
+	for(i = 0;i < 256;i ++)
+		se_pilot_host_vect_handler[i] = (int (*)(void *))se_pilot_dummy_handler;
+	se_pilot_host_vect_handler[0xD8] = (int (*)(void *))host_otgirq;
+	se_pilot_host_vect_handler[0x10] = (int (*)(void *))host_ubs_reset;
+	se_pilot_host_vect_handler[0x14] = (int (*)(void *))host_high_speed;
+
+	se_pilot_host_vect_handler[0x16] = (int (*)(void *))hcout0err_handler;
+	se_pilot_host_vect_handler[0x18] = (int (*)(void *))hcep0out_handler;
+	se_pilot_host_vect_handler[0x1A] = (int (*)(void *))hcin0err_handler;
+	se_pilot_host_vect_handler[0x1C] = (int (*)(void *))hcep0in_handler;
+
+	se_pilot_host_vect_handler[0x22] =(int (*)(void *))hcout1err_handler;
+	se_pilot_host_vect_handler[0x24] =(int (*)(void *))hcout1_handler;
+
+	se_pilot_host_vect_handler[0x26] =(int (*)(void *))hcin1err_handler;//INTR 
+	se_pilot_host_vect_handler[0x28] =(int (*)(void *))hcin1_handler;//INTR 
+
+	se_pilot_host_vect_handler[0x2E] =(int (*)(void *))hcout2err_handler;
+	se_pilot_host_vect_handler[0x30] =(int (*)(void *))hcout2_handler;
+
+
+	se_pilot_host_vect_handler[0x32] = (int (*)(void *))hcin2err_handler;//BULK
+	se_pilot_host_vect_handler[0x34] = (int (*)(void *))hcin2_handler;//BULK
+
+	se_pilot_host_vect_handler[0x3A] = (int (*)(void *))hcout3err_handler;
+	se_pilot_host_vect_handler[0x3C] = (int (*)(void *))hcout3_handler;
+
+	se_pilot_host_vect_handler[0x3E] = (int (*)(void *))hcin3err_handler;//BULK
+	se_pilot_host_vect_handler[0x40] = (int (*)(void *))hcin3_handler;//BULK
+
+	se_pilot_host_vect_handler[0x46] = (int (*)(void *))hcout4err_handler;//BULK
+	se_pilot_host_vect_handler[0x48] = (int (*)(void *))hcout4_handler;//BULK
+
+	se_pilot_host_vect_handler[0x4A] =(int (*)(void *))hcin4err_handler;//INTR
+	se_pilot_host_vect_handler[0x4C] =(int (*)(void *))hcin4_handler;//INTR
+
+	se_pilot_host_vect_handler[0x52] = (int (*)(void *))hcout5err_handler;//BULK
+	se_pilot_host_vect_handler[0x54] = (int (*)(void *))hcout5_handler;//BULK
+
+	se_pilot_host_vect_handler[0x5E] = (int (*)(void *))hcout6err_handler;//BULK
+	se_pilot_host_vect_handler[0x60] = (int (*)(void *))hcout6_handler;//BULK
+
+	se_pilot_host_vect_handler[0x6A] = (int (*)(void *))hcout7err_handler;//BULK
+	se_pilot_host_vect_handler[0x6C] = (int (*)(void *))hcout7_handler;//BULK
+
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+	se_pilot_host_vect_handler[0x76] = (int (*)(void *))hcout8err_bulk_ep0_handler;//BULK
+	se_pilot_host_vect_handler[0x78] = (int (*)(void *))hcout8_bulk_ep0_handler;//BULK
+#endif
+	se_pilot_host_vect_handler[0x56] = (int (*)(void *))hcin5err_handler;//BULK
+	se_pilot_host_vect_handler[0x58] = (int (*)(void *))hcin5_handler;//BULK
+
+	se_pilot_host_vect_handler[0x62] = (int (*)(void *))hcin6err_handler;//BULK
+	se_pilot_host_vect_handler[0x64] = (int (*)(void *))hcin6_handler;//BULK
+
+	se_pilot_host_vect_handler[0x6E] = (int (*)(void *))hcin7err_handler;//INTR
+	se_pilot_host_vect_handler[0x70] = (int (*)(void *))hcin7_handler;//INTR
+}
+
+#ifdef TEST_PORT_CONNECT
+int test_only = 0;
+static int what_to_do = 0;
+#define SIMULATE_CONNECT 1
+static struct timer_list	test_timer;
+static void test_timer_func (unsigned long data)
+{
+	struct pilot_host_hcd *pilot_host = (struct pilot_host_hcd *) data;
+	struct usb_hcd *hcd = pilot_pilot4_to_hcd(pilot_host);
+	printk("test_timer_func uses_new_polling %d what_to_do %x\n",
+		hcd->uses_new_polling,what_to_do);
+	if(what_to_do & SIMULATE_CONNECT) {
+		what_to_do &= ~(SIMULATE_CONNECT);
+		pilot_host->state = OTG_STATE_A_HOST;
+		pilot_host->is_active = 1;
+		pilot_host->xceiv.default_a = 1;
+		pilot_host->port1_status &= ~(USB_PORT_STAT_LOW_SPEED
+					|USB_PORT_STAT_HIGH_SPEED
+					|USB_PORT_STAT_ENABLE
+					);
+		pilot_host->port1_status |= USB_PORT_STAT_CONNECTION
+					|(USB_PORT_STAT_C_CONNECTION << 16);
+		high_speed_device = 1;
+		test_only = 1;
+		usb_hcd_poll_rh_status(hcd);
+	}
+}
+
+#endif
+#ifdef DYNAMIC_CONFIGURE_EP
+#ifdef HOST_DEBUG_STATS
+static DECLARE_BITMAP(device_addr_map, 128);
+static char *ep_name[4] = {"control", "isoc", "bulk", "Interrupt"};
+
+static void populate_ep_stats(struct seq_file *m, struct pilot4_hw_ep * ep)
+{
+	if(ep == NULL) {
+		printk("populate_ep_stats ep == NULL\n");
+		return;
+	}
+	if(ep->bmAttributes > 3)
+		printk("ISSUE ep->bmAttributes %d > 3\n", ep->bmAttributes);
+	seq_printf(m, "Devaddr:%d %s %s ep epnum %d\n",
+		ep->dev_addr, ep_name[ep->bmAttributes],
+		(ep->bEndpointAddress & USB_DIR_IN)?"IN":"OUT", ep->ep_num);
+	seq_printf(m, "total_xfers:%u totalxferlegnth:%llu completed_length:%llu\n",
+		ep->total_urbs, ep->total_transfer_buffer_length, ep->completed_length);
+	seq_printf(m, "successes:%u failures:%u\n", ep->success, ep->errors);
+	if(ep->success) {
+		seq_printf(m, "AverageSuccesstime:%u msecs MaxSuccessDuration:%u msecs total_success_duration %u ", 
+			jiffies_to_msecs((ep->total_success_duration/ep->success)),
+			jiffies_to_msecs(ep->max_success_duration), ep->total_success_duration);
+		//Reset so that we will measure each max every time we read it depends on requirement though
+		ep->max_success_duration = 0;
+	}
+	if(ep->errors) {
+		seq_printf(m, "AverageFailuretime:%u msecs MaxFailureDuration:%u msecs", 
+			jiffies_to_msecs((ep->total_fail_duration/ep->errors)), jiffies_to_msecs(ep->max_failure_duration));
+		//Reset so that we will measure each max every time we read it depends on requirement though
+		ep->max_failure_duration = 0;
+	}
+	seq_puts(m, "\n");
+}
+
+
+static int proc_hcd_dev_show(struct seq_file *m, void *unused_v)
+{
+	int i = 0;
+	struct pilot4_hw_ep * ep;
+	unsigned long	flags;
+	u8 dev_addr = (u8)(u32)(m->private);
+	spin_lock_irqsave(&pilot_host_udc->lock, flags);
+	for(i = 1;i < 8;i++) {
+		if(usb_host_free_in[i].dev_addr == dev_addr && pilot_host_udc->bulk_in[i]) {
+			ep = pilot_host_udc->bulk_in[i];
+			populate_ep_stats(m, ep);
+		}
+		if(usb_host_free_out[i].dev_addr == dev_addr && pilot_host_udc->bulk_out[i]) {
+			ep = pilot_host_udc->bulk_out[i];
+			populate_ep_stats(m, ep);
+		}
+		if(usb_host_free_in[i].dev_addr == dev_addr && pilot_host_udc->intr_in[i]) {
+			ep = pilot_host_udc->intr_in[i];
+			populate_ep_stats(m, ep);
+		}
+		if(usb_host_free_out[i].dev_addr == dev_addr && pilot_host_udc->intr_out[i]) {
+			ep = pilot_host_udc->intr_out[i];
+			populate_ep_stats(m, ep);
+		}
+	}
+	spin_unlock_irqrestore(&pilot_host_udc->lock, flags);
+	return 0;
+}
+static int proc_hcd_dev_open(struct inode *inode, struct file *file)
+{
+	return single_open(file, proc_hcd_dev_show, PDE_DATA(inode));
+}
+
+static const struct file_operations proc_device_ops = {
+	.owner		= THIS_MODULE,
+	.open		= proc_hcd_dev_open,
+	.read		= seq_read,
+	.release	= single_release,
+};
+
+#endif
+
+#ifdef DEBUG_START_FINISH
+int print_ep_detail_based_on_devaddr(int dev_addr, char *manufacturer, int reason)
+{
+	int i = 0;
+	struct pilot4_hw_ep * ep;
+	unsigned long	flags;
+	spin_lock_irqsave(&pilot_host_udc->lock, flags);
+	printk("%s In EP details reason %d:\n", manufacturer, reason);
+
+	for(i = 1;i < 8;i++) {
+		if(usb_host_free_in[i].dev_addr == dev_addr && pilot_host_udc->bulk_in[i]) {
+			ep = pilot_host_udc->bulk_in[i];
+			if(ep->cur_urb) {
+				printk("urb %p status %d curtime %lu starttime %llu\n", ep->cur_urb, ep->cur_urb->status, jiffies, ep->start_time);
+				printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+					ep->cur_urb->actual_length, ep->cur_urb->transfer_buffer_length, ep->cur_urb, ep->cur_qh);
+				printk("epnum %d epcs 0x%x addr %x programmed %d\n",
+					ep->ep_num, *((volatile unsigned char*)(ep->ep_cs_reg)), 
+					(ep->cur_qh)?ep->cur_qh->addr_reg:-1, (ep->cur_qh)?ep->cur_qh->programmed:-1);
+			}
+		}
+		if(usb_host_free_in[i].dev_addr == dev_addr && pilot_host_udc->intr_in[i]) {
+			ep = pilot_host_udc->intr_in[i];
+			if(ep->cur_urb) {
+				printk("intrin:urb %p status %d curtime %lu starttime %llu\n", ep->cur_urb, ep->cur_urb->status, jiffies, ep->start_time);
+				printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+					ep->cur_urb->actual_length, ep->cur_urb->transfer_buffer_length, ep->cur_urb, ep->cur_qh);
+				printk("epnum %d epcs 0x%x addr %x programmed %d\n",
+					ep->ep_num, *((volatile unsigned char*)(ep->ep_cs_reg)), 
+					(ep->cur_qh)?ep->cur_qh->addr_reg:-1, (ep->cur_qh)?ep->cur_qh->programmed:-1);
+			}
+		}
+	}
+	printk("%s Out EP details reason %d:\n", manufacturer, reason);
+	for(i = 1;i < 8;i++) {
+		if(usb_host_free_out[i].dev_addr == dev_addr && pilot_host_udc->bulk_out[i]) {
+			ep = pilot_host_udc->bulk_out[i];
+			if(ep->cur_urb) {
+				printk("urb %p status %d curtime %lu starttime %llu\n", ep->cur_urb, ep->cur_urb->status, jiffies, ep->start_time);
+				printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+					ep->cur_urb->actual_length, ep->cur_urb->transfer_buffer_length, ep->cur_urb, ep->cur_qh);
+				printk("epnum %d epcs 0x%x addr %x programmed %d\n",
+					ep->ep_num, *((volatile unsigned char*)(ep->ep_cs_reg)), 
+					(ep->cur_qh)?ep->cur_qh->addr_reg:-1, (ep->cur_qh)?ep->cur_qh->programmed:-1);
+			}
+
+		}
+		if(usb_host_free_out[i].dev_addr == dev_addr && pilot_host_udc->intr_out[i]) {
+			ep = pilot_host_udc->intr_out[i];
+			if(ep->cur_urb) {
+				printk("urb %p status %d curtime %lu starttime %llu\n", ep->cur_urb, ep->cur_urb->status, jiffies, ep->start_time);
+				printk("actual length %d transferbuflength %d urb %p cur_qh %p\n",
+					ep->cur_urb->actual_length, ep->cur_urb->transfer_buffer_length, ep->cur_urb, ep->cur_qh);
+				printk("epnum %d epcs 0x%x addr %x programmed %d\n",
+					ep->ep_num, *((volatile unsigned char*)(ep->ep_cs_reg)), 
+					(ep->cur_qh)?ep->cur_qh->addr_reg:-1, (ep->cur_qh)?ep->cur_qh->programmed:-1);
+			}
+
+		}
+	}
+	spin_unlock_irqrestore(&pilot_host_udc->lock, flags);
+	return 0;
+}
+EXPORT_SYMBOL(print_ep_detail_based_on_devaddr);
+#else
+int print_ep_detail_based_on_devaddr(int dev_addr, char *manufacturer, int reason)
+{
+	return 0;
+}
+#endif
+static int require_ep(__le16 size, __u8 type, int dir, int dev_addr, char *manufacturer)
+{
+	int i = 0;
+#ifdef HOST_DEBUG_STATS
+	char buf[128];
+	int ind = 0;
+	struct pilot4_hw_ep * ep;
+#endif
+#ifdef EXTRA_DEBUG
+	printk("require_ep size %d type %d dir %x dev_addr %d\n", size, type, dir, dev_addr);
+#endif
+#ifdef HOST_DEBUG_STATS
+	if(dev_addr > 63) {
+		printk("Please correct and implement/use proper addresses for pilot sb host\n");
+	}
+	if(!test_and_set_bit(dev_addr, device_addr_map)) {
+		printk(KERN_DEBUG "require_ep dev_addr %d manufacturer %s\n", dev_addr, manufacturer);
+		sprintf(buf, "driver/%s_%d", manufacturer, dev_addr);
+		proc_create_data(buf, 0, NULL, &proc_device_ops, (void*)dev_addr);
+	}
+#endif
+	if(dir & USB_DIR_IN) {
+		for(i = 1;i < 8;i++) {
+#ifdef EXTRA_DEBUG
+			printk("usb_host_free_in[%d].free %d\n", i, usb_host_free_in[i].free);
+#endif
+			if(usb_host_free_in[i].free) {
+				if(type == USB_ENDPOINT_XFER_BULK) {
+					se_pilot_host_ep_setup("epin-bulk", (USB_DIR_IN|i), USB_ENDPOINT_XFER_BULK, size /* maxpacket */); 
+				} else if(type == USB_ENDPOINT_XFER_INT) {
+					se_pilot_host_ep_setup("epin-in",(USB_DIR_IN|i), USB_ENDPOINT_XFER_INT, size/* maxpacket */);
+				} else {
+					printk("Unknown type\n");
+					return 0;
+				}
+				usb_host_free_in[i].dev_addr = dev_addr;
+#ifdef HOST_DEBUG_STATS
+				ind = 0x10 | (i & 0x7f);
+				ep = &pilot_host_udc->endpoints[ind];
+				ep->dev_addr = dev_addr;
+#endif
+				return 1;
+			}
+		}
+		printk("No free IN endpoints available\n");
+		return 0;
+	} else {
+		for(i = 1;i < 8;i++) {
+			if(usb_host_free_out[i].free) {
+				if(type == USB_ENDPOINT_XFER_BULK) {
+					se_pilot_host_ep_setup("epout-bulk", (USB_DIR_OUT|i), USB_ENDPOINT_XFER_BULK, size /* maxpacket */); 
+				} else if(type == USB_ENDPOINT_XFER_INT) {
+					se_pilot_host_ep_setup("epout-in",(USB_DIR_OUT|i), USB_ENDPOINT_XFER_INT, size/* maxpacket */);
+				} else {
+					printk("Unknown type\n");
+					return 0;
+				}
+				usb_host_free_out[i].dev_addr = dev_addr;
+#ifdef HOST_DEBUG_STATS
+				ind = (i & 0x7f);
+				ep = &pilot_host_udc->endpoints[ind];
+				ep->dev_addr = (u8)dev_addr;
+#endif
+				return 1;
+			}
+		}
+	}
+	printk("No free OUT endpoints available\n");
+	return 0;
+}
+
+/*Free all the endpoints associated with dev_addr */
+void free_ep(int dev_addr, char *manufacturer)
+{
+	int i = 0;
+#ifdef HOST_DEBUG_STATS
+	char buf[128];
+
+	if(dev_addr > 63) {
+		printk("Please correct and implement/use proper addresses for pilot sb host\n");
+	}
+	if(test_and_clear_bit(dev_addr, device_addr_map)) {
+		printk("free ep dev_addr %d manufacturer %s\n", dev_addr, manufacturer);
+		sprintf(buf, "driver/%s_%d", manufacturer, dev_addr);
+		remove_proc_entry(buf, NULL);
+	}
+#endif
+
+	for(i = 1;i < 8;i++) {
+		if(usb_host_free_in[i].dev_addr == dev_addr) {
+			printk("Freeing IN  Endpoint %d from dev %d\n", i, dev_addr);
+			if(usb_host_free_in[i].type == USB_ENDPOINT_XFER_BULK)
+				total_bulk_in_eps--;
+			else if(usb_host_free_in[i].type == USB_ENDPOINT_XFER_INT)
+				total_intr_in_eps--;
+			usb_host_free_in[i].free = 1;
+			usb_host_free_in[i].dev_addr = -1;
+#ifdef HOST_DEBUG_STATS
+			if(pilot_host_udc->bulk_in[i])
+				pilot_host_udc->bulk_in[i]->dev_addr = -1;
+			if(pilot_host_udc->intr_in[i])
+				pilot_host_udc->intr_in[i]->dev_addr = -1;
+#endif
+			pilot_host_udc->bulk_in[i] = pilot_host_udc->intr_in[i] = NULL;
+		}
+		if(usb_host_free_out[i].dev_addr == dev_addr) {
+			printk("Freeing OUT Endpoint %d from dev %d\n", i, dev_addr);
+			if(usb_host_free_out[i].type == USB_ENDPOINT_XFER_BULK)
+				total_bulk_out_eps--;
+			usb_host_free_out[i].free = 1;
+			usb_host_free_out[i].dev_addr = -1;
+#ifdef HOST_DEBUG_STATS
+			if(pilot_host_udc->bulk_out[i])
+				pilot_host_udc->bulk_out[i]->dev_addr = -1;
+			if(pilot_host_udc->intr_out[i])
+				pilot_host_udc->intr_out[i]->dev_addr = -1;
+#endif
+			pilot_host_udc->bulk_out[i] = pilot_host_udc->intr_out[i] = NULL;
+		}
+	}
+}
+
+struct pilot4_hw_ep* chose_bulk_hw_ep(int is_in, struct pilot_host_hcd	*pilot_host, unsigned char dev_addr)
+{
+	int i = 0;
+	if(is_in) {
+		for(i = 1;i < 8;i++) {
+			if(usb_host_free_in[i].dev_addr == dev_addr) {
+				//printk("Chosing  BULK IN  Endpoint %d from dev %d\n", i, dev_addr);
+				if(pilot_host->bulk_in[i] && pilot_host->bulk_in[i]->cur_qh == NULL)
+					return pilot_host->bulk_in[i];
+			}
+		}
+	} else {
+		for(i = 1;i < 8;i++) {
+			if(usb_host_free_out[i].dev_addr == dev_addr) {
+				//printk("Chosing BULK OUT  Endpoint %d from dev %d\n", i, dev_addr);
+				if(pilot_host->bulk_out[i] && pilot_host->bulk_out[i]->cur_qh == NULL)
+					return pilot_host->bulk_out[i];
+			}
+		}
+	}
+	printk("Didnt find BULK EP for Dev %d is_in %d\n", dev_addr, is_in);
+	return NULL;
+}
+
+struct pilot4_hw_ep* chose_int_hw_ep(int is_in, struct pilot_host_hcd	*pilot_host, unsigned char dev_addr)
+{
+	int i = 0;
+	if(is_in) {
+		for(i = 1;i < 8;i++) {
+			if(usb_host_free_in[i].dev_addr == dev_addr) {
+				//printk("Chosing  INT IN  Endpoint %d from dev %d\n", i, dev_addr);
+				if(pilot_host->intr_in[i] && pilot_host->intr_in[i]->cur_qh == NULL)
+					return pilot_host->intr_in[i];
+			}
+		}
+	} else {
+		for(i = 1;i < 8;i++) {
+			if(usb_host_free_out[i].dev_addr == dev_addr) {
+				//printk("Chosing INT OUT  Endpoint %d from dev %d\n", i, dev_addr);
+				if(pilot_host->intr_out[i] && pilot_host->intr_out[i]->cur_qh == NULL)
+					return pilot_host->intr_out[i];
+			}
+		}
+	}
+	printk("Didnt find INT EP for Dev %d is_in %d\n", dev_addr, is_in);
+	return NULL;
+}
+
+#endif
+
+#ifdef DEBUG_START_FINISH
+static int proc_hcd_show(struct seq_file *m, void *unused_v)
+{
+	int i = 0;
+	int ind = 0;
+	struct pilot4_hw_ep * ep;
+	unsigned long	flags;
+	spin_lock_irqsave(&pilot_host_udc->lock, flags);
+	seq_puts(m, "In EP details:\n");
+	for(i = 1;i < 8;i++) {
+		ind = 0x10 | (i & 0x7f);
+		ep = &pilot_host_udc->endpoints[ind];
+		if(ep->cur_urb) {
+			seq_printf(m,"urb %p status %d curtime %lu starttime %llu\n", ep->cur_urb, ep->cur_urb->status, jiffies, ep->start_time);
+			seq_printf(m,"actual length %d transferbuflength %d urb %p cur_qh %p\n",
+				ep->cur_urb->actual_length, ep->cur_urb->transfer_buffer_length, ep->cur_urb, ep->cur_qh);
+			seq_printf(m,"epnum %d epcs 0x%x addr %x programmed %d\n",
+				ep->ep_num, *((volatile unsigned char*)(ep->ep_cs_reg)), 
+				(ep->cur_qh)?ep->cur_qh->addr_reg:-1, (ep->cur_qh)?ep->cur_qh->programmed:-1);
+		}
+	}
+
+	seq_puts(m, "Out EP details:\n");
+	for(i = 1;i < 8;i++) {
+		ind = (i & 0x7f);
+		ep = &pilot_host_udc->endpoints[ind];
+		if(ep->cur_urb) {
+			seq_printf(m,"urb %p status %d curtime %lu starttime %llu\n", ep->cur_urb, ep->cur_urb->status, jiffies, ep->start_time);
+			seq_printf(m,"actual length %d transferbuflength %d urb %p cur_qh %p\n",
+				ep->cur_urb->actual_length, ep->cur_urb->transfer_buffer_length, ep->cur_urb, ep->cur_qh);
+			seq_printf(m,"epnum %d epcs 0x%x addr %x programmed %d\n",
+				ep->ep_num, *((volatile unsigned char*)(ep->ep_cs_reg)), 
+				(ep->cur_qh)?ep->cur_qh->addr_reg:-1, (ep->cur_qh)?ep->cur_qh->programmed:-1);
+		}
+	}
+	spin_unlock_irqrestore(&pilot_host_udc->lock, flags);
+	return 0;
+}
+static int proc_hcd_open(struct inode *inode, struct file *file)
+{
+	return single_open(file, proc_hcd_show, NULL);
+}
+
+static const struct file_operations proc_ops = {
+	.owner		= THIS_MODULE,
+	.open		= proc_hcd_open,
+	.read		= seq_read,
+	.release	= single_release,
+};
+#endif
+
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+struct pilot4_hw_ep *bulk_hw_ep;
+#endif
+int __init pilot4_platform_init(struct device *dev)
+{
+	struct usb_hcd	*hcd = NULL;
+	int i  =0, status = -1;
+	struct pilot_host_hcd *pilot_host = NULL;
+#ifndef DYNAMIC_CONFIGURE_EP
+	int bulk_ep_size = 64;
+#endif
+	int intr_ep_size = 8;
+	hcd = usb_create_hcd(&pilot4_hc_driver, dev, dev_name(dev));
+#ifdef EXTRA_DEBUG
+	printk("usb_create_hcd hcd %p\n", hcd);
+#endif
+	if (!hcd)
+		return -1;
+	pilot_require_ep = require_ep;
+	pilot_free_ep = free_ep;
+	hcd->uses_new_polling = 1;
+	hcd->has_tt = 1;//Akshay:Used by Full speed device on a High speed HCD
+	pilot_host_udc =  pilot_host = hcd_to_pilot_hcd(hcd);
+	pilot_host->base_addr = (unsigned char*) IO_ADDRESS(0x40700000);
+	pilot_host->board_set_vbus = pilot3_set_vbus;
+	
+	/* Initialize max size register offsets for in/out EP */
+	for(i = 0; i < 15;i++) {
+		hcinxmaxpck[i + 1] = 0x1E2 + (i *2);
+		hcoutxmaxpck[i + 1] = 0x3E2 + (i *2);
+	}
+#ifdef EXTRA_DEBUG
+	printk("Setting up EP's\n");
+#endif
+	se_pilot_host_ep_setup("ep0", 0, USB_ENDPOINT_XFER_CONTROL, intr_ep_size);
+#ifndef DYNAMIC_CONFIGURE_EP
+ 	se_pilot_host_ep_setup("epin-in",(USB_DIR_IN|0x01), USB_ENDPOINT_XFER_INT, intr_ep_size/* maxpacket */);
+	se_pilot_host_ep_setup("epin-bulk", (USB_DIR_IN|0x02), USB_ENDPOINT_XFER_BULK, bulk_ep_size /* maxpacket */); 
+	se_pilot_host_ep_setup("epin-bulk", (USB_DIR_IN|0x03), USB_ENDPOINT_XFER_BULK, bulk_ep_size /* maxpacket */); 
+ 	se_pilot_host_ep_setup("epin-in",(USB_DIR_IN|0x4), USB_ENDPOINT_XFER_INT, intr_ep_size/* maxpacket */);
+ 	se_pilot_host_ep_setup("epin-bulk",(USB_DIR_IN|0x5), USB_ENDPOINT_XFER_BULK, bulk_ep_size/* maxpacket */);
+ 	se_pilot_host_ep_setup("epin-bulk",(USB_DIR_IN|0x6), USB_ENDPOINT_XFER_BULK, bulk_ep_size/* maxpacket */);
+ 	se_pilot_host_ep_setup("epin-in",(USB_DIR_IN|0x7), USB_ENDPOINT_XFER_INT, intr_ep_size/* maxpacket */);
+
+ 	se_pilot_host_ep_setup("epout-bulk",(USB_DIR_OUT|0x01), USB_ENDPOINT_XFER_BULK, bulk_ep_size /* maxpacket */);
+ 	se_pilot_host_ep_setup("epout-bulk",(USB_DIR_OUT|0x2), USB_ENDPOINT_XFER_BULK, bulk_ep_size /* maxpacket */);
+ 	se_pilot_host_ep_setup("epout-bulk",(USB_DIR_OUT|0x03), USB_ENDPOINT_XFER_BULK, bulk_ep_size /* maxpacket */);
+ 	se_pilot_host_ep_setup("epout-bulk",(USB_DIR_OUT|0x04), USB_ENDPOINT_XFER_BULK, bulk_ep_size /* maxpacket */);
+ 	se_pilot_host_ep_setup("epout-bulk",(USB_DIR_OUT|0x05), USB_ENDPOINT_XFER_BULK, bulk_ep_size /* maxpacket */);
+#else
+	for(i = 0;i < 8;i++) {
+		pilot_host_udc->bulk_in[i] = pilot_host_udc->intr_in[i] = NULL;
+		pilot_host_udc->intr_out[i] = pilot_host_udc->bulk_out[i] = NULL;
+		usb_host_free_in[i].free = 1;
+		usb_host_free_in[i].dev_addr = -1;
+		usb_host_free_out[i].dev_addr = -1;
+		usb_host_free_out[i].free = 1;	
+	}
+	usb_host_free_in[0].free = usb_host_free_out[0].free = 0;//By defualt EP0 will noe be free
+#endif
+	pilot_host->dev = dev;
+	dev_set_drvdata(dev, pilot_host);
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+	se_pilot_host_ep_setup("epout-bulk",(USB_DIR_OUT|0x08), USB_ENDPOINT_XFER_BULK, 64/* maxpacket */);
+	bulk_hw_ep = &pilot_host_udc->endpoints[8];
+#endif
+	install_vector();
+	status = request_irq(IRQ_USB_1_0, se_pilot_udc_host_interrupt, (IRQF_SHARED|IRQF_DISABLED), "pilot_usb_host", pilot_host);
+	if(status < 0) {
+		printk("request_irq Failed\n");
+		return -1;
+	}
+	spin_lock_init(&pilot_host->lock);
+	status = usb_add_hcd(pilot_pilot4_to_hcd(pilot_host), -1 , 0);
+	if(status) {
+		printk("usb_add_hcd Failed status %d\n", status);
+	}
+#ifdef TEST_PORT_CONNECT
+	init_timer(&test_timer);
+	test_timer.function = test_timer_func;
+	test_timer.data = (unsigned long) pilot_host_udc;
+	what_to_do = SIMULATE_CONNECT;
+	mod_timer(&test_timer, jiffies + msecs_to_jiffies(2000));
+#endif
+#ifdef DEBUG_START_FINISH
+	proc_create("driver/pilot_usb_host", 0, NULL, &proc_ops);
+#endif
+#ifdef HOST_DEBUG_STATS
+	bitmap_clear(device_addr_map, 0, 128);
+#endif
+	return 0;
+	
+}
+
+static int __init pilot_hc_probe(struct platform_device *pdev)
+{
+	struct device	*dev = &pdev->dev;
+	pilot4_platform_init(dev);
+	return 0;
+}
+
+static void
+se_pilot_dummy_release (struct device *dev) 
+{
+	printk("Pilot USB HOST dummy  release %s\n", dev_name(dev));
+}
+
+static struct platform_device           se_pilot_host_pdev = {
+        .name           = (char *) "pilot_usb_host",
+        .id             = -1,
+        .dev            = {
+                .release        = se_pilot_dummy_release,
+        },
+};
+static struct platform_driver pilot4_platform_driver = {
+	.probe          = pilot_hc_probe,
+//	.remove		= __exit_p(se_pilot_udc_remove),//TODO
+	.driver		= {
+		.owner	= THIS_MODULE,
+		.name	= (char *) "pilot_usb_host",
+	},
+};
+
+static int __init pilot_pilot4_init(void)
+{
+#define SYSCONTROL              0x40100100
+	int retval;
+	int sedid = 0;
+	unsigned int temp = 0;
+        sedid = *(volatile unsigned int *)IO_ADDRESS(SYSCONTROL + 0x50);
+        sedid = sedid & 0xFF;
+        //A2 ECO 41
+        if(sedid == 0x42) {
+                printk("USB HOST:A2 ECO41\n");
+		temp = *(volatile unsigned int *)IO_ADDRESS(SYSCONTROL + 0x90);
+		temp = temp | (1 << 23);
+		*(volatile unsigned int *)IO_ADDRESS(SYSCONTROL + 0x90) = temp;
+        }
+	retval = platform_device_register (&se_pilot_host_pdev);
+	//printk("pilot_pilot4_init platform_device_register %d\n", retval);
+
+	retval = platform_driver_register(&pilot4_platform_driver);
+	//printk("pilot_pilot4_init platform_driver_probe %d\n", retval);
+	if (retval < 0)
+		return -1;
+	return 0;
+}
+
+static void __exit pilot_pilot4_cleanup(void)
+{
+	pilot3_set_vbus(pilot_host_udc, 0);
+	usb_remove_hcd(pilot_pilot4_to_hcd(pilot_host_udc));
+	free_irq(IRQ_USB_1_0, pilot_host_udc);
+	printk("%s %d\n", __FUNCTION__, __LINE__);
+	platform_device_unregister (&se_pilot_host_pdev);
+	printk("%s %d\n", __FUNCTION__, __LINE__);
+	platform_driver_unregister(&pilot4_platform_driver);
+	printk("%s %d\n", __FUNCTION__, __LINE__);
+}
+
+module_init(pilot_pilot4_init);
+module_exit(pilot_pilot4_cleanup);
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("Akshay Srinivas <akshay.srinivas@emulex.com>");
