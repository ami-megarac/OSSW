--- linux-3.14.17/drivers/mmc/core/mmc.c	2018-03-30 17:02:38.847542999 +0800
+++ linux/drivers/mmc/core/mmc.c	2018-03-30 17:02:54.171200999 +0800
@@ -294,7 +294,7 @@
 	}
 
 	card->ext_csd.rev = ext_csd[EXT_CSD_REV];
-	if (card->ext_csd.rev > 7) {
+	if (card->ext_csd.rev > 8) {
 		pr_err("%s: unrecognised EXT_CSD revision %d\n",
 			mmc_hostname(card->host), card->ext_csd.rev);
 		err = -EINVAL;
