/***************************************************
*	Include Section
***************************************************/
#include <device.h>
#include <UART_API.h>
/***************************************************
*  User Settings section
***************************************************/
#define SITE_PARTICLES 41
#define SITE_PARTICLE_SIZE 128

#define WIFI_UART_RATE	9600

/***************************************************
*  Site
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

"lhttp.open(\'GET\', \'server.php?a=1\', true);xmlhttp.onreadystatechange = function() {if (xmlhttp.readyState == 4) {if (xmlhttp.sta",

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

/***************************************************
*	API Function Prototype Section
***************************************************/
void WIFI_init (void);

void WIFI_set_data_ptr (U8* data_ptr);

void WIFI_timer_cout(void);	// put into timer callback
void WIFI_while_cout(void);	// put into while