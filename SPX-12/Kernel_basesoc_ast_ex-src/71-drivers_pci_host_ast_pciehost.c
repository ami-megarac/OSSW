--- linux/drivers/pci/host/ast_pciehost.c	1970-01-01 00:00:00.000000000 +0000
+++ linux.new/drivers/pci/host/ast_pciehost.c	2017-05-05 22:21:39.329802504 +0000
@@ -0,0 +1,423 @@
+/*
+ ****************************************************************
+ **                                                            **
+ **    (C)Copyright 2009-2015, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
+ **                                                            **
+ **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+ ****************************************************************
+ */
+#include <linux/kernel.h>
+#include <linux/types.h>
+#include <linux/platform_device.h>
+#include <linux/interrupt.h>
+
+#include <linux/delay.h>
+
+#include <linux/pci.h>
+#include <linux/irq.h>
+#include <linux/spinlock.h>
+#include <linux/msi.h>
+
+#include <asm/mach/pci.h>
+#include <asm/mach-types.h>
+
+#include <mach/hardware.h>
+#include <mach/platform.h>
+#include <mach/irqs.h>
+
+#include <linux/module.h>
+
+#include <linux/slab.h>
+#include <linux/msi.h>
+#include <linux/pci.h>
+
+#include "ast_pciehost.h"
+#include <linux/semaphore.h>
+
+#ifdef CONFIG_PCIE_DEBUG
+#define AST_PCIEDBG(fmt, args...) printk("%s() " fmt, __FUNCTION__, ## args)
+#else
+#define AST_PCIEDBG(fmt, args...)
+#endif
+
+static void *ast_pcie_host_virt_base;
+static void *ast_pciarbiter_virt_base;
+static int pciehost_hw_driver_init = 0;
+
+/* -------------------------------------------------- */
+
+static inline u32 ast_pcie_host_read_reg(u32 offset)
+{
+    return( ioread32( (void * __iomem)ast_pcie_host_virt_base + offset ) );
+}
+
+static inline void ast_pcie_host_write_reg(u32 value, u32 offset) 
+{
+    iowrite32(value, (void * __iomem)ast_pcie_host_virt_base + offset);
+}
+
+/* -------------------------------------------------- */
+
+void ast_pciarbiter_pcie_init(void)
+{
+    iowrite32(0xacedbd1f, (ast_pciarbiter_virt_base + 0x00)); //unlock
+    iowrite32(0x00010004, (ast_pciarbiter_virt_base + 0x04)); //addr_04
+    iowrite32(0x00000002, (ast_pciarbiter_virt_base + 0x08)); //data_vgamm_enable
+    iowrite32(0x00010010, (ast_pciarbiter_virt_base + 0x04)); //addr_10
+    iowrite32(0x80000000, (ast_pciarbiter_virt_base + 0x08)); //data_vgamm_bar
+    iowrite32(0x00000000, (ast_pciarbiter_virt_base + 0x00)); //lock
+}
+
+void ast_sdmc_disable_mem_protection(u8 reg)
+{
+    uint32_t status;
+    
+    *(volatile u32 *)(IO_ADDRESS(AST_SDRAMC_BASE)) = (0xFC600309); //Unlock SDMC register
+    
+    status = *(volatile u32 *)(IO_ADDRESS( (AST_SDRAMC_BASE + 0x08) ));
+    status &= ~(1<< reg);
+    *(volatile u32 *)(IO_ADDRESS( (AST_SDRAMC_BASE + 0x08) )) = status;
+    
+    *(volatile u32 *)(IO_ADDRESS(AST_SDRAMC_BASE)) = 0xaa;			//Lock SDMC register
+}
+
+void ast_scu_init_pcie(void)
+{
+    uint32_t status;
+    status = *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) ));
+    if((status & SCU_RESET_PCIE_DIR) && (!( status  & SCU_RESET_PCIE))) {
+        //do nothing
+        printk("No need init PCIe \n");
+    } else {	
+        
+    //SCU PCIe Initialization
+    *(volatile u32 *)(IO_ADDRESS(AST_SCU_BASE)) = (0x1688A8A8); //Unlock SCU register
+    
+    //pcie host reset
+    status = *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) ));
+    status &= ~SCU_RESET_PCIE;
+    *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) )) = status;
+    
+    status = *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) ));
+    status |= SCU_RESET_PCIE_DIR;
+    *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) )) = status;
+    
+    status = *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) ));
+    status |= SCU_RESET_PCIE;
+    *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) )) = status;
+    
+    mdelay(10);
+    
+    status = *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) ));
+    status &= ~SCU_RESET_PCIE;
+    *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) )) = status;
+    
+    //p2x reset
+    status = *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) ));
+    status &= ~SCU_RESET_P2X;
+    *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_SYS_RESET_CTRL) )) = status;
+    
+    //use 0x7c for clr
+    //For unknown reason, VGA memory size has to be 64MB, aka, hardware strap 3<<2
+    *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_REVISIOIN_ID) )) = SCU_REV_ID_MASK;
+    *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_HARDWARE_STRAP) )) = (3 << 2);
+    
+    status = *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_MISC2_CTRL) ));
+    status |= SCU_MISC2_PCIE_MAPPING_HIGH | SCU_MISC2_MALI_RC_MODE | SCU_MISC2_MALI_DTY_MODE;
+    *(volatile u32 *)(IO_ADDRESS( (AST_SCU_BASE + AST_SCU_MISC2_CTRL) )) = status;
+    
+    *(volatile u32 *)(IO_ADDRESS(AST_SCU_BASE)) = 0;			//Lock SCU register
+        }
+}
+
+void ast_ahbc_pcie_mapping(u8 enable)
+{
+    uint32_t status;
+    
+    *(volatile u32 *)(IO_ADDRESS(AST_AHBC_BASE)) = (0xAEED1A03); //Unlock AHB register
+    
+    status = *(volatile u32 *)(IO_ADDRESS( (AST_AHBC_BASE + AST_AHBC_ADDR_REMAP) ));
+    if(enable)
+        status |= AHBC_PCIE_MAPPING;
+    else
+        status &= ~AHBC_PCIE_MAPPING;
+    *(volatile u32 *)(IO_ADDRESS( (AST_AHBC_BASE + AST_AHBC_ADDR_REMAP) )) = status;
+    
+    *(volatile u32 *)(IO_ADDRESS(AST_AHBC_BASE)) = 0;			//Lock AHB register
+}
+
+
+static int ast_pcie_host_read_config(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *val)
+{
+    if(!(ast_pcie_host_read_reg(AST_PCIE_HOST_LINK) & PCIE_HOST_LINK_STATUS)) {
+        printk("R%x:%x:%x => size = %d, where = %xh, PCIBIOS_DEVICE_NOT_FOUND \n", bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), size, where);
+        return PCIBIOS_DEVICE_NOT_FOUND;
+    }
+    AST_PCIEDBG("R%x:%x:%x => size = %d, where = %xh, ", bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), size, where);
+    
+    if((PCI_SLOT(devfn) > 0) && (bus->number < 2)) {
+        return PCIBIOS_DEVICE_NOT_FOUND;
+    } else if(bus->number == 0) {
+        ast_pcie_cfg_read(0, 
+                        (bus->number << 24) |
+                        (PCI_SLOT(devfn) << 19) |
+                        (PCI_FUNC(devfn) << 16) | 
+                        (where & ~3), val);		
+    } else {
+        ast_pcie_cfg_read(1, 
+                        (bus->number << 24) |
+                        (PCI_SLOT(devfn) << 19) |
+                        (PCI_FUNC(devfn) << 16) | 
+                        (where & ~3), val);
+    }
+    
+    AST_PCIEDBG("val = %xh \n",*val);
+    
+    if (size == 1)
+        *val = (*val >> (8 * (where & 3))) & 0xff;
+    else if (size == 2)
+        *val = (*val >> (8 * (where & 3))) & 0xffff;
+    
+    return PCIBIOS_SUCCESSFUL;
+}
+
+static int ast_pcie_host_write_config(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 val)
+{
+    if(!(ast_pcie_host_read_reg(AST_PCIE_HOST_LINK) & PCIE_HOST_LINK_STATUS)) {
+        printk("W%x:%x:%x => size = %d, where = %xh, PCIBIOS_DEVICE_NOT_FOUND \n", bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), size, where);
+        return PCIBIOS_DEVICE_NOT_FOUND;
+    }
+    AST_PCIEDBG("W%x:%x:%x => size = %d, where = %xh, val = %xh\n", bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), size, where, val);
+    
+    if(bus->number == 0) {
+        ast_pcie_cfg_write(0, (((1 << size) -1) << (where & 0x3)),
+                    (bus->number << 24) |
+                    (PCI_SLOT(devfn) << 19) |
+                    (PCI_FUNC(devfn) << 16) | 
+                    (where & ~3), val << ((where & 0x3) * 8));
+    } else {
+        ast_pcie_cfg_write(1, (((1 << size) -1) << (where & 0x3)),
+                    (bus->number << 24) |
+                    (PCI_SLOT(devfn) << 19) |
+                    (PCI_FUNC(devfn) << 16) | 
+                    (where & ~3), val << ((where & 0x3) * 8));	
+    }
+    
+    return PCIBIOS_SUCCESSFUL;
+}
+
+static struct pci_ops ast_pcie_ops = {
+    .read	= ast_pcie_host_read_config,
+    .write	= ast_pcie_host_write_config,
+};
+
+static struct resource non_mem = {
+    .name	= "AST PCIe non-prefetchable memory region",
+    .start	= AST_PCIE_WIN_BASE,
+    .end	= (0x80000000 - 1),
+    .flags	= IORESOURCE_MEM,
+};
+
+static int ast_pcie_setup(int nr, struct pci_sys_data *sys)
+{
+    printk("PCI nr : %d >>> \n", nr);
+    
+    // px2 decode addr
+    ast_p2x_addr_map(0xF0000000, AST_PCIE_WIN_BASE);
+    
+    // VGA Init for memory cycle
+    ast_pciarbiter_pcie_init();
+    
+    ast_sdmc_disable_mem_protection(16);
+    
+    // Add for Gen2
+    ast_pcie_host_write_reg(0x20, AST_PCIE_HOST_MISC2);
+    ast_pcie_host_write_reg((ast_pcie_host_read_reg(AST_PCIE_HOST_MISC3) & ~(PCIE_HOST_CFG_ADDR_MASK| PCIE_HOST_CFG_WRITE)) | PCIE_HOST_CFG_ADDR(0x090), AST_PCIE_HOST_MISC3);
+    ast_pcie_host_write_reg(ast_pcie_host_read_reg(AST_PCIE_HOST_MISC3) | PCIE_HOST_CFG_WRITE , AST_PCIE_HOST_MISC3);
+    ast_pcie_host_write_reg((ast_pcie_host_read_reg(AST_PCIE_HOST_MISC3) & ~PCIE_HOST_CFG_WRITE) , AST_PCIE_HOST_MISC3);	
+    
+    ast_pcie_cfg_write(0, (((1 << 4) -1) << (PCI_PRIMARY_BUS & 0x3)), (PCI_PRIMARY_BUS & ~3), 0 << ((PCI_PRIMARY_BUS & 0x3) * 8));
+    
+    ast_pcie_cfg_write(0, (((1 << 4) -1) << (PCI_SECONDARY_BUS & 0x3)), (PCI_SECONDARY_BUS & ~3), 1 << ((PCI_SECONDARY_BUS & 0x3) * 8));
+    
+    ast_pcie_cfg_write(0, (((1 << 4) -1) << (PCI_SUBORDINATE_BUS & 0x3)), (PCI_SUBORDINATE_BUS & ~3), 0xf << ((PCI_SUBORDINATE_BUS & 0x3) * 8));
+    
+    //	ast_pcie_cfg_write(0, 0xf, PCI_PRIMARY_BUS, 0);
+    //	ast_pcie_cfg_write(0, 0xf, PCI_SECONDARY_BUS, 1);		
+    //	ast_pcie_cfg_write(0, 0xf, PCI_SUBORDINATE_BUS, 1); 
+    
+    if (request_resource(&iomem_resource, &non_mem)) {
+        printk(KERN_ERR "PCI: unable to allocate non-prefetchable " "memory region\n");
+        return -EBUSY;
+    }
+    pci_add_resource_offset(&sys->resources, &non_mem, sys->mem_offset);
+    
+    return 1;
+}
+
+static int irq_tab[] = {
+    IRQ_PCIE_INTA,
+    IRQ_PCIE_INTB,
+    IRQ_PCIE_INTC,
+    IRQ_PCIE_INTD
+};
+
+static int ast_pcie_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
+{
+    AST_PCIEDBG("slot = %d, pin = %d, irq = %d\n", slot, pin, irq_tab[(slot + pin - 1) % 4]);
+    return irq_tab[(slot + pin - 1) % 4];
+}
+
+static inline void init_pcie_root_complex_bridge(void)
+{
+    // SCU Init PCIe
+    ast_scu_init_pcie();
+    
+    ast_ahbc_pcie_mapping(1);
+    
+    // plda enable 
+    ast_pcie_host_write_reg(PCIE_HOST_UNLOCK, AST_PCIE_HOST_LOCK);
+    ast_pcie_host_write_reg(PCIE_HOST_CLASS_CODE(0x60400) | PCIE_HOST_REV_ID(4), AST_PCIE_HOST_CC);
+    ast_pcie_host_write_reg(PCIE_HOST_ROOT_COMPLEX_ID(0x3), AST_PCIE_HOST_MISC1);
+}
+
+static void ast_pcie_release(struct device * dev)
+{
+    /* Currently not doing any operation on release */
+    return;
+}
+
+
+static struct hw_pci ast_pcie = {
+    .nr_controllers = 1,
+    .map_irq        = ast_pcie_map_irq,		
+    .setup          = ast_pcie_setup,	
+    .ops			= &ast_pcie_ops,
+};
+
+static struct resource ast_pcie_resources[] = {
+    {
+        /* Register space */
+        .name           = "pcie-regs",
+        .start          = AST_PCIE_BASE,
+        .end            = AST_PCIE_BASE + SZ_16K - 1,
+        .flags          = IORESOURCE_MEM,
+    },
+#if 0   
+    {
+        /* Non-prefetch memory */
+        .name           = "pcie-nonprefetch",
+        .start          = AST_PCIE_WIN_BASE,
+        .end                    = AST_PCIE_WIN_BASE + SZ_64K - 1,
+        .flags          = IORESOURCE_MEM,
+    },
+#endif
+#if 0   
+    {
+        /* IO window */
+        .name           = "pcie-io",
+        .start          = AST_PCIE_IO_BASE,
+        .end            = AST_PCIE_IO_BASE + SZ_2M + SZ_1M - 1,
+        .flags          = IORESOURCE_IO,
+    },
+#endif
+#if 0
+    {
+        /* Inbound memory window */
+        .name           = "pcie-inbound0",
+        .start          = AST_DRAM_BASE,
+        .end                    = AST_DRAM_BASE + SZ_2G - 1,
+        .flags          = IORESOURCE_MEM,
+    },
+#endif
+};
+
+
+static struct platform_device ast_pcie_device = {
+    .name   = "ast-pcie",
+    .id  = 0,
+    .resource                       = ast_pcie_resources,
+    .num_resources  = ARRAY_SIZE(ast_pcie_resources),
+    .dev = {
+        .release = ast_pcie_release,
+     }
+};
+
+
+static int __init ast_pcie_host_probe(struct platform_device *pdev)
+{
+    ast_pcie_host_virt_base =  ioremap(AST_PCIE_BASE, SZ_256);	
+    if (!ast_pcie_host_virt_base) {
+        printk(KERN_WARNING "%s: ioremap failed\n", AST_PCIE_HOST_DRIVER_NAME);
+        return -ENOMEM;
+    }
+    
+    ast_pciarbiter_virt_base = ioremap(AST_PCIARBITER_BASE, SZ_32);
+    if (!ast_pciarbiter_virt_base) {
+        printk(KERN_WARNING "%s: AST_PCIARBITER_VA_BASE ioremap failed\n", AST_PCIE_HOST_DRIVER_NAME);
+        return -ENOMEM;
+    }
+    
+    init_pcie_root_complex_bridge();
+    
+    mdelay(110);
+    
+    if (ast_pcie_host_read_reg(AST_PCIE_HOST_LINK) & PCIE_HOST_LINK_STATUS)
+    {
+        printk("%s: PCIe Host Link\n", AST_PCIE_HOST_DRIVER_NAME);
+        pci_common_init_dev(&pdev->dev, &ast_pcie);
+        pciehost_hw_driver_init = 1;
+    } else {
+        printk("%s: PCIe Host UnLink\n", AST_PCIE_HOST_DRIVER_NAME);
+        pciehost_hw_driver_init = 0;
+    }
+    
+    printk("The PCIe Host HW Driver is loaded successfully.\n" );
+    return 0;
+}
+
+static const struct of_device_id ast_pcie_of_match[] = {
+    { .compatible = "ast, ast-pcie", },
+    {},
+};
+MODULE_DEVICE_TABLE(of, ast_pcie_of_match);
+
+static struct platform_driver ast_pcie_driver = {
+    .driver = {
+        .name	= "ast-pcie",
+        .owner	= THIS_MODULE,
+        .of_match_table = ast_pcie_of_match,
+    },
+};
+
+static int __init ast_pcie_host_init(void)
+{  
+    platform_device_register(&ast_pcie_device);
+    platform_driver_probe(&ast_pcie_driver, ast_pcie_host_probe);
+    
+    return 0;
+}
+
+static void __exit ast_pcie_host_exit(void)
+{
+    if (pciehost_hw_driver_init == 1)
+        release_resource(&non_mem);
+    
+    platform_device_unregister(&ast_pcie_device);
+    platform_driver_unregister(&ast_pcie_driver);    
+    iounmap (ast_pciarbiter_virt_base);
+    iounmap (ast_pcie_host_virt_base);
+    pciehost_hw_driver_init = 0;
+}
+
+module_init(ast_pcie_host_init);
+module_exit(ast_pcie_host_exit);
+
+MODULE_AUTHOR( "American Megatrends" );
+MODULE_DESCRIPTION( "ASPEED AST SoC PCIe Host Driver" );
+MODULE_LICENSE( "GPL" );
