--- uboot/drivers/mmc/mmc.c	2015-08-13 14:06:53.554973600 +0800
+++ uboot.new/drivers/mmc/mmc.c	2015-08-24 10:55:57.888446000 +0800
@@ -557,12 +557,18 @@
 	struct mmc_cmd cmd;
 	int err, i;
 
-	/* Some cards seem to need this */
-	mmc_go_idle(mmc);
 
  	/* Asking to the card its capabilities */
 	mmc->op_cond_pending = 1;
 	for (i = 0; i < 2; i++) {
+            		/* Some cards seem to need this idle function, and following CMD1 will send different argument, so we need put the idle function into this for loop.
+		*  Because mmc ver. 09 and emmc 5.1 spec has defined. before to send  CMD1 we must be let MMC Card in the idle state . 
+            		*/
+            		mmc_go_idle(mmc);
+            		/*
+            		*	i = 0 ; CMD1 + (arg = 0)
+            		*	i = 1 ; CMD1 + (arg = 300000)// seems want setting card votage to 3.3 V by CMD1. 
+            		*/
 		err = mmc_send_op_cond_iter(mmc, &cmd, i != 0);
 		if (err)
 			return err;
@@ -578,20 +584,22 @@
 {
 	struct mmc_cmd cmd;
 	int timeout = 1000;
-	uint start;
 	int err;
 
 	mmc->op_cond_pending = 0;
-	start = get_timer(0);
+	/* Because mmc ver. 09 and emmc 5.1 spec has defined. before to send  CMD1 we must be let MMC Card in the idle state .  */
+	mmc_go_idle(mmc);
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
@@ -1133,6 +1141,8 @@
 			break;
 		case 6:
 			mmc->version = MMC_VERSION_4_5;
+		case 7:
+			mmc->version = MMC_VERSION_5_0;
 			break;
 		}
 
@@ -1356,6 +1366,7 @@
 int mmc_start_init(struct mmc *mmc)
 {
 	int err;
+	int retries = 5;
 
 	if (mmc_getcd(mmc) == 0) {
 		mmc->has_init = 0;
@@ -1391,7 +1402,12 @@
 
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
