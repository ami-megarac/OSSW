--- linux/drivers/usb/core/usb.c	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/drivers/usb/core/usb.c	2016-12-19 15:49:32.059414544 +0800
@@ -1050,6 +1050,7 @@
 		pr_info("%s: USB support disabled\n", usbcore_name);
 		return 0;
 	}
+	usb_init_pool_max();
 
 	retval = usb_debugfs_init();
 	if (retval)
