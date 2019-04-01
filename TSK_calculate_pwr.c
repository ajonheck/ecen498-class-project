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

extern MBX_Obj MBX_TSK_calculate_pwr_data_in;
extern MBX_Obj MBX_IDL_disp_power_data_in;

int16_t pwr(const int16_t *l_data, const int16_t *r_data, const int16_t len);

void tsk_calculate_pwr(Arg value_arg)
{
	AudioFrame_t f1, f2;
	int16_t power;
	while(1)
	{
		// wait for first frame
		MBX_pend(&MBX_TSK_calculate_pwr_data_in, &f1, ~0);

		// wait for the second frame
		MBX_pend(&MBX_TSK_calculate_pwr_data_in, &f2, ~0);

		power = pwr(f1.frame, f2.frame, LEN_AUDIO_FRAME);
		MBX_post(&MBX_IDL_disp_power_data_in, &power, 0);
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
