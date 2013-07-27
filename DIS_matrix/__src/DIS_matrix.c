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

#define DIS_REQUEST_PERIOD	 (20000*4)		// DIV 4000 -> ~5 sec
#define DIS_ATTEMPTS_NUMBER	 50

/***************************************************
*	Function Prototype Section
***************************************************/
void timer_cb(void);
void DIS_matrix_while_cout(void);
void DIS_matrix_timer_cout(void);
void get_data_cb(U8 *data);
void get_configuration_cb(U8 *data);
void DIS_prepare_data(void);
//void switch_DIS(void);
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

//static I8 out_str_format[] = "[%s,%s,%s,%s,%s]";
static I8 out_conf_str_format[] = "[%d,%d,%.2d,%d,%.2d,%f]";
//static I8 out_data_str_format[] = "%.3f";
//static I8 out_data_com_str_format[] = "%*6s,%*6s,%*6s,%*6s";

//static I8 output_conf_string[10] = 
//{
//	"0,00,0,00",
//	"0,00,0,00",
//	"0,00,0,00",
//	"0,00,0,00"
//};
//static I8 output_data_string[4][4] = 
//{
//	"000",
//	"000",
//	"000",
//	"000"
//};

//static I8 output_all_data_string[] = "000,000,000,000";
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
//	switch_DIS();
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
//void switch_DIS(void)
//{
//	switch (last_active_DIS)
//	{
//		case DIS1:
//		case DIS2:
//		case DIS3:
//			state = state_next;
//			last_active_DIS++;
//			break;
//		case DIS4:
//			state = WAIT;
//			state_next = CONFIGURATION;
//			last_active_DIS = DIS1;
//			time = DIS_REQUEST_PERIOD;
//			break;
//		default:
//			state = CONFIGURATION;
//			last_active_DIS = DIS1;
//			break;
//	}
//}

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
