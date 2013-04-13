/***************************************************
* Module name: template_API.h
*
* Copyright __year__ Company as an  unpublished work.
* All Rights Reserved.
*
* The information contained herein is confidential
* property of Company. The user, copying, transfer or
* disclosure of such information is prohibited except
* by express written agreement with __Company__.
*
* First written		on __date__ by __user__
* Redesigned 		on __date__ by __user__

* Module Description: connected module template_API for AVR
****************************************************/
/***************************************************
*  Include section
***************************************************/
#include "device.h"
/***************************************************
*   Defines section
***************************************************/
enum TIMER0_PRC
{
	TIMER0_PRC_0 = 1,
	TIMER0_PRC_8,
	TIMER0_PRC_64,
	TIMER0_PRC_256,
	TIMER0_PRC_1024
};
#if !defined (CPU_CLK)
#error "device.h : user not define CPU clock"
#endif
/***************************************************
*  User Settings section
***************************************************/
#define TIMER0_HW_PRC TIMER0_PRC_1024
//#if (TIMER0_HW_PRC == TIMER0_PRC_EXT_F) || (TIMER0_HW_PRC == TIMER0_PRC_EXT_R)
#define TIMER0_HW_TICK_US ((TIMER0_HW_PRC*1000000)/CPU_CLK)
#define TIMER0_HW_OVF_TICK_MS ((TIMER0_HW_TICK_US*256)/1000)
/***************************************************
*   API Function Prototype Section
***************************************************/
void TIMER0_HW_API_init (void (*callback)(void));
/* end of file */