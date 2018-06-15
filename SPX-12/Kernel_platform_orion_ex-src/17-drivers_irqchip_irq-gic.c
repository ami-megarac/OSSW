--- linux_old/drivers/irqchip/irq-gic.c	2015-09-02 19:09:43.766811392 +0800
+++ linux/drivers/irqchip/irq-gic.c	2015-09-02 19:09:58.862811373 +0800
@@ -243,13 +243,30 @@
 }
 
 #ifdef CONFIG_SMP
+#ifdef CONFIG_AUTO_IRQ_BALANCE
+//Enable it by defualt if required disable via kernel cmdline option
+static int pilot4_irq_balance=1;
+static int __init boot_override_irq_balance(char* str)
+{
+    int offset;
+    if (sscanf(str, "%d", &offset) == 1)
+        pilot4_irq_balance = offset;
+
+    return 1;
+}
+__setup("irq_balance=", boot_override_irq_balance);
+#endif
+
 static int gic_set_affinity(struct irq_data *d, const struct cpumask *mask_val,
 			    bool force)
 {
 	void __iomem *reg = gic_dist_base(d) + GIC_DIST_TARGET + (gic_irq(d) & ~3);
 	unsigned int cpu, shift = (gic_irq(d) % 4) * 8;
 	u32 val, mask, bit;
-
+#ifdef CONFIG_AUTO_IRQ_BALANCE
+    cpumask_var_t new_mask;
+    u32 cpu_mask;
+#endif
 	if (!force)
 		cpu = cpumask_any_and(mask_val, cpu_online_mask);
 	else
@@ -261,6 +278,16 @@
 	raw_spin_lock(&irq_controller_lock);
 	mask = 0xff << shift;
 	bit = gic_cpu_map[cpu] << shift;
+#ifdef CONFIG_AUTO_IRQ_BALANCE
+    /*Do not always select first , based on user preference select all cpu's if required
+    */
+        if(pilot4_irq_balance && gic_irq(d) > 31) {
+        cpumask_and(new_mask, mask_val, cpu_online_mask);
+        cpu_mask = *cpumask_bits(new_mask);
+                bit = cpu_mask << shift;
+
+        }
+#endif
 	val = readl_relaxed(reg) & ~mask;
 	writel_relaxed(val | bit, reg);
 	raw_spin_unlock(&irq_controller_lock);
