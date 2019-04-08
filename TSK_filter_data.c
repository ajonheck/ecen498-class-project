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
extern MBX_Obj MBX_IDL_control_LED_input;
extern MBX_Obj MBX_HWI_I2S_TX_data_in;

#define LEN_H 256
#define LEN_DL (LEN_AUDIO_FRAME - 1 + LEN_H)

typedef enum
{
	LPF = 0,
	HPF = 1,
	NOF = 2
}FilterCoeffs_t;

static int16_t h_high[] =
{
         0,      0,      0,      0,      0,      0,      0,      0,      0,      0,     -1,     -1,     -1,     -1,     -1,     -1,
        -1,      0,      0,      1,      2,      3,      3,      4,      3,      3,      2,      0,     -2,     -4,     -6,     -7,
        -8,     -8,     -7,     -5,     -2,      1,      5,      9,     13,     15,     16,     15,     12,      7,      1,     -6,
       -13,    -20,    -25,    -28,    -28,    -25,    -18,     -9,      3,     16,     28,     39,     46,     49,     46,     38,
        24,      6,    -14,    -35,    -54,    -69,    -78,    -78,    -69,    -52,    -27,      4,     37,     70,     97,    116,
       123,    118,     98,     65,     21,    -29,    -80,   -127,   -165,   -187,   -190,   -171,   -132,    -73,     -1,     79,
       158,    226,    276,    299,    290,    248,    172,     69,    -54,   -184,   -309,   -414,   -484,   -507,   -475,   -383,
      -233,    -33,    205,    459,    708,    922,   1074,   1132,   1067,    848,    438,   -215,  -1216,  -2840,  -6124, -20578,
     20578,   6124,   2840,   1216,    215,   -438,   -848,  -1067,  -1132,  -1074,   -922,   -708,   -459,   -205,     33,    233,
       383,    475,    507,    484,    414,    309,    184,     54,    -69,   -172,   -248,   -290,   -299,   -276,   -226,   -158,
       -79,      1,     73,    132,    171,    190,    187,    165,    127,     80,     29,    -21,    -65,    -98,   -118,   -123,
      -116,    -97,    -70,    -37,     -4,     27,     52,     69,     78,     78,     69,     54,     35,     14,     -6,    -24,
       -38,    -46,    -49,    -46,    -39,    -28,    -16,     -3,      9,     18,     25,     28,     28,     25,     20,     13,
         6,     -1,     -7,    -12,    -15,    -16,    -15,    -13,     -9,     -5,     -1,      2,      5,      7,      8,      8,
         7,      6,      4,      2,      0,     -2,     -3,     -3,     -4,     -3,     -3,     -2,     -1,      0,      0,      1,
         1,      1,      1,      1,      1,      1,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
};
int16_t h_low[] =
{
         6,      6,      6,      7,      7,      7,      7,      7,      8,      8,      8,      8,      9,      9,      9,      9,
         9,      9,      9,      9,      9,      9,      8,      8,      7,      7,      6,      5,      4,      3,      2,      1,
        -1,     -2,     -4,     -6,     -8,    -10,    -13,    -15,    -18,    -21,    -24,    -27,    -30,    -33,    -36,    -39,
       -42,    -45,    -49,    -52,    -55,    -58,    -61,    -63,    -66,    -68,    -70,    -72,    -73,    -75,    -75,    -76,
       -76,    -75,    -74,    -72,    -70,    -68,    -64,    -61,    -56,    -51,    -45,    -38,    -31,    -23,    -14,     -5,
         5,     16,     28,     40,     53,     66,     81,     96,    111,    127,    144,    161,    179,    197,    215,    234,
       253,    272,    292,    312,    331,    351,    371,    390,    410,    429,    448,    466,    484,    502,    519,    536,
       552,    567,    581,    595,    608,    620,    630,    640,    649,    657,    664,    670,    674,    678,    680,    681,
       681,    680,    678,    674,    670,    664,    657,    649,    640,    630,    620,    608,    595,    581,    567,    552,
       536,    519,    502,    484,    466,    448,    429,    410,    390,    371,    351,    331,    312,    292,    272,    253,
       234,    215,    197,    179,    161,    144,    127,    111,     96,     81,     66,     53,     40,     28,     16,      5,
        -5,    -14,    -23,    -31,    -38,    -45,    -51,    -56,    -61,    -64,    -68,    -70,    -72,    -74,    -75,    -76,
       -76,    -75,    -75,    -73,    -72,    -70,    -68,    -66,    -63,    -61,    -58,    -55,    -52,    -49,    -45,    -42,
       -39,    -36,    -33,    -30,    -27,    -24,    -21,    -18,    -15,    -13,    -10,     -8,     -6,     -4,     -2,     -1,
         1,      2,      3,      4,      5,      6,      7,      7,      8,      8,      9,      9,      9,      9,      9,      9,
         9,      9,      9,      9,      8,      8,      8,      8,      7,      7,      7,      7,      7,      6,      6,      6,
};

int16_t dlr[LEN_DL];
int16_t dll[LEN_DL];
Void tsk_filter_data(Arg value_arg)
{
	// Thread variables
	int16_t *h, *dl;
	int16_t i, fft_i = 0;
	int16_t fft[256];
	int16_t update_filter;
	AudioFrame_t frame_in;
	AudioFrame_t frame_out;
	FilterCoeffs_t filter_coeffs;
   	LEDdata_t lpf_msg, hpf_msg;

	// initialization
	filter_coeffs = LPF;
	h = h_low;

	// setup LEDs
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

		// perform filter update and LED change if needed
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
    		   	else if(filter_coeffs == HPF)
    		   	{
    		   		filter_coeffs = NOF;
    		   		lpf_msg.state = LED_OFF;
    		   		hpf_msg.state = LED_OFF;
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

		// perform filtering if needed
		if(filter_coeffs != NOF)
		{
			fir_filter(frame_in.frame, LEN_AUDIO_FRAME, h, LEN_H, frame_out.frame, dl);
		}
		else
		{
			memcpy(frame_out.frame, frame_in.frame, sizeof(int16_t)*LEN_AUDIO_FRAME);
		}

		// set channel and post frame to HW tx
		frame_out.channel = frame_in.channel;
		MBX_post(&MBX_HWI_I2S_TX_data_in, &frame_out, 0);

		// buffer left channel frames for FFT display
		if(frame_out.channel == LEFT)
		{
			for(i = 0; i < LEN_AUDIO_FRAME && fft_i < 256;)
			{
				fft[fft_i] = frame_out.frame[i];
				i++;
				fft_i++;
			}
			if(fft_i == 256)
			{
				fft_i = 0;
				MBX_post(&MBX_TSK_fft_data_in, &fft, 0);
			}
		}
	}
}
