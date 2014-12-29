/* bitpack.c
 * By: Connor Taylor and Matt O'Conor
 * This file implements all of the functions declared in bitpack.h.  These 
 * functions can test if a number fits in a certain number of bits, get a 
 * specified number of bits at a specified location in a word, and add a field
 * to a word
 */ 

#include "bitpack.h"
#include "assert.h"
#include <stdio.h>
#include <inttypes.h>
const unsigned NUM_BITS = 64;

/* Makes sure that the unsigned val fits in width number of bits. */
bool Bitpack_fitsu(uint64_t val, unsigned width)
{
        assert(width <= NUM_BITS);
        uint64_t tester = ~0;
        tester = tester >> (NUM_BITS - width);
        if (val > tester){
                return false;
        }
        else{
                return true;
        }
}

/* Same as fitsu, but tests signed values */
bool Bitpack_fitss(int64_t val, unsigned width)
{
        assert(width <= NUM_BITS);
        uint64_t temp1 = ~0;
        temp1 = temp1 >> (NUM_BITS - width + 1);
        int64_t test1 = (int64_t)temp1;

        int64_t test2 = -1;
        test2 = test2 << (width - 1);

        if (val < test2 || val > test1){
                return false;
        }
        else{
                return true;
        }
}

/* Grabs width number of bits from words with a least significant bit of lsb. */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= NUM_BITS);
        assert(lsb < NUM_BITS);
        assert(width + lsb <= NUM_BITS);
        if (width == NUM_BITS){
                return word;
        }
        uint64_t mask = ~0;
        mask = mask >> (NUM_BITS - width);
        mask = mask << lsb;

        mask = mask & word;
        mask = mask >> lsb;
        return mask;
}

/* Same as getu, but returns the value represented as a signed value */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= NUM_BITS);
        assert(lsb < NUM_BITS);
        assert(width + lsb <= NUM_BITS);
        if (width == NUM_BITS){
                return (int64_t)word;
        }
        uint64_t extracted = Bitpack_getu(word, width, lsb);
        uint64_t test_size = 1 << (width - 1);
        if (extracted >= test_size){
                extracted = extracted << (NUM_BITS - width);
                int64_t result = (int64_t)extracted;
                result = result >> (NUM_BITS - width);
                return result;
        }
        else{
                return (int64_t)extracted;
        }
}

/* Adds val to the word passed in at lsb = lsb. Returns a new word with the 
 * new value stored in it */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t val)
{
        assert(Bitpack_fitsu(val, width));
        assert(lsb < NUM_BITS);

        if (width == NUM_BITS){
                return val;
        }
        uint64_t mask = 1 << width;
        mask -= 1;
        mask = mask << lsb;
        mask = ~mask;

        word = word & mask;
        word = (word | (val << lsb));
        return word;
}

/* Same as newu, but stores a signed value in the word */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t val)
{
        assert(Bitpack_fitss(val, width));
        assert(lsb < NUM_BITS);
        if (width == NUM_BITS){
                return val;
        }
        uint64_t mask = 1 << width;
        mask -= 1;
        mask = mask << lsb;
        mask = ~mask;
        word = word & mask;

        mask = (uint64_t)val;
        mask = mask << (NUM_BITS - width);
        mask = mask >> (NUM_BITS - width - lsb);
        word = word | mask;
        return word;
}
