--- linux/include/linux/usb/hcd.h	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/include/linux/usb/hcd.h	2016-12-19 16:11:56.239414544 +0800
@@ -447,6 +447,7 @@
 #endif /* CONFIG_PCI */
 
 /* pci-ish (pdev null is ok) buffer alloc/mapping support */
+void usb_init_pool_max(void);
 int hcd_buffer_create(struct usb_hcd *hcd);
 void hcd_buffer_destroy(struct usb_hcd *hcd);
 
