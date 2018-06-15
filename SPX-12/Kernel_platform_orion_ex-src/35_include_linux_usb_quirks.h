--- linux/include/linux/usb/quirks.h	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/include/linux/usb/quirks.h	2016-12-19 16:13:35.131414544 +0800
@@ -30,4 +30,10 @@
    descriptor */
 #define USB_QUIRK_DELAY_INIT		0x00000040
 
+/* device generates spurious wakeup, ignore remote wakeup capability */
+#define USB_QUIRK_IGNORE_REMOTE_WAKEUP	0x00000200
+
+/* device can't handle device_qualifier descriptor requests */
+#define USB_QUIRK_DEVICE_QUALIFIER	0x00000100
+
 #endif /* __LINUX_USB_QUIRKS_H */
