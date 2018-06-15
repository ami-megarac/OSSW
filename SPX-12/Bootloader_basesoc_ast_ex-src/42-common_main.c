--- uboot_old/common/main.c	2017-01-26 16:50:42.316749792 +0530
+++ uboot/common/main.c	2017-01-26 16:47:17.848750244 +0530
@@ -40,6 +40,8 @@
 
 DECLARE_GLOBAL_DATA_PTR;
 
+extern int Disable_watchdog(void);
+
 #ifdef CONFIG_SIMULATE_ACPI
 void SimulateACPI(void);	
 #endif 
@@ -1138,6 +1140,7 @@
 	int rc;
 	static int initted = 0;
 
+
 	/*
 	 * History uses a global array which is not
 	 * writable until after relocation to RAM.
@@ -1164,6 +1167,10 @@
 	int	col;				/* output column cnt	*/
 	char	c;
 
+#ifdef CONFIG_SPX_FEATURE_HW_FAILSAFE_BOOT
+        Disable_watchdog();
+#endif
+
 	/* print prompt */
 	if (prompt) {
 		plen = strlen (prompt);
