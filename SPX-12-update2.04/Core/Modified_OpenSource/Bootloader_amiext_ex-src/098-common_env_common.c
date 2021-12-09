--- uboot_orig/common/env_common.c	2016-03-30 16:13:17.640812598 +0530
+++ uboot/common/env_common.c	2016-03-31 15:36:49.736630628 +0530
@@ -143,6 +143,7 @@
 		error("Environment import failed: errno = %d\n", errno);
 
 	gd->flags |= GD_FLG_ENV_READY;
+	saveenv();  // Saving the default environment into permanent storage.
 }
 
 
