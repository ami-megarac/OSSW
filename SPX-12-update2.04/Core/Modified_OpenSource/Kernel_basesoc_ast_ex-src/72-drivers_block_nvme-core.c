--- linux/drivers/block/nvme-core.c	2016-11-07 12:23:05.057609421 -0500
+++ linux.new/drivers/block/nvme-core.c	2016-11-07 12:17:25.129600845 -0500
@@ -501,6 +501,9 @@
 	int length = 0, nsegs = 0, split_len = bio->bi_iter.bi_size;
 	int first = 1;
 
+        bvprv.bv_page=NULL;
+        bvprv.bv_len=0;
+        bvprv.bv_offset=0;
 	if (nvmeq->dev->stripe_size)
 		split_len = nvmeq->dev->stripe_size -
 			((bio->bi_iter.bi_sector << 9) &
@@ -2538,6 +2541,7 @@
 #define nvme_slot_reset NULL
 #define nvme_error_resume NULL
 
+/*
 static int nvme_suspend(struct device *dev)
 {
 	struct pci_dev *pdev = to_pci_dev(dev);
@@ -2558,6 +2562,7 @@
 	}
 	return 0;
 }
+*/
 
 static SIMPLE_DEV_PM_OPS(nvme_dev_pm_ops, nvme_suspend, nvme_resume);
 
