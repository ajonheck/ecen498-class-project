#include <std.h>

#include <log.h>

#include "hellocfg.h"
#include "fir.h"
#include "HWI_I2S.h"
#include <mbx.h>

extern MBX_Obj MBX_TSK_filter_data_in;
extern MBX_Obj MBX_TSK_output_mux_data_in;

#define LEN_H 64
#define LEN_DL (LEN_AUDIO_FRAME - 1 + LEN_H)

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
	// Prolouge
	int16_t *x, *h, *dl;
	int16_t dlr[LEN_DL];
	int16_t dll[LEN_DL];
	AudioFrame_t frame_in;
	AudioFrame_t frame_out;

	h = h_low;

	while(1)
	{
		MBX_pend(&MBX_TSK_filter_data_in, &frame_in, ~0);
		x = frame_in.frame;
		dl = ( (frame_in.channel == LEFT) ? dll : dlr );
		fir_filter(x, LEN_AUDIO_FRAME, h, LEN_H, frame_out.frame, dl);
		frame_out.channel = frame_in.channel;
		MBX_post(&MBX_TSK_output_mux_data_in, &frame_out, ~0);
	}
}
