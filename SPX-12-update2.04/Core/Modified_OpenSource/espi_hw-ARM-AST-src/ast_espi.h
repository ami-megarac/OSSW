/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef _AST_ESPI_H_
#define _AST_ESPI_H_

#define AST_ESPI_DRIVER_NAME    "ast_espi"


#define MAX_XFER_BUFF_SIZE      0xFFF        //4096

#define AST_SCU_REG_BASE        0x1E6E2000

#define AST_GPIO_REG_BASE       0x1E780000

#define AST_ESPI_REG_BASE       0x1E6EE000
#define AST_ESPI_IRQ            23

#define AST_ESPI_CHANNEL_NUM    2  /* Peripheral Channel and Virtual Wire Channel */

#define AST_ESPI_PERI_CHAN_ID       0
#define AST_ESPI_VW_CHAN_ID         1
#define AST_ESPI_OOB_CHAN_ID        2
#define AST_ESPI_FLASH_CHAN_ID      3


// SCU register
#define AST_SCU_RESET           0x04        /*  system reset control register */
#define SCU_RESET_ESPI          (0x1 << 5)

// GPIO register
#define GPIO_AC_INT_EN              0x1A8
#define GPIO_AC_INT_SENS_0          0x1AC
#define GPIO_AC_INT_SENS_1          0x1B0
#define GPIO_AC_INT_SENS_2          0x1B4
#define GPIO_AC_INT_STATUS          0x1B8
#define GPIO_AC_DIRECTION           0x1EC
#define GPIO_AC_7                   7

// register
#define AST_ESPI_CTRL               0x00
#define AST_ESPI_STATUS             0x04
#define AST_ESPI_INTR_STATUS        0x08
#define AST_ESPI_INTR_EN            0x0C
#define AST_ESPI_PCP_RX_DMA         0x10
#define AST_ESPI_PCP_RX_CTRL        0x14
#define AST_ESPI_PCP_RX_DATA        0x18
#define AST_ESPI_PCP_TX_DMA         0x20
#define AST_ESPI_PCP_TX_CTRL        0x24
#define AST_ESPI_PCP_TX_DATA        0x28
#define AST_ESPI_PCNP_TX_DMA        0x30
#define AST_ESPI_PCNP_TX_CTRL       0x34
#define AST_ESPI_PCNP_TX_DATA       0x38
#define AST_ESPI_OOB_RX_DMA         0x40
#define AST_ESPI_OOB_RX_CTRL        0x44
#define AST_ESPI_OOB_RX_DATA        0x48
#define AST_ESPI_OOB_TX_DMA         0x50
#define AST_ESPI_OOB_TX_CTRL        0x54
#define AST_ESPI_OOB_TX_DATA        0x58
#define AST_ESPI_FLASH_RX_DMA       0x60
#define AST_ESPI_FLASH_RX_CTRL      0x64
#define AST_ESPI_FLASH_RX_DATA      0x68
#define AST_ESPI_FLASH_TX_DMA       0x70
#define AST_ESPI_FLASH_TX_CTRL      0x74
#define AST_ESPI_FLASH_TX_DATA      0x78
#define AST_ESPI_SYS_EVENT_EN       0x94
#define AST_ESPI_SYS_EVENT          0x98
#define AST_ESPI_GPIO_THROUGH_VW    0x9C

// Slave Registers
#define AST_ESPI_GEN_CAPCONF        0xA0
#define AST_ESPI_CH0_CAPCONF        0xA4
#define AST_ESPI_CH1_CAPCONF        0xA8
#define AST_ESPI_CH2_CAPCONF        0xAC
#define AST_ESPI_CH3_CAPCONF        0xB0

#define AST_ESPI_SYS1_EVENT_EN      0x100
#define AST_ESPI_SYS1_EVENT         0x104
#define AST_ESPI_SYS_INT_T0         0x110
#define AST_ESPI_SYS_INT_T1         0x114
#define AST_ESPI_SYS_INT_T2         0x118
#define AST_ESPI_SYS_EVENT_STATUS   0x11C
#define AST_ESPI_SYS1_INT_T0        0x120
#define AST_ESPI_SYS1_INT_T1        0x124
#define AST_ESPI_SYS1_INT_T2        0x128
#define AST_ESPI_SYS1_EVENT_STATUS  0x12C

// register bit value
/* AST_ESPI_CTRL    -   0x00    :Engine Control */
#define ESPI_CTRL_FLASH_TX_SW_RESET     (0x1 << 31)
#define ESPI_CTRL_FLASH_RX_SW_RESET     (0x1 << 30)
#define ESPI_CTRL_OOB_TX_SW_RESET       (0x1 << 29)
#define ESPI_CTRL_OOB_RX_SW_RESET       (0x1 << 28)
#define ESPI_CTRL_PCNP_TX_SW_RESET      (0x1 << 27)
#define ESPI_CTRL_PCNP_RX_SW_RESET      (0x1 << 26)
#define ESPI_CTRL_PCP_TX_SW_RESET       (0x1 << 25)
#define ESPI_CTRL_PCP_RX_SW_RESET       (0x1 << 24)
#define ESPI_CTRL_FLASH_TX_DMA          (0x1 << 23)
#define ESPI_CTRL_FLASH_RX_DMA          (0x1 << 22)
#define ESPI_CTRL_OOB_TX_DMA            (0x1 << 21)
#define ESPI_CTRL_OOB_RX_DMA            (0x1 << 20)
#define ESPI_CTRL_PCNP_TX_DMA           (0x1 << 19)
/* */
#define ESPI_CTRL_PCP_TX_DMA            (0x1 << 17)
#define ESPI_CTRL_PCP_RX_DMA            (0x1 << 16)
/* */
#define ESPI_CTRL_DIR_RESET             (0x1 << 13)
#define ESPI_CTRL_VAL_RESET             (0x1 << 12)
#define ESPI_CTRL_SW_FLASH_READ         (0x1 << 10)
#define ESPI_CTRL_SW_GPIO_VIRTCH        (0x1 << 9)

// AST_ESPI_INTR_STATUS
#define HW_RESET_EVENT              (0x1 << 31)
#define VW_SYS_EVENT1               (0x1 << 22)
#define FLASH_TX_ERROR              (0x1 << 21)
#define OOB_TX_ERROR                (0x1 << 20)
#define FLASH_TX_ABORT              (0x1 << 19)
#define OOB_TX_ABORT                (0x1 << 18)
#define PCNP_TX_ABORT               (0x1 << 17)
#define PCP_TX_ABORT                (0x1 << 16)
#define FLASH_RX_ABORT              (0x1 << 15)
#define OOB_RX_ABORT                (0x1 << 14)
#define PCNP_RX_ABORT               (0x1 << 13)
#define PCP_RX_ABORT                (0x1 << 12)
#define PCNP_TX_ERROR               (0x1 << 11)
#define PCP_TX_ERROR                (0x1 << 10)
#define VW_GPIO_EVENT               (0x1 << 9)
#define VW_SYS_EVENT                (0x1 << 8)
#define FLASH_TX_COMPLETE           (0x1 << 7)
#define FLASH_RX_COMPLETE           (0x1 << 6)
#define OOB_TX_COMPLETE             (0x1 << 5)
#define OOB_RX_COMPLETE             (0x1 << 4)
#define PCNP_TX_COMPLETE            (0x1 << 3)
#define PCP_TX_COMPLETE             (0x1 << 1)
#define PCP_RX_COMPLETE             (0x1)

// AST_ESPI_PCP_RX_CTRL
#define TRIGGER_TRANSACTION         (0x1 << 31)
#define GET_CYCLE_TYPE(x)           (x & 0xff)
#define GET_TAG(x)                  ((x >> 8) & 0xf)
#define GET_LEN(x)                  ((x >> 12) & 0xfff)

// AST_ESPI_SYS1_EVENT
// AST_ESPI_SYS1_EVENT_STATUS
#define SYS1_SUS_ACK                (0x1 << 20)
#define SYS1_SUS_PWDOWN_ACK         (0x1 << 1)
#define SYS1_SUS_WARN               (0x1)

// AST_ESPI_SYS_EVENT
// AST_ESPI_SYS_EVENT_STATUS
#define SYS_HOST_RESET_ACK          (0x1 << 27)
#define SYS_SLAVE_BOOT_STATUS       (0x1 << 23)
#define SYS_SLAVE_BOOT_DONE         (0x1 << 20)
#define SYS_OOB_RESET_ACK           (0x1 << 16)
#define SYS_HOST_RESET_WARN         (0x1 << 8)
#define SYS_OOB_RESET_WARN          (0x1 << 6)


struct espi_ch_data {
    int     full;
    u32     header;        
    u32     buf_len;    
    u8      *buff;
};

#endif /* _AST_ESPI_H_ */
