/*********************************************************************
* Module name: TIMER1_HW_API.c
*
* Copyright	2011 TeaMiX as an unpublished work.
* All Rights Reserved.
*
* The information contained herein is confidential
* property of TeaMiX. The user, copying, transfer or
* disclosure of such information is prohibyted except
* by express written agreement with TeaMiX.
*
* First	written		on 20.02.2011 by ezh
* Redesigned 		on ______ by _____
*
* Module Description: API for 16-bit Timer1 for ATmega uC
*********************************************************************/
/***************************************************
*	Include Section
***************************************************/
#include "TIMER1_HW_API.h"

/***************************************************
*	Defines section
***************************************************/
#ifndef TIMER1_CAPT_MODE
#define TIMER1_NOISE_CANCELER 0
#define TIMER1_CAPTURE_EDGE 0
#endif

#if defined(__ATmega1281__) || defined (__ATmega88PA__)||defined (__ATmega168PA__)||defined (__IOM328P_H)
#define TIMSK          ( TIMSK1 )
#define TIFR          ( TIFR1 )
#define TICIE1          ( ICIE1 )
#elif defined(__ATmega16__)
#define TIMSK		( TIMSK )
#define TIFR		( TIFR )
#else
	#error	"uC	type undescribed"
#endif

enum
{
	OK = 0,
	ERROR
};
/***************************************************
*	Static Variables Section
***************************************************/
static U32 TIMER1_time = 0;
static U16 TIMER1_back_up_cnt = 0;
static U8 TIMER1_lock_A = 0;
static U8 TIMER1_lock_B = 0;
static void (*TIMER1_timeout_cb)(void);
static void (*TIMER1_timeout_B_cb)(void);
static void (*TIMER1_ovf_cb)(void);
static void (*TIMER1_capt_cb)(U16 timer_cnt);
/***************************************************
*	Function Prototype Section
***************************************************/

/***************************************************************************
* Function name	: U8 TIMER1_timeout_A(U16 time_cnt, void (*callback)(void))
* U16 time_cnt : timeout by compare A
* void (*callback)(void) : user's callback by compare A interrupt
* Created by	: ezh
* Date created	: 20.02.2011 
* Description	: start count timeout and then call callback
* Notes			: enable compare A interrupt 
***************************************************************************/
U8 TIMER1_timeout_A(U16 time_cnt, void (*callback)(void))
{
  	U8 timer_lock = ERROR;
        LOCK_RESOURCE_OR_RETURN(TIMER1_lock_A, timer_lock);
	U16 TIMER1_cnt_temp = TCNT1;							// save LOW byte CNT
	U16 TIMER1_comp_a_temp = (time_cnt / TIMER1_TICK_US);	//set time for timeout in simbols
	TIMER1_comp_a_temp += TIMER1_cnt_temp;					//add cnt shift
	OCR1A = TIMER1_comp_a_temp;								// save HI byte of timeout
	TIMER1_timeout_cb = callback;							//save pointer
	TIFR = ( 1 << OCF1A);
	TIMSK |= (1 << OCIE1A); 								// set compare A interrupt
	if(!TCCR1B&0x07)
	  TCCR1B |= (TIMER1_PRC); 								// start timer 1  
	return(OK);												//return OK
}

/***************************************************************************
* Function name	: U8 TIMER1_timeout_B(U16 time_cnt, void (*callback)(void))
* U16 time_cnt : timeout by compare B
* void (*callback)(void) : user's callback by compare B interrupt
* Created by	: ezh
* Date created	: 20.02.2011 
* Description	: start count timeout and then call callback
* Notes			: enable compare B interrupt 
***************************************************************************/
U8 TIMER1_timeout_B(U16 time_cnt, void (*callback)(void))
{
  	U8 timer_lock = ERROR;
        LOCK_RESOURCE_OR_RETURN(TIMER1_lock_B, timer_lock);
	U16 TIMER1_cnt_temp = TCNT1;							// save LOW byte CNT
	U16 TIMER1_comp_a_temp = (time_cnt / TIMER1_TICK_US);	//set time for timeout in simbols
	TIMER1_comp_a_temp += TIMER1_cnt_temp;					//add cnt shift
	OCR1B = TIMER1_comp_a_temp;								// save HI byte of timeout
	TIMER1_timeout_B_cb = callback;							//save pointer
	TIFR = ( 1 << OCF1B);
	TIMSK |= (1 << OCIE1B); 								// set compare B interrupt
	if(!TCCR1B&0x07)
	  TCCR1B |= (TIMER1_PRC); 								// start timer 1  
	return(OK);												//return OK
}

/*********************************************************
* Function name	: void TIMER1_timeout_A_break(void)
* Created by	: ezh
* Date created	: 20.02.2011 
* Description	: break timeout_A
* Notes			: disable and unlock compare A interrupt 
*********************************************************/
void TIMER1_timeout_A_break(void)
{
	CRITICAL_SECTION(TIMSK &= ~(1 << OCIE1A)); 				// clear compare A interrupt 
	TIFR = ( 1 << OCF1A);
	TIMER1_lock_A = 0;
}

/*********************************************************
* Function name	: void TIMER1_timeout_break_B(void)
* Created by	: ezh
* Date created	: 20.02.2011 
* Description	: break timeout_B
* Notes			: disable and unlock compare B interrupt 
**********************************************************/
void TIMER1_timeout_break_B(void)
{
	CRITICAL_SECTION(TIMSK &= ~(1 << OCIE1B)); 				// clear compare B interrupt
	TIFR = ( 1 << OCF1B);
	TIMER1_lock_B = 0;
}

/********************************************************************
* Function name	: U8 TIMER1_start_time_count (void (*callback)(void))
* void (*callback)(void) : user's callback by overflow interrupt
* Created by	: ezh
* Date created	: 20.02.2011 
* Description	: start time count and call callback by OVF interrupt 
* Notes			: enable overflow interrupt
********************************************************************/
U8 TIMER1_start_time_count (void (*callback)(void))
{
	if(callback)											//check callback pointer
		TIMER1_ovf_cb = callback;							//save pointer
	else
		return(ERROR);										//else - return error
	TIFR |= ( 1 << TOV1);
  	TIMSK |= (1 << TOIE1); 									// set overflow interrupt
  	TCCR1B |= (TIMER1_PRC); 								// start timer 1
	return(OK);												//return OK
}

/*****************************************
* Function name	: U8 TIMER1_stop (void)
* Created by	: ezh
* Date created	: 20.02.2011 
* Description	: stop timer 1
* Notes			: 
******************************************/
void TIMER1_stop (void)
{
	CRITICAL_SECTION(TCCR1B &= 0xf8);						//stop timer
}
/**************************************************************************
* Function name	: U8 TIMER1_stop (void)
* Created by	: ezh
* Date created	: 20.02.2011 
* Description	: reset time
* Notes			: stop timer, save OCR1A and OCR1B, reset TCNT, start timer
**************************************************************************/
void TIMER1_reset (void)
{
  CRITICAL_SECTION(TCCR1B &= 0xf8);      					//stop timer
  U8 temp = (TIMSK &((1<<TICIE1)|(1<<OCIE1A)|(1<<OCIE1B)|(1<<TOIE1)));
  TIMSK &=~((1<<TICIE1)|(1<<OCIE1A)|(1<<OCIE1B)|(1<<TOIE1));     //Disable TC0 interrupt
  
U16 cnt_temp = 0; 
U16 ocr_temp = 0;
cnt_temp = TCNT1;
  if((temp & (1<<OCIE1A))&&(~(TIFR &(1 << OCF1A))))
  {
	ocr_temp = OCR1A;										//save OCR1A
	if(ocr_temp > cnt_temp)
		ocr_temp -= cnt_temp;
	else
		ocr_temp += (0xFFFF - cnt_temp);
		
	OCR1AH = (U8)(ocr_temp >> 8);
	OCR1AL = (U8)(ocr_temp);
  }
    
  if((temp & (1<<OCIE1B))&&(~(TIFR &(1 << OCF1B))))
  {
	ocr_temp = OCR1B;										//save OCR1B
	if(cnt_temp < ocr_temp)
		ocr_temp -= cnt_temp;
	else
		ocr_temp += (0xFFFF - cnt_temp);	
	OCR1BH = (U8)(ocr_temp >> 8);
	OCR1BL = (U8)(ocr_temp);
  }
 
	TIFR |= ( 1 << TOV1); 									//clear OVER ISR flag

	TCNT1H = 0x00;
	TCNT1L = 0x00;
	
	TIMSK |= temp;
	TCCR1B |= (TIMER1_PRC); 								// start timer 1
}

/****************************************************************
* Function name	: U16 TIMER1_get_time_symbols(void)
* Created by	: ezh
* Date created	: 20.02.2011
* Description	: return time in sybols( 1 symbol = TIMER1_TICK)
* Notes			: 
****************************************************************/
U16 TIMER1_get_time_symbols(void)
{
  	U16 TIMER1_cnt_temp = TCNT1;							// save LOW byte CNT
	TIMER1_time += (TIMER1_cnt_temp - TIMER1_back_up_cnt); 	//refresh time
	TIMER1_back_up_cnt = TIMER1_cnt_temp;					//refresh back-up time
	return((TIMER1_time * TIMER1_TICK_US));					//return time in symbols
}

/***************************************************************
* Function name	: void TIMER1_set_time_symbols(U16 time_symbols)
* U16 time_symbols : 
* Created by	: ezh
* Date created	: 20.02.2011
* Description	: set time in sybols( 1 symbol = TIMER1_TICK)
* Notes			: if not passed time_symbols, time is set 0
***************************************************************/
void TIMER1_set_time_symbols(U16 time_symbols)
{
	__disable_interrupt();
  	if(time_symbols)
		TIMER1_time = (time_symbols*(16/TIMER1_TICK_US));	//refresh time
	else
		TIMER1_time = 0;
	__enable_interrupt();
}
/***********************************************************************
* Function name	: U8 TIMER1_start_capt (void (*callback)(U16 timer_cnt))
* Created by	: ezh
* Date created	: 20.02.2011
* Description	: start capture interrupt handler 
* Notes			: use this function for start EXT interrupt 
* on pin CP1 and mesument time between events
***********************************************************************/
U8 TIMER1_start_capt (void (*callback)(U16 timer_cnt))
{
	if(callback)											//check callback pointer
		TIMER1_capt_cb = callback; 							//save pointer
	else
		return(ERROR);										//else - return error
		
	TCCR1B	|= ((TIMER1_NOISE_CANCELER << ICNC1)|(TIMER1_CAPTURE_EDGE << ICES1));
	TIFR = ( 1 << ICF1);
  	TIMSK |= (1 << TICIE1); 								// enable CAP interrupt
	
  	if(TCCR1B & 0x07)										//check timer 1 is run
	  TCCR1B |= (TIMER1_PRC); 								// if not - start timer 1
	  
	return(OK);												//return OK
}

/**********************************************************
* Function name	: void TIMER1_stop_capt (void)
* Created by	: ezh
* Date created	: 20.02.2011
* Description	: stop capture interrupt handler 
* Notes			: use this function for stop EXT interrupt
***********************************************************/
void TIMER1_stop_capt (void)
{
  	CRITICAL_SECTION(TIMSK &= ~(1 << TICIE1)); 				// disable CAP interrupt
}

/****************************************************************************
* Function name	: void TIMER1_COMPA_ISR(void)
* Created by	: ezh
* Date created	: 20.02.2011
* Description	: 8bit timer0 COMP_A IRS
* Notes			: disable interrupt and call callback by compare A interrupt
****************************************************************************/
#pragma vector = TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA_ISR(void)
{ 
	TIMSK &= ~(1 << OCIE1A); 								//disable COMP_A interrupt
	
	U16 TIMER1_cnt_temp = TCNT1;							// save HI byte CNT
	TIMER1_time += (TIMER1_cnt_temp - TIMER1_back_up_cnt);   
  	TIMER1_back_up_cnt = TIMER1_cnt_temp;					//erase cnt
	
    TIMER1_lock_A = 0;
	
	TIMER1_timeout_cb(); 									// call callback function
}

/****************************************************************************
* Function name	: void TIMER1_COMPA_ISR(void)
* Created by	: ezh
* Date created	: 20.02.2011
* Description	: 8bit timer0 COMP_A IRS
* Notes			: disable interrupt and call callback by compare B interrupt
***************************************************************************/
#pragma vector = TIMER1_COMPB_vect
__interrupt void TIMER1_COMPB_ISR(void)
{ 
	TIMSK &= ~(1 << OCIE1B); 								//disable COMP_B interrupt
	
	U16 TIMER1_cnt_temp = TCNT1;							// save HI byte CNT
	TIMER1_time += (TIMER1_cnt_temp - TIMER1_back_up_cnt);   
  	TIMER1_back_up_cnt = TIMER1_cnt_temp;					//erase cnt
	
    TIMER1_lock_B = 0;
	TIMER1_timeout_B_cb(); 									// call callback function
}

/********************************************
* Function name	: void TIMER1_CAPT_ISR(void)
* Created by	: ezh
* Date created	: 20.02.2011
* Description	: 8bit timer0 CAPT IRS
* Notes			: call callback by CAPT
********************************************/
#pragma vector = TIMER1_CAPT_vect
__interrupt void TIMER1_CAPT_ISR(void)
{ 
	U16 TIMER1_cnt_temp = ICR1;								// save HI byte CNT
	TIMER1_time += (TIMER1_cnt_temp - TIMER1_back_up_cnt);   
  	TIMER1_back_up_cnt = TIMER1_cnt_temp;					//erase cnt
	
	TIMER1_capt_cb(TIMER1_cnt_temp);						//call callback function
}

/*****************************************************
* Function name	: void TIMER1_OVF_ISR(void)
* Created by	: ezh
* Date created	: 20.02.2011
* Description	: 8bit timer0 OVF IRS
* Notes			: call callback by overflow interrupt
******************************************************/
#pragma vector = TIMER1_OVF_vect
__interrupt void TIMER1_OVF_ISR(void)
{ 
  	TIMER1_time += (0xFFFF - TIMER1_back_up_cnt);   
  	TIMER1_back_up_cnt = 0;									//erase cnt
	
	if(TIMER1_ovf_cb)
	  TIMER1_ovf_cb();										//call callback function
}