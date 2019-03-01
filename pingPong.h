/*
 * ping_pong.h
 *
 *  Created on: Mar 1, 2019
 *      Author: Andy
 */

#ifndef PINGPONG_H_
#define PINGPONG_H_

#include "stdint.h"

#define LEN_PING_PONG 200

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
	PingPong_t buffer;
} PingPongBuff_t;

PingPong_t write_ping_pong(PingPongBuff_t *buff, int16_t value);
PingPong_t read_ping_pong(PingPongBuff_t *buff, int16_t *read_value);
void setup_ping_pong(PingPongBuff_t * buff, int16_t *ping, int16_t *pong, PingPong_t starting_buff);

#endif /* PINGPONG_H_ */
