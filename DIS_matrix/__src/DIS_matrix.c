/***************************************************
*	Include Section
***************************************************/
#include <device.h>
#include "string.h"
#include "stdio.h"
#include <TIMER0_HW_API.h>
#include <DIS_API.h>
#include <WIFI_API.h>

/***************************************************
*	Defines Section
***************************************************/
enum DIS_matrix_state
{
	BUSY = 0,
	WAIT,
	CONFIGURATION,
	DATA
};

enum DIS_NUMBER
{
	DIS1 = 0,
	DIS2,
	DIS3,
	DIS4
};

enum PORT_STATE
{
	LOW = 0,
	HIGH = 1
};

#define LED1_DDR	DDRC_Bit3
#define LED1_PORT	PORTC_Bit3

#define LED2_DDR	DDRC_Bit5
#define LED2_PORT	PORTC_Bit5

#define LED3_DDR	DDRD_Bit7
#define LED3_PORT	PORTD_Bit7

#define LED4_DDR	DDRC_Bit4
#define LED4_PORT	PORTC_Bit4

#define DIS_REQUEST_PERIOD	 (4000)		// DIV 4000 -> ~ sec
#define DIS_ATTEMPTS_NUMBER	 30

/***************************************************
*	Function Prototype Section
***************************************************/
void timer_cb(void);
void DIS_matrix_while_cout(void);
void DIS_matrix_timer_cout(void);
void get_data_cb(U8 *data);
void get_configuration_cb(U8 *data);
void DIS_prepare_data(void);
void switch_DIS(void);
/***************************************************
*	Static Variables Section
***************************************************/
union
{
	float data_var;
	U8 data_buf[4];
} DIS_data[4];

static U8 DIS_conf[4][4];

static U8 state;
static U8 state_next;
static U16 time;
static U8 last_active_DIS;
static U8 attempts_number;

static I8 out_conf_str_format[] = "[%d,%d,%.2d,%d,%.2d,%f]";

I8 output_data[] = "[1,0,00,0,00,00000000]";

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void main(void)
{
	DDRD = 0xFF;
	PORTD_Bit1 = HIGH;
	
	LED1_DDR = HIGH;
	LED1_PORT = LOW;
	
	LED2_DDR = HIGH;
	LED2_PORT = LOW;
	
	LED3_DDR = HIGH;
	LED3_PORT = LOW;
	
	LED4_DDR = HIGH;
	LED4_PORT = LOW;
	
	DIS_init();
	WIFI_init();
	WIFI_set_data_ptr ((U8 *)output_data);
	TIMER0_HW_API_init (timer_cb);

	state = CONFIGURATION;
	
	__enable_interrupt();
	
	while(1){
		WIFI_while_cout();
		DIS_while_cout();
		DIS_matrix_while_cout();
	}
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void DIS_matrix_while_cout(void)
{
	switch (state)
	{
		case BUSY:
		case WAIT:
			break;
		case CONFIGURATION:
			if(DIS_get_configuration(last_active_DIS, get_configuration_cb)){
				state = BUSY;
				state_next = DATA;	
				LED1_PORT = HIGH;
			}
			break;
		case DATA:
			if(DIS_get_data(last_active_DIS, get_data_cb)){
				state = BUSY;
				state_next = WAIT;
				LED2_PORT = HIGH;
				time = DIS_REQUEST_PERIOD;
			}
			break;
		default:
			last_active_DIS = DIS1;
			state = CONFIGURATION;
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
void get_data_cb(U8 *data)
{
	LED2_PORT = LOW;
	
	if(!data && (attempts_number != DIS_ATTEMPTS_NUMBER)){
		attempts_number++;
		state = DATA;
		return;
	}
	
	if(!data && (attempts_number == DIS_ATTEMPTS_NUMBER)){
		DIS_data[last_active_DIS].data_buf[0] = 0;
		DIS_data[last_active_DIS].data_buf[1] = 0;
		DIS_data[last_active_DIS].data_buf[2] = 0;
		DIS_data[last_active_DIS].data_buf[3] = 0;
	} else if(data) {
		memcpy(DIS_data[last_active_DIS].data_buf, data, 4);
	}
	
	state = state_next;
	attempts_number = 0;
	
	DIS_prepare_data();
	switch_DIS();
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void get_configuration_cb(U8 *data)
{
	LED1_PORT = LOW;
	
	if(!data && (attempts_number != DIS_ATTEMPTS_NUMBER)){
		attempts_number++;
		state = CONFIGURATION;
		return;
	}
	
	if(!data && (attempts_number == DIS_ATTEMPTS_NUMBER)){
		DIS_conf[last_active_DIS][0] = 0;
		DIS_conf[last_active_DIS][1] = 0;
		DIS_conf[last_active_DIS][2] = 0;
		DIS_conf[last_active_DIS][3] = 0;
	} else if(data) {
		memcpy(DIS_conf[last_active_DIS], data, 4);
		DIS_conf[last_active_DIS][0] = DIS_conf[last_active_DIS][0] & 0x7F;
	}
	
	state = state_next;
	attempts_number = 0;
	
	DIS_prepare_data();
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void DIS_prepare_data(void)
{ 
	sprintf(output_data,
				out_conf_str_format,
				last_active_DIS,
				DIS_conf[last_active_DIS][0],
				DIS_conf[last_active_DIS][1],
				DIS_conf[last_active_DIS][2],
				DIS_conf[last_active_DIS][3],
				DIS_data[last_active_DIS].data_var);
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void DIS_matrix_timer_cout(void)
{
	if(WAIT == state)
	{
		if(time){
			time--;
		} else {
			state = CONFIGURATION;
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
void switch_DIS(void)
{
	switch (last_active_DIS)
	{
		case DIS1:
		case DIS2:
		case DIS3:
			last_active_DIS++;
			break;
		case DIS4:
			last_active_DIS = DIS1;
			break;
		default:
			last_active_DIS = DIS1;
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
void timer_cb(void)
{
	WIFI_timer_cout();
	DIS_timer_cout();
	DIS_matrix_timer_cout();
}
