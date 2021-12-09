--- uboot_old/include/soc_kcs.h	1970-01-01 05:30:00.000000000 +0530
+++ uboot/include/soc_kcs.h	2017-08-16 10:55:28.369897690 +0530
@@ -0,0 +1,37 @@
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
+ ***                                                            **
+ ***            All Rights Reserved.                            **
+ ***                                                            **
+ ***        6145-F, Northbelt Parkway, Norcross,                **
+ ***                                                            **
+ ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ ***                                                            **
+ ******************************************************************/
+/* AST KCS Registers offsets used for KCS - YAFU Upgrade */
+
+#ifndef SOC_KCS_H
+#define SOC_KCS_H
+
+#define MAX_KCS_CHANNEL     2
+
+typedef volatile unsigned long LPC_REG_DT;
+
+#define LPC_WRITE_STATUS_BIT            0x00
+#define LPC_READ_STATUS_BIT             0xFF
+
+#define KCS_IDR3			0x24
+#define KCS_ODR3			0x30	
+#define KCS_STATUS_3		0x3C	
+
+#define KCS_IDR2			0x28
+#define KCS_ODR2			0x34
+#define KCS_STATUS_2		0x40
+
+#define KCS_IDR1			0x2C
+#define KCS_ODR1			0x38
+#define KCS_STATUS_1		0x44
+
+#endif
