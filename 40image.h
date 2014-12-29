/* 40image.h
 * By: Connor Taylor
 * This header file declares the functions that are going to be used in 
 * 40 image.  The program will determine whether to compress or decompress
 * based on command line input.  For compression, compress40 will handle 
 * everything and for decompression, decompress 40 will handle everything
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "assert.h"
#include "uarray2.h"
#include "uarray.h"
#include "compress40.h"
#include "readimage.h"
#include "mathfunctions.h"
#include "arith40.h"
#include "codeword.h"
#include <inttypes.h>

/* Apply functions called in compress40/decompress40 that compress or 
 * decompress an image
 */
void compress_and_write(int i, int j, UArray2_T array, void *val, void *cl);
void decompress_image(int i, int j, UArray2_T array, void *val, void *cl);

/* Returns a UArray of Pnm_rgbs holding the 2x2 block of pixels to be 
 * compressed
 */
UArray_T get_pixels (int i, int j, UArray2_T array);

/* Returns a UArray of Pnm_rgbs holding the pixels obtained from reversing
 * compression
 */
UArray_T get_rgb(float pb, float pr, int *coeffs);

/* Used to get the average value of PB and PR */
float get_avg_val(UArray_T pix_array, const char *VAL);

/* Takes a UArray of pixels and gets the scaled cosine coefficients that 
 * should be packed into a codeword
 */
int *get_scaled_coeffs(UArray_T pixels);

