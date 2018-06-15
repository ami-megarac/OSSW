--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/pilot4_lowlevel_init.S 2015-03-23 07:28:07.722464981 -0400
+++ mywork/arch/arm/cpu/armv7/pilot4/pilot4_lowlevel_init.S 2015-07-10 04:07:06.172615988 -0400
@@ -0,0 +1,658 @@
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
+@.include  "buffer.equ"
+.include  "pilot.equ"
+.include "sysctrl.equ"
+@.include  "pilot2.equ"
+.include  "arm.equ"
+.include  "int.equ"
+.include  "ddr_cfg.equ"
+@.include  "psr.equ"
+
+#include <config.h>
+@;#include "include/check.h"
+
+#define DEBUG_PRINT
+#define UART_FIFO_OFFSET	0x00
+#define UART_SR_OFFSET		0x14
+#define UART_SR_TXFULL		0x00000010
+#define CORE1_HOVER_START	0x1000FF80
+#define CORE1_HOVER_SCRATCH 0x1000FF78
+
+#define V_BIT	(1<<13)
+/*BMC CPU Reset-SDK Update*/
+#define S_FRAME_SIZE    72
+
+#define S_OLD_R0        68
+#define S_PSR           64
+#define S_PC            60
+#define S_LR            56
+#define S_SP            52
+
+#define S_IP            48
+#define S_FP            44
+#define S_R10           40
+#define S_R9            36
+#define S_R8            32
+#define S_R7            28
+#define S_R6            24
+#define S_R5            20
+#define S_R4            16
+#define S_R3            12
+#define S_R2            8
+#define S_R1            4
+#define S_R0            0
+
+.macro  irq_save_user_regs
+    sub     sp, sp, #S_FRAME_SIZE
+    stmia   sp, {r0 - r12}                  @ Calling r0-r12
+    add     r8, sp, #S_PC                   @ !! R8 NEEDS to be saved !!
+    @ a reserved stack spot would
+    @ be good.
+    stmdb   r8, {sp, lr}^                   @ Calling SP, LR
+    str     lr, [r8, #0]                    @ Save calling PC
+    mrs     r6, spsr
+    str     r6, [r8, #4]                    @ Save CPSR
+    str     r0, [r8, #8]                    @ Save OLD_R0
+    mov     r0, sp
+.endm
+
+.macro  irq_restore_user_regs
+    ldmia   sp, {r0 - lr}^                  @ Calling r0 - lr
+    mov     r0, r0
+    ldr     lr, [sp, #S_PC]                 @ Get PC
+    add     sp, sp, #S_FRAME_SIZE
+    subs    pc, lr, #4                      @ return & move spsr_svc into
+    @ cpsr
+.endm
+
+
+LOADREMAP:
+	ldr   pc, REMAPD_reset 
+	ldr   pc, REMAPD_Undef 
+	ldr   pc, REMAPD_swi 
+	ldr   pc, REMAPD_pref 
+	ldr   pc, REMAPD_dataabrt
+	ldr   pc, REMAPD_adrexp
+	ldr   pc, REMAPD_irq 
+	ldr	  pc, REMAPD_fiq
+
+
+stbyint_sts         = (1<<2)
+stbyint_en          = (1<<3)
+
+remap_vectors:
+	ldr   pc, _reset_local
+	ldr   pc, _undef_local
+	ldr   pc, _swi_local
+	ldr   pc, _pref_abtr_local
+	ldr   pc, _data_abtr_local
+	ldr   pc, _adr_exp_local
+	ldr   pc, _IRQHandler_LOCAL
+
+_reset_local:
+	.word (REMAPD_reset- CONFIG_SYS_TEXT_BASE)
+
+_swi_local:
+	.word (REMAPD_swi - CONFIG_SYS_TEXT_BASE)
+
+_undef_local:
+	.word (REMAPD_Undef - CONFIG_SYS_TEXT_BASE)
+
+_pref_abtr_local:
+	.word (REMAPD_pref - CONFIG_SYS_TEXT_BASE)
+
+_data_abtr_local:
+	.word (REMAPD_dataabrt - CONFIG_SYS_TEXT_BASE)
+
+_adr_exp_local:
+	.word (REMAPD_adrexp - CONFIG_SYS_TEXT_BASE)
+
+_IRQHandler_LOCAL:
+	.word (REMAPD_irq - CONFIG_SYS_TEXT_BASE)
+
+remap_vectors_offset:
+	.word (remap_vectors - CONFIG_SYS_TEXT_BASE)
+
+REMAPD_reset:
+	ldr r0, =0x40430000 
+	ldr r1, ='R'
+	str r1, [r0]
+	b .
+
+REMAPD_Undef:
+	ldr r0, =0x40430000 
+	ldr r1, ='U'
+	str r1, [r0]
+	b .
+REMAPD_swi:
+	ldr r0, =0x40430000 
+	ldr r1, ='S'
+	str r1, [r0]
+	b .
+REMAPD_pref:
+	ldr r0, =0x40430000 
+	ldr r1, ='P'
+	str r1, [r0]
+	b .
+REMAPD_dataabrt:
+	ldr r0, =0x40430000 
+	ldr r1, ='A'
+	str r1, [r0]
+	b .
+REMAPD_adrexp:
+	ldr r0, =0x40430000 
+	ldr r1, ='E'
+	str r1, [r0]
+	b .
+REMAPD_fiq:
+	ldr r0, =0x40430000 
+	ldr r1, ='F'
+	str r1, [r0]
+	b .
+REMAPD_irq:
+/*BMC CPU Reset-SDK Update*/
+    ldr sp, =0x1000dff0
+    irq_save_user_regs
+    bl gic_handle_irq
+    irq_restore_user_regs
+	b .
+
+.macro  senduart,rd,rx
+	str \rd, [\rx, #UART_FIFO_OFFSET]	@ TXDATA
+.endm
+
+.macro  waituart,rd,rx
+.endm
+
+.macro  busyuart,rd,rx
+1002:	ldr \rd, [\rx, #UART_SR_OFFSET] @ get
+		tst \rd, #UART_SR_TXFULL		@
+		bne 1002b						@ wait if FIFO
+.endm
+
+.macro printch, ch
+		ldr r0, =0x40430000
+		ldr r1, =0
+		busyuart r1, r0 
+		ldr r0, =0x40430000
+		ldr r1, =\ch
+		senduart r1, r0 
+.endm
+
+.global pilot4_lowlevel_init 
+.global Enable_standbyfi_intr
+.global Wait_For_PLLock
+.global Switch_Clk_Pll
+
+TMP_STACK_PTR	=	LMEMSTART + (0xeFF0) @; Last 200 bytes of lmem is poison dont touch!!
+Power_On_Reset	=	1<<1         
+
+#**************************************
+#              Low level init         #
+#**************************************
+.global pilot4_lowlevel_init
+pilot4_lowlevel_init:
+  	stmfd    r13!,{r0-r12,r14}
+    ldr r11, =0x40100240
+    str r13, [r11]
+    
+	mrc	p15, 0, r0, c0, c0, 5		@; Read CPU 	ID register
+	ands	r0, r0, #0x03			@; Mask off, leaving the CPU ID field
+	blne	secondary_cpus_init_exp
+@;Initialize the debug UART on P4 FPGA
+	ldr r0, =0x40430008
+	mov r1, #0x7
+	str r1, [r0]
+	ldr r0, =0x4043000c
+	mov r1, #0x83
+	str r1, [r0]
+	ldr r0, =0x40430000
+	mov r1, #0x1
+	str r1, [r0]
+	ldr r0, =0x4043000c
+	mov r1, #0x3
+	str r1, [r0]
+#ifndef CONFIG_USE_SSP_RESET
+@clear all 3 WD interrupt status and start WDT2
+	ldr r0, =0x40100880
+	ldr r1, [r0]
+	orr r1, r1, #0x40000
+	str r1, [r0]
+	ldr r0, =0x40100890
+	ldr r1, [r0]
+	orr r1, r1, #0x40000
+	str r1, [r0]
+	ldr r0, =0x401008a0
+	ldr r1, [r0]
+	orr r1, r1, #0x40000
+	str r1, [r0]
+
+	ldr r0, =0x40100890
+	ldr r1, [r0]
+	bic r1, r1, #0x1
+	str r1, [r0]
+	ldr r0, =0x4010089C
+	ldr r1, =0x1DC4FFFF
+	str r1, [r0]
+	ldr r0, =0x40100894
+	ldr r1, =0x3
+	str r1, [r0]
+	ldr r0, =0x40100898
+	ldr r1, =0x0
+	str r1, [r0]
+	ldr r0, =0x40100890
+	ldr r1, [r0]
+	orr r1, r1, #0x800000
+	str r1, [r0]
+	ldr r0, =0x40100890
+	ldr r1, [r0]
+	orr r1, r1, #0x400000
+	orr r1, r1, #0x000C00
+	orr r1, r1, #0x000001
+	str r1, [r0]
+#endif
+
+@provide a new stack pointer for gic Init
+	ldr	r13,=TMP_STACK_PTR
+	bl gic_init//BMC CPU Reset-SDK Update
+
+	@@@@load handlers at remap address
+	ldr     r0,remap_vectors_offset
+	ldr     r1,=0x40100230
+	ldr	r10,[r1]
+	add	r0, r0, r10
+	ldr     r1,=0x10004000//BMC CPU Reset-SDK Update
+	
+	ldmia   r0!,{r2-r9}
+	stmia   r1!,{r2-r9}
+
+	ldmia   r0!,{r2-r9}
+	add r2, r2, r10
+	add r3, r3, r10
+	add r4, r4, r10
+	add r5, r5, r10
+	add r6, r6, r10
+	add r7, r7, r10
+	add r8, r8, r10
+	add r9, r9, r10
+	stmia   r1!,{r2-r9}
+	/* Set V=0 in CP15 SCTRL register - for VBAR to point to vector */
+	mrc	p15, 0, r0, c1, c0, 0	@ Read CP15 SCTRL Register
+	bic	r0, #V_BIT @ V = 0
+	mcr	p15, 0, r0, c1, c0, 0	@ Write CP15 SCTRL Register
+
+	/* Set vector address in CP15 VBAR register */
+	ldr	r0, =0x10004000 //BMC CPU Reset-SDK Update
+	mcr	p15, 0, r0, c12, c0, 0	@Set VBAR
+ /*BMC CPU Reset-SDK Update*/
+	//bl start_pilot_timer3
+	mov r0, #0x5B
+	bl gic_unmask_irq
+	cpsie i @; Enable interrupts
+@Pass desired DDR2 Initialization Table Here
+@Skip ddrinit if the code is already executing from DDR
+checkddr:
+	adr r0, checkddr 
+	ldr r1, =0x80000000
+	and r0, r0,r1
+	cmp r0, r1
+	beq noddrinit
+	ldr	r0,=0x401008B0
+	ldr	r1,[r0]
+	movw r2, #0x210
+	TST r1,r2
+	beq	noddrinit
+@;
+@; GIC Init
+@; ---------
+	mov r0, #0x24
+	bl gic_unmask_irq
+	cpsie i @; Enable interrupts
+
+@;defaulting to 4-byte address mode when the bit is set
+	ldr r0, =0x4010010c
+	ldr r1, =0x1
+	ldr r2, [r0]
+	and r2, r2, r1
+	cmp r2, r1
+	bne jumpout
+	ldr r3, =0x0
+	ldr r2, [r0]
+	ldr r1, =0xC00
+	and r2, r2, r1
+	cmp r3, r2
+	beq jumpout
+	ldr r4, =0x4042901C 
+	ldr r0, =0x1000000
+	ldr r1, [r4]
+	orr r1, r1, r0
+	str r1, [r4]
+	ldr r4, =0x40429008 
+	ldr r0, =0x2000000
+	ldr r1, [r4]
+	orr r1, r1, r0
+	str r1, [r4]
+jumpout:
+	ldr r0, =0x40100150
+	ldr r1, [r0]
+	and r1,r1,#0xF
+	cmp r1,#1
+	bls pre_a2_init
+    @; Add any A2 espi related code here
+	ldr r0, =0x40100190
+	ldr r1, [r0]
+	orr r1, r1, #0x10000000
+	str r1, [r0]
+    @; A2 espi init code ends here
+pre_a2_init:
+	@;espi init - program capabilities register
+	ldr r0, =0x40423008
+	ldr r1, =0x0302000F
+	str r1, [r0]
+
+	ldr r0, =0x40100D18
+	ldr r1, =0x0000000F
+	str r1, [r0]
+
+	@;;programming needed in orion initialization
+
+	ldr r0,=0x40100154		@;Enable the VGA OPROM for Pilot4         
+	ldr r1,=0x00000600
+	str r1, [r0]
+
+	ldr r0,=0x401009A4		@Disable MINI_SIO which was enabled by default
+	mov r1, #0x80
+	str r1, [r0]
+
+	ldr r0,=0x401009A8		@Disable MINI_SIO which was enabled by default
+	mov r1, #0x80
+	str r1, [r0]
+	
+	ldr r0,=0x401009AC		@Disable MINI_SIO which was enabled by default
+	mov r1, #0x80
+	str r1, [r0]
+
+	ldr r0, =0x40100130		@set bit 8 for the address aliasing issue 
+    ldr r1, [r0]			@while accessing H2B PCIE device
+	orr r1, r1, #0x180		@Set bit 7 To Disable prefetch for graphics
+	str r1, [r0]
+	
+	ldr r0, =0x40100198		@Set bit 0 To enable G200 as device 0
+	ldr r1, [r0] 
+	orr r1, r1, #1
+	str r1, [r0]
+
+  ldr r0,=0x4044F214		@; Enter Non D0
+	ldr r1, [r0] 
+	ldr r2,=0x00000010
+	orr r1, r1, r2
+	str r1, [r0]
+
+	ldr r0, =0x40100150
+	ldr r1, [r0]
+	and r1, r1, #0xF
+	ldr r2, =0x1
+	cmp r1,r2
+	bhi next_cap_ptr
+	ldr     r0,=0x4044F340  @Overwrtite Next CAP PTR to fix video NOT coming up on some boards
+@;;	ldr     r1,=0x06030001
+	ldr     r1,=0x00030001
+	str     r1, [r0]
+	b jumpcap
+next_cap_ptr:
+	ldr r0, =0x4044F340
+	ldr r1, =0x00035001
+	str r1, [r0]
+jumpcap:
+	ldr r0, =0x40100160		@Set bit 4 to enable USB 2.0 Interrupt to ARM
+	ldr r1, [r0] 
+	orr r1, r1, #0x10
+	str r1, [r0]
+
+	ldr r0, =0x40100820		@Enable bus-hang release
+	ldr r1, [r0] 
+	orr r1, r1, #0x0F
+	str r1, [r0]
+
+	bl	Enable_standbyfi_intr
+
+
+	ldr     r0,=0x40100108             @; SYSCONTROL Bit 31, 15. Shutoff DDR clock.
+	ldr     r1,[r0]
+	ldr     r2,=0x80008000
+	orr     r2,r2,r1
+	str     r2,[r0]	
+	ldr     r0,=0x40100110             @; DDRPLLCTRL with correct divider value.
+
+#ifdef DDR_1600MHZ
+	ldr r1,=0x0027fd68
+#endif
+#ifdef DDR_1333MHZ
+	ldr r1,=0x0027cd68
+#endif
+#ifdef DDR_1066MHZ
+	ldr r1,=0x0027fc27
+#endif
+#ifdef DDR_800MHZ
+	ldr r1,=0x0027fc1f
+#endif
+#ifdef DDR_500MHZ
+	ldr r1,=0x0028fc27
+#endif
+#ifdef DDR_350_400MHZ
+	ldr r1,=0x0028fc1f
+#endif
+#ifdef DDR_200_250MHZ
+	ldr r1,=0x0029fc1f
+#endif
+#ifdef DDR_75_125MHZ
+	ldr r1,=0x002bfc1f
+#endif
+	str r1, [r0]
+
+#ifdef DDR_1600MHZ
+	ldr	r1,=0x0007fd68
+#endif
+#ifdef DDR_1333MHZ
+	ldr	r1,=0x0007cd68
+#endif
+#ifdef DDR_1066MHZ
+	ldr r1,=0x0007fc27
+#endif
+#ifdef DDR_800MHZ
+	ldr r1,=0x0007fc1f
+#endif
+#ifdef DDR_500MHZ
+	ldr r1,=0x0008fc27
+#endif
+#ifdef DDR_350_400MHZ
+	ldr r1,=0x0008fc1f
+#endif
+#ifdef DDR_200_250MHZ
+	ldr r1,=0x0009fc1f
+#endif
+#ifdef DDR_75_125MHZ
+	ldr r1,=0x000bfc1f
+#endif
+	str r1, [r0]
+		
+	bl	Wait_For_PLLock
+	bl	Switch_Clk_Pll
+	
+@Do clk switch for mac0 and mac1. This is required for both FPGA and ASIC
+	ldr	r0,=0x40100108
+	ldr	r1,[r0]
+	ldr	r2,=0x1E00000
+	orr	r2,r2,r1
+	str	r2,[r0]
+
+ /*BMC CPU Reset-SDK Update*/
+@; Here wait for core1 to enter WFE mode
+wait_for_wfe:
+        ldr r0, =0x40100D00 @; The bit number 27 of this register tells if core1 is in WFE
+        ldr r1, =0x04000000
+        ldr r2, [r0]
+        and r1, r1, r2
+        cmp r1, #0x0
+        beq wait_for_wfe
+
+	ldr r0, =0x4010010C @; STRAP_STATUS
+	ldr r1, [r0]
+	ands r1, r1, #0x1
+	beq NOT_OEM1
+@in OEM1 mode, watchdog will keep on triggering and resets PILOT. Hence disabling watchdog 
+@for firmware developement. This needs to be enabled later in production firmware.
+	ldr r0, =0x401008A0	
+	ldr r1, =0x10000
+	str r1, [r0]
+	
+	@Setting MAC0 and MAC1 to RGMII mode for OEM1 mode
+	ldr r0, =0x40100988	
+	ldr r1, [r0]
+	ldr r2, =0x180
+	orr r1, r1, r2
+	str r1, [r0]
+
+	printch 'O'; printch 'E'; printch 'M'; printch '1';
+	printch 'M'; printch 'O'; printch 'D'; printch 'E';  printch '-';
+NOT_OEM1:
+	ldr r0, =0x401008B4
+	ldr r1, =0
+	str r1, [r0]
+@;Write PILOT4 on debug UART
+	printch 'P'
+	printch 'I'
+	printch 'L'
+	printch 'O'
+	printch 'T'
+	printch '4'
+@	printch '\n'
+
+	ldr r0,=0x40100100
+	ldr r1, [r0]
+	bic r1,r1,#0x8
+	str r1,[r0]	//SYSCFG[3]
+
+	mov	r0, #0x24					@; ID
+	bl	disable_irq_id				@; MP_GIC.s
+#if 0 //BMC CPU Reset-SDK Update
+    @ Setting the VBAR is done in gic_init
+	ldr	r0, =0x0
+	mcr	p15, 0, r0, c12, c0, 0	@Set VBAR
+#endif
+
+	bl ddrinit
+
+	ldr r0,=0x40100100
+    ldr r1, [r0]
+    bic r1,r1,#0x2
+	str r1,[r0]               @ remap
+	b no_sw_reset
+
+noddrinit:
+	bl  sw_reset_modules
+no_sw_reset:
+	
+    ldr r11, =0x40100240
+    ldr r13, [r11]  
+	ldmfd r13!, {r0-r12, r15}
+	
+.global secondary_cpus_init_exp
+secondary_cpus_init_exp:
+.ifndef  NEEDED 
+   @; Copy the SMP aware code to teh scratch pad area
+   LDR r0, =dat
+   LDR r2, =CONFIG_SYS_TEXT_BASE
+   SUBS r0, r0, r2 
+   LDR r1, =CORE1_HOVER_START
+   LDMIA	r0!,{r2-r9}
+   STMIA	r1!,{r2-r9}
+	
+   LDMIA	r0!,{r2-r9}
+   STMIA	r1!,{r2-r9}
+
+   LDR	r2, [r0]
+   STR	r2, [r1]
+
+   @; Jump to the scratch pad area
+   LDR r1, =CORE1_HOVER_SCRATCH
+   LDR r2, =CORE1_HOVER_START
+   STR r2, [r1]
+   LDR pc, [r1]
+   b .
+   .endif
+
+Enable_standbyfi_intr:	
+	stmfd r13!,{r0-r2,r14}	
+	ldr	r0,=SYSCONTROL
+
+@clear any of previous standby interrupt status	
+	ldr	r2,=stbyint_sts
+	
+03:		
+	ldr	r1,[r0]	
+	orr	r1,r1,r2
+	str	r1,[r0]
+
+	ldr	r1,[r0]
+	tst     r1,#stbyint_sts
+	bne	03b
+
+@enable stby intr	
+	ldr	r2,=stbyint_en
+	ldr	r1,[r0]
+	orr	r1,r1,r2
+	str	r1,[r0]
+
+	ldmfd	r13!,{r0-r2,r15}
+
+
+@Waits till PLL is stable	
+Wait_For_PLLock:	
+	stmfd  r13!,{r0-r1,r14}
+	ldr     r0,=SCLKSTS
+	ldr     r2,=PLLLOCK
+01:
+	ldr     r1,[r0]
+	and     r1,r1,r2
+	cmp     r1,r2
+	bne     01b
+	ldmfd  r13!,{r0-r1,r15}
+
+
+@make sure that Enable_standbyfi_intr is called before this subroutine and appropriate handler is available
+@This subroutine switches mux to PLL(500/400Mhz) mode
+Switch_Clk_Pll:	
+	stmfd  r13!,{r0-r1,r14}			
+
+	ldr r1,=SCLKCTL
+	ldr r2,=0xafffffff
+	str	r2,[r1]
+	
+  	WFI				@added for cortex A9 by Vikram - Wait for Interrupt
+	
+	ldmfd  r13!,{r0-r1,r15}
+
+
+
+	.ltorg
+	.end
