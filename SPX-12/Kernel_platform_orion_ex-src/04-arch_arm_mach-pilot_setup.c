diff -Naur linux_org/arch/arm/mach-pilot/setup.c linux_new/arch/arm/mach-pilot/setup.c
--- linux_org/arch/arm/mach-pilot/setup.c	1969-12-31 19:00:00.000000000 -0500
+++ linux_new/arch/arm/mach-pilot/setup.c	2016-10-03 17:42:04.644617026 -0400
@@ -0,0 +1,295 @@
+/*
+ *  linux/arch/arm/mach-pilot3hornet/setup.c
+ *
+ *  Copyright (C) 2009 ServerEngines
+ *  Derived from AMI's linux/arch/arm/mach-aviator/setup.c
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
+ *
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation; either version 2 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ *
+ */
+#include <linux/mm.h>
+#include <linux/init.h>
+#include <linux/init.h>
+#include <linux/major.h>
+#include <linux/fs.h>
+#include <linux/platform_device.h>
+#include <linux/device.h>
+#include <linux/serial.h>
+#include <linux/tty.h>
+#include <linux/serial_core.h>
+#include <linux/serial.h>
+#include <linux/serial_8250.h>
+#include <linux/serial_reg.h>
+#include <linux/clk.h>
+#include <mach/serial.h>
+
+#include <asm/pgtable.h>
+#include <asm/page.h>
+#include <asm/mach/map.h>
+#include <asm/setup.h>
+#include <asm/system.h>
+#include <asm/memory.h>
+#include <mach/hardware.h>
+#include <asm/mach-types.h>
+#include <asm/mach/arch.h>
+#include <linux/irqchip/arm-gic.h>
+#include <asm/irq.h>
+
+#include <linux/of_irq.h>
+#include <linux/of_platform.h>
+#include <linux/of.h>
+#include <linux/irqdomain.h>
+
+#include <mach/pilot4-soc.h>
+#include <linux/delay.h>
+
+#include "../../../drivers/mtd/spichips/spiflash.h"
+
+#define CONFIG_SECOND_INTC 1
+#ifdef CONFIG_SECOND_INTC
+#include <asm/mach/irq.h>
+#include <linux/irq.h>
+#include <linux/kallsyms.h>
+#endif
+
+/*SDK Update - BMC CPU Reset Workaround*/
+#include <linux/proc_fs.h>
+#include <linux/seq_file.h>
+#define SPI_ADDR    (SE_BOOT_SPI_VA_BASE + 0x0)
+#define SPI_CMD     (SE_BOOT_SPI_VA_BASE + 0x8)
+#define SPI_STS     (SE_BOOT_SPI_VA_BASE + 0x18)
+#define SPI_FIFO    (SE_BOOT_SPI_VA_BASE + 0x30)
+#define SPI_MISC     (SE_BOOT_SPI_VA_BASE + 0x1C)
+
+
+int io_mapping_done = 0;
+extern void __iomem *uart_base;
+extern void se_pilot4_timer_init(void);
+extern void pilot4_init_irq(void);
+extern void power_putstr(const char *ptr);
+
+static struct map_desc aviator_std_desc[] __initdata =
+{
+	/* SE-PILOT3 Register mappings */
+	PILOT_MAP_DESC,
+	{
+		.virtual	= SCU_PERIPH_VIRT,
+		.pfn		= __phys_to_pfn(SCU_PERIPH_PHYS),
+		.length		= SZ_8K,
+		.type		= MT_DEVICE,
+	}, 
+};
+
+static inline void pilot4_arch_idle(void)
+{
+  /*    
+   * This should do all the clock switching
+   * and wait for interrupt tricks
+   */
+  cpu_do_idle();
+}
+
+static void pilot4_arch_reset(enum reboot_mode reboot_mode, const char *cmd)
+{
+
+  int spi_bank = 0;
+  local_irq_disable();
+  //printk("P4 restart called with mode %d syscfg %lx\n",reboot_mode, *((volatile unsigned long *)(SE_SYS_CLK_VA_BASE + 0x00)));
+
+  // Default the Boot SPI device 
+  restore_spidevice_to_default_state(spi_bank);
+  *((volatile unsigned long *)(SE_SYS_CLK_VA_BASE + 0x00)) &= 0xFFFFFFFC;
+  /* Delay to allow the serial port to show the message */
+  mdelay(50);
+  *(volatile unsigned int *)(SE_LPC_ROM_VA_BASE + 0x854) = 0x83;  //  Default the Controller  SPI  
+  *(volatile unsigned int *)(SE_LPC_ROM_VA_BASE + 0x858) = 0; 	/*Make sure we dont reset other modules*/
+  *(volatile unsigned int *)(SE_LPC_ROM_VA_BASE + 0x850) = 0xD01;
+  //We will never reach here, hwoever if we reach if we reach here pls check wait_for_wfe in uboot
+  soft_restart(0);
+  /* We'll take a jump through zero as a poor second */
+
+}
+
+
+void __init
+se_pilot4_init(void)
+{
+   /*Reset SDHC/EMMC Host controllers. This is needed incase they are used in u-boot*/
+  *(volatile unsigned int *)(SE_LPC_ROM_VA_BASE + 0x854) = 0x40000000;
+  *(volatile unsigned int *)(SE_LPC_ROM_VA_BASE + 0x858) = 0x21;
+  *(volatile unsigned int *)(SE_LPC_ROM_VA_BASE + 0x850) = 0xD01;
+
+  /* Register idle and restart handler */
+  arm_pm_idle = pilot4_arch_idle;
+  arm_pm_restart = pilot4_arch_reset;
+}
+
+void __init
+pilot4_map_io(void)
+{
+	/* Do any initial hardware setup if needed */
+	iotable_init(aviator_std_desc, ARRAY_SIZE(aviator_std_desc));
+	io_mapping_done = 1;
+}
+
+/*SDK Update - BMC CPU Reset Workaround*/
+
+static void wait_for_spi_ready(void) {
+        while ((*(volatile unsigned int *)(SPI_STS)) & 0x01);
+}
+
+static void wait_till_ready(void)
+{
+    volatile unsigned char rdsts = 0;
+    wait_for_spi_ready();
+    *(volatile unsigned int *)(SPI_CMD) = 0x80000105;
+    wait_for_spi_ready();
+
+    rdsts = *(volatile unsigned char*)(SPI_FIFO);
+
+	while ((rdsts & 0x1) == 0x1)
+	{
+    		wait_for_spi_ready();
+            *(volatile unsigned int *)(SPI_CMD) = 0x80000105;
+    		wait_for_spi_ready();
+        	rdsts = *(volatile unsigned char*)(SPI_FIFO);
+	}
+}
+
+
+void reinit_spi(void)
+{
+	volatile unsigned char rd_extern_reg = 0;
+
+	*(volatile unsigned int*)(SPI_MISC) = 0xC0100804;
+	*(volatile unsigned int*)(SPI_ADDR) = 0;
+	
+	wait_till_ready();
+	// If 3B strap is set then send disable 4B mode; just-in-case
+	if ((*(volatile unsigned int*)(IO_ADDRESS(0x4010010c)) & 0x40) != 0x40)
+	{
+		wait_for_spi_ready();
+		*(volatile unsigned int *)(SPI_CMD) = 0x80001106;//write enable
+		wait_for_spi_ready();
+		wait_till_ready();
+
+		wait_for_spi_ready();
+		*(volatile unsigned int *)(SPI_CMD) = 0x800011e9;//exit address 4 byte mode
+		wait_for_spi_ready();
+		wait_till_ready();
+
+		wait_for_spi_ready();
+		*(volatile unsigned int *)(SPI_CMD) = 0x80001104;// write disable
+		wait_for_spi_ready();
+		wait_till_ready();
+
+		wait_for_spi_ready();
+    		*(volatile unsigned int *)(SPI_CMD) = 0x800001C8; // "Read Extended Address Register"
+    		wait_for_spi_ready();
+    		rd_extern_reg = *(volatile unsigned char*)(SPI_FIFO);
+    		//printk("Read Extended Address Register= 0x%llx\n", (long long)rd_extern_reg);
+    			if (rd_extern_reg == 0x01)// "Check Extended Address Register, Isn't It Supported 4 Byte Address Mode"
+    			{
+    				//printk("Extended Address Supported 4 Byte Address Mode\n");
+    				wait_for_spi_ready();
+				*(volatile unsigned int *)(SPI_CMD) = 0x80001106;// "write enabled"
+				wait_for_spi_ready();
+				wait_till_ready();
+
+				wait_for_spi_ready();
+				*(volatile unsigned int *)(SPI_CMD) = 0x800011C5; // "write Extended Address Register"
+				wait_for_spi_ready();
+				*(volatile unsigned char*)(SPI_FIFO) = 0x0;//"set Extended Address Register to 0"
+				wait_till_ready();
+
+				wait_for_spi_ready();
+				*(volatile unsigned int *)(SPI_CMD) = 0x80001104;//"write disable"
+				wait_for_spi_ready();
+				wait_till_ready();
+
+				wait_for_spi_ready();
+    				*(volatile unsigned int *)(SPI_CMD) = 0x800001C8; // "Read Extended Address Register"
+    				wait_for_spi_ready();
+    				rd_extern_reg = *(volatile unsigned char*)(SPI_FIFO);
+    				printk("After Diabled Supported 4 Byte Address Mode, Read Extended Address Register= 0x%llx\n", (long long)rd_extern_reg);
+    			}
+
+		// clear BMISC
+		*(volatile unsigned int*)(SPI_MISC) &= ~(1 << 24);
+	}
+	*(volatile unsigned long*)(SE_SYS_CLK_VA_BASE+0x20) &= 0xFFFFF000;
+	*(volatile unsigned long*)(SE_SYS_CLK_VA_BASE+0x20) |= 0x222;
+
+	*(volatile unsigned int *)(SPI_CMD) &= ~(0xFF000000);
+	wait_for_spi_ready();
+	//printk("reinit_spi done\n");
+}
+
+void do_bmc_cpu_reset(void)
+{
+	int check_timer_reg = 0;
+	#ifdef CONFIG_USE_SSP_RESET
+		local_irq_disable();
+	#endif
+ 		execute_smp_wfe();
+ 	#ifndef CONFIG_USE_SSP_RESET
+		reinit_spi();
+		/*stop timer3*/
+		*((volatile unsigned long *)(IO_ADDRESS(0x40411000)+0x08 + 0x28)) = 0x0;
+		check_timer_reg =  *((volatile unsigned long *)(IO_ADDRESS(0x40411000)+0x08 + 0x28));
+		while(check_timer_reg != 0)
+		{
+			printk("Timer Not Disable!!");				
+			*((volatile unsigned long *)(IO_ADDRESS(0x40411000)+0x08 + 0x28)) = 0x0;
+			check_timer_reg =  *((volatile unsigned long *)(IO_ADDRESS(0x40411000)+0x08 + 0x28));
+		}
+		udelay(1000);//wait for reset prepare setting ready
+	
+		/*Set ARM Processor and Internal ARM Bus Reset*/
+		*(volatile unsigned int *)IO_ADDRESS(0x40100854) = 3; 
+ 		/* Software Reset Enable*/
+		*(volatile unsigned int *)IO_ADDRESS(0x40100850) = 0xc01;
+	#endif
+
+	for(;;)
+		wfe();
+	printk("Issue came out of wfe\n");
+}
+extern void start_pilot_timer3(void);
+
+
+static __init int timer3_init(void)
+{
+	start_pilot_timer3();
+	return 0;
+}
+
+late_initcall(timer3_init);
+
+extern struct smp_operations platform_smp_ops;
+
+MACHINE_START(ORION, "Emulex's Orion Board")
+#ifdef CONFIG_SMP
+  .smp		= smp_ops(platform_smp_ops),
+ #endif
+	.map_io			= pilot4_map_io,
+	.init_irq		= pilot4_init_irq,
+	.init_time			= se_pilot4_timer_init,
+	.atag_offset    = 0x100,
+	.init_machine   = se_pilot4_init,
+MACHINE_END
