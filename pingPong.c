/*
 * pingPong.c
 *
 *  Created on: Mar 1, 2019
 *      Author: Andy
 */

#include "pingPong.h"

PingPong_t read_sample_ping_pong(PingPongBuff_t *buff, int16_t *read_value)
{
	int16_t *active_buff = ( (buff->buffer == PING) ? buff->ping : buff->pong );
	*read_value = active_buff[buff->index];
	buff->index ++;

	if(buff->index >= buff->len)
	{
		buff->index = 0;
		buff->buffer = ( (buff->buffer == PING) ? PONG : PING );
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
	int16_t *active_buff = ( (buff->buffer == PING) ? buff->ping : buff->pong );
	active_buff[buff->index] = value;
	buff->index ++;

	if(buff->index >= buff->len)
	{
		buff->index = 0;
		buff->buffer = ( (buff->buffer == PING) ? PONG : PING );
	}
	return buff->buffer;
}

int16_t* get_and_swap_buffer_ping_pong(PingPongBuff_t *buff)
{
	PingPong_t current_buff = PING;
	int16_t *active_buff = ( current_buff == PING) ? buff->ping : buff->pong ;
	buff->buffer = ( ( current_buff == PING ) ? PONG : PING );
	return active_buff;
}

PingPong_t read_frame_ping_pong(PingPongBuff_t *buff, int16_t *dest)
{
	PingPong_t current_buff = buff->buffer;
	int16_t *data = ( ( current_buff == PING ) ? buff->ping : buff->pong );
	memcpy(dest, data, sizeof(int16_t) * buff->len);
	buff->buffer = ( ( current_buff == PING ) ? PONG : PING );
	buff->index = 0;
	return buff->buffer;
}
PingPong_t write_frame_ping_pong(PingPongBuff_t *buff, int16_t *data)
{
	PingPong_t current_buff = buff->buffer;
	int16_t *dest = ( ( current_buff == PING ) ? buff->ping : buff->pong );
	memcpy(dest, data, sizeof(int16_t) * buff->len);
	buff->buffer = ( ( current_buff == PING ) ? PONG : PING );
	buff->index = 0;
	return buff->buffer;
}
