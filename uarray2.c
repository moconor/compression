/*uarray.c
 * by Shea Clark-Tieche and Connor Taylor
 * This is the implementation of a UArray 2. A UArray2 is a single UArray
 * of structs that hold the width, height, and size of what it's storing, 
 * along with a single UArray. The index for each element in the 2d array 
 * is indexed by multiplying the row it is going to be stored in, by the width
 * of the array, plus the column in which it's going to store. 
 */


#include "uarray2.h"

struct UArray2_T {
        int width, height, size;
        UArray_T array; 
};

/*Allocates a new UArray, making sure the desired dimensions are valid
 * The UArray is a 1D representation of a 2D array.*/
UArray2_T UArray2_new(int width, int height, int size)
{
        assert(width > 0);
        assert(height > 0);
        assert(size > 0);
        UArray2_T new_array = NULL;
        int array_len = width * height;
        
        TRY {
                new_array = ALLOC(sizeof(*new_array));
        
                new_array->height = height;
                new_array->width = width;
                new_array->size = size;
                new_array->array = UArray_new(array_len, size);
        }
        EXCEPT(Mem_Failed) {
                fprintf(stderr, "Not Enough Memory for UArray2\n");
                exit(EXIT_FAILURE);
        }
        END_TRY;

        return new_array;
}

/*Free the memory allocated for the UArray*/
void UArray2_free(UArray2_T arr_to_del)
{
        assert(arr_to_del != NULL);
        UArray_free(&(arr_to_del->array));
        FREE(arr_to_del);
}

int UArray2_width(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->width;
}

int UArray2_height(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->height;
}

int UArray2_size(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->size;
}

/*Returns a void pointer to a desired location in the 2d array*/
void *UArray2_at(UArray2_T uarray2, int row, int col)
{
        assert(uarray2 != NULL);
        int width = UArray2_width(uarray2);
        int height = UArray2_height(uarray2);
        int index = (width * row) + col;
      
        assert(row >= 0 && row < height);
        assert(col >= 0 && col < width);

        return UArray_at(uarray2->array, index);
}

/*Iterates through every element in a 2d array goin row by row
 * and performs a user defined function on them*/
void UArray2_map_row_major(UArray2_T uarray2, 
                           void apply(int r, int c, 
                                      UArray2_T arr,
                                      void *value, void *cl), 
                           void *cl)
{
        int row, col;
        void *temp;

        assert(apply != NULL);

        for (row = 0; row < UArray2_height(uarray2); row++){
                for (col = 0; col < UArray2_width(uarray2); col++){
                        temp = UArray2_at(uarray2, row, col);
                        apply(col, row, uarray2, temp, cl);
                }
        }
}

/*Iterates through every element in a 2d array, going column by column,
 * performing a user defined function on them.*/
void UArray2_map_col_major(UArray2_T uarray2, 
                           void apply(int r, int c,
                                      UArray2_T arr,
                                      void *value, void *cl), 
                           void *cl)
{
        int row, col;
        void *temp;
        assert(apply != NULL);
        
        for (col = 0; col < UArray2_width(uarray2); col++){
                for (row = 0; row < UArray2_height(uarray2); row++){
                        temp = UArray2_at(uarray2, row, col);
                        apply(col, row, uarray2, temp, cl);
                }
        }
}

