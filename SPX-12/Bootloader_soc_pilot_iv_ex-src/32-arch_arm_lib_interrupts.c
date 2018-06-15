--- u-boot-2013.07/arch/arm/lib/interrupts.c	2013-12-13 13:49:25.392958498 -0500
+++ uboot.new/arch/arm/lib/interrupts.c	2013-12-05 12:17:55.071503948 -0500
@@ -37,7 +37,8 @@
 
 #include <common.h>
 #include <asm/proc-armv/ptrace.h>
-
+extern void  gic_handle_irq(void);//BMC CPU Reset-SDK Update
+extern int nc_printf(const char *format, ...);//BMC CPU Reset-SDK Update
 DECLARE_GLOBAL_DATA_PTR;
 
 #ifdef CONFIG_USE_IRQ
@@ -49,14 +50,37 @@
 	IRQ_STACK_START = gd->irq_sp - 4;
 	IRQ_STACK_START_IN = gd->irq_sp + 8;
 	FIQ_STACK_START = IRQ_STACK_START - CONFIG_STACKSIZE_IRQ;
+	/*BMC CPU Reset-SDK Update*/
+	#ifdef  CONFIG_PILOT4
+		return 0;
+	#else
+		return arch_interrupt_init();
+	#endif
+	
 
-	return arch_interrupt_init();
 }
 
+
+static inline void set_vbar(unsigned int val)/*BMC CPU Reset-SDK Update*/
+{
+	asm volatile("mcr p15, 0, %0, c12, c0, 0	@ set VBAR"
+	  : : "r" (val) : "cc");//Update SDK
+ }
+ 
+
 /* enable IRQ interrupts */
 void enable_interrupts (void)
 {
+	extern unsigned int UBOOT_IRQ_VBAR;//Update SDK
 	unsigned long temp;
+	/*BMC CPU Reset-SDK Update*/
+	disable_interrupts();
+	if(UBOOT_IRQ_VBAR != 0) {
+		set_vbar(UBOOT_IRQ_VBAR);
+	} else {
+		nc_printf("UBOOT VBAR still showing 0 not possible?\n");
+	}
+	
 	__asm__ __volatile__("mrs %0, cpsr\n"
 			     "bic %0, %0, #0x80\n"
 			     "msr cpsr_c, %0"
@@ -125,21 +149,21 @@
 
 	flags = condition_codes (regs);
 
-	printf ("pc : [<%08lx>]	   lr : [<%08lx>]\n"
+	nc_printf ("pc : [<%08lx>]	   lr : [<%08lx>]\n"
 		"sp : %08lx  ip : %08lx	 fp : %08lx\n",
 		instruction_pointer (regs),
 		regs->ARM_lr, regs->ARM_sp, regs->ARM_ip, regs->ARM_fp);
-	printf ("r10: %08lx  r9 : %08lx	 r8 : %08lx\n",
+	nc_printf ("r10: %08lx  r9 : %08lx	 r8 : %08lx\n",
 		regs->ARM_r10, regs->ARM_r9, regs->ARM_r8);
-	printf ("r7 : %08lx  r6 : %08lx	 r5 : %08lx  r4 : %08lx\n",
+	nc_printf ("r7 : %08lx  r6 : %08lx	 r5 : %08lx  r4 : %08lx\n",
 		regs->ARM_r7, regs->ARM_r6, regs->ARM_r5, regs->ARM_r4);
-	printf ("r3 : %08lx  r2 : %08lx	 r1 : %08lx  r0 : %08lx\n",
+	nc_printf ("r3 : %08lx  r2 : %08lx	 r1 : %08lx  r0 : %08lx\n",
 		regs->ARM_r3, regs->ARM_r2, regs->ARM_r1, regs->ARM_r0);
-	printf ("Flags: %c%c%c%c",
+	nc_printf ("Flags: %c%c%c%c",
 		flags & CC_N_BIT ? 'N' : 'n',
 		flags & CC_Z_BIT ? 'Z' : 'z',
 		flags & CC_C_BIT ? 'C' : 'c', flags & CC_V_BIT ? 'V' : 'v');
-	printf ("  IRQs %s  FIQs %s  Mode %s%s\n",
+	nc_printf ("  IRQs %s  FIQs %s  Mode %s%s\n",
 		interrupts_enabled (regs) ? "on" : "off",
 		fast_interrupts_enabled (regs) ? "on" : "off",
 		processor_modes[processor_mode (regs)],
@@ -148,57 +172,59 @@
 
 void do_undefined_instruction (struct pt_regs *pt_regs)
 {
-	printf ("undefined instruction\n");
+	nc_printf ("undefined instruction\n");
 	show_regs (pt_regs);
 	bad_mode ();
 }
 
 void do_software_interrupt (struct pt_regs *pt_regs)
 {
-	printf ("software interrupt\n");
+	nc_printf ("software interrupt\n");
 	show_regs (pt_regs);
 	bad_mode ();
 }
 
 void do_prefetch_abort (struct pt_regs *pt_regs)
 {
-	printf ("prefetch abort\n");
+	nc_printf ("prefetch abort\n");
 	show_regs (pt_regs);
 	bad_mode ();
 }
 
 void do_data_abort (struct pt_regs *pt_regs)
 {
-	printf ("data abort\n\n    MAYBE you should read doc/README.arm-unaligned-accesses\n\n");
+	nc_printf ("data abort\n\n    MAYBE you should read doc/README.arm-unaligned-accesses\n\n");
 	show_regs (pt_regs);
 	bad_mode ();
 }
 
 void do_not_used (struct pt_regs *pt_regs)
 {
-	printf ("not used\n");
+	nc_printf ("not used\n");
 	show_regs (pt_regs);
 	bad_mode ();
 }
 
 void do_fiq (struct pt_regs *pt_regs)
 {
-	printf ("fast interrupt request\n");
+	nc_printf ("fast interrupt request\n");
 	show_regs (pt_regs);
 	bad_mode ();
 }
 
+#define CONFIG_USE_IRQ 1//BMC CPU Reset-SDK Update*/
 #ifndef CONFIG_USE_IRQ
 void do_irq (struct pt_regs *pt_regs)
 {
-	printf ("interrupt request\n");
+	nc_printf ("interrupt request\n");
 	show_regs (pt_regs);
 	bad_mode ();
+
 }
 #else
 extern void HandleIntr(void);
 void do_irq (struct pt_regs *pt_regs)
 {
-	HandleIntr();
+	gic_handle_irq();//BMC CPU Reset-SDK Update//HandleIntr();	
 }
 #endif
