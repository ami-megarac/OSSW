--- linux_org/fs/ext4/xattr.c	2019-01-30 10:50:26.441708808 +0800
+++ linux/fs/ext4/xattr.c	2019-01-30 15:06:38.797433584 +0800
@@ -593,14 +593,20 @@
 }
 
 static int
-ext4_xattr_set_entry(struct ext4_xattr_info *i, struct ext4_xattr_search *s)
+ext4_xattr_set_entry(struct ext4_xattr_info *i, struct ext4_xattr_search *s,
+						struct inode *inode)
 {
-	struct ext4_xattr_entry *last;
+	struct ext4_xattr_entry *last, *next;
 	size_t free, min_offs = s->end - s->base, name_len = strlen(i->name);
 
 	/* Compute min_offs and last. */
 	last = s->first;
-	for (; !IS_LAST_ENTRY(last); last = EXT4_XATTR_NEXT(last)) {
+	for (; !IS_LAST_ENTRY(last); last = next) {
+		next = EXT4_XATTR_NEXT(last);
+		if ((void *)next >= s->end) {
+			EXT4_ERROR_INODE(inode, "corrupted xattr entries");
+			return -EIO;
+		}
 		if (!last->e_value_block && last->e_value_size) {
 			size_t offs = le16_to_cpu(last->e_value_offs);
 			if (offs < min_offs)
@@ -779,7 +785,7 @@
 				ce = NULL;
 			}
 			ea_bdebug(bs->bh, "modifying in-place");
-			error = ext4_xattr_set_entry(i, s);
+			error = ext4_xattr_set_entry(i, s, inode);
 			if (!error) {
 				if (!IS_LAST_ENTRY(s->first))
 					ext4_xattr_rehash(header(s->base),
@@ -830,7 +836,7 @@
 		s->end = s->base + sb->s_blocksize;
 	}
 
-	error = ext4_xattr_set_entry(i, s);
+	error = ext4_xattr_set_entry(i, s, inode);
 	if (error == -EIO)
 		goto bad_block;
 	if (error)
@@ -996,7 +1002,7 @@
 
 	if (EXT4_I(inode)->i_extra_isize == 0)
 		return -ENOSPC;
-	error = ext4_xattr_set_entry(i, s);
+	error = ext4_xattr_set_entry(i, s, inode);
 	if (error) {
 		if (error == -ENOSPC &&
 		    ext4_has_inline_data(inode)) {
@@ -1008,7 +1014,7 @@
 			error = ext4_xattr_ibody_find(inode, i, is);
 			if (error)
 				return error;
-			error = ext4_xattr_set_entry(i, s);
+			error = ext4_xattr_set_entry(i, s, inode);
 		}
 		if (error)
 			return error;
@@ -1034,7 +1040,7 @@
 
 	if (EXT4_I(inode)->i_extra_isize == 0)
 		return -ENOSPC;
-	error = ext4_xattr_set_entry(i, s);
+	error = ext4_xattr_set_entry(i, s, inode);
 	if (error)
 		return error;
 	header = IHDR(inode, ext4_raw_inode(&is->iloc));
