--- linux/arch/arm/mach-astevb/ast_p2x.h	1970-01-01 00:00:00.000000000 +0000
+++ linux.new/arch/arm/mach-astevb/ast_p2x.h	2017-05-05 21:45:30.438347811 +0000
@@ -0,0 +1,68 @@
+/****************************************************************
+ **                                                            **
+ **    (C)Copyright 2006-2009, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        5555 Oakbrook Pkwy Suite 200, Norcross              **
+ **                                                            **
+ **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+****************************************************************/
+
+#ifndef _AST_P2X_H_
+#define _AST_P2X_H_
+
+// P2X register
+#define AST_P2X_STATUS_CTRL			0x00
+#define AST_P2X_INTR_EN_STATUS		0x04
+#define AST_P2X_TX_DESC3			0x10
+#define AST_P2X_TX_DESC2			0x14
+#define AST_P2X_TX_DESC1			0x18
+#define AST_P2X_TX_DESC0			0x1C
+#define AST_P2X_TX_DATA_PORT		0x20
+#define AST_P2X_RX_DESC3			0x40
+#define AST_P2X_RX_DESC2			0x44
+#define AST_P2X_RX_DESC1			0x48
+#define AST_P2X_RX_DESC0			0x4C
+#define AST_P2X_RX_DATA_PORT		0x50
+
+#define AST_P2X_MSI_INTR_EN			0x70
+#define AST_P2X_MSI_INTR_STATUS		0x74
+
+#define AST_P2X_DIR_MAP_ADDR		0x80
+#define AST_P2X_DIR_MAP_MASK		0x84
+#define AST_P2X_DIR_MAP_TAG			0x88
+
+// Register bit value
+// AST_P2X_STATUS_CTRL
+#define P2X_GET_RX_LEN(x)			(((x >> 18) & 0xf) * 4)
+#define P2X_RX_IDLE					(1 << 17)
+#define P2X_TX_IDLE					(1 << 16)
+
+#define P2X_RX_MSI_EN				(1 << 5)
+#define P2X_UNLOCK_RX_BUFF			(1 << 4)
+#define P2X_RX_MATCH_EN				(1 << 3)
+#define P2X_DROP_DISABLE			(1 << 2)
+#define P2X_TX_TRIGGER				(1 << 1)
+#define P2X_RX_EN					(1)
+
+// AST_P2X_INTR_EN_STATUS
+#define P2X_INTD_EN					(1 << 21)
+#define P2X_INTC_EN					(1 << 20)
+#define P2X_INTB_EN					(1 << 19)
+#define P2X_INTA_EN					(1 << 18)
+#define P2X_RX_INT_EN				(1 << 17)
+#define P2X_TX_INT_EN				(1 << 16)
+
+#define P2X_INTR_STATUS_MSI			(1 << 6)
+#define P2X_INTR_STATUS_INTD		(1 << 5)
+#define P2X_INTR_STATUS_INTC		(1 << 4)
+#define P2X_INTR_STATUS_INTB		(1 << 3)
+#define P2X_INTR_STATUS_INTA		(1 << 2)
+#define P2X_RX_COMPLETE				(1 << 1)
+#define P2X_TX_COMPLETE				(1)
+
+
+#endif
+
