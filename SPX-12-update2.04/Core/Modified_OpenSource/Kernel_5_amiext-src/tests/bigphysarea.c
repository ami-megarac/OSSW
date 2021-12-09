/*
 * gcc -DMODULE -D__KERNEL__ -O2 -Wall -c -o module.o module.c
 */

#include <linux/init.h>
#include <linux/module.h>

#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/malloc.h>
#include <linux/bigphysarea.h>

/* name is different in 2.0.x and 2.1.x */
#if LINUX_VERSION_CODE < 0x020100
#define ioremap vremap
#define iounmap vfree
#endif


/* nice and high */
static int major = 42;
static int first = 1;

/*
 * Our special open code.
 * MOD_INC_USE_COUNT make sure that the driver memory is not freed
 * while the device is in use.
 */
static int mod_open( struct inode* ino, struct file* filep)
{
	if (first) {
		MOD_INC_USE_COUNT;
		first = 0;
	}
	return -1;
}

/*
 * Now decrement the use count.
 */
static int mod_release( struct inode* ino, struct file* filep)
{
	MOD_DEC_USE_COUNT;
	first = 1;
	return -1;
}

static struct file_operations fops = {
	open: mod_open,
	release: mod_release
};

#define COUNT 10

void *addr[COUNT];

void test_bigphysarea_alloc()
{
	int i;

	for (i = 0; i < COUNT; i++) {
		addr[i] = bigphysarea_alloc_pages(3, 2, GFP_KERNEL);
		printk("addr[%d] = 0x%08x\n", i, (unsigned)addr[i]);
	}
		
}

void test_bigphysarea_free()
{
	int i;

	for (i = 0; i < COUNT; i++) {
		bigphysarea_free_pages(addr[i]);
	}
}


/*
 * And now the modules code and kernel interface.
 */
int __init test_bigphysarea_init(void)
{
	if (register_chrdev(major, "test-module", &fops)) {
		printk("register_chrdev failed: goodbye world :-(\n");
		return -EIO;
	}

	printk("test-module loaded\n");

	test_bigphysarea_alloc();

	return 0;
}

void __exit test_bigphysarea_exit(void)
{

	test_bigphysarea_free();

	if (unregister_chrdev(major, "test-module") != 0)
		printk("cleanup_module failed\n");
	else
		printk("test-module removed\n");
}

module_init(test_bigphysarea_init);
module_exit(test_bigphysarea_exit);
