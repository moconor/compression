/* a2plain.c
 * By: Connor Taylor and Shea Clark-Tieche
 * This file defines the method suite that is used for a UArray2.  The 
 * declarations of the types, functions, and containing struct can be found
 * in a2methods.h
 */

#include <string.h>

#include <a2plain.h>
#include "uarray2.h"

/* define a private version of each function in A2Methods_T that we 
 * implement 
 */
typedef A2Methods_UArray2 A2; /* private abbreviation */

static A2 new(int width, int height, int size) 
{
        return UArray2_new(width, height, size);
}

static A2 new_with_blocksize(int w, int h, int size, int b_size)
{
        (void)b_size;
        return UArray2_new(w, h, size);
}

static void a2free (A2 *array2p) 
{
        UArray2_T *temp = (UArray2_T *)array2p; 
        UArray2_free(*temp);
}

static int width    (A2 array2) { return UArray2_width    (array2); }
static int height   (A2 array2) { return UArray2_height   (array2); }
static int size     (A2 array2) { return UArray2_size     (array2); }
static int blocksize(A2 array2)
{ 
        (void)array2;
        return 1; 
}

/* Our implementation of UArray2_at takes (arr, row, col), so i and j are 
 * reversed for this to account for this.
 */
static A2Methods_Object *at(A2 array2, int i, int j) 
{
  return UArray2_at(array2, j, i);
}

typedef void applyfun(int i, int j, UArray2_T array2, void *elem, void *cl);

static void map_row_major (A2 array2, A2Methods_applyfun apply, void *cl) 
{
        UArray2_map_row_major(array2, (applyfun*)apply, cl);
}

static void map_col_major(A2 array2, A2Methods_applyfun apply, void *cl)
{
        UArray2_map_col_major(array2, (applyfun*)apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void *cl;
};

static void apply_small(int i, int j, 
                        UArray2_T array2, 
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)array2;
        cl->apply(elem, cl->cl);
}

static void small_map_row_major(A2 a2, 
                                A2Methods_smallapplyfun apply, 
                                void *cl) 
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2 a2,
                                A2Methods_smallapplyfun apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize, /* will ignore blocksize */
        a2free,
        width,
        height,
        size,
        blocksize, /* will only return 1 */
        at,
        map_row_major, 
        map_col_major,
        NULL, /* map_block_major */
        map_row_major, /* map_default */
        small_map_row_major,
        small_map_col_major,
        NULL, /* small_map_block_major */
        small_map_row_major, /* small_map_default */
};

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
