#include <std.h>

#include <log.h>

#include "hellocfg.h"
#include "fir.h"
#include "HWI_I2S.h"
#include <mbx.h>
#include "TSK_LED_controller.h"

extern MBX_Obj MBX_TSK_filter_data_in;
extern MBX_Obj MBX_TSK_filter_data_swap_h;
extern MBX_Obj MBX_TSK_output_mux_data_in;
extern MBX_Obj MBX_TSK_LED_controller_input;

#define LEN_H 64
#define LEN_DL (LEN_AUDIO_FRAME - 1 + LEN_H)

typedef enum
{
	LPF = 0,
	HPF = 1
}FilterCoeffs_t;

static int16_t h_low[] =
{
       151,    179,    207,    236,    266,    296,    327,    358,    389,    421,    452,    483,    513,    543,    573,    601,
       629,    656,    682,    706,    729,    750,    770,    788,    805,    819,    832,    842,    851,    857,    862,    864,
       864,    862,    857,    851,    842,    832,    819,    805,    788,    770,    750,    729,    706,    682,    656,    629,
       601,    573,    543,    513,    483,    452,    421,    389,    358,    327,    296,    266,    236,    207,    179,    151,
};

static int16_t h_high[] =
{
       133,    121,    106,     90,     73,     53,     31,      7,    -20,    -49,    -81,   -116,   -154,   -197,   -243,   -294,
      -351,   -414,   -485,   -565,   -656,   -762,   -887,  -1037,  -1221,  -1457,  -1771,  -2215,  -2899,  -4114,  -6919, -20849,
     20849,   6919,   4114,   2899,   2215,   1771,   1457,   1221,   1037,    887,    762,    656,    565,    485,    414,    351,
       294,    243,    197,    154,    116,     81,     49,     20,     -7,    -31,    -53,    -73,    -90,   -106,   -121,   -133,
};

Void tsk_filter_data(Arg value_arg)
{
	// Thread variables
	int16_t *h, *dl;
	int16_t dlr[LEN_DL];
	int16_t dll[LEN_DL];
	int16_t update_filter;
	AudioFrame_t frame_in;
	AudioFrame_t frame_out;
	FilterCoeffs_t filter_coeffs;
   	LEDdata_t lpf_msg, hpf_msg;

	// initialization
	filter_coeffs = LPF;
	h = h_low;

	lpf_msg.led_id = LED_LPF;
	lpf_msg.state = ON;
	hpf_msg.led_id = LED_HPF;
	hpf_msg.state = OFF;

	memset(dll, 0, sizeof(LEN_DL));
	memset(dlr, 0, sizeof(LEN_DL));

	while(1)
	{
		// wait for frame
		MBX_pend(&MBX_TSK_filter_data_in, &frame_in, ~0);

		// perform filter update if necessary
    	if (MBX_pend(&MBX_TSK_filter_data_swap_h, &update_filter, 0) == TRUE)
    	{
    		if(update_filter == 1)
    		{
    		   	if(filter_coeffs == LPF)
    		   	{
    		   		filter_coeffs = HPF;
    		   		h = h_high;
    		   		lpf_msg.state = OFF;
    		   		hpf_msg.state = ON;
    		   	}
    		   	else
    		   	{
    		   		filter_coeffs = LPF;
    		   		h = h_low;
    		   		lpf_msg.state = ON;
    		   		hpf_msg.state = OFF;
    		   	}
    		   	MBX_post(&MBX_TSK_LED_controller_input, &lpf_msg, 0);
    		   	MBX_post(&MBX_TSK_LED_controller_input, &hpf_msg, 0);
    		}
    	}

		// determine delay line
		dl = ( (frame_in.channel == LEFT) ? dll : dlr );

		// perform filtering
		fir_filter(frame_in.frame, LEN_AUDIO_FRAME, h, LEN_H, frame_out.frame, dl);

		// set channel and post frame to mux
		frame_out.channel = frame_in.channel;
		MBX_post(&MBX_TSK_output_mux_data_in, &frame_out, ~0);
	}
}
