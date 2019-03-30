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

extern MBX_Obj MBX_TSK_calculate_pwr_data_in;

int16_t pwr(const int16_t *l_data, const int16_t *r_data, const int16_t len);

void tsk_calculate_pwr(Arg value_arg)
{
	AudioFrame_t frame1, frame2;
	int16_t power;
	while(1)
	{
		frame1.channel = RIGHT;
		frame2.channel = LEFT;
		// wait for first frame
		MBX_pend(&MBX_TSK_calculate_pwr_data_in, &frame1, ~0);

		// wait for the second frame
		MBX_pend(&MBX_TSK_calculate_pwr_data_in, &frame2, ~0);

		power = pwr(frame2.frame, frame2.frame, LEN_AUDIO_FRAME);
	}
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
