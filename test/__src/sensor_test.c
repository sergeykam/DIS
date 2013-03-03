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
*
* First	written	on ______ by ______
*
* Module Description: realization of SPI_API functions for ATmega
*****************************************************************/
/***************************************************
*	Include Section
***************************************************/
#include <device.h>
#include <SPI_API.h>
/***************************************************
*	Defines Section
***************************************************/
enum ss_level
{
  LOW = 0,
  HIGH,
};

#define SS_DDR  DDRB_Bit4
#define SS_PORT PORTB_Bit4

/***************************************************
*	Function Prototype Section
***************************************************/
void read_callback(U8 *Rx_buffer, U8 length);
void analysis_callback(U8 *Rx_buffer, U8 length);
void control_callback(U8 *Rx_buffer, U8 length);
U8 Crc8(U8 *pcBlock, U8 len);
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
  }pos;
  
  U8 frame[7];
	
} packet;
static U8 read_buf[6];
static U8 flag;
/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void main (void)
{
  SS_DDR = HIGH;
  SPI_init (SPI_MASTER + SPI_IDLE_SCK_LOW + SPI_SAMPLE_SETUP + SPI_MSB, 64);
  packet.pos.start_byte = 0xAA;
  packet.pos.cmd_number = 0x00;
  packet.pos.CRC = Crc8(packet.frame, 6);
  
  __enable_interrupt();
  SS_PORT = LOW;
  SPI_transfer (packet.frame, read_buf, 7, read_callback);
  while(1)
  {
    if(flag)
    {
      flag = 0x00;
      SS_PORT = LOW;
      SPI_transfer (0, read_buf, 6, analysis_callback);
    }
  };
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
  SS_PORT = LOW;
  SPI_transfer (0, read_buf, 1, control_callback);
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
  SS_PORT = HIGH;
  if(read_buf[0] == 0x00)
  {
    SPI_transfer (0, read_buf, 1, control_callback);
  }
  else
  {
    if(read_buf[0] == 0xA5)
    {
      flag = 0x01;
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
void analysis_callback(U8 *Rx_buffer, U8 length)
{
  SS_PORT = HIGH;
  __delay_cycles(30);
U8 read_CRC = Crc8(read_buf, 5);
  if(read_CRC == read_buf[5])
  {
    
  }
  else
  {
    
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