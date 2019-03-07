#include <std.h>

#include <log.h>

#include "hellocfg.h"
#include "fir.h"
#include "HWI_I2S.h"
#include <mbx.h>
#include "IDL_IO.h"
#include <string.h>

extern MBX_Obj MBX_TSK_filter_data_in;
extern MBX_Obj MBX_TSK_filter_data_swap_h;
extern MBX_Obj MBX_TSK_output_mux_data_in;
extern MBX_Obj MBX_IDL_control_LED_input;

#define LEN_H 256
#define LEN_DL (LEN_AUDIO_FRAME - 1 + LEN_H)

typedef enum
{
	LPF = 0,
	HPF = 1
}FilterCoeffs_t;

static int16_t h_low_short[] =
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

static int16_t h_low[] =
	{
	        -4,     -3,     -3,     -2,     -2,     -1,     -1,      0,      0,      1,      2,      2,      3,      4,      5,      6,
	         7,      8,      9,     10,     11,     12,     13,     14,     15,     16,     17,     18,     19,     20,     21,     22,
	        22,     22,     23,     23,     22,     22,     21,     20,     19,     17,     16,     13,     11,      8,      5,      2,
	        -2,     -6,    -10,    -15,    -20,    -25,    -30,    -35,    -41,    -47,    -52,    -58,    -64,    -70,    -75,    -81,
	       -86,    -91,    -95,    -99,   -103,   -106,   -108,   -110,   -111,   -112,   -111,   -110,   -107,   -104,   -100,    -94,
	       -88,    -80,    -71,    -61,    -49,    -37,    -23,     -8,      8,     26,     44,     64,     85,    107,    129,    153,
	       178,    203,    229,    256,    283,    310,    338,    366,    395,    423,    451,    478,    506,    533,    559,    585,
	       610,    633,    656,    678,    698,    717,    735,    751,    765,    778,    789,    799,    806,    812,    816,    818,
	       818,    816,    812,    806,    799,    789,    778,    765,    751,    735,    717,    698,    678,    656,    633,    610,
	       585,    559,    533,    506,    478,    451,    423,    395,    366,    338,    310,    283,    256,    229,    203,    178,
	       153,    129,    107,     85,     64,     44,     26,      8,     -8,    -23,    -37,    -49,    -61,    -71,    -80,    -88,
	       -94,   -100,   -104,   -107,   -110,   -111,   -112,   -111,   -110,   -108,   -106,   -103,    -99,    -95,    -91,    -86,
	       -81,    -75,    -70,    -64,    -58,    -52,    -47,    -41,    -35,    -30,    -25,    -20,    -15,    -10,     -6,     -2,
	         2,      5,      8,     11,     13,     16,     17,     19,     20,     21,     22,     22,     23,     23,     22,     22,
	        22,     21,     20,     19,     18,     17,     16,     15,     14,     13,     12,     11,     10,      9,      8,      7,
	         6,      5,      4,      3,      2,      2,      1,      0,      0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
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
   	hpf_msg.led_id = LED_HPF;
   	lpf_msg.state = LED_ON;
   	hpf_msg.state = LED_OFF;
   	MBX_post(&MBX_IDL_control_LED_input, &lpf_msg, 0);
   	MBX_post(&MBX_IDL_control_LED_input, &hpf_msg, 0);

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
    		   		lpf_msg.state = LED_OFF;
    		   		hpf_msg.state = LED_ON;
    		   	}
    		   	else
    		   	{
    		   		filter_coeffs = LPF;
    		   		h = h_low;
    		   		lpf_msg.state = LED_ON;
    		   		hpf_msg.state = LED_OFF;
    		   	}
    		   	MBX_post(&MBX_IDL_control_LED_input, &lpf_msg, 0);
    		   	MBX_post(&MBX_IDL_control_LED_input, &hpf_msg, 0);

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
