/*
 * TSK_LED_controller.h
 *
 *  Created on: Mar 6, 2019
 *      Author: Andy
 */

#ifndef TSK_LED_CONTROLLER_H_
#define TSK_LED_CONTROLLER_H_

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
	ON = LOW,
	OFF = HIGH
}LEDstate_t;

typedef struct
{
	LEDid_t led_id;
	LEDstate_t state;
}LEDdata_t;

void TSK_LED_controller_setup(void);

#endif /* TSK_LED_CONTROLLER_H_ */
