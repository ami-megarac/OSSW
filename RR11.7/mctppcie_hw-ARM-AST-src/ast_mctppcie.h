/****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef _AST_MCTP_PCIE_H_
#define _AST_MCTP_PCIE_H_

#define AST_MCTP_REG_BASE	0x1E6E8000
#define AST_MCTP_DRAM_BASE	0x80000000
#define MCTP_PCIE_HW_MAX_INST 1

// register
#define	AST_MCTP_CTRL		0x00
#define	AST_MCTP_TX_CMD		0x04
#define	AST_MCTP_RX_CMD		0x08
#define	AST_MCTP_ISR		0x0c
#define	AST_MCTP_IER		0x10
#define	AST_MCTP_EID		0x14
#define AST_MCTP_OBFF		0x18

// AST_MCTP_CTRL
#define MCTP_RX_RDY			(1 << 4)
#define MCTP_TX				1
#define MCTP_MSG_MASK       (1 << 8)
#define MCTP_MATCH_EID      (1 << 9)

// AST_MCTP_ISR
#define MCTP_RX_NO_CMD		(1 << 9)
#define MCTP_RX_COMPLETE	(1 << 8)

#define MCTP_TX_LAST		(1 << 1)
#define MCTP_TX_COMPLETE	1

// AST_MCTP_EID
#define MCTP_EP_ID(x)       (x & 0xff)

//TX CMD desc0
#define BUS_NO(x)			((x & 0xff) << 24)
#define DEV_NO(x)			((x & 0x1f) << 19)
#define FUN_NO(x)			((x & 0x7) << 16)
#define ROUTING_TYPE_L(x)   ((x & 0x1) << 14)
#define ROUTING_TYPE_H(x)   (((x & 0x2) >> 1) << 12)
#define TAG_OWN(x)			((x & 0x1) << 13)
#define PKG_SIZE(x)			((x & 0x7ff) << 2) 
#define PADDING_LEN(x)		(x & 0x3)
//TX CMD desc1
#define LAST_CMD			(1 << 31)
#define TX_DATA_ADDR(x)		(((x >> 7) & 0x7fffff) << 8)
#define DEST_EP_ID(x)		(x & 0xff)

//RX CMD desc0
#define GET_PKG_LEN(x)		((x >> 24) & 0x7f)
#define GET_SRC_EP_ID(x)	((x >> 16) & 0xff)
#define GET_ROUTING_TYPE(x)	((x >> 14) & 0x7)
#define GET_SEQ_NO(x)		((x >> 11) & 0x3)
#define GET_MCTP_TAG(x)     ((x >> 8) & 0x7)
#define MCTP_SOM			(1 << 7)
#define MCTP_EOM			(1 << 6)
#define GET_PADDING_LEN(x)	((x >> 4) & 0x3)
#define CMD_UPDATE			(1)
//RX CMD desc1
#define LAST_CMD			(1 << 31)
#define RX_DATA_ADDR(x)		(((x >> 7) & 0x3fffff) << 7)

// Buffer Size
#define MCTP_TX_CMD_SIZE	0
#define MCTP_RX_CMD_SIZE	(MCTP_TX_CMD_SIZE + 512)
#define MCTP_TX_DATA_SIZE	(MCTP_RX_CMD_SIZE + 512)
#define MCTP_RX_DATA_SIZE	(MCTP_TX_DATA_SIZE + 1024)
#define MCTP_RX_FIFO_SIZE	(MCTP_RX_DATA_SIZE + 1024)
#define MAX_XFER_BUFF_SIZE	1024

struct ast_mctp_cmd_desc {
	unsigned int desc0;
	unsigned int desc1;
};

struct ast_mctp_xfer {
	unsigned char xfer_buff[MAX_XFER_BUFF_SIZE];
	unsigned int xfer_len;
	unsigned int ep_id;
	unsigned int bus_no;
	unsigned int dev_no;	
	unsigned int fun_no;
	unsigned char rt;
	
};

struct ast_mctp_info {
	void __iomem	*reg_base;	
	int irq;
	u32 dram_base;	
	u32 hal_id;
	wait_queue_head_t mctp_wq;	
	u8 *mctp_dma;
	dma_addr_t mctp_dma_addr;
	
	struct ast_mctp_cmd_desc *tx_cmd_desc;
	dma_addr_t tx_cmd_desc_dma;
	u8 *tx_data;
	dma_addr_t tx_data_dma;

	struct ast_mctp_cmd_desc *rx_cmd_desc;
	dma_addr_t rx_cmd_desc_dma;
	u8 *rx_data;
	dma_addr_t rx_data_dma;
	u32 rx_index;
	u8 *rx_fifo;
	u8 rx_fifo_index;
	u32 rx_fifo_done;

	u32 flag;
	bool is_open;
	u32 state;
	//rx
	u32 rx_len;
	u8 ep_id;
	u8 rt;	
	u8 seq_no;
    u8 msg_tag;
};


#endif /* _AST_MCTP_PCIE_H_ */
