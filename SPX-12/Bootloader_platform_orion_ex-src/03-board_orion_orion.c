--- u-boot-2013.07/board/orion/orion.c	1969-12-31 19:00:00.000000000 -0500
+++ mywork/board/orion/orion.c	2015-03-24 10:19:03.184899101 -0400
@@ -0,0 +1,74 @@
+#include <common.h>
+#include <config.h>
+#include <asm/processor.h>
+
+#define P4_SCRATCH_128_BASE            	(0x40000000+ 0x0100200)
+#define P4_SYS_CLK_BASE                	(0x40000000+ 0x0100100)
+
+DECLARE_GLOBAL_DATA_PTR;
+
+extern void soc_init(void);
+
+int board_init (void)
+{
+	/*Should match with linux mach type for this board */
+	gd->bd->bi_arch_number = 5555;
+	/* Should match with linux Makefile.boot entry for params-phys-y */
+	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE+0x0100;
+
+	/*2016_11_30 SDK Update Patch 01 Mechanism to hold RESET information in SDK*/
+	*(volatile u32 *)(P4_SCRATCH_128_BASE + 0x7C) = *(volatile u32 *)(P4_SYS_CLK_BASE + 0x7B0);
+
+    if ((*(volatile u32 *)(P4_SYS_CLK_BASE + 0x7B0) & 0x210) != 0x0)
+    {
+		*(volatile u32 *)(P4_SYS_CLK_BASE) &= ~0x2;
+    }
+
+	*(volatile u32 *)(P4_SYS_CLK_BASE + 0x7B0) |= 0xFF7;
+
+       gd->flags = 0;
+
+	/* Initialize SOC related */
+	soc_init();
+
+       icache_enable();
+
+	return 0;
+}
+
+int misc_init_r(void)
+{
+  setenv("verify","n");
+  return 0;
+}
+
+int dram_init (void)
+{
+	bd_t *bd = gd->bd;
+	int remainder;
+	int mem_size = CONFIG_SYS_SDRAM_LEN;
+
+	/* Only one Bank*/
+	bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
+
+	/* Check available memory and fill into bdinfo*/
+	remainder = mem_size % (1024*1024);
+	if(remainder != 0){
+	    //ex : 117.1MB is not accept by Linux kernel
+	    bd->bi_dram[0].size = mem_size - remainder;
+	}else{
+	    bd->bi_dram[0].size = mem_size;
+	}
+
+       gd->ram_size=bd->bi_dram[0].size;
+
+	return (0);
+}
+
+short 
+ReadJumperConfig(void)
+{
+	/* Return Path ID */
+	return -1;
+}
+
