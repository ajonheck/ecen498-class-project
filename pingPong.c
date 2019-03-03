/*
 * pingPong.c
 *
 *  Created on: Mar 1, 2019
 *      Author: Andy
 */

#include "pingPong.h"

PingPong_t read_sample_ping_pong(PingPongBuff_t *buff, int16_t *read_value)
{
	int16_t *active_buff = get_active_buffer(buff);
	*read_value = active_buff[buff->index];
	buff->index ++;

	if(buff->index >= buff->len)
	{
		buff->index = 0;
		swap_active_buffer(buff);
	}
	return buff->buffer;
}

void setup_ping_pong(PingPongBuff_t * buff, int16_t len, int16_t *ping, int16_t *pong, PingPong_t starting_buff)
{
	buff->ping = ping;
	buff->pong = pong;
	buff->buffer = starting_buff;
	buff->index = 0;
	buff->len = len;
}

PingPong_t write_sample_ping_pong(PingPongBuff_t *buff, int16_t value)
{
	int16_t *active_buff = get_active_buffer(buff);
	active_buff[buff->index] = value;
	buff->index ++;

	if(buff->index >= buff->len)
	{
		buff->index = 0;
		swap_active_buffer(buff);
	}
	return buff->buffer;
}

int16_t* get_active_buffer(PingPongBuff_t *buff)
{
	return ( buff->buffer == PING) ? buff->ping : buff->pong;
}

PingPong_t swap_active_buffer(PingPongBuff_t *buff)
{
	buff->buffer = ( ( buff->buffer == PING ) ? PONG : PING );
	return buff->buffer;
}
