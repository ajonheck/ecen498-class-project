/*
 *  nco.h
 *  Created on: Jan 16, 2019
 *      Author: Andy Heck (ajonheck@gmail.com)
 */

#include "stdint.h"

#define LUT_BUFFER 512  // this should be a power of 2, and no LUT can exceed it

/**
 * Struct to store metadata and LUT for an NCO
 */
typedef struct
{
    uint16_t n;                 // power of 2 for length of LUT
    uint16_t m;                 // length of phase accumulator
    uint16_t decimal_len;       // fractional bits in PA
    uint16_t atten_pow;         // attenuation constant (power of 2)
    uint32_t pa;                // phase accumulator
    uint32_t delta;             // phase delta for each  step
    int16_t lut[LUT_BUFFER];    // static buffer to copy LUT into.
} NCO_t;

/**
 * Configures an NCO struct
 * 
 * nco:         Pointer to desired NCO_t struct to be configured
 * m:           Length of the desired phase accumulator (must be under 32)
 * n:           Length of the LUT as power of 2 (i.e. LUT is 512, n = 9)
 * lut:         Pointer to the lookup table to iterate over
 * atten_pow:   Power of 2 to divide the LUT value by (i.e. 1 will halve LUT values)
 * ftone:       Desired output frequency
 * fs:          Sampling rate
 *
 * returns: pointer to nco, which has been newly configured
 * TODO: instead return an enum type for error handling
 */
NCO_t* configureNCO(NCO_t* nco, uint16_t m, uint16_t n, int16_t* lut, int16_t atten_pow, double ftone, double fs);

/**
 * Iterates through the LUT of the given NCO_t pointer and returns the appropriate value.
 * Also increments the phase accumulator.
 * 
 * NCO: pointer to the NCO to be incremented
 *
 * returns: the signed integer value for the waveform.
 */
int16_t getNextValue(NCO_t *NCO);

