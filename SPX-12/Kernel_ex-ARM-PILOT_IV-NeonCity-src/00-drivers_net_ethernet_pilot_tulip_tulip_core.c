--- linux/drivers/net/ethernet/pilot_tulip/tulip_core.c	2016-02-25 16:19:24.335726849 -0500
+++ linux.new/drivers/net/ethernet/pilot_tulip/tulip_core.c	2016-02-25 16:14:09.514165741 -0500
@@ -324,12 +324,12 @@
 #else
     	phyid = tulip_mdio_read(dev, tp->mii.phy_id, MII_PHYSID2);
 #endif
-	if ((phyid == 0) || (phyid == 0xFFFF))	/* Possible NCSI */
-	{
-		tp->phys[tp->phy_index] = INVALID_PHYID;
-		tp->mii.phy_id = INVALID_PHYID;
-		return -2;
-	}
+	//if ((phyid == 0) || (phyid == 0xFFFF))	/* Possible NCSI */
+	//{
+	//	tp->phys[tp->phy_index] = INVALID_PHYID;
+	//	tp->mii.phy_id = INVALID_PHYID;
+	//	return -2;
+	//}
 
   	if (tulip_debug > 4)
 		printk("%s : PHY ID  = 0x%x\n",dev->name,tp->phys[tp->phy_index]-1);
