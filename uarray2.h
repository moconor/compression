/* uarray2.h
 * By: Connor Taylor and Shea Clark-Tieche
 * This header file declares the interface that can be used for the UArray2
 * interface. Give the user the option to create and free a UArray2, get the
 * dimensions, retrieve an element, and map over each element
 */
#ifndef UARRAY2_H
#define UARRAY2_H

#include <uarray.h>
#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <set.h>
#include <assert.h>

#define T UArray2_T
typedef struct T *T;

/* URE to pass any int < 0 */
extern T UArray2_new(int width, int height, int size);

/* URE to pass NULL */
extern void UArray2_free (T uarray2);

/* URE to pass NULL to the dimensions/size functions */
extern int UArray2_width(T uarray2);
extern int UArray2_height(T uarray2);
extern int UArray2_size(T uarray2);

/* URE to pass a NULL UArray and a row or column out of bounds */
extern void *UArray2_at(T uarray2, int row, int col);

/* Goes column by column when mapping over elements 
 * URE to pass a NULL function pointer or UArray2
 */
extern void UArray2_map_col_major(T uarray2, 
                                  void apply(int i, int j,
                                             UArray2_T arr,
                                             void *value,
                                             void *cl),
                                  void *cl);
/* Goes row by row when mapping over elements 
 * URE to pass a NULL function pointer or UArray2
 */
extern void UArray2_map_row_major(T uarray2, 
                                  void apply(int i, int j,
                                             UArray2_T arr,
                                             void *value,
                                             void *cl), 
                                  void *cl);

#undef T
#endif
