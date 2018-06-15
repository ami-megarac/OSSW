--- uboot_old/common/main.c	2017-05-03 12:51:43.432279485 +0530
+++ uboot/common/main.c	2017-05-03 12:55:13.319393660 +0530
@@ -50,6 +50,9 @@
 void inline __show_boot_progress (int val) {}
 void show_boot_progress (int val) __attribute__((weak, alias("__show_boot_progress")));
 
+extern void platformSpecificInit(void);
+extern int Disable_watchdog(void);
+
 #define MAX_DELAY_STOP_STR 32
 
 #define DEBUG_PARSER	0	/* set to 1 to debug */
@@ -382,6 +385,9 @@
 	init_cmd_timeout ();
 # endif	/* CONFIG_BOOT_RETRY_TIME */
 
+    /* Initialize or sets all platform specific registers and tasks.............*/
+    platformSpecificInit();
+
 #ifdef CONFIG_POST
 	if (gd->flags & GD_FLG_POSTFAIL) {
 		s = getenv("failbootcmd");
@@ -1164,6 +1170,8 @@
 	int	col;				/* output column cnt	*/
 	char	c;
 
+	// disable all watchdog
+	Disable_watchdog();
 	/* print prompt */
 	if (prompt) {
 		plen = strlen (prompt);
