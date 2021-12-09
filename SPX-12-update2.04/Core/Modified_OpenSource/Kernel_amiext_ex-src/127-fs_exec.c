--- linux_org/fs/exec.c	2019-01-29 18:41:46.404677488 +0800
+++ linux/fs/exec.c	2019-01-30 13:12:46.434561190 +0800
@@ -196,8 +196,26 @@
 
 	if (write) {
 		unsigned long size = bprm->vma->vm_end - bprm->vma->vm_start;
+		unsigned long ptr_size;
 		struct rlimit *rlim;
 
+		/*
+		 * Since the stack will hold pointers to the strings, we
+		 * must account for them as well.
+		 *
+		 * The size calculation is the entire vma while each arg page is
+		 * built, so each time we get here it's calculating how far it
+		 * is currently (rather than each call being just the newly
+		 * added size from the arg page).  As a result, we need to
+		 * always add the entire size of the pointers, so that on the
+		 * last call to get_arg_page() we'll actually have the entire
+		 * correct size.
+		 */
+		ptr_size = (bprm->argc + bprm->envc) * sizeof(void *);
+		if (ptr_size > ULONG_MAX - size)
+			goto fail;
+		size += ptr_size;		
+		
 		acct_arg_size(bprm, size / PAGE_SIZE);
 
 		/*
@@ -215,13 +233,15 @@
 		 *    to work from.
 		 */
 		rlim = current->signal->rlim;
-		if (size > ACCESS_ONCE(rlim[RLIMIT_STACK].rlim_cur) / 4) {
-			put_page(page);
-			return NULL;
-		}
+		if (size > ACCESS_ONCE(rlim[RLIMIT_STACK].rlim_cur) / 4)
+			goto fail;
 	}
 
 	return page;
+
+fail:
+	put_page(page);
+	return NULL;	
 }
 
 static void put_arg_page(struct page *page)
@@ -299,13 +319,45 @@
 		int write)
 {
 	struct page *page;
+	int ret;
+
+#ifdef CONFIG_STACK_GROWSUP
+	if (write) {
+		ret = expand_downwards(bprm->vma, pos);
+		if (ret < 0)
+			return NULL;
+	}
+#endif
+	ret = get_user_pages(current, bprm->mm, pos,
+			1, write, 1, &page, NULL);
+	if (ret <= 0)
+		return NULL;
+
+	if (write) {
+		unsigned long size = bprm->vma->vm_end - bprm->vma->vm_start;
+		struct rlimit *rlim;
+
+		acct_arg_size(bprm, size / PAGE_SIZE);
 
-	page = bprm->page[pos / PAGE_SIZE];
-	if (!page && write) {
-		page = alloc_page(GFP_HIGHUSER|__GFP_ZERO);
-		if (!page)
+		/*
+		 * We've historically supported up to 32 pages (ARG_MAX)
+		 * of argument strings even with small stacks
+		 */
+		if (size <= ARG_MAX)
+			return page;
+
+		/*
+		 * Limit to 1/4-th the stack size for the argv+env strings.
+		 * This ensures that:
+		 *  - the remaining binfmt code will not run out of stack space,
+		 *  - the program will have a reasonable amount of stack left
+		 *    to work from.
+		 */
+		rlim = current->signal->rlim;
+		if (size > ACCESS_ONCE(rlim[RLIMIT_STACK].rlim_cur) / 4) {
+			put_page(page);
 			return NULL;
-		bprm->page[pos / PAGE_SIZE] = page;
+		}
 	}
 
 	return page;
