--- uboot_orig/common/env_common.c	2016-03-15 19:08:38.193798283 +0530
+++ uboot/common/env_common.c	2016-03-16 17:40:06.788393128 +0530
@@ -34,6 +34,8 @@
 
 DECLARE_GLOBAL_DATA_PTR;
 
+int gDefaultEnvironment = 0;
+
 /************************************************************************
  * Default settings to be used when no valid environment is found
  */
@@ -185,6 +187,12 @@
 	if (himport_r(&env_htab, (char *)ep->data, ENV_SIZE, '\0', 0,
 			0, NULL)) {
 		gd->flags |= GD_FLG_ENV_READY;
+		/* 
+		 * gd(global data) structure has pointer to environment variable array.
+		 * It always point to default_environment assuming it is running from final 
+		 * location.So set gd->env_addr to point to current environment data.
+		*/
+		gd->env_addr = (unsigned long)ep->data;
 		return 1;
 	}
 
@@ -198,6 +206,7 @@
 
 void env_relocate(void)
 {
+	gDefaultEnvironment = 0;
 #if defined(CONFIG_NEEDS_MANUAL_RELOC)
 	env_reloc();
 	env_htab.change_ok += gd->reloc_off;
@@ -207,6 +216,7 @@
 		/* Environment not changable */
 		set_default_env(NULL);
 #else
+		gDefaultEnvironment = 1;
 		bootstage_error(BOOTSTAGE_ID_NET_CHECKSUM);
 		set_default_env("!bad CRC");
 #endif
