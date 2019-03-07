/*
 * IDL_IO.h
 *
 *  Created on: Mar 6, 2019
 *      Author: Andy
 */

#ifndef IDL_IO_H_
#define IDL_IO_H_

#include "ezdsp5502_i2cgpio.h"

typedef enum
{
	LED_LPF = (LED0),
	LED_HPF = (LED1),
	LED_SINE = (LED2),
	LED_FILT = (LED3)
}LEDid_t;

typedef enum
{
	LED_ON = LOW,
	LED_OFF = HIGH
}LEDstate_t;

typedef struct
{
	LEDid_t led_id;
	LEDstate_t state;
}LEDdata_t;


void IDL_IO_setup();

#endif /* IDL_IO_H_ */
