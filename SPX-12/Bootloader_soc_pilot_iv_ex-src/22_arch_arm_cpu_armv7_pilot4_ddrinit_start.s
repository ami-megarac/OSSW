--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/ddrinit/start.s	1969-12-31 19:00:00.000000000 -0500
+++ mywork/arch/arm/cpu/armv7/pilot4/ddrinit/start.s	2015-02-06 05:06:37.000000000 -0500
@@ -0,0 +1,250 @@
+@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
+@
+@ 
+@ Copyright (C) 2004-2014 Emulex. All rights reserved.
+@ EMULEX is a trademark of Emulex.
+@ www.emulex.com
+@ 
+@ This program is free software; you can redistribute it and/or modify it under
+@ the terms of version 2 of the GNU General Public License as published by the
+@ Free Software Foundation.
+@ This program is distributed in the hope that it will be useful. ALL EXPRESS
+@ OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY IMPLIED
+@ WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
+@ NON-INFRINGEMENT, ARE DISCLAIMED, EXCEPT TO THE EXTENT THAT SUCH DISCLAIMERS
+@ ARE HELD TO BE LEGALLY INVALID. See the GNU General Public License for more
+@ details, a copy of which can be found in the file COPYING included
+@ with this package.
+@
+@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
+
+.section "startup"
+.globl _start
+_start:
+	b	reset
+		B	irq
+		B	irq
+		B	irq
+		B	irq
+		B	irq
+		B	irq
+
+LOADFGB:
+		ldr	pc,ROMResetFGB
+		ldr	pc,UndefHandlerFGB
+	        ldr	pc,SWIHandlerFGB
+		ldr	pc,PrefAbortHandlerFGB
+		ldr	pc,DataAbortHandlerFGB
+		ldr	pc,AddrExceptHandlerFGB
+		ldr	pc,IRQHandlerFGB
+		ldr	pc,FIQHandlerFGB
+
+ROMResetFGB: 		.word    	0x10000000 + 0x00000000  @ 0x04000000
+UndefHandlerFGB:    	.word    	0x10000000 + 0x00000004  @ 0x04000004
+SWIHandlerFGB: 		.word    	0x10000000 + 0x00000008  @ 0x04000008
+PrefAbortHandlerFGB:  	.word    	0x10000000 + 0x0000000c  @ 0x0400000c
+DataAbortHandlerFGB:  	.word    	0x10000000 + 0x00000010  @ 0x04000010
+AddrExceptHandlerFGB: 	.word    	0x10000000 + 0x00000014  @ 0x04000014
+IRQHandlerFGB: 		.word    	0x10000000 + 0x00000018  @ 0x04000018
+FIQHandlerFGB: 		.word    	0x10000000 + 0x0000001c  @ 0x0400001c
+LOADFGBEND:
+
+
+@ PLL Control and Status register bits
+SYSCONTROL 		= 0x40100100
+SYS_PLLLOCK             = (1<<14)
+DDR_PLLLOCK             = (1<<29)
+PLL_STDBY               = (1<<21)
+PLLLOCK                 = (SYS_PLLLOCK + DDR_PLLLOCK)
+stbyint_sts             = (1<<2)
+stbyint_en              = (1<<3)
+SCLKSTS                 = (SYSCONTROL + 0x4)
+SCLKCTL                 = (SYSCONTROL + 0x8)
+PLLSTS                  = (SYSCONTROL + 0x10) @ do not exist
+DDR_PLLCTL              = (SYSCONTROL + 0x10)
+PLLCTL                  = (SYSCONTROL + 0x14)
+.globl _bss_start
+_bss_start:
+	.word __bss_start
+
+.globl _bss_end
+_bss_end:
+	.word __bss_end
+
+reset:
+	ldr r1,=stack_space
+	stmia	r1, {r0-r14}
+	ldr	r0, _bss_start		/* find start of bss segment        */
+	ldr	r1, _bss_end		/* stop here                        */
+	mov	r2, #0x00000000		/* clear                            */
+
+clbss_l:str	r2, [r0]		/* clear loop...                    */
+	add	r0, r0, #4
+	cmp	r0, r1
+	ble	clbss_l
+	ldr sp,=0x10008000
+
+.ifdef DDR3_1600MHZ
+	@1600MHz.
+	ldr      r1,=0x0027fd68
+.endif
+.ifdef DDR3_1333MHZ
+	@1333MHz.
+	ldr     r1,=0x0027cd68
+.endif
+.ifdef DDR3_1066MHZ
+	@1066MHz.             
+	ldr     r1,=0x0027fc27
+.endif
+.ifdef DDR3_800MHZ
+	@800MHz.
+	ldr     r1,=0x0027fc1f
+.endif
+.ifdef DDR3_500MHZ
+	@500MHz.
+	ldr     r1,=0x0028fc27
+.endif
+.ifdef DDR3_350_400MHZ
+	@400
+	ldr     r1,=0x0028fc1f
+.endif
+.ifdef DDR3_200_250MHZ
+	@200
+	ldr     r1,=0x0029fc1f
+.endif
+.ifdef DDR3_75_125MHZ
+	@100
+	ldr     r1,=0x002bfc1f
+.endif
+	ldr		r3, =0x40100110
+	str     r1, [r0]
+	
+.ifdef DDR3_1600MHZ
+	@1600MHz.
+	ldr      r1,=0x0007fd68
+.endif
+
+.ifdef DDR3_1333MHZ
+	@1333MHz.
+	ldr     r1,=0x0007cd68
+.endif
+.ifdef DDR3_1066MHZ
+	@1066MHz.
+	ldr     r1,=0x0007fc27
+.endif
+.ifdef DDR3_800MHZ
+	@800MHz.
+	ldr     r1,=0x0007fc1f
+.endif
+.ifdef DDR3_500MHZ
+	@500MHz.
+	ldr     r1,=0x0008fc27
+.endif
+.ifdef DDR3_350_400MHZ
+	@400
+	ldr     r1,=0x0008fc1f
+.endif
+.ifdef DDR3_200_250MHZ
+	@200
+	ldr     r1,=0x0009fc1f
+.endif
+.ifdef DDR3_75_125MHZ
+	@100
+	ldr     r1,=0x000bfc1f
+.endif
+
+	ldr		r3, =0x40100110
+	str     r1, [r3]
+
+.ifdef DDR4
+        mov     r0, #0x1
+.else
+        mov     r0, #0x0
+.endif
+	bl ddrinit
+	ldr r0, =0x80800000
+	ldr r1, =0xabbaabba
+	str r1, [r0]
+	ldr r2, [r0]
+	cmp r1, r2
+	beq here
+
+        ldr     r1, =0x40430000
+        str     r0, [r1]
+        ldr     r0, ='I'
+        ldr     r1, =0x40430000
+        str     r0, [r1]
+        ldr     r0, ='N'
+        ldr     r1, =0x40430000
+        str     r0, [r1]
+        ldr     r0, ='I'
+        ldr     r1, =0x40430000
+        str     r0, [r1]
+        ldr     r0, ='T'
+        ldr     r1, =0x40430000
+        str     r0, [r1]
+        ldr     r0, ='F'
+        ldr     r1, =0x40430000
+        str     r0, [r1]
+        ldr     r0, ='A'
+        ldr     r1, =0x40430000
+        str     r0, [r1]
+        ldr     r0, ='I'
+        ldr     r1, =0x40430000
+        str     r0, [r1]
+        ldr     r0, ='L'
+        ldr     r1, =0x40430000
+        str     r0, [r1]
+here:
+    ldr r0,=0x40100100
+	mov r1,#0
+    str r1,[r0]               @ remap
+	ldr r1,=stack_space
+	ldmia	r1, {r0-r13,r14}
+	bx r14
+
+	.align  5
+	undefined_instruction:
+	mov r0, #1
+	1:  b 1b
+	.align  5
+	software_interrupt:
+	mov r0, #2
+	1:  b 1b
+
+	.align  5
+	prefetch_abort:
+	mov r0, #3
+	1:  b 1b
+
+	.align  5
+	data_abort:
+	mov r0, #4
+	1:  b 1b
+
+	.align  5
+	not_used:
+	mov r0, #5
+	1:  b 1b
+
+	.align  5
+	irq:
+        ldr     r2,=stbyint_sts
+        ldr     r0,=SYSCONTROL
+        ldr     r1,[r0]
+        orr     r1,r1,r2
+        str     r1,[r0]
+	subs    r15, r14, #4
+	mov r0, #6
+	1:  b 1b
+
+	.align  5
+	fiq:
+	mov r0, #7
+	1:  b 1b
+
+
+	.data
+	.align
+	stack_space:
+	.space	64
