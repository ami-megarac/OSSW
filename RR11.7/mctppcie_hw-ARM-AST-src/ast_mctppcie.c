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

 /*
 * File name: ast_mctppcie.c
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#include "driver_hal.h"
#include "mctppcie.h"
#include "mctppcieifc.h"
#include "ast_mctppcie.h"


#define AST_MCTPPCIE_DRIVER_NAME	"ast_mctppcie"

int asigned_EID = 0;
struct ast_mctp_info *ast_mctp;	
static pcie_core_funcs_t *pmctppcie_core_funcs;

static int enable_mctp_pkt(void)
{
	u32 status = 0;

	status = ioread32( ast_mctp->reg_base + AST_MCTP_CTRL );
	iowrite32 (status | MCTP_RX_RDY | MCTP_MSG_MASK | MCTP_MATCH_EID, ast_mctp->reg_base + AST_MCTP_CTRL);
	//iowrite32 (status | MCTP_RX_RDY, ast_mctp->reg_base + AST_MCTP_CTRL);
	
	iowrite32 (MCTP_RX_COMPLETE | MCTP_TX_LAST, ast_mctp->reg_base + AST_MCTP_IER);
	
	return 0;
}

static int mctppcie_rx_recv(mctp_iodata *io)
{
	if ( wait_event_interruptible (ast_mctp->mctp_wq, (ast_mctp->flag == MCTP_EOM)) < 0 )
	{	
		io->status = MCTP_STATUS_ERROR;
		io->len = 0;
		return -ERESTARTSYS;
	}
	
	ast_mctp->flag = 0;

	io->data[0] = ast_mctp->rt;
	io->data[1] = ast_mctp->ep_id;
	io->data[2] = ast_mctp->msg_tag;
	memcpy(io->data + 3, ast_mctp->rx_fifo, ast_mctp->rx_len);
	io->len = ast_mctp->rx_len + 3;
	io->status = MCTP_STATUS_SUCCESS;
	
	ast_mctp->rx_fifo_done = 0;
	ast_mctp->rx_len = 0;
	memset(ast_mctp->rx_fifo, 0,  1024);
	return 0;
}

void mctppcie_wait_tx_complete(struct ast_mctp_info *ast_mctp)
{
	wait_event_interruptible(ast_mctp->mctp_wq, (ast_mctp->flag == MCTP_TX_LAST));
	ast_mctp->flag = 0;
}

static int mctppcie_tx_xfer(mctp_iodata *mctp_xfer)
{
	int i;
	uint32_t reg;
	u32 xfer_len = (mctp_xfer->len /4);
	u32 padding_len;

	if((mctp_xfer->len % 4))
	{
		xfer_len++;
		padding_len = 4 - ((mctp_xfer->len) % 4);
	}
	else
	{
		padding_len = 0;
	}
	
	// TODO: Check TX Command
	/* Data Structure
	 *
	 * byte 0 : Routing Type
	 * byte 1 : Destination EID
	 * byte 2 : Source EID
	 * byte 3 : Bus Number
	 * byte 4 : Device Number
	 * byte 5 : Function Number
	 * byte 6 : Tag Owner
	 * byte 7 - N : payload Data
	 */
	//bit 15 : interrupt enable
	ast_mctp->tx_cmd_desc->desc0 = 0x0000a000 | ROUTING_TYPE_L(mctp_xfer->data[0]) | ROUTING_TYPE_H(mctp_xfer->data[0]) | PKG_SIZE(xfer_len) | BUS_NO(mctp_xfer->data[3]) | DEV_NO(mctp_xfer->data[4]) | FUN_NO(mctp_xfer->data[5]) | PADDING_LEN(padding_len) | TAG_OWN(mctp_xfer->data[6]);

	//set dest endpoint id = 0;			
	ast_mctp->tx_cmd_desc->desc1 &= ~(0xFF);
	ast_mctp->tx_cmd_desc->desc1 |= LAST_CMD | DEST_EP_ID(mctp_xfer->data[1]);

	memset(ast_mctp->tx_data, 0,  1024);
	memcpy(ast_mctp->tx_data, mctp_xfer->data + 7, mctp_xfer->len);
	for(i = 0; i < mctp_xfer->len; i++)
	{
		printk("%02x ", ast_mctp->tx_data[i]);
	}
	//trigger tx
	reg = ioread32( ast_mctp->reg_base + AST_MCTP_CTRL );
	iowrite32 (reg | MCTP_TX, ast_mctp->reg_base + AST_MCTP_CTRL);

	if(!asigned_EID)
	{
		if(mctp_xfer->data[2] != 0)
		{
			//reg = ioread32( ast_mctp->reg_base + AST_MCTP_CTRL );
			//iowrite32 (reg | MCTP_MSG_MASK | MCTP_MATCH_EID, ast_mctp->reg_base + AST_MCTP_CTRL);
			reg = ioread32( ast_mctp->reg_base + AST_MCTP_EID );
			iowrite32 (reg | MCTP_EP_ID(mctp_xfer->data[2]), ast_mctp->reg_base + AST_MCTP_EID);
			asigned_EID = 1;
		}
	}
	//wait interrupt
	mctppcie_wait_tx_complete(ast_mctp);
	
	return MCTP_STATUS_SUCCESS;
}

static void mctppcie_rx_combine_data(struct ast_mctp_info *ast_mctp)
{
	int i;
	u32 rx_len = 0;
	u32 padding_len = 0;
	
	for(i=0;i<8;i++)
	{
		ast_mctp->rx_index %=8; 

		if(ast_mctp->rx_cmd_desc[ast_mctp->rx_index].desc0 & CMD_UPDATE)
		{
			if(ast_mctp->rx_fifo_done != 1)
			{
				if(MCTP_SOM & ast_mctp->rx_cmd_desc[ast_mctp->rx_index].desc0)
				{
					//MCTP_SOM
					ast_mctp->rx_fifo_index = 0;
				}
				
				if(MCTP_EOM & ast_mctp->rx_cmd_desc[ast_mctp->rx_index].desc0)
				{
					//MCTP_EOM ast_mctp->rx_fifo_done
					ast_mctp->rx_fifo_done = 1;
					padding_len = GET_PADDING_LEN(ast_mctp->rx_cmd_desc[ast_mctp->rx_index].desc0);
					ast_mctp->flag = MCTP_EOM;
				}

				rx_len = GET_PKG_LEN(ast_mctp->rx_cmd_desc[ast_mctp->rx_index].desc0) * 4;
				rx_len -= padding_len;

				memcpy(ast_mctp->rx_fifo + (0x40 * ast_mctp->rx_fifo_index), ast_mctp->rx_data + (ast_mctp->rx_index * 0x80), rx_len);
				ast_mctp->rx_fifo_index++;

				ast_mctp->rx_len += rx_len;
				ast_mctp->ep_id = GET_SRC_EP_ID(ast_mctp->rx_cmd_desc[ast_mctp->rx_index].desc0);
				ast_mctp->rt = GET_ROUTING_TYPE(ast_mctp->rx_cmd_desc[ast_mctp->rx_index].desc0);
				ast_mctp->seq_no = GET_SEQ_NO(ast_mctp->rx_cmd_desc[ast_mctp->rx_index].desc0);
				ast_mctp->msg_tag = GET_MCTP_TAG(ast_mctp->rx_cmd_desc[ast_mctp->rx_index].desc0);
				//printk("rx len = %d , epid = %x, rt = %x, seq no = %d, total_len = %d \n",rx_len, ast_mctp->ep_id, ast_mctp->rt, ast_mctp->seq_no, ast_mctp->rx_len);	
			}
			else
			{
				printk("drop \n");
			}

			//RX CMD desc0
			ast_mctp->rx_cmd_desc[ast_mctp->rx_index].desc0 = 0;
			ast_mctp->rx_index++;
			
		}
		else
		{
			printk("index %d break\n",ast_mctp->rx_index);		
			break;
		}
	}	
}

static irqreturn_t mctppcie_handler(int this_irq, void *dev_id)
{
	u32 status = ioread32( ast_mctp->reg_base + AST_MCTP_ISR );

	if (status & MCTP_TX_LAST) 
	{
		iowrite32 ( MCTP_TX_LAST | status, ast_mctp->reg_base + AST_MCTP_ISR);
		ast_mctp->flag = MCTP_TX_LAST;
	}

	if (status & MCTP_TX_COMPLETE) 
	{
		iowrite32 ( MCTP_TX_COMPLETE | status, ast_mctp->reg_base + AST_MCTP_ISR);
	}

	if (status & MCTP_RX_COMPLETE) 
	{
		iowrite32 ( MCTP_RX_COMPLETE | status, ast_mctp->reg_base + AST_MCTP_ISR);
		mctppcie_rx_combine_data(ast_mctp);
	}

	if (status & MCTP_RX_NO_CMD) 
	{
		iowrite32 ( MCTP_RX_NO_CMD | status, ast_mctp->reg_base + AST_MCTP_ISR);
		ast_mctp->flag = MCTP_RX_NO_CMD;
		printk("MCTP_RX_NO_CMD \n");
	}

	if (ast_mctp->flag) 
	{
		wake_up_interruptible(&ast_mctp->mctp_wq);
		return IRQ_HANDLED;
	}
	else 
	{
		printk ("TODO Check MCTP's interrupt %x\n",status);
		return IRQ_NONE;
	}
}

static mctppcie_hal_operations_t ast_mctppcie_hw_ops = {
	downstream_mctp_pkt	:	mctppcie_rx_recv,
	upstream_mctp_pkt	:	mctppcie_tx_xfer,
	enable_mctp_pkt		:	enable_mctp_pkt,
};	

static hw_hal_t ast_mctppcie_hw_hal = {
	.dev_type		= EDEV_TYPE_MCTP_PCIE,
	.owner			= THIS_MODULE,
	.devname		= AST_MCTPPCIE_DRIVER_NAME,
	.num_instances	= MCTP_PCIE_HW_MAX_INST,
	.phal_ops		= (void *) &ast_mctppcie_hw_ops
};

void ast_mctp_hardware_init(void) 
{
	int i=0;
	//uint32_t reg;
	
	iowrite32 (ast_mctp->dram_base, ast_mctp->reg_base + AST_MCTP_EID);
	
	//tx 
	ast_mctp->mctp_dma = dma_alloc_coherent(NULL, 4096, &ast_mctp->mctp_dma_addr, GFP_KERNEL);

	if (!ast_mctp->mctp_dma)
		printk( KERN_ERR "%s: unable to allocate tx Buffer memory\n", AST_MCTPPCIE_DRIVER_NAME);
		
	ast_mctp->tx_cmd_desc = (struct ast_mctp_cmd_desc *)ast_mctp->mctp_dma;
	ast_mctp->tx_cmd_desc_dma = ast_mctp->mctp_dma_addr;
		
	ast_mctp->tx_data = (u8*)(ast_mctp->mctp_dma + MCTP_TX_DATA_SIZE);
	ast_mctp->tx_data_dma = ast_mctp->mctp_dma_addr + MCTP_TX_DATA_SIZE;

	for (i = 0; i < MCTP_TX_DATA_SIZE; i++)
	{
		ast_mctp->tx_data[i] = i;
	}
	
	ast_mctp->tx_cmd_desc->desc1 |= TX_DATA_ADDR(ast_mctp->tx_data_dma);
	
	iowrite32 (ast_mctp->tx_cmd_desc_dma, ast_mctp->reg_base + AST_MCTP_TX_CMD);
		
	//RX 8 buffer 
	ast_mctp->rx_cmd_desc = (struct ast_mctp_cmd_desc *)(ast_mctp->mctp_dma + MCTP_RX_CMD_SIZE);
	ast_mctp->rx_cmd_desc_dma = ast_mctp->mctp_dma_addr + MCTP_RX_CMD_SIZE;

	ast_mctp->rx_data = (u8 *)(ast_mctp->mctp_dma + MCTP_RX_DATA_SIZE);
	ast_mctp->rx_data_dma = ast_mctp->mctp_dma_addr + MCTP_RX_DATA_SIZE;
	
	ast_mctp->rx_index = 0;
	ast_mctp->rx_fifo = (u8 *)(ast_mctp->mctp_dma + MCTP_RX_FIFO_SIZE);
	
	ast_mctp->rx_fifo_done = 0;
	ast_mctp->rx_fifo_index = 0;
	ast_mctp->rx_len = 0;
	memset(ast_mctp->rx_fifo, 0,  1024);

	for(i =0;i< 8;i++)
	{
		ast_mctp->rx_cmd_desc[i].desc0 = 0;
		ast_mctp->rx_cmd_desc[i].desc1 = RX_DATA_ADDR((ast_mctp->rx_data_dma + (0x80 * i)));
		if(i == 7)
			ast_mctp->rx_cmd_desc[i].desc1 |= LAST_CMD;
	}

	iowrite32 (ast_mctp->rx_cmd_desc_dma, ast_mctp->reg_base + AST_MCTP_RX_CMD);

	/*reg = ioread32( ast_mctp->reg_base + AST_MCTP_CTRL );
	iowrite32 (reg | MCTP_RX_RDY, ast_mctp->reg_base + AST_MCTP_CTRL);
	
	iowrite32 (MCTP_RX_COMPLETE | MCTP_TX_LAST, ast_mctp->reg_base + AST_MCTP_IER);*/
}

int ast_mctppcie_init(void)
{
	int status;
	
	extern int mctppcie_core_loaded;
	if (!mctppcie_core_loaded)
		return -1;
			
	if (!(ast_mctp = kzalloc(sizeof(struct ast_mctp_info), GFP_KERNEL))) 
		return -ENOMEM;
	
	ast_mctp->hal_id = register_hw_hal_module(&ast_mctppcie_hw_hal, (void **) &pmctppcie_core_funcs);
	if (ast_mctp->hal_id < 0) 
	{
		printk(KERN_WARNING "%s: register HAL HW module failed\n", AST_MCTPPCIE_DRIVER_NAME);
		return ast_mctp->hal_id;
	}
	
	ast_mctp->reg_base = ioremap(AST_MCTP_REG_BASE, 0x1000);
	if (!ast_mctp->reg_base) 
	{
		printk(KERN_WARNING "%s: ioremap failed\n", AST_MCTPPCIE_DRIVER_NAME);
		unregister_hw_hal_module(EDEV_TYPE_MCTP_PCIE, ast_mctp->hal_id);
		return -ENOMEM;
	}
	
	ast_mctp->dram_base = AST_MCTP_DRAM_BASE;
	
	ast_mctp->irq = request_irq( IRQ_MCTP, mctppcie_handler, IRQF_SHARED, AST_MCTPPCIE_DRIVER_NAME, (void *)AST_MCTP_REG_BASE );
	if( ast_mctp->irq != 0 )
	{
		printk( KERN_ERR "%s: Failed request irq %d, return %d\n", AST_MCTPPCIE_DRIVER_NAME, IRQ_MCTP, ast_mctp->irq);
		unregister_hw_hal_module(EDEV_TYPE_MCTP_PCIE, ast_mctp->hal_id);
		iounmap (ast_mctp->reg_base);
		return -EIO;
	}
	
	ast_mctp->flag = 0;
	init_waitqueue_head(&ast_mctp->mctp_wq);
	
	//SCU Init MCTP
	*(volatile u32 *)(IO_ADDRESS(0x1E6E2000)) = (0x1688A8A8); //Unlock SCU register
	status = *(volatile u32 *)(IO_ADDRESS(0x1E6E2004));
	*(volatile u32 *)(IO_ADDRESS(0x1E6E2004)) = status &= ~(0x01000000);
	*(volatile u32 *)(IO_ADDRESS(0x1E6E2000)) = 0; //Lock SCU register
	
	ast_mctp_hardware_init();
	
	printk("The MCTP Over PCIE HW Driver is loaded successfully.\n" );
	return 0;
}

void ast_mctppcie_exit(void)
{
	unregister_hw_hal_module(EDEV_TYPE_MCTP_PCIE, ast_mctp->hal_id);
	
	free_irq(IRQ_MCTP, (void *)AST_MCTP_REG_BASE);

	iounmap (ast_mctp->reg_base);	
	
	return;
}

module_init (ast_mctppcie_init);
module_exit (ast_mctppcie_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("ASPEED AST SoC MCTP Over PCIE Driver");
MODULE_LICENSE ("GPL");
