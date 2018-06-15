---  linux/arch/arm/include/asm/smp.h	2017-03-29 19:51:55.390789000 +0800
+++ linux/arch/arm/include/asm/smp.h	2017-03-29 19:49:55.326787000 +0800
@@ -42,6 +42,12 @@
  */
 extern void smp_init_cpus(void);
 
+/*SDK Update - BMC CPU Reset Workaround*/
+#ifdef CONFIG_SOC_SE_PILOT4
+	extern void smp_send_wfe(int cpu);
+	extern void execute_smp_wfe(void);
+#endif
+
 
 /*
  * Provide a function to raise an IPI cross call on CPUs in callmap.
