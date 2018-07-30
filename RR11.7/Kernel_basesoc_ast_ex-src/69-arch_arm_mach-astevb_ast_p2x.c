--- linux/arch/arm/mach-astevb/ast_p2x.c	1970-01-01 00:00:00.000000000 +0000
+++ linux.new/arch/arm/mach-astevb/ast_p2x.c	2017-05-05 22:14:11.523272343 +0000
@@ -0,0 +1,355 @@
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
+#include <linux/irq.h>
+#include <linux/msi.h>
+#include <linux/list.h>
+#include <linux/pci_regs.h>
+#include <linux/kernel.h>
+#include <linux/module.h>
+#include <linux/delay.h>
+#include <linux/interrupt.h>
+
+#include <mach/platform.h>
+#include <asm/io.h>
+
+#include <linux/irqchip/chained_irq.h>	
+#include <mach/hardware.h>
+
+#include "ast_p2x.h"
+#include <linux/delay.h>
+
+//#define AST_P2X_DEBUG 
+
+#ifdef AST_P2X_DEBUG
+#define P2XDBUG(fmt, args...) printk("%s() " fmt, __FUNCTION__, ## args)
+#else
+#define P2XDBUG(fmt, args...)
+#endif
+
+static void *ast_p2x_virt_base;
+
+static u8 txTag = 0;
+
+/* -------------------------------------------------- */
+
+static inline u32 ast_p2x_read_reg(u32 offset)
+{
+	return( ioread32( (void * __iomem)ast_p2x_virt_base + offset ) );
+}
+
+static inline void ast_p2x_write_reg(u32 value, u32 offset)
+{
+	iowrite32(value, (void * __iomem)ast_p2x_virt_base + offset);
+}
+
+/* -------------------------------------------------- */
+
+extern void ast_pcie_cfg_read(u8 type, u32 bdf_offset, u32 *value)
+{
+	u32 timeout =0;
+	u32 desc3,desc2;
+	txTag %= 0x7;
+//	printk("Read: type = %d, busfunc = %x ",type, bdf_offset);
+	if((ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS) & P2X_RX_COMPLETE) != 0)
+		printk("Receving complete\n");
+	
+	ast_p2x_write_reg(0x4000001 | (type << 24), AST_P2X_TX_DESC3);	
+	ast_p2x_write_reg(0x200f | (txTag << 8), AST_P2X_TX_DESC2);
+	ast_p2x_write_reg(bdf_offset, AST_P2X_TX_DESC1);
+	ast_p2x_write_reg(0, AST_P2X_TX_DESC0);
+//	ast_p2x_write_reg(0, AST_P2X_TX_DATA_PORT);
+
+	//trigger
+	ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_STATUS_CTRL) | P2X_TX_TRIGGER | P2X_RX_EN, AST_P2X_STATUS_CTRL);	
+
+	//wait 
+//	printf("trigger \n");
+	while(!(ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS) & P2X_RX_COMPLETE)) {
+		timeout++;
+		if(timeout > 10000) {
+			printk("time out \n");
+			*value = 0xffffffff;
+			goto out;
+		}
+	};
+
+	//read 
+	desc3 = ast_p2x_read_reg(AST_P2X_RX_DESC3);	
+	desc2 = ast_p2x_read_reg(AST_P2X_RX_DESC2);
+	ast_p2x_read_reg(AST_P2X_RX_DESC1);	
+
+	if( ((desc3 >> 24) == 0x4A) && 
+		((desc3 & 0xfff) == 0x1) && 
+		((desc2 & 0xe000) == 0)) {
+		*value = ast_p2x_read_reg(AST_P2X_RX_DATA_PORT);
+
+	} else {
+		*value = 0xffffffff;		
+		
+	}
+
+out:
+	txTag++;
+	ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_STATUS_CTRL) | P2X_UNLOCK_RX_BUFF |P2X_RX_EN, AST_P2X_STATUS_CTRL);	
+	ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS) | P2X_TX_COMPLETE | P2X_RX_COMPLETE, AST_P2X_INTR_EN_STATUS);	
+	//wait 
+	while(ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS) & P2X_RX_COMPLETE);
+//	printk("value %x \n",* value);
+}
+
+extern void ast_pcie_cfg_write(u8 type, u8 byte_en, u32 bdf_offset, u32 data)
+{
+    u32 timeout =0;
+	txTag %= 0x7;
+
+
+//	printk("Write byte_en : %x, offset: %x, value = %x \n",byte_en , bdf_offset, data);
+	ast_p2x_write_reg(0x44000001 | (type << 24), AST_P2X_TX_DESC3);	
+	ast_p2x_write_reg(0x2000 | (txTag << 8) | byte_en, AST_P2X_TX_DESC2);
+	ast_p2x_write_reg(bdf_offset, AST_P2X_TX_DESC1);
+	ast_p2x_write_reg(0, AST_P2X_TX_DESC0);
+	ast_p2x_write_reg(data, AST_P2X_TX_DATA_PORT);	
+
+	//trigger
+	ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_STATUS_CTRL) |P2X_TX_TRIGGER | P2X_RX_EN, AST_P2X_STATUS_CTRL);	
+//	printf("trigger \n");	
+	//wait 
+	while(!(ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS) & P2X_RX_COMPLETE));
+
+	//read TODO Check TAG 
+	ast_p2x_read_reg(AST_P2X_RX_DESC3);	
+	ast_p2x_read_reg(AST_P2X_RX_DESC2);
+	ast_p2x_read_reg(AST_P2X_RX_DESC1);	
+//	while(header && tag )
+	txTag++;	
+	ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_STATUS_CTRL) | P2X_UNLOCK_RX_BUFF |P2X_RX_EN, AST_P2X_STATUS_CTRL);	
+	ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS) | P2X_TX_COMPLETE | P2X_RX_COMPLETE, AST_P2X_INTR_EN_STATUS);	
+	//wait 
+	while(ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS) & P2X_RX_COMPLETE) {
+	    mdelay(1);
+	    timeout++;
+	    printk("timeout=%u , ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS)=0x%x\n", timeout, ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS));
+	    if (timeout>10)
+	        break;
+	}
+}
+
+extern void ast_p2x_addr_map(u32 mask, u32 addr)
+{
+	//Address mapping
+	ast_p2x_write_reg(addr, AST_P2X_DIR_MAP_ADDR);
+	ast_p2x_write_reg(mask, AST_P2X_DIR_MAP_MASK);
+	ast_p2x_write_reg(0x00000028, AST_P2X_DIR_MAP_TAG);
+}
+
+static void
+ast_p2x_irq_handler(unsigned int irq, struct irq_desc *desc)
+{
+    u32 msi = 0;
+    u32 i;
+    struct irq_chip *chip = irq_desc_get_chip(desc);
+    u32 sts = ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS);
+
+    P2XDBUG("sts :%x \n",sts);
+
+    chained_irq_enter(chip, desc);
+    
+    if(sts & P2X_INTR_STATUS_INTA) {
+        P2XDBUG("INT_INTA\n");
+        generic_handle_irq(IRQ_PCIE_INTA);
+    }
+
+    if(sts & P2X_INTR_STATUS_INTB) {
+        P2XDBUG("INT_INTB\n");
+        generic_handle_irq(IRQ_PCIE_INTB);
+    }
+
+    if(sts & P2X_INTR_STATUS_INTC) {
+        P2XDBUG("INT_INTC\n");
+        generic_handle_irq(IRQ_PCIE_INTC);
+    }
+
+    if(sts & P2X_INTR_STATUS_INTD) {
+        P2XDBUG("INT_INTD\n");
+        generic_handle_irq(IRQ_PCIE_INTD);
+    }
+
+    if(sts & P2X_INTR_STATUS_MSI) {
+        P2XDBUG("INT_MSI\n");
+        msi = ast_p2x_read_reg(AST_P2X_MSI_INTR_STATUS);
+        for(i = 0; i < 32; i++) {
+            if(msi & (1 << i)) {
+                generic_handle_irq(IRQ_PCIE_MSI0 + i);
+            }
+        }
+    }
+
+    chained_irq_exit(chip, desc);
+}
+
+
+static void ast_p2x_ack_irq(struct irq_data *d)
+{
+    unsigned int p2x_irq = d->irq - IRQ_PCIE_CHAIN_START;
+    if(p2x_irq > 3) {
+        p2x_irq -= 4;
+        ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_MSI_INTR_STATUS) | (1 << p2x_irq) , AST_P2X_MSI_INTR_STATUS);    
+        P2XDBUG("clr irq %d , msi irq %d \n", d->irq, p2x_irq);
+    } else {
+        ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS) | (1 << (p2x_irq + 2)) , AST_P2X_INTR_EN_STATUS);      
+        P2XDBUG("clr irq %d , intabcd %d \n", d->irq, p2x_irq);     
+    }   
+    
+}
+
+static void ast_p2x_mask_irq(struct irq_data *d)
+{
+    unsigned int p2x_irq = d->irq - IRQ_PCIE_CHAIN_START;
+
+    if(p2x_irq > 3) {
+        p2x_irq -= 4;
+        ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_MSI_INTR_EN) & ~(1 << p2x_irq) , AST_P2X_MSI_INTR_EN);   
+        P2XDBUG("disable irq %d , msi irq %d \n", d->irq, p2x_irq);
+    } else {
+        //disable irq
+        ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS) & ~(1 << (p2x_irq + 18)) , AST_P2X_INTR_EN_STATUS);    
+        P2XDBUG("disable irq %d , intabcd %d \n", d->irq, p2x_irq);     
+    }
+    
+}
+
+static void ast_p2x_unmask_irq(struct irq_data *d)
+{
+    unsigned int p2x_irq = d->irq - IRQ_PCIE_CHAIN_START;
+    
+    if(p2x_irq > 3) {
+        p2x_irq -= 4;
+        ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_MSI_INTR_EN) | (1 << p2x_irq) , AST_P2X_MSI_INTR_EN);    
+        P2XDBUG("enable irq %d , msi irq %d \n", d->irq, p2x_irq);          
+    } else {
+        //Enable IRQ ..
+        ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_INTR_EN_STATUS) | (1 << (p2x_irq + 18)) , AST_P2X_INTR_EN_STATUS); 
+        P2XDBUG("enalbe irq %d , intabcd %d \n", d->irq, p2x_irq);      
+    }
+    
+}
+
+static struct irq_chip ast_p2x_irq_chip = {
+    .name       = "P2X",
+    .irq_ack    = ast_p2x_ack_irq,
+    .irq_mask   = ast_p2x_mask_irq,
+    .irq_unmask = ast_p2x_unmask_irq,
+};
+
+#ifdef CONFIG_PCI_MSI
+static DECLARE_BITMAP(msi_irq_in_use, AST_NUM_MSI_IRQS);
+
+static int
+msi_create_irq(void)
+{
+    int irq, pos;
+again:
+    pos = find_first_zero_bit(msi_irq_in_use, AST_NUM_MSI_IRQS);
+    irq = IRQ_PCIE_MSI0 + pos;
+
+    printk("0: pos %d , irq %d \n", pos, irq);
+    if (irq > NR_IRQS)
+        return -ENOSPC;
+    /* test_and_set_bit operates on 32-bits at a time */
+    if (test_and_set_bit(pos, msi_irq_in_use))
+        goto again;
+
+    P2XDBUG("1 : pos = %d, irq = %d\n", pos, irq);
+//  dynamic_irq_init(irq);
+
+    return irq; 
+}
+
+static void
+msi_destroy_irq(unsigned int irq)
+{
+    int pos = irq - IRQ_PCIE_MSI0;
+    P2XDBUG("\n");
+
+//  dynamic_irq_cleanup(irq);
+
+    clear_bit(pos, msi_irq_in_use);
+}
+
+extern void
+arch_teardown_msi_irq(unsigned int irq)
+{
+    P2XDBUG("\n");
+
+    msi_destroy_irq(irq);
+}
+
+int arch_setup_msi_irq(struct pci_dev *pdev, struct msi_desc *desc)
+{
+    int irq = msi_create_irq();
+    struct msi_msg msg;
+
+    P2XDBUG("\n");
+    if (irq < 0)
+        return irq;
+
+    irq_set_msi_desc(irq, desc);    
+
+    msg.address_hi = 0;
+    msg.address_lo = 0x1e6f00f0;
+
+    msg.data = irq - IRQ_PCIE_MSI0;
+    printk("msi.data = %d\n", msg.data);
+
+    write_msi_msg(irq, &msg);
+//  set_irq_chip_and_handler(irq, &ast_p2x_irq_chip, handle_simple_irq);
+    
+    ast_p2x_write_reg(ast_p2x_read_reg(AST_P2X_STATUS_CTRL) | P2X_RX_MSI_EN, AST_P2X_STATUS_CTRL);
+
+    return 0;
+}
+
+
+#endif  // CONFIG_PCI_MSI
+
+#define PCIE_IRQ_NUM 4
+
+extern void __init
+ast_init_p2x_irq(void)
+{
+    int i;
+    P2XDBUG("\n");
+
+    for(i=0;i<ARCH_NR_PCIE + AST_NUM_MSI_IRQS;i++) {
+        irq_set_chip_data(i + IRQ_PCIE_CHAIN_START, NULL);
+        irq_set_chip_and_handler(i + IRQ_PCIE_CHAIN_START, &ast_p2x_irq_chip, handle_level_irq);
+        set_irq_flags(i + IRQ_PCIE_CHAIN_START, IRQF_VALID);
+    }
+
+    irq_set_chained_handler(IRQ_P2X, ast_p2x_irq_handler);
+
+}
+
+static int __init ast_p2x_init(void)
+{
+    P2XDBUG("\n");
+    ast_p2x_virt_base = ioremap(AST_P2X_BASE , SZ_256);
+
+    ast_p2x_write_reg(P2X_DROP_DISABLE, AST_P2X_STATUS_CTRL);
+    ast_init_p2x_irq();
+    return 0;
+}
+
+subsys_initcall(ast_p2x_init);
+
