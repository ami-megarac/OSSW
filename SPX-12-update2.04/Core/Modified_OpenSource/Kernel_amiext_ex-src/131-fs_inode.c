--- linux_org/fs/inode.c	2019-01-30 13:05:10.462973816 +0800
+++ linux/fs/inode.c	2019-01-30 13:05:44.222719587 +0800
@@ -1828,8 +1828,13 @@
 	inode->i_uid = current_fsuid();
 	if (dir && dir->i_mode & S_ISGID) {
 		inode->i_gid = dir->i_gid;
+		/* Directories are special, and always inherit S_ISGID */
 		if (S_ISDIR(mode))
 			mode |= S_ISGID;
+		else if ((mode & (S_ISGID | S_IXGRP)) == (S_ISGID | S_IXGRP) &&
+			 !in_group_p(inode->i_gid) &&
+			 !capable_wrt_inode_uidgid(dir, CAP_FSETID))
+			mode &= ~S_ISGID;		
 	} else
 		inode->i_gid = current_fsgid();
 	inode->i_mode = mode;
