/***************************************************
*	Include Section
***************************************************/
#include <DIS_API.h>
/***************************************************
*	Defines Section
***************************************************/
enum DIS_state
{
	IDLE = 0,
	WAIT,
	BUSY,
	SEND_CMD,
	WAIT_DATA_READY,
	WAITING_DATA,
	GETTING_DATA,
};

enum SS_STATE
{
	LOW = 0,
	HIGH = 1
};
/***************************************************
*	Function Prototype Section
***************************************************/
void read_callback(U8 *Rx_buffer, U8 length);
U8 Crc8(U8 *pcBlock, U8 len);
void ss_low(void);
void ss_high(void);
/***************************************************
*	Variables Section
***************************************************/
union
{
	float x;
	U8 buffer[4];
}float_x;

union read_packet_union
{
	struct
	{
		U8 data[4];
		U8 CRC;	
	}read_pos;
  
	U8 read_frame[5];
	
} read_packet;

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

// STATUSES
static U8 status;
static U8 status_next;
static U8 status_after_transfer;

// timer
static U16 time;

// DIS
static U8 DIS_number;
static U8 control_byte;
static U8 attempt_number = DIS_ATTEMPTS; 
static U8 dummy_byte;
static U8 dummy_arr[7];

// user's callback
static void (*data_ready_cb)(U8*);

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void DIS_init (void)
{	
	SS_DDR_1 = HIGH;
	SS_DDR_2 = HIGH;
	SS_DDR_3 = HIGH;
	SS_DDR_4 = HIGH;

	ss_high();
	
	SPI_init (SPI_MASTER + SPI_IDLE_SCK_LOW + SPI_SAMPLE_SETUP + SPI_MSB, DIS_SPI_PRS);	
	return;
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
void DIS_timer_cout(void)
{
	if(status == WAIT){
		if(time){
			time--;
		} else {
			status = status_next;
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
void DIS_while_cout(void)
{
	switch (status){
		case (BUSY):
		case (IDLE):
		case (WAIT):
			break;
		case (WAIT_DATA_READY):
			if(0xA5 == dummy_arr[0] || 0xA5 == dummy_arr[1]){
				time = DIS_TIME_BTW_ATTEMPTS;
				status_after_transfer = WAIT;
				status_next = WAITING_DATA;
				status = BUSY;
				SPI_transfer (0, &control_byte, 1, read_callback);
			} else {
				if(attempt_number){
					attempt_number--;
					time = DIS_TIME_BTW_ATTEMPTS;
					status_after_transfer = WAIT_DATA_READY;
					status = BUSY;
					SPI_transfer (0, dummy_arr, 1, read_callback);
				} else {
					time = 0;
					attempt_number = DIS_ATTEMPTS;
					status = IDLE;
					data_ready_cb(0);
				}
			}	
			break;
		case (WAITING_DATA):
			switch(control_byte)
			{
				case (0xA5): // still waiting
					time = DIS_TIME_BTW_ATTEMPTS;
					status_after_transfer = WAIT;
					status_next = WAITING_DATA;
					status = BUSY;
					SPI_transfer (&dummy_byte, &control_byte, 1, read_callback);
					break;
				default:
					time = 0;
					status_after_transfer = WAIT;
					status_next = GETTING_DATA;
					status = BUSY;
					SPI_transfer (0,read_packet.read_frame, 5, read_callback);
					break;
			}
			break;
		case (GETTING_DATA):
			ss_high();
			data_ready_cb(read_packet.read_pos.data);
			status = IDLE;
			break;
		default:
			ss_high();
			status = IDLE;
			status_next = IDLE;
			status_after_transfer = IDLE;
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
U8 DIS_get_data(U8 DIS_num, void (*callback)(U8 *data))
{
	if(status != IDLE){
		return 0;
	}

	DIS_number = DIS_num;
	data_ready_cb = callback;

	status_after_transfer = WAIT_DATA_READY;
	status = BUSY;

	write_packet.write_pos.start_byte = 0xAA;
	write_packet.write_pos.cmd_number = 0x01;
	write_packet.write_pos.CRC = Crc8(write_packet.write_frame, 6);
	for(U8 i = 0; i < 4; i++)
	{
		write_packet.write_pos.data[i] = 0x00;
	}

	ss_low();
	if(SPI_transfer (write_packet.write_frame, dummy_arr, 7, read_callback)){
		return 0;
	} else {
		return 1;
	}
}

/**************************************************
* Function name	: 
* Created by	: 
* Date created	: 
* Description	:
* Notes		: 
***************************************************/
U8 DIS_get_configuration(U8 DIS_num, void (*callback)(U8 *configuration))
{
	if(status != IDLE){
		return 0;
	}

	DIS_number = DIS_num;
	data_ready_cb = callback;

	status_after_transfer = WAIT_DATA_READY;
	status = BUSY;

	write_packet.write_pos.start_byte = 0xAA;
	write_packet.write_pos.cmd_number = 0x03;
	write_packet.write_pos.CRC = Crc8(write_packet.write_frame, 6);
	for(U8 i = 0; i < 4; i++)
	{
		write_packet.write_pos.data[i] = 0x00;
	}

	ss_low();
	if(SPI_transfer (write_packet.write_frame, dummy_arr, 7, read_callback)){
		return 0;
	} else {
		return 1;
	}
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
	status = status_after_transfer;
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
	switch (DIS_number)
	{
		case 0x00:
			SS_PORT_1 = LOW;
			SS_PORT_2 = HIGH;
			SS_PORT_3 = HIGH;
			SS_PORT_4 = HIGH;
			break;
		case 0x01:
			SS_PORT_1 = HIGH;
			SS_PORT_2 = LOW;
			SS_PORT_3 = HIGH;
			SS_PORT_4 = HIGH;
			break;
		case 0x02:
			SS_PORT_1 = HIGH;
			SS_PORT_2 = HIGH;
			SS_PORT_3 = LOW;
			SS_PORT_4 = HIGH;
			break;
		case 0x03:
			SS_PORT_1 = HIGH;
			SS_PORT_2 = HIGH;
			SS_PORT_3 = HIGH;
			SS_PORT_4 = LOW;
			break;
		default:
			ss_high();
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
void ss_high(void)
{
	SS_PORT_1 = HIGH;
	SS_PORT_2 = HIGH;
	SS_PORT_3 = HIGH;
	SS_PORT_4 = HIGH;
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
