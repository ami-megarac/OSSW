--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/ddrinit/ddr_options.c	1969-12-31 19:00:00.000000000 -0500
+++ mywork/arch/arm/cpu/armv7/pilot4/ddrinit/ddr_options.c	2015-03-23 09:21:22.390636099 -0400
@@ -0,0 +1,3726 @@
+//////////////////////////////////////////////////
+//#ServerEngines Public License
+//#This license governs use of the accompanying software. If you use the software, you
+//#accept this license. If you do not accept the license, do not use the software.
+//#
+//#1. Definitions
+//#The terms "reproduce," "reproduction," "derivative works," and "distribution" have the
+//#same meaning here as under U.S. copyright law.
+//#A "contribution" is the original software, or any additions or changes to the software.
+//#A "contributor" is any person that distributes its contribution under this license.
+//#"Licensed patents" are a contributor's patent claims that read directly on its contribution.
+//#
+//##2. Grant of Rights
+//###(A) Copyright Grant- Subject to the terms of this license, including the license conditions and limitations in section 3, each contributor grants you a non-exclusive, worldwide, royalty-free copyright license to reproduce its contribution, prepare derivative works of its contribution, and distribute its contribution or any derivative works that you create.
+//##(B) Patent Grant- Subject to the terms of this license, including the license conditions and limitations in section 3, each contributor grants you a non-exclusive, worldwide, royalty-free license under its licensed patents to make, have made, use, sell, offer for sale, import, and/or otherwise dispose of its contribution in the software or derivative works of the contribution in the software.
+//##
+//##3. Conditions and Limitations
+//##(A) No Trademark License- This license does not grant you rights to use any contributors' name, logo, or trademarks.
+//##(B) If you bring a patent claim against any contributor over patents that you claim are infringed by the software, your patent license from such contributor to the software ends automatically.
+//#(C) If you distribute any portion of the software, you must retain all copyright, patent, trademark, and attribution notices that are present in the software.
+//#(D) If you distribute any portion of the software in source code form, you may do so only under this license by including a complete copy of this license with your distribution. If you distribute any portion of the software in compiled or object code form, you may only do so under a license that complies with this license.
+//#(E) The software is licensed "as-is." You bear the risk of using it. The contributors give no express warranties, guarantees or conditions. You may have additional consumer rights under your local laws which this license cannot change. To the extent permitted under your local laws, the contributors exclude the implied warranties of merchantability, fitness for a particular purpose and non-infringement.
+//##
+//###       Project  : Pilot
+// ********************************************************************************
+// Proprietary Information of ServerEngines, Inc.
+//
+// This program is an unpublished work fully protected by the United States
+// copyright laws and is considered a trade secret belonging to ServerEngines, Inc.
+// To the extent that this work may be considered "published", the following notice
+// applies "(C) 2004, 2005, ServerEngines, Inc."
+//
+// Any unauthorized use, reproduction, distribution, display, modification
+// or disclosure of this program is strictly prohibited.
+// 
+//(((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((
+// Defines Description :-
+// ECC - For ECC ON. Which is enabled by default for Actual Hardware.
+// HW_ENV - Enabled for actual Hardware.
+// DDR4 - DDR4 1.2V Technology.
+// DDR3L - DDR3 1.35V Technology.
+// DDR3 - DDR3 1.5V Technology.
+// DDR Data Rate Supported:-
+// -------------------------------------------------------------------------------------------------
+//|     DDR Data Rate  |    Define to be enabled    |  DLL_OFF to |  Latencies                     |
+//|                    |                            |  be defined |                                |
+//|------------------------------------------------------------------------------------------------|
+//|     1600 Mbps      |     DDR3_1600MHZ           |  NO         | DDR3 - WL8RL11 DDR4 - WL11RL12 |
+//|------------------------------------------------------------------------------------------------|
+//|     1300 Mbps      |     DDR3_1333MHZ           |  NO         | DDR3 - WL7RL9 DDR4 - WL9RL10   |
+//|------------------------------------------------------------------------------------------------|
+//|     1000 Mbps      |     DDR3_1066MHZ           |  DDR3 - NO  | DDR3 - WL6RL7 DDR4 - WL9RL10   |
+//|                    |                            |  DDR4 - YES |                                |
+//|------------------------------------------------------------------------------------------------|
+//|     800 Mbps       |     DDR3_800MHZ            |  DDR3 - NO  | DDR3 - WL5RL6 DDR4 - WL9RL10   |
+//|                    |                            |  DDR4 - YES |                                |
+//|------------------------------------------------------------------------------------------------|
+//|     500 Mbps       |     DDR3_500MHZ            |  YES        | DDR3 - WL6RL6 DDR4 - WL9RL10   |
+//|------------------------------------------------------------------------------------------------|
+//|     400 Mbps       |     DDR3_350_400MHZ        |  YES        | DDR3 - WL6RL6 DDR4 - WL9RL10   |
+//|------------------------------------------------------------------------------------------------|
+//|     200 Mbps       |     DDR3_200_250MHZ        |  YES        | DDR3 - WL6RL6 DDR4 - WL9RL10   |
+//|------------------------------------------------------------------------------------------------|
+//|     100 Mbps       |     DDR3_75_125MHZ         |  YES        | DDR3 - WL6RL6 DDR4 - WL9RL10   |
+// -------------------------------------------------------------------------------------------------
+// SKIP_GATE_TRAINING - Enable only for Simulation.
+// SKIP_EYE_TRAINING - Enable only for Simulation. For DDR3_200_250MHZ, DDR3_75_125MHZ.
+// SKIP_FIFO_TRAINING - Enable only for Simulation.
+// SKIP_DEVICE_VREF_TRAINING - Enable only for device VREF training for DDR4.
+// MPR_READ - Enable only for Simulation.
+//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((
+// *************************************************************************************************
+#include "pilot_II.h"
+
+#define FPGA_PHY_RD_LATENCY (DDR_REG_BASE + 0xC000)
+#define FPGA_GATE_DLY_VAL   (DDR_REG_BASE + 0xC004)
+#define FPGA_IDLY_CTRL      (DDR_REG_BASE + 0xC008)
+
+//#define ECC                 1
+//#define HW_ENV              1
+//#define DDR4                1
+//#define DDR3                1
+//#define DDR3L               1
+//#define DDR3_75_125MHZ      1
+//#define DDR3_200_250MHZ     1
+//#define DDR3_350_400MHZ     1
+//#define DDR3_500MHZ         1
+//#define DDR3_800MHZ         1
+//#define DDR3_1066MHZ        1
+//#define DDR3_1333MHZ        1
+//#define DDR3_1600MHZ        1
+//#define WL5RL6              1
+//#define WL6RL6              1
+//#define WL6RL7              1
+//#define WL7RL9              1
+//#define WL8RL11             1
+//#define WL9RL10             1
+//#define WL11RL12            1
+//#define SKIP_GATE_TRAINING  1
+//#define SKIP_EYE_TRAINING   1
+//#define SKIP_FIFO_TRAINING  1
+#define SKIP_DEVICE_VREF_TRAINING 1
+//#define MPR_READ            1
+//#define DLL_OFF             1
+#define STARTFROMCENTERSWEEP_OFF             1
+//#define AUTO_DQCALIB         1
+//#define HW_ENVPRN            1
+
+  //char * LMEMDDRTRAINVAR = 0x1000b000; // Global array (1024 Bytes used).
+    unsigned int LMEMDDRTRAINVAR[1000]={0};
+  tU16 dqsgate_array[512] = { 
+    0x60,
+    0x5f,
+    0x5e,
+    0x5d,
+    0x5c,
+    0x5b,
+    0x5a,
+    0x59,
+    0x58,
+    0x57,
+    0x56,
+    0x55,
+    0x54,
+    0x53,
+    0x52,
+    0x51,
+    0x50,
+    0x4f,
+    0x4e,
+    0x4d,
+    0x4c,
+    0x4b,
+    0x4a,
+    0x49,
+    0x48,
+    0x47,
+    0x46,
+    0x45,
+    0x44,
+    0x43,
+    0x42,
+    0x41,
+    0x00,
+    0x01,
+    0x02,
+    0x03,
+    0x04,
+    0x05,
+    0x06,
+    0x07,
+    0x08,
+    0x09,
+    0x0a,
+    0x0b,
+    0x0c,
+    0x0d,
+    0x0e,
+    0x0f,
+    0x10,
+    0x11,
+    0x12,
+    0x13,
+    0x14,
+    0x15,
+    0x16,
+    0x17,
+    0x18,
+    0x19,
+    0x1a,
+    0x1b,
+    0x1c,
+    0x1d,
+    0x1e,
+    0x1f,
+    0x160,
+    0x15f,
+    0x15e,
+    0x15d,
+    0x15c,
+    0x15b,
+    0x15a,
+    0x159,
+    0x158,
+    0x157,
+    0x156,
+    0x155,
+    0x154,
+    0x153,
+    0x152,
+    0x151,
+    0x150,
+    0x14f,
+    0x14e,
+    0x14d,
+    0x14c,
+    0x14b,
+    0x14a,
+    0x149,
+    0x148,
+    0x147,
+    0x146,
+    0x145,
+    0x144,
+    0x143,
+    0x142,
+    0x141,
+    0x100,
+    0x101,
+    0x102,
+    0x103,
+    0x104,
+    0x105,
+    0x106,
+    0x107,
+    0x108,
+    0x109,
+    0x10a,
+    0x10b,
+    0x10c,
+    0x10d,
+    0x10e,
+    0x10f,
+    0x110,
+    0x111,
+    0x112,
+    0x113,
+    0x114,
+    0x115,
+    0x116,
+    0x117,
+    0x118,
+    0x119,
+    0x11a,
+    0x11b,
+    0x11c,
+    0x11d,
+    0x11e,
+    0x11f,
+    0x260,
+    0x25f,
+    0x25e,
+    0x25d,
+    0x25c,
+    0x25b,
+    0x25a,
+    0x259,
+    0x258,
+    0x257,
+    0x256,
+    0x255,
+    0x254,
+    0x253,
+    0x252,
+    0x251,
+    0x250,
+    0x24f,
+    0x24e,
+    0x24d,
+    0x24c,
+    0x24b,
+    0x24a,
+    0x249,
+    0x248,
+    0x247,
+    0x246,
+    0x245,
+    0x244,
+    0x243,
+    0x242,
+    0x241,
+    0x200,
+    0x201,
+    0x202,
+    0x203,
+    0x204,
+    0x205,
+    0x206,
+    0x207,
+    0x208,
+    0x209,
+    0x20a,
+    0x20b,
+    0x20c,
+    0x20d,
+    0x20e,
+    0x20f,
+    0x210,
+    0x211,
+    0x212,
+    0x213,
+    0x214,
+    0x215,
+    0x216,
+    0x217,
+    0x218,
+    0x219,
+    0x21a,
+    0x21b,
+    0x21c,
+    0x21d,
+    0x21e,
+    0x21f,
+    0x360,
+    0x35f,
+    0x35e,
+    0x35d,
+    0x35c,
+    0x35b,
+    0x35a,
+    0x359,
+    0x358,
+    0x357,
+    0x356,
+    0x355,
+    0x354,
+    0x353,
+    0x352,
+    0x351,
+    0x350,
+    0x34f,
+    0x34e,
+    0x34d,
+    0x34c,
+    0x34b,
+    0x34a,
+    0x349,
+    0x348,
+    0x347,
+    0x346,
+    0x345,
+    0x344,
+    0x343,
+    0x342,
+    0x341,
+    0x300,
+    0x301,
+    0x302,
+    0x303,
+    0x304,
+    0x305,
+    0x306,
+    0x307,
+    0x308,
+    0x309,
+    0x30a,
+    0x30b,
+    0x30c,
+    0x30d,
+    0x30e,
+    0x30f,
+    0x310,
+    0x311,
+    0x312,
+    0x313,
+    0x314,
+    0x315,
+    0x316,
+    0x317,
+    0x318,
+    0x319,
+    0x31a,
+    0x31b,
+    0x31c,
+    0x31d,
+    0x31e,
+    0x31f,
+    0x460,
+    0x45f,
+    0x45e,
+    0x45d,
+    0x45c,
+    0x45b,
+    0x45a,
+    0x459,
+    0x458,
+    0x457,
+    0x456,
+    0x455,
+    0x454,
+    0x453,
+    0x452,
+    0x451,
+    0x450,
+    0x44f,
+    0x44e,
+    0x44d,
+    0x44c,
+    0x44b,
+    0x44a,
+    0x449,
+    0x448,
+    0x447,
+    0x446,
+    0x445,
+    0x444,
+    0x443,
+    0x442,
+    0x441,
+    0x400,
+    0x401,
+    0x402,
+    0x403,
+    0x404,
+    0x405,
+    0x406,
+    0x407,
+    0x408,
+    0x409,
+    0x40a,
+    0x40b,
+    0x40c,
+    0x40d,
+    0x40e,
+    0x40f,
+    0x410,
+    0x411,
+    0x412,
+    0x413,
+    0x414,
+    0x415,
+    0x416,
+    0x417,
+    0x418,
+    0x419,
+    0x41a,
+    0x41b,
+    0x41c,
+    0x41d,
+    0x41e,
+    0x41f,
+    0x560,
+    0x55f,
+    0x55e,
+    0x55d,
+    0x55c,
+    0x55b,
+    0x55a,
+    0x559,
+    0x558,
+    0x557,
+    0x556,
+    0x555,
+    0x554,
+    0x553,
+    0x552,
+    0x551,
+    0x550,
+    0x54f,
+    0x54e,
+    0x54d,
+    0x54c,
+    0x54b,
+    0x54a,
+    0x549,
+    0x548,
+    0x547,
+    0x546,
+    0x545,
+    0x544,
+    0x543,
+    0x542,
+    0x541,
+    0x500,
+    0x501,
+    0x502,
+    0x503,
+    0x504,
+    0x505,
+    0x506,
+    0x507,
+    0x508,
+    0x509,
+    0x50a,
+    0x50b,
+    0x50c,
+    0x50d,
+    0x50e,
+    0x50f,
+    0x510,
+    0x511,
+    0x512,
+    0x513,
+    0x514,
+    0x515,
+    0x516,
+    0x517,
+    0x518,
+    0x519,
+    0x51a,
+    0x51b,
+    0x51c,
+    0x51d,
+    0x51e,
+    0x51f,
+    0x660,
+    0x65f,
+    0x65e,
+    0x65d,
+    0x65c,
+    0x65b,
+    0x65a,
+    0x659,
+    0x658,
+    0x657,
+    0x656,
+    0x655,
+    0x654,
+    0x653,
+    0x652,
+    0x651,
+    0x650,
+    0x64f,
+    0x64e,
+    0x64d,
+    0x64c,
+    0x64b,
+    0x64a,
+    0x649,
+    0x648,
+    0x647,
+    0x646,
+    0x645,
+    0x644,
+    0x643,
+    0x642,
+    0x641,
+    0x600,
+    0x601,
+    0x602,
+    0x603,
+    0x604,
+    0x605,
+    0x606,
+    0x607,
+    0x608,
+    0x609,
+    0x60a,
+    0x60b,
+    0x60c,
+    0x60d,
+    0x60e,
+    0x60f,
+    0x610,
+    0x611,
+    0x612,
+    0x613,
+    0x614,
+    0x615,
+    0x616,
+    0x617,
+    0x618,
+    0x619,
+    0x61a,
+    0x61b,
+    0x61c,
+    0x61d,
+    0x61e,
+    0x61f,
+    0x760,
+    0x75f,
+    0x75e,
+    0x75d,
+    0x75c,
+    0x75b,
+    0x75a,
+    0x759,
+    0x758,
+    0x757,
+    0x756,
+    0x755,
+    0x754,
+    0x753,
+    0x752,
+    0x751,
+    0x750,
+    0x74f,
+    0x74e,
+    0x74d,
+    0x74c,
+    0x74b,
+    0x74a,
+    0x749,
+    0x748,
+    0x747,
+    0x746,
+    0x745,
+    0x744,
+    0x743,
+    0x742,
+    0x741,
+    0x700,
+    0x701,
+    0x702,
+    0x703,
+    0x704,
+    0x705,
+    0x706,
+    0x707,
+    0x708,
+    0x709,
+    0x70a,
+    0x70b,
+    0x70c,
+    0x70d,
+    0x70e,
+    0x70f,
+    0x710,
+    0x711,
+    0x712,
+    0x713,
+    0x714,
+    0x715,
+    0x716,
+    0x717,
+    0x718,
+    0x719,
+    0x71a,
+    0x71b,
+    0x71c,
+    0x71d,
+    0x71e,
+    0x71f
+  };
+
+  tU16 VrefPN_array[256][2] = { 
+    {0x000, 0x1ff},
+    {0x002, 0x1fd},
+    {0x004, 0x1fb},
+    {0x006, 0x1f9},
+    {0x008, 0x1f7},
+    {0x00a, 0x1f5},
+    {0x00c, 0x1f3},
+    {0x00e, 0x1f1},
+    {0x010, 0x1ef},
+    {0x012, 0x1ed},
+    {0x014, 0x1eb},
+    {0x016, 0x1e9},
+    {0x018, 0x1e7},
+    {0x01a, 0x1e5},
+    {0x01c, 0x1e3},
+    {0x01e, 0x1e1},
+    {0x020, 0x1df},
+    {0x022, 0x1dd},
+    {0x024, 0x1db},
+    {0x026, 0x1d9},
+    {0x028, 0x1d7},
+    {0x02a, 0x1d5},
+    {0x02c, 0x1d3},
+    {0x02e, 0x1d1},
+    {0x030, 0x1cf},
+    {0x032, 0x1cd},
+    {0x034, 0x1cb},
+    {0x036, 0x1c9},
+    {0x038, 0x1c7},
+    {0x03a, 0x1c5},
+    {0x03c, 0x1c3},
+    {0x03e, 0x1c1},
+    {0x040, 0x1bf},
+    {0x042, 0x1bd},
+    {0x044, 0x1bb},
+    {0x046, 0x1b9},
+    {0x048, 0x1b7},
+    {0x04a, 0x1b5},
+    {0x04c, 0x1b3},
+    {0x04e, 0x1b1},
+    {0x050, 0x1af},
+    {0x052, 0x1ad},
+    {0x054, 0x1ab},
+    {0x056, 0x1a9},
+    {0x058, 0x1a7},
+    {0x05a, 0x1a5},
+    {0x05c, 0x1a3},
+    {0x05e, 0x1a1},
+    {0x060, 0x19f},
+    {0x062, 0x19d},
+    {0x064, 0x19b},
+    {0x066, 0x199},
+    {0x068, 0x197},
+    {0x06a, 0x195},
+    {0x06c, 0x193},
+    {0x06e, 0x191},
+    {0x070, 0x18f},
+    {0x072, 0x18d},
+    {0x074, 0x18b},
+    {0x076, 0x189},
+    {0x078, 0x187},
+    {0x07a, 0x185},
+    {0x07c, 0x183},
+    {0x07e, 0x181},
+    {0x080, 0x17f},
+    {0x082, 0x17d},
+    {0x084, 0x17b},
+    {0x086, 0x179},
+    {0x088, 0x177},
+    {0x08a, 0x175},
+    {0x08c, 0x173},
+    {0x08e, 0x171},
+    {0x090, 0x16f},
+    {0x092, 0x16d},
+    {0x094, 0x16b},
+    {0x096, 0x169},
+    {0x098, 0x167},
+    {0x09a, 0x165},
+    {0x09c, 0x163},
+    {0x09e, 0x161},
+    {0x0a0, 0x15f},
+    {0x0a2, 0x15d},
+    {0x0a4, 0x15b},
+    {0x0a6, 0x159},
+    {0x0a8, 0x157},
+    {0x0aa, 0x155},
+    {0x0ac, 0x153},
+    {0x0ae, 0x151},
+    {0x0b0, 0x14f},
+    {0x0b2, 0x14d},
+    {0x0b4, 0x14b},
+    {0x0b6, 0x149},
+    {0x0b8, 0x147},
+    {0x0ba, 0x145},
+    {0x0bc, 0x143},
+    {0x0be, 0x141},
+    {0x0c0, 0x13f},
+    {0x0c2, 0x13d},
+    {0x0c4, 0x13b},
+    {0x0c6, 0x139},
+    {0x0c8, 0x137},
+    {0x0ca, 0x135},
+    {0x0cc, 0x133},
+    {0x0ce, 0x131},
+    {0x0d0, 0x12f},
+    {0x0d2, 0x12d},
+    {0x0d4, 0x12b},
+    {0x0d6, 0x129},
+    {0x0d8, 0x127},
+    {0x0da, 0x125},
+    {0x0dc, 0x123},
+    {0x0de, 0x121},
+    {0x0e0, 0x11f},
+    {0x0e2, 0x11d},
+    {0x0e4, 0x11b},
+    {0x0e6, 0x119},
+    {0x0e8, 0x117},
+    {0x0ea, 0x115},
+    {0x0ec, 0x113},
+    {0x0ee, 0x111},
+    {0x0f0, 0x10f},
+    {0x0f2, 0x10d},
+    {0x0f4, 0x10b},
+    {0x0f6, 0x109},
+    {0x0f8, 0x107},
+    {0x0fa, 0x105},
+    {0x0fc, 0x103},
+    {0x0fe, 0x101},
+    {0x100, 0x0ff},
+    {0x102, 0x0fd},
+    {0x104, 0x0fb},
+    {0x106, 0x0f9},
+    {0x108, 0x0f7},
+    {0x10a, 0x0f5},
+    {0x10c, 0x0f3},
+    {0x10e, 0x0f1},
+    {0x110, 0x0ef},
+    {0x112, 0x0ed},
+    {0x114, 0x0eb},
+    {0x116, 0x0e9},
+    {0x118, 0x0e7},
+    {0x11a, 0x0e5},
+    {0x11c, 0x0e3},
+    {0x11e, 0x0e1},
+    {0x120, 0x0df},
+    {0x122, 0x0dd},
+    {0x124, 0x0db},
+    {0x126, 0x0d9},
+    {0x128, 0x0d7},
+    {0x12a, 0x0d5},
+    {0x12c, 0x0d3},
+    {0x12e, 0x0d1},
+    {0x130, 0x0cf},
+    {0x132, 0x0cd},
+    {0x134, 0x0cb},
+    {0x136, 0x0c9},
+    {0x138, 0x0c7},
+    {0x13a, 0x0c5},
+    {0x13c, 0x0c3},
+    {0x13e, 0x0c1},
+    {0x140, 0x0bf},
+    {0x142, 0x0bd},
+    {0x144, 0x0bb},
+    {0x146, 0x0b9},
+    {0x148, 0x0b7},
+    {0x14a, 0x0b5},
+    {0x14c, 0x0b3},
+    {0x14e, 0x0b1},
+    {0x150, 0x0af},
+    {0x152, 0x0ad},
+    {0x154, 0x0ab},
+    {0x156, 0x0a9},
+    {0x158, 0x0a7},
+    {0x15a, 0x0a5},
+    {0x15c, 0x0a3},
+    {0x15e, 0x0a1},
+    {0x160, 0x09f},
+    {0x162, 0x09d},
+    {0x164, 0x09b},
+    {0x166, 0x099},
+    {0x168, 0x097},
+    {0x16a, 0x095},
+    {0x16c, 0x093},
+    {0x16e, 0x091},
+    {0x170, 0x08f},
+    {0x172, 0x08d},
+    {0x174, 0x08b},
+    {0x176, 0x089},
+    {0x178, 0x087},
+    {0x17a, 0x085},
+    {0x17c, 0x083},
+    {0x17e, 0x081},
+    {0x180, 0x07f},
+    {0x182, 0x07d},
+    {0x184, 0x07b},
+    {0x186, 0x079},
+    {0x188, 0x077},
+    {0x18a, 0x075},
+    {0x18c, 0x073},
+    {0x18e, 0x071},
+    {0x190, 0x06f},
+    {0x192, 0x06d},
+    {0x194, 0x06b},
+    {0x196, 0x069},
+    {0x198, 0x067},
+    {0x19a, 0x065},
+    {0x19c, 0x063},
+    {0x19e, 0x061},
+    {0x1a0, 0x05f},
+    {0x1a2, 0x05d},
+    {0x1a4, 0x05b},
+    {0x1a6, 0x059},
+    {0x1a8, 0x057},
+    {0x1aa, 0x055},
+    {0x1ac, 0x053},
+    {0x1ae, 0x051},
+    {0x1b0, 0x04f},
+    {0x1b2, 0x04d},
+    {0x1b4, 0x04b},
+    {0x1b6, 0x049},
+    {0x1b8, 0x047},
+    {0x1ba, 0x045},
+    {0x1bc, 0x043},
+    {0x1be, 0x041},
+    {0x1c0, 0x03f},
+    {0x1c2, 0x03d},
+    {0x1c4, 0x03b},
+    {0x1c6, 0x039},
+    {0x1c8, 0x037},
+    {0x1ca, 0x035},
+    {0x1cc, 0x033},
+    {0x1ce, 0x031},
+    {0x1d0, 0x02f},
+    {0x1d2, 0x02d},
+    {0x1d4, 0x02b},
+    {0x1d6, 0x029},
+    {0x1d8, 0x027},
+    {0x1da, 0x025},
+    {0x1dc, 0x023},
+    {0x1de, 0x021},
+    {0x1e0, 0x01f},
+    {0x1e2, 0x01d},
+    {0x1e4, 0x01b},
+    {0x1e6, 0x019},
+    {0x1e8, 0x017},
+    {0x1ea, 0x015},
+    {0x1ec, 0x013},
+    {0x1ee, 0x011},
+    {0x1f0, 0x00f},
+    {0x1f2, 0x00d},
+    {0x1f4, 0x00b},
+    {0x1f6, 0x009},
+    {0x1f8, 0x007},
+    {0x1fa, 0x005},
+    {0x1fc, 0x003},
+    {0x1fe, 0x001},
+  };
+
+  // First 8 are DDR3, next 8 are DDR4. Frequencies Highest to lowest.
+  tU8 trc_array[16] = {
+    0x12,
+    0x0e,
+    0x0b,
+    0x08,
+    0x05,
+    0x03,
+    0x01,
+    0x01,
+    0x12,
+    0x0f,
+    0x0b,
+    0x08,
+    0x05,
+    0x03,
+    0x01,
+    0x01
+  };
+  tU8 tmrs2nmrs_array[8] = {
+    0x0a,
+    0x08,
+    0x06,
+    0x05,
+    0x03,
+    0x03,
+    0x02,
+    0x01
+  };
+  tU16 tfawtras_array[16] = { 
+    0x0194,
+    0x0151,
+    0x00ed,
+    0x00aa,
+    0x0067,
+    0x0045,
+    0x0023,
+    0x0022,
+    0x018e,
+    0x014c,
+    0x00e9,
+    0x00a7,
+    0x0065,
+    0x0044,
+    0x0022,
+    0x0021
+  };
+  tU8 trfc10_array[16][4] = { 
+    {0x2e, 0x42, 0x6a, 0x8e},
+    {0x26, 0x36, 0x57, 0x74},
+    {0x1d, 0x29, 0x42, 0x59},
+    {0x16, 0x20, 0x34, 0x46},
+    {0x0e, 0x14, 0x21, 0x2c},
+    {0x0a, 0x0f, 0x19, 0x22},
+    {0x05, 0x07, 0x0c, 0x11},
+    {0x02, 0x03, 0x06, 0x08},
+    {0xb2, 0x42, 0x6a, 0x8e},
+    {0x91, 0x36, 0x57, 0x74},
+    {0x6f, 0x29, 0x42, 0x59},
+    {0x58, 0x20, 0x34, 0x46},
+    {0x37, 0x14, 0x21, 0x2c},
+    {0x2b, 0x0f, 0x19, 0x22},
+    {0x15, 0x07, 0x0c, 0x11},
+    {0x0a, 0x03, 0x06, 0x08}
+  };
+  tU8 trfc_array[16][4] = { 
+    {0x2a, 0x3e, 0x66, 0x8a},
+    {0x22, 0x32, 0x53, 0x70},
+    {0x1a, 0x26, 0x3f, 0x56},
+    {0x14, 0x1e, 0x32, 0x44},
+    {0x0c, 0x12, 0x1f, 0x2a},
+    {0x09, 0x0e, 0x18, 0x21},
+    {0x04, 0x06, 0x0b, 0x10},
+    {0x01, 0x02, 0x05, 0x07},
+    {0xae, 0x3e, 0x66, 0x8a},
+    {0x8d, 0x32, 0x53, 0x70},
+    {0x6c, 0x26, 0x3f, 0x56},
+    {0x56, 0x1e, 0x32, 0x44},
+    {0x35, 0x12, 0x1f, 0x2a},
+    {0x2a, 0x0e, 0x18, 0x21},
+    {0x14, 0x06, 0x0b, 0x10},
+    {0x09, 0x02, 0x05, 0x07}
+  };
+
+
+// delay function
+void delay (tU32 amt){
+  while (amt != 0){
+    amt--;
+  }
+}
+
+void DELAY(tU32 count)
+{
+  tU32 i=0;
+#if defined (HW_ENV)
+  for(i=0;i<(count*100);i++) {
+#else
+  for(i=0;i<count;i++) {
+#endif
+    // *(tPVU32)(LMEMDDRTRAINVAR + 0x100);
+#if defined (FPGA_DUT)
+#else
+    *(tPVU32)(0x10000000);
+#endif
+  }
+}//DELAY
+
+#if defined (FPGA_DUT)
+  tU32 ddrinit(tU8 ddr_type){
+  tU32 temp, temp1, ddr;
+  tU32 i;
+
+
+#if defined (HW_ENV)
+  nc_printf("FPGA SVN 656 : DDR init\n");
+#endif
+  //temp = *(tPVU32)(MC_REG_ADDRESS04);
+  // testbus enable
+  //WRITE_REG32(0x4010015c, 0xdffcffff);
+
+  //PHY RESET
+  *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & 0xfffffdff) | 0x00000200;
+  // READ LATENCY
+  *(tPVU32)(DDR_REG_BASE + 0xC000) = (*(tPVU32)(DDR_REG_BASE + 0xC000) & 0xfffffff0) | 0x00000007;
+
+  if (ddr_type == 1) {
+    // 200.
+    // DDR4.r_ddr4_mode=1 (28) r_zq2anycounter=510 (16,15,14,13,12,11,10,9,8),
+    temp = *(tPVU32)(MC_REG_ADDRESS04);
+    temp = temp & ~0x1001ff00;
+    temp = temp | 0x1001fe00; 
+    *(tPVU32)(MC_REG_ADDRESS04) = temp;
+
+    //TYPE 2
+    temp = *(tPVU32)(MC_REG_ADDRESS04);
+    temp = temp & ~0x0e000000;
+    temp = temp | 0x04000000; //TYPE2
+    *(tPVU32)(MC_REG_ADDRESS04) = temp;
+    // tRC = 50 r_act2actsbcounter(24,23,22,21,20).
+    temp = *(tPVU32)(MC_REG_ADDRESS14);
+    temp = temp & ~0x01f00000;
+    temp = temp | 0x00300000; 
+    *(tPVU32)(MC_REG_ADDRESS14) = temp;
+    // DDR4:- tMOD - 24. r_mrs2nmrscounter (15,14,13,12)
+    temp = *(tPVU32)(MC_REG_ADDRESS08);
+    temp = temp & ~0x0000f000;
+    temp = temp | 0x00003000; 
+    *(tPVU32)(MC_REG_ADDRESS08) = temp;
+    // FAW = 35ns (24,23,22,21,20). tRAS = 35ns. r_act2precounter (29,28,27,26,25),r_act2actdb_del_1=1 (18)
+    temp = *(tPVU32)(MC_REG_ADDRESS10);
+    temp = temp & ~0x3ff40000;
+    temp = temp | 0x04340000; 
+    *(tPVU32)(MC_REG_ADDRESS10) = temp;
+    // tRFC = 160ns. r_trfcplus10counter (7,6,5,4,3,2,1,0),
+    // r_ref2actcounter (24,23,22,21,20,19,18,17),
+    temp = *(tPVU32)(MC_REG_ADDRESS04);
+    temp = temp & ~0x01fe00ff;
+    temp = temp | 0x001c000f; 
+    *(tPVU32)(MC_REG_ADDRESS04) = temp;
+    // WL9RL10. DLL_OFF.
+    temp = *(tPVU32)(MC_REG_ADDRESS10);
+    temp = temp & ~0xc000ffff;
+    temp = temp | 0x00000210; 
+    *(tPVU32)(MC_REG_ADDRESS10) = temp;
+
+    DELAY(100);
+
+    //DLL OFF  
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffe7;
+    temp = temp | 0x00000024;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+
+    DELAY(100);
+
+    //allowCke
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00000404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+
+    DELAY(50);
+
+    // MRS3.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00007404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS6.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x0400d404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS5. All 0
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x0400b404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS4. All 0
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00009404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS2. WL9RL10.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00805404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // DLL OFF.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00003404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0); 
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00051404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // ZQ calib.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00000584;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+  } else {
+    // DDR3.
+    // tRC = 48.75. r_act2actsbcounter (24,23,22,21,20).
+    temp = *(tPVU32)(MC_REG_ADDRESS14);
+    temp = temp & ~0x01f00000;
+    temp = temp | 0x00300000; 
+    *(tPVU32)(MC_REG_ADDRESS14) = temp;
+    
+    //TYPE 2
+    temp = *(tPVU32)(MC_REG_ADDRESS04);
+    temp = temp & ~0x0e000000;
+    temp = temp | 0x04000000; //TYPE2
+    *(tPVU32)(MC_REG_ADDRESS04) = temp;
+    // FAW = 50ns (24,23,22,21,20). tRAS = 36ns. r_act2precounter (29,28,27,26,25),
+    temp = *(tPVU32)(MC_REG_ADDRESS10);
+    temp = temp & ~0x3ff00000;
+    temp = temp | 0x04400000; 
+    *(tPVU32)(MC_REG_ADDRESS10) = temp;
+    // tRFC = 350. r_trfcplus10counter (7,6,5,4,3,2,1,0),
+    // r_ref2actcounter (24,23,22,21,20,19,18,17), 
+    temp = *(tPVU32)(MC_REG_ADDRESS04);
+    temp = temp & ~0x01fe00ff;
+    temp = temp | 0x001c000f; 
+    *(tPVU32)(MC_REG_ADDRESS04) = temp;
+    // r_zq2anycounter=254 (16,15,14,13,12,11,10,9,8), 
+    temp = *(tPVU32)(MC_REG_ADDRESS04);
+    temp = temp & ~0x0001ff00;
+    temp = temp | 0x0000fe00; 
+    *(tPVU32)(MC_REG_ADDRESS04) = temp;
+    // WL6RL6
+    temp = *(tPVU32)(MC_REG_ADDRESS10);
+    temp = temp & ~0xc000ffff;
+    temp = temp | 0x00000245; 
+    *(tPVU32)(MC_REG_ADDRESS10) = temp;
+  
+    DELAY(100);
+    //DLL OFF  
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffe7;
+    temp = temp | 0x00000024;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    // DDR3:- tMOD - 12. Program 4. r_mrs2nmrscounter 0x08 (15,14,13,12)
+    temp = *(tPVU32)(MC_REG_ADDRESS08);
+    temp = temp & ~0x0000f000;
+    temp = temp | 0x00004000; 
+    *(tPVU32)(MC_REG_ADDRESS08) = temp;
+
+    DELAY(100);
+
+    //allowCke
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00000404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+
+    DELAY(50);
+
+    // MRS2.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00085404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS3.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00007404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS1.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00063404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS reset DLL.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xff000040;
+    temp = temp | 0x13000000;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0x00ffff87;
+    temp = temp | 0x00211404;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // ZQ calib.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00000584;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS1. DLL OFF.
+    temp = *(tPVU32)(MC_REG_ADDRESS00);
+    temp = temp & ~0xffffffc7;
+    temp = temp | 0x00033444;
+    *(tPVU32)(MC_REG_ADDRESS00) = temp;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    
+    temp = *(tPVU32)(MC_REG_ADDRESS10);
+    temp = temp & ~0x0000081f;
+    temp = temp | 0x00000005; //WL6RL6
+    *(tPVU32)(MC_REG_ADDRESS10) = temp;
+  }
+
+  // Refresh.
+  // Refresh counters based on clock freq.
+  temp = *(tPVU32)(MC_REG_ADDRESS18);
+  temp = temp & ~0x0000ffff;
+  temp = temp | 0x000002004;
+  *(tPVU32)(MC_REG_ADDRESS18) = temp;
+  DELAY(5);
+  temp = *(tPVU32)(MC_REG_ADDRESS18);
+  temp = temp & ~0x00000fff;
+  temp = temp | 0x0000000c3;
+  *(tPVU32)(MC_REG_ADDRESS18) = temp;
+
+  // 
+  temp = *(tPVU32)(MC_REG_ADDRESS08);
+  temp = temp & ~0xffff0f00;
+  temp = temp | 0xa8350300;
+  *(tPVU32)(MC_REG_ADDRESS08) = temp;
+  // 
+  temp = *(tPVU32)(MC_REG_ADDRESS0C);
+  temp = temp & ~0x0ffe003f;
+  temp = temp | 0x06260022;
+  *(tPVU32)(MC_REG_ADDRESS0C) = temp;
+  // 
+  temp = *(tPVU32)(MC_REG_ADDRESS14);
+  temp = temp & ~0xfe0fffff;
+  temp = temp | 0x5005b030;
+  *(tPVU32)(MC_REG_ADDRESS14) = temp;
+  // 
+  temp = *(tPVU32)(MC_REG_ADDRESS28);
+  temp = temp & ~0xfc00f000;
+  temp = temp | 0x90000000;
+  *(tPVU32)(MC_REG_ADDRESS28) = temp;
+  // 
+  temp = *(tPVU32)(MC_REG_ADDRESS34);
+  temp = temp & ~0x7fffffff;
+  temp = temp | 0x42492248;
+  *(tPVU32)(MC_REG_ADDRESS34) = temp;
+  // MAGM.
+  temp = *(tPVU32)(MC_REG_ADDRESS38);
+  temp = temp | MAGM_VALUE;
+  *(tPVU32)(MC_REG_ADDRESS38) = temp;
+  // UAGM
+  temp = *(tPVU32)(MC_REG_ADDRESS3C);
+  temp = temp | UAGM_VALUE;
+  *(tPVU32)(MC_REG_ADDRESS3C) = temp;
+  // 
+  temp = *(tPVU32)(MC_REG_ADDRESS40);
+  temp = temp & ~0x7fffffff;
+  temp = temp | 0x40004d67;
+  *(tPVU32)(MC_REG_ADDRESS40) = temp;
+  // 
+  temp = *(tPVU32)(MC_REG_ADDRESS68);
+  temp = temp & ~0xbfffffff;
+  temp = temp | 0x1f517922;
+  *(tPVU32)(MC_REG_ADDRESS68) = temp;
+  // 
+  temp = *(tPVU32)(MC_REG_ADDRESS48);
+  temp = temp & ~0xffffffff;
+  temp = temp | 0xffffffff;
+  *(tPVU32)(MC_REG_ADDRESS48) = temp;
+  // 
+  temp = *(tPVU32)(MC_REG_ADDRESS54);
+  temp = temp & ~0xffffffff;
+  temp = temp | 0xffffffff;
+  *(tPVU32)(MC_REG_ADDRESS54) = temp;
+  // 
+  temp = *(tPVU32)(MC_REG_ADDRESS70);
+  temp = temp & ~0xffffffff;
+  temp = temp | 0xffffffff;
+  *(tPVU32)(MC_REG_ADDRESS70) = temp;
+  // Lower ECC address
+  *(tPVU32)(MC_REG_ADDRESS58) = LOWER_ECC_ADDRESS;
+  // Upper ECC address
+  *(tPVU32)(MC_REG_ADDRESS5C) = UPPER_ECC_ADDRESS;
+  
+  if (ddr_type == 1) {
+    //BOARD_DELAY_ONE_CLK WL9RL10 DLL_OFF
+    temp = *(tPVU32)(MC_REG_ADDRESS10);
+    temp = temp & ~0x00010fe0;
+    temp = temp | 0x00010a20; 
+    *(tPVU32)(MC_REG_ADDRESS10) = temp;
+  } else {
+    //BOARD_DELAY_ONE_CLK WL6RL6 DLL_OFF
+    temp = *(tPVU32)(MC_REG_ADDRESS10);
+    temp = temp & ~0x00010040;
+    temp = temp | 0x00010040; 
+    *(tPVU32)(MC_REG_ADDRESS10) = temp;
+  }
+
+  //
+  temp = *(tPVU32)(MC_REG_ADDRESS1C);
+  temp = temp & ~0xbfffc000;
+  temp = temp | 0xbc730000;
+  *(tPVU32)(MC_REG_ADDRESS1C) = temp;
+
+  DELAY(5);
+
+  //New FPGA Phy gate training
+#if 0
+
+  tU8 delay_value, low_found;
+  tU8 high_delay_value, low_delay_value;
+  delay_value = 0;
+  low_found = 0;
+  high_delay_value = 0;
+  low_delay_value = 0;
+
+
+  WRITE_REG32(FPGA_PHY_RD_LATENCY, ((READ_REG32(FPGA_PHY_RD_LATENCY) & ~0x00ffff10) | 0x10)); //r_fifo_cntr_reset = 1
+  WRITE_REG32(FPGA_PHY_RD_LATENCY, ((READ_REG32(FPGA_PHY_RD_LATENCY) & ~0x00000010) | 0x00)); //r_fifo_cntr_reset = 0
+  WRITE_REG32(MC_REG_ADDRESS00, ((READ_REG32(MC_REG_ADDRESS00) & ~0x10) | 0x10));
+
+  //nc_printf("FPGA_PHY_RD_LATENCY reg value 1: %x \n", READ_REG32(FPGA_PHY_RD_LATENCY));
+
+  while (delay_value < 0xf) {
+    
+    WRITE_REG32(MC_REG_ADDRESS00, ((READ_REG32(MC_REG_ADDRESS00) & ~0x03) | 0x03));
+    READ_REG32(0x80000000);
+    DELAY(10);
+    temp = READ_REG32(FPGA_PHY_RD_LATENCY);
+    temp = temp & 0x80000000;
+    if ((temp == 0x80000000) && (low_found == 1)) {
+      high_delay_value = delay_value;
+    }
+    if ((temp == 0x80000000) && (low_found == 0)) {
+      low_found =1;
+      low_delay_value = delay_value;
+    }
+    delay_value = delay_value + 1;
+    temp = (delay_value & 0xff);
+    WRITE_REG32(FPGA_GATE_DLY_VAL, ((READ_REG32(FPGA_GATE_DLY_VAL) & ~0x000000ff) | temp)); //program delay
+
+    WRITE_REG32(FPGA_PHY_RD_LATENCY, ((READ_REG32(FPGA_PHY_RD_LATENCY) & ~0x00000010) | 0x10)); //r_fifo_cntr_reset = 1
+    WRITE_REG32(FPGA_PHY_RD_LATENCY, ((READ_REG32(FPGA_PHY_RD_LATENCY) & ~0x00000010) | 0x00)); //r_fifo_cntr_reset = 0
+
+
+  }
+
+ // Program the middle of the passing value for the gate.
+  delay_value = (low_delay_value + high_delay_value);
+  delay_value = (delay_value >> 1);
+  temp = (delay_value & 0xff);
+
+#if defined (HW_ENV)
+  nc_printf("\n GT: low_delay_value: %x  high_delay_value: %x  delay value: %x \n", low_delay_value, high_delay_value, delay_value);
+#endif
+
+#else
+  temp = 7;
+#endif
+
+  WRITE_REG32(FPGA_GATE_DLY_VAL, ((READ_REG32(FPGA_GATE_DLY_VAL) & ~0x000000ff) | temp)); //program delay
+  WRITE_REG32(MC_REG_ADDRESS00, ((READ_REG32(MC_REG_ADDRESS00) & ~0x13) | 0x03));
+
+
+
+#if 0   //DELAY DQ
+#if defined (HW_ENV)
+  nc_printf("DQ EYE\n");
+#endif
+
+  //Rst dq0, dq1 idelays here
+  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000011));
+  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+
+  temp = 0x5;
+
+  for (i = 0; i <= temp; i = i+1) {
+    WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000022));
+    WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+    DELAY(100);
+  }
+
+#endif
+
+#if 0
+  //Eye Training
+
+  
+  tU8 dqs0_dly_low, dqs0_dly_high, dqs0_dly, dqs0_dly_mid;
+  //  tU32 dqs1_dly_low, dqs1_dly_high, dqs1_dly, dqs1_dly_mid;
+  tU8 byte0_0, byte1_0, byte2_0, byte3_0;
+  tU8 byte0_1, byte1_1, byte2_1, byte3_1;
+  tU8 low_eye_found, high_eye_found;
+  // Clear memory
+  //WRITE_REG32(MC_REG_ADDRESS00, ((READ_REG32(MC_REG_ADDRESS00) & ~0x8) | 0x8));
+  //while((*(tPVU32)(MC_REG_ADDRESS00) & 0x08) != 0);
+
+  WRITE_REG32(0x80000000, 0xa5a55a5a);
+  WRITE_REG32((0x80000000+4), 0xa5a55a5a);
+  //temp = READ_REG32(0x80000000);
+  //temp1 = READ_REG32(0x80000000+4);
+  //nc_printf("Initial: temp:%x , temp1:%x \n", temp, temp1);
+  
+
+  //Rst dq0, dq1, dqs0, dqs1 idelays here
+  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00001111));
+  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+
+  //DQS0 
+  dqs0_dly_low = 0;
+  dqs0_dly_high = 0;
+  dqs0_dly = 0;
+  dqs0_dly_mid = 0;
+  low_eye_found = 0;
+  high_eye_found = 0;
+  while (dqs0_dly < 62) {
+    if ((low_eye_found == 0) || (high_eye_found == 0)) {
+      temp = READ_REG32(0x80000000);
+      temp1 = READ_REG32(0x80000000+4);
+      //nc_printf("DQS0: temp:%x , temp1:%x , dqs0_dly:%x \n", temp, temp1, dqs0_dly);
+
+      byte0_0 = temp & 0xff;
+      byte1_0 = (temp >> 8) & 0xff;
+      byte2_0 = (temp >> 16) & 0xff;
+      byte3_0 = (temp >> 24) & 0xff;
+      
+      byte0_1 = temp1 & 0xff;
+      byte1_1 = (temp1 >> 8) & 0xff;
+      byte2_1 = (temp1 >> 16) & 0xff;
+      byte3_1 = (temp1 >> 24) & 0xff;
+#if defined (HW_ENV)
+      nc_printf("DQS0 %x, %x, %x, %x dqs0_dly:%x \n", byte0_0, byte2_0, byte0_1, byte2_1, dqs0_dly);
+#endif
+      if (low_eye_found != 1) {
+	if ((byte0_0 == 0x5a) && (byte2_0 == 0xa5) && (byte0_1 == 0x5a) && (byte2_1 == 0xa5)) {
+	  low_eye_found = 1;
+	  //nc_printf("DQS0 low_eye_found: %x, dqs0_dly_low:%x \n", low_eye_found, dqs0_dly_low);
+	  dqs0_dly_low = dqs0_dly;
+	} else {
+	  dqs0_dly = dqs0_dly + 1;
+	  //prog inc dqs0_dly
+	  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000200));
+	  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+	  DELAY(100);
+	}
+      } else if (high_eye_found != 1) {
+	if ((byte0_0 != 0x5a) || (byte2_0 != 0xa5) || (byte0_1 != 0x5a) || (byte2_1 != 0xa5)) {
+	  //nc_printf("temp0: %x, %x \n", byte0_0, byte2_0);
+	  //nc_printf("temp1: %x, %x \n", byte0_1, byte2_1);
+	  high_eye_found = 1;
+	  dqs0_dly_high = dqs0_dly;
+	  //nc_printf("DQS1 high_eye_found: %x, dqs0_dly_high:%x  \n", high_eye_found, dqs0_dly_high);
+	} else {
+	  dqs0_dly_high = dqs0_dly;
+	  dqs0_dly = dqs0_dly + 1;
+	  //prog inc dqs0_dly
+	  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000200));
+	  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+	  DELAY(100);
+	}
+      }
+    }
+    else
+      {
+	dqs0_dly = 63;
+      }
+  }
+
+  dqs0_dly_mid = dqs0_dly_high + dqs0_dly_low;
+  dqs0_dly_mid = (dqs0_dly_mid >> 1);
+  // reset dqs0 idely
+  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000100));
+  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+  // program idelay to mid value
+  for (i = 0; i <= dqs0_dly_mid; i = i+1) {
+    WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000200));
+    WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+    DELAY(100);
+  }
+#if defined (HW_ENV)
+  nc_printf("FPGA INFO0 low:%x, high: %x, mid: %x, low_eye_found: %x, high_eye_found: %x\n", dqs0_dly_low, dqs0_dly_high, dqs0_dly_mid, low_eye_found, high_eye_found);
+#endif
+
+  
+  //DQS1
+  dqs0_dly_low = 0;
+  dqs0_dly_high = 0;
+  dqs0_dly = 0;
+  dqs0_dly_mid = 0;
+  low_eye_found = 0;
+  high_eye_found = 0;
+  while (dqs0_dly < 62) {
+    if ((low_eye_found != 1) || (high_eye_found != 1)) {
+      temp = READ_REG32(0x80000000);
+      temp1 = READ_REG32(0x80000000+4);
+      //nc_printf("DQS1: temp:%x , temp1:%x , dqs0_dly:%d \n", temp, temp1, dqs0_dly);
+      
+      byte0_0 = temp & 0xff;
+      byte1_0 = (temp >> 8) & 0xff;
+      byte2_0 = (temp >> 16) & 0xff;
+      byte3_0 = (temp >> 24) & 0xff;
+      
+      byte0_1 = temp1 & 0xff;
+      byte1_1 = (temp1 >> 8) & 0xff;
+      byte2_1 = (temp1 >> 16) & 0xff;
+      byte3_1 = (temp1 >> 24) & 0xff;
+#if defined (HW_ENV)
+      nc_printf("DQS1 %x, %x, %x, %x dqs0_dly:%x \n", byte1_0, byte3_0, byte1_1, byte3_1, dqs0_dly);
+#endif
+      if (low_eye_found != 1) {
+	if ((byte1_0 == 0x5a) && (byte3_0 == 0xa5) && (byte1_1 == 0x5a) && (byte3_1 == 0xa5)) {
+	  low_eye_found = 1;
+	  dqs0_dly_low = dqs0_dly;
+	  //nc_printf("DQS1 low_eye_found: %x, dqs0_dly_low:%x \n", low_eye_found, dqs0_dly_low);
+	} else {
+	  dqs0_dly = dqs0_dly + 1;
+	  //prog inc dqs0_dly
+	  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00002000));
+	  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+	  DELAY(100);
+	}
+      } else if (high_eye_found != 1) {
+	if ((byte1_0 != 0x5a) || (byte3_0 != 0xa5) || (byte1_1 != 0x5a) || (byte3_1 != 0xa5)) {
+	  //nc_printf("temp0: %x, %x \n", byte1_0,byte3_0);
+	  //nc_printf("temp1: %x, %x \n", byte1_1, byte3_1);
+	  high_eye_found = 1;
+	  dqs0_dly_high = dqs0_dly;
+	  //nc_printf("DQS1 high_eye_found: %x, dqs0_dly_high:%x \n", high_eye_found, dqs0_dly_high);
+        }
+        else {
+	  dqs0_dly = dqs0_dly + 1;
+	  //prog inc dqs0_dly
+	  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00002000));
+	  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+	  DELAY(100);
+	}
+      }
+    }
+    else
+      {
+	dqs0_dly = 63;
+      }
+  }
+
+  dqs0_dly_mid = (dqs0_dly_high + dqs0_dly_low) >> 1;
+  // reset dqs1 idely
+  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00001000));
+  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+  // program dqs1 idelay to mid value
+  for (i = 0; i <= dqs0_dly_mid; i = i+1) {
+    WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00002000));
+    WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+    DELAY(100);
+  }
+#if defined (HW_ENV)
+  nc_printf("FPGA INFO1 low:%x, high: %x, mid: %x, low_eye_found: %x, high_eye_found: %x\n", dqs0_dly_low, dqs0_dly_high, dqs0_dly_mid, low_eye_found, high_eye_found);
+#endif
+
+#else
+  // reset dqs0 and dqs1 idely
+  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00001100));
+  WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+  // program dqs0 idelay to mid value
+  temp = 0x1a;
+#if defined (HW_ENV)
+  nc_printf("low:%x \n", temp);
+#endif
+  for (i = 0; i <= temp; i = i+1) {
+    WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000200));
+    WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+    DELAY(100);
+  }
+  // program dqs1 idelay to mid value
+  temp = 0x1a;
+#if defined (HW_ENV)
+  nc_printf("high: %x \n", temp);
+#endif
+  for (i = 0; i <= temp; i = i+1) {
+    WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00002000));
+    WRITE_REG32(FPGA_IDLY_CTRL, ((READ_REG32(FPGA_IDLY_CTRL) & 0x0) | 0x00000000));
+    DELAY(100);
+  }
+
+#endif
+
+
+
+
+    WRITE_REG32(FPGA_PHY_RD_LATENCY, ((READ_REG32(FPGA_PHY_RD_LATENCY) & ~0x00000010) | 0x10)); //r_fifo_cntr_reset = 1
+    WRITE_REG32(FPGA_PHY_RD_LATENCY, ((READ_REG32(FPGA_PHY_RD_LATENCY) & ~0x00000010) | 0x00)); //r_fifo_cntr_reset = 0
+
+
+  /**** All interrupts cleared ****/
+  *(tPVU32)(MC_REG_ADDRESS44) = 0xffffffff;
+  *(tPVU32)(MC_REG_ADDRESS50) = 0xffffffff;
+  *(tPVU32)(MC_REG_ADDRESS6C) = 0xffffffff; 
+  *(tPVU32)(MC_REG_ADDRESS08) = (*(tPVU32)(MC_REG_ADDRESS08) | 0x00000418);
+
+  //FOR ECC MODE
+//  temp = *(tPVU32)(MC_REG_ADDRESS08);
+//  temp = temp & ~0x000000ff;
+//  temp = temp | 0x00000067;
+//  *(tPVU32)(MC_REG_ADDRESS08) = temp;
+//
+//  temp = *(tPVU32)(MC_REG_ADDRESS00);
+//  temp = temp & ~0x00000008;
+//  temp = temp | 0x00000008;
+//  *(tPVU32)(MC_REG_ADDRESS00) = temp;
+//  while((*(tPVU32)(MC_REG_ADDRESS00) & 0x08) != 0);
+  //
+
+  *(tPVU32)(MC_REG_ADDRESS28) = (*(tPVU32)(MC_REG_ADDRESS28) | 0x01004900); 
+  *(tPVU32)(MC_REG_ADDRESS40) = (*(tPVU32)(MC_REG_ADDRESS40) | 0x00010000);
+ 
+
+  temp = *(tPVU32)(MC_REG_ADDRESS00);
+  temp = temp & ~0x00000200;
+  temp = temp | 0x00000200;
+  *(tPVU32)(MC_REG_ADDRESS00) = temp;
+
+  DELAY(5);
+
+#if 0
+
+  tU32 test_addr, data, cnt, err_bit;
+    test_addr = 0x80000000;
+    cnt = 0;
+  WRITE_REG32(MC_REG_ADDRESS00, ((READ_REG32(MC_REG_ADDRESS00) & ~0x8) | 0x8));
+  while((*(tPVU32)(MC_REG_ADDRESS00) & 0x08) != 0);
+  while (cnt != 100) {
+
+    data = 0x12345678 + test_addr + data;
+    WRITE_REG32(test_addr, data);
+    temp = READ_REG32(test_addr);
+    if (temp != data) {
+      err_bit = temp ^ data;
+#if defined (HW_ENV)
+      nc_printf("cnt:%d  addr:%x  rcv data:%x  exp data:%x err_bit:%x\n", cnt, test_addr, temp, data, err_bit);
+#endif
+      //return 0;
+      //break;
+    }
+    test_addr = test_addr + 0x4;
+    data = 0xdeadbeef + test_addr + data;
+    WRITE_REG32(test_addr, data);
+    temp = READ_REG32(test_addr);
+    if (temp != data) {
+      err_bit = temp ^ data;
+#if defined (HW_ENV)
+      nc_printf("cnt:%d  addr:%x  rcv data:%x  exp data:%x err_bit:%x\n", cnt, test_addr, temp, data, err_bit); 
+#endif
+     //return 0;
+      //break;
+    }
+    test_addr = test_addr + 0x4;
+    data = 0xbabeface + test_addr + data;
+    WRITE_REG32(test_addr, data);
+    temp = READ_REG32(test_addr);
+    if (temp != data) {
+      err_bit = temp ^ data;
+#if defined (HW_ENV)
+      nc_printf("cnt:%d  addr:%x  rcv data:%x  exp data:%x err_bit:%x\n", cnt, test_addr, temp, data, err_bit);
+#endif
+      //return 0;
+      //break;
+    }
+    test_addr = test_addr + 0x4;
+    data = 0xa5a55a5a + test_addr + data;
+    WRITE_REG32(test_addr, data);
+    temp = READ_REG32(test_addr);
+    if (temp != data) {
+      err_bit = temp ^ data;
+#if defined (HW_ENV)
+      nc_printf("cnt:%d  addr:%x  rcv data:%x  exp data:%x err_bit:%x\n", cnt, test_addr, temp, data, err_bit);
+#endif
+      //return 0;
+      //break;
+    }
+    test_addr = test_addr + 0x4;
+    
+    cnt = cnt + 1;
+  }
+
+  WRITE_REG32(MC_REG_ADDRESS00, ((READ_REG32(MC_REG_ADDRESS00) & ~0x8) | 0x8));
+  while((*(tPVU32)(MC_REG_ADDRESS00) & 0x08) != 0);
+  WRITE_REG32(0x80000000, 0xffffffff);
+  temp = READ_REG32(0x80000000);
+  WRITE_REG32((0x80000000+4), 0xffffffff);
+  temp = READ_REG32(0x80000000+4);
+  WRITE_REG32((0x80000000+8), 0xffffffff);
+  temp = READ_REG32(0x80000000+8);
+  WRITE_REG32((0x80000000+12), 0xffffffff);
+  temp = READ_REG32(0x80000000+12);
+
+  WRITE_REG32(0x80000000+16, 0x0);
+  temp = READ_REG32(0x80000000+16);
+  WRITE_REG32((0x80000000+20), 0xffffffff);
+  temp = READ_REG32(0x80000000+20);
+  WRITE_REG32((0x80000000+24), 0x0);
+  temp = READ_REG32(0x80000000+24);
+  WRITE_REG32((0x80000000+28), 0xffffffff);
+  temp = READ_REG32(0x80000000+28);
+
+  WRITE_REG32(0x80000000+32, 0xffffffff);
+  temp = READ_REG32(0x80000000+32);
+  WRITE_REG32((0x80000000+36), 0x0);
+  temp = READ_REG32(0x80000000+36);
+  WRITE_REG32((0x80000000+40), 0xffffffff);
+  temp = READ_REG32(0x80000000+40);
+  WRITE_REG32((0x80000000+44), 0x0);
+  temp = READ_REG32(0x80000000+44);
+
+#endif
+
+
+  //shutoff mcss register clk
+  *(tPVU32)(SYS_MCSS_CLK_CTRL) = 0xA000;
+#if defined (HW_ENV)
+  nc_printf("DDR INIT DONE\n");
+  return 0;
+#endif 
+  
+
+} //ddrinit
+
+#else
+
+// ASIC SIM ddr init
+tU32 ddrinit(tU8 ddr_type){
+  tU32 upper_dword, lower_dword;
+  tU8 dqscgate0, dqscgate1, otheroft, ienoft, Vrefloopindex, Vrefloopmax, Vrefstarta, ddr_freq;
+  tU32 Vrefdqsrange_best, Vrefdqs_best, Vref144Best;
+  tU8 not_found, min_passing_vref, max_passing_vref;
+
+  //PHY RESET
+  *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & 0xfffffdff) | 0x00000200;
+  // ddr_type 0 - DDR3. 1 - DDR4. 
+#if defined (HW_ENV)
+  nc_printf("\nDDR Initialization Routine Version 1.0 %s %s\n",__DATE__,__TIME__);
+  if (ddr_type == 1) {
+    nc_printf("!!! DDR4 Initialization Started: ");
+  } 
+  else {
+    nc_printf("!!! DDR3 Initialization Started: ");
+  } 
+#if defined (DDR3_75_125MHZ)
+  nc_printf("Frequency=100Mbps !!! \n");
+#endif
+#if defined (DDR3_200_250MHZ)
+  nc_printf("Frequency=200Mbps !!! \n");
+#endif
+#if defined (DDR3_350_400MHZ)
+  nc_printf("Frequency=400Mbps !!! \n");
+#endif
+#if defined (DDR3_500MHZ)
+  nc_printf("Frequency=500Mbps !!! \n");
+#endif
+#if defined (DDR3_800MHZ)
+  nc_printf("Frequency=800Mbps !!! \n");
+#endif
+#if defined (DDR3_1066MHZ)
+  nc_printf("Frequency=1000Mbps !!! \n");
+#endif
+#if defined (DDR3_1333MHZ)
+  nc_printf("Frequency=1300Mbps !!! \n");
+#endif
+#if defined (DDR3_1600MHZ)
+  nc_printf("Frequency=1600Mbps !!! \n");
+#endif
+
+#endif
+  
+  *(tPVU32)(MC_REG_ADDRESS80) = (*(tPVU32)(MC_REG_ADDRESS80) & 0xfffff000) | 0x00000fff;
+
+  // PHY related. DFIDRAMCLKDISABLE = 1'b1. and odd_wr_latency - bit 9, odd_rd_latency - bit 8.
+#if defined (WL5RL6)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xfffffcf7) | 0x00000208;
+#endif
+#if defined (WL6RL6)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xfffffcf7) | 0x00000108;
+#endif
+#if defined (WL6RL7)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xfffffcf7) | 0x00000108;
+#endif
+#if defined (WL7RL9)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xfffffcf7) | 0x00000308;
+#endif
+#if defined (WL8RL11)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xfffffcf7) | 0x00000108;
+#endif
+
+#if defined (WL9RL10)
+#if defined (DLL_OFF)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xfffffcf7) | 0x00000308;
+#else
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xfffffcf7) | 0x00000208;
+#endif
+#endif
+
+#if defined (WL11RL12)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xfffffcf7) | 0x00000208;
+#endif
+
+
+  DELAY(5);
+  // DDR technology.
+#if defined (DDR4)
+  //*(tPVU32)(APB_REG_ADDRESS00) = (*(tPVU32)(APB_REG_ADDRESS00) & 0x00000000) | 0x803b0301; // DBI Not used.
+  *(tPVU32)(APB_REG_ADDRESS00) = (*(tPVU32)(APB_REG_ADDRESS00) & 0x00000000) | 0x80330301; 
+#endif
+#if defined (DDR3)
+  *(tPVU32)(APB_REG_ADDRESS00) = (*(tPVU32)(APB_REG_ADDRESS00) & 0x00000000) | 0x80210201; 
+#endif
+#if defined (DDR3L)
+  *(tPVU32)(APB_REG_ADDRESS00) = (*(tPVU32)(APB_REG_ADDRESS00) & 0x00000000) | 0x80210201;
+#endif
+
+#if defined (DDR3_75_125MHZ) 
+  *(tPVU32)(APB_REG_ADDRESS144) = 0x0000c9c9;
+  *(tPVU32)(APB_REG_ADDRESS270) = 0x0000c9c9;
+#endif
+#if defined (DDR3_200_250MHZ) 
+  *(tPVU32)(APB_REG_ADDRESS144) = 0x0000c9c9;
+  *(tPVU32)(APB_REG_ADDRESS270) = 0x0000c9c9;
+#endif
+  // DLL Setting.
+#if defined (DDR3_75_125MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0x00000000) | 0xe0318800;
+  //*(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0x00000000) | 0xe0218801;
+  ddr_freq = 7;
+#endif
+#if defined (DDR3_200_250MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0x00000000) | 0xe0318800;
+  ddr_freq = 6;
+#endif
+#if defined (DDR3_350_400MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0x00000000) | 0xe0000007;
+  ddr_freq = 5;
+#endif
+#if defined (DDR3_500MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0x00000000) | 0xe0000005;
+  ddr_freq = 4;
+#endif
+#if defined (DDR3_800MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0x00000000) | 0x00000007;
+  ddr_freq = 3;
+#endif
+#if defined (DDR3_1066MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0x00000000) | 0x00000005;
+  ddr_freq = 2;
+#endif
+#if defined (DDR3_1333MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0x00000000) | 0x00000003;
+  ddr_freq = 1;
+#endif
+#if defined (DDR3_1600MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0x00000000) | 0x00000001;
+  ddr_freq = 0;
+#endif
+
+  // ZQ cal setting.
+#if defined (DDR3_75_125MHZ)
+*(tPVU32)(APB_REG_ADDRESS08) = (*(tPVU32)(APB_REG_ADDRESS08) & 0x00000000) | 0x00000186;
+#endif
+#if defined (DDR3_200_250MHZ)
+*(tPVU32)(APB_REG_ADDRESS08) = (*(tPVU32)(APB_REG_ADDRESS08) & 0x00000000) | 0x00000186;
+#endif
+#if defined (DDR3_350_400MHZ)
+*(tPVU32)(APB_REG_ADDRESS08) = (*(tPVU32)(APB_REG_ADDRESS08) & 0x00000000) | 0x00000166;
+#endif
+#if defined (DDR3_500MHZ)
+*(tPVU32)(APB_REG_ADDRESS08) = (*(tPVU32)(APB_REG_ADDRESS08) & 0x00000000) | 0x00000166;
+#endif
+#if defined (DDR3_800MHZ)
+  *(tPVU32)(APB_REG_ADDRESS08) = (*(tPVU32)(APB_REG_ADDRESS08) & 0x00000000) | 0x00000176;
+#endif
+#if defined (DDR3_1066MHZ)
+  *(tPVU32)(APB_REG_ADDRESS08) = (*(tPVU32)(APB_REG_ADDRESS08) & 0x00000000) | 0x00000176;
+#endif
+#if defined (DDR3_1333MHZ)
+  *(tPVU32)(APB_REG_ADDRESS08) = (*(tPVU32)(APB_REG_ADDRESS08) & 0x00000000) | 0x00000186;
+#endif
+#if defined (DDR3_1600MHZ)
+  *(tPVU32)(APB_REG_ADDRESS08) = (*(tPVU32)(APB_REG_ADDRESS08) & 0x00000000) | 0x00000086;
+#endif
+
+  // Remove MDLL reset. Moved from below to save Boot time. SS.
+#if defined (DDR3_75_125MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0xfffffffe);
+#endif
+#if defined (DDR3_200_250MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0xfffffffe);
+#endif
+#if defined (DDR3_350_400MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0xfffffffe);
+#endif
+#if defined (DDR3_500MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0xfffffffe);
+#endif
+#if defined (DDR3_800MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0xfffffffe);
+#endif
+#if defined (DDR3_1066MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0xfffffffe);
+#endif
+#if defined (DDR3_1333MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0xfffffffe);
+#endif
+#if defined (DDR3_1600MHZ)
+  *(tPVU32)(APB_REG_ADDRESS04) = (*(tPVU32)(APB_REG_ADDRESS04) & 0xfffffffe);
+#endif
+
+  // I/O buffer control ODT.
+#if defined (DDR4)
+  *(tPVU32)(APB_REG_ADDRESS0C) = (*(tPVU32)(APB_REG_ADDRESS0C) & 0x00000000) | 0x00000237;
+#else
+  *(tPVU32)(APB_REG_ADDRESS0C) = (*(tPVU32)(APB_REG_ADDRESS0C) & 0x00000000) | 0x00000237;
+#endif
+
+#if defined (DDR3)
+  *(tPVU32)(APB_REG_ADDRESS0C) = (*(tPVU32)(APB_REG_ADDRESS0C) & 0xfffffffe); // Turn off BENSTAT.
+#endif
+
+  // Execute ZQ calibration only for PLLCLK > 200. Moved from below to save Boot time. SS.
+#if defined (DDR3_75_125MHZ)
+  *(tPVU32)(APB_REG_ADDRESS300) = 0xa5000000;
+  *(tPVU32)(APB_REG_ADDRESS308) = (*(tPVU32)(APB_REG_ADDRESS308) & 0xfff00000) | 0x00018001;
+#if defined (DDR4)
+  *(tPVU32)(APB_REG_ADDRESS30C) = (*(tPVU32)(APB_REG_ADDRESS30C) & 0xff000000) | 0x00A11533;
+#endif
+#if defined (DDR3L)
+  *(tPVU32)(APB_REG_ADDRESS30C) = (*(tPVU32)(APB_REG_ADDRESS30C) & 0xff000000) | 0x00A11634;
+#endif
+#if defined (DDR3)
+  *(tPVU32)(APB_REG_ADDRESS30C) = (*(tPVU32)(APB_REG_ADDRESS30C) & 0xff000000) | 0x00A11835;
+#endif
+#else
+#if defined (DDR3_200_250MHZ)
+  *(tPVU32)(APB_REG_ADDRESS300) = 0xa5000000;
+#if defined (DDR4)
+  *(tPVU32)(APB_REG_ADDRESS30C) = (*(tPVU32)(APB_REG_ADDRESS30C) & 0xff000000) | 0x00A11533;
+#endif
+#if defined (DDR3L)
+  *(tPVU32)(APB_REG_ADDRESS30C) = (*(tPVU32)(APB_REG_ADDRESS30C) & 0xff000000) | 0x00A11634;
+#endif
+#if defined (DDR3)
+  *(tPVU32)(APB_REG_ADDRESS30C) = (*(tPVU32)(APB_REG_ADDRESS30C) & 0xff000000) | 0x00A11835;
+#endif
+  *(tPVU32)(APB_REG_ADDRESS308) = (*(tPVU32)(APB_REG_ADDRESS308) & 0xfff00000) | 0x00018001;
+#else
+  *(tPVU32)(APB_REG_ADDRESS08) = (*(tPVU32)(APB_REG_ADDRESS08) & 0xfffffffe) | 0x00000001;
+#endif
+#endif
+
+  // ODT timing. Will change after gate training for OFT.
+#if defined (DDR3_75_125MHZ)
+  *(tPVU32)(APB_REG_ADDRESS10) = (*(tPVU32)(APB_REG_ADDRESS10) & 0x00000000) | 0x44344433;
+#endif
+#if defined (DDR3_200_250MHZ)
+  *(tPVU32)(APB_REG_ADDRESS10) = (*(tPVU32)(APB_REG_ADDRESS10) & 0x00000000) | 0x44344433;
+#endif
+#if defined (DDR3_350_400MHZ)
+  *(tPVU32)(APB_REG_ADDRESS10) = (*(tPVU32)(APB_REG_ADDRESS10) & 0x00000000) | 0x64546433;
+#endif
+#if defined (DDR3_500MHZ)
+  *(tPVU32)(APB_REG_ADDRESS10) = (*(tPVU32)(APB_REG_ADDRESS10) & 0x00000000) | 0x64546433;
+#endif
+#if defined (DDR3_800MHZ)
+  *(tPVU32)(APB_REG_ADDRESS10) = (*(tPVU32)(APB_REG_ADDRESS10) & 0x00000000) | 0x94749443;
+#endif
+#if defined (DDR3_1066MHZ)
+  *(tPVU32)(APB_REG_ADDRESS10) = (*(tPVU32)(APB_REG_ADDRESS10) & 0x00000000) | 0xa585a544;
+#endif
+#if defined (DDR3_1333MHZ)
+  *(tPVU32)(APB_REG_ADDRESS10) = (*(tPVU32)(APB_REG_ADDRESS10) & 0x00000000) | 0xc696c645;
+#endif
+#if defined (DDR3_1600MHZ)
+  *(tPVU32)(APB_REG_ADDRESS10) = (*(tPVU32)(APB_REG_ADDRESS10) & 0x00000000) | 0xe0a0e050;
+#endif
+
+#if defined (WL5RL6)
+  *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0x00000000) | 0x000000b0;
+#endif
+#if defined (WL6RL6)
+  *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0x00000000) | 0x000000b0;
+#endif
+#if defined (WL6RL7)
+  *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0x00000000) | 0x000000b1;
+#endif
+#if defined (WL7RL9)
+  *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0x00000000) | 0x000000b1;
+#endif
+#if defined (WL8RL11)
+  *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0x00000000) | 0x000000b1;
+#endif
+#if defined (WL9RL10)
+  *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0x00000000) | 0x000000b1;
+#endif
+#if defined (WL11RL12)
+  *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0x00000000) | 0x000000b1;
+#endif
+
+  // IO enable etc setting.
+  *(tPVU32)(APB_REG_ADDRESS1C) = (*(tPVU32)(APB_REG_ADDRESS1C) & 0x00000000) | 0x00000006;
+
+  // ZQODT Control.
+#if defined (DDR4)
+  *(tPVU32)(APB_REG_ADDRESS20) = (*(tPVU32)(APB_REG_ADDRESS20) & 0x00000000) | 0x02465555;
+#else
+  *(tPVU32)(APB_REG_ADDRESS20) = (*(tPVU32)(APB_REG_ADDRESS20) & 0x00000000) | 0x02555555;
+#endif
+
+  // DQS Vref code setting.
+#if defined (DDR4)
+  *(tPVU32)(APB_REG_ADDRESS28) = (*(tPVU32)(APB_REG_ADDRESS28) & 0x00000000) | 0x0065019a;
+  *(tPVU32)(APB_REG_ADDRESS20C) = (*(tPVU32)(APB_REG_ADDRESS20C) & 0x00000000) | 0x0065019a;
+  *(tPVU32)(APB_REG_ADDRESS210) = (*(tPVU32)(APB_REG_ADDRESS210) & 0x00000000) | 0x0065019a;
+#else
+  *(tPVU32)(APB_REG_ADDRESS28) = (*(tPVU32)(APB_REG_ADDRESS28) & 0x00000000) | 0x00ff0100;
+  *(tPVU32)(APB_REG_ADDRESS20C) = (*(tPVU32)(APB_REG_ADDRESS20C) & 0x00000000) | 0x80b78148;
+  *(tPVU32)(APB_REG_ADDRESS210) = (*(tPVU32)(APB_REG_ADDRESS210) & 0x00000000) | 0x80b78148;  
+  //#endif
+#endif
+
+  // Hi-Z mask initialization for DQS gating. For now GL mode only with training.
+#if defined (DDR4)
+  *(tPVU32)(APB_REG_ADDRESS30) = (*(tPVU32)(APB_REG_ADDRESS30) & 0x00000000) | 0x80180702; // GL
+#else
+  *(tPVU32)(APB_REG_ADDRESS30) = (*(tPVU32)(APB_REG_ADDRESS30) & 0x00000000) | 0x80180702; // GL
+#endif
+
+  // Write leveling. 
+  *(tPVU32)(APB_REG_ADDRESS40) = (*(tPVU32)(APB_REG_ADDRESS40) & 0x00000000);
+
+  // CDLY Setting.
+  *(tPVU32)(APB_REG_ADDRESS50) = (*(tPVU32)(APB_REG_ADDRESS50) & 0x00000000) | 0x00000101;
+
+  // FIFO pointer reset. Clock skew <= 1DFICLK. 
+#if defined (AUTO_DQCALIB)
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010100;
+#else
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+#endif
+
+  // Auto DQ calib.
+#if defined (AUTO_DQCALIB)
+  *(tPVU32)(APB_REG_ADDRESS60) = (*(tPVU32)(APB_REG_ADDRESS60) & 0x00000000) | 0x00100000;
+  *(tPVU32)(APB_REG_ADDRESS68) = (*(tPVU32)(APB_REG_ADDRESS68) & 0x00000000) | 0x05300000;
+  *(tPVU32)(APB_REG_ADDRESS70) = (*(tPVU32)(APB_REG_ADDRESS70) & 0x00000000) | 0x04180000;
+  *(tPVU32)(APB_REG_ADDRESS78) = (*(tPVU32)(APB_REG_ADDRESS78) & 0x00000000) | 0x04380000;
+  *(tPVU32)(APB_REG_ADDRESS80) = (*(tPVU32)(APB_REG_ADDRESS80) & 0x00000000) | 0x05100000;
+  *(tPVU32)(APB_REG_ADDRESS88) = (*(tPVU32)(APB_REG_ADDRESS88) & 0x00000000) | 0x04300000;
+  *(tPVU32)(APB_REG_ADDRESSB0) = (*(tPVU32)(APB_REG_ADDRESSB0) & 0x00000000) | 0x05380000;
+  *(tPVU32)(APB_REG_ADDRESSD0) = (*(tPVU32)(APB_REG_ADDRESSD0) & 0x00000000) | 0x03110f01;
+  *(tPVU32)(APB_REG_ADDRESSD4) = (*(tPVU32)(APB_REG_ADDRESSD4) & 0x00000000) | 0x07050b12;
+  *(tPVU32)(APB_REG_ADDRESSD8) = (*(tPVU32)(APB_REG_ADDRESSD8) & 0x00000000) | 0x0c000d08;
+  *(tPVU32)(APB_REG_ADDRESSDC) = (*(tPVU32)(APB_REG_ADDRESSDC) & 0x00000000) | 0x00041000;
+  *(tPVU32)(APB_REG_ADDRESSE0) = (*(tPVU32)(APB_REG_ADDRESSE0) & 0x00000000) | 0x00aa00aa;
+  *(tPVU32)(APB_REG_ADDRESSE4) = (*(tPVU32)(APB_REG_ADDRESSE4) & 0x00000000) | 0x00000000;
+  *(tPVU32)(APB_REG_ADDRESSE8) = (*(tPVU32)(APB_REG_ADDRESSE8) & 0x00000000) | 0x00aa00aa;
+  *(tPVU32)(APB_REG_ADDRESSEC) = (*(tPVU32)(APB_REG_ADDRESSEC) & 0x00000000) | 0x00ff0000;
+  *(tPVU32)(APB_REG_ADDRESS120) = (*(tPVU32)(APB_REG_ADDRESS120) & 0x00000000) | 0x00000003;
+  *(tPVU32)(APB_REG_ADDRESS124) = (*(tPVU32)(APB_REG_ADDRESS124) & 0x00000000) | 0x00000201;
+  *(tPVU32)(APB_REG_ADDRESS128) = (*(tPVU32)(APB_REG_ADDRESS128) & 0x00000000) | 0x03060605;
+  *(tPVU32)(APB_REG_ADDRESS12C) = (*(tPVU32)(APB_REG_ADDRESS12C) & 0x00000000) | 0x17060e63;
+  *(tPVU32)(APB_REG_ADDRESS130) = (*(tPVU32)(APB_REG_ADDRESS130) & 0x00000000) | 0x0002050a;
+  *(tPVU32)(APB_REG_ADDRESS13C) = (*(tPVU32)(APB_REG_ADDRESS13C) & 0x00000000) | 0x00000000;
+  *(tPVU32)(APB_REG_ADDRESS20C) = (*(tPVU32)(APB_REG_ADDRESS20C) & 0x00000000) | 0x80b78148;
+  *(tPVU32)(APB_REG_ADDRESS210) = (*(tPVU32)(APB_REG_ADDRESS210) & 0x00000000) | 0x80b78148;
+#endif
+
+  // Clearing DQ calibration execution control.
+#if defined (DDR4)
+  *(tPVU32)(APB_REG_ADDRESS124) = (*(tPVU32)(APB_REG_ADDRESS124) & 0x00000000) | 0x00010001;
+  *(tPVU32)(APB_REG_ADDRESS128) = (*(tPVU32)(APB_REG_ADDRESS128) & 0x00000000) | 0x03060c0b;
+#else
+#if defined (AUTO_DQCALIB)
+#else
+  *(tPVU32)(APB_REG_ADDRESS124) = (*(tPVU32)(APB_REG_ADDRESS124) & 0x00000000);
+#endif
+#endif
+
+  #if defined (DDR3_200_250MHZ)
+  #else
+  #if defined (DDR3_75_125MHZ)
+  #else
+  while((READ_REG32(PHY_REG_ADDRESS04) & 0x40) != 0x40);  // Poll for DFIINITCOMPLETE bit6 which includes ZQ calibration.
+  #endif
+  #endif
+
+  DELAY(5);
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xfffffff7); // DFIDRAMCLKDISABLE = 1'b0
+
+  DELAY(5);
+  // ADD/CMD output setting.
+  WRITE_REG32(APB_REG_ADDRESS1C, 0x00000007); 
+
+  DELAY(150); // Before ddrcfg determines the RESET de-assertion to DDR device.
+
+  // Type Programming.
+#if defined (DDR4)
+  // Type 0. MAX capacity and worst parameters for x16. Temporary 2.
+  *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & 0xf1ffffff) | 0x00000000;
+#else
+  // Type 6. MAX capacity and worst parameters for x16. DLL_OFF is type 2.
+#if defined (DLL_OFF)
+  *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & 0xf1ffffff) | 0x0c000000;
+#else
+  *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & 0xf1ffffff) | 0x0c000000;
+#endif  
+#endif
+
+  ddrcfg(ddr_type, ddr_freq);
+
+  // Write leveling. 
+  *(tPVU32)(APB_REG_ADDRESS40) = (*(tPVU32)(APB_REG_ADDRESS40) & 0x00000000);
+
+  // FIFO pointer initialization. Clock skew <= 1DFICLK.
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010100;
+
+  // DELAY(5);
+
+  // DDR4.
+
+  // VREF Training.
+  // Offset 28, 1F0, 1F4, 1FC, 200, 204, 208, 20C, 210.
+  // DDR4 Read Vref DQ calibration MPR mode. Training Routine.
+
+  // DDR4 Write Vref Training. Training routine.
+
+
+  // DDR4 Memory Setting, DBI, CL resetting.
+
+  // DDR4 PHY DFI, RL resetting.
+
+  // DDR4 Read Vref DQ calibration WR mode. Training routine.
+
+  // Reset the counters.
+  WRITE_REG32(MC_REG_ADDRESS00, ((READ_REG32(MC_REG_ADDRESS00) & 0xfffffffe) | 0x01));
+  WRITE_REG32(MC_REG_ADDRESS00, (READ_REG32(MC_REG_ADDRESS00) & 0xfffffffe));  
+
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xffffdfff) | 0x00002000;
+#if defined (DDR3_75_125MHZ)
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xfffff000) | 0x00000062;
+#endif
+#if defined (DDR3_200_250MHZ)
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xfffff000) | 0x000000c3;
+#endif
+#if defined (DDR3_350_400MHZ)
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xfffff000) | 0x00000186;
+#endif
+#if defined (DDR3_500MHZ)
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xfffff000) | 0x000001e8;
+#endif
+#if defined (DDR3_800MHZ)
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xfffff000) | 0x0000030c;
+#endif
+#if defined (DDR3_1066MHZ)
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xfffff000) | 0x000003cf;
+#endif
+#if defined (DDR3_1333MHZ)
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xfffff000) | 0x000004f4;
+#endif
+#if defined (DDR3_1600MHZ)
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xfffff000) | 0x00000618;
+#endif
+  *(tPVU32)(MC_REG_ADDRESS08) = (*(tPVU32)(MC_REG_ADDRESS08) & 0x0000f0ff) | 0xa8350300;
+  *(tPVU32)(MC_REG_ADDRESS0C) = (*(tPVU32)(MC_REG_ADDRESS0C) & 0xf001ffc0) | 0x06260022;
+  *(tPVU32)(MC_REG_ADDRESS14) = (*(tPVU32)(MC_REG_ADDRESS14) & 0x01f00000) | 0x5001b030;
+  *(tPVU32)(MC_REG_ADDRESS28) = (*(tPVU32)(MC_REG_ADDRESS28) & 0x03ff0fff) | 0x90000000;
+  *(tPVU32)(MC_REG_ADDRESS34) = (*(tPVU32)(MC_REG_ADDRESS34) & 0x80000000) | 0x42492248;
+  // MAGM.
+  *(tPVU32)(MC_REG_ADDRESS38) = (*(tPVU32)(MC_REG_ADDRESS38) & 0x00000000) | MAGM_VALUE;
+  // UAGM
+  *(tPVU32)(MC_REG_ADDRESS3C) = (*(tPVU32)(MC_REG_ADDRESS3C) & 0x00000000) | UAGM_VALUE;
+  *(tPVU32)(MC_REG_ADDRESS40) = (*(tPVU32)(MC_REG_ADDRESS40) & 0x80000000) | 0x40004d67;
+  *(tPVU32)(MC_REG_ADDRESS68) = (*(tPVU32)(MC_REG_ADDRESS68) & 0x40000000) | 0x1f517922;
+  *(tPVU32)(MC_REG_ADDRESS48) = (*(tPVU32)(MC_REG_ADDRESS48) & 0x00000000) | 0xffffffff;
+  *(tPVU32)(MC_REG_ADDRESS54) = (*(tPVU32)(MC_REG_ADDRESS54) & 0x00000000) | 0xffffffff;
+  *(tPVU32)(MC_REG_ADDRESS70) = (*(tPVU32)(MC_REG_ADDRESS70) & 0x00000000) | 0xffffffff;
+  // Lower ECC address
+  *(tPVU32)(MC_REG_ADDRESS58) = LOWER_ECC_ADDRESS;
+  // Upper ECC address
+  *(tPVU32)(MC_REG_ADDRESS5C) = UPPER_ECC_ADDRESS;
+
+//BOARD_DELAY_ONE_CLK. rd2wr delay for certain latencies.
+#if defined (WL6RL6)
+  *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & 0xffffeffff) | 0x00010000;
+#endif
+#if defined (WL7RL9)
+  *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & 0xffffeffff) | 0x00010000;
+#endif
+#if defined (WL9RL10)
+  *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & 0xffffeffff) | 0x00010000;
+#endif
+#if defined (WL11RL12)
+  *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & 0xffffeffff) | 0x00010000;
+#endif
+
+  //
+  *(tPVU32)(MC_REG_ADDRESS1C) = (*(tPVU32)(MC_REG_ADDRESS1C) & 0x40003fff) | 0xbc730000;
+
+
+#if defined (WL5RL6)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xffffe778) | 0x00001881;
+#endif
+#if defined (WL6RL6)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xffffe778) | 0x00001881;
+#endif
+#if defined (WL6RL7)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xffffe778) | 0x00001880;
+#endif
+#if defined (WL7RL9)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xffffe778) | 0x00001881;
+#endif
+#if defined (WL8RL11)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xffffe778) | 0x00001882;
+#endif
+#if defined (WL9RL10)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xffffe778) | 0x00001881; // DLL_OFF change??.
+#endif
+#if defined (WL11RL12)
+  *(tPVU32)(PHY_REG_ADDRESS00) = (*(tPVU32)(PHY_REG_ADDRESS00) & 0xffffe778) | 0x00001882;
+#endif
+  *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & 0xffffffef) | 0x00000010;
+
+  // Gate training.
+#if defined (SKIP_GATE_TRAINING)
+#if defined (DDR4)
+  *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0x00000000) | 0x04150415; // 1600 and board delay = 1. Actual is 441 with training.
+#else
+  // DDR3, 1600.
+  //*(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0x00000000) | 0x04580458; // 1600 and board delay = 1. Actual is 319 with training.
+  // DDR3, 800.
+  *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0x00000000) | 0x02100210; // 1600 and board delay = 1. Actual is 319 with training.
+#endif
+
+#else
+
+  // Read DQS Gate training.
+#if defined (DDR3_75_125MHZ)
+  *(tPVU32)(APB_REG_ADDRESS34) = 0x03600360; 
+#else
+#if defined (DDR3_200_250MHZ)
+  *(tPVU32)(APB_REG_ADDRESS34) = 0x03600360; 
+#else
+  if (gate_training()) {
+#if defined (HW_ENV)
+    nc_printf("!!! ERROR DQS preamble not found !!! \n");
+#endif
+  }
+#endif
+#endif
+#endif
+
+#if defined (DDR4)
+#if defined (DDR3_800MHZ)
+  *(tPVU32)(APB_REG_ADDRESS0C) = (*(tPVU32)(APB_REG_ADDRESS0C) & 0x00000000) | 0x00000207;
+#endif
+#if defined (DDR3_1066MHZ)
+  *(tPVU32)(APB_REG_ADDRESS0C) = (*(tPVU32)(APB_REG_ADDRESS0C) & 0x00000000) | 0x00000207;
+#endif
+#endif
+
+#if defined (DDR3_75_125MHZ)
+  *(tPVU32)(APB_REG_ADDRESS0C) = (*(tPVU32)(APB_REG_ADDRESS0C) & 0x00000000) | 0x00000207;
+#endif
+#if defined (DDR3_200_250MHZ)
+  *(tPVU32)(APB_REG_ADDRESS0C) = (*(tPVU32)(APB_REG_ADDRESS0C) & 0x00000000) | 0x00000207;
+#endif
+#if defined (DDR3_350_400MHZ)
+  *(tPVU32)(APB_REG_ADDRESS0C) = (*(tPVU32)(APB_REG_ADDRESS0C) & 0x00000000) | 0x00000207;
+#endif
+#if defined (DDR3_500MHZ)
+  *(tPVU32)(APB_REG_ADDRESS0C) = (*(tPVU32)(APB_REG_ADDRESS0C) & 0x00000000) | 0x00000207;
+#endif
+
+  //#if defined (DLL_OFF)
+  //#else
+  //#if defined (AUTO_DQCALIB)
+  //#else
+  // ODT timing change based on gate training for OFT.
+  // Max of the gate training coarse values from the two bytes. Reusing variables.
+  dqscgate0 = (*(tPVU32)(APB_REG_ADDRESS34) & 0x00000700) >> 8;
+  dqscgate1 = (*(tPVU32)(APB_REG_ADDRESS34) & 0x07000000) >> 24;
+  if (dqscgate1 > dqscgate0) {
+    dqscgate0 = dqscgate1;
+  }
+  if (dqscgate0 > 0) {
+    otheroft = dqscgate0;
+  } else {
+    otheroft = 0;
+  }
+  if (dqscgate0 > 1) {
+    ienoft = (dqscgate0 - 1);
+  } else {
+    ienoft = 0;
+  }
+  *(tPVU32)(APB_REG_ADDRESS10) = (*(tPVU32)(APB_REG_ADDRESS10) & 0xf0f0f0f0) | (otheroft << 8) | (otheroft << 16) | (otheroft << 24) | ienoft;
+  //*(tPVU32)(APB_REG_ADDRESS10) = (*(tPVU32)(APB_REG_ADDRESS10) & 0x00000000) | 0x9474b463;
+  //#endif
+  //#endif
+
+  *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & 0xffffffef);
+  // FIFO pointer initialization. Clock skew <= 1DFICLK. Reset pointers.
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+    
+  /**** All interrupts cleared ****/
+  *(tPVU32)(MC_REG_ADDRESS44) = 0xffffffff;
+  *(tPVU32)(MC_REG_ADDRESS50) = 0xffffffff;
+  *(tPVU32)(MC_REG_ADDRESS6C) = 0xffffffff; 
+  *(tPVU32)(MC_REG_ADDRESS08) = (*(tPVU32)(MC_REG_ADDRESS08) | 0x00000418);
+  *(tPVU32)(MC_REG_ADDRESS28) = (*(tPVU32)(MC_REG_ADDRESS28) | 0x01004900); 
+  *(tPVU32)(MC_REG_ADDRESS40) = (*(tPVU32)(MC_REG_ADDRESS40) | 0x00010000);
+
+  DELAY(150);
+  // Eye training.
+  // Read data is from Memory or MPR.
+#if defined (MPR_READ)
+      *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xffffdfff);
+      *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & 0xfffffbff) | 0x00000400;  
+      // MRS3. MPR mode.
+      *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00047404;
+      while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+      upper_dword = 0x01010000;
+      lower_dword = 0x01010000;
+#else
+      *(tPVU32)(0x80000000) = 0xa5a55a5a;
+      *(tPVU32)(0x80000004) = 0xa5a55a5a;
+      upper_dword = 0xa5a55a5a;
+      lower_dword = 0xa5a55a5a;
+#endif
+#if defined (SKIP_EYE_TRAINING)
+#if defined (DDR3_75_125MHZ)
+      *(tPVU32)(APB_REG_ADDRESS144) = 0x0000c9c9;
+      *(tPVU32)(APB_REG_ADDRESS270) = 0x0000c9c9;
+#endif
+#if defined (DDR3_200_250MHZ)
+      *(tPVU32)(APB_REG_ADDRESS144) = 0x0000c9c9;
+      *(tPVU32)(APB_REG_ADDRESS270) = 0x0000c9c9;
+#endif
+
+#else
+
+#if defined (DDR4)
+      Vrefloopmax = 50;
+      *(tPVU32)(APB_REG_ADDRESS20C) = (*(tPVU32)(APB_REG_ADDRESS20C) & 0x7fff7fff) | 0x80008000;
+      *(tPVU32)(APB_REG_ADDRESS210) = (*(tPVU32)(APB_REG_ADDRESS210) & 0x7fff7fff) | 0x80008000;
+      Vrefstarta = 140;
+#else
+      Vrefloopmax = 1;
+#endif
+      for (Vrefloopindex = 0; Vrefloopindex < Vrefloopmax; Vrefloopindex = Vrefloopindex + 1) {
+#if defined (DDR4)
+	*(tPVU32)(APB_REG_ADDRESS20C) = (*(tPVU32)(APB_REG_ADDRESS20C) & 0xfe00fe00) | VrefPN_array[Vrefstarta][0] | (VrefPN_array[Vrefstarta][1] << 16);
+	*(tPVU32)(APB_REG_ADDRESS210) = (*(tPVU32)(APB_REG_ADDRESS210) & 0xfe00fe00) | VrefPN_array[Vrefstarta][0] | (VrefPN_array[Vrefstarta][1] << 16);
+#endif
+        eye_training(upper_dword, lower_dword, ddr_type);
+        if (Vrefloopindex == 0) {
+          Vrefdqsrange_best = (*(tPVU32)(LMEMDDRTRAINVAR + 0x238) & 0xff);
+          Vrefdqs_best = Vrefstarta;
+          Vref144Best = *(tPVU32)(APB_REG_ADDRESS144);
+        }
+        else if ((*(tPVU32)(LMEMDDRTRAINVAR + 0x238) & 0xff) > Vrefdqsrange_best) {
+          Vrefdqsrange_best = (*(tPVU32)(LMEMDDRTRAINVAR + 0x238) & 0xff);
+          Vrefdqs_best = Vrefstarta;
+          Vref144Best = *(tPVU32)(APB_REG_ADDRESS144);
+        }
+        Vrefstarta = Vrefstarta + 1;
+      }
+#if defined (DDR4)
+      *(tPVU32)(APB_REG_ADDRESS20C) = (*(tPVU32)(APB_REG_ADDRESS20C) & 0xfe00fe00) | VrefPN_array[Vrefdqs_best][0] | (VrefPN_array[Vrefdqs_best][1] << 16);
+      *(tPVU32)(APB_REG_ADDRESS210) = (*(tPVU32)(APB_REG_ADDRESS210) & 0xfe00fe00) | VrefPN_array[Vrefdqs_best][0] | (VrefPN_array[Vrefdqs_best][1] << 16);
+      *(tPVU32)(APB_REG_ADDRESS144) = Vref144Best;
+      *(tPVU32)(LMEMDDRTRAINVAR + 0x23c) = Vrefdqsrange_best;
+#endif
+      // Range of less than 10 steps is considered as fail.
+      if (ddr_type == 1) {        
+#if defined (HW_ENV)
+	if (Vrefdqsrange_best > 10) {
+#if defined (HW_ENVPRN)
+	  nc_printf("!!! Best Range: %d, Vref index: %d, Vref Register 20C: %x,  Vref Register 210: %x, DQS Delay Register 144: %x !!! \n", Vrefdqsrange_best, Vrefdqs_best, (*(tPVU32)(APB_REG_ADDRESS20C)), (*(tPVU32)(APB_REG_ADDRESS210)), Vref144Best);
+#endif
+#if defined (DDR3_75_125MHZ)
+	  nc_printf("!!! Best Range After Read Vref Training (Read DQ eye) : (%d /64) * 10.0) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_200_250MHZ)
+	  nc_printf("!!! Best Range After Read Vref Training (Read DQ eye) : (%d /64) * 5.0) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_350_400MHZ)
+	  nc_printf("!!! Best Range After Read Vref Training (Read DQ eye) : (%d /64) * 2.5) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_500MHZ)
+	  nc_printf("!!! Best Range After Read Vref Training (Read DQ eye) : (%d /64) * 2.0) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_800MHZ)
+	  nc_printf("!!! Best Range After Read Vref Training (Read DQ eye) : (%d /64) * 1.25) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_1066MHZ)
+	  nc_printf("!!! Best Range After Read Vref Training (Read DQ eye) : (%d /64) * 1.0) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_1333MHZ)
+	  nc_printf("!!! Best Range After Read Vref Training (Read DQ eye) : (%d /64) * 0.769) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_1600MHZ)
+	  nc_printf("!!! Best Range After Read Vref Training (Read DQ eye) : (%d /64) * 0.625) ns !!! \n", Vrefdqsrange_best);
+#endif
+	}
+	else {
+	  nc_printf("!!! ERROR Read eye not found !!! \n");
+	}
+#endif 
+      }
+      else {
+#if defined (HW_ENV)
+	if (Vrefdqsrange_best > 10) {
+#if defined (HW_ENVPRN)
+	  nc_printf("!!! Range: %d, Vref Register 20C: %x,  Vref Register 210: %x, DQS Delay Register 144: %x !!! \n", Vrefdqsrange_best, (*(tPVU32)(APB_REG_ADDRESS20C)), (*(tPVU32)(APB_REG_ADDRESS210)), Vref144Best);
+#endif
+#if defined (DDR3_75_125MHZ)
+	  nc_printf("!!! Best Range (Read DQ eye) : (%d /64) * 10.0) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_200_250MHZ)
+	  nc_printf("!!! Best Range (Read DQ eye) : (%d /64) * 5.0) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_350_400MHZ)
+	  nc_printf("!!! Best Range (Read DQ eye) : (%d /64) * 2.5) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_500MHZ)
+	  nc_printf("!!! Best Range (Read DQ eye) : (%d /64) * 2.0) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_800MHZ)
+	  nc_printf("!!! Best Range (Read DQ eye) : (%d /64) * 1.25) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_1066MHZ)
+	  nc_printf("!!! Best Range (Read DQ eye) : (%d /64) * 1.0) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_1333MHZ)
+	  nc_printf("!!! Best Range (Read DQ eye) : (%d /64) * 0.769) ns !!! \n", Vrefdqsrange_best);
+#endif
+#if defined (DDR3_1600MHZ)
+	  nc_printf("!!! Best Range (Read DQ eye) : (%d /64) * 0.625) ns !!! \n", Vrefdqsrange_best);
+#endif
+	}
+	else {
+	  nc_printf("!!! ERROR Read eye not found !!! \n");
+	}
+#endif 
+      }
+#endif
+
+#if defined (MPR_READ)
+      *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & 0xfffffbff);
+      *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00007404;
+      while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+      *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xffffdfff) | 0x00002000;
+#endif 
+
+#if defined (AUTO_DQCALIB)
+  *(tPVU32)(APB_REG_ADDRESS124) = (*(tPVU32)(APB_REG_ADDRESS124) & 0x00000000) | 0x00000211;
+  while(((*(tPVU32)(APB_REG_ADDRESS124)) & 0x40000000) != 0x40000000);
+#endif
+
+  // FIFO training. Start with 6. Working.
+  // Read data is from write data.
+#if defined (SKIP_FIFO_TRAINING)
+    *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0xffffff0f) | (0x6 << 4);
+#else
+  if (fifo_training(0x9)) {
+#if defined (HW_ENV)
+    nc_printf("!!! ERROR FIFO Training failed !!! \n");
+#endif
+  }
+#endif
+ 
+  // All banks in idle.
+  DELAY(350);
+
+#if defined (DDR4)
+  // Vref training for the device.
+#if defined (SKIP_DEVICE_VREF_TRAINING)
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xffffdfff);
+  // MRS6. Range1, Value=0x17.
+  *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x0417d404;
+  while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+  *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & 0xffffdfff) | 0x00002000;
+#else
+  Vrefloopmax = 30;
+  Vrefstarta =  0;
+  *(tPVU32)(0x80000000) = 0x0;
+  *(tPVU32)(0x80000004) = 0x0;
+  // All banks in idle.
+  DELAY(350);
+  not_found = 1;
+  for (Vrefloopindex = 0; Vrefloopindex < Vrefloopmax; Vrefloopindex = Vrefloopindex + 1) {
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x0480d404 | Vrefstarta << 16;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    DELAY(5);
+    *(tPVU32)(0x80000000) = 0xa5a55a5a;
+    *(tPVU32)(0x80000004) = 0xa5a55a5a;
+    if ((*(tPVU32)(0x80000000) == 0xa5a55a5a) & (*(tPVU32)(0x80000000) == 0xa5a55a5a)) {  
+      if (not_found == 1) {
+        min_passing_vref = Vrefstarta;
+        not_found = 0;
+	Vrefstarta = Vrefstarta + 1;
+	*(tPVU32)(0x80000000) = 0x0;
+	*(tPVU32)(0x80000004) = 0x0;
+	DELAY(5);
+      }
+    }
+    else {
+      if (not_found == 0) {
+        max_passing_vref = Vrefstarta;
+      }
+      else {
+	Vrefstarta = Vrefstarta + 1;
+	*(tPVU32)(0x80000000) = 0x0;
+	*(tPVU32)(0x80000004) = 0x0;
+	DELAY(5);
+      }
+    }
+  // All banks in idle.
+  DELAY(350);
+  }
+  Vrefstarta = min_passing_vref + ((max_passing_vref - min_passing_vref) >> 1);
+  *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x0400d404 | Vrefstarta << 16;
+  while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);     
+#endif
+#endif
+
+  // Program the correct DDR Type based on the size. This works only for x16 devices.
+  ddrsize_calc(ddr_freq);
+
+#if defined (ECC)
+  *(tPVU32)(MC_REG_ADDRESS58) = LOWER_ECC_ADDRESS;
+  *(tPVU32)(MC_REG_ADDRESS5C) = UPPER_ECC_ADDRESS;
+  *(tPVU32)(MC_REG_ADDRESS08) = (*(tPVU32)(MC_REG_ADDRESS08) & 0xffffff00) | 0x00000067;
+  *(tPVU32)(MC_REG_ADDRESS60) = LOWER_ECC_ADDRESS;
+  *(tPVU32)(MC_REG_ADDRESS64) = UPPER_ECC_ADDRESS;
+  *(tPVU32)(MC_REG_ADDRESS0C) = (*(tPVU32)(MC_REG_ADDRESS0C) & 0xbfffffff) | 0x40000000;
+  *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & 0xfffffff7) | 0x00000008;
+  while((*(tPVU32)(MC_REG_ADDRESS00) & 0x08) != 0);
+#endif
+
+  /**** All interrupts cleared ****/
+  WRITE_REG32(MC_REG_ADDRESS44, 0xffffffff);  
+  WRITE_REG32(MC_REG_ADDRESS50, 0xffffffff); 
+  //Clearing single/multi bit error 
+  WRITE_REG32(MC_REG_ADDRESS08, ((READ_REG32(MC_REG_ADDRESS08) & ~0x00000018) | 0x18));  
+
+  *(tPVU32)(MC_REG_ADDRESS28) = (*(tPVU32)(MC_REG_ADDRESS28) | 0x01004900); 
+  *(tPVU32)(MC_REG_ADDRESS40) = (*(tPVU32)(MC_REG_ADDRESS40) | 0x00010000);
+
+  //Write/Read to make sure DDR is initialized properly. Temporary data at LMEMDDRTRAINVAR + 0x300, 0x304.
+  *(tPVU32)(0x80000000) = 0xbabeface;
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x300) = READ_REG32(0x80000000);
+ 
+  *(tPVU32)(0x80000004) = 0xdeadbeef;
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x304) = READ_REG32(0x80000004);
+
+#if defined (HW_ENV)
+  if ((*(tPVU32)(0x80000000) != 0xbabeface) | (*(tPVU32)(0x80000004) != 0xdeadbeef)) { 
+    nc_printf("!!! ERROR Memory Write/Read Test failed !!! \n");
+  }
+#endif
+
+  *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0x00000200) | 0x00000200;
+  DELAY(5);
+
+  //shutoff mcss register clk
+  *(tPVU32)(SYS_MCSS_CLK_CTRL) = 0xA000;
+
+  return 0;
+
+}//ddrinit
+
+void ddrcfg(tU8 ddr_type, tU8 ddr_freq){
+  
+  if (ddr_type == 1) {
+    // DDR4
+    // r_zq2anycounter=510 (16,15,14,13,12,11,10,9,8),
+    *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x0001ff00) | 0x0001fe00;
+#if defined (DDR3_1600MHZ)
+    // r_pre2act_del_1=1.
+    *(tPVU32)(MC_REG_ADDRESS08) = (*(tPVU32)(MC_REG_ADDRESS08) & ~0x02000000) | 0x02000000;
+#endif
+    //r_act2actdb_del_1=1 (18)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0x00040000) | 0x00040000;
+
+    // DDR4:- tMOD - 24. r_mrs2nmrscounter (15,14,13,12)
+    *(tPVU32)(MC_REG_ADDRESS08) = (*(tPVU32)(MC_REG_ADDRESS08) & ~0x0000f000) | tmrs2nmrs_array[ddr_freq] << 12;
+    // tRC = 50. r_act2actsbcounter (24,23,22,21,20).
+    *(tPVU32)(MC_REG_ADDRESS14) = (*(tPVU32)(MC_REG_ADDRESS14) & ~0x01f00000) | trc_array[ddr_freq+8] << 20;
+    // FAW = 35ns (24,23,22,21,20). tRAS = 35ns. r_act2precounter (29,28,27,26,25),
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0x3ff00000) | tfawtras_array[ddr_freq+8] << 20;
+    // tRFC = 350. r_trfcplus10counter (7,6,5,4,3,2,1,0),
+    *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x000000ff) | trfc10_array[ddr_freq+8][0]; 
+    // r_ref2actcounter (24,23,22,21,20,19,18,17), 
+    *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x01fe0000) | trfc_array[ddr_freq+8][0] << 17; 
+
+#if defined (WL11RL12)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0xc000ffff) | 0x0000021d;
+#else
+#if defined (DLL_OFF)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0xc000ffff) | 0x00000210;
+#else
+    // WL9RL10
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0xc000ffff) | 0x00000214;
+#endif
+#endif
+
+    DELAY(100);
+    // De-assertion of RESET.
+#if defined (DLL_OFF)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffe7) | 0x00000024;
+#else
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffe7) | 0x00000004;
+#endif
+   
+    DELAY(100);
+
+    // Assertion of CKE.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00000404;
+
+    DELAY(10);
+
+    // MRS3.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00007404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS6.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x0400d404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS5.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x0400b404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS4.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00009404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#if defined (WL9RL10)
+    // MRS2. WL9RL10.
+    //*(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00805404;
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00005404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#else
+    // MRS2. WL11RL12.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00105404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+#if defined (DLL_OFF)
+    // DLL OFF. MRS1.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00003404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0); 
+    // MRS0.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00051404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#else 
+    // MRS1.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x01013404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0); 
+    // MRS0.
+#if defined (WL9RL10)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x03051404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#else
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x03151404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+#endif
+    // ZQ calib.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00000584;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+
+  } else {
+
+    // DDR3    
+#if defined (DDR3_1066MHZ)
+    // r_act2actdb_del_1=1 (18)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0x00040000) | 0x00040000;
+#endif
+#if defined (DDR3_1333MHZ)
+    // r_act2actdb_del_1=1 (18)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0x00040000) | 0x00040000;
+#endif
+#if defined (DDR3_1600MHZ)
+    // r_act2actdb_del_1=1 (18)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0x00040000) | 0x00040000;
+    // r_act2actdb_del_2=1 (29)
+    *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x20000000) | 0x20000000;
+#endif
+
+    // tRC = 48.75. r_act2actsbcounter (24,23,22,21,20).
+    *(tPVU32)(MC_REG_ADDRESS14) = (*(tPVU32)(MC_REG_ADDRESS14) & ~0x01f00000) | trc_array[ddr_freq] << 20;
+    // FAW = 50ns (24,23,22,21,20). tRAS = 35ns. r_act2precounter (29,28,27,26,25),
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0x3ff00000) | tfawtras_array[ddr_freq] << 20;
+    // tRFC = 350. r_trfcplus10counter (7,6,5,4,3,2,1,0),
+    *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x000000ff) | trfc10_array[ddr_freq][3]; 
+    // r_ref2actcounter (24,23,22,21,20,19,18,17), 
+    *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x01fe0000) | trfc_array[ddr_freq][3] << 17; 
+
+    // r_zq2anycounter=254 (16,15,14,13,12,11,10,9,8), 
+    *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x0001ff00) | 0x0000fe00;
+#if defined (WL8RL11)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0xc000ffff) | 0x0000025b;
+#endif
+#if defined (WL7RL9)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0xc000ffff) | 0x00000212;
+#endif
+#if defined (WL6RL7)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0xc000ffff) | 0x00000249;
+#endif
+#if defined (WL5RL6)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0xc000ffff) | 0x00000204;
+#endif
+#if defined (WL6RL6)
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0xc000ffff) | 0x00000245;
+#endif
+
+    DELAY(100);
+
+    // De-assertion of RESET.
+#if defined (DLL_OFF)
+    //DLL OFF  
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffe7) | 0x00000024;
+#else
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffe7) | 0x00000004;
+#endif
+
+    // DDR3:- tMOD - 12. Program 4. r_mrs2nmrscounter 0x08 (15,14,13,12)
+    *(tPVU32)(MC_REG_ADDRESS08) = (*(tPVU32)(MC_REG_ADDRESS08) & ~0x0000f000) | 0x00004000;
+    DELAY(100);
+
+    // Assertion of CKE.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00000404;
+
+    // MRS2.
+#if defined (WL5RL6)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00005404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+#if defined (WL6RL6)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00085404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+#if defined (WL6RL7)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00085404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+#if defined (WL7RL9)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00105404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+#if defined (WL8RL11)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00185404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+    // MRS3.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00007404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS1.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00043404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+    // MRS reset DLL.
+#if defined (DLL_OFF)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xff000040) | 0x13000000;
+#else
+#if defined (DDR3_800MHZ)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xff000040) | 0x15000040;
+#endif
+#if defined (DDR3_1066MHZ)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xff000040) | 0x19000040;
+#endif
+#if defined (DDR3_1333MHZ)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xff000040) | 0x1b000040;
+#endif
+#if defined (DDR3_1600MHZ)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xff000040) | 0x1d000040;
+#endif
+#endif
+#if defined (WL5RL6)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0x00ffff87) | 0x00211404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+#if defined (WL6RL6)
+    // DLL OFF mode. 
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0x00ffff87) | 0x00211404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+#if defined (WL6RL7)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0x00ffff87) | 0x00311404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+#if defined (WL7RL9)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0x00ffff87) | 0x00511404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+#if defined (WL8RL11)
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0x00ffff87) | 0x00711404;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#endif
+    // ZQ calib.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00000584;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);
+#if defined (DLL_OFF)
+    *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & ~0x00002000);
+    // MRS1.
+    *(tPVU32)(MC_REG_ADDRESS00) = (*(tPVU32)(MC_REG_ADDRESS00) & ~0xffffffc7) | 0x00033444;
+    while(((*(tPVU32)(MC_REG_ADDRESS00)) & 0x1000) != 0);    
+
+    // Change DLL-on to DLL-off after self-refresh.
+    *(tPVU32)(MC_REG_ADDRESS10) = (*(tPVU32)(MC_REG_ADDRESS10) & ~0x00000007) | 0x00000005;
+    
+    *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & ~0x00040000) | 0x00040000;
+    *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & ~0x00018000) | 0x00018000;
+    while(((*(tPVU32)(MC_REG_ADDRESS18)) & 0x20000) == 0);
+    *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & ~0x00018000);
+    while(((*(tPVU32)(MC_REG_ADDRESS18)) & 0x20000) != 0);
+    *(tPVU32)(LMEMDDRTRAINVAR + 0x100) = (*(tPVU32)(MC_REG_ADDRESS10));
+    *(tPVU32)(MC_REG_ADDRESS18) = (*(tPVU32)(MC_REG_ADDRESS18) & ~0x00002000) | 0x00002000;
+    DELAY(5);
+#endif
+
+  }
+  DELAY(5);
+}
+
+tU8 gate_training(void){
+  tU32 dqsgate_min=0, dqsgate_max=0, dqsgate_min_bin, dqsgate_max_bin, dqsgate_avg, gindex;
+  tU8 not_found, break_loop, coarsev, sbit, dqsgate;
+
+  // Intermediate values stored in : - LMEMDDRTRAINVAR + 0x100 to LMEMDDRTRAINVAR + 0x124.
+
+  // Byte 0.
+  // PREDETMODE = 1, DQSGATEDLYC, DQSGATEDLYF = 0. PREDETCLR = 1. 
+  gindex = 0;
+  *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xffff0000) | 0x0000a000 | dqsgate_array[gindex];
+  
+  // Memory read.
+  READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+  DELAY(5);
+
+  // Check PREDET = 0.  
+  not_found = 1;
+  break_loop = 0;
+  if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x00001000) == 0x00000000) {
+    // PREDETCLR = 1 and Memory read.
+    *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xffffdfff) | 0x00002000;
+    READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+    DELAY(5);
+    if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x00001000) == 0x00000000) {
+      dqsgate_min = ((*(tPVU32)(APB_REG_ADDRESS34)) & 0x000007ff);
+      not_found = 0;
+      break_loop = 1;
+    }
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQSGATEMIN Byte0 LOOP0:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+  } else {
+    while (break_loop != 1) {
+      gindex = gindex + 1;
+      *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xffffd800) | dqsgate_array[gindex] | 0x00002000;
+      // Memory read.
+      READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+      DELAY(5);
+      if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x00001000) == 0x00000000) {
+        *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xffffdfff) | 0x00002000;
+        READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+        DELAY(5);
+        if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x00001000) == 0x00000000) {
+	  dqsgate_min = ((*(tPVU32)(APB_REG_ADDRESS34)) & 0x000007ff);
+	  not_found = 0;
+	  break_loop = 1;
+        }
+      }
+#if defined (HW_ENVPRN)
+      nc_printf("!!! DQSGATEMIN Byte0 LOOP1:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+      if (gindex == 512) { 
+        dqsgate_min = ((*(tPVU32)(APB_REG_ADDRESS34)) & 0x000007ff);
+	break_loop = 1; 
+      }
+    }
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x100) = dqsgate_min;
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQSGATEMIN Byte0 Value:%x\n", dqsgate_min);
+#endif
+  // PREDETMODE = 0, DQSGATEDLYC, DQSGATEDLYF = dqsgate_min.
+  gindex = gindex + 1;
+  *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xffff5800) | dqsgate_array[gindex] | 0x00002000;
+  READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+  DELAY(5);
+
+  // Check PREDET = 1.
+  if (not_found == 0) {
+    not_found = 1;
+    break_loop = 0;
+    if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x00001000) == 0x00001000) {
+      not_found = 0;
+      break_loop = 1;
+#if defined (HW_ENVPRN)
+      nc_printf("!!! DQSGATEMIN Byte0 LOOP2:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+    } else {
+      while (break_loop != 1) {
+        gindex = gindex + 1;
+        *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xffffd800) | dqsgate_array[gindex] | 0x00002000;
+	// Memory read.      
+	READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+	DELAY(5);
+	if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x00001000) == 0x00001000) {
+	  not_found = 0;
+	  break_loop = 1;
+	}
+	if (gindex == 512) {         
+          break_loop = 1;
+          not_found = 0;
+        }
+#if defined (HW_ENVPRN)
+        nc_printf("!!! DQSGATEMIN Byte0 LOOP3:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+      }
+    }
+  }
+
+  if (not_found == 0) {
+    not_found = 1;
+    break_loop = 0;
+    // Decrease DQSGATEDLYC, DQSGATEDLYF. PREDET = 0.
+    while (break_loop != 1) {
+      gindex = gindex - 1;
+      *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xffffd800) | dqsgate_array[gindex] | 0x00002000;
+      // Memory read.
+      READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+      DELAY(5);
+      if ((((*(tPVU32)(APB_REG_ADDRESS34)) & 0x00001000) == 0x00000000)) {
+	dqsgate_max = ((*(tPVU32)(APB_REG_ADDRESS34)) & 0x000007ff);
+	not_found = 0;
+	break_loop = 1;
+      }
+      if (gindex == 0) { 
+        dqsgate_max = ((*(tPVU32)(APB_REG_ADDRESS34)) & 0x000007ff);
+        break_loop = 1; 
+      }
+#if defined (HW_ENVPRN)
+      nc_printf("!!! DQSGATEMIN Byte0 LOOP4:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+    }
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x104) = dqsgate_max;  
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQSGATEMAX Byte0 Value:%x\n", dqsgate_max);
+#endif
+
+  // Form the binary number.
+  if (not_found == 0) {
+    coarsev = (dqsgate_min & 0x700) >> 8;
+    sbit = (dqsgate_min & 0x040) >> 6;
+    dqsgate = (dqsgate_min & 0x03f);
+    if (sbit == 0x1) {
+      dqsgate_min_bin = ((0x20 - dqsgate) & 0x3f) | (coarsev << 6) | (((~sbit) << 5) & 0x20);
+    }
+    else {
+      dqsgate_min_bin = (dqsgate & 0x1f) | (coarsev << 6) | (((~sbit) << 5) & 0x20);
+    }
+    coarsev = (dqsgate_max & 0x700) >> 8;
+    sbit = (dqsgate_max & 0x040) >> 6;
+    dqsgate = (dqsgate_max & 0x03f);
+    if (sbit == 0x1) {
+      dqsgate_max_bin = ((0x20 - dqsgate) & 0x3f) | (coarsev << 6) | (((~sbit) << 5) & 0x20);
+    }
+    else {
+      dqsgate_max_bin = (dqsgate & 0x1f) | (coarsev << 6) | (((~sbit) << 5) & 0x20);
+    }
+
+    *(tPVU32)(LMEMDDRTRAINVAR + 0x108) = dqsgate_min_bin;
+    *(tPVU32)(LMEMDDRTRAINVAR + 0x10c) = dqsgate_max_bin;  
+    // Average of min and max.
+    dqsgate_avg = dqsgate_min_bin + (dqsgate_max_bin + 1 - dqsgate_min_bin)/2;
+    *(tPVU32)(LMEMDDRTRAINVAR + 0x110) = dqsgate_avg;
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQSGATEMIN Byte0 bin Value:%x, DQSGATEMAX bin Value:%x, DQSGATEAVG bin Value:%x\n", dqsgate_min_bin, dqsgate_max_bin, dqsgate_avg);
+#endif
+    // Form the signed number.
+    coarsev = (dqsgate_avg & 0x1c0) >> 6;
+    sbit = (dqsgate_avg & 0x020) >> 5;
+    if (sbit == 0x1) {
+      sbit = 0x0;
+    } else {
+      sbit = 0x1;
+    }
+    dqsgate = (dqsgate_avg & 0x3f);
+    if ((dqsgate & 0x20) == 0x00) {
+      dqsgate = (0x20 - dqsgate);
+    }
+    else {
+      dqsgate = (dqsgate & 0x1f);
+    }
+    *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xfffff800) | (sbit << 6) | (coarsev << 8) | dqsgate;
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQSGATE Byte0 Value:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQSGATEMIN0:%x, DQSGATEMAX0:%x, DQSGATEAVG0:%x !!! \ !!! \n", dqsgate_min, dqsgate_max, dqsgate_avg);
+#endif
+  }
+
+  // Byte 1.
+  // PREDETMODE = 1, DQSGATEDLYC, DQSGATEDLYF = 0. PREDETCLR = 1. 
+  if (not_found == 0) {
+    gindex = 0;
+    *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0x0000ffff) | 0xa0000000 | (dqsgate_array[gindex] << 16);
+    
+    // Memory read.
+    READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+    DELAY(5);
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQSGATEMIN Byte1 LOOP0:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+  }
+
+  // Check PREDET = 0.
+  if (not_found == 0) {
+    not_found = 1;
+    break_loop = 0;
+    if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x10000000) == 0x00000000) {
+      // PREDETCLR = 1 and Memory read.
+      *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xdfffffff) | 0x20000000;
+      READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+      DELAY(5);
+      if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x10000000) == 0x00000000) {
+	dqsgate_min = ( ((*(tPVU32)(APB_REG_ADDRESS34)) & 0x07ff0000) >> 16);
+	not_found = 0;
+	break_loop = 1;
+      }
+#if defined (HW_ENVPRN)
+      nc_printf("!!! DQSGATEMIN Byte1 LOOP0:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+    } else {
+      while (break_loop != 1) {
+        gindex = gindex + 1;
+        *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xd800ffff) | (dqsgate_array[gindex] << 16) | 0x20000000;
+	// Memory read.
+	READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+	DELAY(5);
+	if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x10000000) == 0x00000000) {
+	  *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xdfffffff) | 0x20000000;
+	  READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+	  DELAY(5);
+	  if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x10000000) == 0x00000000) {
+	    dqsgate_min = ( ((*(tPVU32)(APB_REG_ADDRESS34)) & 0x07ff0000) >> 16);
+            not_found = 0;
+            break_loop = 1;
+	  }
+	}
+#if defined (HW_ENVPRN)
+        nc_printf("!!! DQSGATEMIN Byte1 LOOP1:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif      
+	if (gindex == 512) { 
+          dqsgate_min = ( ((*(tPVU32)(APB_REG_ADDRESS34)) & 0x07ff0000) >> 16);
+          break_loop = 1; 
+        }
+      }
+    }
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x114) = dqsgate_min;
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQSGATEMIN Byte1 Value:%x\n", dqsgate_min);
+#endif
+  // PREDETMODE = 0, DQSGATEDLYC, DQSGATEDLYF = dqsgate_min.
+  if (not_found == 0) {
+    gindex = gindex + 1;
+    *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0x5800ffff) | (dqsgate_array[gindex] << 16) | 0x20000000;
+    READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+    DELAY(5);
+
+    // Check PREDET = 1.
+    not_found = 1;
+    break_loop = 0;
+    if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x10000000) == 0x10000000) {
+      not_found = 0;
+      break_loop = 1;
+#if defined (HW_ENVPRN)
+      nc_printf("!!! DQSGATEMIN Byte1 LOOP2:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+    } else {
+      while (break_loop != 1) {
+        gindex = gindex + 1;
+        *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xd800ffff) | (dqsgate_array[gindex] << 16) | 0x20000000;
+	// Memory read.
+	READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+	DELAY(5);
+	if (((*(tPVU32)(APB_REG_ADDRESS34)) & 0x10000000) == 0x10000000) {
+	  not_found = 0;
+	  break_loop = 1;
+        }
+	if (gindex == 512) { 
+          not_found = 0;
+          break_loop = 1; 
+        }
+#if defined (HW_ENVPRN)
+        nc_printf("!!! DQSGATEMIN Byte1 LOOP3:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+      }
+    }
+  }
+
+  if (not_found == 0) {
+    not_found = 1;
+    break_loop = 0;
+    // Decrease DQSGATEDLYC, DQSGATEDLYF. PREDET = 0.
+    while (break_loop != 1) {
+      gindex = gindex - 1;
+      *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xd800ffff) | (dqsgate_array[gindex]  << 16) | 0x20000000;
+      // Memory read.
+      READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+      DELAY(5);
+      if ((((*(tPVU32)(APB_REG_ADDRESS34)) & 0x10000000) == 0x00000000)) {
+	dqsgate_max = ( ((*(tPVU32)(APB_REG_ADDRESS34)) & 0x07ff0000) >> 16);
+	not_found = 0;
+	break_loop = 1;
+      }
+      if (gindex == 0) { 
+        dqsgate_max = ( ((*(tPVU32)(APB_REG_ADDRESS34)) & 0x07ff0000) >> 16);
+        break_loop = 1; 
+      }
+#if defined (HW_ENVPRN)
+      nc_printf("!!! DQSGATEMIN Byte1 LOOP4:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+    }
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x118) = dqsgate_max;
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQSGATEMAX Byte1 Value:%x\n", dqsgate_max);
+#endif
+  if (not_found == 0) {
+    // Form the binary number.
+    coarsev = (dqsgate_min & 0x700) >> 8;
+    sbit = (dqsgate_min & 0x040) >> 6;
+    dqsgate = (dqsgate_min & 0x03f);
+    if (sbit == 0x1) {
+      dqsgate_min_bin = ((0x20 - dqsgate) & 0x3f) | (coarsev << 6) | (((~sbit) << 5) & 0x20);
+    }
+    else {
+      dqsgate_min_bin = (dqsgate & 0x1f) | (coarsev << 6) | (((~sbit) << 5) & 0x20);
+    }
+    coarsev = (dqsgate_max & 0x700) >> 8;
+    sbit = (dqsgate_max & 0x040) >> 6;
+    dqsgate = (dqsgate_max & 0x03f);
+    if (sbit == 0x1) {
+      dqsgate_max_bin = ((0x20 - dqsgate) & 0x3f) | (coarsev << 6) | (((~sbit) << 5) & 0x20);
+    }
+    else {
+      dqsgate_max_bin = (dqsgate & 0x1f) | (coarsev << 6) | (((~sbit) << 5) & 0x20);
+    }
+
+    *(tPVU32)(LMEMDDRTRAINVAR + 0x11c) = dqsgate_min_bin;
+    *(tPVU32)(LMEMDDRTRAINVAR + 0x120) = dqsgate_max_bin;
+    // Average of min and max.
+    dqsgate_avg = dqsgate_min_bin + (dqsgate_max_bin + 1 - dqsgate_min_bin)/2;
+    *(tPVU32)(LMEMDDRTRAINVAR + 0x124) = dqsgate_avg;
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQSGATEMIN Byte1 bin Value:%x, DQSGATEMAX bin Value:%x, DQSGATEAVG bin Value:%x\n", dqsgate_min_bin, dqsgate_max_bin, dqsgate_avg);
+#endif
+    // Form the signed number.
+    coarsev = (dqsgate_avg & 0x1c0) >> 6;
+    sbit = (dqsgate_avg & 0x020) >> 5;
+    if (sbit == 0x1) {
+      sbit = 0x0;
+    } else {
+      sbit = 0x1;
+    }
+    dqsgate = (dqsgate_avg & 0x3f);
+    if ((dqsgate & 0x20) == 0x00) {
+      dqsgate = (0x20 - dqsgate);
+    }
+    else {
+      dqsgate = (dqsgate & 0x1f);
+    }
+    *(tPVU32)(APB_REG_ADDRESS34) = (*(tPVU32)(APB_REG_ADDRESS34) & 0xf800ffff) | (sbit << 22) | (coarsev << 24) | (dqsgate << 16);
+#if defined (HW_ENV)
+    nc_printf("!!! DQSGATE Register 34:- Byte1 Byte0 Value:%x\n", (*(tPVU32)(APB_REG_ADDRESS34)));
+#endif
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQSGATEMIN1:%x, DQSGATEMAX1:%x, DQSGATEAVG1:%x !!! \ !!! \n", dqsgate_min, dqsgate_max, dqsgate_avg);
+#endif
+
+  }
+
+  return not_found;
+} // gate_training
+  
+ tU8 eye_training(tU32 upper_dword, tU32 lower_dword, tU8 ddr_type){
+  tU32 temp;
+  tU8 not_found, break_loop, dqsvalue_min, dqsvalue_max, dqsvalue_avg, dqsvalue_min_bin, dqsvalue_max_bin, pbyte, nbyte, psbyte, nsbyte;
+  tU8 gindex, sbit, dqsvalue, range;
+
+  // Intermediate values stored in : - LMEMDDRTRAINVAR + 0x200 to LMEMDDRTRAINVAR + 0x234.
+
+  // Read the default DQS value. First Byte.
+#if defined (STARTFROMCENTERSWEEP_OFF)
+  gindex = 0;
+#else
+  gindex = 32;
+#endif
+  break_loop = 0;
+  not_found = 1;
+  SET_BIT32(APB_REG_ADDRESS144, 7); // Set bit 7 of register offset 144.
+  // Decrement SDLYs until data miscompare.
+  while (break_loop != 1) {
+#if defined (STARTFROMCENTERSWEEP_OFF)
+    gindex = gindex + 1;
+#else
+    gindex = gindex - 1;
+#endif
+    *(tPVU32)(APB_REG_ADDRESS144) = (*(tPVU32)(APB_REG_ADDRESS144) & 0xffffff80) | dqsgate_array[gindex];    
+    temp = READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+    pbyte = temp & 0xff;
+    nbyte = (temp >> 16) & 0xff;
+    temp = READ_REG32(0x80000004);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+    psbyte = temp & 0xff;
+    nsbyte = (temp >> 16) & 0xff;
+#if defined (STARTFROMCENTERSWEEP_OFF)
+    if ( (pbyte == (lower_dword & 0xff)) & (nbyte == ((lower_dword >> 16) & 0xff)) & (psbyte == (upper_dword & 0xff)) & (nsbyte == ((upper_dword >> 16) & 0xff)) ) {
+#else
+    if ( (pbyte != (lower_dword & 0xff)) || (nbyte != ((lower_dword >> 16) & 0xff)) || (psbyte != (upper_dword & 0xff)) || (nsbyte != ((upper_dword >> 16) & 0xff)) ) {
+#endif
+      break_loop = 1;
+      dqsvalue_min = ((*(tPVU32)(APB_REG_ADDRESS144)) & 0x0000007f);
+      not_found = 0;
+    }
+    if (gindex == 0) { 
+      dqsvalue_min = ((*(tPVU32)(APB_REG_ADDRESS144)) & 0x0000007f);
+      break_loop = 1; 
+    }
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQS EYE Byte0 Loop0 :%x pbyte:%x, nbyte:%x, psbyte:%x, nsbyte:%x\n", (*(tPVU32)(APB_REG_ADDRESS144)), pbyte, nbyte, psbyte, nsbyte);
+#endif
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x200) = dqsvalue_min;
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQS EYE min Byte0 :%x\n", dqsvalue_min);
+#endif
+  // Increment SDLYs until data miscompare.
+#if defined (STARTFROMCENTERSWEEP_OFF)
+#else
+  gindex = 32;
+#endif
+  break_loop = 0;
+  not_found = 1;
+  while (break_loop != 1) {
+    gindex = gindex + 1;
+    *(tPVU32)(APB_REG_ADDRESS144) = (*(tPVU32)(APB_REG_ADDRESS144) & 0xffffff80) | dqsgate_array[gindex]; 
+    temp = READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+    pbyte = temp & 0xff;
+    nbyte = (temp >> 16) & 0xff;
+    temp = READ_REG32(0x80000004);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+    psbyte = temp & 0xff;
+    nsbyte = (temp >> 16) & 0xff;
+    if ( (pbyte != (lower_dword & 0xff)) || (nbyte != ((lower_dword >> 16) & 0xff)) || (psbyte != (upper_dword & 0xff)) || (nsbyte != ((upper_dword >> 16) & 0xff)) ) {
+      break_loop = 1;
+      dqsvalue_max = ((*(tPVU32)(APB_REG_ADDRESS144)) & 0x0000007f);
+      not_found = 0;
+    }
+    if (gindex == 63) { 
+     dqsvalue_max = ((*(tPVU32)(APB_REG_ADDRESS144)) & 0x0000007f);
+     break_loop = 1; 
+    }
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQS EYE Byte0 Loop1 :%x pbyte:%x, nbyte:%x, psbyte:%x, nsbyte:%x\n", (*(tPVU32)(APB_REG_ADDRESS144)), pbyte, nbyte, psbyte, nsbyte);
+#endif
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x204) = dqsvalue_max;  
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQS EYE max Byte0 :%x\n", dqsvalue_max);
+#endif
+  // Form the binary number.
+  sbit = (dqsvalue_min & 0x40) >> 6;
+  dqsvalue = (dqsvalue_min & 0x3f);
+  if (sbit == 0x1) {
+    dqsvalue_min_bin = ((0x20 - dqsvalue) & 0x3f) | (((~sbit) << 5) & 0x20);
+  }
+  else {
+    dqsvalue_min_bin = (dqsvalue & 0x1f) | (((~sbit) << 5) & 0x20);
+  }
+  sbit = (dqsvalue_max & 0x40) >> 6;
+  dqsvalue = (dqsvalue_max & 0x3f);
+  if (sbit == 0x1) {
+    dqsvalue_max_bin = ((0x20 - dqsvalue) & 0x3f) | (((~sbit) << 5) & 0x20);
+  }
+  else {
+    dqsvalue_max_bin = (dqsvalue & 0x1f) | (((~sbit) << 5) & 0x20);
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x208) = dqsvalue_min_bin;
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x20c) = dqsvalue_max_bin; 
+  // Average of min and max.
+  dqsvalue_avg = dqsvalue_min_bin + (dqsvalue_max_bin + 1 - dqsvalue_min_bin)/2;
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x210) = dqsvalue_avg;
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQS EYE Byte0 min_bin:%x, max_bin:%x, avg_bin:%x\n", dqsvalue_min_bin, dqsvalue_max_bin, dqsvalue_avg);
+#endif
+  // Form the signed number.
+  sbit = (dqsvalue_avg & 0x020) >> 5;
+  if (sbit == 0x1) {
+    sbit = 0x0;
+  } else {
+    sbit = 0x1;
+  }
+  dqsvalue = (dqsvalue_avg & 0x3f);
+  if ((dqsvalue & 0x20) == 0x00) {
+    dqsvalue = (0x20 - dqsvalue);
+  }
+  else {
+    dqsvalue = (dqsvalue & 0x1f);
+  }
+  *(tPVU32)(APB_REG_ADDRESS144) = (*(tPVU32)(APB_REG_ADDRESS144) & 0xffffff80) | (sbit << 6) | dqsvalue;
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQS EYE Byte0 Value:%x\n",  (*(tPVU32)(APB_REG_ADDRESS144)));
+#endif
+  if (ddr_type != 1) {
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQSVALUEMIN:%x, DQSVALUEMAX:%x, Range:%d for byte 0 !!! \n", dqsvalue_min_bin, dqsvalue_max_bin, (dqsvalue_max_bin + 1 - dqsvalue_min_bin));
+#endif
+  }
+  range = (dqsvalue_max_bin + 1 - dqsvalue_min_bin);
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x214) = range;
+
+  // Second Byte.
+#if defined (STARTFROMCENTERSWEEP_OFF)
+  gindex = 0;
+#else
+  gindex = 32;
+#endif
+  break_loop = 0;
+  not_found = 1;
+  SET_BIT32(APB_REG_ADDRESS144, 15);
+
+  // Decrement SDLYs until data miscompare.
+  while (break_loop != 1) {
+#if defined (STARTFROMCENTERSWEEP_OFF)
+    gindex = gindex + 1;
+#else
+    gindex = gindex - 1;    
+#endif
+    *(tPVU32)(APB_REG_ADDRESS144) = (*(tPVU32)(APB_REG_ADDRESS144) & 0xffff80ff) | (dqsgate_array[gindex] << 8);
+    temp = READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+    pbyte = (temp >> 8) & 0xff;
+    nbyte = (temp >> 24) & 0xff;
+    temp = READ_REG32(0x80000004);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+    psbyte = (temp >> 8) & 0xff;
+    nsbyte = (temp >> 24) & 0xff;
+#if defined (STARTFROMCENTERSWEEP_OFF)
+    if ( (pbyte == ((lower_dword >> 8) & 0xff)) & (nbyte == ((lower_dword >> 24) & 0xff)) & (psbyte == ((upper_dword >> 8) & 0xff)) & (nsbyte == ((upper_dword >> 24) & 0xff)) ) {
+#else
+    if ( (pbyte != ((lower_dword >> 8) & 0xff)) || (nbyte != ((lower_dword >> 24) & 0xff)) || (psbyte != ((upper_dword >> 8) & 0xff)) || (nsbyte != ((upper_dword >> 24) & 0xff)) ) {
+#endif
+      break_loop = 1;
+      dqsvalue_min = ((*(tPVU32)(APB_REG_ADDRESS144)) & 0x00007f00) >> 8;
+      not_found = 0;
+    }
+    if (gindex == 0) { 
+      dqsvalue_min = ((*(tPVU32)(APB_REG_ADDRESS144)) & 0x00007f00) >> 8;
+      break_loop = 1; 
+    }
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQS EYE Byte1 Loop0 :%x pbyte:%x, nbyte:%x, psbyte:%x, nsbyte:%x\n", (*(tPVU32)(APB_REG_ADDRESS144)), pbyte, nbyte, psbyte, nsbyte);
+#endif
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x220) = dqsvalue_min;
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQS EYE min Byte1 :%x\n", dqsvalue_min);
+#endif
+  // Increment SDLYs until data miscompare.
+#if defined (STARTFROMCENTERSWEEP_OFF)
+#else
+  gindex = 32;
+#endif
+  break_loop = 0;
+  not_found = 1;
+  while ((break_loop != 1)) {
+     gindex = gindex + 1;
+     *(tPVU32)(APB_REG_ADDRESS144) = (*(tPVU32)(APB_REG_ADDRESS144) & 0xffff80ff) | (dqsgate_array[gindex] << 8);
+     temp = READ_REG32(0x80000000);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+     pbyte = (temp >> 8) & 0xff;
+     nbyte = (temp >> 24) & 0xff;
+     temp = READ_REG32(0x80000004);
+  *(tPVU32)(APB_REG_ADDRESS18) = (*(tPVU32)(APB_REG_ADDRESS18) & 0x00000000) | 0x00010101;
+     psbyte = (temp >> 8) & 0xff;
+     nsbyte = (temp >> 24) & 0xff;
+     if ( (pbyte != ((lower_dword >> 8) & 0xff)) || (nbyte != ((lower_dword >> 24) & 0xff)) || (psbyte != ((upper_dword >> 8) & 0xff)) || (nsbyte != ((upper_dword >> 24) & 0xff)) ) {
+       break_loop = 1;
+       dqsvalue_max = ((*(tPVU32)(APB_REG_ADDRESS144)) & 0x00007f00) >> 8;
+       not_found = 0;
+     }
+     if (gindex == 63) { 
+       dqsvalue_max = ((*(tPVU32)(APB_REG_ADDRESS144)) & 0x00007f00) >> 8;
+       break_loop = 1; 
+     }
+#if defined (HW_ENVPRN)
+     nc_printf("!!! DQS EYE Byte1 Loop1 :%x pbyte:%x, nbyte:%x, psbyte:%x, nsbyte:%x\n", (*(tPVU32)(APB_REG_ADDRESS144)), pbyte, nbyte, psbyte, nsbyte);
+#endif
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x224) = dqsvalue_max;  
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQS EYE max Byte1 :%x\n", dqsvalue_max);  
+#endif
+  // Form the binary number.
+  sbit = (dqsvalue_min & 0x40) >> 6;
+  dqsvalue = (dqsvalue_min & 0x3f);
+  if (sbit == 0x1) {
+     dqsvalue_min_bin = ((0x20 - dqsvalue) & 0x3f) | (((~sbit) << 5) & 0x20);
+  }
+  else {
+     dqsvalue_min_bin = (dqsvalue & 0x1f) | (((~sbit) << 5) & 0x20);
+  }
+  sbit = (dqsvalue_max & 0x40) >> 6;
+  dqsvalue = (dqsvalue_max & 0x3f);
+  if (sbit == 0x1) {
+     dqsvalue_max_bin = ((0x20 - dqsvalue) & 0x3f) | (((~sbit) << 5) & 0x20);
+  }
+  else {
+     dqsvalue_max_bin = (dqsvalue & 0x1f) | (((~sbit) << 5) & 0x20);
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x228) = dqsvalue_min_bin;
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x22c) = dqsvalue_max_bin; 
+  // Average of min and max.
+  dqsvalue_avg = dqsvalue_min_bin + (dqsvalue_max_bin + 1 - dqsvalue_min_bin)/2;  
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x230) = dqsvalue_avg;
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQS EYE Byte1 min_bin:%x, max_bin:%x, avg_bin:%x\n", dqsvalue_min_bin, dqsvalue_max_bin, dqsvalue_avg);
+#endif  
+  // Form the signed number.
+  sbit = (dqsvalue_avg & 0x020) >> 5;
+  if (sbit == 0x1) {
+     sbit = 0x0;
+  } else {
+     sbit = 0x1;
+  }
+  dqsvalue = (dqsvalue_avg & 0x3f);
+  if ((dqsvalue & 0x20) == 0x00) {
+     dqsvalue = (0x20 - dqsvalue);
+  }
+  else {
+     dqsvalue = (dqsvalue & 0x1f);
+  }
+  *(tPVU32)(APB_REG_ADDRESS144) = (*(tPVU32)(APB_REG_ADDRESS144) & 0xffff80ff) | (sbit << 14) | (dqsvalue << 8);
+
+  if (ddr_type != 1) {
+#if defined (HW_ENVPRN)
+    nc_printf("!!! DQSVALUEMIN:%x, DQSVALUEMAX:%x, Range:%d for byte 1 !!! \n", dqsvalue_min_bin, dqsvalue_max_bin, (dqsvalue_max_bin + 1 - dqsvalue_min_bin));
+#endif
+  }
+
+  if (ddr_type != 1) {
+#if defined (HW_ENVPRN)
+  nc_printf("!!! DQS Delay Register 144 :- Byte1 Byte0 Value:%x\n",  (*(tPVU32)(APB_REG_ADDRESS144)));
+#endif
+  }
+
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x234) = (dqsvalue_max_bin + 1 - dqsvalue_min_bin);
+  if (range > (dqsvalue_max_bin + 1 - dqsvalue_min_bin)) {
+     range = (dqsvalue_max_bin + 1 - dqsvalue_min_bin);
+  }
+  *(tPVU32)(LMEMDDRTRAINVAR + 0x238) = range;
+ 
+  return not_found;
+} // eye_training
+
+tU8 fifo_training(tU8 initfifovalue){
+  tU8 fifovalue, not_found, break_loop;
+
+  // Read FIFO setting. 
+  fifovalue = initfifovalue;
+  *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0xffffff0f) | (fifovalue << 4);
+
+  break_loop = 0;
+  not_found = 1;
+  // Decrement SDLYs until data miscompare.
+  while ((break_loop != 1)) {
+    *(tPVU32)(0x80000000) = 0x55555555;
+    *(tPVU32)(0x80000004) = 0xaaaaaaaa;
+    *(tPVU32)(0x80000008) = 0xdeaddead;
+    *(tPVU32)(0x8000000c) = 0xcafecafe;
+    *(tPVU32)(0x80000010) = 0x12345678;
+    *(tPVU32)(0x80000014) = 0x9abcdef0;
+    *(tPVU32)(0x80000018) = 0xbeefbeef;
+    *(tPVU32)(0x8000001c) = 0xbadbcadb;
+    *(tPVU32)(0x80000020) = 0x5a5a5a5a;
+    *(tPVU32)(0x80000024) = 0xa5a5a5a5;
+    *(tPVU32)(0x80000028) = 0xdadadada;
+    *(tPVU32)(0x8000002c) = 0xcececece;
+    *(tPVU32)(0x80000030) = 0x1d3d5d7d;
+    *(tPVU32)(0x80000034) = 0x9bbbdbfb;
+    *(tPVU32)(0x80000038) = 0xbaeabaea;
+    *(tPVU32)(0x8000003c) = 0xbcdcccdc;
+    DELAY(5);
+    if (((READ_REG32(0x80000000) == 0x55555555) && (READ_REG32(0x80000004) == 0xaaaaaaaa) && (READ_REG32(0x80000008) == 0xdeaddead) && (READ_REG32(0x8000000c) == 0xcafecafe)) &&
+	((READ_REG32(0x80000010) == 0x12345678) && (READ_REG32(0x80000014) == 0x9abcdef0) && (READ_REG32(0x80000018) == 0xbeefbeef) && (READ_REG32(0x8000001c) == 0xbadbcadb)) &&
+	((READ_REG32(0x80000020) == 0x5a5a5a5a) && (READ_REG32(0x80000024) == 0xa5a5a5a5) && (READ_REG32(0x80000028) == 0xdadadada) && (READ_REG32(0x8000002c) == 0xcececece)) &&
+	((READ_REG32(0x80000030) == 0x1d3d5d7d) && (READ_REG32(0x80000034) == 0x9bbbdbfb) && (READ_REG32(0x80000038) == 0xbaeabaea) && (READ_REG32(0x8000003c) == 0xbcdcccdc))) {
+      DELAY(5);
+      *(tPVU32)(0x80000000) = 0x0;
+      *(tPVU32)(0x80000004) = 0x0;
+      *(tPVU32)(0x80000008) = 0x0;
+      *(tPVU32)(0x8000000c) = 0x0;
+      *(tPVU32)(0x80000010) = 0x0;
+      *(tPVU32)(0x80000014) = 0x0;
+      *(tPVU32)(0x80000018) = 0x0;
+      *(tPVU32)(0x8000001c) = 0x0;
+      *(tPVU32)(0x80000020) = 0x0;
+      *(tPVU32)(0x80000024) = 0x0;
+      *(tPVU32)(0x80000028) = 0x0;
+      *(tPVU32)(0x8000002c) = 0x0;
+      *(tPVU32)(0x80000030) = 0x0;
+      *(tPVU32)(0x80000034) = 0x0;
+      *(tPVU32)(0x80000038) = 0x0;
+      *(tPVU32)(0x8000003c) = 0x0;
+      READ_REG32(0x80000000);
+      READ_REG32(0x80000004);
+      READ_REG32(0x80000008);
+      READ_REG32(0x8000000c);
+      READ_REG32(0x80000010);
+      READ_REG32(0x80000014);
+      READ_REG32(0x80000018);
+      READ_REG32(0x8000001c);
+      READ_REG32(0x80000020);
+      READ_REG32(0x80000024);
+      READ_REG32(0x80000028);
+      READ_REG32(0x8000002c);
+      READ_REG32(0x80000030);
+      READ_REG32(0x80000034);
+      READ_REG32(0x80000038);
+      READ_REG32(0x8000003c);
+      fifovalue = fifovalue - 1;
+      DELAY(5);
+#if defined (HW_ENVPRN)
+      nc_printf("!!! FIFO value:%x\n",  (*(tPVU32)(APB_REG_ADDRESS14)));
+#endif
+      *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0xffffff0f) | (fifovalue << 4);
+      if (fifovalue == 0x0) {
+	break_loop = 1;
+	not_found = 1;
+      }
+    }
+    else {
+      break_loop = 1;
+      not_found = 0;
+    }  
+  }
+
+  DELAY(5);
+  if (not_found == 0) {
+    fifovalue = fifovalue + 2;
+    *(tPVU32)(APB_REG_ADDRESS14) = (*(tPVU32)(APB_REG_ADDRESS14) & 0xffffff0f) | (fifovalue << 4);
+  }
+
+#if defined (HW_ENV)
+  nc_printf("!!! fifovalue:%x!!! \n", fifovalue);
+#endif
+
+  return not_found;
+}
+
+//
+//Size Calc for x16 part. 
+void ddrsize_calc(tU8 freq_value) {
+  tU8 ddr_tech_type;
+  *(tPVU32)(MC_REG_ADDRESS08) = (*(tPVU32)(MC_REG_ADDRESS08) & 0xff7fffff) | 0x00800000; 
+  *(tPVU32)(0x80000000) = 0x0; // Write at 0.
+  DELAY(5);
+  while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+  *(tPVU32)(0x80000000+0x08000000) = 0x0; // Write at 128MB+1.
+  DELAY(5);
+  while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+  *(tPVU32)(0x80000000+0x10000000) = 0x0; // Write at 256MB+1.
+  DELAY(5);
+  while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+  *(tPVU32)(0x80000000+0x20000000) = 0x0; // Write at 512MB+1.
+  DELAY(5);
+  while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+  *(tPVU32)(0x80000000+0x40000000) = 0x0; // Write at 1GB+1.
+  DELAY(5);
+  while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+  *(tPVU32)(0x80000000) = 0xcafecafe; // Write at 0.
+  DELAY(5);
+  while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+#if defined (DDR3)
+  *(tPVU32)(0x80000000+0x20000000) = 0xdeaddead; // Write at 512MB+1.
+  DELAY(5);
+  while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+  *(tPVU32)(0x80000000) = 0xcafecafe; // Write at 0.
+  DELAY(5);
+  while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+  if (*(tPVU32)(0x80000000+0x20000000) == 0xdeaddead) {
+    // Type is 6.
+    ddr_tech_type = 6;
+  }
+  else {
+    *(tPVU32)(0x80000000+0x10000000) = 0xdadadada; // Write at 256MB+1.
+    DELAY(5);
+    while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+    *(tPVU32)(0x80000000) = 0xcafecafe; // Write at 0.
+    DELAY(5);
+    while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+    if (*(tPVU32)(0x80000000+0x10000000) == 0xdadadada) {
+      // Type 4.
+      ddr_tech_type = 4;
+    }
+    else {
+      *(tPVU32)(0x80000000+0x08000000) = 0xcacacaca; // Write at 128MB+1.
+      DELAY(5);
+      while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+      *(tPVU32)(0x80000000) = 0xcafecafe; // Write at 0.
+      DELAY(5);
+      while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+      if (*(tPVU32)(0x80000000+0x08000000) == 0xcacacaca) {
+	// Type 2.
+	ddr_tech_type = 2;
+
+      } else {
+	// Type 0.
+	ddr_tech_type = 0;
+      }
+    }
+  }
+
+#else
+  *(tPVU32)(0x80000000+0x40000000) = 0xbabababa; // Write at 1GB+1.
+  DELAY(5);
+  while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+  *(tPVU32)(0x80000000) = 0xcafecafe; // Write at 0.
+  DELAY(5);
+  while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+  if (*(tPVU32)(0x80000000+0x40000000) == 0xbabababa) {
+    // Type is 0.
+    ddr_tech_type = 0;
+  }
+  else {
+    *(tPVU32)(0x80000000+0x20000000) = 0xdeaddead; // Write at 512MB+1.
+    DELAY(5);
+    while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+    *(tPVU32)(0x80000000) = 0xcafecafe; // Write at 0.
+    DELAY(5);
+    while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+    if (*(tPVU32)(0x80000000+0x20000000) == 0xdeaddead) {
+      // Type is 6.
+      ddr_tech_type = 6;
+    }
+    else {
+      *(tPVU32)(0x80000000+0x10000000) = 0xdadadada; // Write at 256MB+1.
+      DELAY(5);
+      while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+      *(tPVU32)(0x80000000) = 0xcafecafe; // Write at 0.
+      DELAY(5);
+      while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+      if (*(tPVU32)(0x80000000+0x10000000) == 0xdadadada) {
+	// Type 4.
+	ddr_tech_type = 4;
+      }
+      else {
+	*(tPVU32)(0x80000000+0x08000000) = 0xcacacaca; // Write at 128MB+1.
+        DELAY(5);
+        while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+	*(tPVU32)(0x80000000) = 0xcafecafe; // Write at 0.
+        DELAY(5);
+        while(((*(tPVU32)(MC_REG_ADDRESS14)) & 0x2) == 0);
+	if (*(tPVU32)(0x80000000+0x08000000) == 0xcacacaca) {
+	  // Type 2.
+	  ddr_tech_type = 2;
+	} 
+      }
+    }
+  }
+#endif
+#if defined (DDR4)
+  *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & 0xf1ffffff) | ddr_tech_type << 25;
+  *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x000000ff) | trfc10_array[freq_value+8][ddr_tech_type/2];
+  *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x01fe0000) | trfc_array[freq_value+8][ddr_tech_type/2] << 17;
+#else
+  *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & 0xf1ffffff) | ddr_tech_type << 25;
+  *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x000000ff) | trfc10_array[freq_value][ddr_tech_type/2];
+  *(tPVU32)(MC_REG_ADDRESS04) = (*(tPVU32)(MC_REG_ADDRESS04) & ~0x01fe0000) | trfc_array[freq_value][ddr_tech_type/2] << 17;
+#endif
+
+}
+
+
+
+#endif
+
+
+
+
+
+
+  
