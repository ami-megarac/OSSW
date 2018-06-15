--- linux/drivers/usb/host/pilot/pilot4_virthub.c	1970-01-01 08:00:00.000000000 +0800
+++ linux.new/drivers/usb/host/pilot/pilot4_virthub.c	2016-12-19 16:10:37.719414544 +0800
@@ -0,0 +1,404 @@
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
+#include <linux/sched.h>
+#include <linux/slab.h>
+#include <linux/errno.h>
+#include <linux/init.h>
+#include <linux/time.h>
+#include <linux/timer.h>
+
+#include <asm/unaligned.h>
+
+#include "pilot4_core.h"
+
+#define DBG(fmt, args...) printk(KERN_CRIT fmt, ## args)
+#if 0 //only for debugging
+#define pr_debug(fmt, ...) \
+	printk(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)
+#endif
+
+static void pilot4_port_suspend(struct pilot_host_hcd *pilot_host, bool do_suspend)
+{
+	u8		power;
+
+	if (do_suspend) {
+
+		power = pilot4_readb(pilot_host->base_addr, OTGCTRL);
+		power &= ~(OTG_BUS_REQ);
+		pilot4_writeb(pilot_host->base_addr, OTGCTRL, 	power);
+		pr_debug("Root port suspended, power %02x\n", power);
+
+		pilot_host->port1_status |= USB_PORT_STAT_SUSPEND;
+		switch (pilot_host->state) {
+		case OTG_STATE_A_HOST:
+			pilot_host->state = OTG_STATE_A_SUSPEND;
+			pilot4_platform_try_idle(pilot_host, 0);
+			break;
+		default:
+			printk("Bogus state pilot_host->state %d\n", pilot_host->state);
+		}
+	} else {
+		power = pilot4_readb(pilot_host->base_addr, OTGCTRL);
+		power |= (OTG_BUS_REQ);
+		power &= ~(OTG_ABUS_DROP);//todo verify if this is required
+		pilot4_writeb(pilot_host->base_addr, OTGCTRL, 	power);
+
+		DBG("Root port resuming, power %02x\n", power);
+
+		/* later, GetPortStatus will stop RESUME signaling */
+		pilot_host->port1_status |= PILOT_PORT_STAT_RESUME;
+		pilot_host->rh_timer = jiffies + msecs_to_jiffies(20);
+	}
+}
+int high_speed_device;
+static void pilot4_port_reset(struct pilot_host_hcd *pilot_host, bool do_reset)
+{
+	u8		power;
+	void __iomem	*mbase = pilot_host->base_addr;
+	int speed_configured = 0;
+
+	if (!is_host_active(pilot_host))
+		return;
+
+	/* NOTE:  caller guarantees it will turn off the reset when
+	 * the appropriate amount of time has passed
+	 */
+	power = pilot4_readb(mbase, HCPORTCTRL);
+	pr_debug("%s portctrl %x\n", __FUNCTION__, power);
+	if (do_reset) {
+
+		high_speed_device = 0;
+		power |= PORT_RESET;
+		pilot4_writeb(mbase, HCPORTCTRL, power);
+
+		pilot_host->port1_status |= USB_PORT_STAT_RESET;
+		pilot_host->port1_status &= ~USB_PORT_STAT_ENABLE;
+		pilot_host->rh_timer = jiffies + msecs_to_jiffies(50);
+	} else {
+		unsigned char usbcs = 0;
+#ifndef DYNAMIC_CONFIGURE_EP
+		extern void configure_endpoints(int speed);
+#else
+		extern void configure_ep0(int speed);
+#endif
+		#define LOWSPEED 1
+#ifdef TEST_PORT_CONNECT
+		extern int test_only;
+#endif
+		pr_debug("root port reset stopped\n");
+#ifdef TEST_PORT_CONNECT
+		pilot_host->port1_status |= USB_PORT_STAT_HIGH_SPEED;
+#else
+		usbcs = pilot4_readb(mbase, HCUSBCS);
+		if(usbcs & LOWSPEED) {
+			printk("Low speed USB device\n");
+			pilot_host->port1_status |= USB_PORT_STAT_LOW_SPEED;
+#ifndef DYNAMIC_CONFIGURE_EP
+			configure_endpoints(USB_SPEED_LOW);//Low speed
+#else
+			configure_ep0(USB_SPEED_LOW);
+#endif
+			pilot_host->speed = USB_SPEED_LOW;
+			speed_configured = 1;
+		}
+		if (high_speed_device) {
+			if(usbcs & LOWSPEED) {
+				printk("\nUnexpected! Got HS interrupt but core says Low speed device!\n");
+			}
+			DBG("high-speed device connected\n");
+			pilot_host->port1_status |= USB_PORT_STAT_HIGH_SPEED;
+			pilot_host->port1_status &= ~USB_PORT_STAT_LOW_SPEED;
+#ifndef DYNAMIC_CONFIGURE_EP
+			configure_endpoints(USB_SPEED_HIGH);//High speed
+#else
+			configure_ep0(USB_SPEED_HIGH);
+#endif
+			pilot_host->speed = USB_SPEED_HIGH;
+			speed_configured = 1;
+		}
+#endif
+		if(!speed_configured) {
+#ifndef DYNAMIC_CONFIGURE_EP
+			configure_endpoints(USB_SPEED_FULL);//Full speed
+#else
+			configure_ep0(USB_SPEED_FULL);
+#endif
+			pilot_host->speed = USB_SPEED_FULL;
+		}
+		pilot_host->port1_status &= ~USB_PORT_STAT_RESET;
+		pilot_host->port1_status |= USB_PORT_STAT_ENABLE
+					| (USB_PORT_STAT_C_RESET << 16)
+					| (USB_PORT_STAT_C_ENABLE << 16);
+		usb_hcd_poll_rh_status(pilot_pilot4_to_hcd(pilot_host));
+
+		pilot_host->vbuserr_retry = VBUSERR_RETRY_COUNT;
+	}
+}
+
+void pilot4_root_disconnect(struct pilot_host_hcd *pilot_host)
+{
+	printk("\n pilot4_root_disconnect!\n");
+	pilot_host->port1_status = (1 << USB_PORT_FEAT_POWER)
+			| (1 << USB_PORT_FEAT_C_CONNECTION);
+
+	usb_hcd_poll_rh_status(pilot_pilot4_to_hcd(pilot_host));
+	pilot_host->is_active = 0;
+
+	switch (pilot_host->state) {
+	case OTG_STATE_A_HOST:
+	case OTG_STATE_A_SUSPEND:
+		pilot_host->state = OTG_STATE_A_WAIT_BCON;
+		pilot_host->is_active = 0;
+		break;
+	case OTG_STATE_A_WAIT_VFALL:
+		pilot_host->state = OTG_STATE_B_IDLE;
+		break;
+	default:
+		printk("Bogus state pilot_host->state %d\n", pilot_host->state);
+	}
+}
+
+
+/*---------------------------------------------------------------------*/
+
+/* Caller may or may not hold pilot_host->lock */
+int pilot4_hub_status_data(struct usb_hcd *hcd, char *buf)
+{
+	struct pilot_host_hcd	*pilot_host = hcd_to_pilot_hcd(hcd);
+	int		retval = 0;
+
+	/* called in_irq() via usb_hcd_poll_rh_status() */
+	if (pilot_host->port1_status & 0xffff0000) {
+		*buf = 0x02;
+		retval = 1;
+	}
+	return retval;
+}
+#define HCUSBIEN	0x198
+#define OTGIEN	0x1C0
+#define USBRESET_IEN (1<<4)
+#define USBHS_IEN (1<<5)
+
+void pilot4_usb_enable_interrupts(struct pilot_host_hcd *pilot_host)
+{
+	u8 ien;
+	printk("USB Host some interrupts wlll be enabled\n");
+	//TODO I dont think we support per port power control
+	//do we need to anything?
+	//enable interrupts
+	ien = pilot4_readb(pilot_host->base_addr, HCUSBIEN);
+	ien |= (USBRESET_IEN|USBHS_IEN);
+	pilot4_writeb(pilot_host->base_addr, HCUSBIEN, ien);
+	ien = pilot4_readb(pilot_host->base_addr, OTGIEN);
+	ien = 0x3F;//Enable all interrupts
+	pilot4_writeb(pilot_host->base_addr, OTGIEN, ien);
+	ien = 0xFF;//EP0 EP1 EP2 EP3 EP4 EP5 EP6 EP7
+	pilot4_writeb(pilot_host->base_addr, hcin07ien, ien);
+	ien = 0xFF;//EP0 EP1 EP2 EP3 EP4 EP5 EP6 EP7
+	pilot4_writeb(pilot_host->base_addr, hcout07ien, ien);
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+	ien = 0x1;//EP8
+	pilot4_writeb(pilot_host->base_addr, hcout815ien, ien);
+#endif
+
+	ien = 0xFF;//EP0 EP1 EP2 EP3 EP4 EP5 EP6 EP7
+	pilot4_writeb(pilot_host->base_addr, hcin07errien, ien);
+
+	ien = 0xFF;//EP0 EP1 EP2 EP3 EP4 EP5 EP6 EP7
+	pilot4_writeb(pilot_host->base_addr, hcout07errien, ien);
+#ifdef USE_BULK_EP_FOR_EP0_OUT
+	ien = 0x1;//EP8
+	pilot4_writeb(pilot_host->base_addr, hcout815errien, ien);
+#endif
+}
+
+int pilot4_hub_control(
+	struct usb_hcd	*hcd,
+	u16		typeReq,
+	u16		wValue,
+	u16		wIndex,
+	char		*buf,
+	u16		wLength)
+{
+	struct pilot_host_hcd	*pilot_host = hcd_to_pilot_hcd(hcd);
+	u32		temp;
+	int		retval = 0;
+	unsigned long	flags;
+
+	spin_lock_irqsave(&pilot_host->lock, flags);
+//	printk("pilot4_hub_control typeReq %x wValue %x wIndex %x wLength %d\n",
+//		typeReq, wValue, wIndex, wLength);
+	if (unlikely(!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags))) {
+		spin_unlock_irqrestore(&pilot_host->lock, flags);
+		return -ESHUTDOWN;
+	}
+
+	/* hub features:  always zero, setting is a NOP
+	 * port features: reported, sometimes updated when host is active
+	 * no indicators
+	 */
+	switch (typeReq) {
+	case ClearHubFeature:
+	case SetHubFeature:
+		switch (wValue) {
+		case C_HUB_OVER_CURRENT:
+		case C_HUB_LOCAL_POWER:
+			break;
+		default:
+			goto error;
+		}
+		break;
+	case ClearPortFeature:
+		if ((wIndex & 0xff) != 1)
+			goto error;
+
+		switch (wValue) {
+		case USB_PORT_FEAT_ENABLE:
+			/*Should we make this port in disabled state?*/
+			break;
+		case USB_PORT_FEAT_SUSPEND:
+			pilot4_port_suspend(pilot_host, false);
+			break;
+		case USB_PORT_FEAT_POWER:
+			pilot4_set_vbus(pilot_host, 0);
+			break;
+		case USB_PORT_FEAT_C_CONNECTION:
+		case USB_PORT_FEAT_C_ENABLE:
+		case USB_PORT_FEAT_C_OVER_CURRENT:
+		case USB_PORT_FEAT_C_RESET:
+		case USB_PORT_FEAT_C_SUSPEND:
+			break;
+		default:
+			goto error;
+		}
+		//DBG("clear feature %d\n", wValue);
+		pilot_host->port1_status &= ~(1 << wValue);
+		break;
+	case GetHubDescriptor:
+		{
+		struct usb_hub_descriptor *desc = (void *)buf;
+
+		desc->bDescLength = 9;
+		desc->bDescriptorType = 0x29;
+		desc->bNbrPorts = 1;
+		desc->wHubCharacteristics = __constant_cpu_to_le16(
+				  0x0001	/* per-port power switching */
+				| 0x0010	/* no overcurrent reporting */
+				);
+		desc->bPwrOn2PwrGood = 5;	/* msec/2 */
+		desc->bHubContrCurrent = 0;
+
+		/* workaround bogus struct definition */
+		desc->u.hs.DeviceRemovable[0] = 0x02;	/* port 1 */
+		desc->u.hs.DeviceRemovable[1] = 0xff;
+		}
+		break;
+	case GetHubStatus:
+		temp = 0;
+		*(__le32 *) buf = cpu_to_le32(temp);
+		break;
+	case GetPortStatus:
+		if (wIndex != 1)
+			goto error;
+
+		/* finish RESET signaling? */
+		if ((pilot_host->port1_status & USB_PORT_STAT_RESET)
+				&& time_after_eq(jiffies, pilot_host->rh_timer))//TODO Come back on this
+			pilot4_port_reset(pilot_host, false);
+
+		/* finish RESUME signaling? */
+		if ((pilot_host->port1_status & PILOT_PORT_STAT_RESUME)
+				&& time_after_eq(jiffies, pilot_host->rh_timer)) {
+			u8		power = 0;
+
+			DBG("root port resume stopped, power %02x\n",
+					power);
+
+			pilot_host->is_active = 1;
+			pilot_host->port1_status &= ~(USB_PORT_STAT_SUSPEND
+					| PILOT_PORT_STAT_RESUME);
+			pilot_host->port1_status |= USB_PORT_STAT_C_SUSPEND << 16;
+			usb_hcd_poll_rh_status(pilot_pilot4_to_hcd(pilot_host));
+			/* NOTE: it might really be A_WAIT_BCON ... */
+			pilot_host->state = OTG_STATE_A_HOST;
+		}
+
+		put_unaligned(cpu_to_le32(pilot_host->port1_status
+					& ~PILOT_PORT_STAT_RESUME),
+				(__le32 *) buf);
+
+		/* port change status is more interesting */
+		//DBG( "port status %08x\n",
+		//		pilot_host->port1_status);
+		break;
+	case SetPortFeature:
+		if ((wIndex & 0xff) != 1)
+			goto error;
+
+		switch (wValue) {
+		case USB_PORT_FEAT_POWER:
+			pilot4_set_vbus(pilot_host, 1);
+			break;
+		case USB_PORT_FEAT_RESET:
+			pilot4_port_reset(pilot_host, true);
+			break;
+		case USB_PORT_FEAT_SUSPEND:
+			pilot4_port_suspend(pilot_host, true);
+			break;
+		case USB_PORT_FEAT_TEST:
+			if (unlikely(is_host_active(pilot_host)))
+				goto error;
+
+			wIndex >>= 8;
+			switch (wIndex) {
+			case 1:
+				pr_debug("TEST_J\n");
+				break;
+			case 2:
+				pr_debug("TEST_K\n");
+				break;
+			case 3:
+				pr_debug("TEST_SE0_NAK\n");
+				break;
+			case 4:
+				pr_debug("TEST_PACKET\n");
+				break;
+			case 5:
+				pr_debug("TEST_FORCE_ENABLE\n");
+				break;
+			case 6:
+				pr_debug("TEST_FIFO_ACCESS\n");
+				break;
+			default:
+				goto error;
+			}
+			break;
+		default:
+			goto error;
+		}
+		pilot_host->port1_status |= 1 << wValue;
+		break;
+
+	default:
+error:
+		/* "protocol stall" on error */
+		retval = -EPIPE;
+	}
+	spin_unlock_irqrestore(&pilot_host->lock, flags);
+	return retval;
+}
+
+
