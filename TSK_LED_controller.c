/*
 * TSK_LED_controller.c
 *
 *  Created on: Mar 6, 2019
 *      Author: Andy
 */

#include <std.h>
#include "hellocfg.h"
#include <mbx.h>
#include "ezdsp5502.h"
#include "TSK_LED_controller.h"

extern MBX_Obj MBX_TSK_LED_controller_input;

void TSK_LED_controller_setup(void)
{
    /* Setup I2C GPIO directions by line*/
    EZDSP5502_I2CGPIO_configLine(  LED_LPF, OUT );
    EZDSP5502_I2CGPIO_configLine(  LED_HPF, OUT );
    EZDSP5502_I2CGPIO_configLine(  LED_SINE, OUT );
    EZDSP5502_I2CGPIO_configLine(  LED_FILT, OUT );

    /* Turn off all LEDs by line*/
    EZDSP5502_I2CGPIO_writeLine(LED_LPF, OFF);
    EZDSP5502_I2CGPIO_writeLine(LED_HPF, OFF);
    EZDSP5502_I2CGPIO_writeLine(LED_SINE, OFF);
    EZDSP5502_I2CGPIO_writeLine(LED_FILT, OFF);
}

void tsk_LED_controller(void)
{
	LEDdata_t lpf, hpf, sine, filt, msg;
	LEDdata_t *ptr;

	lpf.led_id = LED_LPF;
	lpf.state = OFF;
	hpf.led_id = LED_HPF;
	hpf.state = OFF;
	sine.led_id = LED_SINE;
	sine.state = OFF;
	filt.led_id = LED_FILT;
	filt.state = OFF;

	while(1)
	{
		MBX_pend(&MBX_TSK_LED_controller_input, &msg, ~0);
		switch(msg.led_id)
		{
			case LED_LPF:
				ptr = &lpf;
				break;
			case LED_HPF:
				ptr = &hpf;
				break;
			case LED_SINE:
				ptr = &sine;
				break;
			case LED_FILT:
				ptr = &filt;
				break;
		}
		ptr->state = msg.state;
		EZDSP5502_I2CGPIO_writeLine(ptr->led_id, ptr->state);
	}
}
