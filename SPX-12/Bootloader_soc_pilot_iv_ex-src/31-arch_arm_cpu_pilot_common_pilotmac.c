--- uboot/arch/arm/cpu/pilotcommon/pilotmac.c	2017-09-27 16:15:49.139420588 +0800
+++ u-boot-2013.07.new/arch/arm/cpu/pilotcommon/pilotmac.c	2017-09-27 16:42:06.005901101 +0800
@@ -186,6 +186,7 @@
 	unsigned int ioaddr;
 	AST_ETH_PRIV_DATA *hw;
 	u32 val;
+	volatile int timeout = 1000000;
 
 #ifdef  ASTMAC_DEBUG
 	printf("ASTMAC:(%s):Close\n",dev->name);
@@ -228,6 +229,17 @@
 			/* Patch : if Xmit is stopped and Receive buffer is available. exit */
 			if (((val & CSR5_TS_MSK) == CSR5_TS_ST) && (!(val & CSR5_RU)))
 				break;
+
+			timeout--;
+			if(timeout <= 0) 
+			{
+				printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+#ifdef CONFIG_PANIC_AND_BAIL_OUT
+				panic("");
+#endif
+				break;
+			}
+			udelay(1);
 		}
 		while (val != (CSR5_TS_ST| CSR5_RS_ST));
 #ifdef  ASTMAC_DEBUG
@@ -244,7 +256,8 @@
 ast_mac100_reset(struct eth_device *dev)
 {
 
-//	unsigned int ioaddr;
+	unsigned int ioaddr;
+	volatile int timeout = 1000000;
 	u32 val;
 	AST_ETH_PRIV_DATA *hw;
 
@@ -253,7 +266,7 @@
 #endif
 
 	hw = (AST_ETH_PRIV_DATA *)dev->priv;
-//	ioaddr = hw->hw_addr;
+	ioaddr = hw->hw_addr;
 
 	/* Send a Software Reset */ /* Errata for PIlot-II */
 //	outl(CSR0_SWR, ioaddr + AST_MAC_CSR0 );
@@ -268,6 +281,24 @@
 	val = inl(hw->hw_addr+AST_MAC_CSR5);
 	val |= 0x1FFFF;
 	outl(val,hw->hw_addr+AST_MAC_CSR5);
+	udelay(100);    /* Wait for stabilize */
+
+	outl(CSR0_SWR, ioaddr + AST_MAC_CSR0 );
+ 	val=inl(ioaddr + AST_MAC_CSR0 );
+ 	while(val&CSR0_SWR)
+ 	{
+		val=inl(ioaddr + AST_MAC_CSR0 );
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+#ifdef CONFIG_PANIC_AND_BAIL_OUT
+			panic("");
+#endif
+			break;
+		}
+		udelay(1);
+ 	}
 
 	return;
 }
@@ -289,8 +320,8 @@
 		return -1;
 	}
 
-	/* Align to the Dword Boundary */
-	lp->rx_descs =  (RECV_DESC *)((int)(((char *)lp->rx_descs)+sizeof(RECV_DESC)-1)&0xfffffff0);
+	/* Align to the 16 byte Boundary */
+	lp->rx_descs = (RECV_DESC *)((int)(((char *)lp->rx_descs)+sizeof(RECV_DESC)-1)&0xfffffff0);
 
 	/* Get Physical Address for DMA */
 	lp->rx_descs_dma = virt2phys(lp->rx_descs);
@@ -307,8 +338,8 @@
 		return -1;
 	}
 
-	/* Align to the Dword Boundary */
-	lp->rx_buf =  (char *)((int)((((char *)lp->rx_buf)+3))&0xfffffff0);
+	/* Align to the 64 bit Boundary */
+	lp->rx_buf = (char *)((int)((((char *)lp->rx_buf)+7))&0xfffffff8);
 
 	/* Get Physical Address for DMA */
 	lp->rx_buf_dma = virt2phys(lp->rx_buf);
@@ -323,8 +354,9 @@
 		return -1;
 	}
 
-	/* Align to the Dword Boundary */
-	lp->tx_descs =  (XMIT_DESC *)((int)(((char *)lp->tx_descs)+sizeof(XMIT_DESC)-1)&0xfffffff0);
+	/* Align to the 16 byte Boundary */
+	lp->tx_descs = (XMIT_DESC *)((int)(((char *)lp->tx_descs)+sizeof(XMIT_DESC)-1)&0xfffffff0);
+
 
 	/* Get Physical Address for DMA */
 	lp->tx_descs_dma = virt2phys(lp->tx_descs);
@@ -340,8 +372,8 @@
 		return -1;
 	}
 
-	/* Align to the Dword Boundary */
-	lp->tx_buf =  (char *)((int)((((char *)lp->tx_buf)+3))&0xfffffff0);
+	/* Align to the 64 bit Boundary */
+	lp->tx_buf = (char *)((int)((((char *)lp->tx_buf)+7))&0xfffffff8);
 
 	/* Get Physical Address for DMA */
 	lp->tx_buf_dma = virt2phys(lp->tx_buf);
@@ -355,8 +387,8 @@
 		return -1;
 	}
 
-	/* Align to the Dword Boundary */
-	lp->setup_descs =  (XMIT_DESC *)((int)(((char *)lp->setup_descs)+sizeof(XMIT_DESC)-1)&0xfffffff0);
+	/* Align to the 16 byte Boundary */
+	lp->setup_descs = (XMIT_DESC *)((int)(((char *)lp->setup_descs)+sizeof(XMIT_DESC)-1)&0xfffffff0);
 
 	/* Get Physical Address for DMA */
 	lp->setup_descs_dma = virt2phys(lp->setup_descs);
@@ -365,15 +397,16 @@
 	memset((void *)lp->setup_descs, 0, sizeof(XMIT_DESC));
 
 	/* Allocate Memory for Buffers */
-	lp->setup_frame = malloc(SETUP_FRAME_SIZE+3);
+	lp->setup_frame = malloc(SETUP_FRAME_SIZE+7);
+
 	if (lp->setup_frame == NULL)
 	{
 		printf("ERROR:Setup Ring  Buffer allocation error\n");
 		return -1;
 	}
 
-	/* Align to the Dword Boundary */
-	lp->setup_frame =  (char *)((int)((((char *)lp->setup_frame)+3))&0xfffffff0);
+	/* Align to the 64 bit Boundary */
+	lp->setup_frame = (char *)((int)((((char *)lp->setup_frame)+7))&0xfffffff8);
 
 	/* Get Physical Address for DMA */
 	lp->setup_frame_dma = virt2phys(lp->setup_frame);
@@ -386,7 +419,7 @@
 	printf("lp->tx_descs    = %x, lp->tx_descs_dma    = %x\n", lp->tx_descs, lp->tx_descs_dma);
 	printf("lp->tx_buf      = %x, lp->tx_buf_dma      = %x\n", lp->tx_buf, lp->tx_buf_dma);
 	printf("lp->setup_descs = %x, lp->setup_descs_dma = %x\n", lp->setup_descs, lp->setup_descs_dma);
-	printf("lp->setup_frame = %x, lp->setup_frame_dma = %x\n", lp->setup_frame,lp->setup_frame_dma);
+	printf("lp->setup_frame = %x, lp->setup_frame_dma = %x\n", lp->setup_frame, lp->setup_frame_dma);
 #endif
 
 	return 0;
@@ -570,6 +603,8 @@
     unsigned char addr;
     int devnum;
     int data;
+    volatile int timeout = 1000000;
+
     devnum = GetDevNum(dev->name);
     u16 phyid=0;
     //int to=0x100;
@@ -615,10 +650,8 @@
     }
     else if((((phyid&0x3f0)>>0x4)==0x21) || (((phyid&0x3f0)>>0x4)==0x22))
     {
-#ifdef  ASTMAC_DEBUG
         printf("Found Mircel phy %x %x\n",devnum,phyid);
-#endif
-
+#if 0
         //Micrel Tx clk skew programming
         data=0x104;
         ast_miiphy_write(dev->name, addr,0xb,data);
@@ -645,6 +678,31 @@
         ast_miiphy_read(dev->name, addr,0x9,(unsigned short *)&data);
         data|=(0x3<<11);
         ast_miiphy_write(dev->name, addr,0x9, data);
+#else
+
+        //set PILOT4 MACRGMIICTL register to 0x200, Otherwise we will see 
+	      //Tx issues on MAC0 in 1000Mbps speed
+	*((volatile unsigned long *)(SE_SYS_CLK_VA_BASE + 0x88)) = 0x200;
+
+        //Issue soft reset to PHY and wait for the PHY to come out of soft reset
+	ast_miiphy_write(dev->name, addr, 0x0, 0x8000);
+
+	ast_miiphy_read(dev->name, addr, 0x0, (unsigned short *)&data);
+	while((data & 0x8000)==0x8000)
+	{
+		ast_miiphy_read(dev->name, addr, 0x0,(unsigned short *)&data);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+#ifdef CONFIG_PANIC_AND_BAIL_OUT
+			panic("");
+#endif
+			break;
+		}
+		udelay(1);
+	}
+#endif
 
         //restart auto negotiation
         ast_miiphy_read(dev->name, addr,0x0,(unsigned short *)&data);
@@ -661,8 +719,14 @@
 #ifdef  ASTMAC_DEBUG
         if(link_chk_iterations==0x0)
         {
-            printf("Link Is Down For Device%x\n",devnum);
+            ast_miiphy_read(dev->name, addr, 0x1, (unsigned short *)&data);
+            printf("Link Is Down For Device%x data=%x\n",devnum,data);
         }
+        else
+        {
+            ast_miiphy_read(dev->name, addr, 0x1, (unsigned short *)&data);
+       	    printf("Link Is UP For Device%x data=%x\n",devnum, data);
+	      }
 #endif
     }
     else if( (((phyid&0x3f0)>>0x4)==0x0f)|| (((phyid&0x3f0)>>0x4)==0x0c) )
@@ -672,15 +736,28 @@
 #endif
         phyid=0x8000;
         ast_miiphy_write(dev->name, addr, 0x0, phyid);
-        while(phyid==0x8000)
-        {
-            ast_miiphy_read(dev->name, addr, 0x0,&phyid);
-        }
+
+	timeout = 1000000;
+	while(phyid==0x8000)
+	{
+		ast_miiphy_read(dev->name, addr, 0x0,&phyid);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+#ifdef CONFIG_PANIC_AND_BAIL_OUT
+			panic("");
+#endif
+			break;
+		}
+		udelay(1);
+	}
+
 #ifdef  ASTMAC_DEBUG
         printf("phy reset is done and autoneg started\n");
 #endif
 
-        ast_miiphy_write(dev->name, addr, 0x0, 0x1200);
+            ast_miiphy_write(dev->name, addr, 0x0, 0x1200);
     }
     else if( ((phyid&0x3f0)>>0x4) == 0x1D )
    {
