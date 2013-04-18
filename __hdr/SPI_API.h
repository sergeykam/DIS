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
*	Include section
***************************************************/
#include "device.h"

/***************************************************
*	Defines section
***************************************************/
#if	defined	(__ATmega32__) || defined (__ATmega16__)
    #define SPI_PORT	PORTB
	#define	SPI_DD_SS	PINB4		//Slave Select pin
#elif   defined (__ATmega1281__)
    #define SPI_PORT	PORTB
	#define	SPI_DD_SS	PINB0		//Slave Select pin
#elif   defined (__ATmega88PA__)||defined (__ATmega168PA__)
    #define SPI_PORT	PORTB
	#define	SPI_DD_SS	PINB2		//Slave Select pin
#else
	#error	"uC	type undescribed"
#endif

//clock phase
#define		SPI_SAMPLE_SETUP	0x00		//data is sampled on the leading edge
#define		SPI_SETUP_SAMPLE	(1 << CPHA)	//data is sampled on the trailing edge
	
//clock polarity
#define		SPI_IDLE_SCK_LOW	0x00		//SCK is low when idle
#define		SPI_IDLE_SCK_HIGH	(1 << CPOL)	//SCK is high when idle

//status
#define 	SPI_SLAVE 			0x00	
#define 	SPI_MASTER  		(1 << MSTR)

//first transmitted bit
#define		SPI_MSB				0x00		//the MSB of the data word is transmitted first
#define		SPI_LSB				(1 << DORD)	//the LSB of the data word is transmitted first

#define SPI_MODE_0	(SPI_IDLE_SCK_LOW | SPI_SAMPLE_SETUP )
#define SPI_MODE_1	(SPI_IDLE_SCK_LOW | SPI_SETUP_SAMPLE )
#define SPI_MODE_2	(SPI_IDLE_SCK_HIGH | SPI_SAMPLE_SETUP )
#define SPI_MODE_3	(SPI_IDLE_SCK_HIGH | SPI_SETUP_SAMPLE )

#define		SPI_AUTO_SLEEP		1
#define		SPI_USER_SLEEP		0
/***************************************************
*  User Settings section
***************************************************/
#define SPI_MODE (SPI_MODE_0 | SPI_MSB | SPI_MASTER)
#if defined (__ATmega88PA__)
#define	SPI_SLEEP_MODE		SPI_USER_SLEEP
#endif
/***************************************************
*	API Function Prototype Section
***************************************************/
void SPI_init (U8 mode, U8 clk_prescaler);
U8 SPI_transfer (U8 *Tx_buffer, U8 *Rx_buffer, U8 length, void (*callback)(U8 *Rx_buffer, U8 length));
void SPI_break(void);

#if defined (__ATmega88PA__) && (SPI_SLEEP_MODE == SPI_USER_SLEEP) 
void SPI_sleep(void);
void SPI_wakeup(void);
#endif