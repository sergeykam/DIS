/********************************************************
* Module name: TIMER_API.h
*
* Copyright 2005 TeaMiX as an unpublished work.
* All Rights Reserved.
*
* The information contained herein is confidential
* property of TeaMiX. The user, copying, transfer or
* disclosure of such information is prohibited except
* by express written agreement with TeaMiX.
*
* First written on ______ by ______
*
* Module Description: connected module TIMER_API for ATmega
*********************************************************/
/***************************************************
*	Include section
***************************************************/
#include "device.h"

/***************************************************
*	Defines section
***************************************************/
//enum commands {STOP, START, RESET, DIS_INT, EN_INT};
#ifndef	ENABLE_BIT_DEFINITIONS
#define	ENABLE_BIT_DEFINITIONS
#endif

#if		( CPU_CLK == 1000000 )
	#define TIMER1_PRC	((0 << CS12) | (1 << CS11) | (0 << CS10))
	#define TIMER1_TICK_US	1
#elif	( CPU_CLK == 2000000 )
	#define TIMER1_PRC	((0 << CS12) | (1 << CS11) | (0 << CS10))
	#define TIMER1_TICK_US	4
#elif	( CPU_CLK == 4000000 )
	#define TIMER1_PRC	((0 << CS12) | (1 << CS11) | (1 << CS10))
	#define TIMER1_TICK_US	16
#elif	( CPU_CLK == 8000000 )
	#define TIMER1_PRC	((0 << CS12) | (1 << CS11) | (1 << CS10))
	#define TIMER1_TICK_US	8
#else
   	#error "Clock speed not supported."
#endif


/***************************************************
*	Defines section - Time Formats
***************************************************/
#define TIMER1_CAPTURE_EDGE_FALL	0
#define TIMER1_CAPTURE_EDGE_RISE	1

#define TIMER1_CAPT_MODE
#define TIMER1_NOISE_CANCELER 1
#define TIMER1_CAPTURE_EDGE TIMER1_CAPTURE_EDGE_RISE

/***************************************************
*	Function Prototype Section
***************************************************/
U8 TIMER1_timeout_A(U16 time_cnt, void (*callback)(void));
void TIMER1_timeout_A_break(void);

U8 TIMER1_timeout_B(U16 time_cnt, void (*callback)(void));
void TIMER1_timeout_B_break(void);

U8 TIMER1_start_time_count (void (*timer_ovf_cb)(void));
void TIMER1_stop (void);
void TIMER1_reset (void);

U16 TIMER1_get_time_symbols(void);
void TIMER1_set_time_symbols(U16 time_symbols);

U8 TIMER1_start_capt (void (*callback)(U16 timer_cnt));
void TIMER1_stop_capt (void);