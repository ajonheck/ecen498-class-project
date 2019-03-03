/*
 * fir.h
 *
 *  Created on: Feb 1, 2019
 *      Author: Andy
 */

#ifndef FIR_H_
#define FIR_H_

#include "stdint.h"

void fir_filter(
	const int16_t* x,
	const int16_t len_x,
	onchip int16_t* h,
	const int16_t len_h,
	int16_t* y,
	int16_t* delay_line
);

#endif /* FIR_H_ */
