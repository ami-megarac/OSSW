--- linux/drivers/pci/host/ast_pciehost.h	1970-01-01 00:00:00.000000000 +0000
+++ linux.new/drivers/pci/host/ast_pciehost.h	2017-05-05 22:21:44.797721106 +0000
@@ -0,0 +1,73 @@
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
+#ifndef _AST_PCIEHOST_H_
+#define _AST_PCIEHOST_H_
+
+#define AST_PCIE_HOST_DRIVER_NAME		"ast_pcie"
+
+// AHB register
+#define AST_AHBC_ADDR_REMAP				0x8C
+// SCU register
+#define AST_SCU_SYS_RESET_CTRL			0x04
+#define AST_SCU_MISC2_CTRL				0x4C
+#define AST_SCU_HARDWARE_STRAP			0x70
+#define AST_SCU_REVISIOIN_ID			0x7C
+// PCIe host register
+#define AST_PCIE_HOST_CC				0x04
+#define AST_PCIE_HOST_MISC1				0x30
+#define AST_PCIE_HOST_MISC2				0x50
+#define AST_PCIE_HOST_MISC3				0x58
+#define AST_PCIE_HOST_LOCK				0x7C
+
+#define AST_PCIE_HOST_LINK				0xC0
+#define AST_PCIE_HOST_INT				0xC4
+
+// Register bit value
+// AST_AHBC_ADDR_REMAP
+#define AHBC_PCIE_MAPPING				(0x1 << 5)
+// AST_SCU_SYS_RESET_CTRL
+#define SCU_RESET_P2X					(0x1 << 24)
+#define SCU_RESET_PCIE_DIR				(0x1 << 21)
+#define SCU_RESET_PCIE					(0x1 << 19)
+// AST_SCU_MISC2_CTRL
+#define SCU_MISC2_PCIE_MAPPING_HIGH		(0x1 << 15)
+#define SCU_MISC2_MALI_DTY_MODE			(0x1 << 8)
+#define SCU_MISC2_MALI_RC_MODE			(0x1 << 7)
+// AST_SCU_REVISIOIN_ID
+#define SCU_REV_ID_MASK					(0x3 << 3)
+
+// PCIe host bit value
+#define PCIE_HOST_CLASS_CODE(x)			(x << 8)
+#define PCIE_HOST_REV_ID(x)				(x)
+// 0x30
+#define PCIE_HOST_ROOT_COMPLEX_ID(x)	(x << 4)
+// 0x58
+#define PCIE_HOST_CFG_ADDR(x)			(x & 0xfff)
+#define PCIE_HOST_CFG_ADDR_MASK			(0xfff)
+#define PCIE_HOST_MSI_ACK				(0x1 << 15)
+#define PCIE_HOST_CFG_ACK				(0x1 << 14)
+#define PCIE_HOST_CFG_WRITE				(0x1 << 13)
+#define PCIE_HOST_CFG_READ				(0x1 << 12)
+// 0xC0
+#define PCIE_HOST_LINK_STATUS			(0x1 << 5)
+// 0x7C
+#define PCIE_HOST_UNLOCK				0xA8
+
+extern void ast_pcie_cfg_read(u8 type, u32 bdf_offset, u32 *value);
+extern void ast_pcie_cfg_write(u8 type, u8 byte_en, u32 bdf_offset, u32 data);
+extern void ast_p2x_addr_map(u32 mask, u32 addr);
+extern int ast_p2x_init(void);
+extern void ast_p2x_exit(void);
+
+#endif
+
