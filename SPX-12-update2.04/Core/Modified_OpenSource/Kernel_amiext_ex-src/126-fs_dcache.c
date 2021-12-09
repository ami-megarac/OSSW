--- linux_org/fs/dcache.c	2019-01-29 18:30:35.928866361 +0800
+++ linux/fs/dcache.c	2019-01-29 18:33:48.799479431 +0800
@@ -246,6 +246,43 @@
 	kmem_cache_free(dentry_cache, dentry); 
 }
 
+void take_dentry_name_snapshot(struct name_snapshot *name, struct dentry *dentry)
+{
+	spin_lock(&dentry->d_lock);
+	if (unlikely(dname_external(dentry))) {
+		u32 len;
+		char *p;
+
+		for (;;) {
+			len = dentry->d_name.len;
+			spin_unlock(&dentry->d_lock);
+
+			p = kmalloc(len + 1, GFP_KERNEL | __GFP_NOFAIL);
+
+			spin_lock(&dentry->d_lock);
+			if (dentry->d_name.len <= len)
+				break;
+			kfree(p);
+		}
+		memcpy(p, dentry->d_name.name, dentry->d_name.len + 1);
+		spin_unlock(&dentry->d_lock);
+
+		name->name = p;
+	} else {
+		memcpy(name->inline_name, dentry->d_iname, DNAME_INLINE_LEN);
+		spin_unlock(&dentry->d_lock);
+		name->name = name->inline_name;
+	}
+}
+EXPORT_SYMBOL(take_dentry_name_snapshot);
+
+void release_dentry_name_snapshot(struct name_snapshot *name)
+{
+	if (unlikely(name->name != name->inline_name))
+		kfree(name->name);
+}
+EXPORT_SYMBOL(release_dentry_name_snapshot);
+
 /*
  * no locks, please.
  */
