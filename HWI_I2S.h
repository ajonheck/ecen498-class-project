/*
 * HWI_I2S.h
 *
 * Header file for HWI_I2S interrupts
 *
 *  Created on: Mar 3, 2019
 *      Author: Andy
 */

#ifndef HWI_I2S_H_
#define HWI_I2S_H_

#include <stdint.h>

#define LEN_AUDIO_FRAME 48

typedef enum
{
	LEFT = 1,
	RIGHT = 2
}Channel_t;

typedef struct
{
	Channel_t channel;
	int16_t frame[LEN_AUDIO_FRAME];
}AudioFrame_t;

void setup_HWI_I2S(void);

#endif /* HWI_I2S_H_ */
