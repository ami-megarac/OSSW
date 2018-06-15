--- uboot/arch/arm/cpu/pilotcommon/pilotmmc.h	2015-07-22 19:49:22.987435420 +0800
+++ uboot.new/arch/arm/cpu/pilotcommon/pilotmmc.h	2015-07-23 10:48:30.120342000 +0800
@@ -0,0 +1,196 @@
+
+#ifndef __PILOTMMC_H_
+#define __PILOTMMC_H_
+
+typedef unsigned char tU8;
+typedef unsigned short tU16;
+typedef unsigned int tU32;
+typedef volatile unsigned char * tPVU8;
+typedef volatile unsigned short * tPVU16;
+typedef volatile unsigned int * tPVU32;
+
+#if defined(CONFIG_PILOT4)
+#define NO_OF_SDHCS		(CONFIG_SPX_FEATURE_GLOBAL_SD_SLOT_COUNT + CONFIG_SPX_FEATURE_GLOBAL_EMMC_FLASH_COUNT)	//Numbers of SDIO
+#else
+#define NO_OF_SDHCS		1
+#endif
+#define SDHC0			0
+#define SDHC1			1
+#define EMMC			2
+
+#define SDHC_BASE_ADDR		0x40C00000
+#define SYSRCR2				0x40100850
+#define SYSSRERL2			0x40100854
+#define SYSSRERH2			0x40100858
+
+//Register defines
+#define SDHCI_DMA_ADDRESS			0x00
+#define SDHCI_BLOCK_SIZE			0x04
+#define SDHCI_ARGUMENT				0x08
+#define SDHCI_TRANSFER_MODE			0x0C
+#define  SDHCI_TRNS_DMA				0x01
+#define  SDHCI_TRNS_AUTO_CMD12		0x04
+#define  SDHCI_TRNS_READ			0x10
+#define  SDHCI_TRNS_MULTI			0x20
+#define  SD_CMD_RESP_NONE    (0<<16)
+#define  SD_CMD_RESP_136     (1<<16)
+#define  SD_CMD_RESP_48      (2<<16)
+#define  SD_CMD_RESP_48_BUSY (3<<16)
+#define  SD_CMD_CRC			 (1<<19) 
+#define  SD_CMD_INDEX        (1<<20)
+#define  SD_CMD_DATA         (1<<21)
+#define  SD_CMD_TRIG         (1<<31)
+
+#define SDHCI_RESPONSE				0x10
+#define SDHCI_PRESENT_STATE			0x24
+#define  SDHCI_CMD_INHIBIT			0x00000001
+#define  SDHCI_DATA_INHIBIT			0x00000002
+#define  SDHCI_DOING_WRITE			0x00000100
+#define  SDHCI_DOING_READ			0x00000200
+#define  SDHCI_CARD_PRESENT			0x00010000
+#define  SDHCI_CARD_DEBOUNCING		0x00020000
+#define  SDHCI_CARD_DETECTPINLEVEL	0x00040000
+#define  SDHCI_WRITE_PROTECT		0x00080000
+
+#define SDHCI_HOST_CONTROL			0x28
+#define  SDHCI_CTRL_4BITBUS			0x02
+#define  SDHCI_CTRL_HISPD			0x04
+#define  SDHCI_CTRL_TEST_INS		0x40
+#define  SDHCI_CTRL_TEST			0x80
+
+#define SDHCI_CLOCK_CONTROL			0x2C
+#define  SDHCI_DIVIDER_SHIFT		8
+#define  SDHCI_CLOCK_CARD_EN		0x0004
+#define  SDHCI_CLOCK_INT_STABLE		0x0002
+#define  SDHCI_CLOCK_INT_EN			0x0001
+
+#define SD_REFF_CLK_50M		50000000
+#define SD_HOST_HS			0x4
+
+#define SDHCI_INT_STATUS			0x30
+#define SDHCI_INT_ENABLE			0x34
+#define SDHCI_SIGNAL_ENABLE			0x38
+#define  SD_INT_CMD_CMPL     0x00000001
+#define  SD_INT_TRNS_CMPL    0x00000002
+#define  SD_INT_DMA          0x00000008
+#define  SD_INT_CINS         0x00000040
+#define  SD_INT_ERR_CREM     0x00000080
+#define  SD_INT_ERR_WP       0x00000200
+#define  SD_INT_ERR_CTIMEOUT 0x00010000
+#define  SD_INT_ERR_CCRC     0x00020000
+#define  SD_INT_ERR_CEB      0x00040000
+#define  SD_INT_ERR_IDX      0x00080000
+#define  SD_INT_ERR_DTIMEOUT 0x00100000
+#define  SD_INT_ERR_DCRC     0x00200000
+#define  SD_INT_ERR_DEB      0x00400000
+#define  SD_INT_ERR_CLMT     0x00800000
+#define  SD_INT_ERR_ACMD12   0x01000000
+#define  SD_INT_ERR_XDMA     0x04000000
+#define  SD_INT_ERR_TRESP    0x08000000
+#define  SD_INT_ERROR        (SD_INT_ERR_WP | SD_INT_ERR_CTIMEOUT | SD_INT_ERR_CCRC | \
+								SD_INT_ERR_CEB | SD_INT_ERR_IDX | SD_INT_ERR_DTIMEOUT | \
+								SD_INT_ERR_DCRC | SD_INT_ERR_DEB | SD_INT_ERR_CLMT | \
+								SD_INT_ERR_ACMD12 | SD_INT_ERR_XDMA | SD_INT_ERR_TRESP)
+
+#define SDHCI_ACMD12_ERR			0x3C
+
+//controller specific register
+#define SDHCI_FSBLK_CTRL			0x84
+#define SDHCI_RSPTMR				0x104
+#define SDHCI_TXDTTMR				0x108
+#define SDHCI_RXDTTMR				0x10C
+#define SDHCI_LTXSTS				0x118
+#define SDHCI_LMEMADDR				0x130
+#define SDHCI_LMEMCNT				0x134
+#define SDHCI_LMEMCFG				0x138
+#define SDHCI_LMEMCTL				0x13C
+#define SDHCI_LMEMSTATUS			0x140
+
+//Clock Divisor values
+#define PILOT_SDHC_CLK_400KHz   0x0FA
+#define PILOT_SDHC_CLK_20MHz    0x006
+#define PILOT_SDHC_CLK_25MHz    0x004
+#define PILOT_SDHC_CLK_50MHz    0x002
+
+//SDHCI_CLOCK_CONTROL bit definations
+#define SDCLK_SELECT_POS                   0
+#define SDCLK_SELECT_BITS                  0x3
+#define SDCLK_SELECT                       (SDCLK_SELECT_BITS<<SDCLK_SELECT_POS)
+#define SDCLK_CLK_ENABLE_POS               2
+#define SDCLK_CLK_ENABLE_BITS              0x1
+#define SDCLK_CLK_ENABLE                   (SDCLK_CLK_ENABLE_BITS<<SDCLK_CLK_ENABLE_POS)
+#define SDCLK_POLARITY_POS                 3
+#define SDCLK_POLARITY_BITS                0x1
+#define SDCLK_POLARITY                     (SDCLK_POLARITY_BITS<<SDCLK_POLARITY_POS)
+#define FSCLK_DIVISOR_POS                  4
+#define FSCLK_DIVISOR_BITS                 0x3ff
+#define FSCLK_DIVISOR                      (FSCLK_DIVISOR_BITS<<FSCLK_DIVISOR_POS)
+#define DEB_SEL_POS                        22
+#define DEB_SEL_BITS                       0x3
+#define DEB_SEL                            (DEB_SEL_BITS << DEB_SEL_POS)
+
+//SDHCI_LTXSTS bit definitions
+#define LMEM_PG_VALID_EMPTY_POS            12
+#define LMEM_PG_VALID_EMPTY_BITS           0x1
+#define LMEM_PG_VALID_EMPTY                (LMEM_PG_VALID_EMPTY_BITS << LMEM_PG_VALID_EMPTY_POS)
+
+//SDHCI_LMEMCNT bit definitions
+#define LMEM_ALLOCATED_PG_CNT_POS          0
+#define LMEM_ALLOCATED_PG_CNT_BITS         0xffff
+#define LMEM_ALLOCATED_PG_CNT              (LMEM_ALLOCATED_PG_CNT_BITS << LMEM_ALLOCATED_PG_CNT_POS)
+#define LMEM_VALID_DATA_PAGES_CNT_POS      16
+#define LMEM_VALID_DATA_PAGES_CNT_BITS     0xffff
+#define LMEM_VALID_DATA_PAGES_CNT          (LMEM_VALID_DATA_PAGES_CNT_BITS << LMEM_VALID_DATA_PAGES_CNT_POS)
+
+/* SDINTSTS_REG, SDINTSEN_REG, SDINTEN_REG bit definitions */
+#define PIO_BUF_WRITE_READY_INT_POS        4
+#define PIO_BUF_WRITE_READY_INT_BITS       0x1
+#define PIO_BUF_WRITE_READY_INT            (PIO_BUF_WRITE_READY_INT_BITS << PIO_BUF_WRITE_READY_INT_POS)
+
+#define PIO_BUF_READ_READY_INT_POS         5
+#define PIO_BUF_READ_READY_INT_BITS        0x1
+#define PIO_BUF_READ_READY_INT             (PIO_BUF_READ_READY_INT_BITS << PIO_BUF_READ_READY_INT_POS)
+
+//SDHCI_LMEMSTATUS bit definitions
+#define LMEM_COMPLETION_PTR_POS            0
+#define LMEM_COMPLETION_PTR_BITS           0x3ff
+#define LMEM_COMPLETION_PTR                (LMEM_COMPLETION_PTR_BITS << LMEM_COMPLETION_PTR_POS)
+#define LMEM_ALL_PAGES_ALLOCATED_POS       17
+#define LMEM_ALL_PAGES_ALLOCATED_BITS      0x1
+#define LMEM_ALL_PAGES_ALLOCATED           (LMEM_ALL_PAGES_ALLOCATED_BITS << LMEM_ALL_PAGES_ALLOCATED_POS)
+
+/* SDHCI_LMEMADDR bit definitions */
+#define LMEM_FREE_PTR_POS                  0
+#define LMEM_FREE_PTR_BITS                 0x3ff
+#define LMEM_FREE_PTR                      (LMEM_FREE_PTR_BITS << LMEM_FREE_PTR_POS)
+
+/* CMDCTL_REG bit definitions */
+#define CMDINDEX_POS                       24
+#define CMDINDEX_BITS                      0x3f
+#define CMDINDEX                           (CMDINDEX_BITS << CMDINDEX_POS)
+
+
+#define CMD17	(17)		/* READ_SINGLE_BLOCK */
+#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
+#define CMD24	(24)		/* WRITE_BLOCK */
+#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
+
+
+/* function prototype declaration */
+int pilot_set_resp_type(tU32 resp_type);
+tU8 pilot_check_rxdtrdy (struct mmc *);
+void pilot_clear_rxdtrdy (struct mmc *);
+void pilot_set_txdtrdy (struct mmc *);
+tU8 pilot_check_cmd_datadone (struct mmc *);
+tU16 pilot_check_lmem_pgavailable(struct mmc *);
+void pilot_clear_buf_wrreadyint(struct mmc *);
+void pilot_clear_buf_rdreadyint (struct mmc *);
+tU16 pilot_get_lmem_pgvalidcnt(struct mmc *);
+tU16 pilot_get_lmem_pgcompletionptr (struct mmc *);
+void pilot_set_clkctl(struct mmc *, u16 , u8, u8, u8);
+int pilot_do_pio_read(struct mmc *, u32, u32, u32, u32);
+int pilot_do_pio_write(struct mmc *, u32, u32, u32, u32);
+void pilot_store_pio_data_task (struct mmc *, tU16, tU32, tU16);
+void pilot_load_pio_data_task (struct mmc *, tU16, tU32, tU16);
+
+#endif /* __PILOTMMC_H_ */
