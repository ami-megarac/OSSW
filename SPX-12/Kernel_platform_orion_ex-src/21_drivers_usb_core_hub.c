--- linux/drivers/usb/core/hub.c	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/drivers/usb/core/hub.c	2016-12-19 15:46:31.887414544 +0800
@@ -7,6 +7,13 @@
  * (C) Copyright 2001 Brad Hards (bhards@bigpond.net.au)
  *
  */
+/*
+ * Copyright (c) 2010-2015, Emulex Corporation.
+ * Modifications made by Emulex Corporation under the terms of the
+ * GNU General Public License as published by the Free Software
+ * Foundation; either version 2 of the License, or (at your option)
+ * any later version.
+ */
 
 #include <linux/kernel.h>
 #include <linux/errno.h>
@@ -1695,8 +1702,14 @@
 	 * - Change autosuspend delay of hub can avoid unnecessary auto
 	 *   suspend timer for hub, also may decrease power consumption
 	 *   of USB bus.
+	 *
+	 * - If user has indicated to prevent autosuspend by passing
+	 *   usbcore.autosuspend = -1 then keep autosuspend disabled.
 	 */
-	pm_runtime_set_autosuspend_delay(&hdev->dev, 0);
+#ifdef CONFIG_PM_RUNTIME
+	if (hdev->dev.power.autosuspend_delay >= 0)
+		pm_runtime_set_autosuspend_delay(&hdev->dev, 0);
+#endif
 
 	/*
 	 * Hubs have proper suspend/resume support, except for root hubs
@@ -1942,8 +1955,10 @@
 					|| new_state == USB_STATE_SUSPENDED)
 				;	/* No change to wakeup settings */
 			else if (new_state == USB_STATE_CONFIGURED)
-				wakeup = udev->actconfig->desc.bmAttributes
-					 & USB_CONFIG_ATT_WAKEUP;
+				wakeup = (udev->quirks &
+					USB_QUIRK_IGNORE_REMOTE_WAKEUP) ? 0 :
+					udev->actconfig->desc.bmAttributes &
+					USB_CONFIG_ATT_WAKEUP;
 			else
 				wakeup = 0;
 		}
@@ -2039,7 +2054,16 @@
 	if (hcd->driver->free_dev && udev->parent)
 		hcd->driver->free_dev(hcd, udev);
 }
-
+#if defined(CONFIG_USB_PILOT4_HCD_MODULE) || defined(CONFIG_USB_PILOT4_HCD)
+int total_bulk_in_eps, total_bulk_out_eps, total_intr_in_eps;
+int (*pilot_require_ep)(__le16 size, __u8 type, int dir, int dev_addr, char *);
+void (*pilot_free_ep)(int dev_addr, char *);
+EXPORT_SYMBOL(pilot_free_ep);
+EXPORT_SYMBOL(pilot_require_ep);
+EXPORT_SYMBOL(total_bulk_in_eps);
+EXPORT_SYMBOL(total_bulk_out_eps);
+EXPORT_SYMBOL(total_intr_in_eps);
+#endif
 /**
  * usb_disconnect - disconnect a device (usbcore-internal)
  * @pdev: pointer to device being disconnected
@@ -2101,6 +2125,12 @@
 
 	usb_remove_ep_devs(&udev->ep0);
 	usb_unlock_device(udev);
+#if defined(CONFIG_USB_PILOT4_HCD) || defined(CONFIG_USB_PILOT4_HCD_MODULE)
+	total_bulk_in_eps += udev->rqrd_bulk_in_eps;
+	total_bulk_out_eps += udev->rqrd_bulk_out_eps;
+	total_intr_in_eps += udev->rqrd_intr_in_eps;
+	pilot_free_ep(udev->devnum, udev->manufacturer);
+#endif
 
 	/* Unregister the device.  The device driver is responsible
 	 * for de-configuring the device and invoking the remove-device
@@ -2307,6 +2337,151 @@
 		udev->removable = USB_DEVICE_FIXED;
 }
 
+#if defined(CONFIG_USB_PILOT4_HCD_MODULE) || defined(CONFIG_USB_PILOT4_HCD)
+static int check_ep_requirements(struct usb_device * dev)
+{
+	int i = 0, j = 0, k = 0;
+	struct usb_host_config *cp = NULL;
+	int n, nintf, ok = 1;
+	int rqrd_bulk_in_eps = 0, rqrd_bulk_out_eps = 0, rqrd_intr_in_eps = 0, rqrd_intr_out_eps = 0;
+	int alt_rqrd_bulk_in_eps = 0, alt_rqrd_bulk_out_eps = 0, alt_rqrd_intr_in_eps = 0, alt_rqrd_intr_out_eps = 0;
+	u8 intf_rqrd_bulk_in_eps = 0, intf_rqrd_bulk_out_eps = 0, intf_rqrd_intr_in_eps = 0, intf_rqrd_intr_out_eps = 0;
+	/*Assumption: Generally 1 Endpoint within a device will have same maxpacket size for a endpoint type even for different configs and intfs*/
+	__le16 bulk_in_size = 0, bulk_out_size = 0, intr_in_size =0, intr_out_size = 0;
+
+	printk(KERN_DEBUG "Total configurations %d bcdUSB %x bMaxPacketSize0 %d idVendor %x bcdDevice %x\n",
+		dev->descriptor.bNumConfigurations, dev->descriptor.bcdUSB,
+		dev->descriptor.bMaxPacketSize0 , dev->descriptor.idVendor, dev->descriptor.bcdDevice);
+	for (i = 0; i < dev->descriptor.bNumConfigurations; i++) {
+		cp = &dev->config[i];
+		n = nintf = 0;
+		if (cp) {
+			nintf = cp->desc.bNumInterfaces;
+			printk(KERN_DEBUG "Total interfaces in config %d is %d \n",
+				cp->desc.bConfigurationValue, cp->desc.bNumInterfaces);
+			for (; n < nintf; ++n) {
+				struct usb_interface_cache *intfc;
+				struct usb_host_interface *altsetting;
+				struct usb_host_endpoint *endpoint;
+				intfc = cp->intf_cache[n];
+				altsetting = intfc->altsetting;
+				printk(KERN_DEBUG "Total alsetting for intf %d is %d\n", altsetting->desc.bInterfaceNumber, intfc->num_altsetting);
+				for (j = 0; j < intfc->num_altsetting; j++) {
+					printk(KERN_DEBUG "bDescriptorType %x bInterfaceNumber %d bAlternateSetting %d bNumEndpoints %d\n",
+						altsetting->desc.bDescriptorType, altsetting->desc.bInterfaceNumber,
+						altsetting->desc.bAlternateSetting, altsetting->desc.bNumEndpoints);
+					for (k = 0; k < altsetting->desc.bNumEndpoints; ++k) {
+						endpoint = &altsetting->endpoint[k];
+						if(endpoint) {
+							printk(KERN_DEBUG "bDescriptorType %x bEndpointAddress %x bmAttributes %x wMaxPacketSize %d bInterval %x\n",
+								endpoint->desc.bDescriptorType, endpoint->desc.bEndpointAddress,
+								endpoint->desc.bmAttributes ,endpoint->desc.wMaxPacketSize, endpoint->desc.bInterval);
+							if(endpoint->desc.bmAttributes == USB_ENDPOINT_XFER_BULK && 
+								endpoint->desc.bEndpointAddress & USB_DIR_IN)  {
+								rqrd_bulk_in_eps++;
+								if(bulk_in_size != 0 && bulk_in_size != endpoint->desc.wMaxPacketSize)
+									printk("\nDiff! endpoint->desc.wMaxPacketSize %d bulk_in_size %d\n",
+										endpoint->desc.wMaxPacketSize, bulk_in_size);
+								bulk_in_size = max(bulk_in_size, endpoint->desc.wMaxPacketSize);
+							}
+							else if(endpoint->desc.bmAttributes == USB_ENDPOINT_XFER_BULK && 
+								((endpoint->desc.bEndpointAddress & USB_DIR_IN) == USB_DIR_OUT)) {
+								rqrd_bulk_out_eps++;
+								if(bulk_out_size != 0 && bulk_out_size != endpoint->desc.wMaxPacketSize)
+									printk("\nDiff! endpoint->desc.wMaxPacketSize %d bulk_out_size %d\n",
+										endpoint->desc.wMaxPacketSize, bulk_out_size);
+								bulk_out_size = max(bulk_out_size, endpoint->desc.wMaxPacketSize);
+							}
+							else if(endpoint->desc.bmAttributes == USB_ENDPOINT_XFER_INT && 
+								endpoint->desc.bEndpointAddress & USB_DIR_IN) {
+								rqrd_intr_in_eps++;
+								if(intr_in_size != 0 && intr_in_size != endpoint->desc.wMaxPacketSize)
+									printk("\nDiff! endpoint->desc.wMaxPacketSize %d intr_in_size %d\n",
+										endpoint->desc.wMaxPacketSize, intr_in_size);
+								intr_in_size = max(intr_in_size, endpoint->desc.wMaxPacketSize);
+							}
+							else if(endpoint->desc.bmAttributes == USB_ENDPOINT_XFER_INT && 
+								((endpoint->desc.bEndpointAddress & USB_DIR_IN) == USB_DIR_OUT)) {
+								rqrd_intr_out_eps++;
+								if(intr_out_size != 0 && intr_out_size != endpoint->desc.wMaxPacketSize)
+									printk("\nDiff! endpoint->desc.wMaxPacketSize %d intr_out_size %d\n",
+										endpoint->desc.wMaxPacketSize, intr_out_size);
+								intr_out_size = max(intr_out_size, endpoint->desc.wMaxPacketSize);
+							}
+						} else {
+							printk("endpoint is NULL\n");
+						}
+					}
+					altsetting++;
+					alt_rqrd_bulk_in_eps = max(alt_rqrd_bulk_in_eps, rqrd_bulk_in_eps);
+					alt_rqrd_bulk_out_eps = max(alt_rqrd_bulk_out_eps, rqrd_bulk_out_eps);
+					alt_rqrd_intr_in_eps = max(alt_rqrd_intr_in_eps, rqrd_intr_in_eps);
+					alt_rqrd_intr_out_eps = max(alt_rqrd_intr_out_eps, rqrd_intr_out_eps);
+					rqrd_intr_out_eps = rqrd_intr_in_eps = rqrd_bulk_out_eps = rqrd_bulk_in_eps = 0;
+				}
+				intf_rqrd_bulk_in_eps += alt_rqrd_bulk_in_eps;
+				intf_rqrd_bulk_out_eps += alt_rqrd_bulk_out_eps;
+				intf_rqrd_intr_in_eps += alt_rqrd_intr_in_eps;
+				intf_rqrd_intr_out_eps += alt_rqrd_intr_out_eps;
+			}
+		}
+		dev->rqrd_bulk_in_eps = max(dev->rqrd_bulk_in_eps,intf_rqrd_bulk_in_eps);
+		dev->rqrd_bulk_out_eps = max(dev->rqrd_bulk_out_eps, intf_rqrd_bulk_out_eps);
+		dev->rqrd_intr_in_eps = max(dev->rqrd_intr_in_eps, intf_rqrd_intr_in_eps);
+		dev->rqrd_intr_out_eps = max(dev->rqrd_intr_out_eps, intf_rqrd_intr_out_eps);
+		intf_rqrd_bulk_in_eps = intf_rqrd_bulk_out_eps = intf_rqrd_intr_in_eps = intf_rqrd_intr_out_eps = 0;
+	}
+	printk(KERN_DEBUG "Total EP's available:\n");
+	printk(KERN_DEBUG "%10s %10s %10s %10s\n","BULKIN","BULKOUT", "INTRIN", "INTROUT");
+	printk(KERN_DEBUG "%10d %10d %10d %10d\n",total_bulk_in_eps, total_bulk_out_eps, total_intr_in_eps, 0);
+	printk(KERN_DEBUG "Total EP's Required:\n");
+	printk(KERN_DEBUG "%10d %10d %10d %10d\n",
+		dev->rqrd_bulk_in_eps, dev->rqrd_bulk_out_eps, dev->rqrd_intr_in_eps, dev->rqrd_intr_out_eps);
+#if 0
+	if(dev->rqrd_bulk_in_eps > total_bulk_in_eps || dev->rqrd_bulk_out_eps > total_bulk_out_eps ||
+		dev->rqrd_intr_in_eps > total_intr_in_eps || dev->rqrd_intr_out_eps > 0) {
+		dev->rqrd_bulk_in_eps = dev->rqrd_bulk_out_eps = dev->rqrd_intr_in_eps = dev->rqrd_intr_out_eps = 0;
+		pilot_free_ep(dev->devnum);
+		return 1;
+	}
+#else
+	for(i = 0;i < dev->rqrd_bulk_in_eps;i++) {
+		ok = pilot_require_ep(bulk_in_size, USB_ENDPOINT_XFER_BULK, USB_DIR_IN, dev->devnum, dev->manufacturer);
+		if(ok == 0) {
+			pilot_free_ep(dev->devnum, dev->manufacturer);
+			return 1;
+		}
+	}
+	for(i = 0;i < dev->rqrd_bulk_out_eps;i++) {
+		ok = pilot_require_ep(bulk_out_size, USB_ENDPOINT_XFER_BULK, USB_DIR_OUT, dev->devnum, dev->manufacturer);
+		if(ok == 0) {
+			pilot_free_ep(dev->devnum, dev->manufacturer);
+			return 1;
+		}
+	}
+	for(i = 0;i < dev->rqrd_intr_in_eps;i++) {
+		ok = pilot_require_ep(intr_in_size, USB_ENDPOINT_XFER_INT, USB_DIR_IN, dev->devnum, dev->manufacturer);
+		if(ok == 0) {
+			pilot_free_ep(dev->devnum, dev->manufacturer);
+			return 1;
+		}
+	}
+	for(i = 0;i < dev->rqrd_intr_out_eps;i++) {
+		ok = pilot_require_ep(intr_out_size, USB_ENDPOINT_XFER_INT, USB_DIR_OUT, dev->devnum, dev->manufacturer);
+		if(ok == 0) {
+			pilot_free_ep(dev->devnum, dev->manufacturer);
+			return 1;
+		}
+	}
+
+#endif
+	total_bulk_in_eps -= dev->rqrd_bulk_in_eps;
+	total_bulk_out_eps -= dev->rqrd_bulk_out_eps;
+	total_intr_in_eps -= dev->rqrd_intr_in_eps;
+	return 0;
+}
+#endif
+
 /**
  * usb_new_device - perform initial device setup (usbcore-internal)
  * @udev: newly addressed device (in ADDRESS state)
@@ -2354,6 +2529,18 @@
 	err = usb_enumerate_device(udev);	/* Read descriptors */
 	if (err < 0)
 		goto fail;
+#if defined(CONFIG_USB_PILOT4_HCD_MODULE) || defined(CONFIG_USB_PILOT4_HCD)
+	if(udev->parent != NULL) {
+		if(pilot_require_ep == NULL || pilot_free_ep == NULL)
+			panic("pilot_require_ep == NULL || pilot_free_ep == NULL\n");
+		err = check_ep_requirements(udev);
+		if(err) {
+			usb_deauthorize_device(udev);
+		}
+	} else {
+		//printk("Not check EP reqrt for Root Hub\n");
+	}
+#endif
 	dev_dbg(&udev->dev, "udev %d, busnum %d, minor = %d\n",
 			udev->devnum, udev->bus->busnum,
 			(((udev->bus->busnum-1) * 128) + (udev->devnum-1)));
@@ -3174,6 +3361,43 @@
 }
 
 /*
+ * There are some SS USB devices which take longer time for link training.
+ * XHCI specs 4.19.4 says that when Link training is successful, port
+ * sets CSC bit to 1. So if SW reads port status before successful link
+ * training, then it will not find device to be present.
+ * USB Analyzer log with such buggy devices show that in some cases
+ * device switch on the RX termination after long delay of host enabling
+ * the VBUS. In few other cases it has been seen that device fails to
+ * negotiate link training in first attempt. It has been
+ * reported till now that few devices take as long as 2000 ms to train
+ * the link after host enabling its VBUS and termination. Following
+ * routine implements a 2000 ms timeout for link training. If in a case
+ * link trains before timeout, loop will exit earlier.
+ *
+ * FIXME: If a device was connected before suspend, but was removed
+ * while system was asleep, then the loop in the following routine will
+ * only exit at timeout.
+ *
+ * This routine should only be called when persist is enabled for a SS
+ * device.
+ */
+static int wait_for_ss_port_enable(struct usb_device *udev,
+		struct usb_hub *hub, int *port1,
+		u16 *portchange, u16 *portstatus)
+{
+	int status = 0, delay_ms = 0;
+
+	while (delay_ms < 2000) {
+		if (status || *portstatus & USB_PORT_STAT_CONNECTION)
+			break;
+		msleep(20);
+		delay_ms += 20;
+		status = hub_port_status(hub, *port1, portstatus, portchange);
+	}
+	return status;
+}
+
+/*
  * usb_port_resume - re-activate a suspended usb device's upstream port
  * @udev: device to re-activate, not a root hub
  * Context: must be able to sleep; device not locked; pm locks held
@@ -3275,6 +3499,10 @@
 
 	clear_bit(port1, hub->busy_bits);
 
+	if (udev->persist_enabled && hub_is_superspeed(hub->hdev))
+		status = wait_for_ss_port_enable(udev, hub, &port1, &portchange,
+				&portstatus);
+
 	status = check_port_resume_type(udev,
 			hub, port1, status, portchange, portstatus);
 	if (status == 0)
@@ -4094,7 +4322,17 @@
 		speed = "variable speed Wireless";
 	else
 		speed = usb_speed_string(udev->speed);
-
+#if defined(CONFIG_USB_PILOT4_HCD_MODULE) || defined(CONFIG_USB_PILOT4_HCD)
+	//Make sure parent is not Root ub
+	if(udev->parent && udev->parent->parent) {
+		if(udev->speed < udev->parent->speed) {
+			printk("Not enumerating %s device behind %s parent\n", 
+				usb_speed_string(udev->speed), usb_speed_string(udev->parent->speed));
+			retval = -ENOTCONN;
+			goto fail;
+		}
+	}
+#endif
 	if (udev->speed != USB_SPEED_SUPER)
 		dev_info(&udev->dev,
 				"%s %s USB device number %d using %s\n",
@@ -4329,6 +4567,9 @@
 	struct usb_qualifier_descriptor	*qual;
 	int				status;
 
+	if (udev->quirks & USB_QUIRK_DEVICE_QUALIFIER)
+		return;
+
 	qual = kmalloc (sizeof *qual, GFP_KERNEL);
 	if (qual == NULL)
 		return;
@@ -4717,9 +4958,10 @@
 
 		hub = list_entry(tmp, struct usb_hub, event_list);
 		kref_get(&hub->kref);
+		hdev = hub->hdev;
+		usb_get_dev(hdev);
 		spin_unlock_irq(&hub_event_lock);
 
-		hdev = hub->hdev;
 		hub_dev = hub->intfdev;
 		intf = to_usb_interface(hub_dev);
 		dev_dbg(hub_dev, "state %d ports %d chg %04x evt %04x\n",
@@ -4932,6 +5174,7 @@
 		usb_autopm_put_interface(intf);
  loop_disconnected:
 		usb_unlock_device(hdev);
+		usb_put_dev(hdev);
 		kref_put(&hub->kref, hub_release);
 
 	} /* end while (1) */
@@ -5314,7 +5557,9 @@
 	int i;
 	unsigned int noio_flag;
 	struct usb_host_config *config = udev->actconfig;
-
+#if defined(CONFIG_USB_PILOT4_HCD_MODULE) || defined(CONFIG_USB_PILOT4_HCD)
+	extern int print_ep_detail_based_on_devaddr(int dev_addr, char *manufacturer, int reason);
+#endif
 	if (udev->state == USB_STATE_NOTATTACHED ||
 			udev->state == USB_STATE_SUSPENDED) {
 		dev_dbg(&udev->dev, "device reset not allowed in state %d\n",
@@ -5354,7 +5599,10 @@
 			}
 		}
 	}
-
+#if defined(CONFIG_USB_PILOT4_HCD_MODULE) || defined(CONFIG_USB_PILOT4_HCD)
+	(void)print_ep_detail_based_on_devaddr(udev->devnum, udev->manufacturer, 1);
+	//dump_stack();
+#endif
 	ret = usb_reset_and_verify_device(udev);
 
 	if (config) {
