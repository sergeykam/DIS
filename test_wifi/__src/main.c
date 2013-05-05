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
* CONSTS
***************************************************/
//const __flash I8 site[][128] = {
//"<!DOCTYPE html><html><head><title>Матрица ИГС</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><style>*",
//"{margin:0px;padding:0px;}body{font-family:Arial;font-size:14px;}canvas{float:left;border-left:2px solid;border-bottom:2px solid",
//";margin-top:25px;}li{list-style-type:none;}#dim0,#dim1,#dim2,#dim3{margin-bottom:5px;}.c{margin:0 auto;width:1320px;}.active{co",
//"lor:green;}.graph{float:left;position:relative;height:380px;}.descr{float:left;width:80px;}.descr ul li{border-bottom:1px solid",
//" #ccc;padding:5px 0px;}.cl{clear:both;}.x{width:20px;height:18px;position:absolute;right:55px;bottom:32px;}.y{float:left;width:",
//"58px;}.ylvl{margin:14px 9px;text-align:right;}</style></head><body><div class=\"c\"><div class=\"graph\"><div class=\"y\"><span",
//" id=\"dim0\">C, ppm</span><ul><li id=\"max0\" class=\"ylvl\"></li><li id=\"y08\" class=\"ylvl\"></li><li id=\"y07\" class=\"ylvl\"></li><li i",
//"\"y06\" class=\"ylvl\"></li><li id=\"y05\" class=\"ylvl\"></li><li id=\"y04\" class=\"ylvl\"></li><li id=\"y03\" c",
//"lass=\"ylvl\"></li><li id=\"y02\" class=\"ylvl\"></li><li id=\"y01\" class=\"ylvl\"></li><li id=\"y00\" class=\"ylvl\"></li><li ",
//"class=\"ylvl\">0</li></ul></div><canvas id=\"c1\" width=\"500\" height=\"300\">Ваш браузер не поддерживается</canvas><div class=",
//"\"descr\"><ul><li>H<sub>2</sub>S</li><li>5 Вольт</li><li>01/04/2013</li><li>Серия</li><li class=\"active\">Активен</li></ul></di",
//"v><div class=\"x\">t, с</div></div><div class=\"graph\"><div class=\"y\"><span id=\"dim1\">C, ppm</span><ul><li id=\"max1\" clas",
//"s=\"ylvl\"></li><li id=\"y18\" class=\"ylvl\"></li><li id=\"y17\" class=\"ylvl\"></li><li id=\"y16\" class=\"ylvl\"></li><li id=",
//"\"y15\" class=\"ylvl\"></li><li id=\"y14\" class=\"ylvl\"></li><li id=\"y13\" class=\"ylvl\"></li><li id=\"y12\" class=\"ylvl\">",
//"</li><li id=\"y11\" class=\"ylvl\"></li><li id=\"y10\" class=\"ylvl\"></li><li class=\"ylvl\">0</li></ul></div><canvas id=\"c2\"",
//" width=\"500\" height=\"300\"></canvas><div class=\"descr\"><ul><li>H<sub>2</sub>S</li><li>5 Вольт</li><li>01/04/2013</li><li>Се",
//"рия</li><li class=\"active\">Активен</li></ul></div><div class=\"x\">t, с</div></div><div class=\"graph\"><div class=\"y\"><span",
//" id=\"dim2\">C, ppm</span><ul><li id=\"max2\" class=\"ylvl\"></li><li id=\"y28\" class=\"ylvl\"></li><li id=\"y27\" class=\"ylvl",
//"\"></li><li id=\"y26\" class=\"ylvl\"></li><li id=\"y25\" class=\"ylvl\"></li><li id=\"y24\" class=\"ylvl\"></li><li id=\"y23\" ",
//"class=\"ylvl\"></li><li id=\"y22\" class=\"ylvl\"></li><li id=\"y21\" class=\"ylvl\"></li><li id=\"y20\" class=\"ylvl\"></li><li",
//" class=\"ylvl\">0</li></ul></div><canvas id=\"c3\" width=\"500\" height=\"300\"></canvas><div class=\"descr\"><ul><li>H<sub>2</s",
//"ub>S</li><li>5 Вольт</li><li>01/04/2013</li><li>Серия</li><li class=\"active\">Активен</li></ul></div><div class=\"x\">t, с</div",
//"></div><div class=\"graph\"><div class=\"y\"><span id=\"dim3\">C, ppm</span><ul><li id=\"max3\" class=\"ylvl\"></li><li id=\"y38",
//"\" class=\"ylvl\"></li><li id=\"y37\" class=\"ylvl\"></li><li id=\"y36\" class=\"ylvl\"></li><li id=\"y35\" class=\"ylvl\"></li>",
//"<li id=\"y34\" class=\"ylvl\"></li><li id=\"y33\" class=\"ylvl\"></li><li id=\"y32\" class=\"ylvl\"></li><li id=\"y31\" class=\"",
//"ylvl\"></li><li id=\"y30\" class=\"ylvl\"></li><li class=\"ylvl\">0</li></ul></div><canvas id=\"c4\" width=\"500\" height=\"300\"",
//"\"></canvas><div class=\"descr\"><ul><li>H<sub>2</sub>S</li><li>5 Вольт</li><li>01/04/2013</li><li>Серия</li><li class=\"active\"",
//">Активен</li></ul></div><div class=\"x\">t, с</div></div></div></body></html>\n\r"
//};

#define WAITING_TIME  32;
#define TIME_BTW_CMDS 20;

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
U8 wifi_timer_cnt = WAITING_TIME; 
U8 wifi_task = WAIT; // задание для вай вай
U8 wifi_task_after_tx = IDLE; // задание для вай вай
U8 site_part_to_tx = 0;
// task pointer
void (*task_ptr)(void) = 0;
// HTML
U8 Tx_buffer[128];
U8 Rx_buffer[16];
U8 Rx_get_site[] = "GET / HTTP";
U8 Rx_get_data[] = "GET /?a=1 HTTP";
//U8 http_headers[] = "HTTP/1.1$200$OK\nContent-type:$text/html;$charset=utf-8;\n\n\r";
U8 site[] = "HTTP/1.1$200$OK\n\n\r<html><head><title>DIS</title></head><body>Тест</body></html>\n\r";
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
			UART_transmit (site, sizeof(site), Tx_cb);
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
//void site_tx_cb(void){
//	memcpy_P((I8*)Tx_buffer, site[site_part_to_tx], 128);
//	site_part_to_tx++;
//	if(28 != site_part_to_tx){
//		UART_transmit (Tx_buffer, sizeof(Tx_buffer), site_tx_cb);
//	} else {
//		site_part_to_tx = 0;
//		UART_transmit (Tx_buffer, sizeof(Tx_buffer), Tx_cb);
//	}
//	
//}
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