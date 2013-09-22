/***************************************************
*	Include Section
***************************************************/
#include <WIFI_API.h>
#include "string.h"
#include "pgmspace.h"
/***************************************************
*	Defines Section
***************************************************/
#define TIME_BTW_CMDS 			400
#define WAIT_BEFORE_SEND_CMD 	1500
#define UART_RX_BUFFER_SIZE 	32
#define REQUEST_SEARCH_OFFSET 	10

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

#define SITE_PARTICLES 39
#define SITE_PARTICLE_SIZE 128

const __flash I8 site[SITE_PARTICLES][SITE_PARTICLE_SIZE] = {
"<!DOCTYPE html><html><head><title>DIS Matrix</title><style>*{margin:0px;padding:0px;}canvas{    float:left;border-left:2px solid",
";border-bottom:2px solid;margin-top:25px;}li{    list-style-type:none;}#dim0,#dim1,#dim2,#dim3{    margin-bottom:5px;}.c{margin:",
"0 auto; width:1300px;}.active{color:green;}.graph{float:left;position:relative;height:380px;}.descr{float:left;width:80px;}.desc",
"r ul li{border-bottom:1px solid #ccc;padding:5px 0px;}.cl{clear:both;}.x{width:20px;height:18px;position:absolute;right:55px;bot",
"tom:32px;}.y{float:left;width:58px;}ul.ylvl li{margin:10px 9px;text-align:right;}</style></head><body><select id=\"time\"><option ",
"value=\"0\">No</option><option value=\"3\">3 sec</option><option value=\"5\">5 sec</option></select><input type=\"button\" onclick=\"get(",
");\" value=\"Get\" /><div id=\"data\"></div><div id=\"status\"></div><div class=\"c\"><div class=\"graph\"><div class=\"y\"><span id=\"dim0\"><",
"/span><ul class=\"ylvl\"><li id=\"max0\"></li><li id=\"y08\"></li><li id=\"y07\"></li><li id=\"y06\"></li><li id=\"y05\"></li><li id=\"y04\"><",
"/li><li id=\"y03\"></li><li id=\"y02\"></li><li id=\"y01\"></li><li id=\"y00\"></li><li>0</li></ul></div><canvas id=\"c1\" width=\"500\" hei",
"ght=\"300\"></canvas><div class=\"descr\"><ul><li id=\"d01\"></li><li id=\"d02\"></li><li id=\"d03\"></li><li id=\"d04\"></li></ul></div><di",
"v class=\"x\">t,c</div></div><div class=\"graph\"><div class=\"y\"><span id=\"dim1\"></span><ul class=\"ylvl\">    <li id=\"max1\"></li><li ",
"id=\"y18\"></li><li id=\"y17\"></li><li id=\"y16\"></li><li id=\"y15\"></li><li id=\"y14\"></li><li id=\"y13\"></li><li id=\"y12\"></li><li id",
"=\"y11\"></li><li id=\"y10\"></li><li>0</li></ul></div><canvas id=\"c2\" width=\"500\" height=\"300\"></canvas><div class=\"descr\"><ul><li ",
"id=\"d11\"></li><li id=\"d12\"></li><li id=\"d13\"></li><li id=\"d14\"></li></ul></div><div class=\"x\">t,c</div></div><div class=\"graph\">",
"<div class=\"y\"><span id=\"dim2\"></span><ul class=\"ylvl\"><li id=\"max2\"></li><li id=\"y28\"></li><li id=\"y27\"></li><li id=\"y26\"></li>",
"<li id=\"y25\"></li><li id=\"y24\"></li><li id=\"y23\"></li><li id=\"y22\"></li><li id=\"y21\"></li><li id=\"y20\"></li><li>0</li></ul></div",
"><canvas id=\"c3\" width=\"500\" height=\"300\"></canvas><div class=\"descr\"><ul><li id=\"d21\"></li><li id=\"d22\"></li><li id=\"d23\"></li>",
"<li id=\"d24\"></li></ul></div><div class=\"x\">t,c</div></div><div class=\"graph\"><div class=\"y\"><span id=\"dim3\"></span><ul class=\"y",
"lvl\"><li id=\"max3\"></li><li id=\"y38\"></li><li id=\"y37\"></li><li id=\"y36\"></li><li id=\"y35\"></li><li id=\"y34\"></li><li id=\"y33\"><",
"/li><li id=\"y32\"></li><li id=\"y31\"></li><li id=\"y30\"></li><li>0</li></ul></div><canvas id=\"c4\" width=\"500\" height=\"300\"></canvas",
"><div class=\"descr\"><ul><li id=\"d31\"></li><li id=\"d32\"></li><li id=\"d33\"></li><li id=\"d34\"></li></ul></div><div class=\"x\">t,c</d",
"iv></div></div><script>var xPts = 50,yPts = 10,reg = /\\[(\\d),(\\d{1,}),(\\d{1,}),(\\d{1,}),(\\d{1,}),(\\d{1,}.\\d{1,})\\]/,n = \'N/A\',ch",
" = 300,cw = 500,d,y = [[], [], [], []],ad,dd,G = {0: document.getElementById(\"c1\").getContext(\"2d\"),1: document.getElementById(\"",
"c2\").getContext(\"2d\"),2: document.getElementById(\"c3\").getContext(\"2d\"),3: document.getElementById(\"c4\").getContext(\"2d\")},gas =",
" {0: n,1: \'O2\',2: \'H2\',3: \'CH4\',14: \'NO2\'},dt = {0: n, 1: \'Elektrohim\', 2: \'Termokat\'},v = {0: n, 30: \'3 B\', 50: \'5 B\'},s = {1: ",
"\'Active\', 2: \'No Data\', 3: \'Not connected\'},dim = {0: n, 1: \'ppm\', 3: \'%\'};function gById(id, v) {document.getElementById(id).in",
"nerHTML = v;}function get() {var xmlhttp;try {xmlhttp = new ActiveXObject(\"Msxml2.XMLHTTP\");} catch (e) {try {xmlhttp = new Acti",
"veXObject(\"Microsoft.XMLHTTP\");} catch (E) {xmlhttp = false;}}if (!xmlhttp && typeof XMLHttpRequest != \'undefined\') {xmlhttp = n",
"ew XMLHttpRequest();}xmlhttp.open(\'GET\', \'s.php?a=1\', true);     xmlhttp.onreadystatechange = function() {if (xmlhttp.readyState",
" == 4) {if (xmlhttp.status == 200) {d = reg.exec(xmlhttp.responseText);gById(\"data\",xmlhttp.responseText);if (d == null){    gBy",
"Id(\"status\",\"reg_err\");    return;} else {    gById(\"status\",\"reg_ok\");}ad = d[1];dd = d[6];gById(\"d\" + ad + \"1\", dt[d[2]]);gByI",
"d(\"d\" + ad + \"2\", gas[d[3]]);gById(\"dim\" + ad, \'C, \' + dim[d[4]]);gById(\"d\" + ad + \"3\", v[d[5]]);gById(\"d\" + ad + \"4\", (dd) ? s[",
"1] : s[2]);if (y[ad].length >= xPts) {for (var i = 0; i < xPts - 1; i++)y[ad][i] = y[ad][i + 1];y[ad][xPts - 1] = dd;} else {y[a",
"d].push(dd);}newGraph();var sel = document.getElementById(\"time\");var t = sel.options[sel.selectedIndex].value;if (t != 0)setTim",
"eout(get, t * 1000);}}};xmlhttp.send(null);};function MAX(a) {var r = 0;for (var i = 0; i < a.length + 1; i++)if (a[i] > r)r = a",
"[i];return r;};function newGraph() {var max = 0,lvl = 0,curlvl = 0,rest = 0;if (d[4] === \'3\') {max = 100;} else {max = MAX(y[ad]",
");rest = max % 1000;max = max + (1000 - rest);}lvl = max / yPts;gById(\'max\' + ad, max);for (var j = 0; j < yPts - 1; j++) {curlv",
"l += lvl;gById(\'y\' + ad + j, curlvl);}curlvl = 0;G[ad].fillStyle = \"#fff\";G[ad].fillRect(0, 0, cw, ch);G[ad].fillStyle = \"#000\";",
"for (var j = 0; j < y[ad].length; j++) {G[ad].beginPath();G[ad].arc(j * 9, Math.ceil((ch - (ch * parseFloat(y[ad][j]) / max))), ",
};

const __flash I8 site_end[] = "2, 0, 2 * Math.PI);G[ad].fill();G[ad].stroke();}};</script></body></html>";

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
U8 data_request[] = "GET /s.php?a=1";
U8 http_OK_headers[] = "HTTP/1.1$200$OK\r\nConnection:$close\r\n\n\n";
U8 http_404_headers[] = "HTTP/1.1$400$Not$Found\r\nConnection:$close\r\n\n\n";
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
			time = WAIT_BEFORE_SEND_CMD;
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





