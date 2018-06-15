/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

#include <linux/version.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#endif
#include <linux/errno.h>
#include <linux/major.h>
#include <asm/uaccess.h>
#include "bt_timeout.h"	

void 
Bt_OsInitSleepStruct(Bt_OsSleepStruct *Sleep)
{
	init_waitqueue_head(&(Sleep->queue));
	return;
}

void 
Bt_OsWakeupOnTimeout(Bt_OsSleepStruct *Sleep)
{
	/* Wakeup Process */
    wake_up(&(Sleep->queue));
}

long 
Bt_OsSleepOnTimeout(Bt_OsSleepStruct *Sleep,u8 *Var,long msecs)
{
	long timeout;	/* In jiffies */
	u8 *volatile Condition = Var;

	/* Convert milliseconds into jiffies*/
	timeout = (HZ*msecs)/1000; /* Fortify issue :: False Positive */

	/* Sleep on the Condition for a wakeup */
	if (msecs)
	{
		return wait_event_interruptible_timeout(Sleep->queue,(*Condition), timeout); /* Fortify Issue:[Poor style]- False positive*/
	}
	else
	{
		wait_event_interruptible(Sleep->queue, (*Condition));
		return 1;
	
	}
}


