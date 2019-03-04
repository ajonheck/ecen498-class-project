/*
 * nco.c
 *
 *  Created on: Jan 16, 2019
 *      Author: Andy Heck (ajonheck@gmail.com)
 */

// C libraries
#include <string.h>
#include <math.h>

// Project libraries
#include "nco.h"

NCO_t* configureNCO(NCO_t* nco, uint16_t m, uint16_t n, int16_t* lut, int16_t atten_pow, double ftone, double fs)
{
    nco->m = m;
    nco->n = n;
    
    // calculate the number of fractional bits
    nco->decimal_len = m - n;
    
    // determine the length of the given lut and copy it into the struct
    memcpy(nco->lut, lut, pow(2,n) * sizeof(uint16_t));

    nco->atten_pow = atten_pow;

    // calculate the phase delta
    nco->delta = (pow(2,(double)m)*ftone)/fs;
    return nco;
}

int16_t getNextValue(NCO_t* nco)
{
    // incremenet the PA
    nco->pa = nco->pa + nco->delta;
    
    // only use the first n bits to index into LUT
    return nco->lut[nco->pa >> nco->decimal_len] >> nco->atten_pow;
}
