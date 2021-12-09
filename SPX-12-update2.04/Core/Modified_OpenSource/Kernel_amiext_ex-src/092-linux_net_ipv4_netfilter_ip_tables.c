--- linux_org/net/ipv4/netfilter/ip_tables.c	2018-12-17 11:14:56.048022154 +0800
+++ linux/net/ipv4/netfilter/ip_tables.c	2018-12-19 11:48:46.571206734 +0800
@@ -439,6 +439,19 @@
 #endif
 }
 
+static bool find_jump_target(const struct xt_table_info *t,
+			     const void *entry0,
+			     const struct ipt_entry *target)
+{
+	struct ipt_entry *iter;
+
+	xt_entry_foreach(iter, entry0, t->size) {
+		 if (iter == target)
+			return true;
+	}
+	return false;
+}
+
 /* Figures out from what hook each rule can be called: returns 0 if
    there are loops.  Puts hook bitmask in comefrom. */
 static int
@@ -517,6 +530,8 @@
 				size = e->next_offset;
 				e = (struct ipt_entry *)
 					(entry0 + pos + size);
+				if (pos + size >= newinfo->size)
+					return 0;
 				e->counters.pcnt = pos;
 				pos += size;
 			} else {
@@ -535,9 +550,15 @@
 					/* This a jump; chase it. */
 					duprintf("Jump rule %u -> %u\n",
 						 pos, newpos);
+					e = (struct ipt_entry *)
+						(entry0 + newpos);
+					if (!find_jump_target(newinfo, entry0, e))
+						return 0;
 				} else {
 					/* ... this is a fallthru */
 					newpos = pos + e->next_offset;
+					if (newpos >= newinfo->size)
+						return 0;
 				}
 				e = (struct ipt_entry *)
 					(entry0 + newpos);
@@ -565,27 +586,6 @@
 }
 
 static int
-check_entry(const struct ipt_entry *e, const char *name)
-{
-	const struct xt_entry_target *t;
-
-	if (!ip_checkentry(&e->ip)) {
-		duprintf("ip check failed %p %s.\n", e, name);
-		return -EINVAL;
-	}
-
-	if (e->target_offset + sizeof(struct xt_entry_target) >
-	    e->next_offset)
-		return -EINVAL;
-
-	t = ipt_get_target_c(e);
-	if (e->target_offset + t->u.target_size > e->next_offset)
-		return -EINVAL;
-
-	return 0;
-}
-
-static int
 check_match(struct xt_entry_match *m, struct xt_mtchk_param *par)
 {
 	const struct ipt_ip *ip = par->entryinfo;
@@ -662,10 +662,6 @@
 	struct xt_mtchk_param mtpar;
 	struct xt_entry_match *ematch;
 
-	ret = check_entry(e, name);
-	if (ret)
-		return ret;
-
 	j = 0;
 	mtpar.net	= net;
 	mtpar.table     = name;
@@ -729,9 +725,11 @@
 			   unsigned int valid_hooks)
 {
 	unsigned int h;
-
+	int err;
+	
 	if ((unsigned long)e % __alignof__(struct ipt_entry) != 0 ||
-	    (unsigned char *)e + sizeof(struct ipt_entry) >= limit) {
+	    (unsigned char *)e + sizeof(struct ipt_entry) >= limit ||
+	    (unsigned char *)e + e->next_offset > limit) {
 		duprintf("Bad offset %p\n", e);
 		return -EINVAL;
 	}
@@ -743,6 +741,14 @@
 		return -EINVAL;
 	}
 
+	if (!ip_checkentry(&e->ip))
+		return -EINVAL;
+
+	err = xt_check_entry_offsets(e, e->elems, e->target_offset,
+				     e->next_offset);
+	if (err)
+		return err;	
+	
 	/* Check hooks & underflows */
 	for (h = 0; h < NF_INET_NUMHOOKS; h++) {
 		if (!(valid_hooks & (1 << h)))
@@ -1304,56 +1310,18 @@
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
 	struct ipt_entry *iter;
 	unsigned int addend;
-#ifdef CONFIG_COMPAT
-	struct compat_xt_counters_info compat_tmp;
-
-	if (compat) {
-		ptmp = &compat_tmp;
-		size = sizeof(struct compat_xt_counters_info);
-	} else
-#endif
-	{
-		ptmp = &tmp;
-		size = sizeof(struct xt_counters_info);
-	}
-
-	if (copy_from_user(ptmp, user, size) != 0)
-		return -EFAULT;
 
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
+	paddc = xt_copy_counters_from_user(user, len, &tmp, compat);
+	if (IS_ERR(paddc))
+		return PTR_ERR(paddc);
 
-	if (copy_from_user(paddc, user + size, len - size) != 0) {
-		ret = -EFAULT;
-		goto free;
-	}
-
-	t = xt_find_table_lock(net, AF_INET, name);
+	t = xt_find_table_lock(net, AF_INET, tmp.name);
 	if (IS_ERR_OR_NULL(t)) {
 		ret = t ? PTR_ERR(t) : -ENOENT;
 		goto free;
@@ -1361,7 +1329,7 @@
 
 	local_bh_disable();
 	private = t->private;
-	if (private->number != num_counters) {
+	if (private->number != tmp.num_counters) {
 		ret = -EINVAL;
 		goto unlock_up_free;
 	}
@@ -1440,7 +1408,6 @@
 
 static int
 compat_find_calc_match(struct xt_entry_match *m,
-		       const char *name,
 		       const struct ipt_ip *ip,
 		       unsigned int hookmask,
 		       int *size)
@@ -1476,21 +1443,19 @@
 				  struct xt_table_info *newinfo,
 				  unsigned int *size,
 				  const unsigned char *base,
-				  const unsigned char *limit,
-				  const unsigned int *hook_entries,
-				  const unsigned int *underflows,
-				  const char *name)
+				  const unsigned char *limit)
 {
 	struct xt_entry_match *ematch;
 	struct xt_entry_target *t;
 	struct xt_target *target;
 	unsigned int entry_offset;
 	unsigned int j;
-	int ret, off, h;
+	int ret, off;
 
 	duprintf("check_compat_entry_size_and_hooks %p\n", e);
 	if ((unsigned long)e % __alignof__(struct compat_ipt_entry) != 0 ||
-	    (unsigned char *)e + sizeof(struct compat_ipt_entry) >= limit) {
+	    (unsigned char *)e + sizeof(struct compat_ipt_entry) >= limit ||
+	    (unsigned char *)e + e->next_offset > limit) {
 		duprintf("Bad offset %p, limit = %p\n", e, limit);
 		return -EINVAL;
 	}
@@ -1502,8 +1467,11 @@
 		return -EINVAL;
 	}
 
-	/* For purposes of check_entry casting the compat entry is fine */
-	ret = check_entry((struct ipt_entry *)e, name);
+	if (!ip_checkentry(&e->ip))
+		return -EINVAL;
+
+	ret = xt_compat_check_entry_offsets(e, e->elems,
+					    e->target_offset, e->next_offset);
 	if (ret)
 		return ret;
 
@@ -1511,8 +1479,9 @@
 	entry_offset = (void *)e - (void *)base;
 	j = 0;
 	xt_ematch_foreach(ematch, e) {
-		ret = compat_find_calc_match(ematch, name,
+		ret = compat_find_calc_match(ematch, 
 					     &e->ip, e->comefrom, &off);
+						 
 		if (ret != 0)
 			goto release_matches;
 		++j;
@@ -1535,17 +1504,6 @@
 	if (ret)
 		goto out;
 
-	/* Check hooks & underflows */
-	for (h = 0; h < NF_INET_NUMHOOKS; h++) {
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
 
 out:
@@ -1559,19 +1517,18 @@
 	return ret;
 }
 
-static int
+static void
 compat_copy_entry_from_user(struct compat_ipt_entry *e, void **dstptr,
-			    unsigned int *size, const char *name,
+			    unsigned int *size, 
 			    struct xt_table_info *newinfo, unsigned char *base)
 {
 	struct xt_entry_target *t;
 	struct xt_target *target;
 	struct ipt_entry *de;
 	unsigned int origsize;
-	int ret, h;
+	int h;
 	struct xt_entry_match *ematch;
 
-	ret = 0;
 	origsize = *size;
 	de = (struct ipt_entry *)*dstptr;
 	memcpy(de, e, sizeof(struct ipt_entry));
@@ -1580,11 +1537,9 @@
 	*dstptr += sizeof(struct ipt_entry);
 	*size += sizeof(struct ipt_entry) - sizeof(struct compat_ipt_entry);
 
-	xt_ematch_foreach(ematch, e) {
-		ret = xt_compat_match_from_user(ematch, dstptr, size);
-		if (ret != 0)
-			return ret;
-	}
+	xt_ematch_foreach(ematch, e)
+		xt_compat_match_from_user(ematch, dstptr, size);
+
 	de->target_offset = e->target_offset - (origsize - *size);
 	t = compat_ipt_get_target(e);
 	target = t->u.kernel.target;
@@ -1597,181 +1552,88 @@
 		if ((unsigned char *)de - base < newinfo->underflow[h])
 			newinfo->underflow[h] -= origsize - *size;
 	}
-	return ret;
-}
-
-static int
-compat_check_entry(struct ipt_entry *e, struct net *net, const char *name)
-{
-	struct xt_entry_match *ematch;
-	struct xt_mtchk_param mtpar;
-	unsigned int j;
-	int ret = 0;
-
-	j = 0;
-	mtpar.net	= net;
-	mtpar.table     = name;
-	mtpar.entryinfo = &e->ip;
-	mtpar.hook_mask = e->comefrom;
-	mtpar.family    = NFPROTO_IPV4;
-	xt_ematch_foreach(ematch, e) {
-		ret = check_match(ematch, &mtpar);
-		if (ret != 0)
-			goto cleanup_matches;
-		++j;
-	}
-
-	ret = check_target(e, net, name);
-	if (ret)
-		goto cleanup_matches;
-	return 0;
-
- cleanup_matches:
-	xt_ematch_foreach(ematch, e) {
-		if (j-- == 0)
-			break;
-		cleanup_match(ematch, net);
-	}
-	return ret;
 }
 
 static int
 translate_compat_table(struct net *net,
-		       const char *name,
-		       unsigned int valid_hooks,
 		       struct xt_table_info **pinfo,
 		       void **pentry0,
-		       unsigned int total_size,
-		       unsigned int number,
-		       unsigned int *hook_entries,
-		       unsigned int *underflows)
+		       const struct compat_ipt_replace *compatr)
 {
 	unsigned int i, j;
 	struct xt_table_info *newinfo, *info;
 	void *pos, *entry0, *entry1;
 	struct compat_ipt_entry *iter0;
-	struct ipt_entry *iter1;
+	struct ipt_replace repl;
 	unsigned int size;
 	int ret;
 
 	info = *pinfo;
 	entry0 = *pentry0;
-	size = total_size;
-	info->number = number;
-
-	/* Init all hooks to impossible value. */
-	for (i = 0; i < NF_INET_NUMHOOKS; i++) {
-		info->hook_entry[i] = 0xFFFFFFFF;
-		info->underflow[i] = 0xFFFFFFFF;
-	}
+	size = compatr->size;
+	info->number = compatr->num_entries;
 
 	duprintf("translate_compat_table: size %u\n", info->size);
 	j = 0;
 	xt_compat_lock(AF_INET);
-	xt_compat_init_offsets(AF_INET, number);
+	xt_compat_init_offsets(AF_INET, compatr->num_entries);
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
-	for (i = 0; i < NF_INET_NUMHOOKS; i++) {
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
 	for (i = 0; i < NF_INET_NUMHOOKS; i++) {
-		newinfo->hook_entry[i] = info->hook_entry[i];
-		newinfo->underflow[i] = info->underflow[i];
+		newinfo->hook_entry[i] = compatr->hook_entry[i];
+		newinfo->underflow[i] = compatr->underflow[i];
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
+	/* all module references in entry0 are now gone.
+	 * entry1/newinfo contains a 64bit ruleset that looks exactly as
+	 * generated by 64bit userspace.
+	 *
+	 * Call standard translate_table() to validate all hook_entrys,
+	 * underflows, check for loops, etc.
+	 */
 	xt_compat_flush_offsets(AF_INET);
 	xt_compat_unlock(AF_INET);
-	if (ret)
-		goto free_newinfo;
 
-	ret = -ELOOP;
-	if (!mark_source_chains(newinfo, valid_hooks, entry1))
-		goto free_newinfo;
+	memcpy(&repl, compatr, sizeof(*compatr));
 
-	i = 0;
-	xt_entry_foreach(iter1, entry1, newinfo->size) {
-		ret = compat_check_entry(iter1, net, name);
-		if (ret != 0)
-			break;
-		++i;
-		if (strcmp(ipt_get_target(iter1)->u.user.name,
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
-			cleanup_entry(iter1, net);
-		}
-		xt_free_table_info(newinfo);
-		return ret;
+	for (i = 0; i < NF_INET_NUMHOOKS; i++) {
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
+	ret = translate_table(net, newinfo, entry1, &repl);
+	if (ret)
+		goto free_newinfo;
 
 	*pinfo = newinfo;
 	*pentry0 = entry1;
@@ -1780,17 +1642,16 @@
 
 free_newinfo:
 	xt_free_table_info(newinfo);
-out:
-	xt_entry_foreach(iter0, entry0, total_size) {
+	return ret;
+out_unlock:
+	xt_compat_flush_offsets(AF_INET);
+	xt_compat_unlock(AF_INET);
+	xt_entry_foreach(iter0, entry0, compatr->size) {
 		if (j-- == 0)
 			break;
 		compat_release_entry(iter0);
 	}
 	return ret;
-out_unlock:
-	xt_compat_flush_offsets(AF_INET);
-	xt_compat_unlock(AF_INET);
-	goto out;
 }
 
 static int
@@ -1824,10 +1685,7 @@
 		goto free_newinfo;
 	}
 
-	ret = translate_compat_table(net, tmp.name, tmp.valid_hooks,
-				     &newinfo, &loc_cpu_entry, tmp.size,
-				     tmp.num_entries, tmp.hook_entry,
-				     tmp.underflow);
+	ret = translate_compat_table(net, &newinfo, &loc_cpu_entry, &tmp);
 	if (ret != 0)
 		goto free_newinfo;
 
