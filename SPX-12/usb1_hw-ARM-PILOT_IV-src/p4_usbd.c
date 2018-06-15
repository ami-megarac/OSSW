#define P4_USBD_DMA_SUPPORT								1
/* Kernel Header */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>

#include "miscctrl.h"
/* Core Header */
#include "mod_reg.h"
#include "bot.h"
#include "usb_hw.h"

/* HW Header */
#include "p4_usbd.h"
#include "p4_usb.h"

/* Define */
#define P4_USBD_DEBUG_HWINIT 			  				0
#define P4_USBD_DEBUG_IVECT				  				0
#define P4_USBD_DEBUG_SUDAV				  				0	// ivect == 0x0

#define P4_USBD_VENDOR_ID						   0x046B	/* American Megatrends Inc. */
#define P4_USBD_PRODUCT_ID_HUB					   0xFF01
#define P4_USBD_HUB_CONFIG_VALUE						1
#define STRING_LANGUAGE_ID						   0x0409	/* English(US) */

#define STRING_MANUFACTURER								1
#define STRING_PRODUCT									2
#define STRING_SERIAL									3
#define STRING_CONFIG									4
#define STRING_INTERFACE								5

/* Structure */
struct p4_usbd_string {
    uint8_t id;
    char *s;
};

// Port change workqueue
struct workqueue_struct *p4_usbd_pc_wq;
// Data Validate workqueue
struct workqueue_struct *p4_usbd_dv_wq;
struct p4_usbd_workqueue_t {
	struct work_struct p4_usbd_workqueue;
	uint8_t devNum;
	uint16_t retry;
};

#if P4_USBD_DMA_SUPPORT
struct p4_usbd_dma_t {
    uint32_t* in_virt_addr;
    uint32_t* out_virt_addr;
    dma_addr_t in_phy_addr;
    dma_addr_t out_phy_addr;
	uint32_t out_data_len;
	uint8	isDma;
};
static struct p4_usbd_dma_t p4_usbd_dma[1 + P4_USBD_MAX_IN_EP + P4_USBD_MAX_OUT_EP];		// +1 for endpoint0 but useless
#endif

static int  p4_USB_Type_Data[P4_USBD_USE_DEV_NUM] = {0xff, 0xff ,0xff ,0xff};

/* Variable */
static USB_CORE usb_core_module;
static const char *p4_usbd_driver_name = "p4_usb_drv";
static usb_ctrl_driver p4_usbd_ctr_driver[P4_USBD_MAX_DEV_NUM];
static uint8_t P4_USBD_IRQ[] = {P4_USBD_IRQ_HUB,
							  P4_USBD_IRQ_DEV0,
							  P4_USBD_IRQ_DEV1,
							  P4_USBD_IRQ_DEV2,
							  P4_USBD_IRQ_DEV3,
							  P4_USBD_IRQ_DEV4,
							  P4_USBD_IRQ_DEV5,
							  P4_USBD_IRQ_DEV6,
};

static uint8_t P4_USBD_OUTxBCL[8 + 1] = {0, 0x8, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40};
//static uint8_t P4_USBD_OUTxBCH[8 + 1] = {0, 0x9, 0x11, 0x19, 0x21, 0x29, 0x31, 0x39, 0x41};
static uint8_t P4_USBD_OUTxCS[8 + 1] = {0, 0xB, 0x13, 0x1B, 0x23, 0x2B, 0x33, 0x3B, 0x43};
static uint8_t P4_USBD_OUTxCON[8 + 1] = {0, 0xA, 0x12, 0x1A, 0x22, 0x2A, 0x32, 0x3A, 0x42};
static uint8_t P4_USBD_INxBCL[12 + 1] = {0, 0xC, 0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C, 0x44, 0x4C, 0x54, 0x5C, 0x64};
//static uint8_t P4_USBD_INxBCH[12 + 1] = {0, 0xD, 0x15, 0x1D, 0x25, 0x2D, 0x35, 0x3D, 0x45, 0x4D, 0x55, 0x5D, 0x65};
static uint8_t P4_USBD_INxCS[12 + 1] = {0, 0xF, 0x17, 0x1F, 0x27, 0x2F,0x37, 0x3F, 0x47, 0x4F, 0x57, 0x5F, 0x67};
static uint8_t P4_USBD_INxCON[12 + 1] = {0, 0xE, 0x16, 0x1E, 0x26, 0x2E, 0x36, 0x3E, 0x46, 0x4E, 0x56, 0x5E, 0x66};
static uint8_t P4_USBD_FIFOxDAT[12 + 1] = {0, 0x84, 0x88, 0x8C, 0x90, 0x94, 0x98, 0x9C, 0xA0, 0xA4, 0xA8, 0xAC, 0xB0};

/* Hub Configuration */
static void * p4_usbd_hub_cfg_reg_va; 							/* virtual address of I/O registers */
static uint8_t * p4_usbd_hub_cfg_ep0_buf; 						/* buffer for hub ep0 setup IN/OUT */
static uint8_t p4_usbd_hub_suspended = 0;
static uint8_t p4_usbd_hub_cfg_remote_wakeup;					/* 0 for disable remote wakeup, 1 for enable remote wakeup, 2 for suspend state. */
static uint8_t p4_usbd_hub_cfg_addr;							/* address of hub */
static uint8_t p4_usbd_hub_cfg_status;
static uint8_t p4_usbd_hub_cfg_current_config_value;
static struct usb_hub_status p4_usbd_hub_cfg_class_status;
static uint32_t p4_usbd_hub_cfg_dev_num[P4_USBD_MAX_DEV_NUM];
static struct usb_port_status p4_usbd_hub_cfg_port_status[P4_USBD_MAX_DEV_NUM];
static uint8_t p4_usbd_device_hwEnabled[P4_USBD_MAX_DEV_NUM]={0};
static uint8_t p4_usbd_device_connect_to_port[P4_USBD_MAX_DEV_NUM]={0};
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
static uint8_t p4_usbd_hub_connect_to_upstream = 0;
#endif

/* =========================== development variable =========================== */
#if (P4_USBD_DEBUG_SUDAV)
static unsigned long int DBGCount=0;
#endif

/* =========================== development variable =========================== */

/* Function prototype */
static void p4_usbd_hw_enable_dev_fun (uint8_t *);
static void p4_usbd_hw_disable_dev_fun (uint8_t);
static void p4_usbd_hw_disable_dev (uint8_t, uint8_t);
static void p4_usbd_hw_enable_dev (uint8_t, uint8_t);
static int p4_usbd_hw_read_from_host(uint8_t devNum, uint8_t epNum, uint8_t *buf, uint16_t *len, uint8_t is_setup);
static uint8_t p4_usbd_state_init(uint8_t isHWModuleLoading);
static uint8_t p4_usbd_enable_EP1(void);

/* Structure */
static struct usb_device_descriptor p4_usbd_hub_dev_desc = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = __constant_cpu_to_le16(0x0200), 					/* USB Specification 2.0 */
	.bDeviceClass = USB_CLASS_HUB,
	.bDeviceSubClass = 0,										/* http://www.usb.org/developers/defined_class/#BaseClass09h */
	.bDeviceProtocol = 1,
	.bMaxPacketSize0 = 64, 										/* ep0 max packet size */
	.idVendor = __constant_cpu_to_le16(P4_USBD_VENDOR_ID),
	.idProduct = __constant_cpu_to_le16(P4_USBD_PRODUCT_ID_HUB), 
	.bcdDevice = __constant_cpu_to_le16(0x0100),
	.iManufacturer = STRING_MANUFACTURER,
	.iProduct = STRING_PRODUCT,
	.iSerialNumber = STRING_SERIAL,
	.bNumConfigurations = 1 									/* we have only 1 configuration */
};

static struct usb_qualifier_descriptor p4_usbd_hub_dev_qualifier_desc = {
	.bLength = sizeof(p4_usbd_hub_dev_qualifier_desc),
	.bDescriptorType = USB_DT_DEVICE_QUALIFIER,
	.bcdUSB = __constant_cpu_to_le16(0x0200), 					/* USB Specification 2.0 */
	.bDeviceClass = USB_CLASS_HUB,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64, 										/* ep0 max packet size */
	.bNumConfigurations = 1,
	.bRESERVED = 0
};

/* There is only one configuration */
static struct usb_config_descriptor p4_usbd_hub_config_desc = {
	.bLength = USB_DT_CONFIG_SIZE,
	.bDescriptorType = USB_DT_CONFIG,
	/* wTotalLength will computed finally by p4_usbd_get_config_descs */
	.bNumInterfaces = 1, /* we have only 1 interface */
	.bConfigurationValue = P4_USBD_HUB_CONFIG_VALUE,
	.iConfiguration = STRING_CONFIG,
	.bmAttributes = USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,// | USB_CONFIG_ATT_WAKEUP ,
	.bMaxPower = 50 /* 100 mA */
};

/* There is only one interface. */
static struct usb_interface_descriptor p4_usbd_hub_intf_desc = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1, /* status change ep(Interrupt In) */
	.bInterfaceClass = USB_CLASS_HUB,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = STRING_INTERFACE,
};

/* High-speed endpoint descriptors for status change endpoint of hub */
static struct usb_endpoint_descriptor p4_usbd_hub_hs_intr_in_ep_desc = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_IN | 0x01,
	.bmAttributes = USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize = 0x01, /* 1 byte */
	.bInterval = 0x0C /* 12 frames */
};

static const struct usb_descriptor_header *p4_usbd_hub_descriptors[] = {
	(struct usb_descriptor_header *) &p4_usbd_hub_config_desc,
	(struct usb_descriptor_header *) &p4_usbd_hub_intf_desc,
	(struct usb_descriptor_header *) &p4_usbd_hub_hs_intr_in_ep_desc,
	NULL,
};

/* Static strings, in UTF-8 (for simplicity we use only ASCII characters) */
static struct p4_usbd_string p4_usbd_hub_strings_desc[] = {
	{STRING_MANUFACTURER, "American Megatrends Inc."},
	{STRING_PRODUCT, "Virtual Hub"},					/* depend on device type, adjusted during setup */
	{STRING_SERIAL, "serial"},							/* depend on device id, adjusted during setup */
	{STRING_CONFIG, "Self-powered"},
	{STRING_INTERFACE, "7-port Hub"},
	{0, NULL}
};

struct usb_hub_descriptor p4_usbd_hub_class_desc = {
	.bDescLength = USB_DT_HUB_SIZE,
	.bDescriptorType = USB_DT_HUB,
	.bNbrPorts = P4_USBD_MAX_DEV_NUM,
	.wHubCharacteristics = HUB_CHAR_LPSM_IDV | HUB_CHAR_OCPM_IDV | HUB_CHAR_TTTT_32,
	.bPwrOn2PwrGood = 0x32, /* 100 ms */
	.bHubContrCurrent = 100, /* 100 mA */
	.DeviceRemovable = 0x00,
	.PortPwrCtrlMask = 0xFF /* all bit should be 1 for compatibility with USB 1.0 */
};

inline uint8_t p4_usbd_read8_reg(uint16_t reg) {
		return ioread8((void * __iomem)p4_usbd_hub_cfg_reg_va + reg);
}

inline uint16_t p4_usbd_read16_reg(uint16_t reg) {
		return ioread16((void * __iomem)p4_usbd_hub_cfg_reg_va + reg);
}

inline uint32_t p4_usbd_read32_reg(uint32_t reg) {
		return ioread32((void * __iomem)p4_usbd_hub_cfg_reg_va + reg);
}

inline void p4_usbd_write8_reg(uint8_t data, uint16_t reg) {
		iowrite8(data, (void * __iomem)p4_usbd_hub_cfg_reg_va + reg);
}

inline void p4_usbd_write16_reg(uint16_t data, uint16_t reg) {
		iowrite16(data, (void * __iomem)p4_usbd_hub_cfg_reg_va + reg);
}

inline void p4_usbd_write32_reg(uint32_t data, uint32_t reg) {
		iowrite32(data, (void * __iomem)p4_usbd_hub_cfg_reg_va + reg);
}

// Followed by SDK 20160531.PILOT4SDK.tbz, line 111 of pilot4_hub_multi_dev.c
void p4_usbd_reset_data_toggle(uint8_t dir, uint8_t endptno)
{
	if(endptno == 0)
		return ;
	p4_usbd_write8_reg( ((dir << 4) | endptno), P4_USBD_ENDPRST);
	p4_usbd_write8_reg( ((P4_USBD_ENDPRST_TOGRST) | (dir << 4) | endptno ), P4_USBD_ENDPRST);
}

// Followed by SDK 20160531.PILOT4SDK.tbz, line 120 of pilot4_hub_multi_dev.c
void p4_usbd_do_fifo_reset(uint32_t dir, uint32_t  endptno)
{
	if(endptno == 0)
		return ;
	p4_usbd_write8_reg( ((dir << 4) | endptno), P4_USBD_ENDPRST);
	p4_usbd_write8_reg( ((P4_USBD_ENDPRST_FIFORST) | (dir << 4) | endptno ), P4_USBD_ENDPRST);
}

/* Stall (Endpoint 0 stall bit), See ep0cs(page 26) of Pilot4_USB2_0_HUB_V0.6,
 * If the ep0stall bit is set to '1', the HUB sends a STALL handshake for any IN or OUT token to the
 * endpoint 0 during the data or status stages of control transfer.
 * Ep0stall is automatically cleared when a SETUP token arrives. The microcontroller sets this bit by
 * writing a '1' to it or clears this bit by writing a '0' to it. */
inline void p4_usbd_set_ep0_stall(void)
{
	p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_EP0CS) | (P4_USBD_EP0CS_STALL) , P4_USBD_EP0CS);
}

inline void p4_usbd_clear_ep0_stall(void)
{
	p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_EP0CS) & ~(P4_USBD_EP0CS_STALL) , P4_USBD_EP0CS);
}

/* hsnak, See ep0cs(page 26) of Pilot4_USB2_0_HUB_V0.6,
 * If the hsnak bit is set to '1' (and the stall bit ? ep0cs[0] must be set to '0'), the HUB responds with a
 * NAK handshake for every packet in the status stage.
 * The hsnak bit is automatically set to '1' when a SETUP token arrives.
 * The microcontroller clears the hsnak bit by writing a '1' to it. When the hsnak is cleared (and the
 * stall bit ? ep0cs[0] is '0'), the HUB acknowledges the status stage of the control transfer. */

/* IVAN: It's not true. 
 * Writing to the in0bc register will set hsnak to '1' automatically in a control read/write transfer,
 * and i guess hsnak will automatically to to '1' after host receive data in a control read transfer.
 * write 1 to inbsy bit will clear hsnak bit in control read transfer only.
 * hsnak bit is automatically cleared when a SETUP token arrives.
 * */

inline void p4_usbd_clear_ep0_hsnak(void)
{
	p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_EP0CS) | (P4_USBD_EP0CS_HSNAK) , P4_USBD_EP0CS);
}

/* inbsy (IN 0 endpoint busy bit), See ep0cs(page 26) of Pilot4_USB2_0_HUB_V0.6,
 * If inbsy='1' the HUB hardware takes control of the IN 0 endpoint buffer. The IN 0 buffer is armed to
 * send data to host. The microcontroller should not access the IN 0 endpoint when inbsy='1'.
 * If inbsy='0' the microcontroller takes control of the IN 0 endpoint buffer.
 * Inbsy is a read-only bit that is automatically cleared when a SETUP token arrives. The
 * microcontroller sets this bit by reloading the in0bc register. */

/* IVAN: It's not true, If inbsy='1' the usb device driver takes control of the IN 0 endpoint buffer.
 * If inbsy='0' the pilot4 usb hardware takes control of the IN 0 endpoint buffer. 
 * inbsy is a read-only bit that is automatically set when a SETUP token arrives in a control read transfer, 
 * usb device drive clear this bit by write 1 to inbsy bit. */

/* outbsy (OUT 0 endpoint busy bit), See ep0cs(page 26) of Pilot4_USB2_0_HUB_V0.6,
 * If outbsy='1' the HUB hardware takes control of the OUT 0 endpoint buffer. The endpoint 0 buffer
 * is ready to receive data from host. The microcontroller should not access the OUT 0 endpoint
 * buffer when outbsy='1'.
 * If outbsy='0' the microcontroller takes control of the OUT 0 endpoint buffer.
 * Outbsy is a read-only bit that is automatically set when a SETUP token arrives. The outbsy bit is
 * automatically cleared when the HUB receives an error-free OUT data packet. The microcontroller
 * sets this bit by writing a dummy value to the out0bc register. */

inline void p4_usbd_port_enable(uint8_t devNo)
{
	p4_usbd_write32_reg( p4_usbd_read32_reg(P4_USBD_PORTENABLE) | (1 << devNo), P4_USBD_PORTENABLE);
}

inline void p4_usbd_port_disable(uint8_t devNo)
{
	p4_usbd_write32_reg( p4_usbd_read32_reg(P4_USBD_PORTENABLE) & ~(1 << devNo), P4_USBD_PORTENABLE);
}

static uint8_t p4_usbd_ascii_to_unicode(const char *str, uint8_t *buf, uint32_t len)
{
	uint8_t i = 0;
	while (len != 0 && *str != '\0') {
		buf[i] = *str;
		buf[i + 1] = 0x00;
		i += 2;
		str ++;
		len --;
	}
	return i;
}

static uint8_t p4_usbd_get_string_desc(struct p4_usbd_string *table, uint8_t id, uint8_t *buf)
{
	uint8_t i;
	uint8_t len;
	const char *s;

	/* Followed USB2.0 SPEC Table UNICODE String Descriptor */
	/* descriptor 0 has the language id */
	if (id == 0) {
		buf[0] = 4;
		buf[1] = USB_DT_STRING;
		buf[2] = (uint8_t) STRING_LANGUAGE_ID;
		buf[3] = (uint8_t) STRING_LANGUAGE_ID >> 8;
		return 4;
	}

	for (i = 0; table[i].s != NULL; i ++)
		if (table[i].id == id)
			break;

	/* unrecognized: STALL */
	if (table[i].s == NULL)
		return -EINVAL;
	s = table[i].s;

	/* string descriptors have length, tag, then UTF16-LE text */
	len = min((size_t) 126, strlen(s)); /* maximum length of string descriptor is 255, minus the first 2, remains 253 */
	memset(buf + 2, 0x00, 2 * len);
	len = p4_usbd_ascii_to_unicode(s, buf + 2, len);

	buf [0] = 2 + len;
	buf [1] = USB_DT_STRING;

	return buf[0];
}

static uint32_t p4_usbd_fill_config_buf(uint8_t *buf, uint32_t buf_len, const struct usb_descriptor_header **config_descs)
{
	uint32_t desc_len;
	uint32_t total_len;

	if (!config_descs)
		return -EINVAL;

	/* fill buffer from config_descs[] until null descriptor pointer */
	total_len = 0;
	while (*config_descs != NULL) {
		desc_len = (*config_descs)->bLength;
		if (buf_len < desc_len) /* no enough space */
			return -EINVAL;
		memcpy(buf, *config_descs, desc_len);
		buf_len -= desc_len;
		buf += desc_len;
		total_len += desc_len;
		config_descs ++;  /* next descriptor */
	}

	return total_len;
}

static uint32_t p4_usbd_get_config_descs(uint8_t *buf, uint8_t type, uint8_t index)
{
	const struct usb_descriptor_header **descriptors;
	uint32_t len;

	if (index > 0) /* we only support one configuration */
		return -EINVAL;

	descriptors = p4_usbd_hub_descriptors;
	len = p4_usbd_fill_config_buf(buf, P4_USBD_EP0_BUF_SIZE, descriptors);
	((struct usb_config_descriptor *)buf)->bDescriptorType = type;
	((struct usb_config_descriptor *)buf)->wTotalLength = cpu_to_le16(len);

	return len;
}

static void p4_usbd_pc_queue_worker_retry(uint8_t devNum, void (*fnc)(struct work_struct *work), uint16_t retry)
{
	struct p4_usbd_workqueue_t *p4_usbd_qworker_data = (struct p4_usbd_workqueue_t*)kmalloc(sizeof(struct p4_usbd_workqueue_t), GFP_ATOMIC);
	if(p4_usbd_qworker_data == NULL)
	{
		panic("p4_usbd");
	}	
	INIT_WORK((struct work_struct*) p4_usbd_qworker_data, fnc);
	p4_usbd_qworker_data->devNum=devNum;
	p4_usbd_qworker_data->retry=retry;
	queue_work(p4_usbd_pc_wq, (struct work_struct *)p4_usbd_qworker_data);
}

static void p4_usbd_pc_queue_worker(uint8_t devNum, void (*fnc)(struct work_struct *work))
{
	struct p4_usbd_workqueue_t *p4_usbd_qworker_data = (struct p4_usbd_workqueue_t*)kmalloc(sizeof(struct p4_usbd_workqueue_t), GFP_ATOMIC);
	if( p4_usbd_qworker_data == NULL)
	{
		panic("p4_usbd");
	}	
	INIT_WORK((struct work_struct*) p4_usbd_qworker_data, fnc);
	p4_usbd_qworker_data->devNum=devNum;
	p4_usbd_qworker_data->retry=0;
	queue_work(p4_usbd_pc_wq, (struct work_struct *)p4_usbd_qworker_data);
}

static void p4_usbd_resume_port(uint8_t port)
{
	if (p4_usbd_device_connect_to_port[port]) {
		usb_core_module.CoreUsbBusResume(p4_usbd_hub_cfg_dev_num[port]);
	}
}

static void p4_usbd_suspend_port(uint8_t port)
{
	if (p4_usbd_device_connect_to_port[port]) {
		usb_core_module.CoreUsbBusSuspend(p4_usbd_hub_cfg_dev_num[port]);
	}
}

static void p4_usbd_port_reset(uint8_t port)
{
	p4_usbd_hw_disable_dev (p4_usbd_hub_cfg_dev_num[port], 0);
	usb_core_module.CoreUsbBusReset(p4_usbd_hub_cfg_dev_num[port]);
	// See Pilot4_USB2_0_HUB_V0.6.pdf, Section 5 HUB-Device(s) model, second scenario
	p4_usbd_write32_reg(1, P4_USBD_DEVXPORTRSTDB(port));
	p4_usbd_hw_enable_dev (p4_usbd_hub_cfg_dev_num[port], 0);
}

static void p4_usbd_hub_send_port_change(struct work_struct *qworker_data)
{
	uint8_t buf=0;
	struct p4_usbd_workqueue_t *p4_usbd_pc_wq_data=(struct p4_usbd_workqueue_t*)qworker_data;
	uint8_t devNo=p4_usbd_pc_wq_data->devNum;
	uint16_t retry=p4_usbd_pc_wq_data->retry;

	while( (p4_usbd_read8_reg(P4_USBD_INxCS[1]) & P4_USBD_BIT1_SET) ) {
		mdelay(5);
		if(retry > 250) {
			printk(KERN_ERR "P4_USBD(%d): Send Port Change Failure. dev:%d\n", __LINE__, devNo);
			kfree( (void *)qworker_data );
			return ;
		}
		retry++;
		p4_usbd_pc_queue_worker_retry(devNo, &p4_usbd_hub_send_port_change, retry);
		kfree( (void *)qworker_data );
		return;
	}

	buf = (0x02 << devNo);
	p4_usbd_write8_reg(buf, P4_USBD_FIFOxDAT[1]);
	p4_usbd_write16_reg(1, P4_USBD_INxBCL[1]);
	/* the microcontroller reloads the inxcs register with any value to set the busy bit. */
    p4_usbd_write8_reg(0, P4_USBD_INxCS[1]);
	kfree( (void *)qworker_data );

	return;
}
	
static void remote_wakeup_check(void)
{
	/* Followed SDK 20160630.PILOT4SDK.tbz, see line 432 of pilot4_hub_multi_dev.c. */
	if( !p4_usbd_hub_cfg_remote_wakeup && p4_usbd_hub_suspended ) {
		p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_USBCS) | (P4_USBD_BIT3_SET) , P4_USBD_USBCS);
		udelay(50);
        /* Some times Control setup SetFeature Remotewakeup from Host is not receieved
         * after host generally suspends the Hub, But since s/w has not received setup
         * data_valid interrupt for setFeature it doesn't know weather we required to
         * initiate remotewakeup after suspend, So here we assume we need to do remote
         * wakeup and initiate resume signalling
         */
		p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_USBCS) | (P4_USBD_BIT5_SET) , P4_USBD_USBCS);
		mdelay(20);
		p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_USBCS) & ~(P4_USBD_BIT3_SET) , P4_USBD_USBCS);
		p4_usbd_hub_suspended = 0;
	}

	if ( p4_usbd_hub_cfg_remote_wakeup ) {
		p4_usbd_hub_suspended = 0;
		p4_usbd_hub_cfg_remote_wakeup = 0;
		p4_usbd_hub_cfg_status &= ~(1 << USB_DEVICE_REMOTE_WAKEUP);
		p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_USBCS) & ~(P4_USBD_BIT3_SET) , P4_USBD_USBCS);
		// wakeup
		p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_USBCS) | (P4_USBD_BIT5_SET) , P4_USBD_USBCS);
		mdelay(20);
	}
}

/* handle hub endpoint 0 control transfer class-specific setup request */
static uint8_t p4_usbd_class_req(const struct usb_ctrlrequest *ctrlReq, uint8_t *pskip_data_stage)
{
	uint16_t wValue = le16_to_cpu(ctrlReq->wValue);
	uint16_t wIndex = le16_to_cpu(ctrlReq->wIndex);
	uint16_t wLength = le16_to_cpu(ctrlReq->wLength);
	uint8_t descType, descIndex, port;
	uint16_t len = -EOPNOTSUPP;

	switch (ctrlReq->bRequest) {
		case USB_REQ_GET_STATUS:
			switch (ctrlReq->bRequestType) {
				/* Followed USB2.0 SPEC Section 11.24.2 and Section 11.24.2.6 */
				case ( USB_DIR_IN | USB_RT_HUB ):  //Recipient is Hub
					if ( (wValue != 0) || (wIndex != 0) || (wLength != USB_HUB_STATUS_SIZE))
						break;
					memcpy(p4_usbd_hub_cfg_ep0_buf, (const void *) &p4_usbd_hub_cfg_class_status, USB_HUB_STATUS_SIZE);
					len = USB_HUB_STATUS_SIZE;
					break;
				/* Followed USB2.0 SPEC Section 11.24.2 and Section 11.24.2.7 */
				case ( USB_DIR_IN | USB_RT_PORT ): // Recipient is Port
					if ( (wValue != 0) || (wLength != USB_PORT_STATUS_SIZE) )
						break;
					/* The port number must be a valid port number for that hub, greater than zero. */
					/* USB Hub port number is begin from 1, but the status array(port_status) start from 0, so mins 1. */
					port = (wIndex & 0xFF) - 1; 
					if (port >= P4_USBD_MAX_DEV_NUM)
						break;
		
					memcpy(p4_usbd_hub_cfg_ep0_buf, (const void *) &p4_usbd_hub_cfg_port_status[port], USB_PORT_STATUS_SIZE);
					len = USB_PORT_STATUS_SIZE;
					break;
				default:
					printk(KERN_WARNING "P4_USBD(%d): Unknown USB Recipient: bRequestType=%02x\n", __LINE__, ctrlReq->bRequestType);
			}
			break;
		case USB_REQ_CLEAR_FEATURE:
			*pskip_data_stage = 1;
			switch (ctrlReq->bRequestType) {
				case USB_RT_HUB:
					printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
					break;
				case USB_RT_PORT:
					port = (wIndex & 0xFF) - 1; 
					if ( port > P4_USBD_MAX_DEV_NUM )
						break;
					switch (wValue) {
						case USB_PORT_FEAT_CONNECTION:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_ENABLE:
							p4_usbd_hub_cfg_port_status[port].wPortStatus &= ~(USB_PORT_STAT_ENABLE | USB_PORT_STAT_HIGH_SPEED);
							p4_usbd_hw_disable_dev (p4_usbd_hub_cfg_dev_num[port], 0);
							break;
						case USB_PORT_FEAT_SUSPEND:
							p4_usbd_hub_cfg_port_status[port].wPortStatus &= ~USB_PORT_STAT_SUSPEND;
							p4_usbd_hub_cfg_port_status[port].wPortChange |= USB_PORT_STAT_C_SUSPEND;
							p4_usbd_resume_port(port);
							p4_usbd_pc_queue_worker(port, &p4_usbd_hub_send_port_change);
							break;
						case USB_PORT_FEAT_OVER_CURRENT:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_RESET:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_POWER:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_LOWSPEED:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_C_CONNECTION:
							p4_usbd_hub_cfg_port_status[port].wPortChange &= ~USB_PORT_STAT_C_CONNECTION;
							break;
						case USB_PORT_FEAT_C_ENABLE:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_C_SUSPEND:
							p4_usbd_hub_cfg_port_status[port].wPortChange &= ~USB_PORT_STAT_C_SUSPEND;
							break;
						case USB_PORT_FEAT_C_OVER_CURRENT:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_C_RESET:
							p4_usbd_hub_cfg_port_status[port].wPortChange &= ~USB_PORT_STAT_C_RESET;
							break;
						case USB_PORT_FEAT_TEST:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_INDICATOR:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						default:
							printk(KERN_WARNING "P4_USBD(%d): Unknown USB Hub Class Request:  bRequestType=%02x, bRequest=%02x,\
								wValue=%04x, wIndex=%04x, wLength=%x\n", __LINE__, ctrlReq->bRequestType, ctrlReq->bRequest, wValue, wIndex, wLength);
					}
					len = 0;
					break;
				default:
					printk(KERN_WARNING "P4_USBD(%d): Unknown USB Hub Class Request:  bRequestType=%02x, bRequest=%02x,\
							wValue=%04x, wIndex=%04x, wLength=%x\n", __LINE__, ctrlReq->bRequestType, ctrlReq->bRequest, wValue, wIndex, wLength);
			}
			break;
		case USB_REQ_SET_FEATURE:
			*pskip_data_stage = 1;
			switch (ctrlReq->bRequestType) {
				/* Followed USB2.0 SPEC Section 11.24.2.12 Set Hub Feature
				 * It is a Request Error if wValue is not a feature selector listed in Table 11-17 or if wIndex or wLength are not 0 */
				case USB_RT_HUB:
					printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
					break;

				/* Followed USB2.0 SPEC Section 11.24.2.13 Set Port Feature
				 * The port number must be a valid port number for that hub, greater than zero. The port number is in the least
				 * significant byte (bits 7..0) of the wIndex field. The most significant byte of wIndex is zero, except when the
				 * feature selector is PORT_TEST. */
				case USB_RT_PORT:
					/* USB Hub port number is begin from 1 */
					port = (wIndex & 0xFF) - 1; 
					if (port >= P4_USBD_MAX_DEV_NUM)
						break;
		
					switch (wValue) {
						case USB_PORT_FEAT_CONNECTION:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_ENABLE:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_SUSPEND:
							p4_usbd_hub_cfg_port_status[port].wPortStatus |= USB_PORT_STAT_SUSPEND;
							p4_usbd_suspend_port(port);
							break;
						case USB_PORT_FEAT_OVER_CURRENT:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_RESET:
							p4_usbd_hub_cfg_port_status[port].wPortStatus &= ~USB_PORT_STAT_SUSPEND;
							p4_usbd_hub_cfg_port_status[port].wPortStatus |= USB_PORT_STAT_POWER;
							p4_usbd_hub_cfg_port_status[port].wPortChange |= USB_PORT_STAT_C_RESET;
							/* There is a device attached to this port */
							if(p4_usbd_device_connect_to_port[port]) {
								p4_usbd_hub_cfg_port_status[port].wPortStatus |= USB_PORT_STAT_ENABLE;
								p4_usbd_hub_cfg_port_status[port].wPortStatus |= USB_PORT_STAT_CONNECTION;
								/* Pilot 4 running in high speed only. */
								p4_usbd_hub_cfg_port_status[port].wPortStatus |= USB_PORT_STAT_HIGH_SPEED;
                    			p4_usbd_port_reset(port);
							}
							p4_usbd_pc_queue_worker(port, &p4_usbd_hub_send_port_change);
							break;
						case USB_PORT_FEAT_POWER:
							p4_usbd_hub_cfg_port_status[port].wPortStatus |= USB_PORT_STAT_POWER;
							/* There is a device attached to this port */
							if(p4_usbd_device_connect_to_port[port]) {
								/* change port status */
								p4_usbd_hub_cfg_port_status[port].wPortStatus &= ~USB_PORT_STAT_SUSPEND;
								p4_usbd_hub_cfg_port_status[port].wPortStatus |= USB_PORT_STAT_CONNECTION;
								/* Pilot 4 running in high speed only. */
								p4_usbd_hub_cfg_port_status[port].wPortStatus |= USB_PORT_STAT_HIGH_SPEED;
								p4_usbd_hub_cfg_port_status[port].wPortChange |= USB_PORT_STAT_C_CONNECTION;
								p4_usbd_pc_queue_worker(port, &p4_usbd_hub_send_port_change);
							}
							break;
						case USB_PORT_FEAT_LOWSPEED:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_C_CONNECTION:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_C_ENABLE:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_C_SUSPEND:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_C_OVER_CURRENT:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_C_RESET:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_TEST:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						case USB_PORT_FEAT_INDICATOR:
							printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
							break;
						default:
							printk(KERN_WARNING "P4_USBD(%d): Unknown USB Hub Class Request:  bRequestType=%02x, bRequest=%02x,\
								wValue=%04x, wIndex=%04x, wLength=%x\n", __LINE__, ctrlReq->bRequestType, ctrlReq->bRequest, wValue, wIndex, wLength);
						}
					len = 0;
					break;
				default:
					printk(KERN_WARNING "P4_USBD(%d): Unknown USB Hub Class Request:  bRequestType=%02x, bRequest=%02x,\
							wValue=%04x, wIndex=%04x, wLength=%x\n", __LINE__, ctrlReq->bRequestType, ctrlReq->bRequest, wValue, wIndex, wLength);
			}
			break;

		case USB_REQ_GET_DESCRIPTOR:
			if (ctrlReq->bRequestType != (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_DEVICE)) /* 1 01 00000 */
				break;

			/* Followed USB2.0 SPEC Section 9.4.3
			 * The wValue field specifies the descriptor type in the high byte (refer to Table 9-5) and the descriptor index
			 * in the low byte. */
			descType = wValue >> 8;
			descIndex = wValue & 0xFF;
			if (descType != USB_DT_HUB) /* 29H for hub descriptor */
				break;
			if (descIndex != 0) 	/* we only have 1 hub descriptor */
				break;

			/* Followed USB2.0 SPEC Section 11.24.2.5
			 * If wLength is larger than the actual length of the descriptor, then only the actual length is returned. If
			 * wLength is less than the actual length of the descriptor, then only the first wLength bytes of the descriptor
			 * are returned; */
			len = (wLength < USB_DT_HUB_SIZE) ? wLength : USB_DT_HUB_SIZE;
			memcpy(p4_usbd_hub_cfg_ep0_buf, &p4_usbd_hub_class_desc, len);
			break;

		default:
			*pskip_data_stage = 1;
			printk(KERN_WARNING "P4_USBD(%d): Unknown USB Hub Class Request:  bRequestType=%02x, bRequest=%02x, wValue=%04x, wIndex=%04x, wLength=%x\n",
					__LINE__, ctrlReq->bRequestType, ctrlReq->bRequest, wValue, wIndex, wLength);
	}

	return len;
}

/* handle hub endpoint 0 control transfer standard setup request */
static uint8_t p4_usbd_standard_req(const struct usb_ctrlrequest *ctrlReq, uint8_t *pskip_data_stage)
{
	uint16_t wValue = le16_to_cpu(ctrlReq->wValue);
	uint16_t wIndex = le16_to_cpu(ctrlReq->wIndex);
	uint16_t wLength = le16_to_cpu(ctrlReq->wLength);
	uint8_t descType, descIndex, recipient;
	uint16_t len = -EOPNOTSUPP;

	/* Followed USB2.0 SPEC Table 9-3 */
	switch (ctrlReq->bRequest) {
		case USB_REQ_GET_STATUS:
			/* Followed USB2.0 SPEC Section 9.4.5 
			 * If wValue or wLength are not as zero and two, or if wIndex is non-zero for a device status request, then
				the behavior of the device is not specified. */
			/* Status followed USB2.0 SPEC Figure 9-4. Information Returned by a GetStatus() Request to a Device. */
			if ( (wValue != 0) || (wLength != 2) )
				break;

			recipient = ctrlReq->bRequestType & USB_RECIP_MASK;
			if (recipient == USB_RECIP_DEVICE) {
				p4_usbd_hub_cfg_ep0_buf[0] = p4_usbd_hub_cfg_status;
			} else if (recipient == USB_RECIP_INTERFACE) {
				p4_usbd_hub_cfg_ep0_buf[0] = 0;
			} else if (recipient == USB_RECIP_ENDPOINT) {
				p4_usbd_hub_cfg_ep0_buf[0] = 0;
			} else {
				printk(KERN_WARNING "P4_USBD(%d): Unknown device recipient %02x\n", __LINE__, recipient);
				break;
			}
			p4_usbd_hub_cfg_ep0_buf[1] = 0;
			len = 2;
			break;
		case USB_REQ_CLEAR_FEATURE:
			/* Followed USB2.0 SPEC Section 9.4.5, Page 256, 
			 * a ClearFeature(ENDPOINT_HALT) request always results in the data toggle being reinitialized to DATA0. */
			*pskip_data_stage = 1;
			if (wValue == USB_ENDPOINT_HALT) {
				if (ctrlReq->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT)) /* 0 00 00010 */
					break;
				if ((wIndex & 0x7F) == 1) { /* ep1 */
					/* Do Clear Feature HALT */
					printk(KERN_WARNING "P4_USBD(%d): CLEAR HALT for EP1.\n", __LINE__);
					len = 0;
				} else {
					printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
				}
				if(wIndex & USB_DIR_IN) {
    				p4_usbd_reset_data_toggle(DIR_IN, (wIndex & 0x7F));
				}else {
    				p4_usbd_reset_data_toggle(DIR_OUT, (wIndex & 0x7F));
				}
	        } else if (wValue == USB_DEVICE_REMOTE_WAKEUP) {
				if (ctrlReq->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE)) /* 0 00 00010 */
				    break;
				
				/* disable remote wakeup */
				p4_usbd_hub_suspended = 0;
				p4_usbd_hub_cfg_remote_wakeup = 0;
				p4_usbd_hub_cfg_status &= ~(1 << USB_DEVICE_REMOTE_WAKEUP);
				p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_USBCS) & ~(P4_USBD_BIT3_SET) , P4_USBD_USBCS);
				len = 0;
			} else {
				printk(KERN_WARNING "P4_USBD(%d): Unknown feature in USB_REQ_CLEAR_FEATURE.\n", __LINE__);
			}
			break;
		case USB_REQ_SET_FEATURE:
			/* Followed USB2.0 SPEC Section 9.4.9 Set Feature
			   Feature selector values in wValue must be appropriate to the recipient. Only device feature selector values
			   may be used when the recipient is a device; only interface feature selector values may be used when the
			   recipient is an interface, and only endpoint feature selector values may be used when the recipient is an
			   endpoint. 
			   Feature selector followed USB2.0 SPEC Table 9-6. Standard Feature Selectors */
			*pskip_data_stage = 1;
			if (wValue == USB_ENDPOINT_HALT) { 
				if (ctrlReq->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT)) /* 0 00 00010 */
					break;
				len = 0;
				printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
			} else if (wValue == USB_DEVICE_REMOTE_WAKEUP) {
				if (ctrlReq->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE)) /* 0 00 00000 */
					break;

				/* enable remote wakeup */
				p4_usbd_hub_cfg_remote_wakeup = 1;
				p4_usbd_hub_cfg_status |= (1 << USB_DEVICE_REMOTE_WAKEUP);
				p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_USBCS) | (P4_USBD_BIT3_SET) , P4_USBD_USBCS);
				len = 0;
			} else {
				printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
			}
			break;
		case USB_REQ_SET_ADDRESS:
			*pskip_data_stage = 1;
			p4_usbd_hub_cfg_addr = wValue;
			len = 0;
			break;
		case USB_REQ_GET_DESCRIPTOR:
			if (ctrlReq->bRequestType != (USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE)) /* 1 00 00000 */
				break;
			/* Followed USB2.0 SPEC Section 9.4.3
			 * The wValue field specifies the descriptor type in the high byte (refer to Table 9-5) and the descriptor index
			 * in the low byte. */
			descType = wValue >> 8;
			descIndex = wValue & 0xFF;
			switch (descType) {
				case USB_DT_DEVICE:
					len = USB_DT_DEVICE_SIZE;
					memcpy(p4_usbd_hub_cfg_ep0_buf, &p4_usbd_hub_dev_desc, len);
					break;
				case USB_DT_CONFIG:
					/* Followed USB2.0 SPEC Section 9.4.3
					 * A request for a configuration descriptor
					 * returns the configuration descriptor, all interface descriptors, and endpoint descriptors for all of the
					 * interfaces in a single request. The first interface descriptor follows the configuration descriptor. The
					 * endpoint descriptors for the first interface follow the first interface descriptor. If there are additional
					 * interfaces, their interface descriptor and endpoint descriptors follow the first interface’s endpoint
					 * descriptors. Class-specific and/or vendor-specific descriptors follow the standard descriptors they extend or
					 * modify. */
					len = p4_usbd_get_config_descs(p4_usbd_hub_cfg_ep0_buf, descType, descIndex);
					break;
				case USB_DT_STRING:
					/* Followed USB2.0 SPEC 9.4.3
					 * The wIndex field specifies the Language ID for string descriptors or is reset to zero for other descriptors. */
					len = p4_usbd_get_string_desc(p4_usbd_hub_strings_desc, descIndex, p4_usbd_hub_cfg_ep0_buf);
					break;
				case USB_DT_DEVICE_QUALIFIER:
					len = sizeof(p4_usbd_hub_dev_qualifier_desc);
					memcpy(p4_usbd_hub_cfg_ep0_buf, &p4_usbd_hub_dev_qualifier_desc, len);
					break;
				default:
					printk(KERN_WARNING "P4_USBD(%d): Unknown device type %02x\n", __LINE__, descType);
			}
			if ( len > wLength)
				len = wLength;
			break;
		case USB_REQ_SET_CONFIGURATION:
			*pskip_data_stage = 1;
			if (ctrlReq->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE)) /* 0 00 00000 */
				break;

			/* Followed USB2.0 SPEC 9.4.3
			 * The lower byte of the wValue field specifies the desired configuration. This configuration value must be
			 * zero or match a configuration value from a configuration descriptor. If the configuration value is zero, the
			 * device is placed in its Address state. */
			if ( (wValue & 0xFF) == P4_USBD_HUB_CONFIG_VALUE ) {
				p4_usbd_hub_cfg_current_config_value = P4_USBD_HUB_CONFIG_VALUE;
				len = 0;
			} else {
				printk(KERN_WARNING "P4_USBD(%d): Unknown configuration value %02x\n", __LINE__, wValue);
			}
			break;
		default:
			*pskip_data_stage = 1;
			printk(KERN_WARNING "P4_USBD(%d): Unknown USB Standard Request: bRequestType=%02x, bRequest=%02x, wValue=%04x, wIndex=%04x, wLength=%x\n",
					__LINE__, ctrlReq->bRequestType, ctrlReq->bRequest, wValue, wIndex, wLength);
	}
	return len;
}

static uint8_t p4_usbd_ivect_dummy_handler(uint8_t dev, uint8_t ivect)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s, dev=%d, ivect=%x\n", __LINE__, __FUNCTION__, dev, ivect);
	return 0;
}

/* Contents of ivect register: 0x0 */
static uint8_t p4_usbd_ivect_sudav_handler_work(struct work_struct *qworker_data)
{
	uint8_t setup_pkt[8];
	uint8_t i = 0;
	uint16_t len = 0;
	struct usb_ctrlrequest *pUsbCtrlReq = NULL;
	uint8_t *buf;
	struct p4_usbd_workqueue_t *p4_usbd_dv_wq_data=(struct p4_usbd_workqueue_t*)qworker_data;
	uint8_t dev=p4_usbd_dv_wq_data->devNum;
	uint8_t skip_data_stage = 0;

	if ( dev == 0 ) { // Hub

		for ( i = 0 ; i < sizeof(setup_pkt) ; i++) {
			setup_pkt[i]= p4_usbd_read8_reg( P4_USBD_SETUPDAT + i );
		}
		pUsbCtrlReq = (struct usb_ctrlrequest *)setup_pkt;

#if (P4_USBD_DEBUG_SUDAV)
		printk("P4_USBD_DEBUG(%d): bRequestType=%02x, bRequest=%02x, wValue=%04x, wIndex=%04x, wLength=%04x, DBGCount=%ld\n", __LINE__,
			pUsbCtrlReq->bRequestType, pUsbCtrlReq->bRequest, \
			le16_to_cpu(pUsbCtrlReq->wValue),\
			le16_to_cpu(pUsbCtrlReq->wIndex),\
			le16_to_cpu(pUsbCtrlReq->wLength), DBGCount++);
#endif

		if ((pUsbCtrlReq->bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD)		/* stardard request */
		{

			len = p4_usbd_standard_req(pUsbCtrlReq, &skip_data_stage); 

		}
		else if ((pUsbCtrlReq->bRequestType & USB_TYPE_MASK) == USB_TYPE_CLASS)		/* class-specific request */
		{

			len = p4_usbd_class_req(pUsbCtrlReq, &skip_data_stage); 

		}
		else {
			skip_data_stage = 1;

			printk(KERN_WARNING "P4_USBD(%d): Unknown USB Req, bRequestType=%02x, bRequest=%02x, wValue=%04x, wIndex=%04x, wLength=%x\n", __LINE__,
				pUsbCtrlReq->bRequestType, pUsbCtrlReq->bRequest, \
				le16_to_cpu(pUsbCtrlReq->wValue),\
				le16_to_cpu(pUsbCtrlReq->wIndex),\
				le16_to_cpu(pUsbCtrlReq->wLength));
		}
		if( len < 0 ) {
			p4_usbd_set_ep0_stall();
			printk(KERN_ERR "P4_USBD(%d): Error USB response, bRequestType=%02x, bRequest=%02x, wValue=%04x, wIndex=%04x, wLength=%x\n", __LINE__,
				pUsbCtrlReq->bRequestType, pUsbCtrlReq->bRequest, \
				le16_to_cpu(pUsbCtrlReq->wValue),\
				le16_to_cpu(pUsbCtrlReq->wIndex),\
				le16_to_cpu(pUsbCtrlReq->wLength));
		} else {
			/* Transmit Data to Host */
			/* See Endpoint 0 IN byte count register (in0bc)(page 25) of Pilot4_USB2_0_HUB_V0.6,
			 * Writing to the in0bc register arms the IN 0 endpoint (the ep0cs.2 (inbsy) bit is automatically set to '1'). */
			if(!skip_data_stage)
			{ // Do this when control transfer have data stage.
				p4_usbd_write8_reg( len, P4_USBD_IN0BC );
			}
			else
			{ // Do this when control transfer do not have data stage.
				// If control write transfer have data stage, clear hsnak when EP0IN complete(p4_usbd_ivect_ep0in_comp_handler()).
				p4_usbd_clear_ep0_hsnak();
			}
		}
	} else { // Dev
		buf = usb_core_module.CoreUsbGetRxData(dev-1, 0);
		p4_usbd_hw_read_from_host(dev-1, 0, buf, &len, 1);
		pUsbCtrlReq=(struct usb_ctrlrequest *)buf;

#if (P4_USBD_DEBUG_SUDAV)
		printk("P4_USBD_DEBUG(%d): bRequestType=%02x, bRequest=%02x, wValue=%04x, wIndex=%04x, wLength=%04x, DBGCount=%ld\n", __LINE__,
			pUsbCtrlReq->bRequestType, pUsbCtrlReq->bRequest, \
			le16_to_cpu(pUsbCtrlReq->wValue),\
			le16_to_cpu(pUsbCtrlReq->wIndex),\
			le16_to_cpu(pUsbCtrlReq->wLength), DBGCount++);
#endif

		usb_core_module.CoreUsbSetRxDataLen(dev-1, 0, pUsbCtrlReq->wLength);
		usb_core_module.CoreUsbRxHandler0(dev-1, buf, pUsbCtrlReq->wLength, 1);

		// ivan:is it necessary?
		if((pUsbCtrlReq->bRequestType & USB_DIR_IN) == USB_DIR_OUT) {
			if(pUsbCtrlReq->wLength > 0){
				p4_usbd_write8_reg(0x0, P4_USBD_DEVXOUT0BC(dev-1));
			}
		}
	}

	kfree( (void *)qworker_data );
	return 0;
}

/* Contents of ivect register: 0x0 */
static uint8_t p4_usbd_ivect_sudav_handler(uint8_t devNum, uint8_t ivect)
{
	struct p4_usbd_workqueue_t *p4_usbd_qworker_data = (struct p4_usbd_workqueue_t*)kmalloc(sizeof(struct p4_usbd_workqueue_t), GFP_ATOMIC);
	if( p4_usbd_qworker_data == NULL )
	{
		panic("p4_usbd.c");
	}	
	if(devNum==0)
		p4_usbd_write8_reg( P4_USBHUB_USBIRQ_SUDAVIR, P4_USBD_USBIRQ);
	else
		p4_usbd_write32_reg(P4_USBD_BIT1_SET, P4_USBD_DEVXEP0INTSTS(devNum-1));

	INIT_WORK((struct work_struct*) p4_usbd_qworker_data, (void *)&p4_usbd_ivect_sudav_handler_work);
	p4_usbd_qworker_data->devNum=devNum;
	queue_work(p4_usbd_dv_wq,  (struct work_struct *)p4_usbd_qworker_data);

	return 0;
}

/* Contents of ivect register: 0x0C */
static uint8_t p4_usbd_ivect_suspend_handler(uint8_t dev, uint8_t ivect)
{
	if ( dev == 0 ) { // Hub
		p4_usbd_hub_suspended = 1;
		p4_usbd_write8_reg(P4_USBHUB_USBIRQ_SUSPIR, P4_USBD_USBIRQ);
	} else { // Dev
		printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	}
	return 0;
}

/* Contents of ivect register: 0x10 */
static uint8_t p4_usbd_ivect_usbreset_handler(uint8_t dev, uint8_t ivect)
{
	if( dev == 0 ) { // Hub
		p4_usbd_state_init(0);
		p4_usbd_enable_EP1();
		p4_usbd_write8_reg(P4_USBHUB_USBIRQ_URESIR, P4_USBD_USBIRQ);
	} else { // Dev
		p4_usbd_write32_reg(P4_USBD_BIT4_SET, P4_USBD_DEVXEP0INTSTS(dev-1));
		p4_usbd_write32_reg(1, P4_USBD_DEVX2HUBPORTRSTDB(dev-1));
		/* Followed Pilot4 SDK 20160630.PILOT4SDK.tbz, see line 1548 of pilot4_hub_multi_dev.c. */
		udelay(1);
		p4_usbd_write32_reg(0, P4_USBD_DEVX2HUBPORTRSTDB(dev-1));
	}
	return 0;
}

/* Contents of ivect register: 0x14 */
static uint8_t p4_usbd_ivect_hspeed_handler(uint8_t dev, uint8_t ivect)
{
	if ( dev == 0 ) { // Hub
		p4_usbd_write8_reg( P4_USBHUB_USBIRQ_HSPEEDIR, P4_USBD_USBIRQ);
	} else { // Dev
		printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	}
	return 0;
}

/* Contents of ivect register: 0x18 */
static uint8_t p4_usbd_ivect_ep0in_comp_handler(uint8_t dev, uint8_t ivect)
{
	if ( dev == 0 ) { // Hub
		p4_usbd_write8_reg( P4_USBD_BIT0_SET, P4_USBD_IN07IRQ);
		p4_usbd_clear_ep0_hsnak();
	} else { // Dev
		p4_usbd_write32_reg(P4_USBD_BIT2_SET, P4_USBD_DEVXEP0INTSTS(dev-1));
		usb_core_module.CoreUsbTxHandler(dev-1, 0);
	}
	return 0;
}

/* Contents of ivect register: 0x1c */
static uint8_t p4_usbd_ivect_ep0out_comp_handler(uint8_t dev, uint8_t ivect)
{
	uint8_t *buf;
	uint16_t len = 0;

	if ( dev == 0 ) { // Hub
		printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	} else { // Dev
		p4_usbd_write32_reg(P4_USBD_BIT3_SET, P4_USBD_DEVXEP0INTSTS(dev-1));
		buf = usb_core_module.CoreUsbGetRxData(dev-1, 0);
		p4_usbd_hw_read_from_host(dev-1, 0, buf, &len, 0);
		usb_core_module.CoreUsbSetRxDataLen(dev-1, 0, len);
		usb_core_module.CoreUsbRxHandler0(dev-1, buf, len, 0);

		p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_DEVXEP0CTRL(dev-1)) | P4_USBD_BIT1_SET, P4_USBD_DEVXEP0CTRL(dev-1)); // add to fix eth issue.
	}
	return 0;
}

/* Contents of ivect register: 0x24 */
static uint8_t p4_usbd_ivect_ep1in_comp_handler(uint8_t dev, uint8_t ivect)
{
	if ( dev == 0 ) { // Hub
		p4_usbd_write8_reg( P4_USBD_BIT1_SET, P4_USBD_IN07IRQ);
	} else { // Dev
		printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s, dev=%d, ivect=%x\n", __LINE__, __FUNCTION__, dev, ivect);
	}
	return 0;
}

static uint8_t p4_usbd_ivect_epXin_comp_handler(uint8_t dev, uint8_t ivect)
{
	uint8_t epNum = (ivect-9)/3 + 1;
#if P4_USBD_DMA_SUPPORT
	struct p4_usbd_dma_t* p4_usbd_dma_ptr = &p4_usbd_dma[ 1 + epNum ];
#endif

	if ( (epNum > 12) | (epNum < 2) ) {
		printk(KERN_WARNING "P4_USBD(%d): Unknown ep num: %d\n", __LINE__, epNum);
		return 1;
	}

	if ( dev == 0 ) { // Hub
		printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s, dev=%d, ivect=%x\n", __LINE__, __FUNCTION__, dev, ivect);
		return 1;
	} else { // Dev
#if P4_USBD_DMA_SUPPORT
		if(!p4_usbd_dma_ptr->isDma) {
			p4_usbd_write32_reg( 1 << (epNum - 1), P4_USBD_DEVXEPXINTSTS(dev-1));
		}
#else
		p4_usbd_write32_reg( 1 << (epNum - 1), P4_USBD_DEVXEPXINTSTS(dev-1));
#endif
		usb_core_module.CoreUsbTxHandler(dev-1, epNum);
	}
	return 0;
}

static uint8_t p4_usbd_ivect_epXout_comp_handler(uint8_t dev, uint8_t ivect)
{
	uint8_t epNum = (ivect-10)/3 + 1;
	uint32_t val = 0;
	uint8_t *buf;
	uint16_t len;
#if P4_USBD_DMA_SUPPORT
	struct p4_usbd_dma_t* p4_usbd_dma_ptr = &p4_usbd_dma[ 1 + P4_USBD_MAX_IN_EP + epNum ];
#endif

	if ( (epNum > 8) | (epNum < 2) ) {
		printk(KERN_WARNING "P4_USBD(%d): Unknown ep num: %d\n", __LINE__, epNum);
		return 1;
	}

	if ( dev == 0 ) { // Hub
		printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s, dev=%d, ivect=%x\n", __LINE__, __FUNCTION__, dev, ivect);
		return 1;
	} else { // Dev
#if P4_USBD_DMA_SUPPORT
		if(!p4_usbd_dma_ptr->isDma) {
			val = (p4_usbd_read32_reg(P4_USBD_DEVXEPXINTSTS(dev-1)) & (1 << ((epNum - 1) + 16)));
			p4_usbd_write32_reg(val, P4_USBD_DEVXEPXINTSTS(dev-1));
		}
#else
		val = (p4_usbd_read32_reg(P4_USBD_DEVXEPXINTSTS(dev-1)) & (1 << ((epNum - 1) + 16)));
		p4_usbd_write32_reg(val, P4_USBD_DEVXEPXINTSTS(dev-1));
#endif

		buf = usb_core_module.CoreUsbGetRxData(dev-1, epNum);
		p4_usbd_hw_read_from_host(dev-1, epNum, buf, &len, 0);
		usb_core_module.CoreUsbSetRxDataLen(dev-1, epNum, len);
		usb_core_module.CoreUsbRxHandler(dev-1, epNum);
	}
	return 0;
}

/* Contents of ivect register: 0xF0 */
static uint8_t p4_usbd_ivect_dma_handler(uint8_t dev, uint8_t ivect)
{
#if P4_USBD_DMA_SUPPORT
	uint32_t status;
	uint16_t allInEpDMAStatus, i;
	uint8_t allOutEpDMAStatus;
	if ( dev == 0 ) { // Hub
		printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s, dev=%d, ivect=%x\n", __LINE__, __FUNCTION__, dev, ivect);
		return 1;
	} else {
		status = p4_usbd_read32_reg(P4_USBD_DEVXDMACHXINTSTS(dev-1));
		allInEpDMAStatus=(status&0x0FFF);
		allOutEpDMAStatus=( (status>>16) & 0x00FF );
		if(allOutEpDMAStatus){
			for(i=0;i<8;i++){
				if( allOutEpDMAStatus & (1<<i) ) {
					p4_usbd_write32_reg( (1 << (i + 16)), P4_USBD_DEVXDMACHXINTSTS(dev-1));
					// ivect = (outEpNum-1)*3 +10;
					// outEpNum = i+1;
					// ivect = ( (i+1) - 1 )*3+10 = i*3 +10
					p4_usbd_ivect_epXout_comp_handler(dev, i*3+10);
					break;
				}
			}
		}
		if(allInEpDMAStatus){
			for(i=0;i<12;i++){
				if( allInEpDMAStatus & (1<<i) ) {
					p4_usbd_write32_reg((1 << i), P4_USBD_DEVXDMACHXINTSTS(dev-1));
					// ivect = (inEpNum-1)*3 + 9
					// inEpNum = i+1
					// ivect = ( (i+1) -1 )*3 + 9 = i*3+9
					p4_usbd_ivect_epXin_comp_handler(dev, i*3 + 9);
					break;
				}
			}
		}
	}
#else
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s, dev=%d, ivect=%x\n", __LINE__, __FUNCTION__, dev, ivect);
#endif
	return 0;
}

/* Contents of ivect register: 0xF8 */
static uint8_t p4_usbd_ivect_devX_to_hub_handler(uint8_t dev, uint8_t ivect)
{
	uint8_t i=0;
	uint32_t val1;
	if ( dev == 0 ) { // Hub
		for( i = 0; i < P4_USBD_MAX_DEV_NUM; i++) {
			val1 = p4_usbd_read32_reg(P4_USBD_DEVX2HUBINTSTS(i));
			/* bit0 for DeviceX Connect/Disconnect Interrupt */
			if( val1 & 1 ) {
				p4_usbd_write32_reg(1, P4_USBD_DEVX2HUBINTSTS(i));
				if( !p4_usbd_read32_reg(P4_USBD_DEVXDISCONDB(i)) ){
					p4_usbd_port_enable(i);
				} else {
					p4_usbd_port_disable(i);
				}
			}

			/* bit1 for DeviceX Port Reset Done Interrupt */
			if ( val1 & 2 ) {
				p4_usbd_write32_reg(2, P4_USBD_DEVX2HUBINTSTS(i));
				p4_usbd_port_enable(i);
			}

			/* bit3 for DeviceX Mail Box Interrupt */
			if ( val1 & 8 ) {
				printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
			}
		}
	} else { // Dev
		printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	}
	return 0;
}

static uint8_t (* p4_usbd_ivect_handler[]) (uint8_t dev, uint8_t ivect) = 
{
	p4_usbd_ivect_sudav_handler,				// 0
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_suspend_handler,
	p4_usbd_ivect_usbreset_handler,
	p4_usbd_ivect_hspeed_handler,				// 5
	p4_usbd_ivect_ep0in_comp_handler,
	p4_usbd_ivect_ep0out_comp_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_ep1in_comp_handler,
	p4_usbd_ivect_dummy_handler,				// 10
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_epXin_comp_handler,
	p4_usbd_ivect_epXout_comp_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_epXin_comp_handler,			// 15
	p4_usbd_ivect_epXout_comp_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_epXin_comp_handler,
	p4_usbd_ivect_epXout_comp_handler,
	p4_usbd_ivect_dummy_handler,				// 20
	p4_usbd_ivect_epXin_comp_handler,
	p4_usbd_ivect_epXout_comp_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_epXin_comp_handler,
	p4_usbd_ivect_epXout_comp_handler,			// 25
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_epXin_comp_handler,
	p4_usbd_ivect_epXout_comp_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_epXin_comp_handler,			// 30
	p4_usbd_ivect_epXout_comp_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_epXin_comp_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,				// 35
	p4_usbd_ivect_epXin_comp_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_epXin_comp_handler,
	p4_usbd_ivect_dummy_handler,				// 40
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_epXin_comp_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,				// 45
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,				// 50
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,				// 55
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_dma_handler,					// 60
	p4_usbd_ivect_dummy_handler,
	p4_usbd_ivect_devX_to_hub_handler,
};

static uint8_t p4_usbd_get_power_status(void)
{
	int swc_stat0_reg;
	int swc_stat0_val;
	
	swc_stat0_reg = (int)IO_ADDRESS(SE_SYS_WAKEUP_BASE+0xa);
	swc_stat0_val=*(volatile char *)(swc_stat0_reg);

	return (swc_stat0_val & 0x8); 
}

static uint8_t p4_usbd_enable_EP1(void)
{
	uint8_t val;
	val = p4_usbd_read8_reg(P4_USBD_INxCON[1]);
	/* Set to Single Buffering */
	val = val & (P4_USBD_EP_SINGLE_BUFFER);
	/* Followed 20160331.PILOT4SDK pilot4_hub_multi_dev.c */
	val = val | ((u8)(P4_USBD_EP_EP_VALID | (USB_ENDPOINT_XFER_INT<<2) ));
	p4_usbd_write8_reg( val, P4_USBD_INxCON[1]);
	/* Followed 20160331.PILOT4SDK pilot4_hub_multi_dev.c */
	p4_usbd_write8_reg( (P4_USBD_FIFOCTRL_DIR | P4_USBD_FIFOCTRL_AUTO | 1), P4_USBD_FIFOCTRL);
    p4_usbd_reset_data_toggle(DIR_IN, 1);
    p4_usbd_do_fifo_reset(DIR_IN, 1);
    p4_usbd_write8_reg(p4_usbd_read8_reg(P4_USBD_IN07IEN) | (P4_USBD_BIT1_SET),  P4_USBD_IN07IEN);

	return 0;
}

static uint8_t p4_usbd_hw_init(void)
{
	uint32_t val;

	if( !p4_usbd_get_power_status() ) {
		return 0;
	}

	p4_usbd_hub_cfg_ep0_buf = (void * __iomem)p4_usbd_hub_cfg_reg_va + P4_USBD_EP0INDAT;
	/* 
	 * Enable reset, suspend and high speed interrupt in general which is common for all devices
	 * Enable DataValid, EP0 in and EP0 out for this device
	 */
	p4_usbd_write8_reg(0x0, P4_USBD_USBIEN);	// sudavir, sofir, sutokir, suspir, uresir, hspeedir
	p4_usbd_write8_reg(0x0, P4_USBD_USBIRQ);	// sudavir, sofir, sutokir, suspir, uresir, hspeedir
	p4_usbd_write8_reg( P4_USBD_USBIEN_SUDAVIE | P4_USBD_USBIEN_SUSPIE | P4_USBD_USBIEN_URESIE | P4_USBD_USBIEN_HSPIE, P4_USBD_USBIEN);
	if ( p4_usbd_read8_reg(P4_USBD_USBIEN) != (P4_USBD_USBIEN_SUDAVIE | P4_USBD_USBIEN_SUSPIE | P4_USBD_USBIEN_URESIE | P4_USBD_USBIEN_HSPIE)) {
		printk(KERN_ERR "P4_USBD(%d): Can't enable usb hub interrupt\n", __LINE__);
		return -1;
	}
	p4_usbd_write8_reg((u8)(P4_USBD_BIT0_SET), P4_USBD_IN07IEN);		// Enable In 0 Interrupt
	p4_usbd_write8_reg((u8)(P4_USBD_BIT0_SET), P4_USBD_OUT07IEN);		// Enanble Out 0 Interrupt
	p4_usbd_write32_reg(0x0, P4_USBD_PORTENABLE);						// Disable All Port

	/* IVAN,DOUBT: SPEC use 0x20 but 20160331.PILOT4SDK pilot4_hub_multi_dev.c use 0x02. */
	/* Enable the multiple devices support. */
	p4_usbd_write32_reg(0x02, 0x514);
	/* Followed 20160331.PILOT4SDK pilot4_hub_multi_dev.c */
	*(volatile uint32_t*)IO_ADDRESS(SE_SYS_CLK_BASE + 0x54) = 0x630;

	p4_usbd_enable_EP1();

	p4_usbd_write8_reg(P4_USBD_INTR_EN_INTEN, P4_USBD_INTR_EN);										// Enable usbintreq
	p4_usbd_write32_reg(p4_usbd_read32_reg(P4_USBD_BMCINTEN) | P4_USBD_BIT0_SET, P4_USBD_BMCINTEN);	// Enable Hub interrupt
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
	if(p4_usbd_hub_connect_to_upstream == 1)
#endif
	{
		val = p4_usbd_read8_reg(P4_USBD_USBCS) & (u8)~(P4_USBD_BIT6_SET);
		p4_usbd_write8_reg(val, P4_USBD_USBCS);
	}	

#if (P4_USBD_DEBUG_HWINIT)
	printk("P4_USBD_DEBUG(%d): function %s completed\n", __LINE__, __FUNCTION__);
#endif

	return 0;
}

static int  p4_usbd_SWC_interrupt_cb(void)
{
	uint8_t status = 0;
	uint8_t devNum = 0;

	/* Interrupt handlers execute as an asychronous event and in atomic context (interrupt context)often disabling 
	 local processor IRQ disabled on disabling the interrupt line for interrupt for which ISR is in execution.
	 There are many reasons and implications of having a sleep in ISR like interrupt latency, sychronization mess etc.
	 There have been problems discussed on not to having sleep in ISR , But havnt reproduce it so just commenting the code.*/
	
	/*Delay of 10 microseconds is added inorder to avoid the conflict of accessing
	  the registers simultaneously by p4_usbd_SWC_interrupt() and miscctrl_SWMhandler()
	  as, both handler functions will be called when the interrupt occcurs.*/
	//udelay(10);	

	if( p4_usbd_get_power_status() ) {
		status = p4_usbd_hw_init();
		if (status) {
			printk(KERN_ERR "P4_USBD(%d): USB Device Hardware init fail.\n", __LINE__);
			return -1;
		}
		for ( devNum = 0; devNum < P4_USBD_MAX_DEV_NUM; devNum++ ) {
			if(p4_usbd_device_hwEnabled[devNum] == 1) {
				p4_usbd_hw_enable_dev_fun (&devNum);
			}
		}
	} else {
		for ( devNum = 0; devNum < P4_USBD_MAX_DEV_NUM; devNum++ ) {
			if(p4_usbd_device_hwEnabled[devNum] == 1) {
				p4_usbd_hw_disable_dev_fun (devNum);
			}
		}
	}
	return 0;
}

static irqreturn_t p4_usbd_interrupt_dev(int irq, void *dev_id)
{
	uint8_t ivect=0;
	uint8_t dev = irq - P4_USBD_IRQ_HUB;

	if ( ( dev > 0 ) && ( dev < 5 ) ) {
		ivect = p4_usbd_read8_reg( P4_USBD_DEVXIVECT(dev-1) );
		ivect = ivect >> 2;
#if P4_USBD_DEBUG_IVECT
		printk(KERN_INFO "P4_USBD(%d): devNo=%d, ivect=%x\n", __LINE__, dev, ivect);
#endif
		p4_usbd_ivect_handler[ivect](dev, ivect);
		p4_usbd_write8_reg( (P4_USBD_BIT0_SET << dev), P4_USBD_BMCINTSTS );
	} else
		printk(KERN_ERR "P4_USBD(%d): Unknow device number\n", __LINE__);

	return IRQ_HANDLED;
}

static irqreturn_t p4_usbd_interrupt(int irq, void *dev_id)
{
	uint8_t ivect=0;
	if( irq == 62 ) { // Hub
		ivect = p4_usbd_read8_reg( P4_USBD_IVECT );
		ivect = ivect >> 2;
#if P4_USBD_DEBUG_IVECT
		printk(KERN_INFO "P4_USBD(%d): hub ivect=%x\n", __LINE__, ivect);
#endif
		p4_usbd_ivect_handler[ivect](0, ivect);
		p4_usbd_write8_reg( P4_USBD_BIT0_SET, P4_USBD_BMCINTSTS );
	} else
		printk(KERN_ERR "P4_USBD(%d): Unknow device number\n", __LINE__);

	return IRQ_HANDLED;
}

static int __init p4_usbd_probe(struct platform_device *pdev)
{
	uint8_t i, status = -ENODEV;

	p4_usbd_pc_wq = alloc_workqueue("p4_usbd_pc_wq", WQ_HIGHPRI, 1);
	p4_usbd_dv_wq = alloc_workqueue("p4_usbd_dv_wq", WQ_HIGHPRI, 1);

	status = request_irq(P4_USBD_IRQ[0]/*IRQ No here*/, &p4_usbd_interrupt, IRQF_SHARED, p4_usbd_driver_name, pdev);
	if(status < 0) {
		printk(KERN_ERR "P4_USBD(%d): request_irq failed for dev %d\n", __LINE__, i);
		goto cleanup;
	}

	for(i = 1; i < P4_USBD_MAX_DEV_PORT; i++) {
		status = request_irq(P4_USBD_IRQ[i]/*IRQ No here*/, &p4_usbd_interrupt_dev, IRQF_SHARED, p4_usbd_driver_name, pdev);
		if(status < 0) {
			printk(KERN_ERR "P4_USBD(%d): request_irq failed for dev %d\n", __LINE__, i);
			goto cleanup;
		}
	}

	install_swc_handler(p4_usbd_SWC_interrupt_cb, POWER_GOOD_INTR);

	status = p4_usbd_hw_init();

	if (status) {
		printk(KERN_ERR "P4_USBD(%d): USB Device Hardware init fail.\n", __LINE__);
	}
	return status;

cleanup:
	return status;
}

static const struct of_device_id p4_of_usb_match[] = {
	{ .compatible = "p4,usb-2.0-hub", .data = NULL, },
	{},
};

static void p4_usbd_shutdown(struct platform_device *dev)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
}

static struct platform_driver p4_usbd_driver = {
	.shutdown	= p4_usbd_shutdown,
	.driver	 = {
		.owner  = THIS_MODULE,
		.name   = (char *) "pilo4_usb_dev",
		.of_match_table = p4_of_usb_match,
	},
};

static void p4_usbd_release (struct device *dev)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
}

static struct platform_device p4_usbd_pdev = {
	.name		    = (char *) "pilo4_usb_dev",
	.id			 	= -1,
	.dev			= {
		.release	= p4_usbd_release,
	},
};

//EP 0~2 Max size: 64
//EP 2,3 for HID In, EP 4, 5, 6, 7, 8, 9, 10, 11 for Virtual Media In and Out
static ep_config_t p4_usbd_ep_configs[P4_USBD_USE_DEV_NUM][P4_USBD_DEV_EP_NUM] = {
	{
		//cdrom
		//{ 2, IN,  P4_USBD_INTR_EP_SIZE, CONFIGURABLE, 0 },
		//{ 3, IN,  P4_USBD_INTR_EP_SIZE, CONFIGURABLE, 0 },
		{ 8, IN,  P4_USBD_BULK_EP_SIZE, CONFIGURABLE, 0 },
		{ 4, OUT, P4_USBD_BULK_EP_SIZE, CONFIGURABLE, 0 },
	}, 
	{
		//Hdisk
		//{ 2, IN,  P4_USBD_INTR_EP_SIZE, CONFIGURABLE, 0 },
		//{ 3, IN,  P4_USBD_INTR_EP_SIZE, CONFIGURABLE, 0 },
		{ 9, IN,  P4_USBD_BULK_EP_SIZE, CONFIGURABLE, 0 },
		{ 5, OUT, P4_USBD_BULK_EP_SIZE, CONFIGURABLE, 0 },
	},
	{
		//hid
		{ 2, IN,  P4_USBD_INTR_EP_SIZE, CONFIGURABLE, 0 },
		{ 3, IN,  P4_USBD_INTR_EP_SIZE, CONFIGURABLE, 0 },
		{10, IN,  P4_USBD_BULK_EP_SIZE, CONFIGURABLE, 0 },
		{ 6, OUT, P4_USBD_BULK_EP_SIZE, CONFIGURABLE, 0 },
	},
	{
		//ETH
		{ 12, IN,  P4_USBD_INTR_EP_SIZE, CONFIGURABLE, 0 },
		//{ 3, IN,  P4_USBD_INTR_EP_SIZE, CONFIGURABLE, 0 },
		{11, IN,  P4_USBD_BULK_EP_SIZE, CONFIGURABLE, 0 },
		{ 7, OUT, P4_USBD_BULK_EP_SIZE, CONFIGURABLE, 0 },
	},
};
static uint8 p4_ep_devnum_seq[]={CREATE_CDROM_DESCRIPTOR, CREATE_HARDDISK_DESCRIPTOR, CREATE_HID_DESCRIPTOR, CREATE_ETH_DESCRIPTOR};

static int p4_usbd_read_fifo_by_ep(uint8_t devNum, uint8_t epNum, uint8_t *buf, uint16_t *len)
{
	uint16_t i = 0;

	*(len) = p4_usbd_read16_reg(P4_USBD_OUTxBCL[epNum]);
	for(i = 0 ; i < *(len); i++) {
		*(buf) = p4_usbd_read8_reg(P4_USBD_FIFOxDAT[epNum]);
		buf++;
	}
	// sets the busy bit by reloading the outxcs register with a dummy value
	p4_usbd_write8_reg(0, P4_USBD_OUTxCS[epNum]);
	return 0;
}

#if P4_USBD_DMA_SUPPORT
static int p4_usbd_read_DMA_by_ep(uint8_t devNum, uint8_t epNum, uint8_t *buf, uint16_t *len)
{
	struct p4_usbd_dma_t* p4_usbd_dma_ptr = NULL;
	uint8_t* p4_usbd_dma_virt;
	uint16_t p4_usbd_dma_current_data_len;
	uint32_t p4_usbd_dma_cbw_sign;
	uint32_t p4_usbd_dma_cbw_data_len;
	uint8_t p4_usbd_dma_cbw_flag;
	/* add dma_expect_data_len for each out ep, because use one variable dma_expect_data_len for every 
	 * ep will cause following issue.  EIP 309103.
	 *
	 *  EP |                Hex                |current_|  out_  |expect_data|expect_data|expect_data
	 * Num |               Data                |data_len|data_len|    len    | len(EP4)  | len(EP5)
	 * -------------------------------------------------------------------------------------------------
	 *  4  |43425355 00003B14 00000024 E30A0080|   31   |  0x24  |     31    |    31     |   31
	 *     |00000000 01000001 00000000         |        |        |           |           |
	 * -------------------------------------------------------------------------------------------------
	 *  5  |43425355 000000B8 00000012 03060180|   31   |  0x12  |     31    |    31     |   31
	 *     |12000000 00000000 00000000         |        |        |           |           |
	 * -------------------------------------------------------------------------------------------------
	 *  5  |43425355 000000B9 00000000 00060000|   31   |    0   |     31    |    31     |   31
	 *     |00000000 00000000 00000000         |        |        |           |           |
	 * -------------------------------------------------------------------------------------------------
	 *  4  |43425355 00003B15 00000024 E20A0000|   31   |  0x12  |     36    |    36     |   31
	 *     |00000000 01000001 00000000         |        |        |           |           |
	 * -------------------------------------------------------------------------------------------------
	 *  5  |43425355 000000BA 00000012 03060080|  *36   |  0x12  |    *36    |    36     |  *36
	 *     |12000000 00000000 00000000         |        |        |           |           |
	 * -------------------------------------------------------------------------------------------------
	 */
	static uint16_t p4_usbd_dma_expect_data_len[]={MAX_CBW_LENGTH, \
		MAX_CBW_LENGTH, MAX_CBW_LENGTH, MAX_CBW_LENGTH, MAX_CBW_LENGTH, 
		MAX_CBW_LENGTH, MAX_CBW_LENGTH, MAX_CBW_LENGTH, MAX_CBW_LENGTH, MAX_CBW_LENGTH};

	p4_usbd_dma_ptr = &p4_usbd_dma[ 1 + P4_USBD_MAX_IN_EP +epNum ];
	p4_usbd_dma_virt = (uint8_t*)p4_usbd_dma_ptr->out_virt_addr;
	p4_usbd_dma_current_data_len = p4_usbd_dma_expect_data_len[epNum];
	memcpy(&buf[0], &p4_usbd_dma_virt[0], p4_usbd_dma_current_data_len);
	memcpy(&p4_usbd_dma_cbw_sign, &buf[0], 4);

	*(len) = p4_usbd_dma_current_data_len;
	if(CBW_SIGNATURE == p4_usbd_dma_cbw_sign) {
		memcpy(&p4_usbd_dma_cbw_data_len, &buf[8], 4);
		memcpy(&p4_usbd_dma_cbw_flag, &buf[12], 1);
		*(len) = MAX_CBW_LENGTH;
		p4_usbd_dma_ptr->out_data_len = p4_usbd_dma_cbw_data_len;
		if(p4_usbd_dma_cbw_flag == 0) { 
			if(p4_usbd_dma_ptr->out_data_len >= P4_USBD_BULK_EP_SIZE)  {
				p4_usbd_dma_expect_data_len[epNum] = P4_USBD_BULK_EP_SIZE;
			} else if(p4_usbd_dma_ptr->out_data_len == 0) {
				p4_usbd_dma_expect_data_len[epNum] = MAX_CBW_LENGTH;
			} else {
				p4_usbd_dma_expect_data_len[epNum] = p4_usbd_dma_ptr->out_data_len;
			}
		}
	} else {
		p4_usbd_dma_ptr->out_data_len -= p4_usbd_dma_current_data_len;
		if(p4_usbd_dma_ptr->out_data_len >= P4_USBD_BULK_EP_SIZE)  {
			p4_usbd_dma_expect_data_len[epNum] = P4_USBD_BULK_EP_SIZE;
		} else if(p4_usbd_dma_ptr->out_data_len == 0) {
			p4_usbd_dma_expect_data_len[epNum] = MAX_CBW_LENGTH;
		} else {
			p4_usbd_dma_expect_data_len[epNum] = p4_usbd_dma_ptr->out_data_len;
		}
	}

	p4_usbd_write32_reg(p4_usbd_dma_ptr->out_phy_addr, P4_USBD_OUTDMAADDRX(epNum - 1));
	p4_usbd_write32_reg((p4_usbd_dma_expect_data_len[epNum] - 1), P4_USBD_OUTDMACTRLX(epNum - 1));
	p4_usbd_write32_reg(((1 << 31) | (p4_usbd_dma_expect_data_len[epNum] - 1)), P4_USBD_OUTDMACTRLX(epNum - 1));

	return 0;
}
#endif

static uint8_t p4_usbd_write_fifo_by_ep (uint8_t devNum, uint8_t epNum, uint8_t *buf, uint16_t len)
{
	uint16_t timeout=0;
	uint16_t i;

	while((p4_usbd_read8_reg(P4_USBD_INxCS[epNum]) & P4_USBD_BIT1_SET)) {
		udelay(10);
		if(timeout > 5000 ) { // About 50 mseconds
			printk(KERN_ERR "P4_USBD(%d): USB Write FIFO Error.\n", __LINE__);
			return -1;
		}
		timeout++;
	}

	for ( i=0; i< len; i++){
		p4_usbd_write8_reg(*(buf), P4_USBD_FIFOxDAT[epNum]);
		buf++;
	}
	p4_usbd_write16_reg(len, P4_USBD_INxBCL[epNum]);
	/* the microcontroller reloads the inxcs register with any value to set the busy bit. */
    p4_usbd_write8_reg(0, P4_USBD_INxCS[epNum]);

	return 0;
}

static uint8_t p4_usbd_ep0_read_fifo(uint8_t devNum, uint8_t epNum, uint8_t *buf, uint16_t *len)
{
	uint8_t i;
	*(len) = p4_usbd_read8_reg(P4_USBD_DEVXOUT0BC(devNum));
	for( i=0 ; i < (*len) ;i++) {
		*(buf) = p4_usbd_read8_reg(P4_USBD_DEVXOUT0BUF(devNum) + i);
		buf++;
	}

	p4_usbd_write8_reg((*len), P4_USBD_DEVXOUT0BC(devNum));
	// p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_DEVXEP0CTRL(devNum)) | P4_USBD_BIT1_SET, P4_USBD_DEVXEP0CTRL(devNum)); // marked to fix eth issue.

	return 0;
}

static uint8_t p4_usbd_ep0_write_fifo(uint8_t devNum, uint8_t epNum, uint8_t *buf, uint16_t len)
{
	uint16_t i=0;
	/* During set config length ==0 */
	if(len == 0) {
		/* See Pilot 4 SDK, 20160630.PILOT4SDK.tbz, line 2225 of pilot4_hub_multi_dev.c */
		/* For some devices we have to Delay the status in those cases
		 * there will be zero length requests, we just complete the status phase. */
		//p4_usbd_write8_reg ( p4_usbd_read8_reg(P4_USBD_DEVXEP0CTRL(devNum)) | P4_USBD_BIT1_SET, P4_USBD_DEVXEP0CTRL(devNum));
		/*  DEVxEP0CTRL-INBSY: The microprocessor sets this bit by reloading the in0bc register. */
		p4_usbd_write16_reg(len, P4_USBD_DEVXIN0BC(devNum));
		return 0;
	}
	for ( i = 0; i < len; i++) {
		p4_usbd_write8_reg(*(buf), P4_USBD_DEVXIN0BUF(devNum) + i);
		buf++;
	}
	// Write to the byte count register to set INBSY
	p4_usbd_write16_reg(len, P4_USBD_DEVXIN0BC(devNum));
	return 0;
}

static void p4_usbd_hw_enable_dev_fun (uint8_t *DeviceNo)
{
	/* See Pilot 4 SDK, 20160630.PILOT4SDK.tbz, line 912 of pilot4_hub_multi_dev.c use 0xBE,
	 * it conflict to SPEC Pilot4_USB_DEV_BEHIND_HUB_V0.4 page 36 on bit 6(Mailbox Interrupt Enable),
	 * use 0x7E to enable interrupt "Setup Data Valid", IN0, OUT0, Port Reset, Suspend
	 * and Mailbox. */
	p4_usbd_write8_reg(0x7E, P4_USBD_DEVXEP0INTEN(*DeviceNo));
	p4_usbd_write8_reg(0x0B, P4_USBD_DEVX2HUBINTEN(*DeviceNo));
    p4_usbd_write8_reg(p4_usbd_read8_reg(P4_USBD_BMCINTEN) | (2 << *DeviceNo), P4_USBD_BMCINTEN);
	p4_usbd_device_hwEnabled[*DeviceNo] = 1;
    usb_core_module.CoreUsbConfigureHS(*DeviceNo);
}

static void p4_usbd_hw_enable_dev (uint8_t DeviceNo, uint8_t Speed)
{
	if ( p4_usbd_get_power_status() ) {
		if ( p4_usbd_device_hwEnabled[DeviceNo] == 0 ) {
			p4_usbd_hw_enable_dev_fun (&DeviceNo);
		}
	} else {
		p4_usbd_device_hwEnabled[DeviceNo] = 1;
	}
}

static void p4_usbd_hw_disable_dev_fun (uint8_t DeviceNo)
{
	if ( p4_usbd_device_hwEnabled[DeviceNo] == 1) {
		p4_usbd_write8_reg(0x0, P4_USBD_DEVXEP0INTEN(DeviceNo));
		p4_usbd_write8_reg(0x0, P4_USBD_DEVX2HUBINTEN(DeviceNo));
		p4_usbd_write8_reg(p4_usbd_read8_reg(P4_USBD_BMCINTEN) & (~(2 << DeviceNo)), P4_USBD_BMCINTEN);
		// p4_usbd_device_hwEnabled[DeviceNo] = 0; this is not done, as it has to be enabled when power on.
	}
}

static void p4_usbd_hw_disable_dev (uint8_t DeviceNo, uint8_t Speed)
{
	p4_usbd_hw_disable_dev_fun(DeviceNo);
	p4_usbd_device_hwEnabled[DeviceNo] = 0;
}

static int p4_usbd_hw_intr (uint8_t DeviceNo,uint8_t Ep)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	return 0;
}

static void p4_usbd_hw_set_addr (uint8_t devNum,uint8_t addr, uint8_t enable)
{
	if( addr == 0 )
		return;
	p4_usbd_write8_reg(addr, P4_USBD_DEVXFNADDR(devNum));
}

static void p4_usbd_hw_get_addr (uint8_t DeviceNo,uint8_t *Addr, uint8_t *Enable)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
}

static int p4_usbd_hw_write_to_host (uint8_t devNum, uint8_t epNum, uint8_t *buf, uint16_t len)
{
	uint8_t retry = 0;
#if P4_USBD_DMA_SUPPORT
	struct p4_usbd_dma_t* p4_usbd_dma_ptr = NULL;
	uint8_t* p4_usbd_dma_virt;
#endif

	if (epNum == 0) {
		while ( p4_usbd_read8_reg(P4_USBD_DEVXEP0CTRL(devNum)) & P4_USBD_BIT2_SET ) {
			mdelay(1);
			if ( retry > 250 ) {
				printk(KERN_ERR "P4_USBD(%d): Send data to host error.\n", __LINE__);
				return -1;
			}
			retry++;
		}
		if ( len != 0 )
			p4_usbd_ep0_write_fifo(devNum, (epNum & 0x7F), buf, len);
	} else {
		if ( len != 0 ) {
#if P4_USBD_DMA_SUPPORT
			p4_usbd_dma_ptr = &p4_usbd_dma[ 1 + epNum ];
			if ( !p4_usbd_dma_ptr->isDma ) { 
				p4_usbd_write_fifo_by_ep(devNum, (epNum & 0x7F), buf, len);
			} else {
				p4_usbd_dma_virt = (uint8_t*)p4_usbd_dma_ptr->in_virt_addr;
				memcpy(&p4_usbd_dma_virt[0], &buf[0], len);
				p4_usbd_write32_reg(p4_usbd_dma_ptr->in_phy_addr, P4_USBD_INDMAADDRX(epNum-1));
				p4_usbd_write32_reg((len - 1), P4_USBD_INDMACTRLX(epNum - 1));
				p4_usbd_write32_reg(((1 << 31) | (len - 1)), P4_USBD_INDMACTRLX(epNum - 1));
			}
#else
			p4_usbd_write_fifo_by_ep(devNum, (epNum & 0x7F), buf, len);
#endif
		}
	}
	return 0;
}

static void p4_usbd_hw_tx_complete (uint8_t DeviceNo,uint8_t Ep)
{
	return;
}

static int p4_usbd_hw_read_from_host(uint8_t devNum, uint8_t epNum, uint8_t *buf, uint16_t *len, uint8_t is_setup)
{
#if P4_USBD_DMA_SUPPORT
	struct p4_usbd_dma_t* p4_usbd_dma_ptr = NULL;
#endif

	if ( epNum == 0 ) {
		if (is_setup) {
			/* read request data */
			*len = 8;
			memcpy(buf, (void * __iomem)p4_usbd_hub_cfg_reg_va + P4_USBD_DEVXSETUPBUF(devNum), *len);
		} else {
			p4_usbd_ep0_read_fifo(devNum, epNum, buf, len);
		}
	} else {
#if P4_USBD_DMA_SUPPORT
		p4_usbd_dma_ptr = &p4_usbd_dma[ 1 + P4_USBD_MAX_IN_EP +epNum ];
		if ( !p4_usbd_dma_ptr->isDma ) {
			p4_usbd_read_fifo_by_ep(devNum, epNum, buf, len);
		} else {
			p4_usbd_read_DMA_by_ep(devNum, epNum, buf, len);
		}
#else
		p4_usbd_read_fifo_by_ep(devNum, epNum, buf, len);
#endif
	}
	return 0;
}

static int p4_usbd_hw_alloc_buffer (uint8_t devNum, uint8_t epNum, uint16_t fifoSize, uint16_t maxPkt, uint8_t dir, uint8_t epType)
{
#if P4_USBD_DMA_SUPPORT
	int USB_DevType;
	struct p4_usbd_dma_t* p4_usbd_dma_ptr = NULL;
#endif

	if( epNum == 0 )
		return 0;

#if P4_USBD_DMA_SUPPORT
	/*Can't use DMA with USB NIC so disable DMA mode if USB Dev Type is ETH */
	USB_DevType = usb_core_module.CoreUsbGetDeviceType(devNum);
	if(USB_DevType == CREATE_ETH_DESCRIPTOR)
	{
		if( dir == DIR_IN )
		{
			p4_usbd_dma_ptr = &p4_usbd_dma[1 + epNum];
			p4_usbd_dma_ptr->isDma = 0;
		}
		else
		{
			p4_usbd_dma_ptr = &p4_usbd_dma[1 + P4_USBD_MAX_IN_EP + epNum];
			p4_usbd_dma_ptr->isDma = 0;
		}	
		return 0;
	}	
	if(USB_DevType == CREATE_HID_DESCRIPTOR) 
	{ 
		p4_usbd_dma_ptr = &p4_usbd_dma[1 + epNum];
		p4_usbd_dma_ptr->isDma = 0;
		return 0;
	}

	if( dir == DIR_IN ) {
		p4_usbd_dma_ptr = &p4_usbd_dma[ 1 + epNum ];
		p4_usbd_dma_ptr->in_virt_addr = dma_alloc_coherent(NULL, P4_USBD_MAX_DMA_PKTS, &p4_usbd_dma_ptr->in_phy_addr, GFP_DMA|GFP_KERNEL);
		if (p4_usbd_dma_ptr->in_virt_addr == NULL) {
			printk(KERN_WARNING "P4_USBD(%d): Allocate In DMA for ep%d error.\n", __LINE__, epNum);
			return -1;
		}
		p4_usbd_dma_ptr->isDma = 1;
    } else {
		p4_usbd_dma_ptr = &p4_usbd_dma[ 1 + P4_USBD_MAX_IN_EP + epNum ];
		p4_usbd_dma_ptr->out_virt_addr = dma_alloc_coherent(NULL, P4_USBD_MAX_DMA_PKTS, &p4_usbd_dma_ptr->out_phy_addr, GFP_DMA|GFP_KERNEL);
		if (p4_usbd_dma_ptr->out_virt_addr == NULL) {
			printk(KERN_WARNING "P4_USBD(%d): Allocate Out DMA for ep%d error.\n", __LINE__, epNum);
			return -1;
		}
		p4_usbd_dma_ptr->isDma = 1;
		p4_usbd_write32_reg(p4_usbd_dma_ptr->out_phy_addr, P4_USBD_OUTDMAADDRX(epNum - 1));
		p4_usbd_write32_reg((MAX_CBW_LENGTH - 1), P4_USBD_OUTDMACTRLX(epNum - 1));
		p4_usbd_write32_reg(((1 << 31) | (MAX_CBW_LENGTH - 1)), P4_USBD_OUTDMACTRLX(epNum - 1));
	}
#endif

	return 0;
}

static int p4_usbd_hw_enable_ep (uint8_t devNum, uint8_t epNum, uint8_t epDir, uint8_t epType)
{
	uint32_t val;
	switch (epType & USB_ENDPOINT_XFERTYPE_MASK) {
		case USB_ENDPOINT_XFER_INT:
			if(epDir == DIR_IN) {
				p4_usbd_write8_reg(0, P4_USBD_INxCON[epNum]);
				p4_usbd_write8_reg( ( P4_USBD_EP_EP_VALID | (USB_ENDPOINT_XFER_INT << 2) | P4_USBD_EP_SINGLE_BUFFER), P4_USBD_INxCON[epNum]);
				p4_usbd_write8_reg( ( P4_USBD_FIFOCTRL_DIR | P4_USBD_FIFOCTRL_AUTO | epNum), P4_USBD_FIFOCTRL);
				p4_usbd_reset_data_toggle(DIR_IN, epNum);
				p4_usbd_do_fifo_reset(DIR_IN, epNum);
#if P4_USBD_DMA_SUPPORT
				if( !p4_usbd_dma[1 + epNum].isDma ) {
					val = p4_usbd_read32_reg(P4_USBD_DEVXEPXINTEN(devNum));
					p4_usbd_write32_reg( (val | (1 << (epNum - 1))), P4_USBD_DEVXEPXINTEN(devNum));
				} else {
					val = p4_usbd_read32_reg(P4_USBD_DEVXDMACHXINTEN(devNum));
					p4_usbd_write32_reg(( val | ( 1 << (epNum-1) )), P4_USBD_DEVXDMACHXINTEN(devNum));
					// Clear status
					p4_usbd_write32_reg(( val | ( 1 << (epNum-1) )), P4_USBD_DEVXDMACHXINTSTS(devNum));
				}
#else
				val = p4_usbd_read32_reg(P4_USBD_DEVXEPXINTEN(devNum));
				p4_usbd_write32_reg( (val | (1 << (epNum - 1))), P4_USBD_DEVXEPXINTEN(devNum));
#endif
			} else {
				p4_usbd_write8_reg(0, P4_USBD_OUTxCON[epNum]);
				p4_usbd_write8_reg( ( P4_USBD_EP_EP_VALID | (USB_ENDPOINT_XFER_INT << 2) | P4_USBD_EP_SINGLE_BUFFER), P4_USBD_OUTxCON[epNum]);
				p4_usbd_write8_reg( ( P4_USBD_FIFOCTRL_AUTO | epNum), P4_USBD_FIFOCTRL);
				p4_usbd_reset_data_toggle(DIR_OUT, epNum);
				p4_usbd_do_fifo_reset(DIR_OUT, epNum);
#if P4_USBD_DMA_SUPPORT
				if( !p4_usbd_dma[1 + P4_USBD_MAX_IN_EP + epNum].isDma ) {
					val = p4_usbd_read32_reg(P4_USBD_DEVXEPXINTEN(devNum));
					p4_usbd_write32_reg( (val | (1 << (16+(epNum - 1))) ), P4_USBD_DEVXEPXINTEN(devNum));
				} else {
					val = p4_usbd_read32_reg(P4_USBD_DEVXDMACHXINTEN(devNum));
					p4_usbd_write32_reg(( val | ( 1 << (16+(epNum - 1)) )), P4_USBD_DEVXDMACHXINTEN(devNum));
					// Clear status
					p4_usbd_write32_reg(( val | ( 1 << (16+(epNum - 1)) )), P4_USBD_DEVXDMACHXINTSTS(devNum));
				}
#else
				val = p4_usbd_read32_reg(P4_USBD_DEVXEPXINTEN(devNum));
				p4_usbd_write32_reg( (val | (1 << (16+(epNum - 1))) ), P4_USBD_DEVXEPXINTEN(devNum));
#endif
			}
			break;
		case USB_ENDPOINT_XFER_BULK:
			if(epDir == DIR_IN) {
				p4_usbd_write8_reg(0, P4_USBD_INxCON[epNum]);
				p4_usbd_write8_reg( ( P4_USBD_EP_EP_VALID | (USB_ENDPOINT_XFER_BULK << 2) | P4_USBD_EP_SINGLE_BUFFER), P4_USBD_INxCON[epNum]);
				p4_usbd_write8_reg( ( P4_USBD_FIFOCTRL_DIR | P4_USBD_FIFOCTRL_AUTO | epNum), P4_USBD_FIFOCTRL);
				p4_usbd_reset_data_toggle(DIR_IN, epNum);
				p4_usbd_do_fifo_reset(DIR_IN, epNum);
#if P4_USBD_DMA_SUPPORT
				if( !p4_usbd_dma[1 + epNum].isDma ) {
					val = p4_usbd_read32_reg(P4_USBD_DEVXEPXINTEN(devNum));
					p4_usbd_write32_reg( (val | (1 << (epNum - 1))), P4_USBD_DEVXEPXINTEN(devNum));
				} else {
					val = p4_usbd_read32_reg(P4_USBD_DEVXDMACHXINTEN(devNum));
					p4_usbd_write32_reg(( val | ( 1 << (epNum-1) )), P4_USBD_DEVXDMACHXINTEN(devNum));
					// Clear status
					p4_usbd_write32_reg(( val | ( 1 << (epNum-1) )), P4_USBD_DEVXDMACHXINTSTS(devNum));
				}
#else
				val = p4_usbd_read32_reg(P4_USBD_DEVXEPXINTEN(devNum));
				p4_usbd_write32_reg( (val | (1 << (epNum - 1))), P4_USBD_DEVXEPXINTEN(devNum));
#endif
			} else {
				p4_usbd_write8_reg(0, P4_USBD_OUTxCON[epNum]);
				p4_usbd_write8_reg( ( P4_USBD_EP_EP_VALID | (USB_ENDPOINT_XFER_BULK << 2) | P4_USBD_EP_SINGLE_BUFFER), P4_USBD_OUTxCON[epNum]);
				p4_usbd_write8_reg( ( P4_USBD_FIFOCTRL_AUTO | epNum), P4_USBD_FIFOCTRL);
				p4_usbd_reset_data_toggle(DIR_OUT, epNum);
				p4_usbd_do_fifo_reset(DIR_OUT, epNum);
#if P4_USBD_DMA_SUPPORT
				if( !p4_usbd_dma[1 + P4_USBD_MAX_IN_EP + epNum].isDma ) {
					val = p4_usbd_read32_reg(P4_USBD_DEVXEPXINTEN(devNum));
					p4_usbd_write32_reg( (val | (1 << (16+(epNum - 1))) ), P4_USBD_DEVXEPXINTEN(devNum));
				} else {
					val = p4_usbd_read32_reg(P4_USBD_DEVXDMACHXINTEN(devNum));
					p4_usbd_write32_reg(( val | ( 1 << (16+(epNum - 1)) )), P4_USBD_DEVXDMACHXINTEN(devNum));
					// Clear status
					p4_usbd_write32_reg(( val | ( 1 << (16+(epNum - 1)) )), P4_USBD_DEVXDMACHXINTSTS(devNum));
				}
#else
				val = p4_usbd_read32_reg(P4_USBD_DEVXEPXINTEN(devNum));
				p4_usbd_write32_reg( (val | (1 << (16+(epNum - 1))) ), P4_USBD_DEVXEPXINTEN(devNum));
#endif
			}
			break;
		default:
			printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
			return -1;

	}
	return 0;
}

static int p4_usbd_hw_disable_ep (uint8_t DeviceNo,uint8_t Ep, uint8_t EpDir, uint8_t EpType)
{
	return 0;
}

static int p4_usbd_hw_get_ep_status (uint8_t devNum, uint8_t epNum, uint8_t epDir, uint8_t *enable, uint8_t *stall)
{
	if( epNum == 0 ) {
		*stall = ( p4_usbd_read8_reg(P4_USBD_EP0CS) & P4_USBD_EP0CS_STALL )?1:0;
	} else {
		if(epDir) {
			*stall = ( p4_usbd_read8_reg(P4_USBD_INxCON[epNum]) & P4_USBD_BIT6_SET )?1:0;
		} else {
			*stall = ( p4_usbd_read8_reg(P4_USBD_OUTxCON[epNum]) & P4_USBD_BIT6_SET )?1:0;
		}
	}
	return 0;
}

static int p4_usbd_hw_stall_ep (uint8_t devNum, uint8_t epNum, uint8_t epDir)
{
	//printk("stall %d,%d,%d\n", devNum, epNum, epDir);
	if ( epNum == 0 ) {
		p4_usbd_write8_reg((p4_usbd_read8_reg(P4_USBD_EP0CS) & ~P4_USBD_EP0CS_HSNAK) | P4_USBD_EP0CS_STALL, P4_USBD_EP0CS);
	} else {
		if( epDir ) {
			p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_INxCON[epNum]) | P4_USBD_BIT6_SET, P4_USBD_INxCON[epNum]);
		} else {
			p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_OUTxCON[epNum]) | P4_USBD_BIT6_SET, P4_USBD_OUTxCON[epNum]);
		}
	}

    /* call halt handler of device module via core module */
    usb_core_module.CoreUsbHaltHandler(devNum, epNum, epDir);
	return 0;
}

static int p4_usbd_hw_unstall_ep (uint8_t devNum, uint8_t epNum, uint8_t epDir)
{
	usb_core_module.CoreUsbUnHaltHandler(devNum, epNum,epDir);
	if (epNum == 0) {
		p4_usbd_reset_data_toggle( DIR_IN, epNum);
		p4_usbd_reset_data_toggle( DIR_OUT, epNum);
	} else {
		if(epDir) {
			p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_INxCON[epNum]) & ~P4_USBD_BIT6_SET, P4_USBD_INxCON[epNum]);
		} else {
			p4_usbd_write8_reg( p4_usbd_read8_reg(P4_USBD_OUTxCON[epNum]) & ~P4_USBD_BIT6_SET, P4_USBD_OUTxCON[epNum]);
		}
		p4_usbd_reset_data_toggle(epDir, epNum);
	}

	return 0;
}

static void p4_usbd_hw_set_remote_wakeup (uint8_t devNum, uint8_t enable)
{
	return;
}

static uint8_t p4_usbd_hw_get_remote_wakeup (uint8_t DeviceNo)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	return 0;
}

static void p4_usbd_hw_complete_request (uint8_t devNum, uint8_t status, DEVICE_REQUEST *req)
{
	if ( status == 1 )
	{
		p4_usbd_hw_stall_ep(devNum, 0, DIR_IN);
	}

	if( (req->bmRequestType & USB_DIR_IN) == USB_DIR_IN ) {
		p4_usbd_write8_reg( p4_usbd_read8_reg (P4_USBD_DEVXEP0CTRL(devNum)) | P4_USBD_BIT1_SET, P4_USBD_DEVXEP0CTRL(devNum));
	} else {
		if(req->wLength <= 0) {
			p4_usbd_write8_reg( p4_usbd_read8_reg (P4_USBD_DEVXEP0CTRL(devNum)) | P4_USBD_BIT1_SET, P4_USBD_DEVXEP0CTRL(devNum));
		} else {
			p4_usbd_write8_reg(0x0, P4_USBD_DEVXOUT0BC(devNum));
		}
	}

	return;
}

static void p4_usbd_hw_device_disconnect (uint8_t devNo)
{

	if( p4_usbd_device_hwEnabled[devNo] ) { 
		p4_usbd_hw_disable_dev(devNo,0);
	}
	
	if( !p4_usbd_device_connect_to_port[devNo] ) { return; }
	p4_usbd_hub_cfg_port_status[devNo].wPortStatus &= ~USB_PORT_STAT_CONNECTION;
	p4_usbd_hub_cfg_port_status[devNo].wPortStatus &= ~USB_PORT_STAT_ENABLE;
	p4_usbd_hub_cfg_port_status[devNo].wPortStatus &= ~USB_PORT_STAT_SUSPEND;
	p4_usbd_hub_cfg_port_status[devNo].wPortStatus &= ~USB_PORT_STAT_HIGH_SPEED;
	p4_usbd_hub_cfg_port_status[devNo].wPortChange |= USB_PORT_STAT_C_CONNECTION;

	p4_usbd_pc_queue_worker(devNo, &p4_usbd_hub_send_port_change);
	p4_usbd_write8_reg(0, P4_USBD_DEVXDISCONDB(devNo));
	p4_usbd_device_connect_to_port[devNo] = 0;
}

static void p4_usbd_hw_device_reconnect (uint8_t devNo)
{
	if( !p4_usbd_device_hwEnabled[devNo] ) { 
		p4_usbd_hw_enable_dev_fun(&devNo);
	}
	if( p4_usbd_device_connect_to_port[devNo] ) { return; }

	remote_wakeup_check();

	/* change port status */
	p4_usbd_hub_cfg_port_status[devNo].wPortStatus &= ~USB_PORT_STAT_SUSPEND;
	p4_usbd_hub_cfg_port_status[devNo].wPortStatus |= USB_PORT_STAT_CONNECTION;
	/* Pilot 4 running in high speed only. */
	p4_usbd_hub_cfg_port_status[devNo].wPortStatus |= USB_PORT_STAT_HIGH_SPEED;
	p4_usbd_hub_cfg_port_status[devNo].wPortChange |= USB_PORT_STAT_C_CONNECTION;

	p4_usbd_pc_queue_worker(devNo, &p4_usbd_hub_send_port_change);
	p4_usbd_write8_reg(0, P4_USBD_DEVXDISCONDB(devNo));
	p4_usbd_device_connect_to_port[devNo] = 1;
}

static void p4_usbd_hw_remote_wakeup (uint8_t devNo)
{
	remote_wakeup_check();

	/* Below two line is used for windows 2012, because windows 2012 will not send
	   USB_PORT_FEAT_SUSPEND of CLEAR_FEATURE(23 01 02 00 01 00 00 00)command after suspend a port,
	   but it will send such command when a hub suspend and resume.
	   */
	p4_usbd_hub_cfg_port_status[devNo].wPortStatus &= ~USB_PORT_STAT_SUSPEND;
	p4_usbd_resume_port(devNo);

	p4_usbd_hub_cfg_port_status[devNo].wPortChange |= USB_PORT_STAT_C_SUSPEND;
	p4_usbd_pc_queue_worker(devNo, &p4_usbd_hub_send_port_change);
}

static int p4_usbd_hw_set_current_dev_type(int devno, int devtype)
{
	if(devno >= P4_USBD_USE_DEV_NUM)
	{
		printk(KERN_WARNING"Error setting USB DevType for USB Devno %d\n",devno);
		return -1;
	}
	p4_USB_Type_Data[devno] = devtype;
	
	return 0;
}

static int p4_usbd_hw_get_devicetype_ep (int devno, uint8_t* ep_config, uint8_t* num_eps)
{
	int num = 0, USB_DevType = 0;

	if(devno >= P4_USBD_USE_DEV_NUM)
	{
		printk(KERN_WARNING"Error getting USB EP data  for USB Devno %d\n\n",devno);
		return -1;
	}
	USB_DevType = p4_USB_Type_Data[devno];
	for (num = 0; num < P4_USBD_USE_DEV_NUM; num++)
	{
		if(USB_DevType == p4_ep_devnum_seq[num])
		break;
	}

	if(num == P4_USBD_USE_DEV_NUM)
	{	
		printk(KERN_WARNING"Error getting USB EP data  for USB Devno %d\n",devno);
		return -1;
	}

	memcpy(ep_config, (uint8_t*)p4_usbd_ep_configs[num], (sizeof(ep_config_t)*P4_USBD_DEV_EP_NUM));
	*num_eps = P4_USBD_DEV_EP_NUM;
	
	return 0;
}


static int p4_usbd_hw_get_hub_ports_status (uint8_t DevNo, uint8_t PortNum, uint32_t* Status)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	return 0;
}

static void p4_usbd_hw_set_hub_ports_status (uint8_t DevNo, uint8_t PortNum, uint32_t Mask, int SetClear)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
}

static void p4_usbd_hw_hub_enable_device (uint8_t DevNo, uint8_t IfNum)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
}

static void p4_usbd_hw_hub_disable_device (uint8_t DevNo, uint8_t IfNum)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
}

static void p4_usbd_hw_clear_hub_device_enable_flag (uint8_t DevNo, uint8_t IfNum)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
}

static void p4_usbd_hw_set_hub_device_enable_flag (uint8_t DevNo, uint8_t IfNum)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
}

static void p4_usbd_hw_hub_port_bitmap (uint8_t DevNo, uint8_t IfNum)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
}

static void p4_usbd_hw_change_hub_regs (uint8_t DevNo, uint8_t bRequest, uint8_t wValue, uint8_t wIndex)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
}

static int p4_usbd_hw_get_hub_speed (uint8_t DevNo)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	return 0;
}

static int p4_usbd_hw_is_hub_device_connected (uint8_t DevNo, uint8_t ifnum)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	return 0;
}

static int p4_usbd_hw_is_hub_device_enabled (uint8_t DevNo, uint8_t ifnum)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	return 0;
}

#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
static int p4_usbd_hw_upstream_disable (uint8_t DevNo)
{
	p4_usbd_write8_reg((p4_usbd_read8_reg(P4_USBD_USBCS) | (u8)(P4_USBD_BIT6_SET)), P4_USBD_USBCS);
	p4_usbd_hub_connect_to_upstream = 0;
	return 0;
}

static int p4_usbd_hw_upstream_enable (uint8_t DevNo)
{
	p4_usbd_write8_reg((p4_usbd_read8_reg(P4_USBD_USBCS) & (u8)~(P4_USBD_BIT6_SET)), P4_USBD_USBCS);	// Connect to Host
	p4_usbd_hub_connect_to_upstream = 1;
	return 0;
}
#endif

static int p4_usbd_hw_upstream_media_disable (uint8_t DevNo)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	return -1;
}

static int p4_usbd_hw_upstream_media_enable (uint8_t DevNo)
{
	printk(KERN_WARNING "P4_USBD(%d): Unimplemented case in %s\n", __LINE__, __FUNCTION__);
	return -1;
}

/* HUB Hardware functions structure */
USB_HW p4_usbd_hw = {
		.UsbHwEnable					= p4_usbd_hw_enable_dev,
		.UsbHwDisable					= p4_usbd_hw_disable_dev,
		.UsbHwIntr						= p4_usbd_hw_intr,								/* UNIMPLEMENTED */
		.UsbHwSetAddr					= p4_usbd_hw_set_addr,
		.UsbHwGetAddr					= p4_usbd_hw_get_addr,							/* UNIMPLEMENTED */
		.UsbHwWrite						= p4_usbd_hw_write_to_host,						/* Used by USB Core layer only. */
		.UsbTxComplete					= p4_usbd_hw_tx_complete,
		.UsbHwRead						= p4_usbd_hw_read_from_host, 					/* Used by hardware driver itself only. */
		.UsbHwAllocBuffer				= p4_usbd_hw_alloc_buffer,
		.UsbHwEnableEp					= p4_usbd_hw_enable_ep,
		.UsbHwDisableEp					= p4_usbd_hw_disable_ep,						/* UNIMPLEMENTED */
		.UsbHwGetEpStatus				= p4_usbd_hw_get_ep_status,
		.UsbHwStallEp					= p4_usbd_hw_stall_ep,
		.UsbHwUnstallEp					= p4_usbd_hw_unstall_ep,
		.UsbHwSetRemoteWakeup			= p4_usbd_hw_set_remote_wakeup,					/* UNIMPLEMENTED */
		.UsbHwGetRemoteWakeup			= p4_usbd_hw_get_remote_wakeup,					/* UNIMPLEMENTED */
		.UsbHwCompleteRequest			= p4_usbd_hw_complete_request,
		.UsbHwDeviceDisconnect			= p4_usbd_hw_device_disconnect,
		.UsbHwDeviceReconnect			= p4_usbd_hw_device_reconnect,
		.UsbHwRemoteWakeup				= p4_usbd_hw_remote_wakeup,
		.UsbHwGetHubPortsStatus			= p4_usbd_hw_get_hub_ports_status,				/* UNIMPLEMENTED */
		.UsbHwSetHubPortsStatus			= p4_usbd_hw_set_hub_ports_status,				/* UNIMPLEMENTED */
		.UsbHwHubEnableDevice			= p4_usbd_hw_hub_enable_device,					/* UNIMPLEMENTED */
		.UsbHwHubDisableDevice			= p4_usbd_hw_hub_disable_device,				/* UNIMPLEMENTED */
		.UsbHwClearHubDeviceEnableFlag	= p4_usbd_hw_clear_hub_device_enable_flag,		/* UNIMPLEMENTED */
		.UsbHwSetHubDeviceEnableFlag	= p4_usbd_hw_set_hub_device_enable_flag,		/* UNIMPLEMENTED */
		.UsbHwHubPortBitmap				= p4_usbd_hw_hub_port_bitmap,					/* UNIMPLEMENTED */
		.UsbHwChangeHubRegs				= p4_usbd_hw_change_hub_regs,					/* UNIMPLEMENTED */
		.UsbHwGetHubSpeed				= p4_usbd_hw_get_hub_speed,						/* UNIMPLEMENTED */
		.UsbHwIsHubDeviceConnected		= p4_usbd_hw_is_hub_device_connected,			/* UNIMPLEMENTED */
		.UsbHwIsHubDeviceEnabled		= p4_usbd_hw_is_hub_device_enabled,				/* UNIMPLEMENTED */
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
		.UsbHwUpstreamDisable			= p4_usbd_hw_upstream_disable,
		.UsbHwUpstreamEnable			= p4_usbd_hw_upstream_enable,
#endif
		.UsbHwUpstreamMediaDisable		= p4_usbd_hw_upstream_media_disable,			/* UNIMPLEMENTED */
		.UsbHwUpstreamMediaEnable		= p4_usbd_hw_upstream_media_enable,				/* UNIMPLEMENTED */
		/* Usable Endpoints, excluding Control 0 endpoints */
		.NumEndpoints					= (P4_USBD_DEV_EP_NUM * P4_USBD_USE_DEV_NUM),
		.BigEndian						= 0,/* Little Endian */
		.WriteFifoLock					= 0, /* write FIFO lock is not needed */
		.EP0Size						= P4_USBD_EP0_BUF_SIZE,
		.SupportedSpeeds				= SUPPORT_HIGH_SPEED,
#if 0
		.EpMultipleDMASupport;
		.EpMultipleDMASize;
		.IsThisHUBDevice;
		.NumHUBPorts;
#endif
		.UsbHwGetdevicetypeEps					= p4_usbd_hw_get_devicetype_ep,	
		.UsbHwSetCurrentDevType					= p4_usbd_hw_set_current_dev_type,
};

int p4_usbd_driver_init(uint8_t dev_num, USB_HW *usb_hw_module, void **dev_config)
{
	uint8_t i;
	for (i = 0; i < P4_USBD_USE_DEV_NUM; i ++) {
		if ( p4_usbd_hub_cfg_dev_num[i] == 0xFF) {
			break;
		}
	}

	if (i == P4_USBD_USE_DEV_NUM)
		return -1;

	p4_usbd_hub_cfg_dev_num[i] = dev_num;
	memcpy(usb_hw_module, &p4_usbd_hw, sizeof(USB_HW));
	*dev_config = &(p4_usbd_hub_cfg_dev_num[i]);

	return 0;
}

void p4_usbd_driver_exit(uint8_t dev_num)
{
	if (dev_num == -1)
		return;
	p4_usbd_hub_cfg_dev_num[dev_num] = 0xFF;
}

static uint8_t p4_usbd_state_init(uint8_t isHWModuleLoading)
{
	uint8_t i = 0;
	uint32_t p4_usbd_sedid=0, p4_usbd_p2connect=0;
	p4_usbd_hub_suspended = 0;
	p4_usbd_hub_cfg_remote_wakeup = 0x0;
	p4_usbd_hub_cfg_addr = 0xFF;
	p4_usbd_hub_cfg_status = (1 << USB_DEVICE_SELF_POWERED); //set init status as self powered device
	p4_usbd_hub_cfg_current_config_value = 0x0;
	p4_usbd_hub_cfg_class_status.wHubStatus = 0x0;
	p4_usbd_hub_cfg_class_status.wHubChange = 0x0;
	if(!isHWModuleLoading){
		// Disable All Port
		p4_usbd_write32_reg(0x0, P4_USBD_PORTENABLE);
	}

	if(isHWModuleLoading) {
		p4_usbd_hub_cfg_reg_va = (uint8_t*)ioremap(P4_USBD_REG_BASE, SZ_1M);
		if( p4_usbd_hub_cfg_reg_va == NULL) {
			printk(KERN_ERR "P4_USBD(%d): Could not map the USB 2 registers.\n", __LINE__);
			return 1;
		}

		/* See SDK 20160831, line 2998 of file pilot4_hub_multi_dev.c */
		// A2
		p4_usbd_sedid = ((int)IO_ADDRESS(SE_SYS_CLK_BASE+0x50)) & 0xFF;
		if( p4_usbd_sedid == 0x42 ) {
			p4_usbd_p2connect= (int)IO_ADDRESS(SE_FGE_BASE+0x74);
			if(!(p4_usbd_p2connect & (1 << 13))) {
				printk(KERN_ERR "P4_USBD(%d): USB device is disabled by H/W\n.", __LINE__);
				return 1;
			}
		}
		if ( (p4_usbd_read32_reg(P4_USBD_SSPDEVLOCK) & 0x1) == 0x1 ) {
			printk(KERN_ERR "P4_USBD(%d): USB is claimed by SSP\n.", __LINE__);
			return 1;
		}
	}

	for( i = 0 ; i < P4_USBD_MAX_DEV_NUM ; i++ ) {
		if(isHWModuleLoading) {
			p4_usbd_hub_cfg_dev_num[i] = 0xFF;
		}
		p4_usbd_hub_cfg_port_status[i].wPortStatus=0;
		p4_usbd_hub_cfg_port_status[i].wPortChange=0;
	}

	return 0;
}

static int __init p4_usbd_module_init(void)
{
	uint8_t retval = 0;
	uint8_t i = 0;

#if P4_USBD_DMA_SUPPORT
	printk(KERN_INFO"Initializing P4 USB Device Module with DMA Enabled.\n");
#else
	printk(KERN_INFO"Initializing P4 USB Device Module.\n");
#endif
	retval = p4_usbd_state_init(1);

	if( retval ) {
		printk(KERN_ERR "P4_USBD(%d): Initializing P4 USB Device Module Fail. \n", __LINE__);
		return -1;
	}

	if (get_usb_core_funcs(&usb_core_module) != 0) {
		printk(KERN_ERR "P4_USBD(%d): %s: get core module functions failed\n", __LINE__, p4_usbd_driver_name);
		return -1;
	}

	for( i = 0 ; i < P4_USBD_MAX_DEV_NUM ; i++ ) {
		sprintf(p4_usbd_ctr_driver[i].name, "usbd-%d", i + 1);
		p4_usbd_ctr_driver[i].module = THIS_MODULE;
		p4_usbd_ctr_driver[i].usb_driver_init = p4_usbd_driver_init;
		p4_usbd_ctr_driver[i].usb_driver_exit = p4_usbd_driver_exit;
		p4_usbd_ctr_driver[i].devnum = 0xFF;
	}

	for(i = 0 ; i < P4_USBD_USE_DEV_NUM ; i++) {
		retval = register_usb_chip_driver_module(&p4_usbd_ctr_driver[i]);
	}

	retval = platform_device_register (&p4_usbd_pdev);
	if(retval < 0)
		printk(KERN_ERR "P4_USBD(%d): platform_device_register failed for USB, return value: %d\n", __LINE__, retval);

	return platform_driver_probe(&p4_usbd_driver, p4_usbd_probe);
}

static void __exit p4_usbd_module_exit(void)
{
	uint8_t i=0;
	printk(KERN_INFO"Unloading P4 USB Device Module.\n");
	platform_driver_unregister(&p4_usbd_driver);
	for(i=0;i<P4_USBD_USE_DEV_NUM;i++) {
		unregister_usb_chip_driver_module(&p4_usbd_ctr_driver[i]);
	}
	flush_workqueue(p4_usbd_pc_wq);
	destroy_workqueue(p4_usbd_pc_wq);
	flush_workqueue(p4_usbd_dv_wq);
	destroy_workqueue(p4_usbd_dv_wq);
	iounmap(p4_usbd_hub_cfg_reg_va);
	uninstall_swc_handler(p4_usbd_SWC_interrupt_cb, POWER_GOOD_INTR);
	//free dma buf
	//clear and disconnect
}

module_init(p4_usbd_module_init);
module_exit(p4_usbd_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("Pilot4 Virtual USB Device Controller driver");
MODULE_LICENSE("GPL");

