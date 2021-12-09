--- linux_org/fs/ext4/super.c	2019-01-30 10:56:50.415333130 +0800
+++ linux/fs/ext4/super.c	2019-01-30 14:53:24.577845829 +0800
@@ -2068,11 +2068,13 @@
 
 /* Called at mount-time, super-block is locked */
 static int ext4_check_descriptors(struct super_block *sb,
+				  ext4_fsblk_t sb_block,
 				  ext4_group_t *first_not_zeroed)
 {
 	struct ext4_sb_info *sbi = EXT4_SB(sb);
 	ext4_fsblk_t first_block = le32_to_cpu(sbi->s_es->s_first_data_block);
 	ext4_fsblk_t last_block;
+	ext4_fsblk_t last_bg_block = sb_block + ext4_bg_num_gdb(sb, 0) + 1;
 	ext4_fsblk_t block_bitmap;
 	ext4_fsblk_t inode_bitmap;
 	ext4_fsblk_t inode_table;
@@ -2096,8 +2098,15 @@
 		if ((grp == sbi->s_groups_count) &&
 		   !(gdp->bg_flags & cpu_to_le16(EXT4_BG_INODE_ZEROED)))
 			grp = i;
-
 		block_bitmap = ext4_block_bitmap(sb, gdp);
+		if (block_bitmap >= sb_block + 1 &&
+		    block_bitmap <= last_bg_block) {
+			ext4_msg(sb, KERN_ERR, "ext4_check_descriptors: "
+				 "Block bitmap for group %u overlaps "
+				 "block group descriptors", i);
+			if (!(sb->s_flags & MS_RDONLY))
+				return 0;
+		}		
 		if (block_bitmap < first_block || block_bitmap > last_block) {
 			ext4_msg(sb, KERN_ERR, "ext4_check_descriptors: "
 			       "Block bitmap for group %u not in group "
@@ -2105,6 +2114,14 @@
 			return 0;
 		}
 		inode_bitmap = ext4_inode_bitmap(sb, gdp);
+		if (inode_bitmap >= sb_block + 1 &&
+		    inode_bitmap <= last_bg_block) {
+			ext4_msg(sb, KERN_ERR, "ext4_check_descriptors: "
+				 "Inode bitmap for group %u overlaps "
+				 "block group descriptors", i);
+			if (!(sb->s_flags & MS_RDONLY))
+				return 0;
+		}		
 		if (inode_bitmap < first_block || inode_bitmap > last_block) {
 			ext4_msg(sb, KERN_ERR, "ext4_check_descriptors: "
 			       "Inode bitmap for group %u not in group "
@@ -2112,6 +2129,14 @@
 			return 0;
 		}
 		inode_table = ext4_inode_table(sb, gdp);
+		if (inode_table >= sb_block + 1 &&
+		    inode_table <= last_bg_block) {
+			ext4_msg(sb, KERN_ERR, "ext4_check_descriptors: "
+				 "Inode table for group %u overlaps "
+				 "block group descriptors", i);
+			if (!(sb->s_flags & MS_RDONLY))
+				return 0;
+		}		
 		if (inode_table < first_block ||
 		    inode_table + sbi->s_itb_per_group - 1 > last_block) {
 			ext4_msg(sb, KERN_ERR, "ext4_check_descriptors: "
@@ -3853,7 +3878,7 @@
 			goto failed_mount2;
 		}
 	}
-	if (!ext4_check_descriptors(sb, &first_not_zeroed)) {
+	if (!ext4_check_descriptors(sb, logical_sb_block, &first_not_zeroed)) {
 		ext4_msg(sb, KERN_ERR, "group descriptors corrupted!");
 		goto failed_mount2;
 	}
