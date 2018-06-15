--- u-boot/drivers/serial/serial.c	2013-07-23 17:28:13.000000000 +0530
+++ u-boot.new/drivers/serial/serial.c	2014-07-23 14:30:31.698255145 +0530
@@ -175,6 +175,7 @@
 serial_initfunc(s3c44b0_serial_initialize);
 serial_initfunc(sa1100_serial_initialize);
 serial_initfunc(sh_serial_initialize);
+serial_initfunc(pilot_serial_initialize);
 
 /**
  * serial_register() - Register serial driver with serial driver core
@@ -267,6 +268,7 @@
 	s3c44b0_serial_initialize();
 	sa1100_serial_initialize();
 	sh_serial_initialize();
+	pilot_serial_initialize();
 
 	serial_assign(default_serial_console()->name);
 }
