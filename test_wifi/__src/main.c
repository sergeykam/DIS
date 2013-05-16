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
#include <TIMER0_HW_API.h>
#include "string.h"
#include "pgmspace.h"
/***************************************************
*   Function Prototype Section
***************************************************/
void Tx_cb (void);
void Rx_cb (U8*, U8);
void error_cb (U8*, U8, U8);
void timeout_cb (U8*, U8);
void timer_cb (U8 *, U8);
void exit_cb(void);
void site_tx_cb(void);
/***************************************************
* DEFINES
***************************************************/
#define WAITING_TIME  32
#define TIME_BTW_CMDS 20


#define SITE_PARTICLES 42
#define SITE_PARTICLE_SIZE 128

/***************************************************
* CONSTS
***************************************************/

const __flash I8 site[SITE_PARTICLES][SITE_PARTICLE_SIZE] = {
<!DOCTYPE html><html><head><title>������� ���</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><style>*

const __flash I8 site_end[] = ";case 3:return c4;}};};</script></body></html>\n";

enum{
  IDLE = 0,
  WAIT,
  SEND_SITE,
  SEND_DATA,
  CMD,
  CLOSE,
  EXIT,
  BUSY
};

/***************************************************
* Static Variables Section
***************************************************/
// CMDs
U8 cmd[] = "$$$";
U8 close[] = "close\n";
U8 ex[] = "exit\n";
// statuses
U8 wifi_status = IDLE;  // текущая работа с вай фай
U8 wifi_timer_cnt = 1; 
U8 wifi_task = WAIT; // задание для вай вай
U8 wifi_task_after_tx = IDLE; // задание для вай вай
U8 site_part_to_tx = 0;
// task pointer
void (*task_ptr)(void) = 0;
// HTML
U8 Tx_buffer[SITE_PARTICLE_SIZE];
U8 Rx_buffer[16];
U8 Rx_get_site[] = "GET / HTTP";
U8 Rx_get_data[] = "GET /?a=1 HTTP";
U8 http_headers[] = "HTTP/1.1$200$OK\n\n";
//U8 site[] = "HTTP/1.1$200$OK\n\n\r<html><head><title>DIS</title></head><body>Тест</body></html>\n\r";
//U8 site[] = "<html><head><title>DIS</title></head><body>Тест</body></html>\n\r";

void main(void)
{
  	DDRD = 0xFF;
	PORTD = 0xFF;
    UART_init (9600, DATABIT8 + STOPBIT1 + NOPARITY);
	TIMER0_HW_API_init ((void(*)(void))timer_cb);
	__enable_interrupt();
	UART_set_Rx_tout_cb (timeout_cb);
	UART_set_Rx_error_cb (error_cb);
	UART_receive (Rx_buffer, sizeof(Rx_buffer), 1000, Rx_cb);
    while(1){
		
    }
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
	if(IDLE == wifi_status){	// если не в режиме ожидания то забиваем на все пакеты
		if((0 == memcmp(Rx_buffer,Rx_get_site,10)) || (0 == memcmp(&Rx_buffer[1],Rx_get_site,10))){ // запрос на сайт
			Rx_buffer[0] = 0;	// портим буффер Rx
			// ставим на ожидание, а потом передачу
			wifi_status = WAIT; 
			wifi_task = SEND_SITE;
		} else {
			if(0 == memcmp(Rx_buffer,Rx_get_data,10)){
				Rx_buffer[0] = 0;
				// тут передаем данные
			} else {
				UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
			}
		}
	}
}
/**************************************************
* Function name	: void Tx_cb (void)
* Created by		: halfin
* Date created		: 03.03.04
* Description		: UART callback function
* Notes:	called under ISR
**************************************************/
void timer_cb (U8 *buffer, U8 length)
{
	switch (wifi_status)
	{
  		case IDLE:
			break;
  		case WAIT:
			if(wifi_timer_cnt){
				wifi_timer_cnt--;
			} else {
				wifi_timer_cnt = WAITING_TIME; 
				wifi_status = wifi_task;
			}
			break;
		case SEND_SITE:
			wifi_status = BUSY;
			wifi_task = CMD;
			wifi_task_after_tx = WAIT;
			UART_transmit (http_headers, sizeof(http_headers)-1, site_tx_cb);
			break;
  		case CMD:
			wifi_status = BUSY;
			wifi_task = CLOSE;
			wifi_task_after_tx = WAIT;
			wifi_timer_cnt = TIME_BTW_CMDS;
			UART_transmit (cmd, 3, Tx_cb);
			break;
  		case CLOSE:
			wifi_status = BUSY;
			wifi_task = EXIT;
			wifi_task_after_tx = WAIT;
			wifi_timer_cnt = TIME_BTW_CMDS;
			UART_transmit (close, 7, Tx_cb);
			break;
  		case EXIT:
			wifi_status = IDLE;
    		UART_transmit (ex, 6, exit_cb);
			break;
		case BUSY:
			break;
		default:
			wifi_status = IDLE;
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
void Tx_cb(void){
	wifi_status = wifi_task_after_tx;
}
/**************************************************
* Function name	: void Tx_cb (void)
* Created by		: halfin
* Date created		: 03.03.04
* Description		: UART callback function
* Notes:	called under ISR
**************************************************/
void exit_cb(void){
	UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
}
/**************************************************
* Function name	: void Tx_cb (void)
* Created by		: halfin
* Date created		: 03.03.04
* Description		: UART callback function
* Notes:	called under ISR
**************************************************/
void site_tx_cb(void){
	if(site_part_to_tx < SITE_PARTICLES){
		memcpy_P((I8*)Tx_buffer, site[site_part_to_tx], SITE_PARTICLE_SIZE);
		site_part_to_tx++;
		UART_transmit (Tx_buffer, sizeof(Tx_buffer), site_tx_cb);
	} else {
		site_part_to_tx = 0;
		memcpy_P((I8*)Tx_buffer, site_end, sizeof(site_end));
		UART_transmit (Tx_buffer, sizeof(site_end)-1, Tx_cb);
	}
	
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