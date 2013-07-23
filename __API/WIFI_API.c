/***************************************************
*	Include Section
***************************************************/
#include <WIFI_API.h>
#include "string.h"
#include "pgmspace.h"
/***************************************************
*	Defines Section
***************************************************/
#define TIME_BTW_CMDS 			1000
#define WAIT_BEFORE_SEND_CMD 	1000
#define UART_RX_BUFFER_SIZE 	64
#define REQUEST_SEARCH_OFFSET 	8
#define SENSOR_DATA_SIZE	48

enum WIFI_state
{
  IDLE = 0,
  WAIT,
  SEND_HTTP_HEADERS,
  SEND_404_HEADERS,
  SEND_SITE,
  SEND_DATA,
  CMD,
  CLOSE,
  EXIT,
  BUSY
};

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
*	SITE
***************************************************/

const __flash I8 site[SITE_PARTICLES][SITE_PARTICLE_SIZE] = {
"<!DOCTYPE html><html><head><title>DIS Matrix</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><style>*{",
"margin:0px;padding:0px;}body{font-family:Arial;font-size:14px;}canvas{float:left;border-left:2px solid;border-bottom:2px solid;m",
"argin-top: 25px;}li{list-style-type: none;}#dim0,#dim1,#dim2,#dim3{margin-bottom: 5px;}.c{margin: 0 auto;width: 1300px;}.active{",
"color: green;}.graph{float: left;position: relative;height: 380px;}.descr{float: left;width: 80px;}.descr ul li{border-bottom: 1",
"px solid #ccc;padding: 5px 0px;}.cl{clear:both;}.x{width: 20px;height: 18px;position: absolute;right: 55px;bottom: 32px;}.y{floa",
"t: left;width: 58px;}.ylvl{margin: 14px 9px;text-align: right;}</style></head><body>            <button onclick=\"get(); return f",
"alse;\">Get data</button><div id=\"data\"></div><div class=\"c\"><div class=\"graph\"><div class=\"y\"><span id=\"dim0\">C, ppm</span><ul><",
"li id=\"max0\" class=\"ylvl\"></li><li id=\"y08\" class=\"ylvl\"></li><li id=\"y07\" class=\"ylvl\"></li><li id=\"y06\" class=\"ylvl\"></li><li ",
"id=\"y05\" class=\"ylvl\"></li><li id=\"y04\" class=\"ylvl\"></li><li id=\"y03\" class=\"ylvl\"></li><li id=\"y02\" class=\"ylvl\"></li><li id=\"",
"y01\" class=\"ylvl\"></li><li id=\"y00\" class=\"ylvl\"></li><li class=\"ylvl\">0</li></ul></div><canvas id=\"c1\" width=\"500\" height=\"300\"",
"></canvas><div class=\"descr\"><ul><li>H<sub>2</sub>S</li><li>5 V</li><li>01/04/2013</li><li></li><li class=\"active\">Active</li></",
"ul></div><div class=\"x\">t, ñ</div></div><div class=\"graph\"><div class=\"y\"><span id=\"dim1\">C, ppm</span><ul><li id=\"max1\" class=\"",
"ylvl\"></li><li id=\"y18\" class=\"ylvl\"></li><li id=\"y17\" class=\"ylvl\"></li><li id=\"y16\" class=\"ylvl\"></li><li id=\"y15\" class=\"ylvl",
"\"></li><li id=\"y14\" class=\"ylvl\"></li><li id=\"y13\" class=\"ylvl\"></li><li id=\"y12\" class=\"ylvl\"></li><li id=\"y11\" class=\"ylvl\"></",
"li><li id=\"y10\" class=\"ylvl\"></li><li class=\"ylvl\">0</li></ul></div><canvas id=\"c2\" width=\"500\" height=\"300\"></canvas><div class",
"=\"descr\"><ul><li>H<sub>2</sub>S</li><li>5 V</li><li>01/04/2013</li><li></li><li class=\"active\">Active</li></ul></div><div class=",
"\"x\">t, ñ</div></div><div class=\"graph\"><div class=\"y\"><span id=\"dim2\">C, ppm</span><ul><li id=\"max2\" class=\"ylvl\"></li><li id=\"y",
"28\" class=\"ylvl\"></li><li id=\"y27\" class=\"ylvl\"></li><li id=\"y26\" class=\"ylvl\"></li><li id=\"y25\" class=\"ylvl\"></li><li id=\"y24\" ",
"class=\"ylvl\"></li><li id=\"y23\" class=\"ylvl\"></li><li id=\"y22\" class=\"ylvl\"></li><li id=\"y21\" class=\"ylvl\"></li><li id=\"y20\" clas",
"s=\"ylvl\"></li><li class=\"ylvl\">0</li></ul></div><canvas id=\"c3\" width=\"500\" height=\"300\"></canvas><div class=\"descr\"><ul><li>H<s",
"ub>2</sub>S</li><li>5 V</li><li>01/04/2013</li><li></li><li class=\"active\">Active</li></ul></div><div class=\"x\">t, ñ</div></div>",
"<div class=\"graph\"><div class=\"y\"><span id=\"dim3\">C, ppm</span><ul><li id=\"max3\" class=\"ylvl\"></li><li id=\"y38\" class=\"ylvl\"></l",
"i><li id=\"y37\" class=\"ylvl\"></li><li id=\"y36\" class=\"ylvl\"></li><li id=\"y35\" class=\"ylvl\"></li><li id=\"y34\" class=\"ylvl\"></li><l",
"i id=\"y33\" class=\"ylvl\"></li><li id=\"y32\" class=\"ylvl\"></li><li id=\"y31\" class=\"ylvl\"></li><li id=\"y30\" class=\"ylvl\"></li><li cl",
"ass=\"ylvl\">0</li></ul></div><canvas id=\"c4\" width=\"500\" height=\"300\"></canvas><div class=\"descr\"><ul><li>H<sub>2</sub>S</li><li>",
"5 V</li><li>01/04/2013</li><li></li><li class=\"active\">Active</li></ul></div><div class=\"x\">t, ñ</div></div></div><script>var xP",
"ts=50,yPts=10,ch=300,cw=500,descr={},data=[[],[],[],[]],c1=document.getElementById(\"c1\").getContext(\"2d\"),c2=document.getElement",
"ById(\"c2\").getContext(\"2d\"),c3=document.getElementById(\"c3\").getContext(\"2d\"),c4=document.getElementById(\"c4\").getContext(\"2d\");",
"function get(){var xmlhttp;try{xmlhttp = new ActiveXObject(\"Msxml2.XMLHTTP\");}catch(e){try{xmlhttp = new ActiveXObject(\"Microsof",
"t.XMLHTTP\");}catch(E){xmlhttp = false;}}if(!xmlhttp && typeof XMLHttpRequest != \'undefined\') {xmlhttp = new XMLHttpRequest();}xm",
"lhttp.open(\'GET\', \'?a=1\', true);xmlhttp.onreadystatechange = function() {if (xmlhttp.readyState == 4)           {if (xmlhttp.sta",
"tus == 200) {parseAns();newScale();newGraph();                                                                writeRes(xmlhttp.r",
"esponseText);}}};xmlhttp.send(null);};function parseAns(o) {};function maximum(arr) {var res = 0;for (var i = 0; i < arr.length ",
"+ 1; i++) {if (arr[i] > res) {res = arr[i];}}return res;};function newScale() {var max = 0,lvl = 0,curlvl = 0,rest = 0;for (var ",
"i = 0; i < 4; i++) {max = maximum(data[i]);rest = max % 1000;max = max + (1000 - rest);lvl = max / yPts;document.getElementById(",
"\'max\' + i).innerHTML = max;for (var j = 0; j < yPts - 1; j++) {curlvl += lvl;document.getElementById(\'y\' + i + j).innerHTML = cu",
"rlvl;}curlvl = 0;}};function newGraph() {var max=0;for (var i=0;i<4;i++) {clr(i);for (var j=0;j<xPts;j++) {max = document.getEle",
"mentById(\"max\" + i).innerHTML;drawRect(i, j * 9, (ch * data[i][j] / max));}}};function drawRect(num,x,y) {var t=getG(num);t.fill",
"Rect(x,ch - y, 5, 2);};function clr(num) {var t = getG(num);t.fillStyle = \"#fff\";t.fillRect(0, 0, cw, ch);t.fillStyle = \"#f00\";}",
";function getG(n) {switch (n) {case 0:return c1;case 1:return c2;case 2:return c3;case 3:return c4;}};                          ",
"                                      function writeRes(d){                                       document.getElementById(\"data\"",
};

const __flash I8 site_end[] = ").innerHTML = d;}</script></body></html>";

/***************************************
*		VARs
***************************************/

// CMDs
U8 cmd[] = "$$$";
U8 close[] = "close\n";
U8 ex[] = "exit\n";
// statuses
static U8 status;
static U16 time; 
static U8 task;
static U8 task_after_tx;
static U8 site_part_to_tx;
// HTML
U8 Tx_buffer[SITE_PARTICLE_SIZE];
U8 Rx_buffer[UART_RX_BUFFER_SIZE];
U8 site_request[] = "GET / HTTP";
U8 data_request[] = "GET /?a=1";
U8 http_OK_headers[] = "HTTP/1.1$200$OK\r\nServer:$Atmega16\r\nConnection:$close\r\n\n\n";
U8 http_404_headers[] = "HTTP/1.1$400$Not$Found\r\nServer:$Atmega16\r\nConnection:$close\r\n\n\n";
// user's settings
static U8* user_data_ptr;

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void WIFI_init (void)
{
	DDRD = 0xFF;
	PORTD = 0xFF;
	UART_init (WIFI_UART_RATE, DATABIT8 + STOPBIT1 + NOPARITY);
	UART_set_Rx_tout_cb (timeout_cb);
	UART_set_Rx_error_cb (error_cb);
	UART_receive (Rx_buffer, sizeof(Rx_buffer), 1000, Rx_cb);
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void Rx_cb (U8 *buffer, U8 length)
{
	if(IDLE == status){	
		for(U8 i = 0; i < REQUEST_SEARCH_OFFSET; i++){
			if(0 == memcmp(&Rx_buffer[i],site_request,10)){
				status = SEND_HTTP_HEADERS;
				task_after_tx = SEND_SITE;
				return;
			} else if(0 == memcmp(&Rx_buffer[i],data_request,9)){
				status = SEND_HTTP_HEADERS;
				task_after_tx = SEND_DATA;
				return;
			}
		}
		status = SEND_404_HEADERS;
		task_after_tx = IDLE;
	}
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void WIFI_set_data_ptr (U8* data_ptr)
{
	user_data_ptr = data_ptr;
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void WIFI_timer_cout(void)
{
	if(WAIT == status){
		if(time){
			time--;
		} else { 
			status = task;
		}		
	}
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void WIFI_while_cout(void)
{
	switch (status)
	{
		case BUSY:
  		case IDLE:
		case WAIT:
			break;
		case SEND_HTTP_HEADERS:
			status = BUSY;
			UART_transmit (http_OK_headers, sizeof(http_OK_headers)-1, Tx_cb);
			break;
		case SEND_404_HEADERS:
			status = BUSY;
			task = CMD;
			task_after_tx = WAIT;
			time = WAIT_BEFORE_SEND_CMD;
			UART_transmit (http_404_headers, sizeof(http_404_headers)-1, Tx_cb);
			break;
		case SEND_SITE:
			status = BUSY;
			task = CMD;
			task_after_tx = WAIT;
			time = WAIT_BEFORE_SEND_CMD;
			site_tx_cb();
			break;
		case SEND_DATA:
			status = BUSY;
			task = CMD;
			task_after_tx = WAIT;
			time = WAIT_BEFORE_SEND_CMD;
			UART_transmit (user_data_ptr, SENSOR_DATA_SIZE, Tx_cb);
			break;
  		case CMD:
			status = BUSY;
			task = CLOSE;
			task_after_tx = WAIT;
			time = TIME_BTW_CMDS;
			UART_transmit (cmd, 3, Tx_cb);
			break;
  		case CLOSE:
			status = BUSY;
			task = EXIT;
			task_after_tx = WAIT;
			time = TIME_BTW_CMDS;
			UART_transmit (close, 7, Tx_cb);
			break;
  		case EXIT:
			status = IDLE;
			task_after_tx = IDLE;
			task = IDLE;
    		UART_transmit (ex, 6, exit_cb);
			break;
		default:
			status = IDLE;
			task = IDLE;
			task_after_tx = IDLE;
			UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
			break;
	}
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void timeout_cb (U8 *buffer, U8 length)
{
	UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void error_cb (U8 *buffer, U8 length, U8 error)
{
	UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
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
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void exit_cb(void){
	UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void Tx_cb(void){
	status = task_after_tx;
}





