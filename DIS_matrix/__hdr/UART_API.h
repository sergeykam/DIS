/***************************************************
* Module name: UART_API.h
*
* Copyright 2004 Company as an  unpublished work.
* All Rights Reserved.
*
* The information contained herein is confidential
* property of Company. The user, copying, transfer or
* disclosure of such information is prohibited except
* by express written agreement with Company.
*
* First written on 02.03.04 by halfin
*
* Module Description: connected module UART API for AVR
***************************************************/
/***************************************************
*  Include section
***************************************************/

/***************************************************
*  User Settings section
***************************************************/
#define QUARTZ_FREQUENCY 8000000	/* set freq Hz */
//#define UART_INTERNAL_TIMER_ISR
#define UART_INTERNAL_TIMER_PERIOD	1
#if !defined (QUARTZ_FREQUENCY)
#error "UART API.h : user settings undefined"
#endif
/***************************************************
*   Defines section
***************************************************/
#define EVEN		0x10
#define ODD			0x30
#define NOPARITY	0x00

#define	STOPBIT1	0x00
#define STOPBIT2	0x08

#define DATABIT5	0x00
#define DATABIT6	0x02
#define DATABIT7	0x04
#define DATABIT8	0x06
/***************************************************
*   API Function Prototype Section
***************************************************/
extern void UART_init (U32, U8);
extern U8 UART_transmit(U8*, U8, void(*)(void));
extern U8 UART_Tx_query (void);
extern void UART_break_transmit (void);

extern U8 UART_receive (U8*, U8, U16, void (*)(U8*, U8));
extern void UART_set_Rx_tout_cb (void (*)(U8*, U8));
extern void UART_set_Rx_error_cb (void (*)(U8*, U8, U8));
extern U8 UART_Rx_query (void);
extern void UART_break_receive (void);
extern void UART_timer (void);

