--- linux/drivers/usb/core/quirks.c	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/drivers/usb/core/quirks.c	2016-12-19 15:48:24.063414544 +0800
@@ -44,6 +44,9 @@
 	/* Creative SB Audigy 2 NX */
 	{ USB_DEVICE(0x041e, 0x3020), .driver_info = USB_QUIRK_RESET_RESUME },
 
+	/* Microsoft Wireless Laser Mouse 6000 Receiver */
+	{ USB_DEVICE(0x045e, 0x00e1), .driver_info = USB_QUIRK_RESET_RESUME },
+
 	/* Microsoft LifeCam-VX700 v2.0 */
 	{ USB_DEVICE(0x045e, 0x0770), .driver_info = USB_QUIRK_RESET_RESUME },
 
@@ -93,6 +96,16 @@
 	{ USB_DEVICE(0x04e8, 0x6601), .driver_info =
 			USB_QUIRK_CONFIG_INTF_STRINGS },
 
+	/* Elan Touchscreen */
+	{ USB_DEVICE(0x04f3, 0x0089), .driver_info =
+			USB_QUIRK_DEVICE_QUALIFIER },
+
+	{ USB_DEVICE(0x04f3, 0x009b), .driver_info =
+			USB_QUIRK_DEVICE_QUALIFIER },
+
+	{ USB_DEVICE(0x04f3, 0x016f), .driver_info =
+			USB_QUIRK_DEVICE_QUALIFIER },
+
 	/* Roland SC-8820 */
 	{ USB_DEVICE(0x0582, 0x0007), .driver_info = USB_QUIRK_RESET_RESUME },
 
@@ -160,6 +173,10 @@
 	{ USB_VENDOR_AND_INTERFACE_INFO(0x046d, USB_CLASS_VIDEO, 1, 0),
 	  .driver_info = USB_QUIRK_RESET_RESUME },
 
+	/* ASUS Base Station(T100) */
+	{ USB_DEVICE(0x0b05, 0x17e0), .driver_info =
+			USB_QUIRK_IGNORE_REMOTE_WAKEUP },
+
 	{ }  /* terminating entry must be last */
 };
 
