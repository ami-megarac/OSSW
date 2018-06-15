--- uboot/drivers/mmc/mmc.c	2013-07-23 19:58:13.000000000 +0800
+++ uboot.new/drivers/mmc/mmc.c	2015-07-23 10:20:08.768512000 +0800
@@ -326,6 +326,9 @@
 		return 0;
 	}
 
+#if !defined(CONFIG_PILOT4)
+	/* No need this in pilot since it supports AUTOCMD12 */
+
 	/* SPI multiblock writes terminate using a special
 	 * token, not a STOP_TRANSMISSION request.
 	 */
@@ -338,6 +341,7 @@
 			return 0;
 		}
 	}
+#endif
 
 	/* Waiting for the ready status */
 	if (mmc_send_status(mmc, timeout))
@@ -395,7 +399,8 @@
 
 	if (mmc_send_cmd(mmc, &cmd, &data))
 		return 0;
-
+#if !defined(CONFIG_PILOT4)
+	/* No need this in pilot since it supports AUTOCMD12 */
 	if (blkcnt > 1) {
 		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
 		cmd.cmdarg = 0;
@@ -405,7 +410,7 @@
 			return 0;
 		}
 	}
-
+#endif
 	return blkcnt;
 }
 
@@ -683,12 +688,13 @@
 
 	if (err)
 		return err;
-
+#if !defined(CONFIG_PILOT4)
 	/* Now check to see that it worked */
 	err = mmc_send_ext_csd(mmc, ext_csd);
 
 	if (err)
 		return err;
+#endif
 
 	/* No high-speed support */
 	if (!ext_csd[EXT_CSD_HS_TIMING])
@@ -1134,6 +1140,9 @@
 		case 6:
 			mmc->version = MMC_VERSION_4_5;
 			break;
+		case 7:
+			mmc->version = MMC_VERSION_5_0;
+			break;
 		}
 
 		/*
@@ -1185,6 +1194,16 @@
 		return err;
 
 	/* Restrict card's capabilities by what the host can do */
+#if defined(CONFIG_PILOT4)
+	// Pilot4 A2 evb mount eMMC which is compliant to 5.0 spec.
+	// Pilot4 eMMC host controller however is implemented as 4.51 spec.
+	// If eMMC run on 8bit mode the DS pin will be active as output pin and cause issue.
+	if (mmc->version == MMC_VERSION_5_0)
+	{
+		mmc->host_caps &= ~MMC_MODE_8BIT;
+		mmc->host_caps |= MMC_MODE_4BIT;
+	}
+#endif
 	mmc->card_caps &= mmc->host_caps;
 
 	if (IS_SD(mmc)) {
