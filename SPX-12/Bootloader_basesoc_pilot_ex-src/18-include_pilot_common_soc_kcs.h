--- uboot_old/include/soc_kcs.h	1970-01-01 05:30:00.000000000 +0530
+++ uboot/include/soc_kcs.h	2017-08-16 13:59:27.849307109 +0530
@@ -0,0 +1,36 @@
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
+#define MAX_KCS_CHANNEL    0x02
+
+typedef volatile unsigned char LPC_REG_DT;
+
+#define LPC_WRITE_STATUS_BIT            0x00
+#define LPC_READ_STATUS_BIT             0xFF
+
+#define KCS_IDR1      0x00
+#define KCS_ODR1      0x00
+#define KCS_CMD1      0x02
+#define KCS_STATUS_1    0x03
+
+#define KCS_IDR2      0x00
+#define KCS_ODR2      0x00
+#define KCS_CMD2      0x02
+#define KCS_STATUS_2    0x03
+
+#endif
+
