--- uboot_ori/board/ast2500evb/ast2500evb.c	2017-03-09 19:34:51.050112233 +0530
+++ uboot/board/ast2500evb/ast2500evb.c	2017-03-09 19:43:16.646109301 +0530
@@ -114,10 +114,10 @@
 	hdmi_init();
 #endif	
 	
-#ifdef CONFIG_AST3100EVB
-	
 	*((volatile ulong*) 0x1e6e2000) = 0x1688A8A8;	/* unlock SCU */
-	
+
+#ifdef CONFIG_AST3100EVB	
+
 	/* Set OSC output to 12 MHz for Codec */
 	*(volatile u32 *)(0x1e6e2010) |= 0x00000030;
 	mdelay(1);									/* Wait for SCU14 becomes 0 */
@@ -137,10 +137,23 @@
 	/* Drive GPIO pins for LED, Power On, USB HUB Reset */
      *(volatile u32 *)(0x1e780004) |= 0x0000000e;	
      *(volatile u32 *)(0x1e780000) |= 0x0000000f;
-	
+
+#endif
+
+        *(volatile u32 *)(0x1e6e2080) &= 0xFF00FFFF; /* Disable UART3, configure GPIO */
+        *(volatile u32 *)(0x1e6e2070) |= 0x02400000; /* Enable GPIOE Passthrough and eSPI mode */
+
+	*(volatile u32 *)(0x1e6e2004) &= ~(0x200); // Clear reset PWM controller
+
 	*((volatile ulong*) SCU_KEY_CONTROL_REG) = 0; /* lock SCU */
 	
-#endif	
+	/* Set all PWM to the highest duty cycle by default */
+	*(volatile u32 *)(AST_PWM_FAN_BASE + 0x0)  |= 0x00000F01;// Enable PWM A~D
+	*(volatile u32 *)(AST_PWM_FAN_BASE + 0x8)   = 0x0;
+	*(volatile u32 *)(AST_PWM_FAN_BASE + 0xC)   = 0x0;
+	*(volatile u32 *)(AST_PWM_FAN_BASE + 0x40) |= 0x00000F00;// Enable PWM E~H
+	*(volatile u32 *)(AST_PWM_FAN_BASE + 0x48)  = 0x0;
+	*(volatile u32 *)(AST_PWM_FAN_BASE + 0x4C)  = 0x0;
 
 	setenv("verify", "n");
 	return (0);
