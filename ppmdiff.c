#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <except.h>
#include <string.h>
#include <pnmrdr.h>
#include "ppmdiff.h"

int main(int argc, char *argv[])
{
        if (argc == 1){
                fprintf(stderr, "no args given\n");
                exit(EXIT_FAILURE);
        } else if (argc > 3){
                fprintf(stderr, "Usage: file1.ppm file2.ppm " 
                        "One may be on stdin \n");
                exit(EXIT_FAILURE);
        }

        FILE *input1 = NULL;
        FILE *input2 = NULL;
        Pnmrdr_T rdr1 = NULL;
        Pnmrdr_T rdr2 = NULL;
        read_cmd_args(argc, argv, &input1, &input2);

        TRY{
                rdr1 = Pnmrdr_new(input1);
                rdr2 = Pnmrdr_new(input2);
        } EXCEPT(Pnmrdr_Badformat){
                fprintf(stderr, "one or more of your files isnt a pnm file\n");
                exit(EXIT_FAILURE);
        } END_TRY;
        assert(rdr1 && rdr2);
        float diff = find_diff(rdr1, rdr2);
        printf("Diff is: %f\n", diff);
}

void read_cmd_args(int argc, char *argv[], FILE **input1, FILE **input2)
{
        int i;
        int stdin_used = 0;
        for (i = 1; i < argc; i++){
                if (strcmp(argv[i], "-") == 0){
                        if (stdin_used == 0){
                                if (i == 1){
                                        *input1 = stdin;
                                } else {
                                        *input2 = stdin;
                                }
                                stdin_used = 1; 
                        } else{
                                fprintf(stderr, "cannot have 2 files "
                                         "from stdin\n");
                                exit(EXIT_FAILURE);     
                        }
                }
                if (i == 1){
                        *input1 = fopen(argv[i], "r");
                }
                else{
                        *input2 = fopen(argv[i], "r");
                }
        }
}

float find_diff(Pnmrdr_T rdr1, Pnmrdr_T rdr2)
{
        float diff;
        Pnmrdr_mapdata hdr1 = Pnmrdr_data(rdr1);
        Pnmrdr_mapdata hdr2 = Pnmrdr_data(rdr2);
        if (hdr1.type != Pnmrdr_rgb || hdr2.type != Pnmrdr_rgb){
                fprintf(stderr, "one or more of your files isnt a ppm\n");
                exit(EXIT_FAILURE);
        }
        if (hdr1.width != hdr2.width || hdr1.height != hdr2.height){
                fprintf(stderr, "this program only supports images with"
                        " dimensions that differ by at most 1\n");
                exit(EXIT_FAILURE); 
        }
        
        float sum;
        for (unsigned i = 0; i < hdr1.height; i++){
                for (unsigned j = 0; j < hdr1.width; j++){
                        int r1, r2, g1, g2, b1, b2;
                        r1 = Pnmrdr_get(rdr1);
                        g1 = Pnmrdr_get(rdr1);
                        b1 = Pnmrdr_get(rdr1);
                        r2 = Pnmrdr_get(rdr2);
                        g2 = Pnmrdr_get(rdr2);
                        b2 = Pnmrdr_get(rdr2);
                        sum = pow(r1 - r2, 2) + pow(g1 - g2, 2) + 
                              pow(b1 - b2, 2);
                        float denom = 3 * hdr1.height * hdr2.width;
                        diff = sum / denom;
                        diff = sqrt(diff); 
                }
        }
        return diff;
}
