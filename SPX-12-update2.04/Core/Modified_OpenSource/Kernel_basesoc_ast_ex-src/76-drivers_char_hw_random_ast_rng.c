--- linuxi.new/drivers/char/hw_random/ast_rng.c	2018-10-10 01:40:49.986272228 +0800
+++ linuxi/drivers/char/hw_random/ast_rng.c	2018-10-10 01:38:51.562272228 +0800
@@ -0,0 +1,64 @@
+/****************************************************************
+ **                                                            **   
+ **    (C)Copyright 2009-2015, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        5555 Oakbrook Pkwy Suite 200, Norcross              **
+ **                                                            **
+ **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+ ****************************************************************/
+
+/****************************************************************
+ *
+ * ast_rng.c
+ * AST2500 HW random number generator driver
+ *
+*****************************************************************/
+
+#include <linux/module.h>
+#include <linux/random.h>
+#include <linux/platform_device.h>
+#include <linux/hw_random.h>
+#include <linux/io.h>
+
+#define AST_SCU_REG_BASE            AST_SCU_VA_BASE/* 0x1e6e2000 */
+#define AST_SCU_RAMDOM_DATA         0x78        /*  random number generator data output*/ 
+#define ASTRNG_NAME                 "ast_rng"
+#define BYTES_READ                  4
+
+int ast_rng_data_read(struct hwrng *rng, u32 *data)
+{
+	*data = ioread32((void __iomem*)AST_SCU_REG_BASE+AST_SCU_RAMDOM_DATA);   
+	return BYTES_READ;
+}
+
+static struct hwrng ast_rng_ops = {
+	.name		= ASTRNG_NAME,
+	.data_read	= ast_rng_data_read,
+};
+
+
+static int ast_rng_probe(void)
+{
+	return hwrng_register(&ast_rng_ops);
+}
+
+static void ast_rng_remove(void)
+{
+	hwrng_unregister(&ast_rng_ops);
+}
+
+
+static int __init ast_rng_init(void)
+{
+    return ast_rng_probe();
+}
+
+module_init(ast_rng_init); 
+module_exit(ast_rng_remove); 
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc.");
+MODULE_DESCRIPTION("HW random number generator driver");
