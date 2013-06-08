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
*С€
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
#define TIME_BTW_CMDS 255
#define WAIT_BEFORE_SEND_CMD 255


#define SITE_PARTICLES 41
#define SITE_PARTICLE_SIZE 128

/***************************************************
* CONSTS
***************************************************/

const __flash I8 site[SITE_PARTICLES][SITE_PARTICLE_SIZE] = {
"<!DOCTYPE html><html><head><title>Матрица ИГС</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=cp1251\"><style>",
"*{margin:0px;padding:0px;}body{font-family:Arial;font-size:14px;}canvas{float:left;border-left:2px solid;border-bottom:2px solid",
";margin-top:25px;}li{list-style-type: none;}#dim0,#dim1,#dim2,#dim3{margin-bottom: 5px;}.c{margin: 0 auto;width: 1320px;}.active",
"{color: green;}.graph{float: left;position: relative;height: 380px;}.descr{float: left;width: 80px;}.descr ul li{border-bottom: ",
"1px solid #ccc;padding: 5px 0px;}.cl{clear:both;}.x{width: 20px;height: 18px;position: absolute;right: 55px;bottom: 32px;}.y{flo",
"at: left;width: 58px;}.ylvl{margin: 14px 9px;text-align: right;}</style></head><body><div class=\"c\"><div class=\"graph\"><div clas",
"s=\"y\"><span id=\"dim0\">C, ppm</span><ul><li id=\"max0\" class=\"ylvl\"></li><li id=\"y08\" class=\"ylvl\"></li><li id=\"y07\" class=\"ylvl\">",
"</li><li id=\"y06\" class=\"ylvl\"></li><li id=\"y05\" class=\"ylvl\"></li><li id=\"y04\" class=\"ylvl\"></li><li id=\"y03\" class=\"ylvl\"></li",
"><li id=\"y02\" class=\"ylvl\"></li><li id=\"y01\" class=\"ylvl\"></li><li id=\"y00\" class=\"ylvl\"></li><li class=\"ylvl\">0</li></ul></div>",
"<canvas id=\"c1\" width=\"500\" height=\"300\">Ваш браузер не поддерживается</canvas><div class=\"descr\"><ul><li>H<sub>2</sub>S</li><li",
">5 Вольт</li><li>01/04/2013</li><li>Серия</li><li class=\"active\">Активен</li></ul></div><div class=\"x\">t, с</div></div><div clas",
"s=\"graph\"><div class=\"y\"><span id=\"dim1\">C, ppm</span><ul><li id=\"max1\" class=\"ylvl\"></li><li id=\"y18\" class=\"ylvl\"></li><li id=",
"\"y17\" class=\"ylvl\"></li><li id=\"y16\" class=\"ylvl\"></li><li id=\"y15\" class=\"ylvl\"></li><li id=\"y14\" class=\"ylvl\"></li><li id=\"y13",
"\" class=\"ylvl\"></li><li id=\"y12\" class=\"ylvl\"></li><li id=\"y11\" class=\"ylvl\"></li><li id=\"y10\" class=\"ylvl\"></li><li class=\"ylvl",
"\">0</li></ul></div><canvas id=\"c2\" width=\"500\" height=\"300\"></canvas><div class=\"descr\"><ul><li>H<sub>2</sub>S</li><li>5 Вольт</",
"li><li>01/04/2013</li><li>Серия</li><li class=\"active\">Активен</li></ul></div><div class=\"x\">t, с</div></div><div class=\"graph\">",
"<div class=\"y\"><span id=\"dim2\">C, ppm</span><ul><li id=\"max2\" class=\"ylvl\"></li><li id=\"y28\" class=\"ylvl\"></li><li id=\"y27\" clas",
"s=\"ylvl\"></li><li id=\"y26\" class=\"ylvl\"></li><li id=\"y25\" class=\"ylvl\"></li><li id=\"y24\" class=\"ylvl\"></li><li id=\"y23\" class=\"y",
"lvl\"></li><li id=\"y22\" class=\"ylvl\"></li><li id=\"y21\" class=\"ylvl\"></li><li id=\"y20\" class=\"ylvl\"></li><li class=\"ylvl\">0</li></",
"ul></div><canvas id=\"c3\" width=\"500\" height=\"300\"></canvas><div class=\"descr\"><ul><li>H<sub>2</sub>S</li><li>5 Вольт</li><li>01/",
"04/2013</li><li>Серия</li><li class=\"active\">Активен</li></ul></div><div class=\"x\">t, с</div></div><div class=\"graph\"><div class",
"=\"y\"><span id=\"dim3\">C, ppm</span><ul><li id=\"max3\" class=\"ylvl\"></li><li id=\"y38\" class=\"ylvl\"></li><li id=\"y37\" class=\"ylvl\"><",
"/li><li id=\"y36\" class=\"ylvl\"></li><li id=\"y35\" class=\"ylvl\"></li><li id=\"y34\" class=\"ylvl\"></li><li id=\"y33\" class=\"ylvl\"></li>",
"<li id=\"y32\" class=\"ylvl\"></li><li id=\"y31\" class=\"ylvl\"></li><li id=\"y30\" class=\"ylvl\"></li><li class=\"ylvl\">0</li></ul></div><",
"canvas id=\"c4\" width=\"500\" height=\"300\"></canvas><div class=\"descr\"><ul><li>H<sub>2</sub>S</li><li>5 Вольт</li><li>01/04/2013</l",
"i><li>Серия</li><li class=\"active\">Активен</li></ul></div><div class=\"x\">t, с</div></div></div><script>window.onload = function(",
") {var xPts=50,yPts=10,ch=300,cw=500,descr={},data=[[],[],[],[]],c1=document.getElementById(\"c1\").getContext(\"2d\"),c2=document.g",
"etElementById(\"c2\").getContext(\"2d\"),c3=document.getElementById(\"c3\").getContext(\"2d\"),c4=document.getElementById(\"c4\").getConte",
"xt(\"2d\");function get(){var xmlhttp;try{xmlhttp = new ActiveXObject(\"Msxml2.XMLHTTP\");}catch(e){try{xmlhttp = new ActiveXObject(",
"\"Microsoft.XMLHTTP\");}catch(E){xmlhttp = false;}}if(!xmlhttp && typeof XMLHttpRequest != \'undefined\') {xmlhttp = new XMLHttpRequ",
"est();}xmlhttp.open(\'GET\', \'server.php?a=1\', true);xmlhttp.onreadystatechange = function() {if (xmlhttp.readyState == 4) {if (xm",
"lhttp.status == 200) {console.log(JSON.parse(xmlhttp.responseText));parseAns(JSON.parse(xmlhttp.responseText));newScale();newGra",
"ph();}}};xmlhttp.send(null);};function parseAns(o) {descr = o.s;for (var i = 0; i < 4; i++) {if (data[i].length >= xPts) {for (v",
"ar j = 0; j < xPts; j++) {data[i][j] = data[i][j + 1];}data[i][data[i].length - 1] = o[i];} else {data[i][data[i].length] = o[i]",
";}}};function maximum(arr) {var res = 0;for (var i = 0; i < arr.length + 1; i++) {if (arr[i] > res) {res = arr[i];}}return res;}",
";function newScale() {var max = 0,lvl = 0,curlvl = 0,rest = 0;for (var i = 0; i < 4; i++) {max = maximum(data[i]);rest = max % 1",
"000;max = max + (1000 - rest);lvl = max / yPts;document.getElementById(\'max\' + i).innerHTML = max;for (var j = 0; j < yPts - 1; ",
"j++) {curlvl += lvl;document.getElementById(\'y\' + i + j).innerHTML = curlvl;}curlvl = 0;}};function newGraph() {var max=0;for (v",
"ar i=0;i<4;i++) {clr(i);for (var j=0;j<xPts;j++) {max = document.getElementById(\"max\" + i).innerHTML;drawRect(i, j * 9, (ch * da",
"ta[i][j] / max));}}};function drawRect(num,x,y) {var t=getG(num);t.fillRect(x,ch - y, 5, 2);};function clr(num) {var t = getG(nu",
"m);t.fillStyle = \"#fff\";t.fillRect(0, 0, cw, ch);t.fillStyle = \"#f00\";};function getG(n) {switch (n) {case 0:return c1;case 1:re",
};

const __flash I8 site_end[] = "turn c2;case 2:return c3;case 3:return c4;}};};</script></body></html>\n";

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
U8 wifi_status = IDLE;  // С‚РµРєСѓС‰Р°СЏ СЂР°Р±РѕС‚Р° СЃ РІР°Р№ С„Р°Р№
U16 wifi_timer_cnt = 1; 
U8 wifi_task = WAIT; // Р·Р°РґР°РЅРёРµ РґР»СЏ РІР°Р№ РІР°Р№
U8 wifi_task_after_tx = IDLE; // Р·Р°РґР°РЅРёРµ РґР»СЏ РІР°Р№ РІР°Р№
U8 site_part_to_tx = 0;
// task pointer
void (*task_ptr)(void) = 0;
// HTML
U8 Tx_buffer[SITE_PARTICLE_SIZE];
U8 Rx_buffer[16];
U8 Rx_get_site[] = "GET / HTTP";
U8 Rx_get_data[] = "GET /?a=1 HTTP";
U8 http_headers[] = "HTTP/1.1$200$OK\n\n";
//U8 site[] = "HTTP/1.1$200$OK\n\n\r<html><head><title>DIS</title></head><body>РўРµСЃС‚</body></html>\n\r";
//U8 site[] = "<html><head><title>DIS</title></head><body>РўРµСЃС‚</body></html>\n\r";

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
	if(IDLE == wifi_status){	
		if((0 == memcmp(Rx_buffer,Rx_get_site,10)) || (0 == memcmp(&Rx_buffer[1],Rx_get_site,10))){ 
			Rx_buffer[0] = 0;
			wifi_status = WAIT; 
			wifi_task = SEND_SITE;
		} else {
			if(0 == memcmp(Rx_buffer,Rx_get_data,10)){
				Rx_buffer[0] = 0;
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
		case BUSY:
  		case IDLE:
			break;
  		case WAIT:
			if(wifi_timer_cnt){
				wifi_timer_cnt--;
			} else { 
				wifi_status = wifi_task;
			}
			break;
		case SEND_SITE:
			wifi_status = BUSY;
			wifi_task = CMD;
			wifi_task_after_tx = WAIT;
			wifi_timer_cnt = WAIT_BEFORE_SEND_CMD;
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