/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/fs.h>
#include <coreTypes.h>
#include "usb_hw.h"
#include "bot.h"
#include "iusb.h"
#include "usb_ioctl.h"
#include "usb_core.h"
#include "helper.h"
#include "dbgout.h"
#include "mod_reg.h"
#include "iusb-inc.h"

/* Local Definitions */
#define DEBUG_HDISK			0x01
#define VENDOR_ID			1131	/* AMI = Decimal 1131 */
#define PRODUCT_ID_HUB		0xFF01
#define PRODUCT_ID_COMP_HID	0xFF10	/*Composite Hid Device : Mouse & Keybd*/
#define PRODUCT_ID_MOUSE	0xFF11
#define PRODUCT_ID_KEYBD	0xFF12
#define PRODUCT_ID_CDROM	0xFF20
#define PRODUCT_ID_FIX_HD	0xFF30	/* Fixed Harddisk */
#define PRODUCT_ID_REM_HD	0xFF31	/* Removable Harddisk */
#define PRODUCT_ID_FLOPPY	0xFF40
#define PRODUCT_ID_SUP_COMP	0xFF90	/* All in One - Super Combo */
#define PRODUCT_ID_COMP	    0xFF91	/* CD & Floppy Combo */
#define PRODUCT_ID_LUN_COMP 0xFF92	/* CD & Floppy Lun Based Combo */
#define DEVICE_REV			0x0100
#define AMI_HDISK_SERIAL_NUM_STRING "AAAABBBBCCCC3"

#if defined(SOC_AST2300) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_PILOT_IV) || defined(SOC_AST1250)
#define MAX_HD_INSTANCES		4
#define MAX_SD_INSTANCES		1
#define MAX_LUN_SUPPORT		(MAX_HD_INSTANCES + MAX_SD_INSTANCES)
#else
#define MAX_LUN_SUPPORT		0x05
#endif

/* Global Variables */
TDBG_DECLARE_DBGVAR(hdisk);

/* Function Prototypes */
static int CreateHdiskDescriptor(uint8 DevNo);
static int HdiskIoctl (unsigned int cmd,unsigned long arg, int* RetVal);
static int FillDevInfo (IUSB_DEVICE_INFO* iUSBDevInfo);
static int RemoteScsiCall(uint8 DevNo, uint8 ifnum, BOT_IF_DATA *MassData, uint8 LunNo);
static int SetInterfaceAuthKey (uint8 DevNo, uint8 IfNum, uint32 Key);
static int ClearInterfaceAuthKey (uint8 DevNo, uint8 IfNum);

/* Static Variables */
static uint32 IfaceAuthKeys[1] = {0};
static USB_DEV	UsbDev = {
	.DevUsbCreateDescriptor 	= CreateHdiskDescriptor,
	.DevUsbIOCTL 				= HdiskIoctl,
	.DevUsbSetKeybdSetReport	= NULL,
	.DevUsbMiscActions 			= NULL,
	.DevUsbRemoteScsiCall 		= RemoteScsiCall,
	.DevUsbFillDevInfo 			= FillDevInfo,
	.DevUsbSetInterfaceAuthKey	= SetInterfaceAuthKey,
	.DevUsbClearInterfaceAuthKey	= ClearInterfaceAuthKey,
				};
static USB_CORE UsbCore;
static USB_IUSB iUSB;

static int 	hdisk_notify_sys(struct notifier_block *this, unsigned long code, void *unused);
static struct notifier_block hdisk_notifier =
{
       .notifier_call = hdisk_notify_sys,
};

static char *HWModuleName = NULL;
static usb_device_driver hdisk_device_driver =
{
	.module 		= THIS_MODULE,
	.descriptor     = CREATE_HARDDISK_DESCRIPTOR,
	.devnum         = 0xFF,
#if defined SOC_PILOT_IV
	.speeds         = (SUPPORT_HIGH_SPEED),
#else
	.speeds         = (SUPPORT_FULL_SPEED | SUPPORT_HIGH_SPEED),
#endif
	.maxlun			= 0, // it is 0 based field, 1 means this device uses LUN0 and LUN1
	.DisconnectOnRegister = 1,
};

static struct ctl_table_header *my_sys 	= NULL;
static struct ctl_table hdiskctlTable [] =
{
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
        {"DebugLevel",&(TDBG_FORM_VAR_NAME(hdisk)),sizeof(int),0644,NULL,&proc_dointvec,NULL},
#else 
    #if (LINUX_VERSION_CODE >  KERNEL_VERSION(2,6,15)) 
        {CTL_UNNUMBERED,"DebugLevel",&(TDBG_FORM_VAR_NAME(hdisk)),sizeof(int),0644,NULL,NULL,&proc_dointvec},
    #else
        {1,"DebugLevel",&(TDBG_FORM_VAR_NAME(hdisk)),sizeof(int),0644,NULL,&proc_dointvec},
#endif
#endif
        {0}
};
static uint8 NumLunComboHD = 0;
static uint8 LunNumAssignment [MAX_LUN_SUPPORT] = { 0xFF };
static uint8 HDInstannceAssignment [MAX_LUN_SUPPORT] = { 0xFF };

static int Interval = 0;
module_param (HWModuleName, charp, 0000);
MODULE_PARM_DESC(HWModuleName, "which hardware module is correspond to Hard disk device module");
module_param (Interval, int, S_IRUGO);
MODULE_PARM_DESC(Interval, "bInterval field of Endpoint Descriptor");

static int
AuthenticateIOCTLCaller (unsigned long arg, int* RetVal, int argHasIUSBHeader)
{
	uint32 AuthKey;
	int Temp;
	uint8 Instance;
	uint8 IfNum;
	IUSB_HEADER iUsbHeader;
	IUSB_IOCTL_DATA IoctlData;

	if (argHasIUSBHeader)
	{
		Temp = __copy_from_user((void *)(&iUsbHeader),(void*)arg, sizeof(IUSB_HEADER));
		AuthKey = iUsbHeader.Key;
		IfNum = iUsbHeader.InterfaceNo;
		Instance = iUsbHeader.Instance;
	}
	else
	{
		Temp = __copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA));
		AuthKey = IoctlData.Key;
		IfNum = IoctlData.DevInfo.IfNum;
		Instance = IoctlData.DevInfo.Instance;
	}
	
	if (Instance >= MAX_LUN_SUPPORT)
	{
		TWARN ("AuthenticateIOCTLCaller(): Invalid Instance %d\n", Instance);
		*RetVal = -EINVAL;
		return -1;
	}
	if (HDInstannceAssignment[Instance] == 0xFF)
	{
		TWARN ("AuthenticateIOCTLCaller(): Invalid Instance %d\n", Instance);
		*RetVal = -EINVAL;
		return -1;
	}
	if (Temp)
	{
		TWARN ("AuthenticateIOCTLCaller(): __copy_from_user failed\n");
		*RetVal = -EFAULT;
		return -1;
	}
	if (IfNum > 0)
	{
		TWARN ("AuthenticateIOCTLCaller(): Invalid Interface Number %d\n", IfNum);
		*RetVal = -EINVAL;
		return -1;
	}
	if (0 == IfaceAuthKeys[IfNum])
	{
		TWARN ("AuthenticateIOCTLCaller(): Auth Key has not been set for this interface\n");
		*RetVal = -EPERM;
		return -1;
	}
	if (AuthKey != IfaceAuthKeys[IfNum])
	{
		TWARN ("AuthenticateIOCTLCaller(): Auth Key mismatch, expected %x received %x\n",
														IfaceAuthKeys[IfNum], AuthKey);
		*RetVal = -EPERM;
		return -1;
	}
	return 0;
}

static int
HdiskIoctl (unsigned int cmd,unsigned long arg, int* RetVal)
{
	uint8 IfNum;
	uint8 DevNo;
	uint8 HdiskType;
	BOT_IF_DATA *BotIfData;
	SCSI_INQUIRY_PACKET *Inquiry;
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
	SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET *EventStatus;
#endif
	int i;
	uint8 Instance;
	IUSB_IOCTL_DATA IoctlData;
	
	switch (cmd)
	{

	case USB_HDISK_REQ:	
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"USB_HDISK_REQ IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 1))
		{
			TWARN ("HdiskIoctl(): Invalid Auth Key in USB_HDISK_REQ IOCTL\n");
			return 0;
		}
		if (__copy_from_user((void *)(&Instance),&((IUSB_HEADER*)arg)->Instance, sizeof(uint8)))
		{
			TWARN ("HdiskIoctl(): __copy_from_user failed in USB_HDISK_REQ IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		/* This call will sleep and forms a complete IUSB Packet on Return */
		*RetVal = iUSB.iUSBHdiskRemoteWaitRequest((IUSB_SCSI_PACKET *)arg, 
					HDInstannceAssignment[Instance]);
		return 0;

	case USB_HDISK_RES:
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"USB_HDISK_RES IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 1))
		{
			TWARN ("HdiskIoctl(): Invalid Auth Key in USB_HDISK_RES IOCTL\n");
			return 0;
		}
		*RetVal = iUSB.iUSBScsiSendResponse((IUSB_SCSI_PACKET *)arg);
		return 0;

	case USB_HDISK_ACTIVATE:
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"USB_HDISK_ACTIVATE IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HdiskIoctl(): Invalid Auth Key in USB_HDISK_ACTIVATE IOCTL\n");
			return 0;
		}
		if (__copy_from_user((void *)(&Instance),&((IUSB_IOCTL_DATA*)arg)->DevInfo.Instance, sizeof(uint8)))
		{
			TWARN ("HdiskIoctl(): __copy_from_user failed in USB_HDISK_ACTIVATE IOCTL\n");
			*RetVal = -1;
			return 0;
		}
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("HdiskIoctl(): __copy_from_user failed in USB_HDISK_ACTIVATE IOCTL\n");
			*RetVal = -1;
			return 0;
		}

		IfNum = IoctlData.DevInfo.IfNum;
		DevNo = IoctlData.DevInfo.DevNo;
		BotIfData = (BOT_IF_DATA*) UsbCore.CoreUsbGetInterfaceData (DevNo, IfNum);
		EventStatus = &(BotIfData->EventStatus[Instance]);
		EventStatus->EventCode 	 = 0x02; // NewMedia
		EventStatus->MediaStatus = 0x02; // Media Present
#endif
		*RetVal = iUSB.iUSBHdiskRemoteActivate (HDInstannceAssignment[Instance]);
		return 0;

	case USB_HDISK_EXIT:
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"USB_HDISK_EXIT IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HdiskIoctl(): Invalid Auth Key in USB_HDISK_EXIT IOCTL\n");
			return 0;
		}
		if (__copy_from_user((void *)(&Instance),&((IUSB_IOCTL_DATA*)arg)->DevInfo.Instance, sizeof(uint8)))
		{
			TWARN ("HdiskIoctl(): __copy_from_user failed in USB_HDISK_EXIT IOCTL\n");
			*RetVal = -1;
			return 0;
		}
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("CdromIoctl(): __copy_from_user failed in USB_HDISK_EXIT IOCTL\n");
			*RetVal = -1;
			return 0;
		}

		IfNum = IoctlData.DevInfo.IfNum;
		DevNo = IoctlData.DevInfo.DevNo;
		BotIfData = (BOT_IF_DATA*) UsbCore.CoreUsbGetInterfaceData (DevNo, IfNum);
		EventStatus = &(BotIfData->EventStatus[Instance]);
		EventStatus->EventCode   = 0x01; // EjectRequest
		EventStatus->MediaStatus = 0x02; // Media Present
#endif

		*RetVal = iUSB.iUSBHdiskRemoteDeactivate (HDInstannceAssignment[Instance]);
		return 0;
		
	case USB_DEVICE_DISCONNECT:
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"USB_DEVICE_DISCONNECT IOCTL...\n");
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("HdiskIoctl(): __copy_from_user failed in USB_DEVICE_DISCONNECT IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if (hdisk_device_driver.devnum != IoctlData.DevInfo.DevNo)
			return -1;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HdiskIoctl(): Invalid Auth Key in USB_DEVICE_DISCONNECT IOCTL\n");
			return 0;
		}
		*RetVal = UsbCore.CoreUsbDeviceDisconnect (hdisk_device_driver.devnum);
		return 0;
		
	case USB_DEVICE_RECONNECT:
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"USB_DEVICE_RECONNECT IOCTL...\n");
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("HdiskIoctl(): __copy_from_user failed in USB_DEVICE_RECONNECT IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if (hdisk_device_driver.devnum != IoctlData.DevInfo.DevNo)
			return -1;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HdiskIoctl(): Invalid Auth Key in USB_DEVICE_RECONNECT IOCTL\n");
			return 0;
		}
		*RetVal = UsbCore.CoreUsbDeviceReconnect (hdisk_device_driver.devnum);
		return 0;			

	case USB_HDISK_SET_TYPE:
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"USB_HDISK_SET_TYPE IOCTL...\n");
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("HdiskIoctl(): __copy_from_user failed in USB_HDISK_SET_TYPE IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		IfNum = IoctlData.DevInfo.IfNum;
		DevNo = IoctlData.DevInfo.DevNo;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HdiskIoctl(): Invalid Auth Key in USB_HDISK_SET_TYPE IOCTL\n");
			return 0;
		}
		HdiskType = IoctlData.Data;
		BotIfData = (BOT_IF_DATA*) UsbCore.CoreUsbGetInterfaceData (DevNo, IfNum);
		Inquiry = &(BotIfData->Inquiry[IoctlData.DevInfo.Instance]);
		Inquiry->RMB = HdiskType;
		*RetVal = 0;
		return 0;
		
	case USB_HDISK_GET_TYPE:
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"USB_HDISK_GET_TYPE IOCTL...\n");
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("HdiskIoctl(): __copy_from_user failed in USB_HDISK_GET_TYPE IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		IfNum = IoctlData.DevInfo.IfNum;
		DevNo = IoctlData.DevInfo.DevNo;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HdiskIoctl(): Invalid Auth Key in USB_HDISK_GET_TYPE IOCTL\n");
			return 0;
		}
		BotIfData = (BOT_IF_DATA*) UsbCore.CoreUsbGetInterfaceData (DevNo, IfNum);
		Inquiry = &(BotIfData->Inquiry[IoctlData.DevInfo.Instance]);
		*RetVal = Inquiry->RMB;
		return 0;	

	case USB_HDISK_RST_IF:
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"USB_HDISK_RST_IF IOCTL...\n");
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("HdiskIoctl(): __copy_from_user failed in USB_HDISK_RST_IF IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if (hdisk_device_driver.devnum != IoctlData.DevInfo.DevNo)
			return -1;
		DevNo = IoctlData.DevInfo.DevNo;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HdiskIoctl(): Invalid Auth Key in USB_HDISK_RST_IF IOCTL\n");
			return 0;
		}
		hdisk_device_driver.maxlun = 0;
		NumLunComboHD = 0;
		for (i = 0; i < MAX_LUN_SUPPORT; i++)
		{
			if (iUSB.iUSBReleaseHdiskInstanceIndex (i))
			{
				TWARN ("HdiskIoctl(): Error Releasing HDISK Instance %d\n", i);
			}
		}
		memset (LunNumAssignment, 0xFF, sizeof (LunNumAssignment));
		UsbCore.CoreUsbSetNumLUN (DevNo, hdisk_device_driver.maxlun);

		*RetVal = 0;
		return 0;

	case USB_HDISK_ADD_HD:
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"USB_HDISK_ADD_HD IOCTL...\n");
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("HdiskIoctl(): __copy_from_user failed in USB_HDISK_ADD_HD IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if (hdisk_device_driver.devnum != IoctlData.DevInfo.DevNo)
			return -1;
		IfNum = IoctlData.DevInfo.IfNum;
		DevNo = IoctlData.DevInfo.DevNo;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HdiskIoctl(): Invalid Auth Key in USB_HDISK_ADD_HD IOCTL\n");
			return 0;
		}
		BotIfData = (BOT_IF_DATA*) UsbCore.CoreUsbGetInterfaceData (DevNo, IfNum);
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
		EventStatus = &(BotIfData->EventStatus[hdisk_device_driver.maxlun]);
		EventStatus->EventCode   = 0x0;  // 0h NoChg
		EventStatus->MediaStatus = 0x1; // 00b Door or Tray opena
#endif
		Inquiry = &(BotIfData->Inquiry[hdisk_device_driver.maxlun]);
		
		Inquiry->ISO_ECMA_ANSI		=	0;
		Inquiry->ResponseDataFormat =	1;
		Inquiry->AdditionalLength	=	sizeof(SCSI_INQUIRY_PACKET)-5;

		Inquiry->PeripheralType 	=	PERIPHERAL_DIRECT;
		Inquiry->RMB				=	MEDIUM_REMOVABLE;

#ifdef CONFIG_SPX_FEATURE_HDD_MEDIA_VENDOR_INFO

        memset((void *)&Inquiry->VendorInfo[0], ' ', 8);
        memset((void *)&Inquiry->ProductRev[0], ' ', 4);
        memset((void *)&Inquiry->ProductInfo[0], ' ', 16);

        if ( ( strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_VENDOR_INFO) <= 8 ) && 
             ( strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_REVISION) <= 4) && 
             ( strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_INFO) <= 13 ) )
        {
             strncpy((char *)&Inquiry->VendorInfo[0], CONFIG_SPX_FEATURE_HDD_MEDIA_VENDOR_INFO, strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_VENDOR_INFO));
             strncpy((char *)&Inquiry->ProductRev[0], CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_REVISION, strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_REVISION));
             strncpy((char *)&Inquiry->ProductInfo[0], CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_INFO, strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_INFO));
        }
        else
        {
             TCRIT (" Length of any Hard Disk Media Configuration is out of range");
             return -1;
        }
#else
        strncpy((char *)&Inquiry->VendorInfo[0],"AMI     ",8);
        strncpy((char *)&Inquiry->ProductRev[0],"1.00",4);
        strncpy((char *)&Inquiry->ProductInfo[0],"Virtual HDisk   ",16);
#endif

		Inquiry->ProductInfo[13] = 0x30 + NumLunComboHD;

		LunNumAssignment[hdisk_device_driver.maxlun] = iUSB.iUSBGetHdiskInstanceIndex ();
		if (0xff == LunNumAssignment[hdisk_device_driver.maxlun])
		{
			TCRIT ("HdiskIoctl: Unable to get Free HDISK instance from iUSB module");
			return -1;
		}
		HDInstannceAssignment[NumLunComboHD] = LunNumAssignment[hdisk_device_driver.maxlun];
		NumLunComboHD++;
		UsbCore.CoreUsbSetNumLUN (DevNo, hdisk_device_driver.maxlun);
		hdisk_device_driver.maxlun++;
		*RetVal = 0;
		return 0;


	default:
		TDBG_FLAGGED(hdisk, DEBUG_HDISK,"Unsupported Hdisk IOCTL received...\n");
			return -1;
	}

}

static int
CreateHdiskDescriptor(uint8 DevNo)
{
	uint8 *MyDescriptor;
	uint16 TotalSize = 0;
	SCSI_INQUIRY_PACKET *Inquiry;
	BOT_IF_DATA *IfData;
    uint32 inquiry_data = 0;
	USB_DEVICE_DESC	*DevDesc;

	INTERFACE_INFO HDIfInfo =
	{
		NULL,UsbCore.CoreUsbBotRxHandler,UsbCore.CoreUsbBotReqHandler,UsbCore.CoreUsbBotProcessHandler,
		UsbCore.CoreUsbBotHaltHandler,UsbCore.CoreUsbBotUnHaltHandler,UsbCore.CoreUsbBotRemHandler,
		NULL,0,0,0,0

	};  
	
	/***********************CREATE HDisk INTERFACE DATA*********************/
	HDIfInfo.InterfaceData =(void *) vmalloc(sizeof(BOT_IF_DATA));
	if (HDIfInfo.InterfaceData == NULL)
	{
		TCRIT("CreateHdiskDescriptor():Memory Alloc Failure for Interface\n");
		return -1;
	}

	/* Initialize Interface Data */
	IfData = (BOT_IF_DATA*)(HDIfInfo.InterfaceData);

	UsbCore.CoreUsb_OsInitSleepStruct(&(IfData->UnHaltedSleep));
	IfData->UnHalted 		= 1;
	IfData->SenseKey		= 0;
	IfData->SenseCode		= 0;
	IfData->SenseCodeQ		= 0;
	IfData->NoMediumRetries = MAX_NOMEDIUM_RETRIES;
	IfData->ScsiDataOutLen		= 0;
	IfData->LastSeqNo		= 0;	/* Invalid Seq No */
    inquiry_data = sizeof(SCSI_INQUIRY_PACKET) * (MAX_LUN_SUPPORT + 1);
	
	IfData->Inquiry = (SCSI_INQUIRY_PACKET*) vmalloc (inquiry_data);
	if (IfData->Inquiry == NULL)
	{
		TCRIT("CreateHdiskDescriptor():Memory Alloc Failure for Inquiry Data\n");
		return -1;
	}	
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
	IfData->EventStatus = (SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET*) vmalloc (sizeof(SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET)* 
							(MAX_LUN_SUPPORT + 1));
	if (IfData->EventStatus == NULL)
	{
		vfree (IfData->Inquiry);
		TCRIT("CreateHdiskDescriptor():Memory Alloc Failure for Interface\n");
		return -1;
	}
	IfData->EventStatus->EventCode   = 0;
	IfData->EventStatus->MediaStatus = 1;
#endif

	/* Form the Inquiry Packet Packet */
	Inquiry = &(IfData->Inquiry[LUN_0]);
	memset((void *)Inquiry, 0, inquiry_data);

	Inquiry->ISO_ECMA_ANSI		=	0;
	Inquiry->ResponseDataFormat	=	1;

	Inquiry->AdditionalLength	=	sizeof(SCSI_INQUIRY_PACKET)-5;

	/* HardDisk Specific Values in Inquiry Packet */
	Inquiry->PeripheralType		= 	PERIPHERAL_DIRECT;
	Inquiry->RMB				= 	MEDIUM_REMOVABLE;

#ifdef CONFIG_SPX_FEATURE_HDD_MEDIA_VENDOR_INFO

    memset((void *)&Inquiry->VendorInfo[0], ' ', 8);
    memset((void *)&Inquiry->ProductRev[0], ' ', 4);
    memset((void *)&Inquiry->ProductInfo[0], ' ', 16);

    if ( ( strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_VENDOR_INFO) <= 8 ) && 
         ( strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_REVISION) <= 4) && 
         ( strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_INFO) <= 13 ) )
    {
         strncpy((char *)&Inquiry->VendorInfo[0], CONFIG_SPX_FEATURE_HDD_MEDIA_VENDOR_INFO, strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_VENDOR_INFO));
         strncpy((char *)&Inquiry->ProductRev[0], CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_REVISION, strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_REVISION));
         strncpy((char *)&Inquiry->ProductInfo[0], CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_INFO, strlen(CONFIG_SPX_FEATURE_HDD_MEDIA_PRODUCT_INFO));
    }
    else
    {
         TCRIT (" Length of any Hard Disk Media Configuration is out of range");
         return -1;
    }
#else
    strncpy((char *)&Inquiry->VendorInfo[0],"AMI     ",8);
    strncpy((char *)&Inquiry->ProductRev[0],"1.00",4);
    strncpy((char *)&Inquiry->ProductInfo[0],"Virtual HDisk   ",16);
#endif

	
	/****************** Initialize FDisk  Interface Data End ****************/

	TotalSize = sizeof(USB_DEVICE_DESC);
	TotalSize += sizeof(USB_CONFIG_DESC);
	TotalSize += sizeof(USB_INTERFACE_DESC);
	TotalSize += (2*sizeof(USB_ENDPOINT_DESC));

	MyDescriptor = UsbCore.CoreUsbCreateDescriptor(DevNo,TotalSize);
	if (MyDescriptor == NULL)
	{
		TCRIT("CreateHdiskDescriptor(): Memory Allocation Failure\n");
		vfree (IfData->Inquiry);
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
		vfree (IfData->EventStatus);
#endif
		return -1;
	}

	if (UsbCore.CoreUsbAddDeviceDesc(DevNo,VENDOR_ID,PRODUCT_ID_REM_HD,DEVICE_REV,
				"American Megatrends Inc.","Virtual HardDisk Device", 0) != 0)
	{
		TCRIT("CreateHdiskDescriptor: Error in creating Device Desc\n");
		vfree (IfData->Inquiry);
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
		vfree (IfData->EventStatus);
#endif
		return -1;
	}
	
	DevDesc = UsbCore.CoreUsbGetDeviceDesc (DevNo);
	DevDesc->iSerialNumber = UsbCore.CoreUsbAddStringDescriptor(DevNo, AMI_HDISK_SERIAL_NUM_STRING);

	if (UsbCore.CoreUsbAddCfgDesc(DevNo,1,0) != 0)
	{
		TCRIT("CreateHdiskDescriptor(): Error in creating Cfg Desc\n");
		vfree (IfData->Inquiry);
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
		vfree (IfData->EventStatus);
#endif
		return -1;
	}
	
	if (UsbCore.CoreUsbAddInterfaceDesc(DevNo,2,MASS_INTERFACE,MASS_SCSI_INTERFACE,
						MASS_BULK_ONLY_PROTOCOL,"Virtual HardDisk",&HDIfInfo) !=0)

	{
		TCRIT("CreateHdiskDescriptor(): Error in creating Interface Desc\n");
		vfree (IfData->Inquiry);
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
		vfree (IfData->EventStatus);
#endif
		return -1;
	}

	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,IN,BULK,0x40,0x200,Interval,DATA_EP) != 0)
	{
		TCRIT("CreateHdiskDescriptor(): Error in creating IN EndPointDesc\n");
		vfree (IfData->Inquiry);
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
		vfree (IfData->EventStatus);
#endif
		return -1;
	}

	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,OUT,BULK,0x40,0x200,Interval,DATA_EP) != 0)
	{
		TCRIT("CreateHdiskDescriptor(): Error in creating OUT EndPointDesc\n");
		vfree (IfData->Inquiry);
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500) || defined(SOC_AST1250)
		vfree (IfData->EventStatus);
#endif
		return -1;
	}
	return 0;				
}                                                               

static int
RemoteScsiCall(uint8 DevNo, uint8 ifnum, BOT_IF_DATA *MassData, uint8 LunNo)
{
	return iUSB.iUSBRemoteScsiCall (DevNo,ifnum,MassData,LunNo, LunNumAssignment[LunNo]);
}

static int
FillDevInfo (IUSB_DEVICE_INFO* iUSBDevInfo)
{
	uint8 NumDevices = 0;

	iUSBDevInfo->DeviceType = IUSB_DEVICE_HARDDISK;
	iUSBDevInfo->DevNo      = hdisk_device_driver.devnum;
    iUSBDevInfo->IfNum      = 0; 
	iUSBDevInfo->LockType	= LOCK_TYPE_SHARED;
	iUSBDevInfo++;
	NumDevices++;

	return NumDevices;
}

static int 
SetInterfaceAuthKey (uint8 DevNo, uint8 IfNum, uint32 Key)
{
	TDBG_FLAGGED(hdisk, DEBUG_HDISK,"SetInterfaceAuthKey() is called\n");
	if (DevNo != hdisk_device_driver.devnum)
	{
		TWARN ("SetInterfaceAuthKey(): Invalid Device Number %d\n", DevNo);
		return -1;
	}
	if (IfNum != 0)
	{
		TWARN ("SetInterfaceAuthKey(): Invalid Interface Number %d\n", IfNum);
		return -1;
	}
	IfaceAuthKeys[IfNum] = Key;
	return 0;
}

static int 
ClearInterfaceAuthKey (uint8 DevNo, uint8 IfNum)
{
	TDBG_FLAGGED(hdisk, DEBUG_HDISK,"ClearInterfaceAuthKey() is called\n");
	if (DevNo != hdisk_device_driver.devnum)
	{
		TWARN ("ClearInterfaceAuthKey(): Invalid Device Number %d\n", DevNo);
		return -1;
	}
	if (IfNum != 0)
	{
		TWARN ("ClearInterfaceAuthKey(): Invalid Interface Number %d\n", IfNum);
		return -1;
	}
	IfaceAuthKeys[IfNum] = 0;
	return 0;
}

static int
init_hdisk_device_module (void)
{
	int ret_val;
	int devnum;

	printk ("Loading Hdisk Device Module...\n");
	get_usb_core_funcs (&UsbCore);
	get_iusb_funcs (&iUSB);
	if ((HWModuleName != NULL) && (sizeof(hdisk_device_driver.name) > strlen(HWModuleName)))
	{
		if(snprintf(hdisk_device_driver.name,sizeof(hdisk_device_driver.name),"%s", HWModuleName)>=(signed)sizeof(hdisk_device_driver.name))
		TCRIT("init_hdisk_device_module-source buffer truncated\n");
	}
	devnum = UsbCore.CoreUsbGetFreeUsbDev (&hdisk_device_driver);
	if (devnum == -1)
	{
		TCRIT ("init_hdisk_device_module: Unable to get Free USB Hardware Module for Hard disk Device");
		return -1;
	}

	ret_val = iUSB.iUSBGetHdiskInstanceIndex ();
	if (-1 == ret_val)
	{
		TCRIT ("init_hdisk_device_module: Unable to get Free Hard disk instance from iUSB module");
		return -1;
	}
	
	LunNumAssignment[LUN_0] = (uint8)ret_val;
	HDInstannceAssignment[0] = LunNumAssignment[LUN_0];	
	

	hdisk_device_driver.devnum=devnum;
	if(NULL != DevInfo[devnum].UsbDevice.UsbHwSetCurrentDevType)
	{
		if(DevInfo[devnum].UsbDevice.UsbHwSetCurrentDevType (devnum, hdisk_device_driver.descriptor))
		{
			TCRIT ("Error setting Current registering Dev Type to USB Hw module.\n");
			return -1;
		}
	}
	if (0 != CreateHdiskDescriptor (devnum))
	{
		TCRIT ("init_hdisk_device_module: Unable to create hard disk device descriptor\n");
		UsbCore.CoreUsbDestroyDescriptor (devnum);
		return -1;
	}

	ret_val = register_usb_device (&hdisk_device_driver, &UsbDev);
	if (!ret_val) 
	{
		register_reboot_notifier (&hdisk_notifier);
		my_sys = AddSysctlTable("hdisk",&hdiskctlTable[0]);
	}
	else
	{
		TCRIT ("init_hdisk_device_module: hard disk Device failed to register with USB core\n");
	}

	return ret_val;
}

static void
exit_hdisk_device_module (void)
{
	int i;
	printk ("Unloading hard disk Device Module...\n");

	for (i = 0; i < MAX_LUN_SUPPORT; i++)
	{
		if (LunNumAssignment[i] == 0xFF) continue;
		if (iUSB.iUSBReleaseHdiskInstanceIndex(i))
		{
			TWARN ("exit_hdisk_device_module(): Error Releasing Hard Disk Instance\n");
		}
	}
	unregister_usb_device (&hdisk_device_driver);
	unregister_reboot_notifier (&hdisk_notifier);
	if (my_sys) RemoveSysctlTable(my_sys);
	return;
}

static int
hdisk_notify_sys(struct notifier_block *this, unsigned long code, void *unused)
{
	TDBG_FLAGGED(hdisk, DEBUG_HDISK,"hard disk Device Reboot notifier...\n");
	if (code == SYS_DOWN || code == SYS_HALT)
	{
	   unregister_usb_device (&hdisk_device_driver);
	}
	return NOTIFY_DONE;
}


module_init(init_hdisk_device_module);
module_exit(exit_hdisk_device_module);

MODULE_AUTHOR("Rama Rao Bisa <RamaB@ami.com>");
MODULE_DESCRIPTION("USB Hard disk Device module");
MODULE_LICENSE("GPL");
