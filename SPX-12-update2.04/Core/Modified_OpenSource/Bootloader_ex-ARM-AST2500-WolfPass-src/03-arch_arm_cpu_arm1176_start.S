--- uboot/arch/arm/cpu/arm1176/start.S	2019-06-12 14:55:46.328924389 +0800
+++ uboot.new/arch/arm/cpu/arm1176/start.S	2019-06-14 10:34:16.335372404 +0800
@@ -151,7 +151,30 @@
 	bic	r0, r0, #0x3f
 	orr	r0, r0, #0xd3
 	msr	cpsr, r0
-
+#ifdef CONFIG_EARLYBOOT_ESPI_HANDSHAKE
+continue_early_init:                  /* Reset is done or not required */
+	ldr   r0, =0x1e6e2000         /* Unlock SCU */
+	ldr   r1, =0x1688a8a8
+	str   r1, [r0]
+	ldr   r0, =0x1e6e2070
+	ldr   r2, =0x2000000
+	ldr   r1, [r0]
+	tst   r1, r2			/* Check eSPI is enabled */
+	beq   espi_early_init_done
+	ldr   r0, =0x1e6e2070
+	ldr   r1, [r0]
+	orr   r1, r1, #0x2000000
+	str   r1, [r0]
+	ldr   r0, =0x1e6e2000		/* Lock SCU */
+	ldr   r1, =0x1
+	str   r1, [r0]
+	/* Timing from ESPI master requires OOB channel ready bit be set early */
+	ldr   r0, =0x1e6ee000
+	ldr   r1, [r0]                    /* ESPI000: ESPI Engine Control Reg */
+	orr   r1, r1, #0x10 /* Set OOB Channel Ready bit */
+	str   r1, [r0]
+espi_early_init_done:
+#endif 
 /*
  *************************************************************************
  *
