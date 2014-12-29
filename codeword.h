/* codeword.c
 * By: Connor Taylor and Matt O'Conor
 * This file declares the interface for the Codeword_T datatype.  This is
 * an abstraction of a codeword that is used by the 40image program. 
 */

/* codeword.h
 * By: Connor Taylor and Matt O'Conor
 * This interface declares the functions that are used to pack bits in the
 * specified codeword format. The pack_bits functions should be called from
 * the compressor to pack all the fields in one word
 */

#ifndef CODEWORD_H
#define CODEWORD_H
#include <mem.h>
#include <assert.h>
#include <stdint.h>
#include "mathfunctions.h"
#include "bitpack.h"
#include <inttypes.h>

/* Constants to use with codeword
 * const char *PB = "PB";
 * const char *PR = "PR";
 * const char *A  = "A";
 * const char *B  = "B";
 * const char *C  = "C";
 * const char *D  = "D";
 */

int get_width(const char *ITEM);
int get_lsb(const char *ITEM); 

uint64_t get_uvalue(uint64_t cword, const char *ITEM); 
int64_t get_svalue(uint64_t cwd, const char *ITEM);

uint64_t set_uvalue(uint64_t cwd, uint64_t val, const char *ITEM);
uint64_t set_svalue(uint64_t cwd, int64_t val, const char *ITEM);

/* Packs ALL the bits into one uint64_t in the proper locations */
uint64_t pack_bits(unsigned pb, unsigned pr, int *coeffs);

#undef T
#endif
