/*********************************************************************
* Module name: UART_API.c
*
* Copyright	2004 TeaMiX as an unpublished work.
* All Rights Reserved.
*
* The information contained herein is confidential
* property of TeaMiX. The user, copying, transfer or
* disclosure of such information is prohibited except
* by express written agreement with TeaMiX.
*
* First	written		on 02.03.04	by halfin
* Redesigned 		on 23.11.04	by halfin
*
* Module Description: realization of UART_API functions for ATmega uC
*********************************************************************/
/***************************************************
*	Include Section
***************************************************/
#ifndef	ENABLE_BIT_DEFINITIONS
#define	ENABLE_BIT_DEFINITIONS
#endif

#include <intrinsics.h>
#include <standard.h>
#include <UART_API.h>
#include <device.h>

/***************************************************
*	Defines	Section
***************************************************/
#if	defined (__IOM16_H) || defined (__IOM8515_H) || defined (__IOM8535_H) || defined (__IOM323_H) || defined (__IOM32_H)
	#pragma	vector = USART_TXC_vect
			__interrupt	void UART_Tx_TXC_ISR (void);	//USART Tx Complete interrupt
	#pragma	vector = USART_UDRE_vect
			__interrupt	void UART_Tx_UDRE_ISR (void);	//USART Data Register Empty interrupt
	#pragma	vector = USART_RXC_vect
			__interrupt	void UART_Rx_ISR (void);		//USART Rx Complete interrupt
			
#elif   defined (__ATmega16__)
	#pragma	vector = USART_TXC_vect
			__interrupt	void UART_Tx_TXC_ISR (void);	//USART Tx Complete interrupt
	#pragma	vector = USART_UDRE_vect
			__interrupt	void UART_Tx_UDRE_ISR (void);	//USART Data Register Empty interrupt
	#pragma	vector = USART_RXC_vect
			__interrupt	void UART_Rx_ISR (void);		//USART Rx Complete interrupt

#elif   defined (__ATmega128__)
	#pragma	vector = USART0_TXC_vect
			__interrupt	void UART_Tx_TXC_ISR (void);	//USART Tx Complete interrupt
	#pragma	vector = USART0_UDRE_vect
			__interrupt	void UART_Tx_UDRE_ISR (void);	//USART Data Register Empty interrupt
	#pragma	vector = USART0_RXC_vect
			__interrupt	void UART_Rx_ISR (void);		//USART Rx Complete interrupt
			
#elif   defined (__ATmega48__)||defined(__ATmega168PA__)||defined(__ATmega328P__)
	#pragma	vector = USART_TX_vect
			__interrupt	void UART_Tx_TXC_ISR (void);	//USART Tx Complete interrupt
	#pragma	vector = USART_UDRE_vect
			__interrupt	void UART_Tx_UDRE_ISR (void);	//USART Data Register Empty interrupt
	#pragma	vector = USART_RX_vect
			__interrupt	void UART_Rx_ISR (void);		//USART Rx Complete interrupt

#define UDR		UDR0
#define UCSRA	UCSR0A
#define UCSRB	UCSR0B
#define UCSRC	UCSR0C
#define UBRRL	UBRR0L
#define UBRRH	UBRR0H

/* USART0 Control and Status Register A */
#define    RXC		RXC0
#define    TXC		TXC0
#define    UDRE		UDRE0
#define    FE		FE0
#define    DOR		DOR0
#define    PE		UPE0
#define    U2X		U2X0
#define    MPCM		MPCM0

/* USART0 Control and Status Register B */
#define    RXCIE   RXCIE0
#define    TXCIE   TXCIE0
#define    UDRIE   UDRIE0
#define    RXEN    RXEN0
#define    TXEN    TXEN0
#define    UCSZ2   YCSZ0
#define    RXB8    RXB80
#define    TXB8    TXB80

/* USART0 Control and Status Register C */
#define    UMSEL   UMSEL0
#define    UPM1    UPM10
#define    UPM0    UPM00
#define    USBS    USBS0
#define    UCSZ1   UCSZ00
#define    UCSZ0   UCSZ00
#define    UCPOL   UCPOL0

			
			

#else
	#error "uC type undescribed"
#endif

#define	UART_BPS2400	(QUARTZ_FREQUENCY/19200)-1
#define	UART_BPS4800	(QUARTZ_FREQUENCY/38400)-1
#define	UART_BPS9600	(QUARTZ_FREQUENCY/76800)-1
#define	UART_BPS14400	(QUARTZ_FREQUENCY/115200)-1
#define	UART_BPS19200	(QUARTZ_FREQUENCY/153600)-1
#define	UART_BPS28800	(QUARTZ_FREQUENCY/230400)-1
#define	UART_BPS38400	(QUARTZ_FREQUENCY/307200)-1
#define	UART_BPS57600	(QUARTZ_FREQUENCY/460800)-1
#define	UART_BPS76800	(QUARTZ_FREQUENCY/614400)-1
#define	UART_BPS115200	(QUARTZ_FREQUENCY/921600)-1
#define	UART_BPS230400	(QUARTZ_FREQUENCY/1843200)-1
#define	UART_BPS250000	(QUARTZ_FREQUENCY/2000000)-1	
#define	UART_BPS500000	(QUARTZ_FREQUENCY/4000000)-1
#define	UART_BPS1000000	(QUARTZ_FREQUENCY/8000000)-1


/***************************************************
*	Function Prototype Section
***************************************************/
//static void UART_init_Timer0 (void);


/***************************************************
*	Static Variables Section
***************************************************/
static void	(*UART_Tx_cb)(void);				//Tx complete callback pointer
static void	(*UART_Rx_done_cb)(U8*, U8);		//Rx complete callback pointer
static void	(*UART_Rx_error_cb)(U8*, U8, U8);	//Rx error callback pointer
static void	(*UART_Rx_tout_cb)(U8*, U8);		//Rx timeout callback pointer

static volatile	U8 	*UART_Rx_buffer,			//pointer to receive-data buffer
					*UART_Tx_buffer,			//pointer to transmit-data buffer
					*UART_Rx_cb_ptr;			//pointer to receive-data buffer for receive callback functions

static volatile	U8 	UART_Tx_cnt = 0,			//Tx byte counter
					UART_Rx_cnt	= 0,			//Rx byte counter
					UART_cb_cnt = 0;			//byte counter parameter for callback functions

static volatile	U16	UART_timeout_cnt = 0;



/******************************************************
* Function name		: UART_init (U32 baudrate, U8 mode)
*		U32	baudrate	: required baudrate	(bps)
*		U8	mode		: mode of UART
*
* Created by	: halfin
* Date created	: 03.03.04
* Description	: initialization of UART
* Notes			: restrictions,	odd	modes
*******************************************************/
void UART_init (U32	baudrate, U8 mode)
{
	UCSRC =	0;			//clean previous settings (URSEL is 0, enter in UBRRH)

	//set baudrate
//	switch (baudrate)	//set baudrate
//	{
//		case 2400:
//			UBRRH =	(UART_BPS2400 >> 8) & 0x0F;
//			UBRRL =	UART_BPS2400 & 0xFF;
//			break;
//		case 4800:
//			UBRRH =	(UART_BPS4800 >> 8) & 0x0F;
//			UBRRL =	UART_BPS4800 & 0xFF;
//			break;
//		case 9600:
			UBRRH =	(UART_BPS9600 >> 8) & 0x0F;
			UBRRL =	UART_BPS9600 & 0xFF;
//			break;
//		case 14400:
//			UBRRH =	(UART_BPS14400 >> 8) & 0x0F;
//			UBRRL =	UART_BPS14400 & 0xFF;
//			break;
//		case 19200:
//			UBRRH =	(UART_BPS19200 >> 8) & 0x0F;
//			UBRRL =	UART_BPS19200 & 0xFF;
//			break;
//		case 28800:
//			UBRRH =	(UART_BPS28800 >> 8) & 0x0F;
//			UBRRL =	UART_BPS28800 & 0xFF;
//			break;
//		case 38400:
//			UBRRH =	(UART_BPS38400 >> 8) & 0x0F;
//			UBRRL =	UART_BPS38400 & 0xFF;
//			break;
//		case 57600:
//			UBRRH =	(UART_BPS57600 >> 8) & 0x0F;
//			UBRRL =	UART_BPS57600 & 0xFF;
//			break;
//		case 76800:
//			UBRRH =	(UART_BPS76800 >> 8) & 0x0F;
//			UBRRL =	UART_BPS76800 & 0xFF;
//			break;
//		case 115200:
//			UBRRH =	(UART_BPS115200 >> 8) & 0x0F;
//			UBRRL =	UART_BPS115200 & 0xFF;
//			break;
//		case 230400:
//			UBRRH =	(UART_BPS230400 >> 8) & 0x0F;
//			UBRRL =	UART_BPS230400 & 0xFF;
//			break;
//		case 250000:
//			UBRRH =	(UART_BPS250000 >> 8) & 0x0F;
//			UBRRL =	UART_BPS250000 & 0xFF;
//			break;
//		case 500000:
//			UBRRH =	(UART_BPS500000 >> 8) & 0x0F;
//			UBRRL =	UART_BPS500000 & 0xFF;
//			break;
//		case 1000000:
//			UBRRH =	(UART_BPS1000000 >>	8) & 0x0F;
//			UBRRL =	UART_BPS1000000 & 0xFF;
//			break;
//		default:
//			UBRRH =	((QUARTZ_FREQUENCY/(8 *	baudrate))	- 1)>>8;
//			UBRRL =	(QUARTZ_FREQUENCY/(8 *	baudrate)) - 1;
//			break;		
//	}//end of switch
	UCSRA =	(1 << U2X);				//double speed

#if defined (__IOM128_H)||defined (__ATmega48__)||defined(__ATmega168PA__)||defined(__ATmega328P__)
	UCSRC =	mode;	//set UART mode (URSEL is 1, enter in UCSRC)
#else
	UCSRC =	(1 << URSEL) | mode;	//set UART mode (URSEL is 1, enter in UCSRC)
#endif
	
#ifdef UART_INTERNAL_TIMER_ISR
	UART_init_Timer0();
#endif

}//end of UART_init


/****************************** TRANSMIT functions *****************************************
* Function name		: U8 UART_transmit (U8 *buffer,	U8 length, void	(*callback)(void))
*		returns			: 0	if transmission begins sucsessfully
*						  number of bytes unsended in previous transmission otherwise
*		U8 *buffer		: pointer to transmit-data buffer
*		U8 length		: length of transmitted packet
*		*callback		: pointer to callback function called after completing transmission
*
* Created by	: halfin
* Date created	: 03.03.04
* Description	: function tries to begin transmitting required # of bytes
* Notes			: callback function is called under USART Tx Complete ISR (if length != 0 ?)
*******************************************************************************************/
U8 UART_transmit (U8 *buffer, U8 length, void (*callback)(void))
{
	LOCK_RESOURCE_OR_RETURN (UART_Tx_cnt, length);

	UART_Tx_buffer = buffer;
	UART_Tx_cb = callback;

	switch (length)
	{
		case 0:
			UART_Tx_cb();
			return 0;
		case 1:
			UCSRB |= (1 << TXEN)|(1 << TXCIE);		//enable Tx and Tx Complete IRq
			UDR	= *UART_Tx_buffer;					//transmit byte
			return 0;
		case 2:
			UCSRB |= (1 << TXEN)|(1 << TXCIE);		//enable Tx and Tx Complete IRq
			UDR	= *UART_Tx_buffer++;				//transmit bytes
			UDR	= *UART_Tx_buffer;
			return 0;
		default:
			UCSRB |= (1 << TXEN);					//enable Tx
			UDR	= *UART_Tx_buffer++;				//transmit bytes
			UDR	= *UART_Tx_buffer++;
			UCSRB |= (1	<< UDRIE);					//enable UDR Empty IRq
			UART_Tx_cnt	-= 2;
			return 0;
	}//end of switch (length)
}

/********************************************************
* Function name		: U8 query_Tx_UART (void)
*	 	returns			: # of bytes remained to transmit
*
* Created by	: halfin
* Date created	: 03.03.04
* Description	: nonblocked query
********************************************************/
U8 UART_Tx_query (void)
{
	return UART_Tx_cnt;
}	

/*******************************************************************************************
* Function name		: __interrupt void UART_Tx_UDRE_ISR	(void)
*
* Created by	: halfin
* Date created	: 23.11.04
* Description	: UART transmit	buffer UDR Empty ISR
* Notes			: after placing last byte to transmit buffer, ISR is switched to Tx Complete
********************************************************************************************/
#pragma	vector = USART_UDRE_vect
			__interrupt	void UART_Tx_UDRE_ISR (void)
{
	UCSRA |= (1	<< TXC);				//clear Tx complete interrupt flag

	UDR	= *UART_Tx_buffer++;			//transmit next byte

	if (--UART_Tx_cnt)					//if byte is not last
	   return;

	//switch to another ISR
	UART_Tx_cnt	= 1;					//disable other transmission processes
	UCSRB &= ~(1 <<	UDRIE);				//disable UDR Empty IRq
	UCSRB |= (1	<< TXCIE);				//enable Tx Complete IRq
}

/************************************************************************************
* Function name		: __interrupt void UART_Tx_TXC_ISR (void)
*
* Created by	: halfin
* Date created	: 23.11.04
* Description	: UART transmit	complete ISR
* Notes			: after transmitting last byte, callback function is called under ISR
*************************************************************************************/
#pragma	vector = USART_TX_vect
			__interrupt	void UART_Tx_TXC_ISR (void)
{
	UART_Tx_cnt	= 0;
	UCSRB &= ~((1 << TXEN)|(1 << TXCIE));	//disable Tx and Tx Complete IRq
	UART_Tx_cb();
}

/****************************** RECEIVE functions *****************************************************************
* Function name		: U8 UART_receive (U8 *buffer, U8 length, U16 timeout, void	(*callback)(U8 *buffer, U8 length))
*		returns			: 0	if receive begins sucsessfully
*						  # of waited bytes in previous receiving otherwise
*		U8 * buffer		: pointer to receive buffer
*		U8 length		: length of waited packet
*		U16	timeout		: maximum time between received bytes in timer ticks (1ms if INTERNAL_TIMER_ISR)
*						0 -	w/o	timeout
*		*callback		:	pointer	to callback	function called	after receiving	completed
*				I8 * buffer	:	pointer	to receive buffer
*				U8 length	:	length of packet waited
*
* Created by	: halfin
* Date created	: 03.03.04
* Description	: trying to	begin receiving	required # of bytes
* Notes			:	1. callback	function receiving under UART receive complete ISR
*					2. function	doesn't	restart	external timeout timer,	this may be	problem	before 1st byte
*************************************************************************************************************/
U8 UART_receive	(U8	*buffer, U8	length,	U16	timeout, void (*callback)(U8*, U8))
{
	LOCK_RESOURCE_OR_RETURN (UART_Rx_cnt, length);

	UART_Rx_buffer = buffer;
	UART_cb_cnt	= length;
	UART_Rx_cb_ptr = buffer;
	UART_Rx_done_cb = callback;
	UCSRB |= (1	<< RXEN)|(1	<< RXCIE);

	UART_timeout_cnt = timeout;
	return 0;
}

/********************************************************************************
* Function name	: __interrupt void UART_Rx_ISR (void)
* Created by		: halfin
* Date created		: 03.03.04
* Description		: UART receive complete	interrupt ISR
* Notes				: if receive complete callback function	called under this ISR
*	if error occured corresponding callback	function called	under this ISR
********************************************************************************/
#pragma	vector = USART_RX_vect
			__interrupt	void UART_Rx_ISR (void)
{
	if (UCSRA &	((1<<FE)|(1<<DOR)|(1<<PE)))
	{
		UCSRB &= ~((1<<RXEN)|(1<<RXCIE));
		UART_Rx_cnt	= 0;
		UART_timeout_cnt = 0;
		UART_Rx_error_cb ((U8*)UART_Rx_cb_ptr, UART_Rx_cnt, UCSRA);
		return;
	}

	*UART_Rx_buffer++ = UDR;
	if (-- UART_Rx_cnt)	return;
	UART_timeout_cnt = 0;
	UCSRB &= ~((1 << RXEN)|(1 << RXCIE));
	UART_Rx_done_cb ((U8*)UART_Rx_cb_ptr, UART_cb_cnt);
}

/**************************************************************************************************
* Function name		: void set_UART_Rx_error_cb	(void (*callback)(U8* buffer, U8 length, U8	error))
*	 *callback		: pointer to function called back if receive error occured
*		U8*	buffer	: pointer to receive buffer
*		U8 length	: #	of bytes received before error
*		U8 error	: error	code
* Created by		: halfin
* Date created		: 03.03.04
* Description		: set callback function	called during receive data error
* Notes				: callback function	called under UART receive ISR!
**************************************************************************************************/
void UART_set_Rx_error_cb (void	(*callback)(U8*, U8, U8))
{
	UART_Rx_error_cb = callback;
}

/***********************************************************************************************
* Function name		: void set_UART_Rx_tout_cb	(void (*callback)(U8* buffer, U8 length))
*	 *callback		: pointer to function called back if receive timeout occured
*		U8*	buffer	: pointer to receive buffer
*		U8 length	: #	of bytes received before timeout
* Created by		: halfin
* Date created		: 03.03.04
* Description		: set callback function called if time between received bytes exceed timeout
* Notes				: callback function	called under UART receive ISR!
***********************************************************************************************/
void UART_set_Rx_tout_cb (void (*callback)(U8*, U8))
{
	UART_Rx_tout_cb = callback;
}

/***************************************************
* Function name		: void break_receive_UART (void)
* Created by		: halfin
* Date created		: 03.03.04
* Description		: terminate	receive	immediatly
***************************************************/
#pragma inline
void UART_break_receive	(void)
{
	UCSRB &= ~(1<<RXEN)|((1<<RXCIE));	//disable receive
	UART_cb_cnt	= UART_Rx_cnt;
	UART_Rx_cnt	= UDR;					//clear interrupt flag
	UART_Rx_cnt	= 0;
	UART_timeout_cnt = 0;
}//end of function break_receive_UART


/*****************************************************************
* Function name		: void UART_Timer (void)
* Created by		: halfin
* Date created		: 03.03.04
* Description		: check	UART Rx	timeout	counter
* Notes			: if external timer	ISR	used, function must	called
*	from any periodical	timer ISR
*****************************************************************/
void UART_timer_cout	(void)
{
	if (UART_timeout_cnt)
	{
		if (--UART_timeout_cnt) return ;
		UART_break_receive ();
		UART_Rx_tout_cb ((U8*)UART_Rx_cb_ptr, UART_Rx_cnt);
	}//end if
 }

//#ifdef UART_INTERNAL_TIMER_ISR
//#pragma	vector = TIMER0_OVF_vect
//		__interrupt	void timer0_ISR	(void);
//
///*****************************************************************************
//* Function name	: Timer0_ISR (void)
//* Created by		: halfin
//* Date created		: 03.03.04
//* Description		: 8bit timer0 ISR every	1ms
//* Notes				: function mast	be used	only if	INTERNAL_TIMER_ISR choosen
//*****************************************************************************/
//__interrupt	void timer0_ISR	(void)
//{
//	TCNT0 =	256 - (UART_INTERNAL_TIMER_PERIOD * 63);
//	UART_timer();
//}//end of function timer0_ISR
//
///***************************************************************************
//* Function name	: void UART_init_Timer0	(void)
//* Created by		: halfin
//* Date created		: 03.03.04
//* Description		: 8bit timer initialization; enable	interrupt every	1 ms
//* Notes			: function must	be used	only if	INTERNAL_TIMER_ISR choosen
//***************************************************************************/
//void UART_init_Timer0 (void)
//{
//	TCCR0 =	3;											//prescaler = 64
//	TCNT0 =	256 - (UART_INTERNAL_TIMER_PERIOD * 63);
//	TIMSK |= (1<<TOIE0);								//overflow IRQ enable
//}
//#endif	//UART_INTERNAL_TIMER_ISR
