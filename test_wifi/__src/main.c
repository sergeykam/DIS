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
*ш
* First	written	on ______ by ______
*
* Module Description: realization of SPI_API functions for ATmega
*****************************************************************/
/***************************************************
*	Include Section
***************************************************/
#include <device.h>
#include <UART_API.h>
#include "string.h"
/***************************************************
*   Function Prototype Section
***************************************************/
void Tx_cb (void);
void Rx_cb (U8*, U8);
//void getIP_cb (U8*, U8);
void error_cb (U8*, U8, U8);
void timeout_cb (U8*, U8);
void closeRoutine_cb (U8 *, U8);
/***************************************************
* CONSTS
***************************************************/
//const __flash U8 cmd = "$$$";
//const __flash U8 close = "close\r";
//const __flash U8 exit = "exit\r";

enum{
  CMD = 0,
  CLOSE = 1,
  EXIT = 2
};

/***************************************************
* Static Variables Section
***************************************************/

U8 cmd[] = "$$$";
U8 close[] = "close\n";
U8 ex[] = "exit\n";

U8 wifi_status = 0;

U8 callback_msg[] = "-under_ISR";
U8 waiting_msg[] = "-background_process";
U8 Rx_buffer[16];
U8 Rx_get[] = "GET / HTTP";
U8 Tx_buffer[] = "<html><head><title>DIS</title></head><body>Тест</body></html>";
U8 error_msg[] = "-error";
U8 timeout_msg[] = "-nothing received";

void main(void)
{
  	DDRD = 0xFF;
	PORTD = 0xFF;
    UART_init (9600, DATABIT8 + STOPBIT1 + NOPARITY);
	__enable_interrupt();
	UART_set_Rx_tout_cb (timeout_cb);
	UART_set_Rx_error_cb (error_cb);
	UART_receive (Rx_buffer, sizeof(Rx_buffer), 1000, Rx_cb);
    while(1){
		
    }
}

/**************************************************
* Function name	: void Tx_cb (void)
* Created by		: halfin
* Date created		: 03.03.04
* Description		: UART callback function
* Notes:	called under ISR
**************************************************/
void Tx_cb (void)
{
	UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
}

/**************************************************
* Function name	: void Rx_cb (U8 *buffer, U8 length)
* U8 *buffer - ptr to receive buffer
* U8 length - received buffer length
* Created by		: halfin
* Date created		: 03.03.04
* Description		: UART callback function
* Notes:	called under ISR
**************************************************/
void Rx_cb (U8 *buffer, U8 length)
{
    if(0 == memcmp(Rx_buffer,Rx_get,10)){
	  	__delay_cycles(100000);
	    Rx_buffer[0] = 0;
	  	UART_transmit (Tx_buffer, sizeof(Tx_buffer), (void(*)(void))closeRoutine_cb);
    } else {
	  	UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
	}
}
/**************************************************
* Function name	: void Tx_cb (void)
* Created by		: halfin
* Date created		: 03.03.04
* Description		: UART callback function
* Notes:	called under ISR
**************************************************/
void closeRoutine_cb (U8 *buffer, U8 length)
{
  switch (wifi_status)
  {
  case CMD:
	wifi_status = CLOSE;
	__delay_cycles(100000);
	UART_transmit (cmd, 3, (void(*)(void))closeRoutine_cb);
	break;
  case CLOSE:
	wifi_status = EXIT;
	__delay_cycles(100000);
	UART_transmit (close, 7, (void(*)(void))closeRoutine_cb);
	break;
  case EXIT:
    wifi_status = CMD;
	__delay_cycles(100000);
    UART_transmit (ex, 6, (void(*)(void))Rx_cb);
	break;
  }
}
/**************************************************
* Function name	: void Tx_cb (void)
* Created by		: halfin
* Date created		: 03.03.04
* Description		: UART callback function
* Notes:	called under ISR
**************************************************/
void exitRoutine_cb (void)
{
	UART_transmit (ex, sizeof(ex), Tx_cb);
	__delay_cycles(500);
}








/**************************************************
* Function name		: void error_cb	(I8 *buffer, U8 length, U8 error)
*		I8*	buffer	: pointer to receive buffer
*		U8 length	: #	of bytes received before error
*		U8 error	: error	code
* Created by		: halfin
* Date created		: 03.03.2004
* Description		: callback function	called during receive data error
* Notes				: callback function	called under UART receive ISR!
**************************************************/
void error_cb (U8 *buffer, U8 length, U8 error)
{
	UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
}

/**************************************************
* Function name		: void timeout_cb	(I8 *buffer, U8 length)
*		I8*	buffer	: pointer to receive buffer
*		U8 length	: #	of bytes received before error
* Created by		: halfin
* Date created		: 04.12.2004
* Description		: callback function	called if timeout occured
* Notes				: callback function	called under timer ISR!
**************************************************/
void timeout_cb (U8 *buffer, U8 length)
{
	UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
}