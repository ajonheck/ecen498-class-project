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
#include "c55x.h"
#include "IDL_IO.h"

extern MBX_Obj MBX_IDL_disp_fft_data_in;
extern MBX_Obj MBX_IDL_disp_fft_mode;
extern MBX_Obj MBX_IDL_control_LED_input;

#define WRITE_LEN (48+1)

typedef enum
{
	FFT = 0,
	POWER = 1
}DispMode_t;

DispMode_t disp_mode;

int16_t bin_lut[17] =
{
		0x0000, // 0
		0x0080,	0x00C0,	0X00E0,	0X00F0, // 4 lsb of lower byte
		0X00F8,	0X00FC, 0X00FE,	0X00FF, // 4 msb of lower byte
		0X80FF, 0XC0FF, 0XE0FF, 0XF0FF, // 4 lsb of upper byte
		0XF8FF, 0XFCFF, 0XFEFF, 0xFFFF, // 4 msb of upper byte

};

int16_t fft_bin_edge[16] =
{
		10, 14, 20,	28,
		40, 58, 83, 118,
		169, 241, 344, 492,
		702, 1004, 1433, 2048
};

int16_t pwr_bin_edge[16] =
{
		9, 11, 14,	18,
		22, 27, 34, 43,
		54, 67, 84, 105,
		131, 164, 204, 256
};

void IDL_disp_fft_init(void)
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

	disp_mode = FFT;
}

void idl_disp_fft(void)
{
	int16_t press, j, i, upper_byte, lower_byte;
	LEDdata_t power_led, fft_led;
	int16_t fft[96];
	int16_t cmd[WRITE_LEN];
	int16_t cmd_i = 1;
	int16_t *bin;
	int32_t x;

	// handle any updates to the displayed data
	if(MBX_pend(&MBX_IDL_disp_fft_mode, &press, 0) == TRUE)
	{
		disp_mode = (disp_mode == FFT ? POWER : FFT);
		if(disp_mode == POWER)
		{
			power_led.led_id = LED_PWR;
			power_led.state = LED_ON;
			fft_led.led_id = LED_FFT;
			fft_led.state = LED_OFF;
		}
		else
		{
			power_led.led_id = LED_PWR;
			power_led.state = LED_OFF;
			fft_led.led_id = LED_FFT;
			fft_led.state = LED_ON;
		}
		MBX_post(&MBX_IDL_control_LED_input, &power_led, 0);
		MBX_post(&MBX_IDL_control_LED_input, &fft_led, 0);
	}

	// handle display of new data
	if(MBX_pend(&MBX_IDL_disp_fft_data_in, &fft, 0) == TRUE)
	{
		// Determine what bins to use
		if(disp_mode == FFT)
		{
			bin = fft_bin_edge;
		}
		else
		{
			bin = pwr_bin_edge;
		}
		for(i = 96; i > 0; i--)
		{
			// Optional squaring of FFT
			if(disp_mode == POWER)
			{
				x = _lsmpy(fft[i], fft[i]);
				x = _lshrs(_rnd(x), 16);
			} else {
				x = fft[i];
			}

			// generate display value
			for(j = 0; bin[j] < x && j < 16; j ++ );

			upper_byte = (bin_lut[j] & 0xFF00) >> 8;
			lower_byte = bin_lut[j] & 0x00FF;

			// Generate multi-data command
			cmd[cmd_i] = upper_byte;
			cmd_i ++;
			cmd[cmd_i] = lower_byte;
			cmd_i ++;

			// If command is full write to display
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

