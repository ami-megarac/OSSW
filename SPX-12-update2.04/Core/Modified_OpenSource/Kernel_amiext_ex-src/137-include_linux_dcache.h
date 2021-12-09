--- linux_org/include/linux/dcache.h	2019-01-29 18:32:50.439227210 +0800
+++ linux/include/linux/dcache.h	2019-01-29 18:36:42.427251230 +0800
@@ -465,4 +465,11 @@
 {
 	return mult_frac(val, sysctl_vfs_cache_pressure, 100);
 }
+struct name_snapshot {
+	const char *name;
+	char inline_name[DNAME_INLINE_LEN];
+};
+void take_dentry_name_snapshot(struct name_snapshot *, struct dentry *);
+void release_dentry_name_snapshot(struct name_snapshot *);
+
 #endif	/* __LINUX_DCACHE_H */
