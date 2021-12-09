--- linux_org/fs/ext4/balloc.c	2019-01-30 10:57:00.550951718 +0800
+++ linux/fs/ext4/balloc.c	2019-01-30 11:10:27.437267842 +0800
@@ -183,7 +183,6 @@
 	unsigned int bit, bit_max;
 	struct ext4_sb_info *sbi = EXT4_SB(sb);
 	ext4_fsblk_t start, tmp;
-	int flex_bg = 0;
 	struct ext4_group_info *grp;
 
 	J_ASSERT_BH(bh, buffer_locked(bh));
@@ -205,22 +204,19 @@
 
 	start = ext4_group_first_block_no(sb, block_group);
 
-	if (EXT4_HAS_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_FLEX_BG))
-		flex_bg = 1;
-
 	/* Set bits for block and inode bitmaps, and inode table */
 	tmp = ext4_block_bitmap(sb, gdp);
-	if (!flex_bg || ext4_block_in_group(sb, tmp, block_group))
+	if (ext4_block_in_group(sb, tmp, block_group))
 		ext4_set_bit(EXT4_B2C(sbi, tmp - start), bh->b_data);
 
 	tmp = ext4_inode_bitmap(sb, gdp);
-	if (!flex_bg || ext4_block_in_group(sb, tmp, block_group))
+	if (ext4_block_in_group(sb, tmp, block_group))
 		ext4_set_bit(EXT4_B2C(sbi, tmp - start), bh->b_data);
 
 	tmp = ext4_inode_table(sb, gdp);
 	for (; tmp < ext4_inode_table(sb, gdp) +
 		     sbi->s_itb_per_group; tmp++) {
-		if (!flex_bg || ext4_block_in_group(sb, tmp, block_group))
+		if (ext4_block_in_group(sb, tmp, block_group))
 			ext4_set_bit(EXT4_B2C(sbi, tmp - start), bh->b_data);
 	}
 
