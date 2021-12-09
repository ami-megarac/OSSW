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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#endif

#include <linux/errno.h>
#include <linux/major.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
//#include <linux/ide.h>

#include "kcs.h"	



#if (LINUX_VERSION_CODE >  KERNEL_VERSION(3,14,17))
static struct KcsTimerObj{
	struct timer_list poll_timer;
	Kcs_OsSleepStruct *dev;
	unsigned long data;
}kcsTimerObj;
extern KCSBuf_T* GlobalKCSBuffer;
#endif

void Kcs_OsInitSleepStruct(Kcs_OsSleepStruct *Sleep);
void Kcs_OsWakeupOnTimeout(Kcs_OsSleepStruct *Sleep);
long Kcs_OsSleepOnTimeout(Kcs_OsSleepStruct *Sleep,u8 *Var,long msecs);

#if (LINUX_VERSION_CODE > KERNEL_VERSION (3,14,17))
	void SleepTimeOut(struct timer_list *timer_list);	
#else
	void SleepTimeOut(unsigned long SleepPtr);
#endif


void 
Kcs_OsInitSleepStruct(Kcs_OsSleepStruct *Sleep)
{
#if (LINUX_VERSION_CODE > KERNEL_VERSION (3,14,17))	
	init_waitqueue_head(&(Sleep->queue));
	Sleep->Timeout = 0;
#else
	init_waitqueue_head(&(Sleep->queue));
	init_timer(&(Sleep->tim));  
	Sleep->Timeout = 0;
#endif
	return;
}


void 
Kcs_OsWakeupOnTimeout(Kcs_OsSleepStruct *Sleep)
{
	//int dtimer;

	/* Wakeup Process and Kill timeout handler */
#if (LINUX_VERSION_CODE > KERNEL_VERSION (3,14,17))	
	if(!in_softirq())
	 {
		del_timer(&kcsTimerObj.poll_timer);
	 }
#else
	del_timer(&(Sleep->tim));
#endif	
	wake_up(&(Sleep->queue));

	if (Sleep->Timeout)
		printk("WARNING:Wakeup for Sleep Struct Called after timeout\n");
	return;
}



long 
Kcs_OsSleepOnTimeout(Kcs_OsSleepStruct *Sleep,u8 *Var,long msecs)
{
	long timeout;	/* In jiffies */
	volatile u8 *volatile Condition = Var; 

	/* Convert milliseconds into jiffies*/
	timeout = (HZ*msecs)/1000;
#if (LINUX_VERSION_CODE < KERNEL_VERSION (3,14,17))
	/* Start the timeout */
	Sleep->tim.function = (void*)SleepTimeOut;
	/* Removed in latets Kernel*/
	Sleep->tim.data     = (unsigned long)Sleep;
	Sleep->tim.expires  = jiffies+timeout;
	Sleep->Timeout      = 0;
	if (msecs)
		add_timer(&(Sleep->tim));
#if DEBUG_ARCH
	printk("Sleeping on Sleep Structure\n");
#endif
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION (3,14,17))
	if(!timer_pending(&kcsTimerObj.poll_timer))
	{
		timer_setup(&kcsTimerObj.poll_timer,SleepTimeOut,0);
		kcsTimerObj.poll_timer.expires = jiffies+timeout;
		kcsTimerObj.dev = &GlobalKCSBuffer->KcsReqWait;
		add_timer (&kcsTimerObj.poll_timer);
	}
	Sleep->Timeout      = 0;

#endif
	

	/* Sleep on the Condition for a wakeup */
	wait_event_interruptible(Sleep->queue,((*Condition)||(Sleep->Timeout)));

	/* Normally del_timer will be called by Wakeup or timeout routine */
	/* But when wakeup is called before add_timer() and wait_event(),
	   it will not be killed by wakeup routine and will be killed only
	   after a timeout happens. So we kill it here as soon as possible */
#if (LINUX_VERSION_CODE > KERNEL_VERSION (3,14,17))
	 if(!in_softirq())
	 {
		del_timer(&kcsTimerObj.poll_timer);
	 }
#else
	if (msecs)
		del_timer(&Sleep->tim);
#endif

	if (Sleep->Timeout)
		return 0;
	return timeout;
}

void 
#if (LINUX_VERSION_CODE > KERNEL_VERSION (3,14,17))
 SleepTimeOut(struct timer_list *timer_list)
#else
 SleepTimeOut(unsigned long SleepPtr)
#endif
{	
	
#if(LINUX_VERSION_CODE > KERNEL_VERSION (3,14,17))
	struct KcsTimerObj *kcsTimerObj = from_timer(kcsTimerObj, timer_list, poll_timer);
	Kcs_OsSleepStruct *Sleep = kcsTimerObj->dev;

	
	/* Set Timeout and Wakeup Sleep Process */
	Sleep->Timeout = 1;
	wake_up(&(Sleep->queue));
#else
	Kcs_OsSleepStruct *Sleep = (Kcs_OsSleepStruct *)SleepPtr;
	Sleep->Timeout = 1;
	wake_up(&(Sleep->queue));
#endif
	return;
}


