/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/*****************************************************************
 *
 * platform_neoncity.h
 * platform driver related 
 *
 * Author: Suresh V <sureshv@amiindia.co.in>
 *
 *****************************************************************/
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/*Header File which includes GPIO Pin Multiplexing #define */

#define GPIO_MUX_CTRL 1

#define GPIO_PORT_2 2
#define GPIO_PORT_3 3
#define GPIO_PORT_14 14
#define GPIO_PORT_16 16
#define GPIO_PORT_17 17
#define GPIO_PORT_18 18
#define GPIO_PORT_19 19
#define GPIO_PORT_20 20
#define GPIO_PORT_21 21
#define GPIO_PORT_22 22
#define GPIO_PORT_23 23
#define GPIO_PORT_24 24
#define GPIO_PORT_25 25
#define GPIO_PORT_26 26
#define GPIO_PORT_27 27
#define GPIO_PORT_28 28
#define GPIO_PORT_29 29

#define PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL     0x04
#define PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL      0x18
#define PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_DATA  0x0C
#define PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE  0x00
#define PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG   0x00
#define PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE  0x0C
#define PILOT_TOP_LEVEL_PIN_CTRL_GPIO1_ENABLE  0x08

#define PILOT_TOP_LEVEL_PIN_CTRL_MOUSE_CLK   0x00200000 /* Bit 21 */
#define PILOT_TOP_LEVEL_PIN_CTRL_MOUSE_DATA  0x00100000 /* Bit 20 */
#define PILOT_TOP_LEVEL_PIN_CTRL_KEYBD_CLK   0x00080000 /* Bit 19 */
#define PILOT_TOP_LEVEL_PIN_CTRL_KEYBD_DATA  0x00040000 /* Bit 18 */


#define PILOT_GPIO_2_ENABLE_OUTPUT_DIRECTION    0x02
#define PILOT_GPIO_2_ENABLE_DATA_LOW    		0x08
#define PILOT_GPIO_15_ENABLE_OUTPUT_DIRECTION	0x17
#define PILOT_GPIO_15_ENABLE_DATA_LOW    		0x18
#define PILOT_GPIO_2_LED_BLINK_RATE		0x0C
#define PILOT_GPIO_15_LED_BLINK_RATE             0x1D


#define GPIO_ENABLE_0                  0x00000001 /* Bit 0 */
#define GPIO_ENABLE_1                  0x00000002 /* Bit 1 */
#define GPIO_ENABLE_2                  0x00000004 /* Bit 2 */
#define GPIO_ENABLE_3                  0x00000008 /* Bit 3 */
#define GPIO_ENABLE_4                  0x00000010 /* Bit 4 */
#define GPIO_ENABLE_5                  0x00000020 /* Bit 5 */
#define GPIO_ENABLE_6                  0x00000040 /* Bit 6 */
#define GPIO_ENABLE_7                  0x00000080 /* Bit 7 */
#define GPIO_ENABLE_8                  0x00000100 /* Bit 8 */
#define GPIO_ENABLE_9                  0x00000200 /* Bit 9 */
#define GPIO_ENABLE_10                 0x00000400 /* Bit 10 */
#define GPIO_ENABLE_11                 0x00000800 /* Bit 11 */
#define GPIO_ENABLE_12                 0x00001000 /* Bit 11 */
#define GPIO_ENABLE_13                 0x00002000 /* Bit 11 */
#define GPIO_ENABLE_14                 0x00004000 /* Bit 11 */
#define GPIO_ENABLE_15                 0x00008000 /* Bit 11 */
#define GPIO_ENABLE_16                 0x00010000 /* Bit 16 */
#define GPIO_ENABLE_17                 0x00020000 /* Bit 17 */
#define GPIO_ENABLE_18                 0x00040000 /* Bit 18 */
#define GPIO_ENABLE_19                 0x00080000 /* Bit 19 */
#define GPIO_ENABLE_20                 0x00100000 /* Bit 20 */
#define GPIO_ENABLE_21                 0x00200000 /* Bit 21 */
#define GPIO_ENABLE_22                 0x00400000 /* Bit 22 */
#define GPIO_ENABLE_23                 0x00800000 /* Bit 23 */
#define GPIO_ENABLE_24                 0x01000000 /* Bit 24 */
#define GPIO_ENABLE_25                 0x02000000 /* Bit 25 */
#define GPIO_ENABLE_26                 0x04000000 /* Bit 26 */
#define GPIO_ENABLE_27                 0x08000000 /* Bit 27 */
#define GPIO_ENABLE_28                 0x10000000 /* Bit 28 */
#define GPIO_ENABLE_29                 0x20000000 /* Bit 29 */
#define GPIO_ENABLE_30                 0x40000000 /* Bit 30 */
#define GPIO_ENABLE_31                 0x80000000 /* Bit 31 */

#define STRAP_OPTIONS                        0x001FFFFF /* Bit 0 to 20 is the Strap Options */
#define ENABLE_OVERRIDE_STRAP_OPTION         0x80000000 /* Software Strap Enable 31 Bit */
#define RGMII_1_MODE_STRAP                   0x00100000 /* Bit 20 for RGMII 1 STRAP */
#define HSPI_EN_STRAP                        0x00002000 /* Bit 13 for Host SPI STRAP*/
#define UART_STRAP                           0x00000080 /* Bit 7 for UART STRAP */
#define SDCARD_STRAP                         0x00000040 /* Bit 6 for SD CARD STRAP */
#define EXT_BUS_STRAP                        0x00000020 /* Bit 5 for EXT BUS STRAP */
#define RGMII_0_MODE_STRAP                   0x00000010 /* Bit 4 for RGMII 0 STRAP */
#define MAC1_STRAP                           0x00000008 /* Bit 3 for MAC 1 STRAP */


#define PACKED __attribute__ ((packed))

typedef unsigned char 	uint8; 
typedef struct
{
    uint8_t PortNum;
    uint8_t PinNum;
    uint8_t AltFunc;

}PACKED platform_value_t;

#endif
