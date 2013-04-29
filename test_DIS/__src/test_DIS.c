/****************************************************************
* Module name: SPI_API.c
*
* Copyright	2005 TeaMiX as an unpublished work.
* All Rights Reserved.
*
* The information contained herein is confidential
* property of TeaMiX. The user, copying, transfer or
* disclosure of	such information is	prohibited except
* by express written agreement with	TeaMiX.
*�
* First	written	on ______ by ______
*
* Module Description: realization of SPI_API functions for ATmega
*****************************************************************/
/***************************************************
*	Include Section
***************************************************/
#include "string.h"
#include <device.h>
#include <SPI_API.h>
#include <TIMER0_HW_API.h>
/***************************************************
*	Defines Section
***************************************************/
enum ss_level
{
  LOW = 0,
  HIGH,
};

enum DIS_state
{
	IDLE = 0,
	WAIT,
	BUSY,
	SEND_CMD,
	WAIT_DATA_READY,
	GET_DATA,
};

#define SS_DDR_1  DDRB_Bit2
#define SS_PORT_1 PORTB_Bit2

#define SS_DDR_2  DDRC_Bit0
#define SS_PORT_2 PORTC_Bit0

#define SS_DDR_3  DDRC_Bit1
#define SS_PORT_3 PORTC_Bit1

#define SS_DDR_4  DDRC_Bit2
#define SS_PORT_4 PORTC_Bit2

#define OK					0x01
#define ERROR_CONTROL_BYTE	0x02

#define CONFIGURATION		0x01
#define DATA				0x03


#define ERROR_CNT			10

// TIMINGS
#define DIS_NO_TIMER				0
#define DIS_WAITING_TIME			50
#define DIS_TIME_BTW_1_2_PHASE		10
#define DIS_TIME_BTW_2_3_PHASE		1
/***************************************************
*	Function Prototype Section
***************************************************/
void read_callback(U8 *Rx_buffer, U8 length);
//void control_callback(U8 *Rx_buffer, U8 length);
U8 Crc8(U8 *pcBlock, U8 len);
//void sensor_cb (void);
void ss_low(void);
void ss_high(void);
void timer_cb(void);
void DIS_cout(void);
/***************************************************
*	Static Variables Section
***************************************************/
union
{
	float x;
	U8 buffer[4];
}float_x;

union write_packet_union
{
	struct
	{
		U8 start_byte;
		U8 cmd_number;
		U8 data[4];
		U8 CRC;	
	}write_pos;
  
	U8 write_frame[7];
	
} write_packet;

union read_packet_union
{
	struct
	{
		U8 data[4];
		U8 CRC;	
	}read_pos;
  
	U8 read_frame[5];
	
} read_packet;

union
{
	struct 
	{
		U8 sort_gas;
		U8 sort_sensor;
		U8 unit;
		U8 voltage;
		union
		{
			float data_var;
			U8 data_buf[4];
		} data;
		U8 status;
	}sensor_data;
	
	U8 data_buffer[9];
}sensor_union[4];


// STATUSES
U8 DIS_status = IDLE;
U8 DIS_status_next = IDLE;
U8 DIS_status_after_transfer = IDLE;

// timer
U8 DIS_time = DIS_WAITING_TIME;

// DIS
static U8 DIS_number;
static U8 DIS_control_byte;

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void main (void)
{	
	SS_DDR_1 = HIGH;
	SS_DDR_2 = HIGH;
	SS_DDR_3 = HIGH;
	SS_DDR_4 = HIGH;
	SPI_init (SPI_MASTER + SPI_IDLE_SCK_LOW + SPI_SAMPLE_SETUP + SPI_MSB, 128);
	TIMER0_HW_API_init (timer_cb);
	DIS_status_next = SEND_CMD;
	DIS_status = WAIT;
	DIS_number = 0x00;
	__enable_interrupt();
 
	while(1)
	{
		DIS_cout();
	};
}
/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void DIS_cout(void){
	switch (DIS_status){
		case (IDLE):
		case (WAIT):
		case (BUSY):
			break;
		case (SEND_CMD):
			// ����������� ���� �����
			DIS_status_after_transfer = WAIT;
			DIS_status_next = WAIT_DATA_READY;
			DIS_status = BUSY;
			DIS_time = DIS_TIME_BTW_1_2_PHASE;
			// ��������� ����� ��� ��������
			write_packet.write_pos.start_byte = 0xAA;
			write_packet.write_pos.cmd_number = 0x02;
			write_packet.write_pos.CRC = Crc8(write_packet.write_frame, 6);
			for(U8 i = 0; i < 4; i++)
			{
				write_packet.write_pos.data[i] = 0x00;
			}
			ss_low();
			SPI_transfer (write_packet.write_frame, read_packet.read_frame, 7, read_callback);
			break;
		case (WAIT_DATA_READY):
			switch(DIS_control_byte)
			{
				case (0x00):
				case (0xA5):
					DIS_time = DIS_TIME_BTW_1_2_PHASE;
					DIS_status_after_transfer = WAIT;
					DIS_status_next = WAIT_DATA_READY;
					ss_low();
					SPI_transfer (0, &DIS_control_byte, 1, read_callback);
					break;
				case (0x55):
				case (0x5A):
				case (0xCC):
				case (0xDD):
				case (0xFF):
//					break;
				default:
					DIS_time = DIS_NO_TIMER;
					DIS_status_after_transfer = WAIT_DATA_READY;
					DIS_status = BUSY;
					ss_low();
					SPI_transfer (0, &DIS_control_byte, 1, read_callback);
//					SPI_transfer (0, read_packet.read_frame, 5, read_callback);
					break;
			}
			break;
		case (GET_DATA):
			memcpy(read_packet.read_frame,sensor_union[DIS_number].sensor_data.data.data_buf,4);
			break;
		default:
			break;
	}
};

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void timer_cb(void)
{
	if(DIS_status == WAIT){
		if(DIS_time){
			DIS_time--;
		} else {
			DIS_status = DIS_status_next;
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
void ss_low(void)
{
  if(DIS_number == 0x00)
	SS_PORT_1 = LOW;
  if(DIS_number == 0x01)
	SS_PORT_2 = LOW;
  if(DIS_number == 0x02)
	SS_PORT_3 = LOW;
  if(DIS_number == 0x03)
	SS_PORT_4 = LOW;
  __delay_cycles(100);
}
/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void ss_high(void)
{
  if(DIS_number == 0x00)
	SS_PORT_1 = HIGH;
  if(DIS_number == 0x01)
	SS_PORT_2 = HIGH;
  if(DIS_number == 0x02)
	SS_PORT_3 = HIGH;
  if(DIS_number == 0x03)
	SS_PORT_4 = HIGH;
  __delay_cycles(100);
}
/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void read_callback(U8 *Rx_buffer, U8 length)
{
	ss_high();
	DIS_status = DIS_status_after_transfer;
}
/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
U8 Crc8(U8 *pcBlock, U8 len)
{
    U8 crc = 0xFF;
    U8 i;
 
    while (len--)
    {
        crc ^= *pcBlock++;
 
        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }
 
    return crc;
}