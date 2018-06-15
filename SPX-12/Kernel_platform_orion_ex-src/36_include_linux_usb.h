--- linux/include/linux/usb.h	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/include/linux/usb.h	2016-12-19 16:14:39.515414544 +0800
@@ -581,6 +581,9 @@
 	struct usb3_lpm_parameters u1_params;
 	struct usb3_lpm_parameters u2_params;
 	unsigned lpm_disable_count;
+#if defined(CONFIG_USB_PILOT4_HCD) || defined(CONFIG_USB_PILOT4_HCD_MODULE)
+        u8 rqrd_bulk_in_eps, rqrd_bulk_out_eps, rqrd_intr_in_eps, rqrd_intr_out_eps;
+#endif
 };
 #define	to_usb_device(d) container_of(d, struct usb_device, dev)
 
