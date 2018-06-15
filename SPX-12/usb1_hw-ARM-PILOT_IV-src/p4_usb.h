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

#ifndef __P4_USB_H__
#define __P4_USB_H__		// Followed USB SPEC2.0 USED BY P4

#include <linux/usb/ch9.h>

/* Hub request types */
#define USB_RT_HUB	(USB_TYPE_CLASS | USB_RECIP_DEVICE)
#define USB_RT_PORT	(USB_TYPE_CLASS | USB_RECIP_OTHER)

/* Hub descriptor, See USB 2.0 spec Table 11-13 */
#define USB_DT_HUB			(USB_TYPE_CLASS | 0x09)
#define USB_DT_HUB_SIZE		9
/* we have 7 ports, so DeviceRemovable and PortPwrCtrlMask only has 1 byte */
struct usb_hub_descriptor {
	int8_t  bDescLength;
	int8_t  bDescriptorType;
	int8_t  bNbrPorts;
	__le16 wHubCharacteristics;
	int8_t  bPwrOn2PwrGood;
	int8_t  bHubContrCurrent;
	/* add 1 bit for hub status change; round to bytes */
	int8_t  DeviceRemovable;
	int8_t  PortPwrCtrlMask;
} __attribute__ ((packed));

/*
 * wHubCharacteristics (masks)
 * See USB 2.0 spec Table 11-13, offset 3
 */
#define HUB_CHAR_LPSM			0x0003 /* D1 .. D0 */
#define HUB_CHAR_COMPOUND		0x0004 /* D2       */
#define HUB_CHAR_OCPM			0x0018 /* D4 .. D3 */
#define HUB_CHAR_TTTT			0x0060 /* D6 .. D5 */
#define HUB_CHAR_PORTIND		0x0080 /* D7       */

#define HUB_CHAR_LPSM_GANGED	0x0000
#define HUB_CHAR_LPSM_IDV		0x0001
#define HUB_CHAR_OCPM_GLOBAL	0x0000
#define HUB_CHAR_OCPM_IDV		0x0008
#define HUB_CHAR_TTTT_8			0x0000
#define HUB_CHAR_TTTT_16		0x0020
#define HUB_CHAR_TTTT_24		0x0040
#define HUB_CHAR_TTTT_32		0x0060

/*
 * Port feature numbers
 * See USB 2.0 spec Table 11-17
 */
#define USB_PORT_FEAT_CONNECTION			0
#define USB_PORT_FEAT_ENABLE				1
#define USB_PORT_FEAT_SUSPEND				2
#define USB_PORT_FEAT_OVER_CURRENT			3
#define USB_PORT_FEAT_RESET					4
#define USB_PORT_FEAT_POWER					8
#define USB_PORT_FEAT_LOWSPEED				9
#define USB_PORT_FEAT_C_CONNECTION			16
#define USB_PORT_FEAT_C_ENABLE				17
#define USB_PORT_FEAT_C_SUSPEND				18
#define USB_PORT_FEAT_C_OVER_CURRENT		19
#define USB_PORT_FEAT_C_RESET				20
#define USB_PORT_FEAT_TEST					21
#define USB_PORT_FEAT_INDICATOR				22

/* 
 * Hub Status and Hub Change
 * See USB 2.0 spec Table 11-19 and Table 11-20
 */
struct usb_hub_status {
	__le16 wHubStatus;
	__le16 wHubChange;
} __attribute__ ((packed));
#define USB_HUB_STATUS_SIZE sizeof(struct usb_hub_status)

/*
 * Hub Status & Hub Change bit masks
 * See USB 2.0 spec Table 11-19 and Table 11-20
 * Bits 0 and 1 for wHubStatus and wHubChange
 * Bits 2 to 15 are reserved for both
 */
#define HUB_STATUS_LOCAL_POWER		0x0001
#define HUB_STATUS_OVERCURRENT		0x0002
#define HUB_CHANGE_LOCAL_POWER		0x0001
#define HUB_CHANGE_OVERCURRENT		0x0002

/* 
 * Port Status and Port Change
 * See USB 2.0 spec Table 11-21 and Table 11-22
 */
struct usb_port_status {
	__le16 wPortStatus;
	__le16 wPortChange;	
} __attribute__ ((packed));
#define USB_PORT_STATUS_SIZE sizeof(struct usb_port_status)

/* 
 * wPortStatus bit field
 * See USB 2.0 spec Table 11-21
 */
#define USB_PORT_STAT_CONNECTION	0x0001
#define USB_PORT_STAT_ENABLE		0x0002
#define USB_PORT_STAT_SUSPEND		0x0004
#define USB_PORT_STAT_OVERCURRENT	0x0008
#define USB_PORT_STAT_RESET			0x0010
/* bits 5 to 7 are reserved */
#define USB_PORT_STAT_POWER			0x0100
#define USB_PORT_STAT_LOW_SPEED		0x0200
#define USB_PORT_STAT_HIGH_SPEED	0x0400
#define USB_PORT_STAT_TEST			0x0800
#define USB_PORT_STAT_INDICATOR		0x1000
/* bits 13 to 15 are reserved */

/* 
 * wPortChange bit field
 * See USB 2.0 spec Table 11-22
 */
#define USB_PORT_STAT_C_CONNECTION	0x0001
#define USB_PORT_STAT_C_ENABLE		0x0002
#define USB_PORT_STAT_C_SUSPEND		0x0004
#define USB_PORT_STAT_C_OVERCURRENT	0x0008
#define USB_PORT_STAT_C_RESET		0x0010
/* bits 5 to 15 are reserved */

#endif /* __P4_USB_H__ */
