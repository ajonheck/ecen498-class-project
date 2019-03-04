
#include <std.h>

#include <log.h>

#include "hellocfg.h"
#include "ezdsp5502.h"
#include "stdint.h"
#include "aic3204.h"
#include "ezdsp5502_mcbsp.h"
#include "csl_mcbsp.h"
#include "pingPong.h"
#include <stdint.h>
#include <Swi.h>
#include "fir.h"
#include <string.h>
#include <mbx.h>

extern MBX_Obj MBX_TSK_filter_data;
extern MBX_Obj MBX_HWI_TX;

#define LEN_PING_PONG 48
#define LEN_H 64
#define LEN_DL (LEN_PING_PONG - 1 + LEN_H)
typedef enum
{
	LEFT = 1,
	RIGHT = 2
}Channel_t;

typedef struct
{
	Channel_t channel;
	int16_t frame[48];
}AudioFrame_t;

extern MCBSP_Handle aicMcbsp;

// buff for filter write
PingPongBuff_t l_out_pingpong;
PingPongBuff_t r_out_pingpong;

// buff for tx read
PingPongBuff_t l_tx_pingpong;
PingPongBuff_t r_tx_pingpong;

// Setup rx ping-pong buff
int16_t l_tx_ping[LEN_PING_PONG];
int16_t l_tx_pong[LEN_PING_PONG];

int16_t r_tx_ping[LEN_PING_PONG];
int16_t r_tx_pong[LEN_PING_PONG];

// Keep track of which channel to write to for tx and rx
Channel_t rx_channel = LEFT;
Channel_t tx_channel = LEFT;

AudioFrame_t l_in_frame;
int16_t l_in_frame_index = 0;

AudioFrame_t r_in_frame;
int16_t r_in_frame_index = 0;

int16_t h_low[] =
{
       151,    179,    207,    236,    266,    296,    327,    358,    389,    421,    452,    483,    513,    543,    573,    601,
       629,    656,    682,    706,    729,    750,    770,    788,    805,    819,    832,    842,    851,    857,    862,    864,
       864,    862,    857,    851,    842,    832,    819,    805,    788,    770,    750,    729,    706,    682,    656,    629,
       601,    573,    543,    513,    483,    452,    421,    389,    358,    327,    296,    266,    236,    207,    179,    151,
};

int16_t h_high[] =
{
       133,    121,    106,     90,     73,     53,     31,      7,    -20,    -49,    -81,   -116,   -154,   -197,   -243,   -294,
      -351,   -414,   -485,   -565,   -656,   -762,   -887,  -1037,  -1221,  -1457,  -1771,  -2215,  -2899,  -4114,  -6919, -20849,
     20849,   6919,   4114,   2899,   2215,   1771,   1457,   1221,   1037,    887,    762,    656,    565,    485,    414,    351,
       294,    243,    197,    154,    116,     81,     49,     20,     -7,    -31,    -53,    -73,    -90,   -106,   -121,   -133,
};

void audio_setup()
{
	l_in_frame.channel = LEFT;
	r_in_frame.channel = RIGHT;

	// Setupt filter write buffers; must be opposite of data buffs
	setup_ping_pong(&l_out_pingpong, LEN_PING_PONG, l_tx_ping, l_tx_pong, PING);
	setup_ping_pong(&r_out_pingpong, LEN_PING_PONG, r_tx_ping, r_tx_pong, PING);

	// Setup tx buffers; must be opposite of out buffs
	setup_ping_pong(&l_tx_pingpong, LEN_PING_PONG, l_tx_ping, l_tx_pong, PONG);
	setup_ping_pong(&r_tx_pingpong, LEN_PING_PONG, r_tx_ping, r_tx_pong, PONG);

}

void HWI_I2S_Rx(void)
{
	AudioFrame_t *frame;
	int16_t *index;
	int16_t sample = MCBSP_read16(aicMcbsp);

	if (rx_channel == LEFT)
	{
		index = &l_in_frame_index;
		frame = &l_in_frame;
		rx_channel = RIGHT;
	} else {
		index = &r_in_frame_index;
		frame = &r_in_frame;
		rx_channel = LEFT;
	}

	frame->frame[*index] = sample;
	*index = *index + 1;

	if(*index >= 48){
		*index = 0;
		MBX_post(&MBX_TSK_filter_data, frame, 0);
	}
}

void HWI_I2S_Tx(void)
{
	int16_t sample;
	AudioFrame_t frame_in;

	if( MBX_pend(&MBX_HWI_TX, &frame_in, 0) == TRUE)
	{
		if(frame_in.channel == LEFT)
		{
			memcpy(get_inactive_buffer(&l_tx_pingpong), frame_in.frame, sizeof(int16_t) * 48);
		}
		else
		{
			memcpy(get_inactive_buffer(&r_tx_pingpong), frame_in.frame, sizeof(int16_t) * 48);
		}
	}

	if (tx_channel == LEFT)
	{
		read_sample_ping_pong(&l_tx_pingpong, &sample);
		tx_channel = RIGHT;
	}
	else
	{
		read_sample_ping_pong(&r_tx_pingpong, &sample);
		tx_channel = LEFT;
	}
	MCBSP_write16(aicMcbsp,sample);
}

Void filterData(Arg value_arg)
{
	// Prolouge
	int16_t *x, *y, *h, *dl;
	int16_t dlr[LEN_DL];
	int16_t dll[LEN_DL];
	AudioFrame_t frame_in;
	AudioFrame_t frame_out;

	h = h_low;

	while(1)
	{
		MBX_pend(&MBX_TSK_filter_data, &frame_in, ~0);
		x = frame_in.frame;

		if(frame_in.channel == LEFT)
		{
			y = get_active_buffer(&l_out_pingpong);
			dl = dll;
		}
		else
		{
			y = get_active_buffer(&r_out_pingpong);
			dl = dlr;
		}

		fir_filter(x, LEN_PING_PONG, h, LEN_H, frame_out.frame, dl);

		frame_out.channel = frame_in.channel;

		MBX_post(&MBX_HWI_TX, &frame_out, ~0);
	}
}
