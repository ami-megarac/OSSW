--- linux-3.14.17/drivers/net/ethernet/pilot_tulip/tulip_core.c	2017-09-26 16:35:09.425945219 +0800
+++ linux-3.14.17.new/drivers/net/ethernet/pilot_tulip/tulip_core.c	2017-09-26 18:16:12.112203795 +0800
@@ -516,6 +516,12 @@
 	int next_tick = 3*HZ;
 //	u32 reg;
 	int i;
+#ifdef  PILOT_PAUSE_FR_ENABLE
+	u16 *mac_addrs = (u16 *)dev->dev_addr;
+	unsigned int MACL;
+	unsigned int MACH;
+#endif
+	volatile int timeout = 1000000;
 
 #ifdef CONFIG_TULIP_NAPI
 	napi_enable(&tp->napi);
@@ -531,10 +537,26 @@
 
 	/* Reset Pilot-III Chip */
 	iowrite32((ioread32(ioaddr + CSR0)|0x1), ioaddr + CSR0);
-        printk("Pilot-III: Waiting for MAC soft reset to be completed ...\n");
-	 while(ioread32(ioaddr + CSR0)&0x1 );
-	 printk("Pilot-III: MAC soft reset completed\n");
+        printk("Pilot-IV: Waiting for MAC soft reset to be completed ...\n");
+
+	while(ioread32(ioaddr + CSR0)&0x1 )
+	{
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printk("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+
+#ifdef CONFIG_PANIC_AND_BAIL_OUT
+			panic("");
+#endif
+			break;
+		}
+		udelay(1);
+	}
+
+	 printk("Pilot-IV: MAC soft reset completed\n");
 	 
+
  	/* Reset the chip, holding bit 0 set at least 50 PCI cycles. */
 //	iowrite32(0x00000001, ioaddr + CSR0);
 //	pci_read_config_dword(tp->pdev, PCI_COMMAND, &reg);  /* flush write */
@@ -551,6 +573,32 @@
 	iowrite32((ioread32(ioaddr + CSR0)|0x2), ioaddr + CSR0);
 #endif
 	iowrite32( (CSR11_TIM), (ioaddr + CSR11)); 
+
+#ifdef PILOT_PAUSE_FR_ENABLE
+	switch (tp->phy_model) {
+	case PHY_MARVELL:
+		MACL=mac_addrs[0];
+		MACL|=(mac_addrs[1]<<16);
+		MACH=mac_addrs[2];
+
+		iowrite32(MACL, (ioaddr + CSR16));
+		iowrite16(MACH, (ioaddr + CSR17));
+		iowrite32(PAUSE_QUANTA_TIME, (ioaddr + CSR18) );
+		iowrite32(PAUSE_THRESHOLD_VALUE, (ioaddr + CSR19) );
+		iowrite32( (ioread32(ioaddr + CSR20) | FLOW_CTRL_ENBALE), (ioaddr+ CSR20) );
+		iowrite32( (ioread32(ioaddr + CSR20) | (TX_PAUSE_ENABLE | RX_PAUSE_ENABLE)), (ioaddr+ CSR20) );
+		tulip_mdio_write (dev, tp->mii.phy_id & 0x1f, MII_ADVERTISE, (tulip_mdio_read(dev, tp->mii.phy_id & 0x1f, MII_ADVERTISE) | ADVERTISE_PAUSE_CAP));
+		break;
+
+	case PHY_MICREL:
+		//Pause frames will not be supported for MICREL phy. Please read Errata of Micrel Phy
+		break;
+
+	default:
+		break;
+	}
+#endif
+
 /* Pilot-III Addition End */
 	if (tulip_debug > 1)
 		printk(KERN_DEBUG "%s: tulip_up(), irq==%d.\n", dev->name, dev->irq);
@@ -1220,6 +1268,97 @@
         return mii_link_ok(&nic->mii);
 }
 
+static void tulip_get_pauseparam(struct net_device *netdev, struct ethtool_pauseparam *pause)
+{
+#if PILOT_PAUSE_FR_ENABLE
+	struct tulip_private *nic = netdev_priv(netdev);
+
+	if ( nic->sym_advertise == TULIP_FC_RX_PAUSE) {
+		pause->rx_pause = 1;
+		pause->tx_pause = 0;
+	} else if ( nic->sym_advertise == TULIP_FC_TX_PAUSE) {
+		pause->rx_pause = 0;
+		pause->tx_pause = 1;
+	} else if ( nic->sym_advertise == TULIP_FC_FULL) {
+		pause->rx_pause = 1;
+		pause->tx_pause = 1;
+	} else {
+		pause->rx_pause = 0;
+		pause->tx_pause = 0;
+	}
+#endif
+}
+
+static int tulip_set_pauseparam(struct net_device *netdev, struct ethtool_pauseparam *pause)
+{
+#if PILOT_PAUSE_FR_ENABLE
+	struct tulip_private *nic = netdev_priv(netdev);
+	struct ethtool_cmd ecmd;
+	unsigned int csr20;
+	u32 advert = 0, tmp;
+
+	switch (nic->phy_model) {
+
+	case PHY_MICREL:
+		printk(KERN_INFO "Pause frames will not be supported for MICREL phy. Please read Errata of Micrel Phy\n");
+		return -EOPNOTSUPP;
+
+	default:
+		break;
+	}
+
+	ethtool_override_set_pauseparam (nic->phy_index,netdev,pause);
+	if (nic->full_duplex)
+	{
+		mii_ethtool_gset(&(nic->mii),&ecmd);
+		csr20 = ioread32(nic->base_addr + CSR20);
+		advert = tulip_mdio_read(netdev, nic->mii.phy_id, MII_ADVERTISE);
+		tmp=advert;
+
+		if (pause->rx_pause)
+		{			
+			if (pause->tx_pause)
+			{	
+				nic->sym_advertise = TULIP_FC_FULL;
+				csr20 |= TX_PAUSE_ENABLE;
+				csr20 |= RX_PAUSE_ENABLE;
+				tmp |= ADVERTISE_PAUSE_CAP;
+				tmp &= ~ADVERTISE_PAUSE_ASYM;
+			}			
+			else
+			{
+				nic->sym_advertise = TULIP_FC_RX_PAUSE;
+				csr20 &= ~TX_PAUSE_ENABLE;
+				csr20 |= RX_PAUSE_ENABLE;
+				tmp |= ADVERTISE_PAUSE_CAP;
+	                        tmp |= ADVERTISE_PAUSE_ASYM;
+			}
+		}			
+		else if (pause->tx_pause)
+		{
+			nic->sym_advertise = TULIP_FC_TX_PAUSE;
+			csr20 |= TX_PAUSE_ENABLE;
+			csr20 &= ~RX_PAUSE_ENABLE;
+			tmp &= ~ADVERTISE_PAUSE_CAP;
+	                tmp |= ADVERTISE_PAUSE_ASYM;
+		}			
+		else
+		{
+			nic->sym_advertise = TULIP_FC_NONE;
+			csr20 &= ~TX_PAUSE_ENABLE;
+			csr20 &= ~RX_PAUSE_ENABLE;
+			tmp &= ~ADVERTISE_PAUSE_CAP;
+	                tmp &= ~ADVERTISE_PAUSE_ASYM;
+		}			
+		iowrite32(csr20, (nic->base_addr+ CSR20));
+		if (advert != tmp) {
+			tulip_mdio_write(netdev, nic->mii.phy_id, MII_ADVERTISE, tmp);
+			nic->mii.advertising = tmp;
+		}
+	}	
+#endif	
+	return 0;			
+}	
 
 static const struct ethtool_ops ops = {
 	.get_drvinfo = tulip_get_drvinfo,
@@ -1227,6 +1366,8 @@
 	.nway_reset	 = tulip_nway_reset,
 	.get_settings = tulip_get_settings,
 	.set_settings = tulip_set_settings,
+	.get_pauseparam = tulip_get_pauseparam,
+	.set_pauseparam = tulip_set_pauseparam,
 };
 
 
