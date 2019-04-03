/*
 * TSK_calculate_pwr.c
 *
 *  Created on: Mar 30, 2019
 *      Author: Andy
 */

#include <std.h>
#include "hellocfg.h"
#include <stdint.h>
#include "HWI_I2S.h"
#include <mbx.h>
#include "c55x.h"

#define NUM_SAMPLED_FRAMES 28

extern MBX_Obj MBX_TSK_calculate_pwr_data_in;
extern MBX_Obj MBX_IDL_disp_power_data_in;

int16_t pwr(const int16_t *data, const int16_t len);

void tsk_calculate_pwr(Arg value_arg)
{
	AudioFrame_t fin;

	int16_t power, i;
	int40_t power_accum;
	while(1)
	{
		power_accum = 0;
		for(i = 0; i < NUM_SAMPLED_FRAMES; i ++)
		{
			// calculate power for multiple frames
			MBX_pend(&MBX_TSK_calculate_pwr_data_in, &fin, ~0);
			power_accum += pwr(fin.frame, LEN_AUDIO_FRAME);
		}

		power = power_accum / NUM_SAMPLED_FRAMES;

		MBX_post(&MBX_IDL_disp_power_data_in, &power, 0);
	}
}

int16_t pwr(const int16_t *data, const int16_t len)
{
	int40_t p = 0;
	int16_t value, i;
	for(i = 0; i < len; i ++)
	{
		value = data[i];
		p += _lsmpy(value, value);
	}
	p = _lshrs(_rnd(p / len),16);
	return (int16_t) p;
}
