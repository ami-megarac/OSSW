--- uboot/arch/arm/cpu/pilotcommon/pilotmmc.c	2015-07-22 19:49:19.959203771 +0800
+++ uboot.new/arch/arm/cpu/pilotcommon/pilotmmc.c	2015-07-23 10:55:00.658779000 +0800
@@ -0,0 +1,1006 @@
+#include <common.h>
+#include <asm/errno.h>
+#include <malloc.h>
+#include <mmc.h>
+#include "pilotmmc.h"
+
+
+static void sd_iowrite32(void * sd_no, u32 OutAddress, u32 Value)
+{
+	u32 sd = (u32)sd_no;
+	OutAddress += (SDHC_BASE_ADDR + (sd * 0x1000));
+	*(volatile u32 *) OutAddress = Value;
+}
+
+static u32 sd_ioread32(void * sd_no, u32 InAddress)
+{
+	volatile u32 temp=0;
+	u32 sd = (u32)sd_no;
+
+	InAddress += (SDHC_BASE_ADDR + (sd * 0x1000));
+	temp = *(volatile u32 *) InAddress;
+	return temp;
+}
+
+
+static void init_port(struct mmc *mmc)
+{
+	// Reset SDHC controller
+    u32 sd = (u32)mmc->priv;
+
+	volatile u32 EmmcCtlReg;
+	volatile int timeout = 1000000;
+	if(sd == 0){
+    	*(volatile u32*)(SYSSRERL2) = 0x1 << 30;       //SDHC0
+        *(volatile u32*)(SYSSRERH2) = 0x0;
+    }else if (sd == 1){
+    	*(volatile u32*)(SYSSRERL2) = 0x0;
+        *(volatile u32*)(SYSSRERH2) = 0x1; //SDHC1
+    }else if (sd == 2){
+    	*(tPVU32)(SYSSRERL2) = 0x0;
+        *(tPVU32)(SYSSRERH2) = 0x1 << 5; //EMMC
+    } else{
+    	return;
+    }
+
+	*(tPVU32)(SYSRCR2) = 0xD01;
+
+	udelay(5000);
+    while ( (*(tPVU32)(SYSRCR2) & 0x1) == 0x1)
+	{
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}
+	timeout = 1000000;
+
+	// Clear control2
+	do
+	{
+		EmmcCtlReg = sd_ioread32(mmc->priv, SDHCI_LMEMCTL);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	} while(EmmcCtlReg != 0x3010000);
+    sd_iowrite32(mmc->priv, SDHCI_ACMD12_ERR, 0x00); 
+	sd_iowrite32(mmc->priv, SDHCI_CLOCK_CONTROL, 0xFA5);
+	
+	timeout = 1000000;
+    do{
+		sd_iowrite32(mmc->priv, SDHCI_TXDTTMR, 0x1854232);
+        EmmcCtlReg = sd_ioread32(mmc->priv, SDHCI_TXDTTMR);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+    }while(EmmcCtlReg != 0x1854232);
+	
+	timeout = 1000000;
+	do{
+		sd_iowrite32(mmc->priv, SDHCI_RXDTTMR, 0x7FFFFFFF);
+        EmmcCtlReg = sd_ioread32(mmc->priv, SDHCI_RXDTTMR);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+    }while(EmmcCtlReg != 0x7FFFFFFF);
+	
+	timeout = 1000000;
+	// Reset to normal 400Khz mode
+    do{
+		sd_iowrite32(mmc->priv, SDHCI_FSBLK_CTRL, 0x0);
+        EmmcCtlReg = sd_ioread32(mmc->priv, SDHCI_FSBLK_CTRL);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}while(EmmcCtlReg != 0x0);
+
+	timeout = 1000000;
+	// increase the Ncr timer
+    do{
+		sd_iowrite32(mmc->priv, SDHCI_RSPTMR, 0xFF);
+		sd_ioread32(mmc->priv, SDHCI_RSPTMR);
+        EmmcCtlReg = sd_ioread32(mmc->priv, SDHCI_RSPTMR);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}while(EmmcCtlReg != 0xFF);
+
+	timeout = 1000000;
+	// Clearing the SDHC_CTL register bit-width etc.
+    do{
+		sd_iowrite32(mmc->priv, SDHCI_HOST_CONTROL, 0);
+        EmmcCtlReg = sd_ioread32(mmc->priv, SDHCI_HOST_CONTROL);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}while(EmmcCtlReg != 0x0);
+
+	timeout = 1000000;
+	// Program LMEM PG REG
+    do{
+		sd_iowrite32(mmc->priv, SDHCI_LMEMCTL, 0x2010000);
+        EmmcCtlReg = sd_ioread32(mmc->priv, SDHCI_LMEMCTL);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}while(EmmcCtlReg != 0x2010000);
+	
+	timeout = 1000000;
+    do{
+		sd_iowrite32(mmc->priv, SDHCI_LMEMCFG, 0x480);
+        EmmcCtlReg = sd_ioread32(mmc->priv, SDHCI_LMEMCFG);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}while(EmmcCtlReg != 0x480);
+
+    timeout = 1000000;
+	do{
+		sd_iowrite32(mmc->priv, SDHCI_SIGNAL_ENABLE, 0x1ffffff);
+        EmmcCtlReg = sd_ioread32(mmc->priv, SDHCI_SIGNAL_ENABLE);
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}while(EmmcCtlReg != 0x1ffffff);
+	
+	timeout = 1000000;
+    do{
+		sd_iowrite32(mmc->priv, SDHCI_INT_ENABLE, 0x1ffffff);
+        EmmcCtlReg = sd_ioread32(mmc->priv, SDHCI_INT_ENABLE);
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}while(EmmcCtlReg != 0x1ffffff);
+}
+
+
+int pilot_set_resp_type(tU32 resp_type)
+{
+	volatile tU32 cmdreg=0;
+
+	if(resp_type == MMC_RSP_NONE)
+	{
+		cmdreg = SD_CMD_RESP_NONE;
+	}
+	else if (resp_type == MMC_RSP_R1)
+	{
+		cmdreg =  SD_CMD_RESP_48 | SD_CMD_INDEX | SD_CMD_CRC;
+	}
+	else if (resp_type == MMC_RSP_R1b)
+	{
+		cmdreg = SD_CMD_RESP_48_BUSY | SD_CMD_INDEX | SD_CMD_CRC;
+	}
+	else if (resp_type == MMC_RSP_R2)
+	{
+		cmdreg = SD_CMD_RESP_136;
+	}
+	else if (resp_type == MMC_RSP_R3)
+	{
+		cmdreg = SD_CMD_RESP_48;
+	}
+	else if (resp_type == MMC_RSP_R6)
+	{
+		cmdreg = SD_CMD_RESP_48 | SD_CMD_CRC;
+	}
+	else if (resp_type == MMC_RSP_R7)
+	{
+		cmdreg = SD_CMD_RESP_48 | SD_CMD_CRC;
+	}
+	else
+	{
+		printf("Unknown Response type = %x\n", resp_type);
+	}
+	return cmdreg;
+}
+
+
+int pilot_sdh_request(struct mmc *mmc, struct mmc_cmd *cmd,
+		struct mmc_data *data)
+{
+	volatile u32 status;
+	volatile u32 cmdreg=0;
+	volatile u32 temp;
+	volatile int result = 0;
+	volatile u32 sdhci_clkctrl;
+	volatile u32 cnt=0;
+	volatile u32 make_sure;
+	volatile int timeout = 1000000;
+
+	cmd->response[0] = 0;
+	cmdreg = pilot_set_resp_type(cmd->resp_type);
+	cmdreg |= (cmd->cmdidx << CMDINDEX_POS);
+
+	if(data)
+	{
+		/* fatwrite command was issuing a mmc write with data->blocks=0 some times, 
+		 * which was making mmc writes to fail. Hence return if data->blocks=0 */
+		if(data->blocks == 0)
+			return 0;
+
+		cmdreg |= SD_CMD_DATA;
+	}
+
+	// Wait until the device is willing to accept commands
+	do {
+		status = sd_ioread32(mmc->priv, SDHCI_PRESENT_STATE);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	} while (status & (SDHCI_CMD_INHIBIT|SDHCI_DATA_INHIBIT));
+
+	/*
+	 * 512 bytes
+	 * This is only relevant for data commands.
+	 */
+	if (cmdreg & SD_CMD_DATA)
+	{
+		sdhci_clkctrl = sd_ioread32(mmc->priv, SDHCI_HOST_CONTROL);
+		if (mmc->bus_width == 8)
+		{
+			sdhci_clkctrl |= SDHCI_CTRL_HISPD;
+		}
+		else if (mmc->bus_width == 4)
+		{
+			sdhci_clkctrl |= SDHCI_CTRL_4BITBUS;
+		}
+		else if (mmc->bus_width == 1)
+		{
+			sdhci_clkctrl &= ~SDHCI_CTRL_4BITBUS;
+		}
+		else
+			printf("Invalid bus_width\n");
+
+		sd_iowrite32(mmc->priv, SDHCI_HOST_CONTROL, sdhci_clkctrl);
+
+		if( !((cmd->cmdidx==CMD24) || (cmd->cmdidx==CMD25)) )
+		{
+			cmdreg |= SDHCI_TRNS_READ;
+		}
+
+		if (data->blocks > 1)
+			cmdreg |= SDHCI_TRNS_MULTI | SDHCI_TRNS_AUTO_CMD12;
+
+		temp = data->blocks;
+		temp = temp << 12;
+		temp |= data->blocksize;
+		sd_iowrite32(mmc->priv, SDHCI_BLOCK_SIZE, temp);
+	}
+
+	sd_iowrite32(mmc->priv, SDHCI_ARGUMENT, cmd->cmdarg);
+
+	timeout = 1000000;
+	do{
+		make_sure = sd_ioread32(mmc->priv, SDHCI_PRESENT_STATE);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}while((make_sure & 0x00F00000) != 0x00F00000);
+
+	// Clear all pending interrupt status
+	sd_iowrite32(mmc->priv, SDHCI_INT_STATUS, 0x1ffffff);
+
+	cmdreg &= ~(CMDINDEX);
+	sd_iowrite32(mmc->priv, SDHCI_TRANSFER_MODE, cmdreg);
+	cmdreg |= (cmd->cmdidx << CMDINDEX_POS);
+	// Initiate the command
+	cmdreg |= SD_CMD_TRIG;
+	
+	sd_iowrite32(mmc->priv, SDHCI_TRANSFER_MODE, cmdreg);
+
+	if( ((cmd->cmdidx==CMD24) || (cmd->cmdidx==CMD25)) )
+	{
+		result = pilot_do_pio_write(mmc, data->blocks, data->blocksize, cmd->cmdarg, (u32)data->dest);
+	}
+	else
+	{
+		// Poll until operation complete
+		while (1) 
+		{
+			status = sd_ioread32(mmc->priv, SDHCI_INT_STATUS);
+
+			if (status & SD_INT_ERROR)
+			{
+				if ((cmd->cmdidx == CMD17) || (cmd->cmdidx == CMD18))
+				{
+					printf("MMC: Error reading sector %d (0x%08x)\n", cmd->cmdarg, cmd->cmdarg);
+				}
+				if (status & (SD_INT_ERR_CTIMEOUT|SD_INT_ERR_DTIMEOUT))
+				{
+					result = TIMEOUT;
+					goto exit;
+				}
+				else
+				{
+					result = COMM_ERR;
+					goto exit;
+				}
+			}
+			if (status & SD_INT_CMD_CMPL)
+			{
+				if (cmdreg & SD_CMD_DATA)
+				{
+					if ( (status & SD_INT_TRNS_CMPL) || (data->blocks > 1) )
+					{
+						pilot_do_pio_read(mmc, data->blocks, data->blocksize, cmd->cmdarg, (u32)data->dest);
+						break;
+					}
+					cnt++;
+					udelay(5000);
+					if(cnt>20)
+					{
+						result = TIMEOUT;
+						goto exit;
+					}
+				}
+				else
+					break;
+			}
+		}
+	}
+
+	if (cmd->resp_type & MMC_RSP_PRESENT)
+	{
+		if (cmd->resp_type & MMC_RSP_136)
+		{
+			// CRC is stripped so we need to do some shifting
+			// response type 2
+			cmd->response[0] = sd_ioread32(mmc->priv, SDHCI_RESPONSE + 0x0C);
+			cmd->response[1] = sd_ioread32(mmc->priv, SDHCI_RESPONSE + 0x08);
+			cmd->response[2] = sd_ioread32(mmc->priv, SDHCI_RESPONSE + 0x04);
+			cmd->response[3] = sd_ioread32(mmc->priv, SDHCI_RESPONSE);
+		}
+		else
+		{
+			// response type 1, 1b, 3, 6
+			cmd->response[0] = sd_ioread32(mmc->priv, SDHCI_RESPONSE + 0x04);
+			cmd->response[1] = sd_ioread32(mmc->priv, SDHCI_RESPONSE + 0x08);
+			cmd->response[2] = sd_ioread32(mmc->priv, SDHCI_RESPONSE + 0x0C);
+		}
+	}
+
+exit:
+	// Clear all pending interrupt status
+	sd_iowrite32(mmc->priv, SDHCI_INT_STATUS, 0x1ffffff);
+
+	return result;
+}
+
+
+void pilot_sdh_set_ios(struct mmc *mmc)
+{
+ 	volatile u32 sdhci_clkctrl;
+	volatile u32 clk = PILOT_SDHC_CLK_400KHz;
+
+	sdhci_clkctrl = sd_ioread32(mmc->priv, SDHCI_HOST_CONTROL);
+
+	pilot_set_clkctl(mmc, PILOT_SDHC_CLK_400KHz, 0x0, 0x0, 0x01);
+	sd_iowrite32(mmc->priv, SDHCI_RXDTTMR, 0x7fffffff);
+	sd_iowrite32(mmc->priv, SDHCI_TXDTTMR, 0x1854232);
+	udelay(5000);	
+
+	if(mmc->clock>=50000000)
+	{
+		clk = PILOT_SDHC_CLK_50MHz;
+		pilot_set_clkctl(mmc, clk, 0x0, 0x0, 0x1);
+		sd_iowrite32(mmc->priv, SDHCI_FSBLK_CTRL, 0xc0c00000);
+	}
+	else if(mmc->clock>=25000000)
+	{
+		clk = ((sd_ioread32(mmc->priv, SDHCI_CLOCK_CONTROL) & FSCLK_DIVISOR_BITS) >> FSCLK_DIVISOR_POS);
+	
+		clk = PILOT_SDHC_CLK_25MHz;
+		pilot_set_clkctl(mmc, clk, 0x0, 0x0, 0x1);
+		sd_iowrite32(mmc->priv, SDHCI_FSBLK_CTRL, 0x0);
+	}
+	else if(mmc->clock>=20000000)
+	{
+		clk = ((sd_ioread32(mmc->priv, SDHCI_CLOCK_CONTROL) & FSCLK_DIVISOR_BITS) >> FSCLK_DIVISOR_POS);
+	
+		clk = PILOT_SDHC_CLK_20MHz;
+		pilot_set_clkctl(mmc, clk, 0x0, 0x0, 0x1);
+		sd_iowrite32(mmc->priv, SDHCI_FSBLK_CTRL, 0x0);
+	}
+	else
+	{
+		clk = ((sd_ioread32(mmc->priv, SDHCI_CLOCK_CONTROL) & FSCLK_DIVISOR_BITS) >> FSCLK_DIVISOR_POS);
+
+		clk = PILOT_SDHC_CLK_400KHz;
+		pilot_set_clkctl(mmc, clk, 0x0, 0x0, 0x01);
+		sd_iowrite32(mmc->priv, SDHCI_FSBLK_CTRL, 0x0);
+	}
+
+	udelay(5000);
+	pilot_set_clkctl(mmc, clk, 0x0, 0x1, 0x01);
+	udelay(5000);
+
+	// Configure the bus_width
+	if (mmc->bus_width == 8)
+	{
+		sdhci_clkctrl |= SDHCI_CTRL_HISPD;
+	}
+	else if (mmc->bus_width == 4)
+	{
+		sdhci_clkctrl |= SDHCI_CTRL_4BITBUS;
+	}
+	else if (mmc->bus_width == 1)
+	{
+		sdhci_clkctrl &= ~SDHCI_CTRL_4BITBUS;
+	}
+	else
+		printf("Invalid bus_width\n");
+
+	sd_iowrite32(mmc->priv, SDHCI_HOST_CONTROL, sdhci_clkctrl);
+}
+
+
+int pilot_sdh_init(struct mmc *mmc)
+{
+	init_port(mmc);
+	return 0;
+}
+
+
+int cpu_mmc_init(bd_t *bd)
+{
+	int i;
+	struct mmc *mmc[NO_OF_SDHCS];
+	
+	for(i=0; i<NO_OF_SDHCS; i++)
+	{
+		mmc[i] = calloc((sizeof(struct mmc) + 10), 1);
+		if (!mmc[i]) {
+			return -ENOMEM;
+		}
+#if defined(CONFIG_PILOT4)
+		if (CONFIG_SPX_FEATURE_GLOBAL_SD_SLOT_COUNT)
+		{
+			if(i == SDHC0)
+			{
+				sprintf(mmc[i]->name, "SDHC%d", i);
+				mmc[i]->host_caps = MMC_MODE_4BIT | MMC_MODE_HS | MMC_MODE_HC | MMC_MODE_HS_52MHz;
+				mmc[i]->priv = (void *)SDHC0;
+			}
+			else if((i == SDHC1) && (i < CONFIG_SPX_FEATURE_GLOBAL_SD_SLOT_COUNT))
+			{
+				sprintf(mmc[i]->name, "SDHC%d", i);
+				mmc[i]->host_caps = MMC_MODE_4BIT | MMC_MODE_HS | MMC_MODE_HC | MMC_MODE_HS_52MHz;
+				mmc[i]->priv = (void *)SDHC1;
+			}
+		}
+		if (CONFIG_SPX_FEATURE_GLOBAL_EMMC_FLASH_COUNT)
+		{
+			if(i == CONFIG_SPX_FEATURE_GLOBAL_SD_SLOT_COUNT)
+			{
+				sprintf(mmc[i]->name, "EMMC%d", i);
+				mmc[i]->host_caps = MMC_MODE_4BIT | MMC_MODE_8BIT | MMC_MODE_HS | MMC_MODE_HC | MMC_MODE_HS_52MHz;
+				mmc[i]->priv = (void *)EMMC;
+			}
+		}
+#else
+		if(i == SDHC0)
+		{
+			sprintf(mmc[i]->name, "SDHC%d", i);
+			mmc[i]->host_caps = MMC_MODE_4BIT | MMC_MODE_HS | MMC_MODE_HC | MMC_MODE_HS_52MHz;
+			mmc[i]->priv = (void *)SDHC0;
+		}
+#endif
+		mmc[i]->send_cmd = pilot_sdh_request;
+		mmc[i]->set_ios = pilot_sdh_set_ios;
+		mmc[i]->init = pilot_sdh_init;
+
+		mmc[i]->voltages =  MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_29_30 | MMC_VDD_30_31
+			| MMC_VDD_31_32 |MMC_VDD_32_33 | MMC_VDD_33_34; 
+
+		mmc[i]->f_max = 50000000;
+		mmc[i]->f_min = 400000;
+		mmc[i]->b_max = 0x200;
+		mmc[i]->block_dev.part_type = PART_TYPE_DOS;
+
+		mmc_register(mmc[i]);
+	}
+	
+	return 0;
+}
+
+
+void pilot_set_clkctl(struct mmc *mmc, u16 fsclk_divisor, u8 sdclk_polarity, 
+		u8 sdclk_enable, u8 sdclk_select)
+{
+	volatile u32 tempData;
+	volatile int timeout = 1000000;
+
+	tempData = sd_ioread32(mmc->priv, SDHCI_CLOCK_CONTROL);
+
+	//clear all the required fields
+	tempData &= (~(SDCLK_SELECT));
+	tempData &= (~(SDCLK_CLK_ENABLE));
+	tempData &= (~(SDCLK_POLARITY));
+	tempData &= (~(FSCLK_DIVISOR));
+
+	//store all the required values
+	tempData |= ((fsclk_divisor & FSCLK_DIVISOR_BITS)<<FSCLK_DIVISOR_POS);
+	tempData |= ((sdclk_polarity & SDCLK_POLARITY_BITS)<<SDCLK_POLARITY_POS);
+	tempData |= ((sdclk_enable & SDCLK_CLK_ENABLE_BITS)<<SDCLK_CLK_ENABLE_POS);
+	tempData |= ((sdclk_select & SDCLK_SELECT_BITS)<<SDCLK_SELECT_POS);
+	
+	sd_iowrite32(mmc->priv, SDHCI_CLOCK_CONTROL, tempData);
+
+	if(sdclk_enable == 0)
+	{
+		do{
+			tempData = sd_ioread32(mmc->priv, SDHCI_CLOCK_CONTROL);
+			timeout--;
+			if(timeout <= 0) 
+			{
+				printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+				break;
+			}
+			udelay(1);
+		}while((tempData & 0x2) != 0x2);
+	}
+}
+
+
+int pilot_do_pio_read(struct mmc *mmc, u32 blkcnt, u32 blklen, u32 deviceAddr, u32 hostAddr)
+{
+	volatile u32 tempData;
+	volatile u32 hostmemAddr;
+	volatile u16 LMEMAddr;
+	volatile u16 LMEMBaseAddr;
+	volatile u16 LMEMLimitAddr;
+	volatile u16 tmp_blkcnt;
+	volatile u16 LMEMPGValidCnt;
+	volatile u16 LMEMCompPtr;
+	volatile u16 tmp_LMEMPGValidCnt;
+	volatile u16 rollover_LMEMPGValidCnt;
+	volatile u16 new_blkcnt;
+	volatile u16 new_blklen;
+	volatile u8 rxDtResult;
+	volatile u8 LMEMPGSize;
+	volatile int timeout = 1000000;
+
+	if ((blkcnt & 0x1ff) == 0) {
+		new_blkcnt = 0x200;
+	} else {
+		new_blkcnt = (blkcnt & 0x1ff);
+	}
+
+	if ((blklen & 0xfff) == 0) {
+		new_blklen = 0x1000;
+	} else {
+		new_blklen = (blklen & 0xfff);
+	}
+
+	hostmemAddr = hostAddr;
+	tmp_blkcnt = 0;
+	LMEMBaseAddr = 0x400;
+	LMEMLimitAddr = 0xBFF;
+	LMEMPGSize = 0x80;
+
+	while (tmp_blkcnt < new_blkcnt)
+	{
+		rollover_LMEMPGValidCnt = 0;
+
+		//Check Rx Data Ready
+		rxDtResult = pilot_check_rxdtrdy(mmc);
+		if (rxDtResult != 0)
+		{
+			printf("Pio data read fail\n");
+			return -1;
+		}
+		LMEMPGValidCnt = pilot_get_lmem_pgvalidcnt(mmc);
+
+		LMEMCompPtr = pilot_get_lmem_pgcompletionptr(mmc);
+
+		tmp_LMEMPGValidCnt = 0;
+		while (tmp_LMEMPGValidCnt < LMEMPGValidCnt)
+		{
+			if (rollover_LMEMPGValidCnt == 0)
+			{
+				if ((LMEMBaseAddr + ((LMEMCompPtr + (tmp_LMEMPGValidCnt * LMEMPGSize)) << 2)) > LMEMLimitAddr)
+				{
+					rollover_LMEMPGValidCnt = tmp_LMEMPGValidCnt;
+					LMEMAddr = LMEMBaseAddr;
+				}
+				else
+				{
+					LMEMAddr = LMEMBaseAddr + ((LMEMCompPtr + (tmp_LMEMPGValidCnt * LMEMPGSize)) << 2);
+				}
+			}
+			else
+			{
+				if ((LMEMBaseAddr + (((tmp_LMEMPGValidCnt - rollover_LMEMPGValidCnt) * LMEMPGSize) << 2)) > LMEMLimitAddr)
+				{
+					rollover_LMEMPGValidCnt = tmp_LMEMPGValidCnt;
+					LMEMAddr = LMEMBaseAddr;
+				}
+				else
+				{
+					LMEMAddr = LMEMBaseAddr + (((tmp_LMEMPGValidCnt - rollover_LMEMPGValidCnt) * LMEMPGSize) << 2);
+				}
+			}
+
+			pilot_store_pio_data_task(mmc, new_blklen, hostmemAddr, LMEMAddr);
+			hostmemAddr += new_blklen;
+			tmp_LMEMPGValidCnt = tmp_LMEMPGValidCnt + 1;
+			tmp_blkcnt = tmp_blkcnt + 1;
+			pilot_clear_rxdtrdy(mmc);
+		}
+	}
+
+	//Check CMD done
+	pilot_check_cmd_datadone(mmc);
+	pilot_clear_buf_rdreadyint(mmc);
+
+	tempData  = sd_ioread32(mmc->priv, SDHCI_PRESENT_STATE);
+	tempData = tempData & 0x00f00000;
+	while (tempData != 0x00f00000)
+	{
+		tempData  = sd_ioread32(mmc->priv, SDHCI_PRESENT_STATE);
+		tempData = tempData & 0x00f00000;
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}
+
+	tmp_blkcnt = 0;
+	return 0;
+}
+
+
+int pilot_do_pio_write(struct mmc *mmc, u32 blkcnt, u32 blklen, u32 deviceAddr, u32 hostAddr)
+{
+	volatile tU32 tempData;
+	volatile tU32 hostmemAddr;
+	volatile tU16 tmp_blkcnt;
+	volatile tU16 AllocIndex;
+	volatile tU16 new_blkcnt;
+	volatile tU16 new_blklen;
+	volatile tU16 LMEMBaseAddr;
+	volatile tU16 LMEMAddr;
+	volatile int ret=0;
+	volatile int timeout = 1000000;
+
+	new_blkcnt = blkcnt;
+	new_blklen = blklen;
+
+	tmp_blkcnt = 0;
+	hostmemAddr = hostAddr;
+	LMEMBaseAddr = 0x400;
+
+	while (tmp_blkcnt < new_blkcnt)
+	{
+		AllocIndex = pilot_check_lmem_pgavailable(mmc);
+		LMEMAddr = (LMEMBaseAddr + (AllocIndex << 2));
+		pilot_load_pio_data_task(mmc, new_blklen, hostmemAddr, LMEMAddr);
+		pilot_set_txdtrdy(mmc);
+		hostmemAddr += new_blklen;
+		tmp_blkcnt = tmp_blkcnt + 1;
+	}
+
+	//Check CMD done
+	ret = pilot_check_cmd_datadone(mmc);
+	pilot_clear_buf_wrreadyint(mmc);
+
+	tempData  = sd_ioread32(mmc->priv, SDHCI_PRESENT_STATE);
+	tempData = tempData & 0x00f00000;
+	while (tempData != 0x00f00000)
+	{
+		tempData  = sd_ioread32(mmc->priv, SDHCI_PRESENT_STATE);
+		tempData = tempData & 0x00f00000;
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}
+
+	return ret;
+}
+
+
+tU8 pilot_check_rxdtrdy (struct mmc *mmc)
+{
+	volatile tU32 tempData;
+	volatile tU32 readData;
+	volatile tU8 vldEmpty;
+	volatile int timeout = 1000000;
+
+	tempData  = 0x0;
+	readData  = 0x0;
+	vldEmpty  = 1;
+
+	while (((tempData & 0x00000800) != 0x00000800) && (vldEmpty == 1))
+	{
+		tempData  = sd_ioread32(mmc->priv, SDHCI_PRESENT_STATE);
+		readData  = sd_ioread32(mmc->priv, SDHCI_LTXSTS);
+		vldEmpty  = ((readData & LMEM_PG_VALID_EMPTY) >> LMEM_PG_VALID_EMPTY_POS);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}
+	return (0);
+}
+
+
+tU16 pilot_check_lmem_pgavailable(struct mmc *mmc)
+{
+	volatile tU32 tempData;
+	volatile int timeout = 1000000;
+
+	tempData = LMEM_ALL_PAGES_ALLOCATED;
+	while ((tempData & LMEM_ALL_PAGES_ALLOCATED) != 0x00000000)
+	{
+		tempData = sd_ioread32(mmc->priv, SDHCI_LMEMSTATUS);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}
+
+	tempData = sd_ioread32(mmc->priv, SDHCI_LMEMADDR);
+	tempData &= LMEM_FREE_PTR;
+	tempData = tempData >> LMEM_FREE_PTR_POS;
+
+	return(tempData & LMEM_FREE_PTR_BITS);
+}
+
+
+tU16 pilot_get_lmem_pgvalidcnt(struct mmc *mmc)
+{
+	volatile tU32 tempData;
+
+	tempData = sd_ioread32(mmc->priv, SDHCI_LMEMCNT);
+	tempData &= LMEM_VALID_DATA_PAGES_CNT;
+	tempData = tempData >> LMEM_VALID_DATA_PAGES_CNT_POS;
+
+	return(tempData & LMEM_VALID_DATA_PAGES_CNT_BITS);
+}
+
+
+tU16 pilot_get_lmem_pgcompletionptr (struct mmc *mmc)
+{
+	volatile tU32 tempData;
+
+	tempData = sd_ioread32(mmc->priv, SDHCI_LMEMSTATUS);
+	tempData &= LMEM_COMPLETION_PTR;
+	tempData = tempData >> LMEM_COMPLETION_PTR_POS;
+
+	return(tempData & LMEM_COMPLETION_PTR_BITS);
+}
+
+
+tU8 pilot_check_cmd_datadone (struct mmc *mmc)
+{
+	volatile tU32 tempData;
+	volatile tU32 f=0x00000003;
+	volatile tU32 i=0;
+	volatile int timeout = 1000000;
+
+	tempData = 0x0;
+
+	while (((tempData & 0x00000003) != 0x3)) 
+	{
+		tempData = sd_ioread32(mmc->priv, SDHCI_INT_STATUS);
+
+		if(tempData !=0)
+		{
+			f &= ~(tempData);
+			sd_iowrite32(mmc->priv, SDHCI_INT_STATUS, (tempData & 0x3));
+			timeout--;
+			if(timeout <= 0) 
+			{
+				printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+				break;
+			}
+			udelay(1);
+		}
+		udelay(5000);
+		i++;
+		
+		if(i==20)	//timeout
+			return TIMEOUT;
+		if(f == 0)
+		{
+			break;
+		}
+	}
+
+	return 0;
+}
+
+
+void pilot_clear_buf_wrreadyint(struct mmc *mmc)
+{
+	volatile tU32 tempData = 0;
+	volatile int timeout = 1000000;
+
+	while (((tempData & PIO_BUF_WRITE_READY_INT) >> PIO_BUF_WRITE_READY_INT_POS) != 0x1)
+	{
+		tempData = sd_ioread32(mmc->priv, SDHCI_INT_STATUS);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}
+	tempData &= PIO_BUF_WRITE_READY_INT;
+	sd_iowrite32(mmc->priv, SDHCI_INT_STATUS, tempData);
+}
+
+
+void pilot_clear_buf_rdreadyint (struct mmc *mmc)
+{
+	volatile tU32 tempData = 0;
+	volatile int timeout = 1000000;
+
+	while (((tempData & PIO_BUF_READ_READY_INT) >> PIO_BUF_READ_READY_INT_POS) != 0x1)
+	{
+		tempData = sd_ioread32(mmc->priv, SDHCI_INT_STATUS);
+		timeout--;
+		if(timeout <= 0) 
+		{
+			printf("%s %d Condition NOT met even after wait!!! Breaking while loop!!!\n", __FUNCTION__, __LINE__);
+			break;
+		}
+		udelay(1);
+	}
+	tempData &= PIO_BUF_READ_READY_INT;
+	sd_iowrite32(mmc->priv, SDHCI_INT_STATUS, tempData);
+}
+
+
+void pilot_set_txdtrdy (struct mmc *mmc)
+{
+	volatile tU32 tempData;
+
+	tempData = sd_ioread32(mmc->priv, SDHCI_PRESENT_STATE);
+	tempData = tempData | ((0x1) << 10);
+	sd_iowrite32(mmc->priv, SDHCI_PRESENT_STATE, tempData);
+}
+
+
+void pilot_clear_rxdtrdy (struct mmc *mmc)
+{
+	volatile tU32 tempData;
+
+	tempData = sd_ioread32(mmc->priv, SDHCI_PRESENT_STATE);
+	tempData = tempData | ((0x1) << 11);
+	sd_iowrite32(mmc->priv, SDHCI_PRESENT_STATE, tempData);
+}
+
+
+void pilot_store_pio_data_task (struct mmc *mmc, tU16 blklen, tU32 hostAddr, tU16 LMEMAddr)
+{
+	volatile tU16 tmp_blklen;
+	volatile tU16 tmp_blklen_cnt;
+	volatile tU32 tempData;
+	volatile tU32 hostData;
+	volatile tU32 sd_no;
+	volatile tU32 LMEMFullAddr;
+
+	sd_no = (tU32)mmc->priv;
+
+	tempData = SDHC_BASE_ADDR + (sd_no * 0x1000);
+
+	LMEMFullAddr = tempData + LMEMAddr;
+
+	tmp_blklen = (blklen >> 2);
+	tmp_blklen_cnt = 0;
+
+	while (tmp_blklen_cnt < tmp_blklen)
+	{
+		hostData = *(tPVU32)(LMEMFullAddr + (tmp_blklen_cnt << 2));
+		*(tPVU8)(hostAddr + (tmp_blklen_cnt << 2) + 0) = (hostData & (0xff));
+		hostData = (hostData >> 8);
+		*(tPVU8)(hostAddr + (tmp_blklen_cnt << 2) + 1) = (hostData & (0xff));
+		hostData = (hostData >> 8);
+		*(tPVU8)(hostAddr + (tmp_blklen_cnt << 2) + 2) = (hostData & (0xff));
+		hostData = (hostData >> 8);
+		*(tPVU8)(hostAddr + (tmp_blklen_cnt << 2) + 3) = (hostData & (0xff));
+		hostData = (hostData >> 8);
+
+		tmp_blklen_cnt += 1;
+	}
+}
+
+
+void pilot_load_pio_data_task(struct mmc *mmc, tU16 blklen, tU32 hostAddr, tU16 LMEMAddr)
+{
+	volatile tU16 tmp_blklen;
+	volatile tU16 tmp_blklen_cnt;
+	volatile tU32 hostData;
+	volatile tU8 hostDataByte;
+	volatile tU32 tempData;
+	volatile tU32 LMEMFullAddr;
+	volatile tU32 sd_no;
+
+	sd_no = (tU32)mmc->priv;
+
+	tempData = SDHC_BASE_ADDR + (sd_no * 0x1000);
+
+	LMEMFullAddr = tempData + LMEMAddr;
+
+	tmp_blklen = (blklen >> 2);
+	tmp_blklen_cnt = 0;
+
+	while (tmp_blklen_cnt < tmp_blklen)
+	{
+		hostData = 0;
+		hostDataByte = *(tPVU8)(hostAddr + (tmp_blklen_cnt << 2) + 0);
+		hostData |= (hostDataByte << 0);
+		hostDataByte = *(tPVU8)(hostAddr + (tmp_blklen_cnt << 2) + 1);
+		hostData |= (hostDataByte << 8);
+		hostDataByte = *(tPVU8)(hostAddr + (tmp_blklen_cnt << 2) + 2);
+		hostData |= (hostDataByte << 16);
+		hostDataByte = *(tPVU8)(hostAddr + (tmp_blklen_cnt << 2) + 3);
+		hostData |= (hostDataByte << 24);
+
+		*(tPVU32)(LMEMFullAddr + (tmp_blklen_cnt << 2)) = hostData;
+		tmp_blklen_cnt += 1;
+	}
+}
