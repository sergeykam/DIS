/***************************************************
*	Include Section
***************************************************/
#include <WIFI_API.h>
#include "string.h"
#include "pgmspace.h"
/***************************************************
*	Defines Section
***************************************************/
#define WAITING_TIME  32
#define TIME_BTW_CMDS 255
#define WAIT_BEFORE_SEND_CMD 255

enum WIFI_state
{
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
*	Variables Section
***************************************************/
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
U8 Rx_buffer[16];
U8 site_request[] = "GET / HTTP";
U8 data_request[] = "GET /?a=1";
U8 http_headers[] = "HTTP/1.1$200$OK\n\n";
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
		if((0 == memcmp(Rx_buffer,site_request,10)) || (0 == memcmp(&Rx_buffer[1],site_request,10))){ 
			Rx_buffer[0] = 0;
			status = SEND_SITE;
		} else {
			if(0 == memcmp(Rx_buffer,data_request,9)){
				Rx_buffer[0] = 0;
				status = SEND_DATA;
			} else {
				UART_receive (Rx_buffer, sizeof(Rx_buffer), 10000, Rx_cb);
			}
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
			break;
		case SEND_SITE:
			status = BUSY;
			task = CMD;
			task_after_tx = WAIT;
			time = WAIT_BEFORE_SEND_CMD;
			UART_transmit (http_headers, sizeof(http_headers)-1, site_tx_cb);
			break;
		case SEND_DATA:
			status = BUSY;
			task = CMD;
			task_after_tx = WAIT;
			time = WAIT_BEFORE_SEND_CMD;
			UART_transmit (user_data_ptr, 32, Tx_cb);
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





