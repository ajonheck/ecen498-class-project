/*
 * ping_pong.h
 *
 *  Created on: Mar 1, 2019
 *      Author: Andy
 */

#ifndef PINGPONG_H_
#define PINGPONG_H_

#include "stdint.h"

typedef enum
{
	PING,
	PONG
} PingPong_t;

typedef struct
{
	int16_t *ping;
	int16_t *pong;
	int16_t index;
	int16_t len;
	PingPong_t buffer;
} PingPongBuff_t;

PingPong_t read_sample_ping_pong(PingPongBuff_t *buff, int16_t *read_value);
void setup_ping_pong(PingPongBuff_t * buff, int16_t len, int16_t *ping, int16_t *pong, PingPong_t starting_buff);
PingPong_t write_sample_ping_pong(PingPongBuff_t *buff, int16_t value);
PingPong_t read_frame_ping_pong(PingPongBuff_t *buff, int16_t *dest);
PingPong_t write_frame_ping_pong(PingPongBuff_t *buff, int16_t *data);
int16_t* get_and_swap_buffer_ping_pong(PingPongBuff_t *buff);
#endif /* PINGPONG_H_ */
