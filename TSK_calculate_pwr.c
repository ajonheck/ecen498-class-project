/*
 * TSK_calculate_pwr.c
 *
 *  Created on: Mar 30, 2019
 *      Author: Andy
 */

#include <std.h>
#include "hellocfg.h"
#include "HWI_I2S.h"
#include <mbx.h>
#include "c55x.h"
#include "lcd.h"
#include "string.h"

extern MBX_Obj MBX_TSK_calculate_pwr_data_in;
extern MBX_Obj MBX_IDL_disp_power_data_in;

#define NUM_DISPLAYED_SAMPLES 48

int16_t pwr_buff[NUM_DISPLAYED_SAMPLES];
int16_t pwr_ptr;

int16_t bin_lut[17] =
{
		0x0000, // 0
		0x0080,	0x00C0,	0X00E0,	0X00F0, // 4 lsb of lower byte
		0X00F8,	0X00FC, 0X00FE,	0X00FF, // 4 msb of lower byte
		0X80FF, 0XC0FF, 0XE0FF, 0XF0FF, // 4 lsb of upper byte
		0XF8FF, 0XFCFF, 0XFEFF, 0xFFFF, // 4 msb of upper byte

};

int16_t pwr(const int16_t *l_data, const int16_t *r_data, const int16_t len);

void tsk_calculate_pwr(Arg value_arg)
{
	AudioFrame_t f1, f2, fin;
	int16_t power, i;
	while(1)
	{
		// wait for first frame
		MBX_pend(&MBX_TSK_calculate_pwr_data_in, &f1, ~0);

		// wait for the second frame
		MBX_pend(&MBX_TSK_calculate_pwr_data_in, &f2, ~0);

		MBX_pend(&MBX_TSK_calculate_pwr_data_in, &fin, ~0);

		for(i = 0; i < LEN_AUDIO_FRAME; i ++ )
		{
			int16_t t = _sadd((f1[i] >> 1), (fin[i] >> 1));
			f1[i] = t;
		}


		MBX_pend(&MBX_TSK_calculate_pwr_data_in, &fin, ~0);

		for(i = 0; i < LEN_AUDIO_FRAME; i ++ )
		{
			f2[i] = _sadd(f2[i] >> 1, fin[i] >> 1);
		}

		power = pwr(f1.frame, f2.frame, LEN_AUDIO_FRAME);
		MBX_post(&MBX_IDL_disp_power_data_in, &power, 0);
	}
}

void idl_disp_power_init(void)
{
	osd9616_init( );   // Initialize  Display

	osd9616_send(0x00,0x2e);  // Deactivate Scrolling

	osd9616_send(0x00,0XC0);  // Set normal display mode

	// set vertical fill
	osd9616_send(0x00, 0x20); // change addressing mode command
	osd9616_send(0x00, 0x01); // set addressing mode to vertical mode

	// Set column boundaries
	osd9616_send(0x00, 0x21); // set column command
	osd9616_send(0x00, 0x00); // set low column to 0
	osd9616_send(0x00, 0x5F); // set high column to 95

	// Set page boundaries
	osd9616_send(0x00, 0x22); // set page command
	osd9616_send(0x00, 0x00); // set low page to 0
	osd9616_send(0x00, 0x01); // set high page to 1
	pwr_ptr = 0;
}

void idl_disp_power(void)
{
	int16_t pwr, count = 0, col = 0;
	int16_t i;
	MBX_pend(&MBX_IDL_disp_power_data_in, &pwr, ~0);
	for(i = 0; col < pwr && i < 16; i ++ )
	{
		col |= 1 << i;
		count ++;
	}
	pwr_buff[NUM_DISPLAYED_SAMPLES-1] = bin_lut[count];
	for(i = 0; i < NUM_DISPLAYED_SAMPLES; i ++)
	{
		int16_t upper_byte = (pwr_buff[i] & 0xFF00) >> 8;
		int16_t lower_byte = pwr_buff[i] & 0x00FF;
		// write two columns
		osd9616_send(0x40,upper_byte);
		osd9616_send(0x40,lower_byte);
		osd9616_send(0x40,upper_byte);
		osd9616_send(0x40,lower_byte);
	}

	// shift data
	memcpy(pwr_buff, &pwr_buff[1], (NUM_DISPLAYED_SAMPLES - 1)*sizeof(int16_t));
}

int16_t pwr(const int16_t *l_data, const int16_t *r_data, const int16_t len)
{
	int40_t p = 0;
	int16_t value, i;
	for(i = 0; i < len; i ++)
	{
		value = _sadd(l_data[i] >> 1, r_data[i] >> 1);
		p += _lsmpy(value, value);
	}
	p = _lshrs(_rnd(p / len),16);
	return (int16_t) p;
}
