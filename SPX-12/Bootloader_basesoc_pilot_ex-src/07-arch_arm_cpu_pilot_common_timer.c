--- uboot_old/arch/arm/cpu/pilotcommon/pilottimer.c	2017-08-09 15:05:34.350547884 +0530
+++ uboot/arch/arm/cpu/pilotcommon/pilottimer.c	2017-08-09 11:59:32.031278437 +0530
@@ -0,0 +1,232 @@
+/*
+ * (C) Copyright 2006
+ * American Megatrends Inc.
+ *
+ * Timer functions for the Pilot-II SOC
+ *
+ * See file CREDITS for list of people who contributed to this
+ * project.
+ *
+ * This program is free software; you can redistribute it and/or
+ * modify it under the terms of the GNU General Public License as
+ * published by the Free Software Foundation; either version 2 of
+ * the License, or (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+ * MA 02111-1307 USA
+ */
+
+#include <common.h>
+#include <asm/proc-armv/ptrace.h>
+#include <pilot_hw.h>
+
+#define READ_TIMER (*(volatile ulong *)(TIMER1_COUNT_REG));
+#define TIMER_LOAD_VAL P_CLK
+
+#define ULONG_ROLLVER_CORRECTION (~(0UL)/(P_CLK/1000))
+
+static volatile ulong timestamp;
+static volatile ulong lastdec;
+
+#ifdef CONFIG_PILOT4
+/*polot_timer.c all of code is BMC CPU Reset-SDK Update*/
+#include <configs/orion.h>
+#endif
+
+#define CLOCK_TICK_RATE 25000000
+#define PRINTCH(x) very_early_putc(x)
+#define u32 unsigned int
+#ifdef CONFIG_PILOT4
+#define wfe()   __asm__ __volatile__ ("wfe" : : : "memory")
+#endif
+
+extern void very_early_putc(const char c);
+extern void gic_mask_irq(int irq_nr);
+extern void gic_unmask_irq(int irq_nr);
+
+int timer_init (void)
+{
+    /* Load the counter and start timer */
+    *(volatile ulong *)(TIMER1_LOAD_REG) = TIMER_LOAD_VAL;
+    *(volatile ulong *)(TIMER1_CONTROL_REG) = TIMER_ENABLE | TIMER_PERIODICAL;
+
+    /* init the timestamp and lastdec value */
+    reset_timer_masked();
+
+    return (0);
+}
+
+/*
+ * timer without interrupts
+ */
+
+void reset_timer (void)
+{
+    reset_timer_masked ();
+}
+
+ulong get_timer (ulong base)
+{
+    ulong time_mask = get_timer_masked();
+    if ((int)(time_mask - base) < 0)
+    {
+        return ((ULONG_ROLLVER_CORRECTION - base) + time_mask);
+    }
+
+    return (time_mask - base);
+}
+
+void set_timer (ulong t)
+{
+    timestamp = t;
+}
+
+/* delay x useconds AND perserve advance timstamp value */
+void __udelay (unsigned long usec)
+{
+    udelay_masked(usec);
+}
+
+void reset_timer_masked (void)
+{
+    /* reset time */
+    lastdec = READ_TIMER;  /* capure current decrementer value time */
+    timestamp = 0;         /* start "advancing" time stamp from 0 */
+}
+
+ulong get_timer_masked (void)
+{
+    volatile ulong now = READ_TIMER;        /* current tick value */
+
+    if (lastdec >= now) {       /* normal mode (non roll) */
+        /* normal mode */
+        timestamp += (lastdec - now); /* move stamp fordward with absoulte diff ticks */
+    } else {            /* we have overflow of the count down timer */
+        /* nts = ts + ld + (TLV - now)
+         * ts=old stamp, ld=time that passed before passing through -1
+         * (TLV-now) amount of time after passing though -1
+         * nts = new "advancing time stamp"...it could also roll and cause problems.
+         */
+        timestamp += (lastdec + TIMER_LOAD_VAL - now);
+    }
+    lastdec = now;
+
+    return (timestamp)/(P_CLK/1000);
+}
+
+/* waits specified delay value and resets timestamp */
+void udelay_masked (unsigned long usec)
+{
+//  volatile ulong tmo;
+//  volatile ulong endtime;
+//  volatile signed long diff;
+//  volatile ulong water_mark;
+
+    long small_granul = (usec*1000)/40;
+    unsigned long prev = READ_TIMER; 
+
+    while(small_granul > 0){
+
+    volatile ulong now = READ_TIMER;                /* current tick value */
+    if(prev == now){
+        continue;
+    }
+
+        if (prev >= now) {           /* normal mode (non roll) */
+                /* normal mode */
+                small_granul -= (prev - now); /* move stamp fordward with absoulte diff ticks */
+        } else {                        /* we have overflow of the count down timer */
+                small_granul -= (prev + TIMER_LOAD_VAL - now); /* move stamp fordward with absoulte diff ticks */
+        }
+        prev = now;
+    }
+}
+
+
+/*
+ * This function is derived from PowerPC code (read timebase as long long).
+ * On ARM it just returns the timer value.
+ */
+unsigned long long get_ticks(void)
+{
+    return get_timer(0);
+}
+
+/*
+ * This function is derived from PowerPC code (timebase clock frequency).
+ * On ARM it returns the number of timer ticks per second.
+ */
+ulong get_tbclk (void)
+{
+    ulong tbclk;
+
+    tbclk = CONFIG_SYS_HZ;
+    return tbclk;
+}
+
+
+/*polot_timer.c all of code is BMC CPU Reset-SDK Update*/
+void pilot_timer3_interrupt(void)
+{
+    volatile u32 sysrst_status;
+    volatile u32 *sysrst_status_ptr;
+    sysrst_status = (volatile u32)(0x401008b0);
+    sysrst_status_ptr = (void*)sysrst_status;
+#ifndef CONFIG_USE_SSP_RESET
+    // Ping WDT2
+   *(volatile unsigned int*)(0x4010089C) = 0x1DC4FFFF;
+    *(volatile unsigned int*)(0x40100890) |= 0x800000;
+#endif
+    if(*(volatile u32*)sysrst_status_ptr & (1 << 10)) 
+    {
+	PRINTCH('B');
+	PRINTCH('M');
+	PRINTCH('C');
+	PRINTCH('\n');
+	gic_mask_irq(91);
+    gic_mask_irq(86);
+#ifndef CONFIG_USE_SSP_RESET
+    *(volatile unsigned int*)(0x401008b0) |= (1 << 10);
+    *(volatile unsigned int*)(0x40100858) = 0;
+    *(volatile unsigned int*)(0x40100854) = 3;
+    *(volatile unsigned int*)(0x40100850) |= 1;
+#endif
+#ifdef CONFIG_PILOT4
+	while(1)
+		wfe();
+#endif
+    }
+}
+
+static void set_periodic_timer3(void)
+{
+    volatile unsigned long dummy_read;
+    unsigned long ctrl;
+    /* Disable timer and interrups */
+    *((volatile unsigned long *)(SE_TIMER_VA_BASE+0x08 + 0x28)) = 0x0;//40411000+0x08+0x28= 0x40411030 = 0 Disable timer and interrups 
+    dummy_read = *((volatile unsigned long*) (SE_TIMER_VA_BASE+0x0c + 0x28));
+    dummy_read++;
+    /* Load counter values 100ms*/
+    *((volatile unsigned long *)(SE_TIMER_VA_BASE + 0x28)) = ((CLOCK_TICK_RATE + 10/2) / 10);
+    ctrl = 0x3;
+    *((volatile unsigned long *)(SE_TIMER_VA_BASE+0x08 + 0x28)) = ctrl;
+}
+
+
+void stop_pilot_timer3(void)
+{
+            *((volatile unsigned long *)(SE_TIMER_VA_BASE+0x08 + 0x28)) = 0x0;
+}
+
+void start_pilot_timer3(void)
+{   
+            set_periodic_timer3();
+            gic_unmask_irq(86);
+}
