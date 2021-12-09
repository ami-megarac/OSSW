diff -Naur uboot_old/arch/arm/cpu/astcommon/ast_eth.c uboot_new/arch/arm/cpu/astcommon/ast_eth.c
--- uboot_old/arch/arm/cpu/astcommon/ast_eth.c	2016-11-25 17:52:41.724745964 +0800
+++ uboot_new/arch/arm/cpu/astcommon/ast_eth.c	2016-11-25 18:14:30.020745964 +0800
@@ -101,6 +101,21 @@
 #define REALTEK_RTL82X1X_OUI	0x000732
 #define MICREL_KS8721_PHYID_OUI        0x000885 
 
+/* Broadcom BCM54610 moel name */
+#define BCM54610_MODEL_NAME	0x16
+
+/* addresses of Broadcom BCM54610 PHY specific registers */
+#define BROADCOM_BCM54610_REG_ADDR_AUX_CTRL_STAT     0x19
+
+/* Bits of Broadcom BCM54610 Auxiliary Control/Status Register - Register 0x19 */
+#define BROADCOM_BCM54610_REG_AUX_CTRL_STAT_MASK	   0x0700
+#define BROADCOM_BCM54610_REG_AUX_CTRL_STAT_FULL_1000  0x0700
+#define BROADCOM_BCM54610_REG_AUX_CTRL_STAT_HALF_1000  0x0600
+#define BROADCOM_BCM54610_REG_AUX_CTRL_STAT_FULL_100   0x0500
+#define BROADCOM_BCM54610_REG_AUX_CTRL_STAT_HALF_100   0x0300
+#define BROADCOM_BCM54610_REG_AUX_CTRL_STAT_FULL_10    0x0200
+#define BROADCOM_BCM54610_REG_AUX_CTRL_STAT_HALF_10    0x0100
+
 /* Descriptor bits. */
 #define TXDMA_OWN	0x80000000	/* TX Own Bit */
 #define RXPKT_RDY	0x80000000	/* RX Own Bit */
@@ -360,14 +375,60 @@
 	switch (phy_oui) {
 	case BROADCOM_BCM5221_OUI:
 	case BROADCOM_BCM5241_OUI:
-		phy_reg = ast_eth_read_phy_reg(dev, 0x18, phy_addr);
-
-		if ((phy_reg & 0x0001) == 0)
-			ctrl_reg &= ~MACCR_FULLDUP;
-		if ((phy_reg & 0x0002) == 0)
-			ctrl_reg &= ~MACCR_SPEED_100M_MODE;
-		break;
-	 case BROADCOM_BCM5461_OUI: 
+		if(phy_model != BCM54610_MODEL_NAME)
+		{
+			phy_reg = ast_eth_read_phy_reg(dev, 0x18, phy_addr);
+
+			if ((phy_reg & 0x0001) == 0)
+				ctrl_reg &= ~MACCR_FULLDUP;
+			if ((phy_reg & 0x0002) == 0)
+				ctrl_reg &= ~MACCR_SPEED_100M_MODE;
+			break;
+		}
+		else
+		{
+			ast_eth_write_phy_reg(dev, 0x17, phy_addr, 0xd34);
+		    phy_reg = ast_eth_read_phy_reg(dev, 0x15, phy_addr);
+		    phy_reg |= 0x02;
+		 	ast_eth_write_phy_reg(dev, 0x15, phy_addr, phy_reg);
+		    ast_eth_write_phy_reg(dev, 0x17, phy_addr, 0x0);
+		    ast_eth_write_phy_reg(dev, 0x1C, phy_addr, 0x8c00);
+		    ast_eth_write_phy_reg(dev, 0x18, phy_addr, 0xf0e7);
+			
+			phy_reg = ast_eth_read_phy_reg(dev, BROADCOM_BCM54610_REG_ADDR_AUX_CTRL_STAT, phy_addr);
+			phy_reg = phy_reg &  BROADCOM_BCM54610_REG_AUX_CTRL_STAT_MASK; //phy status mask 
+		    switch (phy_reg) {
+		    	case BROADCOM_BCM54610_REG_AUX_CTRL_STAT_FULL_1000:
+		        	phy_duplex = DUPLEX_FULL;
+		            phy_speed = SPEED_1000M;
+		    		break;
+		        case BROADCOM_BCM54610_REG_AUX_CTRL_STAT_HALF_1000:
+		            phy_duplex = DUPLEX_HALF;
+		            phy_speed = SPEED_1000M;
+		         	break;
+		        case BROADCOM_BCM54610_REG_AUX_CTRL_STAT_FULL_100:
+		            phy_duplex = DUPLEX_FULL;
+		            phy_speed = SPEED_100M;
+		         	break;
+		        case BROADCOM_BCM54610_REG_AUX_CTRL_STAT_HALF_100:
+		            phy_duplex = DUPLEX_HALF;
+		            phy_speed = SPEED_100M;
+					break;
+		        case BROADCOM_BCM54610_REG_AUX_CTRL_STAT_FULL_10:
+		            phy_duplex = DUPLEX_FULL;
+		            phy_speed = SPEED_10M;
+		         	break;
+		        case BROADCOM_BCM54610_REG_AUX_CTRL_STAT_HALF_10:
+		            phy_duplex = DUPLEX_HALF;
+		            phy_speed = SPEED_10M;
+		         	break;
+		        default:
+		         	phy_duplex = DUPLEX_FULL;
+		            phy_speed = SPEED_100M;
+		         	break;
+			}
+		}
+	case BROADCOM_BCM5461_OUI: 
     case BROADCOM_BCM54612E_OUI: 
     case BROADCOM_BCM5421_OUI:
     	ast_eth_write_phy_reg(dev, 0x17, phy_addr, 0xd34); 
