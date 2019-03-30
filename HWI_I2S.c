/*
 * HWI_I2S.c
 *
 *  Created on: Mar 3, 2019
 *      Author: Andy
 */

#include <std.h>

#include <string.h>
#include <mbx.h>

#include "aic3204.h"
#include "ezdsp5502_mcbsp.h"
#include "csl_mcbsp.h"
#include "ezdsp5502.h"
#include "HWI_I2S.h"
#include "hellocfg.h"

#include "pingPong.h"

// external variables
extern MCBSP_Handle aicMcbsp;
extern MBX_Obj MBX_TSK_calculate_pwr_data_in;
extern MBX_Obj MBX_TSK_filter_data_in;
extern MBX_Obj MBX_HWI_I2S_TX_data_in;

// tx and rx channel state
static Channel_t rx_channel = LEFT;
static Channel_t tx_channel = LEFT;

// Static frames for input
static AudioFrame_t l_in_frame;
static int16_t l_in_frame_index = 0;

static AudioFrame_t r_in_frame;
static int16_t r_in_frame_index = 0;

// buffers for txing
static PingPongBuff_t l_tx_pingpong;
static PingPongBuff_t r_tx_pingpong;

static int16_t l_tx_ping[LEN_AUDIO_FRAME];
static int16_t l_tx_pong[LEN_AUDIO_FRAME];

static int16_t r_tx_ping[LEN_AUDIO_FRAME];
static int16_t r_tx_pong[LEN_AUDIO_FRAME];

void HWI_I2S_setup(void)
{
	l_in_frame.channel = LEFT;
	r_in_frame.channel = RIGHT;

	setup_ping_pong(&l_tx_pingpong, LEN_AUDIO_FRAME, l_tx_ping, l_tx_pong, PING);
	setup_ping_pong(&r_tx_pingpong, LEN_AUDIO_FRAME, r_tx_ping, r_tx_pong, PING);
}

void HWI_I2S_RX(void)
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

	if(*index >= LEN_AUDIO_FRAME){
		*index = 0;
		MBX_post(&MBX_TSK_filter_data_in, frame, 0);
		MBX_post(&MBX_TSK_calculate_pwr_data_in, frame, 0);
	}
}

void HWI_I2S_TX(void)
{
	int16_t sample;
	AudioFrame_t frame_in;

	// If any frames are waiting to be read, put them in inactive buffer
	if( MBX_pend(&MBX_HWI_I2S_TX_data_in, &frame_in, 0) == TRUE)
	{
		if(frame_in.channel == LEFT)
		{
			memcpy(get_inactive_buffer(&l_tx_pingpong), frame_in.frame, sizeof(int16_t) * LEN_AUDIO_FRAME);
		}
		else
		{
			memcpy(get_inactive_buffer(&r_tx_pingpong), frame_in.frame, sizeof(int16_t) * LEN_AUDIO_FRAME);
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
