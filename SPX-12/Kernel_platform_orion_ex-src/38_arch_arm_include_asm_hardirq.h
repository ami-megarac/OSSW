---  linux/arch/arm/include/asm/hardirq.h	2017-03-29 19:51:55.390789000 +0800
+++  linux/arch/arm/include/asm/hardirq.h	2017-03-29 19:49:22.822542999 +0800
@@ -5,7 +5,11 @@
 #include <linux/threads.h>
 #include <asm/irq.h>
 
-#define NR_IPI	8
+#ifdef CONFIG_SOC_SE_PILOT4
+	#define  NR_IPI 9	/*SDK Update - BMC CPU Reset Workaround*/
+#else
+	#define NR_IPI	8
+#endif
 
 typedef struct {
 	unsigned int __softirq_pending;
