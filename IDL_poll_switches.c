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
#include "TSK_output_mux.h"
#include <mbx.h>
#include "hellocfg.h"

#define SW_FILT (SW1)
#define SW_SINE (SW0)

static int16_t filter_switch;
static int16_t sine_switch;

extern MBX_Obj MBX_TSK_filter_data_swap_h;
extern MBX_Obj MBX_TSK_output_mux_source;

void IDL_poll_siwtches_setup(){
    EZDSP5502_I2CGPIO_configLine(  SW_FILT, IN );
    EZDSP5502_I2CGPIO_configLine(  SW_SINE, IN );
    filter_switch = EZDSP5502_I2CGPIO_readLine(SW_FILT);
    sine_switch = EZDSP5502_I2CGPIO_readLine(SW_SINE);
}

void IDL_poll_switches()
{
	int16_t reading;

    reading = EZDSP5502_I2CGPIO_readLine(SW_FILT);
    if(reading != filter_switch && filter_switch == HIGH)
    {
    	int16_t true = 1;
    	MBX_post(&MBX_TSK_filter_data_swap_h, &true, ~0);
    }
    filter_switch = reading;

    reading = EZDSP5502_I2CGPIO_readLine(SW_SINE);
    if(reading != sine_switch && sine_switch == HIGH)
    {
    	MuxSource_t mux_source = SINE;
    	MBX_post(&MBX_TSK_output_mux_source, &mux_source, ~0);
    }
    sine_switch = reading;
}
