/* mathfunctions.h
 * By: Connor Taylor and Matt O'Conor
 * This module contains a bunch of math functions that are used in the 40image
 * program to tranform various elements. When a function wants a constant 
 * value, the user must pass in one of the constants declared below to get
 * the corresponding value
 */

#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include <stdint.h>
#include <math.h>
#include <mem.h>
#include <pnm.h>
#include <string.h>
#include <arith40.h>

extern const char *Y;
extern const char *Y1;
extern const char *Y2;
extern const char *Y3;
extern const char *Y4;
extern const char *PB;
extern const char *PR;
extern const char *A;
extern const char *B;
extern const char *C;
extern const char *D;
extern const int DENOM;

/* Functions that will convert from RGB to component and back */
float comp_trans(Pnm_rgb pixel, const char *VAL);
Pnm_rgb rgb_trans(float y, float pb, float pr);

/* Functions that will convert and reverse a discrete cosine transformation */
float DCT_inverse(float a, float b, float c, float d, const char *VAL);
float DCT_trans(float y1, float y2, float y3, float y4, const char *VAL);

/* Functions that will quantize and reverse a quantization */
int quantize(float value, const char *TYPE);
float rev_quantize(int index, const char *TYPE);

#endif
