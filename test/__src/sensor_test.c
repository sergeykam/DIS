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
*ø
* First	written	on ______ by ______
*
* Module Description: realization of SPI_API functions for ATmega
*****************************************************************/
/***************************************************
*	Include Section
***************************************************/
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

#define SS_DDR_1  DDRB_Bit4
#define SS_PORT_1 PORTB_Bit4

#define SS_DDR_2  DDRA_Bit0
#define SS_PORT_2 PORTA_Bit0

#define SS_DDR_3  DDRA_Bit1
#define SS_PORT_3 PORTA_Bit1

#define SS_DDR_4  DDRA_Bit2
#define SS_PORT_4 PORTA_Bit2

#define OK					0x01
#define ERROR_CONTROL_BYTE	0x02


#define CONFIGURATION		0x01
#define DATA				0x03

#define ERROR_CNT			10
/***************************************************
*	Function Prototype Section
***************************************************/
void read_callback(U8 *Rx_buffer, U8 length);
void control_callback(U8 *Rx_buffer, U8 length);
U8 Crc8(U8 *pcBlock, U8 len);
void sensor_read (U8 command);
void sensor_cb (void);
void ss_low(void);
void ss_high(void);
void timer_cb(void);
void check_number(void);
/***************************************************
*	Static Variables Section
***************************************************/
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
    U8 control_byte;
    U8 status_byte;
    U8 data[4];
    U8 CRC;	
  }read_pos;
  
  U8 read_frame[7];
	
} read_packet;

union
{
	struct 
	{
	  U8 sort_gas;
	  U8 sort_sensor;
	  U8 unit;
	  U8 voltage;
	  U16 data;
	  U8 status;
	}sensor_data;
	
	U8 data_buffer[7];
	
}sensor_union[4];


static U8 DIS_command;
static U8 DIS_number = 0x00;
static U8 error_cnt = 0x00;
static U8 configuration_flag;
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
  SPI_init (SPI_MASTER + SPI_IDLE_SCK_LOW + SPI_SAMPLE_SETUP + SPI_MSB, 64);
  TIMER0_HW_API_init (timer_cb);

  sensor_read (CONFIGURATION);
  __enable_interrupt();
 
  while(1)
  {

  };
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
  if(configuration_flag == 0x01)
  	sensor_read (DATA);
}
/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void sensor_read (U8 command)
{
  DIS_command = command;
  
  write_packet.write_pos.start_byte = 0xAA;
  write_packet.write_pos.cmd_number = command;
  write_packet.write_pos.CRC = Crc8(write_packet.write_frame, 6);
  ss_low();
  __delay_cycles(10);
  SPI_transfer (write_packet.write_frame, read_packet.read_frame, 7, read_callback);
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
  __delay_cycles(100);
  SPI_transfer (0, read_packet.read_frame, 7, control_callback);
}
/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void control_callback(U8 *Rx_buffer, U8 length)
{
	if(read_packet.read_pos.control_byte == 0x00)
	{
		error_cnt++;
		if(error_cnt < ERROR_CNT)
		{
			SPI_transfer (0, read_packet.read_frame, 7, control_callback);
		}
		else
		{
		  	error_cnt = 0x00;
			ss_high();
			sensor_union[DIS_number].sensor_data.status = ERROR_CONTROL_BYTE;
			sensor_cb();
		}
	}
	else
	{
		if(read_packet.read_pos.control_byte == 0xA5)
		{
		  	error_cnt = 0x00;
		  	ss_high();
			sensor_union[DIS_number].sensor_data.status = OK;
			sensor_cb();
		}
		else
		{
			error_cnt++;
			if(error_cnt < ERROR_CNT)
			{
			  	ss_high();
				__delay_cycles(10);
				ss_low();
  				__delay_cycles(10);
  				SPI_transfer (write_packet.write_frame, read_packet.read_frame, 7, read_callback);
			}
			else
			{
			  	error_cnt = 0x00;
			  	ss_high();
				sensor_union[DIS_number].sensor_data.status = ERROR_CONTROL_BYTE;
				sensor_cb();
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
void sensor_cb (void)
{
  if(sensor_union[DIS_number].sensor_data.status == OK)
  {
	  if(DIS_command == CONFIGURATION)
	  {
		sensor_union[DIS_number].sensor_data.sort_sensor = read_packet.read_pos.data[0];
		sensor_union[DIS_number].sensor_data.sort_gas = read_packet.read_pos.data[1];
		sensor_union[DIS_number].sensor_data.unit = read_packet.read_pos.data[2];
		sensor_union[DIS_number].sensor_data.voltage = read_packet.read_pos.data[3];
	  }
	  if(DIS_command == DATA)
	  {
		sensor_union[DIS_number].sensor_data.data = ((U16)read_packet.read_pos.data[1] << 8)|read_packet.read_pos.data[0];
	  }
	  check_number();
	  
	  
  }
  if(sensor_union[DIS_number].sensor_data.status == ERROR_CONTROL_BYTE)
  {
	for(U8 i = 0x00; i < 6; i++)
	{
	  sensor_union[DIS_number].data_buffer[i] = 0x00;
	}
	check_number();
  }
}
/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void check_number(void)
{
	if(DIS_number < 3)
	  {
		DIS_number++;
		sensor_read(DIS_command);
	  }
	  else
	  {
		DIS_number = 0x00;
	  	configuration_flag = 0x01;
	  }
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