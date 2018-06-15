--- u-boot-2013.07/arch/arm/cpu/armv7/lowlevel_init.S	2015-03-23 07:23:59.410458733 -0400
+++ mywork/arch/arm/cpu/armv7/lowlevel_init.S	2015-04-22 08:23:12.190148852 -0400
@@ -52,6 +52,7 @@
 	/*
 	 * go setup pll, mux, memory
 	 */
-	bl	s_init
+	bl	pilot4_lowlevel_init
+	
 	pop	{ip, pc}
 ENDPROC(lowlevel_init)
