--- linux/drivers/usb/core/devio.c	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/drivers/usb/core/devio.c	2016-12-19 15:44:44.403414544 +0800
@@ -1411,7 +1411,7 @@
 	u = (is_in ? URB_DIR_IN : URB_DIR_OUT);
 	if (uurb->flags & USBDEVFS_URB_ISO_ASAP)
 		u |= URB_ISO_ASAP;
-	if (uurb->flags & USBDEVFS_URB_SHORT_NOT_OK)
+	if (uurb->flags & USBDEVFS_URB_SHORT_NOT_OK && is_in)
 		u |= URB_SHORT_NOT_OK;
 	if (uurb->flags & USBDEVFS_URB_NO_FSBR)
 		u |= URB_NO_FSBR;
