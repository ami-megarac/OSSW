--- uboot/arch/arm/cpu/astcommon/eth_scu.c	2016-10-11 11:08:02.499710692 +0800
+++ uboot_new/arch/arm/cpu/astcommon/eth_scu.c	2016-10-11 11:08:05.063710683 +0800
@@ -30,9 +30,11 @@
 {
 #if defined(CONFIG_AST3200)
     int  skuid = 2; // the default is A1 demo board
-    u32  reg;       
-    
-    reg = 0x3082e45e;
+    u32  reg;
+
+    reg = ast_scu_read(AST_SCU_HW_STRAP1);
+    reg &= 0x00800000;
+    reg |= 0x3002e45e;
     ast_scu_write(reg, AST_SCU_HW_STRAP1);
     ast_scu_write(~reg, AST_SCU_REVISION_ID);
 #endif
