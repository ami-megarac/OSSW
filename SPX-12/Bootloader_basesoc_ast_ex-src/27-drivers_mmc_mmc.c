--- uboot/drivers/mmc/mmc.c	2015-08-13 14:06:53.554973600 +0800
+++ uboot.new/drivers/mmc/mmc.c	2015-08-24 10:55:57.888446000 +0800
@@ -578,20 +578,21 @@
 {
 	struct mmc_cmd cmd;
 	int timeout = 1000;
-	uint start;
 	int err;
 
 	mmc->op_cond_pending = 0;
-	start = get_timer(0);
+
 	do {
 		err = mmc_send_op_cond_iter(mmc, &cmd, 1);
 		if (err)
 			return err;
-		if (get_timer(start) > timeout)
-			return UNUSABLE_ERR;
-		udelay(100);
-	} while (!(mmc->op_cond_response & OCR_BUSY));
 
+		udelay(1000);
+	} while (!(mmc->op_cond_response & OCR_BUSY) && timeout--);
+
+	if (timeout <= 0)
+		return UNUSABLE_ERR;
+		
 	if (mmc_host_is_spi(mmc)) { /* read OCR for spi */
 		cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
 		cmd.resp_type = MMC_RSP_R3;
@@ -1133,6 +1134,8 @@
 			break;
 		case 6:
 			mmc->version = MMC_VERSION_4_5;
+		case 7:
+			mmc->version = MMC_VERSION_5_0;
 			break;
 		}
 
@@ -1356,6 +1359,7 @@
 int mmc_start_init(struct mmc *mmc)
 {
 	int err;
+	int retries = 5;
 
 	if (mmc_getcd(mmc) == 0) {
 		mmc->has_init = 0;
@@ -1391,7 +1395,12 @@
 
 	/* If the command timed out, we check for an MMC card */
 	if (err == TIMEOUT) {
-		err = mmc_send_op_cond(mmc);
+		// Force resend CMD1 to deal with Card busy 
+		do{
+			err = mmc_send_op_cond(mmc);
+
+			retries--;
+		}while(!(retries <= 0));
 
 		if (err && err != IN_PROGRESS) {
 			printf("Card did not respond to voltage select!\n");
