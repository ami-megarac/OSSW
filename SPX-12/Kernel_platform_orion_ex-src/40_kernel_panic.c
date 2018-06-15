--- linux/kernel/panic.c	2017-03-31 13:54:17.879272000 +0800
+++ linux/kernel/panic.c	2017-03-29 19:51:13.950079000 +0800
@@ -176,6 +176,11 @@
 		disabled_wait(caller);
 	}
 #endif
+	/*SDK Update - BMC CPU Reset Workaround*/
+	printk("PANIC-WFE\n");
+	while(1)
+		wfe();//SDK Update
+
 	local_irq_enable();
 	for (i = 0; ; i += PANIC_TIMER_STEP) {
 		touch_softlockup_watchdog();
