/* readimage.h
 * By: Connor Taylor and Matt O'Conor
 * This header file holds the interface for using the read image module.
 * This module can read in and print out images in either ppm or compressed 
 * binary format.  
 */

#ifndef READIMAGE_H
#define READIMAGE_H

#include <pnm.h>
#include <assert.h>
#include <except.h>
#include <mem.h>
#include <uarray2.h>
#include <a2methods.h>
#include <a2plain.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bitpack.h"

/* Pass the dimensions of the image through this to make sure they are even */
int trim_dim(int dim);

Pnm_ppm read_image(FILE *fp);

/* Reads in one codeword, NOT all of them*/
uint64_t read_word(FILE *fp);

/* Writes ONE codeword to stdout */
void write_compressed(uint64_t word);

#endif

 
