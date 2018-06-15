--- linux/arch/arm/include/asm/mach/pci.h	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/arch/arm/include/asm/mach/pci.h	2016-10-31 14:11:38.748731600 +0800
@@ -73,7 +73,7 @@
 /*
  * Call this with your hw_pci struct to initialise the PCI system.
  */
-void pci_common_init_dev(struct device *, struct hw_pci *);
+extern void pci_common_init_dev(struct device *, struct hw_pci *);
 
 /*
  * Compatibility wrapper for older platforms that do not care about
