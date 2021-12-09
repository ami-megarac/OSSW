--- linux_org/fs/namei.c	2019-01-29 18:32:35.867837693 +0800
+++ linux/fs/namei.c	2019-01-29 18:35:02.205023428 +0800
@@ -4125,9 +4125,9 @@
 	       struct inode **delegated_inode)
 {
 	int error;
-	int is_dir = d_is_directory(old_dentry) || d_is_autodir(old_dentry);
-	const unsigned char *old_name;
-
+	int is_dir = d_is_directory(old_dentry) || d_is_autodir(old_dentry);	
+	struct name_snapshot old_name;
+	
 	if (old_dentry->d_inode == new_dentry->d_inode)
  		return 0;
  
@@ -4145,16 +4145,16 @@
 	if (!old_dir->i_op->rename)
 		return -EPERM;
 
-	old_name = fsnotify_oldname_init(old_dentry->d_name.name);
+	take_dentry_name_snapshot(&old_name, old_dentry);
 
 	if (is_dir)
 		error = vfs_rename_dir(old_dir,old_dentry,new_dir,new_dentry);
 	else
 		error = vfs_rename_other(old_dir,old_dentry,new_dir,new_dentry,delegated_inode);
 	if (!error)
-		fsnotify_move(old_dir, new_dir, old_name, is_dir,
+		fsnotify_move(old_dir, new_dir, old_name.name, is_dir,
 			      new_dentry->d_inode, old_dentry);
-	fsnotify_oldname_free(old_name);
+	release_dentry_name_snapshot(&old_name);
 
 	return error;
 }
