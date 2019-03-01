
#include <std.h>

#include <log.h>

#include "hellocfg.h"
#include "ezdsp5502.h"
#include "stdint.h"
#include "aic3204.h"
#include "ezdsp5502_mcbsp.h"
#include "csl_mcbsp.h"
#include "pingPong.h"

typedef enum
{
	LEFT,
	RIGHT
}Channel_t;

extern MCBSP_Handle aicMcbsp;

// Setup rx buffers
PingPongBuff_t l_rx_buff;
PingPongBuff_t r_rx_buff;

// Setup tx buffers
PingPongBuff_t l_tx_buff;
PingPongBuff_t r_tx_buff;

int16_t l_ping_buff[LEN_PING_PONG];
int16_t l_pong_buff[LEN_PING_PONG];

int16_t r_ping_buff[LEN_PING_PONG];
int16_t r_pong_buff[LEN_PING_PONG];

// Keep track of which channel to write to for tx and rx
Channel_t rx_channel = LEFT;
Channel_t tx_channel = LEFT;

void audioProcessingSetup()
{
	// Setup tx buffers
	setup_ping_pong(&l_rx_buff, l_ping_buff, l_pong_buff, PING);
	setup_ping_pong(&r_rx_buff, r_ping_buff, r_pong_buff, PING);

	// Setup rx buffers
	setup_ping_pong(&l_tx_buff, l_ping_buff, l_pong_buff, PONG);
	setup_ping_pong(&r_tx_buff, r_ping_buff, r_pong_buff, PONG);
}

void HWI_I2S_Rx(void)
{
	int16_t sample = MCBSP_read16(aicMcbsp);
	if (rx_channel == LEFT)
	{
		write_ping_pong(&l_rx_buff, sample);
		rx_channel = RIGHT;
	}
	else
	{
		write_ping_pong(&r_rx_buff, sample);
		rx_channel = LEFT;
	}
}

void HWI_I2S_Tx(void)
{
	int16_t sample;
	if (tx_channel == LEFT)
	{
		read_ping_pong(&l_tx_buff, &sample);
		tx_channel = RIGHT;
	}
	else
	{
		read_ping_pong(&r_tx_buff, &sample);
		tx_channel = LEFT;
	}
	MCBSP_write16(aicMcbsp,sample);
}



