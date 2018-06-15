--- linux/arch/arm/kernel/smp.c	2017-03-29 19:50:16.237237000 +0800
+++ linux/arch/arm/kernel/smp.c 2017-03-29 19:51:55.390789000 +0800	
@@ -69,6 +69,11 @@
 	IPI_CPU_STOP,
 	IPI_IRQ_WORK,
 	IPI_COMPLETION,
+	/*SDK Update - BMC CPU Reset Workaround*/
+#ifdef CONFIG_SOC_SE_PILOT4
+	IPI_WFE,
+#endif
+
 };
 
 static DECLARE_COMPLETION(cpu_running);
@@ -471,6 +476,12 @@
 	S(IPI_CPU_STOP, "CPU stop interrupts"),
 	S(IPI_IRQ_WORK, "IRQ work interrupts"),
 	S(IPI_COMPLETION, "completion interrupts"),
+
+/*SDK Update - BMC CPU Reset Workaround*/	
+#ifdef CONFIG_SOC_SE_PILOT4
+	S(IPI_WFE, "Wfe interrupts"),
+#endif
+
 };
 
 void show_ipi_list(struct seq_file *p, int prec)
@@ -520,14 +531,23 @@
 		dump_stack();
 		raw_spin_unlock(&stop_lock);
 	}
-
+	
 	set_cpu_online(cpu, false);
 
 	local_fiq_disable();
 	local_irq_disable();
-
+/*SDK Update - BMC CPU Reset Workaround*/	
+#ifdef CONFIG_SOC_SE_PILOT4
+        raw_spin_lock(&stop_lock);
+        printk(KERN_CRIT "CPU%u: WFE\n", cpu);
+        raw_spin_unlock(&stop_lock);
+        while (1)
+                wfe();
+#else
 	while (1)
 		cpu_relax();
+#endif
+
 }
 
 static DEFINE_PER_CPU(struct completion *, cpu_completion);
@@ -575,6 +595,18 @@
 		scheduler_ipi();
 		break;
 
+/*SDK Update - BMC CPU Reset Workaround*/
+#ifdef CONFIG_SOC_SE_PILOT4
+	case IPI_WFE:
+	#ifdef CONFIG_USE_SSP_RESET
+		local_irq_disable();
+	#endif
+		//printk("core1 going to wfe\n");
+		for(;;)
+			wfe();
+		break;
+#endif
+
 	case IPI_CALL_FUNC:
 		irq_enter();
 		generic_smp_call_function_interrupt();
@@ -620,6 +652,43 @@
 	smp_cross_call(cpumask_of(cpu), IPI_RESCHEDULE);
 }
 
+/*SDK Update - BMC CPU Reset Workaround*/
+#ifdef CONFIG_SOC_SE_PILOT4
+void smp_send_wfe(int cpu)
+{
+	smp_cross_call(cpumask_of(cpu), IPI_WFE);
+}
+EXPORT_SYMBOL(smp_send_wfe);
+
+void execute_smp_wfe(void)
+{
+	int cpu;
+	int i = 0x0;
+	u32 wfe_status;
+
+	cpu = task_cpu(current);
+
+	//printk("Entered %s cpu %d\n", __FUNCTION__, cpu);
+
+	for_each_online_cpu(cpu) {
+		if (cpu == smp_processor_id())
+			continue;
+		//printk("sending wfe to %d cpu\n", cpu);
+		smp_send_wfe(cpu);
+		#ifndef CONFIG_USE_SSP_RESET
+		for(i = 0; i < 0xFFFFF; i++) {
+			wfe_status = *(volatile unsigned int *)IO_ADDRESS(0x40100D00);
+			//printk("wfe_status %x\n", wfe_status);
+			if(!(wfe_status & (1<<27)))
+				continue;
+			break;
+		}
+		#endif
+	}
+}
+EXPORT_SYMBOL(execute_smp_wfe);
+#endif
+
 void smp_send_stop(void)
 {
 	unsigned long timeout;
