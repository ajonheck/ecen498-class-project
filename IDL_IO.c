/*
 * IDL_poll_switches.c
 *
 *  Created on: Mar 6, 2019
 *      Author: Andy
 */
#include <std.h>
#include "ezdsp5502.h"
#include "ezdsp5502_i2cgpio.h"
#include "csl_gpio.h"
#include "stdio.h"
#include "stdint.h"
#include <mbx.h>
#include "hellocfg.h"
#include "IDL_IO.h"

#define SW_FILT (SW1)
#define SW_DISP (SW0)

static int16_t filter_switch;
static int16_t sine_switch;

extern MBX_Obj MBX_TSK_filter_data_swap_h;
extern MBX_Obj MBX_IDL_control_LED_input;
extern MBX_Obj MBX_IDL_disp_fft_mode;

void IDL_IO_setup(){

    EZDSP5502_I2CGPIO_configLine( SW_FILT, IN );
    EZDSP5502_I2CGPIO_configLine( SW_DISP, IN );
    filter_switch = LOW;
    sine_switch = LOW;

    EZDSP5502_I2CGPIO_configLine( LED_LPF, OUT );
    EZDSP5502_I2CGPIO_configLine( LED_HPF, OUT );
    EZDSP5502_I2CGPIO_configLine( LED_PWR, OUT );
    EZDSP5502_I2CGPIO_configLine( LED_FFT, OUT );

	EZDSP5502_I2CGPIO_writeLine(LED_FFT, LED_ON);
}

void idl_control_LED()
{
	LEDdata_t led_msg;
	if(MBX_pend(&MBX_IDL_control_LED_input, &led_msg, 0) == TRUE)
	{
		EZDSP5502_I2CGPIO_writeLine(led_msg.led_id, led_msg.state);
	}
}

void idl_poll_switches()
{
	int16_t reading;

	// get filter switch reading and determine if a press event occured
    reading = EZDSP5502_I2CGPIO_readLine(SW_FILT);
    if(reading != filter_switch && filter_switch == HIGH)
    {
    	int16_t true = 1;
    	MBX_post(&MBX_TSK_filter_data_swap_h, &true, 0);
    }
    filter_switch = reading;


	// get disp switch reading and determine if a press event occured
    reading = EZDSP5502_I2CGPIO_readLine(SW_DISP);
    if(reading != sine_switch && sine_switch == HIGH)
    {
    	int16_t true = 1;
    	MBX_post(&MBX_IDL_disp_fft_mode, &true, 0);
    }
    sine_switch = reading;
}
