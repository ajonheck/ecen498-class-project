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

void tsk_calculate_pwr(Arg value_arg)
{
	AudioFrame_t frame1, frame2;
	int40_t accum;
	int16_t i, ave;

	while(1)
	{
		// wait for first frame
		MBX_pend(&MBX_TSK_calculate_pwr_data_in, &frame1, ~0);

		// wait for the second frame
		MBX_pend(&MBX_TSK_calculate_pwr_data_in, &frame2, ~0);

		// verify the two frames are different channels
		if(frame1.channel != frame2.channel)
		{
			// calculate power of average of the two channels
			accum = 0;
			for(i = 0; i < LEN_AUDIO_FRAME; i ++)
			{
				// find average by dividing each sample by 2 then adding
				ave = _sadd(_shrs(frame1.frame[i], 1), _shrs(frame2.frame[i], 1));
				accum = _smac(accum, ave, ave);
			}
			accum = _lshrs(_rnd(accum),16);
			accum /= LEN_AUDIO_FRAME;
		}
	}

}
