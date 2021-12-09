--- linux_org/net/ipv4/netfilter/arp_tables.c	2018-12-17 11:14:56.044022401 +0800
+++ linux/net/ipv4/netfilter/arp_tables.c	2018-12-19 11:44:40.360982279 +0800
@@ -362,6 +362,19 @@
 	return memcmp(arp, &uncond, sizeof(uncond)) == 0;
 }
 
+static bool find_jump_target(const struct xt_table_info *t,
+			     const void *entry0,
+			     const struct arpt_entry *target)
+{
+	struct arpt_entry *iter;
+
+	xt_entry_foreach(iter, entry0, t->size) {
+		 if (iter == target)
+			return true;
+	}
+	return false;
+}
+
 /* Figures out from what hook each rule can be called: returns 0 if
  * there are loops.  Puts hook bitmask in comefrom.
  */
@@ -435,6 +448,8 @@
 				size = e->next_offset;
 				e = (struct arpt_entry *)
 					(entry0 + pos + size);
+				if (pos + size >= newinfo->size)
+					return 0;
 				e->counters.pcnt = pos;
 				pos += size;
 			} else {
@@ -454,9 +469,15 @@
 					/* This a jump; chase it. */
 					duprintf("Jump rule %u -> %u\n",
 						 pos, newpos);
+					e = (struct arpt_entry *)
+						(entry0 + newpos);
+					if (!find_jump_target(newinfo, entry0, e))
+						return 0;
 				} else {
 					/* ... this is a fallthru */
 					newpos = pos + e->next_offset;
+					if (newpos >= newinfo->size)
+						return 0;
 				}
 				e = (struct arpt_entry *)
 					(entry0 + newpos);
@@ -470,25 +491,6 @@
 	return 1;
 }
 
-static inline int check_entry(const struct arpt_entry *e, const char *name)
-{
-	const struct xt_entry_target *t;
-
-	if (!arp_checkentry(&e->arp)) {
-		duprintf("arp_tables: arp check failed %p %s.\n", e, name);
-		return -EINVAL;
-	}
-
-	if (e->target_offset + sizeof(struct xt_entry_target) > e->next_offset)
-		return -EINVAL;
-
-	t = arpt_get_target_c(e);
-	if (e->target_offset + t->u.target_size > e->next_offset)
-		return -EINVAL;
-
-	return 0;
-}
-
 static inline int check_target(struct arpt_entry *e, const char *name)
 {
 	struct xt_entry_target *t = arpt_get_target(e);
@@ -518,10 +520,6 @@
 	struct xt_target *target;
 	int ret;
 
-	ret = check_entry(e, name);
-	if (ret)
-		return ret;
-
 	t = arpt_get_target(e);
 	target = xt_request_find_target(NFPROTO_ARP, t->u.user.name,
 					t->u.user.revision);
@@ -566,9 +564,11 @@
 					     unsigned int valid_hooks)
 {
 	unsigned int h;
-
+	int err;
+	
 	if ((unsigned long)e % __alignof__(struct arpt_entry) != 0 ||
-	    (unsigned char *)e + sizeof(struct arpt_entry) >= limit) {
+	    (unsigned char *)e + sizeof(struct arpt_entry) >= limit ||
+	    (unsigned char *)e + e->next_offset > limit) {
 		duprintf("Bad offset %p\n", e);
 		return -EINVAL;
 	}
@@ -580,6 +580,14 @@
 		return -EINVAL;
 	}
 
+	if (!arp_checkentry(&e->arp))
+		return -EINVAL;
+
+	err = xt_check_entry_offsets(e, e->elems, e->target_offset,
+				     e->next_offset);
+	if (err)
+		return err;
+	
 	/* Check hooks & underflows */
 	for (h = 0; h < NF_ARP_NUMHOOKS; h++) {
 		if (!(valid_hooks & (1 << h)))
@@ -680,10 +688,8 @@
 		}
 	}
 
-	if (!mark_source_chains(newinfo, repl->valid_hooks, entry0)) {
-		duprintf("Looping hook\n");
+	if (!mark_source_chains(newinfo, repl->valid_hooks, entry0))
 		return -ELOOP;
-	}
 
 	/* Finally, each sanity check must pass */
 	i = 0;
@@ -1116,56 +1122,18 @@
 	unsigned int i, curcpu;
 	struct xt_counters_info tmp;
 	struct xt_counters *paddc;
-	unsigned int num_counters;
-	const char *name;
-	int size;
-	void *ptmp;
 	struct xt_table *t;
 	const struct xt_table_info *private;
 	int ret = 0;
 	void *loc_cpu_entry;
 	struct arpt_entry *iter;
 	unsigned int addend;
-#ifdef CONFIG_COMPAT
-	struct compat_xt_counters_info compat_tmp;
 
-	if (compat) {
-		ptmp = &compat_tmp;
-		size = sizeof(struct compat_xt_counters_info);
-	} else
-#endif
-	{
-		ptmp = &tmp;
-		size = sizeof(struct xt_counters_info);
-	}
+	paddc = xt_copy_counters_from_user(user, len, &tmp, compat);
+	if (IS_ERR(paddc))
+		return PTR_ERR(paddc);
 
-	if (copy_from_user(ptmp, user, size) != 0)
-		return -EFAULT;
-
-#ifdef CONFIG_COMPAT
-	if (compat) {
-		num_counters = compat_tmp.num_counters;
-		name = compat_tmp.name;
-	} else
-#endif
-	{
-		num_counters = tmp.num_counters;
-		name = tmp.name;
-	}
-
-	if (len != size + num_counters * sizeof(struct xt_counters))
-		return -EINVAL;
-
-	paddc = vmalloc(len - size);
-	if (!paddc)
-		return -ENOMEM;
-
-	if (copy_from_user(paddc, user + size, len - size) != 0) {
-		ret = -EFAULT;
-		goto free;
-	}
-
-	t = xt_find_table_lock(net, NFPROTO_ARP, name);
+	t = xt_find_table_lock(net, NFPROTO_ARP, tmp.name);
 	if (IS_ERR_OR_NULL(t)) {
 		ret = t ? PTR_ERR(t) : -ENOENT;
 		goto free;
@@ -1173,7 +1141,7 @@
 
 	local_bh_disable();
 	private = t->private;
-	if (private->number != num_counters) {
+	if (private->number != tmp.num_counters) {
 		ret = -EINVAL;
 		goto unlock_up_free;
 	}
@@ -1199,6 +1167,18 @@
 }
 
 #ifdef CONFIG_COMPAT
+struct compat_arpt_replace {
+	char				name[XT_TABLE_MAXNAMELEN];
+	u32				valid_hooks;
+	u32				num_entries;
+	u32				size;
+	u32				hook_entry[NF_ARP_NUMHOOKS];
+	u32				underflow[NF_ARP_NUMHOOKS];
+	u32				num_counters;
+	compat_uptr_t			counters;
+	struct compat_arpt_entry	entries[0];
+};
+
 static inline void compat_release_entry(struct compat_arpt_entry *e)
 {
 	struct xt_entry_target *t;
@@ -1207,24 +1187,22 @@
 	module_put(t->u.kernel.target->me);
 }
 
-static inline int
+static int
 check_compat_entry_size_and_hooks(struct compat_arpt_entry *e,
 				  struct xt_table_info *newinfo,
 				  unsigned int *size,
 				  const unsigned char *base,
-				  const unsigned char *limit,
-				  const unsigned int *hook_entries,
-				  const unsigned int *underflows,
-				  const char *name)
+				  const unsigned char *limit)
 {
 	struct xt_entry_target *t;
 	struct xt_target *target;
 	unsigned int entry_offset;
-	int ret, off, h;
+	int ret, off;
 
 	duprintf("check_compat_entry_size_and_hooks %p\n", e);
 	if ((unsigned long)e % __alignof__(struct compat_arpt_entry) != 0 ||
-	    (unsigned char *)e + sizeof(struct compat_arpt_entry) >= limit) {
+	    (unsigned char *)e + sizeof(struct compat_arpt_entry) >= limit ||
+	    (unsigned char *)e + e->next_offset > limit) {
 		duprintf("Bad offset %p, limit = %p\n", e, limit);
 		return -EINVAL;
 	}
@@ -1236,8 +1214,11 @@
 		return -EINVAL;
 	}
 
-	/* For purposes of check_entry casting the compat entry is fine */
-	ret = check_entry((struct arpt_entry *)e, name);
+	if (!arp_checkentry(&e->arp))
+		return -EINVAL;
+
+	ret = xt_compat_check_entry_offsets(e, e->elems, e->target_offset,
+					    e->next_offset);
 	if (ret)
 		return ret;
 
@@ -1261,17 +1242,6 @@
 	if (ret)
 		goto release_target;
 
-	/* Check hooks & underflows */
-	for (h = 0; h < NF_ARP_NUMHOOKS; h++) {
-		if ((unsigned char *)e - base == hook_entries[h])
-			newinfo->hook_entry[h] = hook_entries[h];
-		if ((unsigned char *)e - base == underflows[h])
-			newinfo->underflow[h] = underflows[h];
-	}
-
-	/* Clear counters and comefrom */
-	memset(&e->counters, 0, sizeof(e->counters));
-	e->comefrom = 0;
 	return 0;
 
 release_target:
@@ -1280,18 +1250,17 @@
 	return ret;
 }
 
-static int
+static void
 compat_copy_entry_from_user(struct compat_arpt_entry *e, void **dstptr,
-			    unsigned int *size, const char *name,
+			    unsigned int *size,
 			    struct xt_table_info *newinfo, unsigned char *base)
 {
 	struct xt_entry_target *t;
 	struct xt_target *target;
 	struct arpt_entry *de;
 	unsigned int origsize;
-	int ret, h;
+	int h;
 
-	ret = 0;
 	origsize = *size;
 	de = (struct arpt_entry *)*dstptr;
 	memcpy(de, e, sizeof(struct arpt_entry));
@@ -1312,144 +1281,80 @@
 		if ((unsigned char *)de - base < newinfo->underflow[h])
 			newinfo->underflow[h] -= origsize - *size;
 	}
-	return ret;
 }
 
-static int translate_compat_table(const char *name,
-				  unsigned int valid_hooks,
-				  struct xt_table_info **pinfo,
+static int translate_compat_table(struct xt_table_info **pinfo,
 				  void **pentry0,
-				  unsigned int total_size,
-				  unsigned int number,
-				  unsigned int *hook_entries,
-				  unsigned int *underflows)
+				  const struct compat_arpt_replace *compatr)
 {
 	unsigned int i, j;
 	struct xt_table_info *newinfo, *info;
 	void *pos, *entry0, *entry1;
 	struct compat_arpt_entry *iter0;
-	struct arpt_entry *iter1;
+	struct arpt_replace repl;
 	unsigned int size;
 	int ret = 0;
 
 	info = *pinfo;
 	entry0 = *pentry0;
-	size = total_size;
-	info->number = number;
-
-	/* Init all hooks to impossible value. */
-	for (i = 0; i < NF_ARP_NUMHOOKS; i++) {
-		info->hook_entry[i] = 0xFFFFFFFF;
-		info->underflow[i] = 0xFFFFFFFF;
-	}
+	size = compatr->size;
+	info->number = compatr->num_entries;
 
 	duprintf("translate_compat_table: size %u\n", info->size);
 	j = 0;
 	xt_compat_lock(NFPROTO_ARP);
-	xt_compat_init_offsets(NFPROTO_ARP, number);
+	xt_compat_init_offsets(NFPROTO_ARP, compatr->num_entries);
 	/* Walk through entries, checking offsets. */
-	xt_entry_foreach(iter0, entry0, total_size) {
+	xt_entry_foreach(iter0, entry0, compatr->size) {
 		ret = check_compat_entry_size_and_hooks(iter0, info, &size,
 							entry0,
-							entry0 + total_size,
-							hook_entries,
-							underflows,
-							name);
+							entry0 + compatr->size);
 		if (ret != 0)
 			goto out_unlock;
 		++j;
 	}
 
 	ret = -EINVAL;
-	if (j != number) {
+	if (j != compatr->num_entries) {
 		duprintf("translate_compat_table: %u not %u entries\n",
-			 j, number);
+			 j, compatr->num_entries);
 		goto out_unlock;
 	}
 
-	/* Check hooks all assigned */
-	for (i = 0; i < NF_ARP_NUMHOOKS; i++) {
-		/* Only hooks which are valid */
-		if (!(valid_hooks & (1 << i)))
-			continue;
-		if (info->hook_entry[i] == 0xFFFFFFFF) {
-			duprintf("Invalid hook entry %u %u\n",
-				 i, hook_entries[i]);
-			goto out_unlock;
-		}
-		if (info->underflow[i] == 0xFFFFFFFF) {
-			duprintf("Invalid underflow %u %u\n",
-				 i, underflows[i]);
-			goto out_unlock;
-		}
-	}
-
 	ret = -ENOMEM;
 	newinfo = xt_alloc_table_info(size);
 	if (!newinfo)
 		goto out_unlock;
 
-	newinfo->number = number;
+	newinfo->number = compatr->num_entries;
 	for (i = 0; i < NF_ARP_NUMHOOKS; i++) {
 		newinfo->hook_entry[i] = info->hook_entry[i];
 		newinfo->underflow[i] = info->underflow[i];
 	}
 	entry1 = newinfo->entries[raw_smp_processor_id()];
 	pos = entry1;
-	size = total_size;
-	xt_entry_foreach(iter0, entry0, total_size) {
-		ret = compat_copy_entry_from_user(iter0, &pos, &size,
-						  name, newinfo, entry1);
-		if (ret != 0)
-			break;
-	}
+	size = compatr->size;
+	xt_entry_foreach(iter0, entry0, compatr->size)
+		compat_copy_entry_from_user(iter0, &pos, &size,
+					    newinfo, entry1);
+						
+	/* all module references in entry0 are now gone */
+						
 	xt_compat_flush_offsets(NFPROTO_ARP);
 	xt_compat_unlock(NFPROTO_ARP);
-	if (ret)
-		goto free_newinfo;
-
-	ret = -ELOOP;
-	if (!mark_source_chains(newinfo, valid_hooks, entry1))
-		goto free_newinfo;
 
-	i = 0;
-	xt_entry_foreach(iter1, entry1, newinfo->size) {
-		ret = check_target(iter1, name);
-		if (ret != 0)
-			break;
-		++i;
-		if (strcmp(arpt_get_target(iter1)->u.user.name,
-		    XT_ERROR_TARGET) == 0)
-			++newinfo->stacksize;
-	}
-	if (ret) {
-		/*
-		 * The first i matches need cleanup_entry (calls ->destroy)
-		 * because they had called ->check already. The other j-i
-		 * entries need only release.
-		 */
-		int skip = i;
-		j -= i;
-		xt_entry_foreach(iter0, entry0, newinfo->size) {
-			if (skip-- > 0)
-				continue;
-			if (j-- == 0)
-				break;
-			compat_release_entry(iter0);
-		}
-		xt_entry_foreach(iter1, entry1, newinfo->size) {
-			if (i-- == 0)
-				break;
-			cleanup_entry(iter1);
-		}
-		xt_free_table_info(newinfo);
-		return ret;
+	memcpy(&repl, compatr, sizeof(*compatr));
+	for (i = 0; i < NF_ARP_NUMHOOKS; i++) {
+		repl.hook_entry[i] = newinfo->hook_entry[i];
+		repl.underflow[i] = newinfo->underflow[i];
 	}
 
-	/* And one copy for every other CPU */
-	for_each_possible_cpu(i)
-		if (newinfo->entries[i] && newinfo->entries[i] != entry1)
-			memcpy(newinfo->entries[i], entry1, newinfo->size);
+	repl.num_counters = 0;
+	repl.counters = NULL;
+	repl.size = newinfo->size;
+	ret = translate_table(newinfo, entry1, &repl);
+	if (ret)
+		goto free_newinfo;
 
 	*pinfo = newinfo;
 	*pentry0 = entry1;
@@ -1458,31 +1363,18 @@
 
 free_newinfo:
 	xt_free_table_info(newinfo);
-out:
-	xt_entry_foreach(iter0, entry0, total_size) {
+	return ret;
+out_unlock:
+	xt_compat_flush_offsets(NFPROTO_ARP);
+	xt_compat_unlock(NFPROTO_ARP);
+	xt_entry_foreach(iter0, entry0, compatr->size) {
 		if (j-- == 0)
 			break;
 		compat_release_entry(iter0);
 	}
 	return ret;
-out_unlock:
-	xt_compat_flush_offsets(NFPROTO_ARP);
-	xt_compat_unlock(NFPROTO_ARP);
-	goto out;
 }
 
-struct compat_arpt_replace {
-	char				name[XT_TABLE_MAXNAMELEN];
-	u32				valid_hooks;
-	u32				num_entries;
-	u32				size;
-	u32				hook_entry[NF_ARP_NUMHOOKS];
-	u32				underflow[NF_ARP_NUMHOOKS];
-	u32				num_counters;
-	compat_uptr_t			counters;
-	struct compat_arpt_entry	entries[0];
-};
-
 static int compat_do_replace(struct net *net, void __user *user,
 			     unsigned int len)
 {
@@ -1513,10 +1405,7 @@
 		goto free_newinfo;
 	}
 
-	ret = translate_compat_table(tmp.name, tmp.valid_hooks,
-				     &newinfo, &loc_cpu_entry, tmp.size,
-				     tmp.num_entries, tmp.hook_entry,
-				     tmp.underflow);
+	ret = translate_compat_table(&newinfo, &loc_cpu_entry, &tmp);
 	if (ret != 0)
 		goto free_newinfo;
 
