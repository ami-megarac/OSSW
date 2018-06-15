--- linux_old/drivers/net/ethernet/pilot_tulip/media.c	2016-07-12 19:53:37.210089248 +0530
+++ linux/drivers/net/ethernet/pilot_tulip/media.c	2016-07-12 19:57:50.458087779 +0530
@@ -540,12 +540,12 @@
 #else
         phyid = tulip_mdio_read(dev, phy, MII_PHYSID2);
 #endif
-        if ((phyid == 0) || (phyid == 0xFFFF))  /* Possible NCSI */
-        {
-                printk(KERN_WARNING "tulip%d : Unable to detect PHY at user specfied ID %d\n",board_idx,phy-1);
-                tp->phys[board_idx]=INVALID_PHYID;
-                return;
-        }
+//        if ((phyid == 0) || (phyid == 0xFFFF))  /* Possible NCSI */
+//       {
+//            printk(KERN_WARNING "tulip%d : Unable to detect PHY at user specfied ID %d\n",board_idx,phy-1);
+//          tp->phys[board_idx]=INVALID_PHYID;
+//             return;
+//       }
 
 	/* OUI[23:22] are not represented in PHY ID registers */
 	mii_status = tulip_mdio_read(dev, phy, MII_PHYSID1);
