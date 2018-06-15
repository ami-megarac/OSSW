--- u-boot-2013.07/arch/arm/cpu/armv7/start.S	2015-03-23 07:23:59.410458733 -0400
+++ mywork/arch/arm/cpu/armv7/start.S	2015-07-08 12:40:38.193040244 -0400
@@ -35,7 +35,7 @@
 #include <asm/system.h>
 #include <linux/linkage.h>
 
-.globl _start
+.global _start
 _start: b	reset
 	ldr	pc, _undefined_instruction
 	ldr	pc, _software_interrupt
@@ -78,8 +78,7 @@
  * setup stack
  *
  *************************************************************************/
-
-.globl _TEXT_BASE
+.global _TEXT_BASE
 _TEXT_BASE:
 #if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_TEXT_BASE)
 	.word	CONFIG_SPL_TEXT_BASE
@@ -93,6 +92,10 @@
 .globl _bss_start_ofs
 _bss_start_ofs:
 	.word __bss_start - _start
+/*BMC CPU Reset-SDK Update*/
+.global	_image_copy_end_ofs
+_image_copy_end_ofs:
+	.word 	__image_copy_end - _start
 
 .globl _bss_end_ofs
 _bss_end_ofs:
@@ -118,12 +121,37 @@
 .globl IRQ_STACK_START_IN
 IRQ_STACK_START_IN:
 	.word	0x0badc0de
+/*BMC CPU Reset-SDK Update*/
+.globl UBOOT_IRQ_VBAR
+UBOOT_IRQ_VBAR:
+	.word 0
 
 /*
  * the actual reset code
  */
 
 reset:
+/*BMC CPU Reset-SDK Update*/
+#if defined(PILOT4_ORION)
+
+	/* Enable SPI prefetch if it is not OEM-1 mode*/
+	ldr r0, =0x4010010C @; STRAP_STATUS
+	ldr r1, [r0]
+	ands r1, r1, #0x1
+	bne skip_spi_prefetch
+	ldr     r2,=0x10
+	ldr     r0,=0x40200010
+	ldr     r1,[r0]
+	orr     r1,r1,r2
+	str     r1,[r0]
+
+skip_spi_prefetch:
+	/* Put secondary core to sleep */
+	mrc	p15, 0, r0, c0, c0, 5		@; Read CPU 	ID register
+	ands	r0, r0, #0x03			@; Mask off, leaving the CPU ID field
+	blne	secondary_cpus_init_exp
+#endif
+
 	bl	save_boot_params
 	/*
 	 * disable interrupts (FIQ and IRQ), also set the cpu to SVC32 mode,
@@ -136,13 +164,14 @@
 	orrne	r0, r0, #0x13		@ set SVC mode
 	orr	r0, r0, #0xc0		@ disable FIQ and IRQ
 	msr	cpsr,r0
+	
 
 /*
  * Setup vector:
  * (OMAP4 spl TEXT_BASE is not 32 byte aligned.
  * Continue to use ROM code vector only in OMAP4 spl)
  */
-#if !(defined(CONFIG_OMAP44XX) && defined(CONFIG_SPL_BUILD))
+#if (!(defined(CONFIG_OMAP44XX) && defined(CONFIG_SPL_BUILD)) && !defined(PILOT4_ORION))
 	/* Set V=0 in CP15 SCTRL register - for VBAR to point to vector */
 	mrc	p15, 0, r0, c1, c0, 0	@ Read CP15 SCTRL Register
 	bic	r0, #CR_V		@ V = 0
@@ -155,15 +184,115 @@
 
 	/* the mask ROM code should have PLL and others stable */
 #ifndef CONFIG_SKIP_LOWLEVEL_INIT
+    adr r1, _start
+    ldr r0, =0x40100230     /*Storing _start Address of u-boot to appropriately calculate irq vectors*/
+    str r1, [r0]
+    
+    mrc     p15, 0, r0, c0, c0, 5           @; Read CPU     ID register
+    ands    r0, r0, #0x03                   @; Mask off, leaving the CPU ID field
+    ldrne sp,=0x1000eff0
+    ldreq sp,=0x1000dff0
+    bl disable_caches
+    bl invalidate_caches
+    
 	bl	cpu_init_cp15
 	bl	cpu_init_crit
 #endif
+/*BMC CPU Reset-SDK Update*/
+
 
+	bl start_pilot_timer3//BMC CPU Reset-SDK Update
 	bl	_main
 
 /*------------------------------------------------------------------------------*/
 
 ENTRY(c_runtime_cpu_setup)
+/*BMC CPU Reset-SDK Update*/
+	/* Set up the stack						    */
+stack_setup:
+	mov	sp, r4
+
+	adr	r0, _start
+	cmp	r0, r6
+	moveq	r9, #0		/* no relocation. relocation offset(r9) = 0 */
+	beq	clear_bss		/* skip relocation */
+	mov	r1, r6			/* r1 <- scratch for copy_loop */
+	ldr	r3, _image_copy_end_ofs
+	add	r2, r0, r3		/* r2 <- source end address	    */
+
+copy_loop:
+	ldmia	r0!, {r9-r10}		/* copy from source address [r0]    */
+	stmia	r1!, {r9-r10}		/* copy to   target address [r1]    */
+	cmp	r0, r2			/* until source end address [r2]    */
+	blo	copy_loop
+
+	/*
+	 * fix .rel.dyn relocations
+	 */
+	ldr	r0, _TEXT_BASE		/* r0 <- Text base */
+	sub	r9, r6, r0		/* r9 <- relocation offset */
+	ldr	r10, _dynsym_start_ofs	/* r10 <- sym table ofs */
+	add	r10, r10, r0		/* r10 <- sym table in FLASH */
+	ldr	r2, _rel_dyn_start_ofs	/* r2 <- rel dyn start ofs */
+	add	r2, r2, r0		/* r2 <- rel dyn start in FLASH */
+	ldr	r3, _rel_dyn_end_ofs	/* r3 <- rel dyn end ofs */
+	add	r3, r3, r0		/* r3 <- rel dyn end in FLASH */
+fixloop:
+	ldr	r0, [r2]		/* r0 <- location to fix up, IN FLASH! */
+	add	r0, r0, r9		/* r0 <- location to fix up in RAM */
+	ldr	r1, [r2, #4]
+	and	r7, r1, #0xff
+	cmp	r7, #23			/* relative fixup? */
+	beq	fixrel
+	cmp	r7, #2			/* absolute fixup? */
+	beq	fixabs
+	/* ignore unknown type of fixup */
+	b	fixnext
+fixabs:
+	/* absolute fix: set location to (offset) symbol value */
+	mov	r1, r1, LSR #4		/* r1 <- symbol index in .dynsym */
+	add	r1, r10, r1		/* r1 <- address of symbol in table */
+	ldr	r1, [r1, #4]		/* r1 <- symbol value */
+	add	r1, r1, r9		/* r1 <- relocated sym addr */
+	b	fixnext
+fixrel:
+	/* relative fix: increase location by offset */
+	ldr	r1, [r0]
+	add	r1, r1, r9
+fixnext:
+	str	r1, [r0]
+	add	r2, r2, #8		/* each rel.dyn entry is 8 bytes */
+	cmp	r2, r3
+	blo	fixloop
+	b	clear_bss
+_rel_dyn_start_ofs:
+	.word __rel_dyn_start - _start
+_rel_dyn_end_ofs:
+	.word __rel_dyn_end - _start
+_dynsym_start_ofs:
+	//.word __dynsym_start - _start
+
+clear_bss:
+	ldr	r0, _bss_start_ofs
+	ldr	r1, _bss_end_ofs
+	mov	r4, r6			/* reloc addr */
+	add	r0, r0, r4
+	add	r1, r1, r4
+	mov	r2, #0x00000000		/* clear			    */
+
+clbss_l:cmp	r0, r1			/* clear loop... */
+	bhs	clbss_e			/* if reached end of bss, exit */
+	str	r2, [r0]
+	add	r0, r0, #4
+	b	clbss_l
+clbss_e:
+
+/*
+ * We are done. Do not return, instead branch to second part of board
+ * initialization, now running from RAM.
+ */
+jump_2_ram:
+
 /*
  * If I-cache is enabled invalidate it
  */
@@ -177,9 +306,14 @@
  */
 	/* Set vector address in CP15 VBAR register */
 	ldr     r0, =_start
-	mcr     p15, 0, r0, c12, c0, 0  @Set VBAR
-
-	bx	lr
+     /*BMC CPU Reset-SDK Update*/
+	add     r0, r0, r9
+	@mcr     p15, 0, r0, c12, c0, 0  @Set VBAR
+	ldr r1,=UBOOT_IRQ_VBAR
+	add r1, r1, r9
+	str r0, [r1]
+	/* jump to it ... */
+	mov	pc, lr
 
 ENDPROC(c_runtime_cpu_setup)
 
@@ -469,3 +603,73 @@
 
 #endif /* CONFIG_USE_IRQ */
 #endif /* CONFIG_SPL_BUILD */
+
+
+  .global disable_caches
+  @; void disable_caches(void)
+disable_caches:
+  MRC     p15, 0, r0, c1, c0, 0   @; Read System Control Register configuration data
+  BIC     r0, r0, #(1 << 2)       @; Clear C bit
+  BIC     r0, r0, #(1 << 12)      @; Clear I bit
+  MCR     p15, 0, r0, c1, c0, 0   @; Write System Control Register configuration data
+  BX      lr
+
+  .global invalidate_caches
+  @; void invalidate_caches(void)@;
+invalidate_caches:
+  PUSH    {r4-r12}
+
+  @;
+  @; Based on code example given in section B2.2.4/11.2.4 of ARM DDI 0406B
+  @;
+
+  MOV     r0, #0
+  MCR     p15, 0, r0, c7, c5, 0     @; ICIALLU - Invalidate entire I Cache, and flushes branch target cache ref to arm v7 arch manual
+
+  MRC     p15, 1, r0, c0, c0, 1     @; Read CLIDR  cache level ID reg type of cache at each level, level of coherency pg 1814 armv7 arch ref manual
+  @ANDS    r3, r0, #7000000         @;   getting level of coherency bits[26:24]
+  ldr     r1, =0x7000000
+  ANDS    r3, r0,r1
+
+  MOV     r3, r3, LSR #23           @; Cache level value (naturally aligned)  level 1 cache alignment gives 0x2
+  BEQ     invalidate_caches_finished  @;  if r3=0 :i.e. if no cache?
+  MOV     r10, #0
+
+invalidate_caches_loop1:
+  ADD     r2, r10, r10, LSR #1      @; Work out 3xcachelevel  r2=0 initially
+  MOV     r1, r0, LSR r2            @; bottom 3 bits are the Cache type for this level
+  AND     r1, r1, #7                @; get those 3 bits alone :In r4p1 this value is 0x3
+  CMP     r1, #2
+  BLT     invalidate_caches_skip    @; no cache or only instruction cache at this level
+  MCR     p15, 2, r10, c0, c0, 0    @; write the Cache Size selection register  CSSELR pg 1832 armv7 arch ref manual level 1 data cache selected by 0x0
+  ISB                               @; ISB to sync the change to the CacheSizeID reg
+  MRC     p15, 1, r1, c0, c0, 0     @; reads current Cache Size ID register CCSIDR pg 1528 armv7 arch ref manual
+  AND     r2, r1, #7               @; extract the line length field  getting line size[2:0]
+  ADD     r2, r2, #4                @; add 4 for the line length offset (log2 16 bytes)  8 word cache line so r2=1 + 4  :This is for the format 2power of 5 = 32 register format Enjay!
+  LDR     r4, =0x3FF
+  ANDS    r4, r4, r1, LSR #3        @; R4 is the max number on the way size (right aligned)  reading associativity -1
+  CLZ     r5, r4                    @; R5 is the bit position of the way size increment  count leading zeroes ex: assoc 0x3 clz = 30
+  LDR     r7, =0x00007FFF
+  ANDS    r7, r7, r1, LSR #13       @; R7 is the max number of the index size (right aligned)  reading no of sets in the cache
+
+invalidate_caches_loop2:
+  MOV     r9, R4                    @; R9 working copy of the max way size (right aligned)
+
+invalidate_caches_loop3:
+  ORR     r11, r10, r9, LSL r5      @; factor in the way number and cache number into R11  r9 << 30 | r10 :B3.12.31 c7, Cache and branch predictor maintenance functions in armv7 spec
+  ORR     r11, r11, r7, LSL r2      @; factor in the index number  r7<<r2(=5)
+  MCR     p15, 0, r11, c7, c6, 2    @; DCISW - invalidate by set/way  refer to pg 1943 in arm v7 arch ref manual
+  SUBS    r9, r9, #1                @; decrement the way number
+  BGE     invalidate_caches_loop3
+  SUBS    r7, r7, #1                @; decrement the index  decrement the set number
+  BGE     invalidate_caches_loop2
+invalidate_caches_skip:
+  ADD     r10, r10, #2              @; increment the cache number  to cmp with cache level value in r3 :+ 2 because format of the register data for a set/way operation
+  CMP     r3, r10
+  BGT     invalidate_caches_loop1
+
+invalidate_caches_finished:
+  POP     {r4-r12}
+  BX      lr
+
+
