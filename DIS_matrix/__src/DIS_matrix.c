/***************************************************
*	Include Section
***************************************************/
#include <device.h>
#include <TIMER0_HW_API.h>

/***************************************************
*	Defines Section
***************************************************/
#define SS_DDR_1  DDRB_Bit4
#define SS_PORT_1 PORTB_Bit4

#define SS_DDR_2  DDRC_Bit0
#define SS_PORT_2 PORTC_Bit0

#define SS_DDR_3  DDRC_Bit1
#define SS_PORT_3 PORTC_Bit1

#define SS_DDR_4  DDRC_Bit2
#define SS_PORT_4 PORTC_Bit2

/***************************************************
*	Function Prototype Section
***************************************************/
// Common
void timer_cb(void);

/***************************************************
*	Static Variables Section
***************************************************/



/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void main(void)
{
	__enable_interrupt();
	
    while(1){
		DIS_cout();
		WIFI_cout();
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
	// DIS timer handling
	if(DIS_status == WAIT){
		if(DIS_time){
			DIS_time--;
		} else {
			DIS_status = DIS_status_next;
		}
	}
	// WIFI status handling
	if(wifi_status == WAIT){
		if(wifi_timer_cnt){
			wifi_timer_cnt--;
		} else { 
			wifi_status = wifi_task;
		}
	}
}