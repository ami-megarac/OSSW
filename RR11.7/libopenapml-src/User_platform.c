/*
 * User Platform Layer and OS Customization - V1.0
 *
 ******************************************************************************


===========================================================
License Agreement

Copyright (c) 2008, 2009 Advanced Micro Devices, Inc.

All rights reserved.

Redistribution and use in any form of this material and any product thereof
including software in source or binary forms, along with any related
documentation, with or without modification ("this material"), is permitted
provided that the following conditions are met:

+ Redistributions of source code of any software must retain the above
copyright notice and all terms of this license as part of the code.

+ Redistributions in binary form of any software must reproduce the above
copyright notice and all terms of this license in any related documentation
and/or other materials.

+ Neither the names nor trademarks of Advanced Micro Devices, Inc. or any
copyright holders or contributors may be used to endorse or promote products
derived from this material without specific prior written permission.

+ Notice about U.S. Government restricted rights: This material is provided
with RESTRICTED RIGHTS. Use, duplication or disclosure by the U.S. Government
is subject to the full extent of restrictions set forth in FAR52.227 and
DFARS252.227 et seq., or any successor or applicable regulations. Use of this
material by the U.S. Government constitutes acknowledgment of the proprietary
rights of Advanced Micro Devices, Inc. and any copyright holders and
contributors.

+ In no event shall anyone redistributing or accessing or using this material
commence or participate in any arbitration or legal action relating to this
material against Advanced Micro Devices, Inc. or any copyright holders or
contributors. The foregoing shall survive any expiration or termination of this
license or any agreement or access or use related to this material.

+ ANY BREACH OF ANY TERM OF THIS LICENSE SHALL RESULT IN THE IMMEDIATE
REVOCATION OF ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE THIS MATERIAL.

THIS MATERIAL IS PROVIDED BY ADVANCED MICRO DEVICES, INC. AND ANY COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" IN ITS CURRENT CONDITION AND WITHOUT ANY
REPRESENTATIONS, GUARANTEE, OR WARRANTY OF ANY KIND OR IN ANY WAY RELATED TO
SUPPORT, INDEMNITY, ERROR FREE OR UNINTERRUPTED OPERATION, OR THAT IT IS FREE
FROM DEFECTS OR VIRUSES.  ALL OBLIGATIONS ARE HEREBY DISCLAIMED - WHETHER
EXPRESS, IMPLIED, OR STATUTORY - INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED
WARRANTIES OF TITLE, MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
ACCURACY, COMPLETENESS, OPERABILITY, QUALITY OF SERVICE, OR NON-INFRINGEMENT.
IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, PUNITIVE,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, REVENUE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED OR BASED ON ANY THEORY OF LIABILITY
ARISING IN ANY WAY RELATED TO THIS MATERIAL, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE. THE ENTIRE AND AGGREGATE LIABILITY OF ADVANCED MICRO DEVICES,
INC. AND ANY COPYRIGHT HOLDERS AND CONTRIBUTORS SHALL NOT EXCEED TEN DOLLARS
(US $10.00). ANYONE REDISTRIBUTING OR ACCESSING OR USING THIS MATERIAL ACCEPTS
THIS ALLOCATION OF RISK AND AGREES TO RELEASE ADVANCED MICRO DEVICES, INC. AND
ANY COPYRIGHT HOLDERS AND CONTRIBUTORS FROM ANY AND ALL LIABILITIES,
OBLIGATIONS, CLAIMS, OR DEMANDS IN EXCESS OF TEN DOLLARS (US $10.00). THE
FOREGOING ARE ESSENTIAL TERMS OF THIS LICENSE AND, IF ANY OF THESE TERMS ARE
CONSTRUED AS UNENFORCEABLE, FAIL IN ESSENTIAL PURPOSE, OR BECOME VOID OR
DETRIMENTAL TO ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR
CONTRIBUTORS FOR ANY REASON, THEN ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE
THIS MATERIAL SHALL TERMINATE IMMEDIATELY. MOREOVER, THE FOREGOING SHALL
SURVIVE ANY EXPIRATION OR TERMINATION OF THIS LICENSE OR ANY AGREEMENT OR
ACCESS OR USE RELATED TO THIS MATERIAL.

NOTICE IS HEREBY PROVIDED, AND BY REDISTRIBUTING OR ACCESSING OR USING THIS
MATERIAL SUCH NOTICE IS ACKNOWLEDGED, THAT THIS MATERIAL MAY BE SUBJECT TO
RESTRICTIONS UNDER THE LAWS AND REGULATIONS OF THE UNITED STATES OR OTHER
COUNTRIES, WHICH INCLUDE BUT ARE NOT LIMITED TO, U.S. EXPORT CONTROL LAWS SUCH
AS THE EXPORT ADMINISTRATION REGULATIONS AND NATIONAL SECURITY CONTROLS AS
DEFINED THEREUNDER, AS WELL AS STATE DEPARTMENT CONTROLS UNDER THE U.S.
MUNITIONS LIST. THIS MATERIAL MAY NOT BE USED, RELEASED, TRANSFERRED, IMPORTED,
EXPORTED AND/OR RE-EXPORTED IN ANY MANNER PROHIBITED UNDER ANY APPLICABLE LAWS,
INCLUDING U.S. EXPORT CONTROL LAWS REGARDING SPECIFICALLY DESIGNATED PERSONS,
COUNTRIES AND NATIONALS OF COUNTRIES SUBJECT TO NATIONAL SECURITY CONTROLS.
MOREOVER, THE FOREGOING SHALL SURVIVE ANY EXPIRATION OR TERMINATION OF ANY
LICENSE OR AGREEMENT OR ACCESS OR USE RELATED TO THIS MATERIAL.

This license forms the entire agreement regarding the subject matter hereof and
supersedes all proposals and prior discussions and writings between the parties
with respect thereto. This license does not affect any ownership, rights,
title, or interest in, or relating to, this material. No terms of this license
can be modified or waived, and no breach of this license can be excused, unless
done so in a writing signed by all affected parties. Each term of this license
is separately enforceable. If any term of this license is determined to be or
becomes unenforceable or illegal, such term shall be reformed to the minimum
extent necessary in order for this license to remain in effect in accordance
with its terms as modified by such reformation. This license shall be governed
by and construed in accordance with the laws of the State of Texas without
regard to rules on conflicts of law of any state or jurisdiction or the United
Nations Convention on the International Sale of Goods. All disputes arising out
of this license shall be subject to the jurisdiction of the federal and state
courts in Austin, Texas, and all defenses are hereby waived concerning personal
jurisdiction and venue of these courts.
============================================================


 ******************************************************************************
 */


#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


//#include "i2c-dev.h"
#include "libi2c.h"
#include "hal_hw.h"
#include "IPMIConf.h"
#include "Apml_fdk.h"

/* Holds i2c bus name string */
static char m_i2c_bus_name [64];

#define MAX_I2C_RETRIES 3


/**
 * @fn get_i2c_bus_name
 * @brief This function is used by all below i2c apis to get bus name
 *
**/
char*
get_i2c_bus_name (int bus_num)
{
   /* Conver Bus number into corresponding i2c device */
   sprintf(m_i2c_bus_name,"/dev/i2c%d", bus_num);

   return &m_i2c_bus_name[0];
}

/* **************************************************************************/
/*                                                                          */
/*                          Private Static Defines                          */
/*                                                                          */
/* **************************************************************************/

const uint8 rmi_addr_lst[] = {
	0x3C,
	0x3D,
	0x3E,
	0x3F,
	0x38,
	0x39,
	0x3A,
	0x3B 
};

/* Maps user_platform status messages to descriptions */
const USER_ERR_DESC err_desc[] = {
	{ APML_FAILURE,	"Error in Communicating with APML" },
};


/* **************************************************************************/
/*                                                                          */
/*          Mandatory Private User Platform Function Implementations        */
/*                                                                          */
/* **************************************************************************/



/* Called by FDK logging, prints user platform error message */
void apml_print_user_error(uint32 res)
{
	uint32 i, desc_len;

	desc_len = sizeof(err_desc) / sizeof(USER_ERR_DESC);

	for (i = 0; i < desc_len; i++)
	{
		if (err_desc[i].code == res)
		{
			printf("%s", err_desc[i].desc);
			return;
		}
	}
}



/* Called at FDK initialization to configure the FDK and read the current
 * processor status
 *
 * A robust implementation should read RMI 0x41 register and set the 
 * dev->rmi_core_target.  The default works for now however. */
uint32 user_platform_init(APML_DEV_CTL *dev,int BMCInst)
{
	uint8			tmp8;
	uint32			i,socket, res,die;
#ifndef CONFIG_SPX_FEATURE_APML_ONLY_TSI_SUPPORT
//	uint32 j;
#endif
	USER_PLATFORM_DATA	*platform;
	BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];

	platform = &dev->platform;
	/* Initialize platform structure */
	for (socket=0; socket < APML_MAX_PROCS; socket++)
    { 
         if (socket == 0)
         {
            for (die=0; die < APML_MAX_DIES; die++){
		            platform->addr[socket][die] = rmi_addr_lst[die];
                    //printf("platform->addr[%d][%d] = 0x%x\n",socket,die, platform->addr[socket][die]);
                }
         }
         else if (socket == 1)
         {
                for (die=0; die < APML_MAX_DIES; die++){
                    platform->addr[socket][die] = rmi_addr_lst[die + 4]; 
                    //printf("platform->addr[%d][%d] = 0x%x\n",socket,die, platform->addr[socket][die]);
                    }
         }
        
    }
	platform->last_addr_val = 0;
	platform->apml_smbus_number = pBMCInfo->IpmiConfig.APMLBusNumber;

#ifdef CONFIG_SPX_FEATURE_APML_ONLY_TSI_SUPPORT
	//j=0;
	/* Iterate each possible processor in the platform */
	for (i = 0; i < APML_MAX_PROCS; i++) {
		/* Configure each proc to have 1 core to send commands 
		   without errors from user_get_proc_addr() */
		dev->cores[i] = 1;

		/* Read the CPU temp via TSU reg#1*/
		res = apml_read_tsi_reg(dev, i,0, 1, &tmp8,BMCInst);		
		
		/* If there was an error, disable the processor */
		if (res != APML_SUCCESS) {
			platform->addr[i] = 0;
			dev->cores[i] = 0;
			
		}
	}

#else
	/* Iterate each possible processor in the platform.*/
	for (i = 0; i < APML_MAX_PROCS; i++) {
		/* Configure each proc to have 1 core to send commands 
		   without errors from user_get_proc_addr() */
		dev->cores[i] = 1;

		/* Read the APML version */
		res = apml_read_rmi_reg(dev, i,0, 0, 0x00, &tmp8,BMCInst);//using first Die should be ok
		if (res == APML_SUCCESS) {
			dev->rmi_rev[i] = tmp8;

			/* This FDK only understands 0x02 (version 1.0)& 0x03 (version 1.1) */
             if ( !( tmp8 == 0x03 ||tmp8 == 0x02 || tmp8 == 0x01 || tmp8 == 0x10) ) {  //latest version
				return(APML_BAD_RMI_VERSION);
			}

			/* Enable TSI SMBus timeouts */
            for (die=0; die < APML_MAX_DIES; die++)
            {
			    res = apml_write_rmi_reg(dev, i,die, PROC_USE_TSI,
				    0x22, 0x80,BMCInst);
			    if (res == APML_SUCCESS) {
				/* Turn on all APML PEC */
				tmp8 = 0xE1;
				    res = apml_write_rmi_reg(dev, i,die, 0, 0x01, tmp8,BMCInst); 
				if (res == APML_SUCCESS) {
                       printf("Skip checking core enable status registers because the AMD firmware is not ready\n");
                        #if 0

					    /* Read core enabled status 0-7 */
					    res = apml_read_rmi_reg(dev, i, die,0,
						    0x04, &tmp8,BMCInst); 
					    if (res == APML_SUCCESS) {
                            printf(" what do we have for read core enable status reg 0-7 = 0x%x\n",tmp8);
						    /* Store # of cores */
						    for (j=1; j < 256; j = j * 2) {
							    if ((tmp8 & j) == j) {
								    dev->cores[i] = dev->cores[i] + 1;
                                    printf("read core enable status 0-7, dev->cores[%d] = %d\n",i,dev->cores[i]);
							    }
						    }

						/* Read core enabled status 8-15 */
						    res = apml_read_rmi_reg(dev, i, die,0, 0x05, &tmp8,BMCInst); 
						if (res == APML_SUCCESS) {

							/* Store # of cores */
							for (j=1; j < 256; j = j * 2) {
								if ((tmp8 & j) == j) {
									dev->cores[i] = dev->cores[i] + 1;
									}
								
							}
						}
						dev->cores[i]--; /* Remove the fake core we added to start */
					}
                        #endif
				}
			}
            }
		}

		/* If there was an error, disable the processor */
		if (res != APML_SUCCESS) {
			dev->cores[i] = 0;
            for (die=0; die<APML_MAX_DIES;die++)
			    platform->addr[i][die] = 0;
		}
	}
	
#endif

	return(APML_SUCCESS);
}



/* Gracefully closes the FDK */
uint32 user_platform_close(APML_DEV_CTL *ctl)
{
	uint32 counter;
    uint32 j;

	/* Invalidate all of the processors */
	for (counter = 0; counter < APML_MAX_PROCS; counter++) {
        for (j = 0; j < APML_MAX_DIES; j++) {
		ctl->platform.addr[counter][j] = 0;
        }
		ctl->cores[counter] = 0;
	}

	return(APML_SUCCESS);
}



/* SMBus Write byte - transmits data[7:0] */
uint32 user_smbus_write_byte(
	APML_DEV_CTL	*dev,
	uint8 		proc_addr,
	uint32		reg,
	uint32		data,
	int			BMCInst)
{

    uint32	res = APML_SUCCESS;
    uint8	retries = 0, wr_len=sizeof(uint8);
    int32	uerr;
    BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];

    if(0)
    {
    	dev=dev; /* -Wextra, fix for unused parameters */
    	data=data;
    }
    if(g_HALI2CHandle[HAL_I2C_MW] != NULL)
    {
        for (retries = 0; retries < MAX_I2C_RETRIES; ++retries)
        {
            uerr = ((ssize_t(*)(char *,u8,u8 *,size_t))g_HALI2CHandle[HAL_I2C_MW]) (get_i2c_bus_name((int)pBMCInfo->IpmiConfig.APMLBusNumber), proc_addr, (uint8*) &reg, wr_len);
             
            if(uerr >= 0){
                break;
            }
        }

        if (retries == MAX_I2C_RETRIES)
        {
            res = APML_FAILURE;
        }
    }
    else
    {
        res = APML_FAILURE;
    }

    return(res);
}



/* SMBus write word - transmits data[7:0] first, data[15:8] second */
uint32 user_smbus_write_word(
	APML_DEV_CTL	*dev,
	uint8		proc_addr,
	uint32		reg,
	uint32		data,
	int			BMCInst)
{

    uint32	res= APML_SUCCESS;
    uint8  writeBuf[2];
    writeBuf[0] = reg;
    writeBuf[1] = data;
    int32   uerr;
    uint8	retries = 0, wr_len=sizeof(writeBuf);
    BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];

    if(0)
    {
    	dev=dev; /* -Wextra, fix for unused parameters */
    }
    if(g_HALI2CHandle[HAL_I2C_MW] != NULL)
    {
        for (retries = 0; retries < MAX_I2C_RETRIES; ++retries)
        {
            uerr = ((ssize_t(*)(char *,u8,u8 *,size_t))g_HALI2CHandle[HAL_I2C_MW]) 
                (get_i2c_bus_name((int)pBMCInfo->IpmiConfig.APMLBusNumber), proc_addr, (uint8*) &writeBuf[0], wr_len);
             
            if(uerr >= 0){
                break;
            }
        }
        if (retries == MAX_I2C_RETRIES)
        {
            res = APML_FAILURE;
        }
    }
    else
    {
        res = APML_FAILURE;
    }

    return (res);
}



/* SMBus read byte - 1 byte placed into data[7:0] */
uint32 user_smbus_read_byte(
	APML_DEV_CTL	*dev,
	uint8		proc_addr,
	uint32		reg,
	uint32		*data,
	int			BMCInst)
{
    uint32	res = APML_SUCCESS;
    int32	uerr;
    uint8 retries = 0;
    uint8	wr_len, rd_len;
    wr_len = rd_len = sizeof(uint8);
    BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];

    if(0)
    {
    	dev=dev; /* -Wextra, fix for unused parameters */
    }
    if(g_HALI2CHandle[HAL_I2C_RW] != NULL)
    {
        for (retries = 0; retries < MAX_I2C_RETRIES; ++retries)
        {
                 uerr = ((int(*)(char *,u8,u8 *,u8 *,size_t,size_t))g_HALI2CHandle[HAL_I2C_RW]) (get_i2c_bus_name((int)pBMCInfo->IpmiConfig.APMLBusNumber),
                                                        proc_addr, (uint8*) &reg, (uint8 *) data, wr_len, rd_len);
                if(uerr >= 0){
                    break;
                }
        }

    	if (retries == MAX_I2C_RETRIES)
    	{
    		res = APML_FAILURE;	
    	}
    }
    else
    {
        res = APML_FAILURE;
    }

	return (res);
}



/* SMBus read word - The first byte on the bus is placed into data[7:0],
 * the second byte into data[15:8] */
uint32 user_smbus_read_word(
	APML_DEV_CTL	*dev,
	uint8		proc_addr,
	uint32		reg,
	uint32		*data,
	int			BMCInst)
{
    uint32	res = APML_SUCCESS;
    int32	uerr;
    uint8 retries = 0;
    BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];

    if(0)
    {
    	dev=dev; /* -Wextra, fix for unused parameters */
    }
    if(g_HALI2CHandle[HAL_I2C_RW] != NULL)
    {
        for (retries = 0; retries < MAX_I2C_RETRIES; ++retries)
        {
             uerr = ((int(*)(char *,u8,u8 *,u8 *,size_t,size_t))g_HALI2CHandle[HAL_I2C_RW]) (get_i2c_bus_name((int)pBMCInfo->IpmiConfig.APMLBusNumber),
                                                proc_addr, (uint8*) &reg, (uint8*)data,sizeof(uint8), sizeof(uint16));
             
            if(uerr >= 0){
                break;
            }

        }
        
        if (retries == MAX_I2C_RETRIES)
        {
            res = APML_FAILURE;
        }
    }
    else
    {
        res = APML_FAILURE;
    }

    return (res);
}



/* SMBus block write-block read process call
 * xmit_data[0] is the first byte transmitted on the wire and 
 * xmt_data[xmt_len-1] is the last.
 * rcv_data[0] is the first byte received after the repeated start. */
uint32 user_smbus_bwr_brd_process(
	APML_DEV_CTL	*dev,
	uint8		proc_addr,
	uint8		*xmt_data,
	uint32		xmt_len,
	uint8		*rcv_data,
	uint32		rcv_len,
	int			BMCInst)
{
    int32	uerr;
    uint32	res= APML_SUCCESS;
    uint8 retries = 0;
    BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];

    if(0)
    {
    	dev=dev; /* -Wextra, fix for unused parameters */
    }
    if(g_HALI2CHandle[HAL_I2C_RW] != NULL)
    {
        for (retries = 0; retries < MAX_I2C_RETRIES; ++retries)
        {
            uerr = ((int(*)(char *,u8,u8 *,u8 *,size_t,size_t))g_HALI2CHandle[HAL_I2C_RW]) (get_i2c_bus_name((int)pBMCInfo->IpmiConfig.APMLBusNumber),
                        proc_addr, xmt_data, rcv_data, xmt_len, rcv_len);
             
            if(uerr >= 0){
                break;
            }
        }
        if (retries == MAX_I2C_RETRIES)
        {
            res = APML_FAILURE;
        }
    }
    else
    {
        res = APML_FAILURE;
    }

    return (res);
}



/* Convert a processor enumeration into a 7-bit processor SMBus address */
uint32 user_get_proc_addr(
	APML_DEV_CTL	*dev,
	//uint8		proc,
	uint8       socketid,
	uint8       dieid,
	uint8		options,
	uint8		*addr)
{
	*addr = 0;

	if (socketid >= APML_MAX_PROCS) {
		return(APML_BAD_PROC_NDX);
	}

    if (dieid >= APML_MAX_DIES) {
		return(APML_BAD_DIE_NDX);
	}
	if (dev->cores[socketid] == 0) {
		return(APML_UNK_PROC_NUM);
	}

	*addr = dev->platform.addr[socketid][dieid];
	if ((options & PROC_USE_TSI) == PROC_USE_TSI) {
		*addr += USE_TSI_ADDR;
	}

	return(APML_SUCCESS);
}

