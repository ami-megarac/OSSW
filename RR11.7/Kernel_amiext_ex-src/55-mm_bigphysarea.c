--- linux.old/mm/bigphysarea.c	1969-12-31 19:00:00.000000000 -0500
+++ linux-3.14.17-ami/mm/bigphysarea.c	2014-08-22 11:48:22.000000000 -0400
@@ -0,0 +1,377 @@
+/* linux/mm/bigphysarea.c, M. Welsh (mdw@cs.cornell.edu)
+ * Copyright (c) 1996 by Matt Welsh.
+ * Extended by Roger Butenuth (butenuth@uni-paderborn.de), October 1997
+ * Extended for linux-2.1.121 till 2.4.0 (June 2000)
+ *     by Pauline Middelink <middelink@polyware.nl>
+ *
+ * This is a set of routines which allow you to reserve a large (?) 
+ * amount of physical memory at boot-time, which can be allocated/deallocated
+ * by drivers. This memory is intended to be used for devices such as 
+ * video framegrabbers which need a lot of physical RAM (above the amount
+ * allocated by kmalloc). This is by no means efficient or recommended;
+ * to be used only in extreme circumstances.
+ *
+ *   This program is free software; you can redistribute it and/or modify
+ *   it under the terms of the GNU General Public License as published by
+ *   the Free Software Foundation; either version 2 of the License, or
+ *   (at your option) any later version.
+ * 
+ *   This program is distributed in the hope that it will be useful,
+ *   but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *   GNU General Public License for more details.
+ *
+ *   You should have received a copy of the GNU General Public License
+ *   along with this program; if not, write to the Free Software
+ *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
+ *
+ */
+
+//#include <linux/autoconf.h>
+#include <linux/ptrace.h>
+#include <linux/types.h>
+#include <linux/kernel.h>
+#include <linux/init.h>
+#include <linux/pci.h>
+#include <linux/proc_fs.h>
+#include <linux/seq_file.h>
+#include <linux/string.h>
+#include <linux/mm.h>
+#include <linux/bootmem.h>
+#include <linux/errno.h>
+#include <linux/slab.h>
+#include <linux/bigphysarea.h>
+
+
+static int init2(int priority);
+
+typedef struct range_struct {
+	struct range_struct *next;
+	caddr_t base;			/* base of allocated block */
+	size_t  size;			/* size in bytes */
+} range_t;
+
+/*
+ * 0: nothing initialized
+ * 1: bigphysarea_pages initialized
+ * 2: free list initialized
+ */
+static int	init_level = 0;
+static int	bigphysarea_pages = 0;
+static caddr_t	bigphysarea = 0;
+static range_t	*free_list = NULL;
+static range_t	*used_list = NULL;
+static struct resource mem_resource = { .name="Bigphysarea", .start=0, .end=0, .flags=IORESOURCE_MEM|IORESOURCE_BUSY };
+
+static
+ssize_t bigphys_proc_read(struct file *file, char *buf, size_t size, loff_t *ppos)
+
+{
+	char *p=buf;
+	range_t *ptr;
+	int     free_count, free_total, free_max;
+	int     used_count, used_total, used_max;
+	int copied;
+
+	if (init_level == 1)
+	  init2(GFP_KERNEL);
+
+	free_count = 0;
+	free_total = 0;
+	free_max   = 0;
+	for (ptr = free_list; ptr != NULL; ptr = ptr->next) {
+		free_count++;
+		free_total += ptr->size;
+		if (ptr->size > free_max)
+			free_max = ptr->size;
+	}
+
+	used_count = 0;
+	used_total = 0;
+	used_max   = 0;
+	for (ptr = used_list; ptr != NULL; ptr = ptr->next) {
+		used_count++;
+		used_total += ptr->size;
+		if (ptr->size > used_max)
+			used_max = ptr->size;
+	}
+	
+	if (*ppos != 0)			/* EOF . Also no seek read*/
+		return 0;
+
+	if (bigphysarea_pages == 0) {
+		 p+=sprintf(p, "No big physical area allocated!\n");
+		copied = p-buf;
+		*ppos+=copied;
+		return copied;
+
+	}
+	  
+	p += sprintf(p, "Big physical area, size %ld kB\n",
+		     bigphysarea_pages * PAGE_SIZE / 1024);
+	p += sprintf(p, "                       free list:             used list:\n");
+	p += sprintf(p, "number of blocks:      %8d               %8d\n",
+		     free_count, used_count);
+	p += sprintf(p, "size of largest block: %8d kB            %8d kB\n",
+		     free_max / 1024, used_max / 1024);
+	p += sprintf(p, "total:                 %8d kB            %8d kB\n",
+		     free_total / 1024, used_total /1024);
+
+
+	copied = p-buf;
+	*ppos+=copied;
+	return copied;
+}
+
+
+static const struct file_operations bigphys_proc_fops = {
+	.read		= bigphys_proc_read,
+};
+
+static
+int __init bigphysarea_init(void)
+{
+	struct proc_dir_entry *res = NULL;
+
+	if (bigphysarea_pages == 0 || bigphysarea == 0)
+		return -EINVAL;
+
+	/* create to /proc entry for it */
+	res = proc_create("bigphysarea", 044, NULL,
+			  &bigphys_proc_fops);
+	init_level = 1;
+
+	printk(KERN_INFO "bigphysarea: Allocated %d pages at 0x%p.\n",
+	       bigphysarea_pages, bigphysarea);
+
+	return 0;
+}
+
+__initcall(bigphysarea_init);
+
+/*
+ * call when 'bigphysarea=' is given on the commandline.
+ *
+ * Strangely, bootmem is still active during this call, but
+ * during the processing of the initcalls it isn't anymore!
+ * So we alloc the needed memory here instead of bigphysarea_init().
+ */
+static
+int __init bigphysarea_setup(char *str)
+{
+	int par;
+	if (get_option(&str,&par)) {
+		bigphysarea_pages = par;
+		// Alloc the memory
+		bigphysarea = alloc_bootmem_low_pages(bigphysarea_pages<<PAGE_SHIFT);
+		if (!bigphysarea) {
+			printk(KERN_CRIT "bigphysarea: not enough memory for %d pages\n",bigphysarea_pages);
+			return -ENOMEM;
+		}
+
+		// register the resource for it
+		mem_resource.start = (unsigned long)bigphysarea;
+		mem_resource.end = mem_resource.start + (bigphysarea_pages<<PAGE_SHIFT);
+		request_resource(&iomem_resource, &mem_resource);
+	}
+	return 1;
+}
+
+__setup("bigphysarea=", bigphysarea_setup);
+
+/*
+ * When we have pages but don't have a freelist, put all pages in
+ * one free list entry. Return 0 on success, 1 on error.
+ */
+static
+int init2(int priority)
+{
+	if (init_level == 1) {
+		free_list = kmalloc(sizeof(range_t), priority);
+		if (free_list != NULL) {
+			free_list->next = NULL;
+			free_list->base = bigphysarea;
+			free_list->size = bigphysarea_pages * PAGE_SIZE;
+			init_level = 2;
+			return 0;
+		}
+	}
+	return 1;
+}
+
+
+/*
+ * Allocate `count' pages from the big physical area. Pages are aligned to
+ * a multiple of `align'. `priority' has the same meaning in kmalloc, it
+ * is needed for management information.
+ * This function may not be called from an interrupt!
+ */
+caddr_t bigphysarea_alloc_pages(int count, int align, int priority)
+{
+	range_t *range, **range_ptr, *new_range, *align_range;
+	caddr_t aligned_base=0;
+
+	if (init_level < 2)
+		if (init2(priority))
+			return 0;
+	new_range   = NULL;
+	align_range = NULL;
+
+	if (align == 0)
+		align = PAGE_SIZE;
+	else
+		align = align * PAGE_SIZE;
+	/*
+	 * Search a free block which is large enough, even with alignment.
+	 */
+	if( free_list == NULL )
+		printk("free_list is NULL\n");
+	range_ptr = &free_list;
+	while (*range_ptr != NULL) {
+		range = *range_ptr;
+		aligned_base =
+		  (caddr_t)((((unsigned long)range->base + align - 1) / align) * align);
+		if (aligned_base + count * PAGE_SIZE <= 
+		    range->base + range->size)
+			break;
+	     range_ptr = &range->next;
+	}
+	if (*range_ptr == NULL)
+		return 0;
+	range = *range_ptr;
+	/*
+	 * When we have to align, the pages needed for alignment can
+	 * be put back to the free pool.
+	 * We check here if we need a second range data structure later
+	 * and allocate it now, so that we don't have to check for a
+	 * failed kmalloc later.
+	 */
+	if (aligned_base - range->base + count * PAGE_SIZE < range->size) {
+		new_range = kmalloc(sizeof(range_t), priority);
+		if (new_range == NULL)
+			return NULL;
+	}
+	if (aligned_base != range->base) {
+		align_range = kmalloc(sizeof(range_t), priority);
+		if (align_range == NULL) {
+			if (new_range != NULL)
+				kfree(new_range);
+			return NULL;
+		}
+		align_range->base = range->base;
+		align_range->size = aligned_base - range->base;
+		range->base = aligned_base;
+		range->size -= align_range->size;
+		align_range->next = range;
+		*range_ptr = align_range;
+		range_ptr = &align_range->next;
+	}
+	if (new_range != NULL) {
+		/*
+		 * Range is larger than needed, create a new list element for
+		 * the used list and shrink the element in the free list.
+		 */
+		new_range->base        = range->base;
+		new_range->size        = count * PAGE_SIZE;
+		range->base = new_range->base + new_range->size;
+		range->size = range->size - new_range->size;
+	} else {
+		/*
+		 * Range fits perfectly, remove it from free list.
+		 */
+		*range_ptr = range->next;
+		new_range = range;
+	}
+	/*
+	 * Insert block into used list
+	 */
+	new_range->next = used_list;
+	used_list = new_range;
+
+	return new_range->base;
+}
+
+/*
+ * Free pages allocated with `bigphysarea_alloc_pages'. `base' must be an
+ * address returned by `bigphysarea_alloc_pages'.
+ * This function my not be called from an interrupt!
+ */
+void bigphysarea_free_pages(caddr_t base)
+{
+	range_t *prev, *next, *range, **range_ptr;
+  
+	/*
+	 * Search the block in the used list.
+	 */
+	for (range_ptr = &used_list;
+	     *range_ptr != NULL;
+	     range_ptr = &(*range_ptr)->next)
+		if ((*range_ptr)->base == base)
+			break;
+	if (*range_ptr == NULL) {
+		printk("bigphysarea_free_pages(0x%08lx), not allocated!\n",
+		       (unsigned long)base);
+		return;
+	}
+	range = *range_ptr;
+	/*
+	 * Remove range from the used list:
+	 */
+	*range_ptr = (*range_ptr)->next;
+	/*
+	 * The free-list is sorted by address, search insertion point
+	 * and insert block in free list.
+	 */
+	for (range_ptr = &free_list, prev = NULL;
+	     *range_ptr != NULL;
+	     prev = *range_ptr, range_ptr = &(*range_ptr)->next)
+		if ((*range_ptr)->base >= base)
+			break;
+	range->next  = *range_ptr;
+	*range_ptr   = range;
+	/*
+	 * Concatenate free range with neighbors, if possible.
+	 * Try for upper neighbor (next in list) first, then
+	 * for lower neighbor (predecessor in list).
+	 */
+	if (range->next != NULL &&
+	    range->base + range->size == range->next->base) {
+		next = range->next;
+		range->size += range->next->size;
+		range->next = next->next;
+		kfree(next);
+	}
+	if (prev != NULL &&
+	    prev->base + prev->size == range->base) {
+		prev->size += prev->next->size;
+		prev->next = range->next;
+		kfree(range);
+	}
+}
+
+caddr_t bigphysarea_alloc(int size)
+{
+	int pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
+
+	return bigphysarea_alloc_pages(pages, 1, GFP_KERNEL);
+}
+
+void bigphysarea_free(caddr_t addr, int size)
+{
+	(void)size;
+	bigphysarea_free_pages(addr);
+}
+
+unsigned long bigphysarea_get_max_size(void)
+{
+	unsigned long free_max=0;
+	range_t *ptr;
+	if (init_level == 1)
+		init2(GFP_KERNEL);
+	for (ptr = free_list; ptr != NULL; ptr = ptr->next) 
+	{
+		if (ptr->size > free_max)
+			free_max = ptr->size;
+	}
+	return free_max;
+}
