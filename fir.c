
#include "fir.h"
#include "c55x.h"
#include <stdlib.h>

void fir_filter(
	const int16_t* x,
	const int16_t len_x,
	const int16_t* h,
	const int16_t len_h,
	int16_t* y,
	int16_t* delay_line
) {
	// Loop variables
	int16_t i,j;
	int40_t accum;

	// Determine where to start putting x into the delay line
	int16_t dl_x_start = len_h - 1;

	// Determine the lenght of the delay line
	int16_t len_dl = len_x + dl_x_start;

	// Copy x into the delay line
	memcpy(&delay_line[dl_x_start], x, len_x*sizeof(int16_t));

	// Perform filtering
	for(i = 0; i < len_x; i ++)
	{
		accum = 0;
		for(j = 0; j < len_h; j ++)
		{
			accum = _smac(accum, h[j], delay_line[i+j]);
		}
		// Round the accumulator and right shift 16
		accum = _lshrs(_rnd(accum),16);
		y[i] = accum;
	}

	// Copy residual back to start of delay line
	memcpy(delay_line, &delay_line[len_dl - len_h + 1], (dl_x_start)*sizeof(int16_t));
}
