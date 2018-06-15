--- linux/drivers/net/ethernet/pilot_tulip/tulip_core.c	2017-02-16 13:10:43.933890934 +0530
+++ linux_modified/drivers/net/ethernet/pilot_tulip/tulip_core.c	2017-02-17 12:08:55.977708272 +0530
@@ -559,7 +559,11 @@
 
 #ifdef PILOT_PAUSE_FR_ENABLE
 	switch (tp->phy_model) {
-	case PHY_MARVELL:
+	case PHY_MICREL:
+		//Pause frames will not be supported for MICREL phy. Please read Errata of Micrel Phy
+		break;
+
+	default:
 		MACL=mac_addrs[0];
 		MACL|=(mac_addrs[1]<<16);
 		MACH=mac_addrs[2];
@@ -572,13 +576,6 @@
 		iowrite32( (ioread32(ioaddr + CSR20) | (TX_PAUSE_ENABLE | RX_PAUSE_ENABLE)), (ioaddr+ CSR20) );
 		tulip_mdio_write (dev, tp->mii.phy_id & 0x1f, MII_ADVERTISE, (tulip_mdio_read(dev, tp->mii.phy_id & 0x1f, MII_ADVERTISE) | ADVERTISE_PAUSE_CAP));
 		break;
-
-	case PHY_MICREL:
-		//Pause frames will not be supported for MICREL phy. Please read Errata of Micrel Phy
-		break;
-
-	default:
-		break;
 	}
 #endif
 
