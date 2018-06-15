/****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
***************************************************************/

#ifndef __P4_USBD_H__
#define __P4_USBD_H__		// Followed P4 USB DEVICE SPEC

/* DEFINE */
#define P4_USBD_ENABLE					(1)
#define P4_USBD_DISABLE					(0)
#define P4_USBD_BIT0_SET				(1<<0)
#define P4_USBD_BIT1_SET				(1<<1)
#define P4_USBD_BIT2_SET				(1<<2)
#define P4_USBD_BIT3_SET				(1<<3)
#define P4_USBD_BIT4_SET				(1<<4)
#define P4_USBD_BIT5_SET				(1<<5)
#define P4_USBD_BIT6_SET				(1<<6)
#define P4_USBD_BIT7_SET				(1<<7)
#define P4_USBD_BIT8_SET				(1<<8)
#define P4_USBD_BIT9_SET				(1<<9)
#define P4_USBD_BIT10_SET				(1<<10)
#define P4_USBD_BIT11_SET				(1<<11)
#define P4_USBD_BIT12_SET				(1<<12)
#define P4_USBD_BIT13_SET				(1<<13)
#define P4_USBD_BIT14_SET				(1<<14)
#define P4_USBD_BIT15_SET				(1<<15)

#define P4_USBD_MAX_DEV_PORT			8	/* include hub */
#define P4_USBD_MAX_DEV_NUM				(P4_USBD_MAX_DEV_PORT-1)
#define P4_USBD_USE_DEV_NUM				4
#define P4_USBD_DEV_EP_NUM				4 /* endpoint for each device */
#define P4_USBD_EP0_BUF_SIZE			64
#define P4_USBD_INTR_EP_SIZE			64
#define P4_USBD_BULK_EP_SIZE			512
#define P4_USBD_MAX_IN_EP				12
#define P4_USBD_MAX_OUT_EP				8
/* Sometimes the length of received data of DMA is over than BULK_EP_SIZE. */
#define P4_USBD_MAX_DMA_PKTS			(P4_USBD_BULK_EP_SIZE*2)
#define P4_USBD_REG_BASE				0x40800000

#define P4_USBD_IRQ_HUB					62
#define P4_USBD_IRQ_DEV0				63
#define P4_USBD_IRQ_DEV1				64
#define P4_USBD_IRQ_DEV2				65
#define P4_USBD_IRQ_DEV3				66
#define P4_USBD_IRQ_DEV4				67
#define P4_USBD_IRQ_DEV5				68
#define P4_USBD_IRQ_DEV6				69

#define P4_USBD_EP_SINGLE_BUFFER		0x0
#define P4_USBD_EP_DOUBLE_BUFFER		0x1
#define P4_USBD_EP_TRIPLE_BUFFER		0x2
#define P4_USBD_EP_QUAD_BUFFER			0x3
#define P4_USBD_EP_EP_VALID				0x80

#define P4_USBD_OUT0BC			 		0x000
#define P4_USBD_IN0BC			 		0x001
#define P4_USBD_EP0CS			 		0x002
#define P4_USBD_EP0INDAT		 		0x100
/* 8 bytes Setup Data buffer */
#define P4_USBD_SETUPDAT				0x180 /* 0x180 to 0x187 */
#define P4_USBD_IN07IRQ					0x188
#define P4_USBD_IN812IRQ				0x189
#define P4_USBD_USBIRQ					0x18C
#define P4_USBD_IN07IEN					0x194
#define P4_USBD_OUT07IEN				0x196
#define P4_USBD_USBIEN					0x198
#define P4_USBD_IVECT					0x1A0
#define P4_USBD_ENDPRST					0x1A2
#define P4_USBD_USBCS					0x1A3
#define P4_USBD_FIFOCTRL				0x1A8
#define P4_USBD_INTR_EN					0x1B1
#define P4_USBD_INDMAADDRX(ep)			(0x400 + (8 * (ep)))
#define P4_USBD_INDMACTRLX(ep)			(0x404 + (8 * (ep)))
#define P4_USBD_OUTDMAADDRX(ep)			(0x480 + (8 * (ep)))
#define P4_USBD_OUTDMACTRLX(ep)			(0x484 + (8 * (ep)))
#define P4_USBD_BMCINTEN				0x700
#define P4_USBD_BMCINTSTS				0x704
#define P4_USBD_SSPDEVLOCK				0x718
#define P4_USBD_PORTENABLE				0x720

#define P4_USBD_DEVXEP0INTEN(port)		(0x800 + (0x100 * (port)))
#define P4_USBD_DEVXEPXINTEN(port)		(0x804 + (0x100 * (port)))
#define P4_USBD_DEVXDMACHXINTEN(port)	(0x808 + (0x100 * (port)))
#define P4_USBD_DEVX2HUBINTEN(port)		(0x80C + (0x100 * (port)))
#define P4_USBD_DEVXIVECT(port)			(0x810 + (0x100 * (port)))
#define P4_USBD_DEVXEP0INTSTS(port)		(0x814 + (0x100 * (port)))
#define P4_USBD_DEVXEPXINTSTS(port)		(0x818 + (0x100 * (port)))
#define P4_USBD_DEVXDMACHXINTSTS(port)	(0x81C + (0x100 * (port)))
#define P4_USBD_DEVXDISCONDB(port)		(0x830 + (0x100 * (port)))
#define P4_USBD_DEVXPORTRSTDB(port)		(0x834 + (0x100 * (port)))
#define P4_USBD_DEVX2HUBPORTRSTDB(port)	(0x83C + (0x100 * (port)))
#define P4_USBD_DEVX2HUBINTSTS(port)	(0x850 + (0x100 * (port)))
#define P4_USBD_DEVXOUT0BC(port)		(0x1000 + (0x1000 * (port)))
#define P4_USBD_DEVXIN0BC(port)			(0x1001 + (0x1000 * (port)))
#define P4_USBD_DEVXEP0CTRL(port)		(0x1002 + (0x1000 * (port)))
#define P4_USBD_DEVXIN0BUF(port)		(0x1100 + (0x1000 * (port)))
#define P4_USBD_DEVXOUT0BUF(port)		(0x1140 + (0x1000 * (port)))
#define P4_USBD_DEVXSETUPBUF(port)		(0x1180 + (0x1000 * (port)))
#define P4_USBD_DEVXFNADDR(port)		(0x11A6 + (0x1000 * (port)))

/* Detail define */
/* P4_USBD_EP0CS 0x2 */
#define P4_USBD_EP0CS_STALL				0x01
#define P4_USBD_EP0CS_HSNAK				0x02
#define P4_USBD_EP0CS_INBSY				0x04
#define P4_USBD_EP0CS_OUTBSY			0x08
/* P4_USBHUB_USBIRQ 18C */
#define P4_USBHUB_USBIRQ_SUDAVIR		(1 << 0)
#define P4_USBHUB_USBIRQ_SOFIR			(1 << 1)
#define P4_USBHUB_USBIRQ_SUTOKIR		(1 << 2)
#define P4_USBHUB_USBIRQ_SUSPIR			(1 << 3)
#define P4_USBHUB_USBIRQ_URESIR			(1 << 4)
#define P4_USBHUB_USBIRQ_HSPEEDIR		(1 << 5)
/* P4_USBD_USBIEN 198 */
#define P4_USBD_USBIEN_SUDAVIE			(1 << 0)
#define P4_USBD_USBIEN_SOFIE			(1 << 1)
#define P4_USBD_USBIEN_SUTOKIE			(1 << 2)
#define P4_USBD_USBIEN_SUSPIE			(1 << 3)
#define P4_USBD_USBIEN_URESIE			(1 << 4)
#define P4_USBD_USBIEN_HSPIE			(1 << 5)
/* P4_USBHUB_ENDPRST 1A2 */
#define P4_USBD_ENDPRST_EPN				(0x0F)
#define P4_USBD_ENDPRST_DIR				(1 << 4)
#define P4_USBD_ENDPRST_TOGRST			(1 << 5)
#define P4_USBD_ENDPRST_FIFORST			(1 << 6)
#define P4_USBD_ENDPRST_TOGGLE			(1 << 7)
/* P4_USBD_FIFOCTRL 1A8 */
#define P4_USBD_FIFOCTRL_EP				(0x0F)
#define P4_USBD_FIFOCTRL_DIR			(1 << 4)
#define P4_USBD_FIFOCTRL_AUTO			(1 << 5)
#define P4_USBD_FIFOCTRL_CMIT			(1 << 6)
#define P4_USBD_FIFOCTRL_ACC			(1 << 7)
/* P4_USBHUB_INTR_EN 1B1 */
#define P4_USBD_INTR_EN_INTEN			(1 << 0)
#define P4_USBD_INTR_EN_DMAEN			(1 << 1)
#define P4_USBD_INTR_EN_WUEN			(1 << 2)
#define P4_USBD_INTR_EN_FIFOEN			(1 << 3)

#endif /* __P4_USBD_H__ */

