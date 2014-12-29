/* codeword.c
 * By: Connor Taylor and Matt O'Conor
 * This implementation holds functions that handle packing bits in the correct
 * format.  This way, if the format of the packing changes, all that needs
 * to be done is that the LSB constants need to be changed. The user never 
 * knows the LSBs of the items
 */
#include "codeword.h"

const int LSB_PB = 4;
const int LSB_PR = 0;
const int LSB_A = 26;
const int LSB_B = 20;
const int LSB_C = 14;
const int LSB_D = 8;
const int INDEX_WIDTH = 4;
const int SSI_WIDTH = 6;
const int USI_WIDTH = 6;

int get_width(const char *ITEM) 
{
        int width = 0;
        if ( strcmp(ITEM, PB) == 0 || strcmp(ITEM, PR) == 0){
                width = INDEX_WIDTH;
        } else if (strcmp(ITEM, B) == 0 || strcmp(ITEM, C) == 0 ||
                 strcmp(ITEM, D) == 0){
                width = SSI_WIDTH;
        } else {
                width = USI_WIDTH;
        }

        return width;
}

int get_lsb(const char *ITEM) 
{
        int lsb = -1;
        if (strcmp(ITEM, PB) == 0){
                lsb = LSB_PB;
        } else if (strcmp(ITEM, PR) == 0){
                lsb = LSB_PR;
        } else if (strcmp(ITEM, A) == 0) {
                lsb = LSB_A;
        } else if (strcmp(ITEM, B) == 0) {
                lsb = LSB_B;
        } else if (strcmp(ITEM, C) == 0) {
                lsb = LSB_C;
        } else if (strcmp(ITEM, D) == 0){
                lsb = LSB_D;
        }

        return lsb;
} 

uint64_t get_uvalue(uint64_t cword, const char *ITEM) 
{
        int lsb = get_lsb(ITEM);
        int width = get_width(ITEM);
        
        return Bitpack_getu(cword, width, lsb);
}

int64_t get_svalue(uint64_t cword, const char *ITEM)
{
        int lsb = get_lsb(ITEM);
        int width = get_width(ITEM);

        return Bitpack_gets(cword, width, lsb);
}

uint64_t set_uvalue(uint64_t cwd, uint64_t val, const char * ITEM)
{
        int lsb = get_lsb(ITEM);
        int width = get_width(ITEM);

        return Bitpack_newu(cwd, width, lsb, val);
}

uint64_t set_svalue(uint64_t cwd, int64_t val, const char *ITEM)
{
        int lsb = get_lsb(ITEM);
        int width = get_width(ITEM);

        return Bitpack_news(cwd, width, lsb, val);
}

/* Function called by 40image that handles packing the bits in the proper way
 */
uint64_t pack_bits(unsigned pb, unsigned pr, int *coeffs)
{
        uint64_t word = 0;
        word = set_uvalue(word, pb, PB);
        word = set_uvalue(word, pr, PR);
        word = set_uvalue(word, coeffs[0], A);
        word = set_svalue(word, coeffs[1], B);
        word = set_svalue(word, coeffs[2], C);
        word = set_svalue(word, coeffs[3], D);
        return word;
}
