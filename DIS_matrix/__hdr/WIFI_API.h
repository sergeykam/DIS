/***************************************************
*	Include Section
***************************************************/
#include <device.h>
#include <UART_API.h>
/***************************************************
*  User Settings section
***************************************************/
#define SENSOR_DATA_SIZE	25

#define WIFI_UART_RATE	9600

/***************************************************
*	API Function Prototype Section
***************************************************/
void WIFI_init (void);

void WIFI_set_data_ptr (U8* data_ptr);

void WIFI_timer_cout(void);	// put into timer callback
void WIFI_while_cout(void);	// put into while