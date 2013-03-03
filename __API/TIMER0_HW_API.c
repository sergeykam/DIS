/**********************************************************************
* Module name: TIMER0_HW_API.c
*
* Copyright 2011 Company as an  unpublished work.
* All Rights Reserved.
*
* The information contained herein is confidential
* property of Company. The user, copying, transfer or
* disclosure of such information is prohibited except
* by express written agreement with Company.
*
* Module Description: realization of TIMER0_HW_API functions for ATmega
***********************************************************************/
/*******************************************************
*	Include Section
********************************************************/
#ifndef	ENABLE_BIT_DEFINITIONS
#define	ENABLE_BIT_DEFINITIONS
#endif

#include "TIMER0_HW_API.h"
/*******************************************************
*	Defines	Section
********************************************************/
#if	defined(__ATmega16__) || defined(__ATmega16A__) 
#define TCCR0 TCCR0
#define TIMSK0 TIMSK
#define TIFR0 TIFR
#define TIFR0_MASK 0xFC
#elif   defined(__ATmega88PA__ ) || defined(__ATmega168PA__)||defined (__ATmega328P__) || defined(__ATmega1281__)
#define TCCR0 TCCR0B
#define TIMSK0 TIMSK0
#define TIFR0 TIFR0
#define TIFR0_MASK 0xFC

#elif	defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega88A__)
#define TCCR0 TCCR0B
#define TIMSK0 TIMSK0
#define TIFR0 TIFR0
#define TIFR0_MASK 0xFC
#elif   defined(__ATmega88PA__ ) || defined(__ATmega168PA__)||defined (__ATmega328P__) || defined(__ATmega1281__)|| defined(__ATmega88P__)
#define TCCR0 TCCR0B
#define TIMSK0 TIMSK0
#define TIFR0 TIFR0
#define TIFR0_MASK 0xFC
#else
	#error "TIMER0_HW_API.h: CPU type not supported"
#endif

/*******************************************************
*	Function Prototype Section
********************************************************/


/*******************************************************
*	Static Variables Section
********************************************************/
static void	(*TIMER0_HW_API_OVF_cb)(void);				//timer overfolow callback pointer

/*******************************************************************************
* Function name	: TIMER0_HW_API_init (void (*callback)(void))
*void (*callback)(void)	: user's callback
* Created by	: 
* Date created	: 
* Description	: initialization of Hardware Timer0 
* Notes			: 
*******************************************************************************/
void TIMER0_HW_API_init (void (*callback)(void))
{
	TCCR0 = 0;
	TIFR0 &= TIFR0_MASK;
	TCNT0 = 0;
	TIMSK0 |= 1;
	
	if(callback)
		TIMER0_HW_API_OVF_cb = callback;
	
	TCCR0 |= TIMER0_HW_PRC;
	return;
}
/*******************************************************************************
* Function name	: void TIMER0_HW_OVF_ISR (void)
* Created by	: 
* Date created	: 
* Description	: Timer0 OVF ISR
* Notes			: 
*******************************************************************************/
#pragma	vector = TIMER0_OVF_vect
__interrupt void TIMER0_HW_OVF_ISR (void)
{
	if(TIMER0_HW_API_OVF_cb)
	  TIMER0_HW_API_OVF_cb();
	return;
}
/* end of file */