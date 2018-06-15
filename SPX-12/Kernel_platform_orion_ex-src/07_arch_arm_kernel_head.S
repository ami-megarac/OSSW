--- linux-3.14.17/arch/arm/kernel/head.S	2015-03-07 04:14:34.000000000 +0530
+++ linux-3.14.17.orion/arch/arm/kernel/head.S	2015-07-15 16:21:30.435601239 +0530
@@ -27,6 +27,9 @@
 #include CONFIG_DEBUG_LL_INCLUDE
 #endif
 
+#define PHYS_IO 0x40000000
+#define PHYS_IO_END  0x41000000
+
 /*
  * swapper_pg_dir is the virtual address of the initial page table.
  * We place the page tables 16K below KERNEL_RAM_VADDR.  Therefore, we must
@@ -204,6 +207,24 @@
 #endif
 #endif
 
+    ldr r7,=0x0c12 @ io_mmuflags
+
+    mov r6, #(PHYS_IO)>>20           @ start of IO section
+    orr r3, r7, r6, lsl #20     @ flags
+    str r3, [r4, r6, lsl #2]        @ identity
+
+    add r0, r4,  #(PHYS_IO & 0xff000000) >> 18
+    str r3, [r0, #(PHYS_IO & 0x00f00000) >> 18]!
+    ldr r6, =(PHYS_IO_END - 1)
+    add r0, r0, #4
+    add r6, r4, r6, lsr #18
+    1:  cmp r0, r6
+    add r3, r3, #1 << 20
+    strls   r3, [r0], #4
+    bls 1b
+
+
+
 	ldr	r7, [r10, #PROCINFO_MM_MMUFLAGS] @ mm_mmuflags
 
 	/*
@@ -347,6 +368,7 @@
 #if defined(CONFIG_SMP)
 	.text
 ENTRY(secondary_startup)
+  bl v7_invalidate_l1
 	/*
 	 * Common entry point for secondary CPUs.
 	 *
@@ -357,6 +379,56 @@
 
  ARM_BE8(setend	be)				@ ensure we are in BE8 mode
 
+#if defined(EXTENDED)
+  MOV     r0, #0
+  MCR     p15, 0, r0, c7, c5, 0     @; ICIALLU - Invalidate entire I Cache, and flushes branch target cache samar ref to arm v7 arch manual
+
+  MRC     p15, 1, r0, c0, c0, 1     @; Read CLIDR samar cache level ID reg type of cache at each level, level of coherency pg 1814 armv7 arch ref manual                                                     
+  @ANDS    r3, r0, #7000000         @;  samar getting level of coherency bits[26:24]                                                                                     
+  ldr     r1, =0x7000000
+  ANDS    r3, r0,r1
+
+  MOV     r3, r3, LSR #23           @; Cache level value (naturally aligned) samar level 1 cache alignment gives 0x2                                             
+  BEQ     invalidate_caches_finished  @; samar if r3=0 Akshay:i.e. if no cache?
+  MOV     r10, #0
+
+invalidate_caches_loop1:
+  ADD     r2, r10, r10, LSR #1      @; Work out 3xcachelevel samar r2=0 initially                                                  
+  MOV     r1, r0, LSR r2            @; bottom 3 bits are the Cache type for this level
+  AND     r1, r1, #7                @; get those 3 bits alone Akshay:In r4p1 this value is 0x3
+  CMP     r1, #2
+  BLT     invalidate_caches_skip    @; no cache or only instruction cache at this level
+  MCR     p15, 2, r10, c0, c0, 0    @; write the Cache Size selection register samar CSSELR pg 1832 armv7 arch ref manual level 1 data cache selected by 0x0                                                                                    
+  ISB                               @; ISB to sync the change to the CacheSizeID reg
+  MRC     p15, 1, r1, c0, c0, 0     @; reads current Cache Size ID register CCSIDR pg 1528 armv7 arch ref manual                                                                                     
+  AND     r2, r1, #7               @; extract the line length field samar getting line size[2:0]                                                                                         
+  ADD     r2, r2, #4                @; add 4 for the line length offset (log2 16 bytes) samar 8 word cache line so r2=1 + 4  Akshay:This is for the format 2power of 5 = 32 register format Enjay!
+  LDR     r4, =0x3FF
+  ANDS    r4, r4, r1, LSR #3        @; R4 is the max number on the way size (right aligned) samar reading associativity -1
+  CLZ     r5, r4                    @; R5 is the bit position of the way size increment samar count leading zeroes ex: assoc 0x3 clz = 30
+  LDR     r7, =0x00007FFF
+  ANDS    r7, r7, r1, LSR #13       @; R7 is the max number of the index size (right aligned) samar reading no of sets in the cache
+
+invalidate_caches_loop2:
+  MOV     r9, R4                    @; R9 working copy of the max way size (right aligned)
+
+invalidate_caches_loop3:
+ ORR     r11, r10, r9, LSL r5      @; factor in the way number and cache number into R11 samar r9 << 30 | r10 Akshay:B3.12.31 c7, Cache and branch predictor maintenance functions in armv7 spec
+  ORR     r11, r11, r7, LSL r2      @; factor in the index number samar r7<<r2(=5)                                                                       
+  MCR     p15, 0, r11, c7, c6, 2    @; DCISW - invalidate by set/way samar refer to pg 1943 in arm v7 arch ref manual                                                                                           
+  SUBS    r9, r9, #1                @; decrement the way number
+  BGE     invalidate_caches_loop3
+  SUBS    r7, r7, #1                @; decrement the index samar decrement the set number                                                          
+  BGE     invalidate_caches_loop2
+
+invalidate_caches_skip:
+  ADD     r10, r10, #2              @; increment the cache number samar to cmp with cache level value in r3 Akshay:+ 2 because format of the register data for a set/way operation
+  CMP     r3, r10
+  BGT     invalidate_caches_loop1
+
+invalidate_caches_finished:
+#endif
+
 #ifdef CONFIG_ARM_VIRT_EXT
 	bl	__hyp_stub_install_secondary
 #endif
