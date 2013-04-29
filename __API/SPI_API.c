/****************************************************************
* Module name: SPI_API.c
*
* Copyright	2005 TeaMiX as an unpublished work.
* All Rights Reserved.
*
* The information contained herein is confidential
* property of TeaMiX. The user, copying, transfer or
* disclosure of	such information is	prohibited except
* by express written agreement with	TeaMiX.
*
* First	written	on ______ by ______
*
* Module Description: realization of SPI_API functions for ATmega
*****************************************************************/
/***************************************************
*	Include Section
***************************************************/
#ifndef	ENABLE_BIT_DEFINITIONS
#define	ENABLE_BIT_DEFINITIONS
#endif

#include "SPI_API.h"


/***************************************************
*	Defines Section
***************************************************/
#if	defined	(__ATmega32__) || defined (__ATmega16__)
	#define		SPI_DDR			DDRB		//SPI Port DDR
	#define		SPI_DD_MOSI		PINB5		//Master Output Slave Input pin
	#define		SPI_DD_MISO		PINB6		//Master Input Slave Output pin
	#define		SPI_DD_SCK		PINB7		//Serial Clock pin

	#pragma vector = SPI_STC_vect
			__interrupt void SPI_STC_ISR (void);	//SPI Serial Transfer Complete interrupt
#elif   defined (__ATmega1281__)
    #define		SPI_DDR			DDRB		//SPI Port DDR
	#define		SPI_DD_MOSI		PINB2		//Master Output Slave Input pin
	#define		SPI_DD_MISO		PINB3		//Master Input Slave Output pin
	#define		SPI_DD_SCK		PINB1		//Serial Clock pin

    #pragma vector = SPI_STC_vect
			__interrupt void SPI_STC_ISR (void);	//SPI Serial Transfer Complete interrupt
#elif   defined(__ATmega88PA__ ) || defined(__ATmega168PA__) || defined(__IOM328P_H)
    #define		SPI_DDR			DDRB		//SPI Port DDR
	#define		SPI_DD_MOSI		PINB3		//Master Output Slave Input pin
	#define		SPI_DD_MISO		PINB4		//Master Input Slave Output pin
	#define		SPI_DD_SCK		PINB5		//Serial Clock pin

#if (SPI_SLEEP_MODE != SPI_USER_SLEEP)
	inline void SPI_wakeup(void);
	inline void SPI_sleep(void);
#endif
    #pragma vector = SPI_STC_vect
			__interrupt void SPI_STC_ISR (void);	//SPI Serial Transfer Complete interrupt			__interrupt void SPI_STC_ISR (void);	//SPI Serial Transfer Complete interrupt
#else
	#error	"uC	type undescribed"
#endif


/***************************************************
*	Function Prototype Section
***************************************************/


/***************************************************
*	Static Variables Section
***************************************************/
static U8 	SPI_transfer_cnt = 0;			//byte counter
static U8	SPI_transfer_cb_length;			//value of length-parameter for SPI_transfer_cb function

static U8	*SPI_Tx_buffer,					//pointer to transmit-data buffer
			*SPI_Rx_buffer,					//pointer to receive-data buffer
			*SPI_transfer_cb_Rx_buffer;		//pointer to receive-data buffer for SPI_transfer_cb function
			
static void (*SPI_transfer_cb)(U8*, U8);	//SPI transfer complete callback function pointer



/*******************************************************************************************************
* Function name		: void SPI_init (U8 mode, U8 clk_prescaler)
*		U8 mode				: SPI mode:
*									status (slave or master)	(SPI_SLAVE or SPI_MASTER)
*																(+)
*									clock polarity				(SPI_IDLE_SCK_LOW or SPI_IDLE_SCK_HIGH)
*																(+)
*									clock phase					(SPI_SAMPLE_SETUP or SPI_SETUP_SAMPLE)
*																(+)
*									first transmitted bit		(SPI_MSB or SPI_LSB)
*
*		U8 clk_prescaler 	: prescaler of uC clock frequency (2, 4, 8, 16, 32, 64, 128)
*
* Created by		:
* Data created		:
* Description		: function initialises SPI
* Notes				:
********************************************************************************************************/
void SPI_init (U8 mode, U8 clk_prescaler)
{
	SPCR = 0x00;					//clean previous settings

									//set status (master or slave)
	if (mode & SPI_MASTER)			//if master
	{
		SPI_DDR |= (1 << SPI_DD_SS) | (1 << SPI_DD_MOSI) | (1 << SPI_DD_SCK);	//set SPI Port configuration

		switch (clk_prescaler)		//set clock prescaler
		{
			case 2:					//prescaler division ratio 2
				SPSR |= (1 << SPI2X);
				break;
			case 4:					//prescaler division ratio 4
				SPSR &= ~(1 << SPI2X);
      			break;
			case 8:					//prescaler division ratio 8
				SPSR |= (1 << SPI2X);
				SPCR |= (1 << SPR0);
      			break;
			case 16:				//prescaler division ratio 16
				SPSR &= ~(1 << SPI2X);
				SPCR |= (1 << SPR0);
				break;
			case 32:				//prescaler division ratio 32
				SPSR |= (1 << SPI2X);
				SPCR |= (1 << SPR1);
				break;
			case 64:				//prescaler division ratio 64
				SPSR &= ~(1 << SPI2X);
				SPCR |= (1 << SPR1);
				break;
			case 128:				//prescaler division ratio 128
				SPSR &= ~(1 << SPI2X);
				SPCR |= (1 << SPR0) | (1 << SPR1);
				break;
			default:				//default prescaler division ratio 64
				SPSR |= (1 << SPI2X);
				SPCR |= (1 << SPR0) | (1 << SPR1);
		}//end of switch (clk_prescaler)
	}
	else	//if slave
		SPI_DDR |= (1 << SPI_DD_MISO);	//set SPI port configuration

	SPCR |= mode;	//set clock polarity
	
#if (SPI_SLEEP_MODE == SPI_AUTO_SLEEP)
	SPI_sleep();
#endif
}//end of SPI_init


/******************************************************************************************************************************
* Function name		: U8 SPI_transfer (U8 *Tx_buffer, U8 *Rx_buffer, U8 length, void (*callback)(U8 *Rx_buffer, U8 length))
*		returns			: 0		if transfer begins successfully
*						  >0	number of bytes unreceived/untransmitted in previous transfer
*		U8 *Tx_buffer	: pointer to transmit-data buffer	(0 if transmission is disabled)
*		U8 *Rx_buffer	: pointer to receive-data buffer	(0 if receive is disabled)
*		U8 length		: length of packet
*		*callback      	: pointer to callback function called after completing transfer
*				U8 *Rx_buffer	: pointer to receive-data buffer
*				U8 length		: length of packet
*
* Created by	:
* Data created	:
* Description	: function tries to begin transfer of data packets between master and slave devices
* Notes			:
*******************************************************************************************************************************/
U8 SPI_transfer (U8 *Tx_buffer, U8 *Rx_buffer, U8 length, void (*callback)(U8 *Rx_buffer, U8 length))
{
	LOCK_RESOURCE_OR_RETURN(SPI_transfer_cnt, length);

#if (SPI_SLEEP_MODE == SPI_AUTO_SLEEP)
	SPI_wakeup();
#endif
	
	SPI_Tx_buffer = Tx_buffer;
	SPI_Rx_buffer = Rx_buffer;

	SPI_transfer_cb = callback;
	SPI_transfer_cb_Rx_buffer = Rx_buffer;		
	SPI_transfer_cb_length = length;
	
	if (!length)
	{
		SPI_transfer_cb(SPI_transfer_cb_Rx_buffer, SPI_transfer_cb_length);	//call callback function
		return 0;
	}
		
	
	SPCR |= (1 << SPE) | (1 << SPIE);	//enable SPI & SPI_STC interrupt
	
	if (SPI_Tx_buffer)
        {//if transmission is enabled
		SPDR = *SPI_Tx_buffer;			//transfer bytes (data transmission)
        
        }
	else								//if transmission is disabled
		SPDR = 0;						//receive byte (null transmission)
	
	return 0;
}//end of SPI_transfer

/*******************************************************************************************************
* Function name		: __interrupt void SPI_STC_ISR (void)
* Created by		:
* Date created		:
* Description		:	SPI Serial Transfer Complete ISR,
*						function	saves received byte					if receive is enabled
*									starts 	transfer of next bytes		if data transfer is not complete
*											receive of next byte		if transmission is disabled
*											transmission of next byte	if receive is disabled
*									calls callback function				if data transfer is complete
* Notes				:
********************************************************************************************************/
#pragma vector = SPI_STC_vect
  __interrupt void SPI_STC_ISR (void)
{
  __delay_cycles(100);
	if (SPI_Rx_buffer)					//if receive is enabled
	{
		*SPI_Rx_buffer = SPDR;			//write received byte in receive-data buffer
		SPI_Rx_buffer++;
	}

	if (--SPI_transfer_cnt)				//if received/transmitted byte is not last
	{	
		if (SPI_Tx_buffer)				//if transmission is enabled
			SPDR = *++SPI_Tx_buffer;	//transfer next bytes (data transmission)
		else							//if transmission is disabled
			SPDR = 0;					//receive next byte (null transmission)
	}
	else								//if received/transmitted byte is last
	{
		SPCR &= ~((1 << SPE) | (1 << SPIE));								//disable SPI & SPI_STC interrupt
		SPI_transfer_cb(SPI_transfer_cb_Rx_buffer, SPI_transfer_cb_length);	//call callback function
#if (SPI_SLEEP_MODE == SPI_AUTO_SLEEP)
//		SPI_sleep();
#endif
	}
	return;
}//end of SPI interrupt processor

/*******************************************************************************************************
* Function name		: void SPI_break(void)
* Created by		:
* Date created		:
* Description		:	SPI break Transfer 
* Notes				:
********************************************************************************************************/
void SPI_break(void)
{
  	SPCR &= ~((1 << SPE) | (1 << SPIE));								//disable SPI & SPI_STC interrupt
	SPI_transfer_cnt = 0;
	U8 temp = SPDR;
}

#if   defined (__ATmega88PA__)
/********************************************************************
* Function name	: void I2C_sleep(void)
*
* Created by	:
* Data created	:
* Description	: function for save registers of I2C and turn off module
* Notes			:
********************************************************************/
void SPI_sleep(void)
{
	if(SPI_transfer_cnt)
	  return;
	
  	U16 save_reg = SPCR;
	save_reg <<=8;
	save_reg |= SPSR;
	SPI_Tx_buffer = (U8*)(save_reg);
	SPI_transfer_cb_length = SPDR;
	PRR |= (1 << PRSPI); //turn off SPI
}

/********************************************************************
* Function name	: void I2C_wakeup(void)
*
* Created by	:
* Data created	:
* Description	: function for restore registers of I2C and turn on module
* Notes			:
********************************************************************/
void SPI_wakeup(void)
{
	if((PRR & (1 << PRSPI))== 0)
	  return;
	
	PRR &= ~(1 << PRSPI); //turn on SPI
	U16 save_reg = (U16)(SPI_Tx_buffer);
	SPCR = (save_reg >> 8);
	SPSR = save_reg;
	SPDR = SPI_transfer_cb_length;	
}
#endif