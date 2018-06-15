--- u-boot_orig/arch/arm/cpu/astcommon/regs-ahbc.h	1969-12-31 19:00:00.000000000 -0500
+++ u-boot_mod/arch/arm/cpu/astcommon/regs-ahbc.h	2015-12-23 20:00:49.880568509 -0500
@@ -0,0 +1,42 @@
+/* arch/arm/mach-aspeed/include/mach/regs-ast1010-scu.h
+ *
+ * Copyright (C) 2012-2020  ASPEED Technology Inc.
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License version 2 as
+ * published by the Free Software Foundation.
+ *
+ *   History      : 
+ *    1. 2012/12/29 Ryan Chen Create
+ * 
+********************************************************************************/
+#ifndef __AST_AHBC_H
+#define __AST_AHBC_H                     1
+
+//#include <asm/arch/aspeed.h>
+
+/* Register for AHBC */
+#define AST_AHBC_PROTECT			0x00		/*	Protection Key Register 	*/
+
+
+#define AST_AHBC_PRIORITY_CTRL		0x80		/*	Priority Cortrol Register */
+
+#define AST_AHBC_ADDR_REMAP		0x8C		/*	Address Remapping Register */
+
+/*	AST_AHBC_PROTECT			0x00		Protection Key Register 	*/
+#define AHBC_PROTECT_UNLOCK		0xAEED1A03
+
+/*	AST_AHBC_ADDR_REMAP		0x8C		Address Remapping Register */
+#define AHBC_PCI_REMAP1			(1 << 5)
+#define AHBC_PCI_REMAP0			(1 << 4)
+
+#if defined(AST_SOC_G5) 
+#define AHBC_PCIE_MAP				(1 << 5)
+#define AHBC_LPC_PLUS_MAP			(1 << 4)
+#else
+#define AHBC_BOOT_REMAP			1
+#endif
+
+
+#endif
+
