--- linux/drivers/usb/core/buffer.c	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/drivers/usb/core/buffer.c	2016-12-19 15:41:26.891414544 +0800
@@ -22,17 +22,25 @@
  */
 
 /* FIXME tune these based on pool statistics ... */
-static const size_t	pool_max[HCD_BUFFER_POOLS] = {
-	/* platforms without dma-friendly caches might need to
-	 * prevent cacheline sharing...
-	 */
-	32,
-	128,
-	512,
-	PAGE_SIZE / 2
-	/* bigger --> allocate pages */
+static size_t pool_max[HCD_BUFFER_POOLS] = {
+	32, 128, 512, 2048,
 };
 
+void __init usb_init_pool_max(void)
+{
+	/*
+	 * The pool_max values must never be smaller than
+	 * ARCH_KMALLOC_MINALIGN.
+	 */
+	if (ARCH_KMALLOC_MINALIGN <= 32)
+		;			/* Original value is okay */
+	else if (ARCH_KMALLOC_MINALIGN <= 64)
+		pool_max[0] = 64;
+	else if (ARCH_KMALLOC_MINALIGN <= 128)
+		pool_max[0] = 0;	/* Don't use this pool */
+	else
+		BUILD_BUG();		/* We don't allow this */
+}
 
 /* SETUP primitives */
 
