--- linux-3.14.17/arch/arm/mach-pilot/smp.c	1970-01-01 05:30:00.000000000 +0530
+++ linux-3.14.17.orion/arch/arm/mach-pilot/smp.c	2015-04-08 19:55:52.229785580 +0530
@@ -0,0 +1,140 @@
+/*
+ * This file contains Xilinx specific SMP code, used to start up
+ * the second processor.
+ *
+ * Copyright (C) 2011 Xilinx
+ *
+ * based on linux/arch/arm/mach-realview/platsmp.c
+ *
+ * Copyright (C) 2002 ARM Ltd.
+ *
+ * This software is licensed under the terms of the GNU General Public
+ * License version 2, as published by the Free Software Foundation, and
+ * may be copied, distributed, and modified under those terms.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ */
+#include <linux/jiffies.h>
+#include <linux/init.h>
+#include <linux/io.h>
+#include <linux/smp.h>
+#include <asm/smp.h>
+#include <linux/cpu.h>
+#include <asm/cacheflush.h>
+#include <asm/smp_scu.h>
+//#include <linux/irqchip/arm-gic.h>
+#include <mach/pilot4-soc.h>
+
+extern void secondary_startup(void);
+
+extern void __iomem *p4smp_base_addr;
+
+static DEFINE_SPINLOCK(boot_lock);
+
+/* Secondary CPU kernel startup is a 2 step process. The primary CPU
+ * starts the secondary CPU by giving it the address of the kernel and
+ * then sending it an event to wake it up. The secondary CPU then
+ * starts the kernel and tells the primary CPU it's up and running.
+ */
+void __cpuinit platform_secondary_init(unsigned int cpu)
+{
+	/*
+	 * if any interrupts are already enabled for the primary
+	 * core (e.g. timer irq), then they will not have been enabled
+	 * for us: do so
+	 */
+	//gic_secondary_init(NULL,CPU_STARTING,NULL); //This is required to make the secondary processor no about the interrupts of P1
+
+/* Indicate to the primary core that the secondary is up and running.
+   * Let the write buffer drain.
+   */
+  __raw_writel(0xBADABABA, p4smp_base_addr + 0xC);
+
+	wmb();
+
+	/*
+	 * Synchronise with the boot thread.
+	 */
+	spin_lock(&boot_lock);
+	spin_unlock(&boot_lock);
+}
+
+int __cpuinit platform_boot_secondary(unsigned int cpu, struct task_struct *idle)
+{
+	unsigned long timeout;
+
+	/*
+	 * set synchronisation state between this boot processor
+	 * and the secondary one
+	 */
+	spin_lock(&boot_lock);
+
+  /* Initialize the boot status and give the secondary core
+   * the start address of the kernel, let the write buffer drain
+   */
+      __raw_writel(0, p4smp_base_addr + 4);
+      __raw_writel(virt_to_phys(secondary_startup), p4smp_base_addr + 4);
+      __raw_writel(0xABBAADDA, p4smp_base_addr + 0xC);
+
+      printk("Sending event to CPU1, wake up lazy bones!!!!\n");
+
+	wmb();
+
+	/*
+	 * Send an event to wake the secondary core from WFE state.
+	 */
+	sev();
+
+ /*
+   * Wait for the other CPU to boot, but timeout if it doesn't
+   */
+  timeout = jiffies + (1 * HZ);
+  while ((__raw_readl(p4smp_base_addr+ 0xC) !=
+        0xBADABABA) &&
+        (time_before(jiffies, timeout)))
+    rmb();
+
+   printk("Second Guy up, lot to do!!!!\n");
+
+	/*
+	 * now the secondary core is starting up let it run its
+	 * calibrations, then wait for it to finish
+	 */
+	spin_unlock(&boot_lock);
+
+	return 0;
+}
+
+/*
+ * Initialise the CPU possible map early - this describes the CPUs
+ * which may be present or become present in the system.
+ */
+void __init platform_smp_init_cpus(void)
+{
+	int i, ncores;
+
+
+	ncores = scu_get_core_count(SCU_PERIPH_BASE);
+       printk("Total Number of CPU's are %d\n",ncores);
+
+	for (i = 0; i < ncores; i++)
+		set_cpu_possible(i, true);
+
+	//set_smp_cross_call(gic_raise_softirq);
+}
+
+void __init platform_smp_prepare_cpus(unsigned int max_cpus)
+{
+	scu_enable(SCU_PERIPH_BASE);
+}
+
+struct smp_operations platform_smp_ops __initdata = {
+  .smp_init_cpus    = platform_smp_init_cpus,
+  .smp_prepare_cpus = platform_smp_prepare_cpus,
+  .smp_boot_secondary = platform_boot_secondary,
+  .smp_secondary_init = platform_secondary_init,
+};
+
