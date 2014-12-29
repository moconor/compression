/* 
 * 40image.c an image compressor and decompressor
 * Written by Connor Taylor and Matt O'Conor 
 */

#include "40image.h"
static void (*compress_or_decompress)(FILE *input) = compress40;
/* handles command line arguments and calls compress or decompress */
int main(int argc, char *argv[])
{
        int i;
        for (i = 1; i < argc; i++) {
                if (!strcmp(argv[i], "-c")) {
                        compress_or_decompress = compress40;
                } else if (!strcmp(argv[i], "-d")) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", 
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                        "       %s -c [filename]\n",
                                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1); // at most one file on command line
        if (i < argc) {
                FILE* fp = fopen(argv[i], "r");
                assert(fp);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }
}
/* reads the PPM image and maps through compressing each block
 * frees the image
 */
void compress40(FILE *fp)
{
        Pnm_ppm image = read_image(fp);
        UArray2_map_row_major(image->pixels, compress_and_write, NULL);
        Pnm_ppmfree(&image);
}
/* sets new height and width to be tested against
 * tests to make sure we're at the start of a 2x2 block
 * gets DC component info and DCT coefficients 
 * packs the codeword and writes it
 */
void compress_and_write(int i, int j, UArray2_T array, void *val, void *cl)
{
        (void)cl;
        (void)val;
        int h = trim_dim(UArray2_height(array));
        int w = trim_dim(UArray2_width(array));
        
        if ((i % 2 == 0) && (j % 2 == 0)){
                if (i == w || j == h){
                        return;
                }
                UArray_T pix_array = get_pixels(i, j, array);

                float avg_pb = get_avg_val(pix_array, PB);
                float avg_pr = get_avg_val(pix_array, PR);
                unsigned index_pb = Arith40_index_of_chroma(avg_pb);
                unsigned index_pr = Arith40_index_of_chroma(avg_pr);
                int *coeffs = get_scaled_coeffs(pix_array);

                uint64_t word = pack_bits(index_pb, index_pr, coeffs);
                if (i == 0 && j == 0){
                        printf("COMP40 Compressed image format 2\n%u %u",
                                w, h);
                        printf("\n"); 
                }
                FREE(coeffs);
                UArray_free(&pix_array);
                write_compressed(word);
        }
}
/* gets avg pb or pr from the pix_array */
float get_avg_val(UArray_T pix_array, const char *VAL)
{
        float sum = 0.0;
        int i;
        for (i = 0; i < UArray_length(pix_array); i++) {
                Pnm_rgb temp = (Pnm_rgb)UArray_at(pix_array, i);
                float temp_val = comp_trans(temp, VAL);
                sum += temp_val;
        }
        return sum / i; 
}
/* gets the y value for each pixel 
 * calculates the scaled coefficient and returns these values in an array 
 */
int *get_scaled_coeffs(UArray_T pixels)
{
        int *coeffs = ALLOC(sizeof(float) * 4);
        float y1 = comp_trans((Pnm_rgb)UArray_at(pixels, 0), Y);
        float y2 = comp_trans((Pnm_rgb)UArray_at(pixels, 1), Y);
        float y3 = comp_trans((Pnm_rgb)UArray_at(pixels, 2), Y);
        float y4 = comp_trans((Pnm_rgb)UArray_at(pixels, 3), Y);

        coeffs[0] = quantize(DCT_trans(y1, y2, y3, y4, A), A);
        coeffs[1] = quantize(DCT_trans(y1, y2, y3, y4, B), B);
        coeffs[2] = quantize(DCT_trans(y1, y2, y3, y4, C), C);
        coeffs[3] = quantize(DCT_trans(y1, y2, y3, y4, D), D);

        return coeffs;
}
/* given indices that indicate the start of a 2x2 block
 * gets pointers for each of the four pixels
 * deep copies the info and returns and array of pixels
 */
UArray_T get_pixels(int col, int row, UArray2_T array) 
{
        Pnm_rgb pix1 = (Pnm_rgb)UArray2_at(array, row, col);
        Pnm_rgb pix2 = (Pnm_rgb)UArray2_at(array, row, col + 1);
        Pnm_rgb pix3 = (Pnm_rgb)UArray2_at(array, row + 1, col);
        Pnm_rgb pix4 = (Pnm_rgb)UArray2_at(array, row + 1, col + 1);
        UArray_T pix_array = UArray_new(4, sizeof(*pix1));
        
        Pnm_rgb temp1 = (Pnm_rgb)UArray_at(pix_array, 0);
        temp1->red = pix1->red;
        temp1->green = pix1->green;
        temp1->blue = pix1->blue;
        Pnm_rgb temp2 = (Pnm_rgb)UArray_at(pix_array, 1);
        temp2->red = pix2->red;
        temp2->green = pix2->green;
        temp2->blue = pix2->blue;
        Pnm_rgb temp3 = (Pnm_rgb)UArray_at(pix_array, 2);
        temp3->red = pix3->red;
        temp3->green = pix3->green;
        temp3->blue = pix3->blue;
        Pnm_rgb temp4 = (Pnm_rgb)UArray_at(pix_array, 3);
        temp4->red = pix4->red;
        temp4->green = pix4->green;
        temp4->blue = pix4->blue;
        return pix_array;
}
/* allocates a blank ppm 
 * gets height and width from the compressed image and sets these values
 * goes through the blank image and decompresses each codeword
 * writes the new ppm to stdout
 * frees the image
 */
void decompress40(FILE *fp)
{
        Pnm_ppm image = ALLOC(sizeof(*image));
        unsigned height, width;
        int read = fscanf(fp,"COMP40 Compressed image format 2\n%u %u", 
                                &width, &height);
        assert(read == 2);
        int c = getc(fp);
        assert(c == '\n');

        image->methods = uarray2_methods_plain;
        image->height = height;
        image->width = width;
        image->denominator = DENOM;
        image->pixels = UArray2_new(width, height, sizeof(struct Pnm_rgb));
        UArray2_map_row_major(image->pixels, decompress_image, fp);
        Pnm_ppmwrite(stdout, image);
        Pnm_ppmfree(&image);
}
/* tests to see if at the start of a 2x2 block
 * extracts each field from the codeword
 * performs the rgb transformation on the DC component values
 * writes these four pixels to their proper location in the UArray2
 * frees dynamically allocated memory
 */
void decompress_image(int i, int j, UArray2_T image, void *val, void *cl)
{
        (void)val;
        FILE *fp = (FILE *)cl;

        if (i % 2 == 0 && j % 2 == 0){
                uint64_t word = read_word(fp);
                unsigned index_pb = get_uvalue(word, PB);
                unsigned index_pr = get_uvalue(word, PR);
                float pb = Arith40_chroma_of_index(index_pb);
                float pr = Arith40_chroma_of_index(index_pr);

                unsigned a = get_uvalue(word, A);
                int b = get_svalue(word, B);
                int c = get_svalue(word, C);
                int d = get_svalue(word, D);
                
                int *coeffs = ALLOC(sizeof(int) * 4);
                coeffs[0] = (int)a;
                coeffs[1] = b;
                coeffs[2] = c;
                coeffs[3] = d;

                UArray_T pixels = get_rgb(pb, pr, coeffs);
                for (int k = 0; k < 4; k++){
                        Pnm_rgb pixel = (Pnm_rgb)UArray_at(pixels, k);
                        Pnm_rgb img_pix = (Pnm_rgb)UArray2_at(image, j + k / 2,
                                                                i + k % 2);
                        img_pix->red = pixel->red;
                        img_pix->green = pixel->green;
                        img_pix->blue = pixel->blue;
                }
                FREE(coeffs);
                UArray_free(&pixels);
        }
}
/* given scaled DC component info
 * calls the reverse quantization and inverse DCT functions
 * performs the RGB transformation and deep copies the info
 * frees the old pixels and returns the new ones
 */
UArray_T get_rgb(float pb, float pr, int *coeffs)
{
        float a = rev_quantize(coeffs[0], A);
        float b = rev_quantize(coeffs[1], B);
        float c = rev_quantize(coeffs[2], C);
        float d = rev_quantize(coeffs[3], D);
        float y1 = DCT_inverse(a, b, c, d, Y1);
        float y2 = DCT_inverse(a, b, c, d, Y2);
        float y3 = DCT_inverse(a, b, c, d, Y3);
        float y4 = DCT_inverse(a, b, c, d, Y4);
        Pnm_rgb pix1 = rgb_trans(y1, pb, pr);
        Pnm_rgb pix2 = rgb_trans(y2, pb, pr);
        Pnm_rgb pix3 = rgb_trans(y3, pb, pr);
        Pnm_rgb pix4 = rgb_trans(y4, pb, pr);

        UArray_T pixels = UArray_new(4, sizeof(*pix1));

        Pnm_rgb temp1 = (Pnm_rgb)UArray_at(pixels, 0);
        temp1->red = pix1->red;
        temp1->green = pix1->green;
        temp1->blue = pix1->blue;
        Pnm_rgb temp2 = (Pnm_rgb)UArray_at(pixels, 1);
        temp2->red = pix2->red;
        temp2->green = pix2->green;
        temp2->blue = pix2->blue;
        Pnm_rgb temp3 = (Pnm_rgb)UArray_at(pixels, 2);
        temp3->red = pix3->red;
        temp3->green = pix3->green;
        temp3->blue = pix3->blue;
        Pnm_rgb temp4 = (Pnm_rgb)UArray_at(pixels, 3);
        temp4->red = pix4->red;
        temp4->green = pix4->green;
        temp4->blue = pix4->blue;

        FREE(pix1);
        FREE(pix2);
        FREE(pix3);
        FREE(pix4);

        return pixels;
}

