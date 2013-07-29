/***************************************************
*	Include Section
***************************************************/
#include "string.h"
#include <device.h>
#include <SPI_API.h>
/***************************************************
*  User Settings section
***************************************************/
#define SS_DDR_1  DDRC_Bit1
#define SS_PORT_1 PORTC_Bit1

#define SS_DDR_2  DDRC_Bit0
#define SS_PORT_2 PORTC_Bit0

#define SS_DDR_3  DDRB_Bit2
#define SS_PORT_3 PORTB_Bit2

#define SS_DDR_4  DDRC_Bit2
#define SS_PORT_4 PORTC_Bit2

// SPI PRESCALER (2,4,8,16,32,64,128)
#define DIS_SPI_PRS 	2

// TIMINGS

#define DIS_TIME_BTW_ATTEMPTS			4

// ATTEMPSTS
#define DIS_ATTEMPTS				40
/***************************************************
*	API Function Prototype Section
***************************************************/
void DIS_init (void);

U8 DIS_get_data(U8 DIS_num, void (*callback)(U8 *data));
U8 DIS_get_configuration(U8 DIS_num, void (*callback)(U8 *configuration));

void DIS_timer_cout(void);	// put into timer callback
void DIS_while_cout(void);	// put into while
