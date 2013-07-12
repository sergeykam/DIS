/***************************************************
*	Include Section
***************************************************/
#include <device.h>
#include "string.h"
#include <TIMER0_HW_API.h>
#include <DIS_API.h>
//#include <WIFI_API.h>

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

#define DIS_REQUEST_PERIOD	 20000		// DIV 4000 -> sec
#define DIS_ATTEMPTS_NUMBER	 50

/***************************************************
*	Function Prototype Section
***************************************************/
void timer_cb(void);
void DIS_matrix_while_cout(void);
void DIS_matrix_timer_cout(void);
void get_data_cb(U8 *data);
void get_configuration_cb(U8 *data);
/***************************************************
*	Static Variables Section
***************************************************/
union
{
	struct 
	{
		U8 sort_sensor;
		U8 sort_gas;	
		U8 unit;
		U8 voltage;
		union
		{
			float data_var;
			U8 data_buf[4];
		} data;
	} data_pos;
	
	U8 frame[32];
} DIS_data[4];

static U8 state;
static U8 state_next;
static U16 time;
static U8 last_active_DIS;
static U8 attempts_number;

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void main(void)
{
	DIS_init();
//	WIFI_init();
//	WIFI_set_data_ptr (DIS_data[0].frame);
	TIMER0_HW_API_init (timer_cb);

	state = CONFIGURATION;
	
	__enable_interrupt();
	
	while(1){
//		WIFI_while_cout();
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
			}
			break;
		case DATA:
			if(DIS_get_data(last_active_DIS, get_data_cb)){
				state = BUSY;
				state_next = WAIT;
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
	if(data){
		memcpy(DIS_data[last_active_DIS].data_pos.data.data_buf, data, 4);
		state = state_next;
//		switch (last_active_DIS)
//		{
//			case DIS1:
//			case DIS2:
//			case DIS3:
//				state = state_next;
//				last_active_DIS++;
//				break;
//			case DIS4:
//				state = WAIT;
//				state_next = CONFIGURATION;
//				last_active_DIS = DIS1;
//				time = DIS_REQUEST_PERIOD;
//				break;
//			default:
//				state = CONFIGURATION;
//				last_active_DIS = DIS1;
//				break;
//		}
	} else {
		attempts_number++;
		if(attempts_number == DIS_ATTEMPTS_NUMBER){
			state = state_next;
			attempts_number = 0;
			
			DIS_data[last_active_DIS].data_pos.data.data_buf[0] = 0;
			DIS_data[last_active_DIS].data_pos.data.data_buf[1] = 0;
			DIS_data[last_active_DIS].data_pos.data.data_buf[2] = 0;
			DIS_data[last_active_DIS].data_pos.data.data_buf[3] = 0;
		} else {
			state = DATA;
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
void get_configuration_cb(U8 *data)
{
	if(data){
		memcpy(&DIS_data[last_active_DIS].data_pos.sort_sensor, data, 4);
		state = state_next;
	} else {
		attempts_number++;
		if(attempts_number == DIS_ATTEMPTS_NUMBER){
			state = state_next;
			DIS_data[last_active_DIS].data_pos.sort_sensor = 0;
			attempts_number = 0;
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
void timer_cb(void)
{
//	WIFI_timer_cout();
	DIS_timer_cout();
	DIS_matrix_timer_cout();
}
