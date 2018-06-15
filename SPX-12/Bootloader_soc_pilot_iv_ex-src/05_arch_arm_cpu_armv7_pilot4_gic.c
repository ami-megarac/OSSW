--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/gic.c	1969-12-31 19:00:00.000000000 -0500
+++ mywork/arch/arm/cpu/armv7/pilot4/gic.c	2015-02-24 12:37:26.608017404 -0500
@@ -0,0 +1,405 @@
+/*******************************************************************************
+ *
+ *
+ * Copyright (C) 2004-2014 Emulex. All rights reserved.
+ * EMULEX is a trademark of Emulex.
+ * www.emulex.com
+ *
+ * This program is free software; you can redistribute it and/or modify it under
+ * the terms of version 2 of the GNU General Public License as published by the
+ * Free Software Foundation.
+ * This program is distributed in the hope that it will be useful. ALL EXPRESS
+ * OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY IMPLIED
+ * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
+ * NON-INFRINGEMENT, ARE DISCLAIMED, EXCEPT TO THE EXTENT THAT SUCH DISCLAIMERS
+ * ARE HELD TO BE LEGALLY INVALID. See the GNU General Public License for more
+ * details, a copy of which can be found in the file COPYING included
+ * with this package.
+ *
+ ********************************************************************************/
+/*gic.c all of code is BMC CPU Reset-SDK Update*/
+#define readl_relaxed(c) (*(volatile unsigned int*)(c))
+#define writel_relaxed(v,c)	(*(volatile unsigned int*)(c) = v)
+typedef unsigned char bool;
+#define true 1
+#define false 0
+
+
+#define GIC_CPU_CTRL			0x00
+#define GIC_CPU_PRIMASK			0x04
+#define GIC_CPU_BINPOINT		0x08
+#define GIC_CPU_INTACK			0x0c
+#define GIC_CPU_EOI			0x10
+#define GIC_CPU_RUNNINGPRI		0x14
+#define GIC_CPU_HIGHPRI			0x18
+
+#define GIC_DIST_CTRL			0x000
+#define GIC_DIST_CTR			0x004
+#define GIC_DIST_ENABLE_SET		0x100
+#define GIC_DIST_ENABLE_CLEAR		0x180
+#define GIC_DIST_PENDING_SET		0x200
+#define GIC_DIST_PENDING_CLEAR		0x280
+#define GIC_DIST_ACTIVE_BIT		0x300
+#define GIC_DIST_PRI			0x400
+#define GIC_DIST_TARGET			0x800
+#define GIC_DIST_CONFIG			0xc00
+#define GIC_DIST_SOFTINT		0xf00
+
+#define NULL (void*)0
+#define u32 unsigned int
+extern void pilot_timer3_interrupt(void);//BMC CPU Reset-SDK Update
+/*
+ * IRQ line status.
+ *
+ * Bits 0-7 are the same as the IRQF_* bits in linux/interrupt.h
+ *
+ * IRQ_TYPE_NONE		- default, unspecified type
+ * IRQ_TYPE_EDGE_RISING		- rising edge triggered
+ * IRQ_TYPE_EDGE_FALLING	- falling edge triggered
+ * IRQ_TYPE_EDGE_BOTH		- rising and falling edge triggered
+ * IRQ_TYPE_LEVEL_HIGH		- high level triggered
+ * IRQ_TYPE_LEVEL_LOW		- low level triggered
+ *
+ */
+enum {
+	IRQ_TYPE_NONE		= 0x00000000,
+	IRQ_TYPE_EDGE_RISING	= 0x00000001,
+	IRQ_TYPE_EDGE_FALLING	= 0x00000002,
+	IRQ_TYPE_EDGE_BOTH	= (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING),
+	IRQ_TYPE_LEVEL_HIGH	= 0x00000004,
+	IRQ_TYPE_LEVEL_LOW	= 0x00000008,
+};
+
+/*BMC CPU Reset-SDK Update*/
+#define SERIAL_THR                     0x00                     /*  Transmitter Holding Register(Write).*/
+#define SERIAL_RBR                     0x00                     /*  Receive Buffer register (Read).*/
+#define SERIAL_IER                     0x04                     /*  Interrupt Enable register.*/
+#define SERIAL_IIR                     0x08                     /*  Interrupt Identification register(Read).*/
+#define SERIAL_FCR                     0x08                     /*  FIFO control register(Write).*/
+#define SERIAL_LCR                     0x0C                     /*  Line Control register.*/
+#define SERIAL_MCR                     0x10                     /*  Modem Control Register.*/
+#define SERIAL_LSR                     0x14                     /*  Line status register(Read) .*/
+#define SERIAL_MSR                     0x18                     /*  Modem Status register (Read).*/
+#define SERIAL_SPR                     0x1C             /*  Scratch pad register */
+#define SERIAL_DLL                     0x0              /*  Divisor Register LSB */
+#define SERIAL_DLM                     0x4              /*  Divisor Register MSB */
+
+#define SERIAL_LSR_THRE                0x20             /* THR Empty */
+
+static inline unsigned int get_config_base(void);
+static unsigned int gic_data_cpu_base(void)
+{
+    unsigned int gic_cpu_base = 0;
+    unsigned int peri_base;
+    peri_base = get_config_base();
+    gic_cpu_base = peri_base + 0x100;
+    return gic_cpu_base;
+}
+
+static unsigned int gic_data_dist_base(void)
+{
+    unsigned int gic_dist_base = 0;
+    unsigned int peri_base;
+    peri_base = get_config_base();
+    gic_dist_base = peri_base + 0x1000;
+    return gic_dist_base;
+}
+
+void very_early_putc(const char c)
+{
+    volatile unsigned int status=0;
+
+    /* Wait for Ready */
+    do
+    {
+        status = *(volatile unsigned char *)(0x40430000+SERIAL_LSR);
+    }
+    while (!((status & SERIAL_LSR_THRE)==SERIAL_LSR_THRE) );
+
+    /* Write Character */
+    *(volatile unsigned char *)(0x40430000+SERIAL_THR) = c;
+
+    return;
+}
+
+#define PRINTCH(x) very_early_putc(x)
+static inline unsigned int get_config_base(void)
+{
+	unsigned int val;
+	asm("mrc p15, 4, %0, c15, c0, 0	@ PERIPHBASE pase" : "=r" (val) : : "cc");
+	return val;
+}
+
+static inline unsigned int get_cr(void)
+{
+	unsigned int val;
+	asm("mrc p15, 0, %0, c1, c0, 0	@ get CR" : "=r" (val) : : "cc");
+	return val;
+}
+
+
+void gic_mask_irq(int irq_nr)
+{
+   //BMC CPU Reset-SDK Update
+    unsigned int gic_cpu_base = 0, gic_dist_base = 0;
+    unsigned int peri_base;
+    peri_base = get_config_base();
+    gic_cpu_base = peri_base + 0x100;
+    gic_dist_base = peri_base + 0x1000;
+
+	u32 mask = 1 << (irq_nr % 32);
+
+	writel_relaxed(mask, gic_dist_base + GIC_DIST_ENABLE_CLEAR + (irq_nr / 32) * 4);
+}
+
+void gic_unmask_irq(int irq_nr)
+{
+  //BMC CPU Reset-SDK Update 
+   u32 mask = 1 << (irq_nr % 32);
+    unsigned int gic_cpu_base = 0, gic_dist_base = 0;
+    unsigned int peri_base;
+    peri_base = get_config_base();
+    gic_cpu_base = peri_base + 0x100;
+    gic_dist_base = peri_base + 0x1000;
+
+    writel_relaxed(mask, gic_dist_base + GIC_DIST_ENABLE_SET + (irq_nr / 32) * 4);
+}
+
+void gic_eoi_irq(int irq_nr)
+{
+   //BMC CPU Reset-SDK Update
+    unsigned int gic_cpu_base = 0;
+    unsigned int peri_base;
+    peri_base = get_config_base();
+    gic_cpu_base = peri_base + 0x100;
+
+    writel_relaxed(irq_nr, gic_cpu_base + GIC_CPU_EOI);
+}
+
+int gic_set_type(int gicirq, unsigned int type)
+{
+    unsigned int base;
+    unsigned int gic_cpu_base = 0, gic_dist_base = 0;
+    unsigned int peri_base;
+    u32 enablemask = 1 << (gicirq % 32);
+    u32 enableoff = (gicirq / 32) * 4;
+    u32 confmask = 0x2 << ((gicirq % 16) * 2);
+    u32 confoff = (gicirq / 16) * 4;
+    bool enabled = false;
+    u32 val;
+    peri_base = get_config_base();
+    gic_cpu_base = peri_base + 0x100;
+    gic_dist_base = peri_base + 0x1000;
+    base = gic_dist_base;
+
+    /* Interrupt configuration for SGIs can't be changed */
+    if (gicirq < 16)
+        return -1;
+
+    if (type != IRQ_TYPE_LEVEL_HIGH && type != IRQ_TYPE_EDGE_RISING)
+        return -1;
+
+    val = readl_relaxed(base + GIC_DIST_CONFIG + confoff);
+    if (type == IRQ_TYPE_LEVEL_HIGH)
+        val &= ~confmask;
+    else if (type == IRQ_TYPE_EDGE_RISING)
+        val |= confmask;
+
+    /*
+     * As recommended by the spec, disable the interrupt before changing
+     * the configuration
+     */
+    if (readl_relaxed(base + GIC_DIST_ENABLE_SET + enableoff) & enablemask) {
+        writel_relaxed(enablemask, base + GIC_DIST_ENABLE_CLEAR + enableoff);
+        enabled = true;
+    }
+
+    writel_relaxed(val, base + GIC_DIST_CONFIG + confoff);
+
+    if (enabled)
+        writel_relaxed(enablemask, base + GIC_DIST_ENABLE_SET + enableoff);
+
+    return 0;
+}
+
+void gic_fiq_spurious(void)
+{
+    PRINTCH('F');
+    PRINTCH('I');
+    PRINTCH('Q');
+    PRINTCH('S');
+    PRINTCH('P');
+    PRINTCH('\n');
+    PRINTCH('\r');
+}
+
+#define wfe()	__asm__ __volatile__ ("wfe" : : : "memory")
+void  gic_handle_irq(void)
+{
+#define stbyint_sts         (1<<2)
+#define SYSCONTROL		0x40100100
+    char ch;
+    u32 irqstat, irqnr;
+    unsigned int gic_cpu_base = 0, gic_dist_base = 0;
+    unsigned int cpu_base;
+    unsigned int peri_base;
+    volatile unsigned long dummy_read;
+    peri_base = get_config_base();
+    gic_cpu_base = peri_base + 0x100;
+    gic_dist_base = peri_base + 0x1000;
+    cpu_base = gic_cpu_base;
+
+    do {
+        irqstat = readl_relaxed(cpu_base + GIC_CPU_INTACK);
+        irqnr = irqstat & ~0x1c00;
+        if ((irqnr > 15 && irqnr < 1021)) {
+            if(irqnr == 36) {
+                *(volatile unsigned int*)SYSCONTROL |= stbyint_sts;	
+                PRINTCH('I');
+                PRINTCH('R');
+                PRINTCH('Q');
+                PRINTCH('3');
+                PRINTCH('6');
+                PRINTCH('\n');
+                PRINTCH('\r');
+                /* Handle IRQ Here */
+            } 
+            else if(irqnr == 86) 
+            {
+     
+                  pilot_timer3_interrupt();
+            } 
+            else if(irqnr == 91)
+            {
+                PRINTCH('W');
+                PRINTCH('D');
+                PRINTCH('T');
+                PRINTCH('\n');
+                PRINTCH('\r');
+                gic_mask_irq(91);
+                gic_mask_irq(86);
+#ifndef CONFIG_USE_SSP_RESET
+                /* clear WDT pre trigger interrupt*/
+                if ((*(volatile unsigned int*)(0x40100880) & 0x40000) == 0x40000)
+                    *(volatile unsigned int*)(0x40100880) |= 0x40000;
+                if ((*(volatile unsigned int*)(0x40100890) & 0x40000) == 0x40000)
+                    *(volatile unsigned int*)(0x40100890) |= 0x40000;
+                if ((*(volatile unsigned int*)(0x401008a0) & 0x40000) == 0x40000)
+                    *(volatile unsigned int*)(0x401008a0) |= 0x40000;
+#endif
+                while(1)
+                    wfe();
+            } else {
+                PRINTCH('U');
+                PRINTCH('N');
+                PRINTCH('H');
+                PRINTCH('I');
+                PRINTCH('R');
+                PRINTCH('Q');
+            }
+            gic_eoi_irq(irqnr);
+            dummy_read=*((volatile unsigned long*) (0x40411000+0x0c + 0x28));//Timer3 excute EOI
+            continue;
+        }
+        if (irqnr < 16) {
+            writel_relaxed(irqstat, cpu_base + GIC_CPU_EOI);
+            continue;
+        }
+        break;
+    } while (1);
+}
+
+
+static void gic_cpu_init(void)
+{
+    void *dist_base = (void*)gic_data_dist_base();
+    void *base = (void*)gic_data_cpu_base();
+    int i;
+
+    /*
+     * Deal with the banked PPI and SGI interrupts - disable all
+     * PPI interrupts, ensure all SGI interrupts are enabled.
+     */
+    writel_relaxed(0xffff0000, dist_base + GIC_DIST_ENABLE_CLEAR);
+    writel_relaxed(0x0000ffff, dist_base + GIC_DIST_ENABLE_SET);
+
+    /*
+     * Set priority on PPI and SGI interrupts
+     */
+    for (i = 0; i < 32; i += 4)
+        writel_relaxed(0xa0a0a0a0, dist_base + GIC_DIST_PRI + i * 4 / 4);
+
+    writel_relaxed(0xf0, base + GIC_CPU_PRIMASK);
+    writel_relaxed(1, base + GIC_CPU_CTRL);
+}
+
+static void gic_dist_init(u32 cpu, unsigned int gic_irqs)
+{
+    unsigned int i;
+    u32 cpumask;
+    void *base = (void*)gic_data_dist_base();
+
+    cpumask = 1 << cpu;
+    cpumask |= cpumask << 8;
+    cpumask |= cpumask << 16;
+
+    writel_relaxed(0, base + GIC_DIST_CTRL);
+
+    /*
+     * Set all global interrupts to be level triggered, active low.
+     */
+    for (i = 32; i < gic_irqs; i += 16)
+        writel_relaxed(0, base + GIC_DIST_CONFIG + i * 4 / 16);
+
+    /*
+     * Set all global interrupts to this CPU only.
+     */
+    for (i = 32; i < gic_irqs; i += 4)
+        writel_relaxed(cpumask, base + GIC_DIST_TARGET + i * 4 / 4);
+
+    /*
+     * Set priority on all global interrupts.
+     */
+    for (i = 32; i < gic_irqs; i += 4)
+        writel_relaxed(0xa0a0a0a0, base + GIC_DIST_PRI + i * 4 / 4);
+
+    /*
+     * Disable all interrupts.  Leave the PPI and SGIs alone
+     * as these enables are banked registers.
+     */
+    for (i = 32; i < gic_irqs; i += 32)
+        writel_relaxed(0xffffffff, base + GIC_DIST_ENABLE_CLEAR + i * 4 / 32);
+
+    writel_relaxed(1, base + GIC_DIST_CTRL);
+}
+
+static inline void arch_local_irq_enable(void)
+{
+    asm volatile(
+            "	cpsie i			@ arch_local_irq_enable"
+            :
+            :
+            : "memory", "cc");
+}
+static  unsigned int get_cpsr(void)
+{
+    unsigned int val;
+    asm("mrs        %0, cpsr        @ get CR" : "=r" (val) : : "cc");
+    return val;
+}
+
+void gic_init(void)
+{
+    unsigned int gic_cpu_base = 0;
+    unsigned int gic_dist_base = 0;
+    int gic_irqs;
+    unsigned int peri_base;
+
+    peri_base = get_config_base();
+    gic_cpu_base = peri_base + 0x100;
+    gic_dist_base = peri_base + 0x1000;
+    gic_irqs = readl_relaxed(gic_data_dist_base() + GIC_DIST_CTR) & 0x1f;
+    gic_irqs = (gic_irqs + 1) * 32;
+    gic_dist_init(0, gic_irqs);
+    gic_cpu_init();
+}
