--- linux_org/net/netfilter/x_tables.c	2018-12-17 10:45:17.756790096 +0800
+++ linux/net/netfilter/x_tables.c	2018-12-19 11:52:32.152515706 +0800
@@ -435,6 +435,47 @@
 }
 EXPORT_SYMBOL_GPL(xt_check_match);
 
+/** xt_check_entry_match - check that matches end before start of target
+ *
+ * @match: beginning of xt_entry_match
+ * @target: beginning of this rules target (alleged end of matches)
+ * @alignment: alignment requirement of match structures
+ *
+ * Validates that all matches add up to the beginning of the target,
+ * and that each match covers at least the base structure size.
+ *
+ * Return: 0 on success, negative errno on failure.
+ */
+static int xt_check_entry_match(const char *match, const char *target,
+				const size_t alignment)
+{
+	const struct xt_entry_match *pos;
+	int length = target - match;
+
+	if (length == 0) /* no matches */
+		return 0;
+
+	pos = (struct xt_entry_match *)match;
+	do {
+		if ((unsigned long)pos % alignment)
+			return -EINVAL;
+
+		if (length < (int)sizeof(struct xt_entry_match))
+			return -EINVAL;
+
+		if (pos->u.match_size < sizeof(struct xt_entry_match))
+			return -EINVAL;
+
+		if (pos->u.match_size > length)
+			return -EINVAL;
+
+		length -= pos->u.match_size;
+		pos = ((void *)((char *)(pos) + (pos)->u.match_size));
+	} while (length > 0);
+
+	return 0;
+}
+
 #ifdef CONFIG_COMPAT
 int xt_compat_add_offset(u_int8_t af, unsigned int offset, int delta)
 {
@@ -504,13 +545,14 @@
 }
 EXPORT_SYMBOL_GPL(xt_compat_match_offset);
 
-int xt_compat_match_from_user(struct xt_entry_match *m, void **dstptr,
-			      unsigned int *size)
+void xt_compat_match_from_user(struct xt_entry_match *m, void **dstptr,
+			       unsigned int *size)
 {
 	const struct xt_match *match = m->u.kernel.match;
 	struct compat_xt_entry_match *cm = (struct compat_xt_entry_match *)m;
 	int pad, off = xt_compat_match_offset(match);
 	u_int16_t msize = cm->u.user.match_size;
+	char name[sizeof(m->u.user.name)];
 
 	m = *dstptr;
 	memcpy(m, cm, sizeof(*cm));
@@ -524,10 +566,12 @@
 
 	msize += off;
 	m->u.user.match_size = msize;
+	strlcpy(name, match->name, sizeof(name));
+	module_put(match->me);
+	strncpy(m->u.user.name, name, sizeof(m->u.user.name));
 
 	*size += off;
 	*dstptr += msize;
-	return 0;
 }
 EXPORT_SYMBOL_GPL(xt_compat_match_from_user);
 
@@ -558,8 +602,125 @@
 	return 0;
 }
 EXPORT_SYMBOL_GPL(xt_compat_match_to_user);
+
+/* non-compat version may have padding after verdict */
+struct compat_xt_standard_target {
+	struct compat_xt_entry_target t;
+	compat_uint_t verdict;
+};
+
+int xt_compat_check_entry_offsets(const void *base, const char *elems,
+				  unsigned int target_offset,
+				  unsigned int next_offset)
+{
+	long size_of_base_struct = elems - (const char *)base;
+	const struct compat_xt_entry_target *t;
+	const char *e = base;
+
+	if (target_offset < size_of_base_struct)
+		return -EINVAL;	
+	
+	if (target_offset + sizeof(*t) > next_offset)
+		return -EINVAL;
+
+	t = (void *)(e + target_offset);
+	if (t->u.target_size < sizeof(*t))
+		return -EINVAL;
+
+	if (target_offset + t->u.target_size > next_offset)
+		return -EINVAL;
+
+	if (strcmp(t->u.user.name, XT_STANDARD_TARGET) == 0 &&
+	    COMPAT_XT_ALIGN(target_offset + sizeof(struct compat_xt_standard_target)) != next_offset)
+		return -EINVAL;	
+	
+	/* compat_xt_entry match has less strict aligment requirements,
+	 * otherwise they are identical.  In case of padding differences
+	 * we need to add compat version of xt_check_entry_match.
+	 */
+	BUILD_BUG_ON(sizeof(struct compat_xt_entry_match) != sizeof(struct xt_entry_match));
+
+	return xt_check_entry_match(elems, base + target_offset,
+				    __alignof__(struct compat_xt_entry_match));
+}
+EXPORT_SYMBOL(xt_compat_check_entry_offsets);
 #endif /* CONFIG_COMPAT */
 
+/**
+ * xt_check_entry_offsets - validate arp/ip/ip6t_entry
+ *
+ * @base: pointer to arp/ip/ip6t_entry
+ * @elems: pointer to first xt_entry_match, i.e. ip(6)t_entry->elems 
+ * @target_offset: the arp/ip/ip6_t->target_offset
+ * @next_offset: the arp/ip/ip6_t->next_offset
+ *
+ * validates that target_offset and next_offset are sane and that all
+ * match sizes (if any) align with the target offset.
+ *
+ * This function does not validate the targets or matches themselves, it
+ * only tests that all the offsets and sizes are correct, that all
+ * match structures are aligned, and that the last structure ends where
+ * the target structure begins.
+ *
+ * Also see xt_compat_check_entry_offsets for CONFIG_COMPAT version.
+ *
+ * The arp/ip/ip6t_entry structure @base must have passed following tests:
+ * - it must point to a valid memory location
+ * - base to base + next_offset must be accessible, i.e. not exceed allocated
+ *   length.
+ *
+ * A well-formed entry looks like this:
+ *
+ * ip(6)t_entry   match [mtdata]  match [mtdata] target [tgdata] ip(6)t_entry
+ * e->elems[]-----'                              |               |
+ *                matchsize                      |               |
+ *                                matchsize      |               |
+ *                                               |               |
+ * target_offset---------------------------------'               |
+ * next_offset---------------------------------------------------'
+ *
+ * elems[]: flexible array member at end of ip(6)/arpt_entry struct.
+ *          This is where matches (if any) and the target reside.
+ * target_offset: beginning of target.
+ * next_offset: start of the next rule; also: size of this rule.
+ * Since targets have a minimum size, target_offset + minlen <= next_offset.
+ *
+ * Every match stores its size, sum of sizes must not exceed target_offset.
+ * 
+ * Return: 0 on success, negative errno on failure.
+ */
+int xt_check_entry_offsets(const void *base,
+			   const char *elems,
+			   unsigned int target_offset,
+			   unsigned int next_offset)
+{
+	long size_of_base_struct = elems - (const char *)base;
+	const struct xt_entry_target *t;
+	const char *e = base;
+
+	/* target start is within the ip/ip6/arpt_entry struct */
+	if (target_offset < size_of_base_struct)
+		return -EINVAL;
+
+	if (target_offset + sizeof(*t) > next_offset)
+		return -EINVAL;
+
+	t = (void *)(e + target_offset);
+	if (t->u.target_size < sizeof(*t))
+		return -EINVAL;
+	
+	if (target_offset + t->u.target_size > next_offset)
+		return -EINVAL;
+
+	if (strcmp(t->u.user.name, XT_STANDARD_TARGET) == 0 &&
+	    XT_ALIGN(target_offset + sizeof(struct xt_standard_target)) != next_offset)
+		return -EINVAL;	
+	
+	return xt_check_entry_match(elems, base + target_offset,
+				    __alignof__(struct xt_entry_match));
+}
+EXPORT_SYMBOL(xt_check_entry_offsets);
+
 int xt_check_target(struct xt_tgchk_param *par,
 		    unsigned int size, u_int8_t proto, bool inv_proto)
 {
@@ -610,6 +771,80 @@
 }
 EXPORT_SYMBOL_GPL(xt_check_target);
 
+/**
+ * xt_copy_counters_from_user - copy counters and metadata from userspace
+ *
+ * @user: src pointer to userspace memory
+ * @len: alleged size of userspace memory
+ * @info: where to store the xt_counters_info metadata
+ * @compat: true if we setsockopt call is done by 32bit task on 64bit kernel
+ *
+ * Copies counter meta data from @user and stores it in @info.
+ *
+ * vmallocs memory to hold the counters, then copies the counter data
+ * from @user to the new memory and returns a pointer to it.
+ *
+ * If @compat is true, @info gets converted automatically to the 64bit
+ * representation.
+ *
+ * The metadata associated with the counters is stored in @info.
+ *
+ * Return: returns pointer that caller has to test via IS_ERR().
+ * If IS_ERR is false, caller has to vfree the pointer.
+ */
+void *xt_copy_counters_from_user(const void __user *user, unsigned int len,
+				 struct xt_counters_info *info, bool compat)
+{
+	void *mem;
+	u64 size;
+
+#ifdef CONFIG_COMPAT
+	if (compat) {
+		/* structures only differ in size due to alignment */
+		struct compat_xt_counters_info compat_tmp;
+
+		if (len <= sizeof(compat_tmp))
+			return ERR_PTR(-EINVAL);
+
+		len -= sizeof(compat_tmp);
+		if (copy_from_user(&compat_tmp, user, sizeof(compat_tmp)) != 0)
+			return ERR_PTR(-EFAULT);
+
+		strlcpy(info->name, compat_tmp.name, sizeof(info->name));
+		info->num_counters = compat_tmp.num_counters;
+		user += sizeof(compat_tmp);
+	} else
+#endif
+	{
+		if (len <= sizeof(*info))
+			return ERR_PTR(-EINVAL);
+
+		len -= sizeof(*info);
+		if (copy_from_user(info, user, sizeof(*info)) != 0)
+			return ERR_PTR(-EFAULT);
+
+		info->name[sizeof(info->name) - 1] = '\0';
+		user += sizeof(*info);
+	}
+
+	size = sizeof(struct xt_counters);
+	size *= info->num_counters;
+
+	if (size != (u64)len)
+		return ERR_PTR(-EINVAL);
+
+	mem = vmalloc(len);
+	if (!mem)
+		return ERR_PTR(-ENOMEM);
+
+	if (copy_from_user(mem, user, len) == 0)
+		return mem;
+
+	vfree(mem);
+	return ERR_PTR(-EFAULT);
+}
+EXPORT_SYMBOL_GPL(xt_copy_counters_from_user);
+
 #ifdef CONFIG_COMPAT
 int xt_compat_target_offset(const struct xt_target *target)
 {
@@ -625,6 +860,7 @@
 	struct compat_xt_entry_target *ct = (struct compat_xt_entry_target *)t;
 	int pad, off = xt_compat_target_offset(target);
 	u_int16_t tsize = ct->u.user.target_size;
+	char name[sizeof(t->u.user.name)];
 
 	t = *dstptr;
 	memcpy(t, ct, sizeof(*ct));
@@ -638,6 +874,9 @@
 
 	tsize += off;
 	t->u.user.target_size = tsize;
+	strlcpy(name, target->name, sizeof(name));
+	module_put(target->me);
+	strncpy(t->u.user.name, name, sizeof(t->u.user.name));
 
 	*size += off;
 	*dstptr += tsize;
