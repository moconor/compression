/* readimage.c
 * By: Connor Taylor and Matt O'Conor
 * This implementation defines all the functions used to read in and write
 * both ppm images and compressed images
 */

#include "readimage.h"
#include <inttypes.h>

Pnm_ppm read_image(FILE *fp)
{
        A2Methods_T methods = uarray2_methods_plain;
        Pnm_ppm image = Pnm_ppmread(fp, methods);
        return image;
}

/* Reads in one word by grabbing 8 bits at a time. Reads words given in 
 * big-endian order
 */
uint64_t read_word(FILE *fp)
{
        int i;
        uint64_t word = 0;
        unsigned lsb = 0;
        uint64_t byte;
        for (i = 0; i < 4; i++){
                byte = getc(fp);
                lsb = 32 - ((i + 1) * 8);
                word = Bitpack_newu(word, 8, lsb, byte);
        }
        return word;
}

/* Prints out one word 8 bits at a time in big-endian order. */
void write_compressed(uint64_t word)
{
        int i;
        unsigned lsb = 0;
        uint64_t new_word = 0;
        for (i = 3; i >= 0; i--){
                uint64_t byte = Bitpack_getu(word, 8, i * 8);
                lsb = i * 8; 
                new_word = Bitpack_newu(new_word, 8, lsb, byte);
                putchar(byte);
        }
}

/* Checks if a dimension is divisible by 2. If yes, it returns the original
 * dimension, otherwise, it returns dimension - 1
 */
int trim_dim(int dim)
{
        if (dim % 2 == 0){
                return dim;
        }
        else{
                return dim - 1;
        }
}

