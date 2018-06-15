--- linux-3.14.17/arch/arm/soc-pilot/pilot-irq.c	1970-01-01 05:30:00.000000000 +0530
+++ linux-3.14.17.orion/arch/arm/soc-pilot/pilot-irq.c	2015-04-03 15:48:24.101140394 +0530
@@ -0,0 +1,62 @@
+/*
+ *  linux/arch/arm/mach-xxx/xxx-irq.c
+ *
+ *  Copyright (C) 2015 American Megatrends Inc
+ *
+ *  SE PILOT-II SOC IRQ handling functions
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation; either version 2 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#include <linux/init.h>
+#include <linux/interrupt.h>
+#include <linux/list.h>
+#include <asm/mach/irq.h>
+#include <asm/irq.h>
+#include <mach/hardware.h>
+#include <linux/irqchip/arm-gic.h>
+#include <linux/of_irq.h>
+#include <linux/irqdomain.h>
+#include <linux/kallsyms.h>
+#include <asm/mach-types.h>
+#include <linux/io.h>
+#include <linux/irqchip/chained_irq.h>
+
+extern void power_putstr(const char *ptr);
+
+void __iomem *p4smp_base_addr = NULL;
+static inline unsigned int get_config_base(void)
+{
+  unsigned int val;
+  asm("mrc p15, 4, %0, c15, c0, 0 @ PERIPHBASE pase" : "=r" (val) : : "cc");
+  return val;
+}
+
+void __init pilot4_init_irq(void)
+{
+  unsigned int peri_base;
+  void __iomem *gic_cpu_base_addr;
+  void __iomem *gic_dist_base_addr;
+
+  peri_base = get_config_base();
+
+  gic_dist_base_addr = ioremap((peri_base + 0x1000), SZ_4K);
+
+  gic_cpu_base_addr = ioremap((peri_base + 0x100), SZ_4K);
+  p4smp_base_addr = ioremap(0x40100D00 , SZ_128);
+  printk("pilot4_init_irq %p %p peri_base %x\n", gic_dist_base_addr, gic_cpu_base_addr, peri_base);
+  gic_init(0, -1, gic_dist_base_addr, gic_cpu_base_addr);
+}
+
