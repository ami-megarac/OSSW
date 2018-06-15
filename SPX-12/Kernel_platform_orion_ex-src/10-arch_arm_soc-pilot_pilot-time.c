--- linux-3.14.17/arch/arm/soc-pilot/pilot-time.c	1970-01-01 05:30:00.000000000 +0530
+++ linux/arch/arm/soc-pilot/pilot-time.c	2015-08-11 11:23:41.157923058 +0530
@@ -0,0 +1,322 @@
+/*
+ *  linux/arch/arm/mach-xxx/xxx-time.c
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
+ *
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
+/*
+ * Copyright (c) 2010-2015, Emulex Corporation.
+ * Modifications made by Emulex Corporation under the terms of the
+ * GNU General Public License as published by the Free Software
+ * Foundation; either version 2 of the License, or (at your option)
+ * any later version.
+ */
+
+#include <linux/kernel.h>
+#include <linux/interrupt.h>
+#include <linux/time.h>
+#include <linux/init.h>
+#include <linux/timex.h>
+
+#include <mach/hardware.h>
+#include <asm/io.h>
+#include <asm/irq.h>
+#include <asm/uaccess.h>
+#include <asm/mach/irq.h>
+#include <asm/mach/time.h>
+#include <linux/clocksource.h>
+#include <linux/clockchips.h>
+#include <asm/smp_twd.h>
+
+//#define CLOCK_TICK_RATE1			(25*1000*1000*2)
+#define OUR_LATCH  ((CLOCK_TICK_RATE + HZ/2) / HZ)	/* For divider */
+
+static inline unsigned long get_elapsed(void)
+{
+	unsigned long counter;
+	counter = *(volatile unsigned long *)(SE_TIMER_VA_BASE + 0x4);  // Current Value of Counter
+	return OUR_LATCH-counter;
+}
+
+static irqreturn_t pilot_timer_interrupt(int irq, void *dev_id);
+static struct irqaction se_pilot4_timer_irq = {
+	.name		= "Pilot4 Timer Tick",
+	.handler	= pilot_timer_interrupt,
+	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
+};
+
+static struct clock_event_device pilot_clockevent;
+
+static irqreturn_t pilot_timer_interrupt(int irq, void *dev_id)
+{
+	volatile unsigned long dummy_read;
+	struct clock_event_device *evt = &pilot_clockevent;
+	/* Clear Interrupt */
+	dummy_read = *((volatile unsigned long*) (SE_TIMER_VA_BASE+0x0c));
+
+	evt->event_handler(evt);
+
+	return IRQ_HANDLED;
+}
+
+/*SDK Update - BMC CPU Reset Workaround*/
+static void start_wdt2(void);
+static void wdt2_ping(void);
+extern void do_bmc_cpu_reset(void);
+static irqreturn_t pilot_timer3_interrupt(int irq, void *dev_id)
+{
+	volatile u32 sysrst_status;
+	volatile u32 *sysrst_status_ptr;
+	volatile unsigned long dummy_read;
+	unsigned long flags;
+
+	local_irq_save(flags);
+
+	/* Reload watchdog duration */
+	dummy_read = *((volatile unsigned long*) (SE_TIMER_VA_BASE+0x0c + 0x28));
+
+#ifndef CONFIG_USE_SSP_RESET
+	wdt2_ping();
+#endif
+	sysrst_status = (volatile u32)IO_ADDRESS(0x401008b0);
+	sysrst_status_ptr = (void*)sysrst_status;
+	if(*(volatile u32*)sysrst_status_ptr & (1 << 10))
+	{
+		#ifdef CONFIG_USE_SSP_RESET
+			local_irq_disable();
+		#else
+			*(volatile u32*)sysrst_status_ptr = (1 << 10);
+			printk("Resetting BMC CPU\n");
+		#endif
+		do_bmc_cpu_reset();
+	}
+ 
+	local_irq_restore(flags);
+	return IRQ_HANDLED;
+}
+
+static int pilot_set_next_event(unsigned long evt,
+			struct clock_event_device *unused)
+{
+	unsigned long ctrl = 1;//Timer enable
+	*((volatile unsigned long *)(SE_TIMER_VA_BASE+0x08)) = 0;//Disable
+	*((volatile unsigned long *)(SE_TIMER_VA_BASE)) = evt;
+	/* Enable Interrupts and timer */
+	*((volatile unsigned long *)(SE_TIMER_VA_BASE+0x08)) = ctrl;
+	return 0;
+}
+
+static void pilot_set_mode(enum clock_event_mode mode,
+			struct clock_event_device *clk)
+{
+	volatile unsigned long dummy_read;
+	unsigned long ctrl;
+	switch (mode) {
+	case CLOCK_EVT_MODE_PERIODIC:
+		printk("CLOCK_EVT_MODE_PERIODIC OUR_LATCH %x CLOCK_TICK_RATE %d\n",  OUR_LATCH, CLOCK_TICK_RATE);
+		/* timer load already set up */
+		/* Disable timer and interrups */
+		*((volatile unsigned long *)(SE_TIMER_VA_BASE+0x08)) = 0x0;
+		dummy_read = *((volatile unsigned long*) (SE_TIMER_VA_BASE+0x0c));
+		/* Load counter values */
+		*((volatile unsigned long *)(SE_TIMER_VA_BASE)) = (OUR_LATCH);
+		ctrl = 0x3;
+		break;
+	case CLOCK_EVT_MODE_ONESHOT:
+		printk("CLOCK_EVT_MODE_ONESHOT OUR_LATCH %x CLOCK_TICK_RATE %d\n",  OUR_LATCH, CLOCK_TICK_RATE);
+		dummy_read = *((volatile unsigned long*) (SE_TIMER_VA_BASE+0x0c));
+		/* Load counter values */
+		/* period set, and timer enabled in 'next_event' hook */
+		ctrl = 0x0;//One shot
+		break;
+	case CLOCK_EVT_MODE_UNUSED:
+	case CLOCK_EVT_MODE_SHUTDOWN:
+	default:
+		ctrl = 0;
+	}
+	/* Enable Interrupts and timer */
+	*((volatile unsigned long *)(SE_TIMER_VA_BASE+0x08)) = ctrl;
+	printk("0 %x 4 %x 8 %x c %x\n", *((volatile unsigned int*)(SE_TIMER_VA_BASE+0x0)), 
+		*((volatile unsigned int*)(SE_TIMER_VA_BASE+0x04)), *((volatile unsigned int*)(SE_TIMER_VA_BASE+0x08)),
+		*((volatile unsigned int*)(SE_TIMER_VA_BASE+0x0c)));
+}
+
+
+/*SDK Update - BMC CPU Reset Workaround*/
+/*Wdt2*/
+#define SYSWCR              0x90
+#define SYSWRERL            0x94
+#define SYSWRERH            0x98
+#define SYSWCFR             0x9C
+
+#define WDT_TCLK			381
+#define WDT_EN              0x800000
+#define PRE_TRIG_EN         0x400000
+#define TRIG_WDT            0x1
+#define RST_PULSE_WIDTH     0xC     /*using default value 0xC*(40.96us)=491.52us*/
+
+#define PRETIMEOUT 			20
+#define HEARTBEAT			172
+static void __iomem *wdt_reg = ( void __iomem *)IO_ADDRESS(0x40100800);
+
+void start_wdt2(void)
+{
+	volatile u32 reg;
+	volatile u32 wdt_cnt;
+	
+	/* Set watchdog duration */
+	wdt_cnt = ((WDT_TCLK * PRETIMEOUT) << 16) | (WDT_TCLK * HEARTBEAT);
+	writel(wdt_cnt, wdt_reg + SYSWCFR);
+
+	/* Enable ARM reset on watchdog */
+	writel(0x3, wdt_reg + SYSWRERL);
+	writel(0x0, wdt_reg + SYSWRERH);
+
+	/* Trigger reset on watchdog */
+	reg = readl(wdt_reg + SYSWCR);
+	reg |= WDT_EN;
+	writel(reg, wdt_reg + SYSWCR);
+	/* Enable watchdog timer */
+	reg = readl(wdt_reg + SYSWCR);
+	reg &=~(0xff<<8);
+	reg |= PRE_TRIG_EN;
+	reg |= TRIG_WDT;
+	reg |= RST_PULSE_WIDTH << 8;
+	
+	writel(reg, wdt_reg + SYSWCR);
+}
+
+static void wdt2_ping(void)
+{
+	volatile u32 reg;
+	volatile u32 wdt_cnt;
+
+	/* Reload watchdog duration */
+	wdt_cnt = ((WDT_TCLK * PRETIMEOUT) << 16) | (WDT_TCLK * HEARTBEAT);
+	writel(wdt_cnt, wdt_reg + SYSWCFR);
+
+	/* Trigger reset on watchdog */
+	reg = readl(wdt_reg + SYSWCR);
+	reg |= WDT_EN;
+
+	writel(reg, wdt_reg + SYSWCR);
+}
+
+static void set_periodic_timer3(void)
+{
+	volatile unsigned long dummy_read;
+	unsigned long ctrl;
+	volatile u32 bmccpu_rerl;
+	volatile u32 *bmccpu_rerl_ptr; 
+ 
+	bmccpu_rerl = (volatile u32)IO_ADDRESS(0x401008b4);
+	bmccpu_rerl_ptr = (void*)bmccpu_rerl;
+	*(volatile u32*)bmccpu_rerl_ptr &= ~(1);	//mask ARM reset
+	
+	#ifndef CONFIG_USE_SSP_RESET
+		start_wdt2();
+	#endif
+
+	/* Disable timer and interrups */
+	*((volatile unsigned long *)(SE_TIMER_VA_BASE+0x08 + 0x28)) = 0x0;
+	dummy_read = *((volatile unsigned long*) (SE_TIMER_VA_BASE+0x0c + 0x28));
+	/* Load counter values 100ms*/
+	*((volatile unsigned long *)(SE_TIMER_VA_BASE + 0x28)) = ((CLOCK_TICK_RATE + 10/2) / 10);
+	ctrl = 0x3;
+	*((volatile unsigned long *)(SE_TIMER_VA_BASE+0x08 + 0x28)) = ctrl;
+	printk("Timer3 100ms periodic timer startedd\n");
+}
+static struct clock_event_device pilot_clockevent = {
+	.name		= "pilot_sys_timer1",
+	.features	= CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
+	.set_next_event	= pilot_set_next_event,
+	.set_mode	= pilot_set_mode,
+	.rating		= 300,
+};
+
+static cycle_t notrace pilot4_read_cycles(struct clocksource *cs)
+{
+        return (cycle_t) (0xffffffff - *((volatile unsigned long *)(SE_TIMER_VA_BASE + 0x14 +0x4)));
+}
+
+cycle_t notrace pilot4_read_raw_cycle(void)
+{
+        return (cycle_t) (0xffffffff - *((volatile unsigned long *)(SE_TIMER_VA_BASE + 0x14 +0x4)));
+}
+
+unsigned int  notrace read_raw_cycle(void)
+{
+        return (u32)*((volatile unsigned long *)(SE_TIMER_VA_BASE + 0x14 +0x4));
+}
+EXPORT_SYMBOL(read_raw_cycle);
+static struct clocksource pilot_clk_src = {
+        .name           = "pilot4_timer2",
+        .rating         = 200,
+        .read           = pilot4_read_cycles,
+        .mask           = CLOCKSOURCE_MASK(32),
+        .flags          = CLOCK_SOURCE_IS_CONTINUOUS,
+};
+
+
+#ifdef CONFIG_HAVE_ARM_TWD
+static DEFINE_TWD_LOCAL_TIMER(twd_local_timer,
+			      0x40460600, 29);
+#endif
+static void __init clk_src_pilot_init(void)
+{
+
+       /* timer load already set up */
+       /* Disable timer and interrups */
+       *((volatile unsigned long *)(SE_TIMER_VA_BASE+0x08 +0x14)) = 0x0;
+       /* Load counter values */
+       *((volatile unsigned long *)(SE_TIMER_VA_BASE + 0x14)) = (0xffffffff);
+        printk("Registering clock source timercrnt addr %x\n", (SE_TIMER_VA_BASE + 0x14 +0x4));
+       *((volatile unsigned long *)(SE_TIMER_VA_BASE + 0x8 + 0x14)) = 5;//no interrupt ,free running , start
+        clocksource_register_hz(&pilot_clk_src, 25000000);
+}
+
+/*SDK Update - BMC CPU Reset Workaround*/
+void start_pilot_timer3(void)
+ {
+	int err;
+	set_periodic_timer3();
+	err = request_irq(86, &pilot_timer3_interrupt, IRQF_SHARED, "timer3_cpu_reset", (void*)0xDEADBEEF);
+	if(err < 0) {
+		printk(KERN_ERR "Error requesting Timer3 irq\n");
+	}
+}
+
+void __init
+se_pilot4_timer_init(void)
+{
+
+	int  err;
+ pilot_clockevent.cpumask = cpumask_of(smp_processor_id());
+	clockevents_config_and_register(&pilot_clockevent, 25*1000*1000,
+					0xff, 0xffffffff);
+	/* Setup Timer Interrupt routine */
+	setup_irq(IRQ_TIMER_0, &se_pilot4_timer_irq);
+	printk("mult %x shift %x max_delta_ns %llx min_delta_ns %llx\n",pilot_clockevent.mult, pilot_clockevent.shift, pilot_clockevent.max_delta_ns, pilot_clockevent.min_delta_ns);
+	printk("Pilot-4 Timer configured\n");
+#ifdef CONFIG_HAVE_ARM_TWD
+	err = twd_local_timer_register(&twd_local_timer);
+	if (err)
+		pr_err("twd_local_timer_register failed %d\n", err);
+#endif
+	clk_src_pilot_init();
+	return;
+}
