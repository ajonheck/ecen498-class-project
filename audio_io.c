
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

extern  SWI_Obj  SWI_filter_thread;

#define LEN_PING_PONG 48

typedef enum
{
	LEFT,
	RIGHT
}Channel_t;

extern MCBSP_Handle aicMcbsp;

// buff for rx write
PingPongBuff_t l_rx_pingpong;
PingPongBuff_t r_rx_pingpong;

// buff for filter read
PingPongBuff_t l_data_pingpong;
PingPongBuff_t r_data_pingpong;

// buff for filter write
PingPongBuff_t l_out_pingpong;
PingPongBuff_t r_out_pingpong;

// buff for tx read
PingPongBuff_t l_tx_pingpong;
PingPongBuff_t r_tx_pingpong;

// Setup tx ping-pong buff
int16_t l_tx_ping[LEN_PING_PONG];
int16_t l_tx_pong[LEN_PING_PONG];

int16_t r_tx_ping[LEN_PING_PONG];
int16_t r_tx_pong[LEN_PING_PONG];

// Setup rx ping-pong buff
int16_t l_rx_ping[LEN_PING_PONG];
int16_t l_rx_pong[LEN_PING_PONG];

int16_t r_rx_ping[LEN_PING_PONG];
int16_t r_rx_pong[LEN_PING_PONG];

// Keep track of which channel to write to for tx and rx
Channel_t rx_channel = LEFT;
Channel_t tx_channel = LEFT;

void audio_setup()
{
	// Setup rx buffers
	setup_ping_pong(&l_rx_pingpong, LEN_PING_PONG, l_rx_ping, l_rx_pong, PING);
	setup_ping_pong(&r_rx_pingpong, LEN_PING_PONG, r_rx_ping, r_rx_pong, PING);

	// Setup filter read buffers; must be oppposite of rx buffs
	setup_ping_pong(&l_data_pingpong, LEN_PING_PONG, l_rx_ping, l_rx_pong, PONG);
	setup_ping_pong(&r_data_pingpong, LEN_PING_PONG, r_rx_ping, r_rx_pong, PONG);

	// Setupt filter write buffers; must be opposite of data buffs
	setup_ping_pong(&l_out_pingpong, LEN_PING_PONG, l_tx_ping, l_tx_pong, PING);
	setup_ping_pong(&r_out_pingpong, LEN_PING_PONG, r_tx_ping, r_tx_pong, PING);

	// Setup tx buffers; must be opposite of out buffs
	setup_ping_pong(&l_tx_pingpong, LEN_PING_PONG, l_tx_ping, l_tx_pong, PONG);
	setup_ping_pong(&r_tx_pingpong, LEN_PING_PONG, r_tx_ping, r_tx_pong, PONG);
}

void HWI_I2S_Rx(void)
{
	int16_t sample = MCBSP_read16(aicMcbsp);
	if (rx_channel == LEFT)
	{
		write_sample_ping_pong(&l_rx_pingpong, sample);
		rx_channel = RIGHT;
	}
	else
	{
		// Check if the buffer switches
		PingPong_t previous_buffer = r_rx_pingpong.buffer;
		if( previous_buffer != write_sample_ping_pong(&r_rx_pingpong, sample) )
		{
			// Fire off SWI since the last right buff is now written
			SWI_post(&SWI_filter_thread);
		}
		rx_channel = LEFT;
	}
}

void HWI_I2S_Tx(void)
{
	int16_t sample;
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

void SWI_filter_data(void)
{
	int16_t output[LEN_PING_PONG];
	int16_t frame[LEN_PING_PONG];

	read_frame_ping_pong(&l_data_pingpong, frame);
	write_frame_ping_pong(&l_out_pingpong, frame);

	read_frame_ping_pong(&r_data_pingpong, frame);
	write_frame_ping_pong(&r_out_pingpong, frame);
}

