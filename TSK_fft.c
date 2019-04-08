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
#include "string.h"
#include "fft/FFT.h"

#define NUM_SAMPLED_FRAMES 28

extern MBX_Obj MBX_TSK_fft_data_in;
extern MBX_Obj MBX_IDL_disp_fft_data_in;

extern ExtU_FFT_T FFT_U;
extern ExtY_FFT_T FFT_Y;

void tsk_fft(Arg value_arg)
{
	FFT_initialize();
	while(1)
	{
		MBX_pend(&MBX_TSK_fft_data_in, FFT_U.In1, ~0);
		FFT_step();
		MBX_post(&MBX_IDL_disp_fft_data_in, FFT_Y.Out1, ~0);
	}
}
