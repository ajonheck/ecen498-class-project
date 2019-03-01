/*
 * pingPong.c
 *
 *  Created on: Mar 1, 2019
 *      Author: Andy
 */

#include "pingPong.h"

PingPong_t write_ping_pong(PingPongBuff_t *buff, int16_t value)
{
	int16_t *active_buff = ( (buff->buffer == PING) ? buff->ping : buff->pong );
	active_buff[buff->index] = value;
	buff->index ++;

	if(buff->index >= LEN_PING_PONG)
	{
		buff->index = 0;
		buff->buffer = ( (buff->buffer == PING) ? PONG : PING );
	}
	return buff->buffer;
}


void setup_ping_pong(PingPongBuff_t * buff, int16_t *ping, int16_t *pong, PingPong_t starting_buff)
{
	buff->ping = ping;
	buff->pong = pong;
	buff->buffer = starting_buff;
	buff->index = 0;
}

PingPong_t read_ping_pong(PingPongBuff_t *buff, int16_t *read_value)
{
	int16_t *active_buff = ( (buff->buffer == PING) ? buff->ping : buff->pong );
	*read_value = active_buff[buff->index];
	buff->index ++;

	if(buff->index >= LEN_PING_PONG)
	{
		buff->index = 0;
		buff->buffer = ( (buff->buffer == PING) ? PONG : PING );
	}
	return buff->buffer;
}
