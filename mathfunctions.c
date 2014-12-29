/* mathfunctions.c
 * By: Connor Taylor and Matt O'Conor
 * This implementation defines all of the math functions needed to compress
 * and decompress an image.
 */

#include "mathfunctions.h"
#define A_SCALE 63
#define BCD_SCALE 103

const char *Y = "Y";
const char *Y1 = "Y1";
const char *Y2 = "Y2";
const char *Y3 = "Y3";
const char *Y4 = "Y4";
const char *PB = "Pb";
const char *PR = "Pr";
const char *A = "a";
const char *B = "b";
const char *C = "c";
const char *D = "d";
const int DENOM = 255;

/******************************************************************************
 *                              DC component                                  *
 *****************************************************************************/

/* Takes a pixel and the constant value that tells the function which value
 * to compute and returns the component version of that value
 */
float comp_trans(Pnm_rgb pixel, const char *VAL)
{
        float red = (float)pixel->red;
        float blue = (float)pixel->blue;
        float green = (float)pixel->green;
        float result;

        if (strcmp(VAL, Y) == 0){
                result = 0.299 * red + 0.587 * green + 0.114 * blue;    
        }
        else if (strcmp(VAL, PB) == 0){
                result = -0.168736 * red - 0.331264 * green + 0.5 * blue;
        }
        else if (strcmp(VAL, PR) == 0){
                result = 0.5 * red - 0.418688 * green - 0.081312 * blue;
        }
        else{
                fprintf(stderr, "Must use Y, PB, or PR for VAL\n");
        }
        result /= DENOM; 
        return result;
}

/* Takes in the y, pb, and pr component values and returns a new pixel that 
 * holds the r, g, and b values obtained from the calculations
 */
Pnm_rgb rgb_trans(float y, float pb, float pr)
{
        int r, g, b;
        Pnm_rgb pixel = ALLOC(sizeof(*pixel));
        r = (1.0 * y + 0.0 * pb + 1.402 * pr) * DENOM + .5;
        g = (1.0 * y - 0.344136 * pb - 0.714136 * pr) * DENOM + .5;
        b = (1.0 * y + 1.772 * pb + 0.0 * pr) * DENOM + .5;

        if (r > DENOM){
                r = DENOM;
        } 
        if (g > DENOM){
                g = DENOM;
        }
        if (b > DENOM){
                b = DENOM;
        }
        if (r < 0){ 
                r = 0;
        }
        if (g < 0){
                g = 0;
        }
        if (b < 0){
                b = 0;
        }

        pixel->red = (unsigned)r;
        pixel->blue = (unsigned)b;
        pixel->green = (unsigned)g;
        return pixel;
}

/******************************************************************************
 *                                      DCT                                   *
 *****************************************************************************/

/* Takes in all the brightness values and the value of the coefficient desired
 * and returns the result.  For b, c, and d, if the result is greater than 
 * 0.3 or less than -0.3, it is rounded.
 */
float DCT_trans(float y1, float y2, float y3, float y4, const char *VAL)
{
        float result;
        float denom = 4.0;
        if (strcmp(VAL, A) == 0){
                result = (y4 + y3 + y2 + y1) / denom;
        } else if (strcmp(VAL, B) == 0){
                result = (y4 + y3 - y2 - y1) / denom;
        } else if (strcmp(VAL, C) == 0){
                result = (y4 - y3 + y2 - y1) / denom;
        } else if (strcmp(VAL, D) == 0){
                result = (y4 - y3 - y2 + y1) / denom;
        } else{
                fprintf(stderr, "Must use A, B, C, or D for VAL\n");
        }

        if (strcmp(VAL, A) != 0){
                if (result > 0.3){
                        result = 0.3;
                } else if (result < -0.3){
                        result = -0.3;
                }
        }
        return result;  
}

/* Takes in the cosine coefficients and the desired brightness and reverse
 * the DCT.
 */
float DCT_inverse(float a, float b, float c, float d, const char *VAL)
{
        float result;
        if (strcmp(VAL, Y1) == 0){
                result = a - b - c + d;
        } else if (strcmp(VAL, Y2) == 0){
                result = a - b + c -d;
        } else if (strcmp(VAL, Y3) == 0){
                result = a + b - c - d;
        } else if (strcmp(VAL, Y4) == 0){
                result = a + b + c + d;
        } else{
                fprintf(stderr, "Must use Y1, Y2, Y3, or Y4 for VAL\n");
        }
        return result;  

}

/******************************************************************************
 *                              Quantization, baby                            *
 *****************************************************************************/

/* Takes in the value and the constant declaring what scale to use and returns
 * a value that will fit in the number of bits that that field can have.
 */
int quantize(float value, const char *TYPE)
{
        float result = 0.0;
        if (strcmp(TYPE, A) == 0){
                result = value * A_SCALE;
                result += .5;
                unsigned val = (unsigned)result;
                return val;
        } else{
                result = value * BCD_SCALE;
                result += .5;
                return (int)result;
        }
}

/* Takes in a scaled index and the constant declaring the scale to be used and
 * returns the unquantized value that was stored
 */
float rev_quantize(int index, const char *TYPE)
{
        float result = 0.0;
        if (strcmp(TYPE, A) == 0){
                result  = (float)index / A_SCALE;
        } else{
                result = (float)index / BCD_SCALE;
        }
        return result;
}
