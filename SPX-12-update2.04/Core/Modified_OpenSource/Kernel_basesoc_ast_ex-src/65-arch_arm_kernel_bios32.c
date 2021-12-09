--- linux/arch/arm/kernel/bios32.c	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/arch/arm/kernel/bios32.c	2016-10-31 14:07:11.198107800 +0800
@@ -549,6 +549,7 @@
 		pci_bus_add_devices(bus);
 	}
 }
+EXPORT_SYMBOL(pci_common_init_dev);
 
 #ifndef CONFIG_PCI_HOST_ITE8152
 void pcibios_set_master(struct pci_dev *dev)
