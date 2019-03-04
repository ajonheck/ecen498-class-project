/*
 * TSK_mux_output.c
 *
 *  Created on: Mar 3, 2019
 *      Author: Andy
 */

#include <std.h>

#include "hellocfg.h"
#include "HWI_I2S.h"
#include <mbx.h>

extern MBX_Obj MBX_HWI_I2S_TX_data_in;
extern MBX_Obj MBX_TSK_output_mux_data_in;

void tsk_output_mux(void)
{
	// Prolouge
	AudioFrame_t frame;

	while(1)
	{
		MBX_pend(&MBX_TSK_output_mux_data_in, &frame, ~0);

		if(0){
			MBX_post(&MBX_HWI_I2S_TX_data_in, &frame, ~0);
		} else {
			MBX_post(&MBX_HWI_I2S_TX_data_in, &frame, ~0);
		}
	}
}
