/*
 * IDL_disp_power.c
 *
 *  Created on: Mar 31, 2019
 *      Author: Andy
 */

#include <std.h>
#include "hellocfg.h"
#include <stdint.h>
#include <mbx.h>
#include "IDL_disp.h"
#include "lcd.h"

extern MBX_Obj MBX_IDL_disp_fft_data_in;

#define WRITE_LEN (16+1)

int16_t bin_lut[17] =
{
		0x0000, // 0
		0x0080,	0x00C0,	0X00E0,	0X00F0, // 4 lsb of lower byte
		0X00F8,	0X00FC, 0X00FE,	0X00FF, // 4 msb of lower byte
		0X80FF, 0XC0FF, 0XE0FF, 0XF0FF, // 4 lsb of upper byte
		0XF8FF, 0XFCFF, 0XFEFF, 0xFFFF, // 4 msb of upper byte

};

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

	osd9616_send(0x00,0x2e);  // Deactivate Scrolling
}

void idl_disp_power(void)
{
	int16_t j, i, upper_byte, lower_byte, count = 0, col = 0;
	int16_t fft[96];
	int16_t cmd[WRITE_LEN];
	int16_t cmd_i = 1;

	if(MBX_pend(&MBX_IDL_disp_fft_data_in, &fft, 0) == TRUE)
	{
		for(i = 96; i > 0; i--)
		{
			count = 0;
			col = 0;
			// generate display value
			for(j = 0; col < fft[i] && j < 16; j ++ )
			{
				col |= 1 << j;
				count ++;
			}

			upper_byte = (bin_lut[count] & 0xFF00) >> 8;
			lower_byte = bin_lut[count] & 0x00FF;

			cmd[cmd_i] = upper_byte;
			cmd_i ++;
			cmd[cmd_i] = lower_byte;
			cmd_i ++;
			if(cmd_i == WRITE_LEN)
			{
				cmd_i = 0;
				cmd[cmd_i] = 0x40;
				cmd_i ++;
				EZDSP5502_I2C_write( OSD9616_I2C_ADDR, cmd, WRITE_LEN );
			}
		}
	}
}

