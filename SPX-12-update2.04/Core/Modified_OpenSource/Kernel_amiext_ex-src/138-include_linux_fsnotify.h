--- linux_org/include/linux/fsnotify.h	2019-01-29 18:36:24.526191616 +0800
+++ linux/include/linux/fsnotify.h	2019-01-29 18:37:19.481781273 +0800
@@ -308,35 +308,4 @@
 	}
 }
 
-#if defined(CONFIG_FSNOTIFY)	/* notify helpers */
-
-/*
- * fsnotify_oldname_init - save off the old filename before we change it
- */
-static inline const unsigned char *fsnotify_oldname_init(const unsigned char *name)
-{
-	return kstrdup(name, GFP_KERNEL);
-}
-
-/*
- * fsnotify_oldname_free - free the name we got from fsnotify_oldname_init
- */
-static inline void fsnotify_oldname_free(const unsigned char *old_name)
-{
-	kfree(old_name);
-}
-
-#else	/* CONFIG_FSNOTIFY */
-
-static inline const char *fsnotify_oldname_init(const unsigned char *name)
-{
-	return NULL;
-}
-
-static inline void fsnotify_oldname_free(const unsigned char *old_name)
-{
-}
-
-#endif	/*  CONFIG_FSNOTIFY */
-
 #endif	/* _LINUX_FS_NOTIFY_H */
