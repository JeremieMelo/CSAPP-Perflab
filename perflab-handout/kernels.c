/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "",              /* Team name */
    "",     /* First member full name */
    "",  /* First member email address */

    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst)
{
	int i, j;
	int dst_base = (dim - 1)*dim;
	dst += dst_base;

	for (i = 0;i < dim;i += 16) {
		for (j = 0; j < dim;j++) {
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;
			*dst = *src;src += dim;dst++;

			*dst = *src;src++;
			src -= (dim << 4) - dim;//src?-=?31*dim
			dst -= 15 + dim;
		}
		dst += dst_base + dim;
		dst += 16;
		src += (dim << 4) - dim;//src?+=31*dim
	}
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr);   
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst)
{
	
int i, j, limit, m;


dst[0].red = (src[0].red + src[1].red + src[dim + 1].red + src[dim].red) >> 2;
dst[0].blue = (src[0].blue + src[1].blue + src[dim].blue + src[dim + 1].blue) >> 2;
dst[0].green = (src[0].green + src[1].green + src[dim].green + src[dim + 1].green) >> 2;

i = dim * 2 - 1;
dst[dim - 1].red = (src[dim - 2].red + src[dim - 1].red + src[i - 1].red + src[i].red) >> 2;
dst[dim - 1].blue = (src[dim - 2].blue + src[dim - 1].blue + src[i - 1].blue + src[i].blue) >> 2;
dst[dim - 1].green = (src[dim - 2].green + src[dim - 1].green + src[i - 1].green + src[i].green) >> 2;

j = dim*(dim - 1);
i = dim*(dim - 2);
dst[j].red = (src[j].red + src[j + 1].red + src[i].red + src[i + 1].red) >> 2;
dst[j].blue = (src[j].blue + src[j + 1].blue + src[i].blue + src[i + 1].blue) >> 2;
dst[j].green = (src[j].green + src[j + 1].green + src[i].green + src[i + 1].green) >> 2;

j = dim*dim - 1;
i = dim*(dim - 1) - 1;
dst[j].red = (src[j - 1].red + src[j].red + src[i - 1].red + src[i].red) >> 2;
dst[j].blue = (src[j - 1].blue + src[j].blue + src[i - 1].blue + src[i].blue) >> 2;
dst[j].green = (src[j - 1].green + src[j].green + src[i - 1].green + src[i].green) >> 2;
//up
limit = dim - 1;
i = dim + 1;
for (j = 1;j < limit;)
{
	dst[j].red = (src[j].red + src[j - 1].red + src[j + 1].red + src[i].red + src[i + 1].red + src[i - 1].red) / 6;
	dst[j].green = (src[j].green + src[j - 1].green + src[j + 1].green + src[i].green + src[i + 1].green + src[i - 1].green) / 6;
	dst[j].blue = (src[j].blue + src[j - 1].blue + src[j + 1].blue + src[i].blue + src[i + 1].blue + src[i - 1].blue) / 6;
	j++;i++;
}
//bottom
limit  = dim*dim - 1;

for (j = dim*(dim - 1) + 1, i=j-dim;j < limit;)
{
	dst[j].red = (src[j].red + src[j - 1].red + src[j + 1].red + src[i].red + src[i + 1].red + src[i - 1].red) / 6;
	dst[j].green = (src[j].green + src[j - 1].green + src[j + 1].green + src[i].green + src[i + 1].green + src[i - 1].green) / 6;
	dst[j].blue = (src[j].blue + src[j - 1].blue + src[j + 1].blue + src[i].blue + src[i + 1].blue + src[i - 1].blue) / 6;
	j++;i++;
}
//right
m = dim - 1;
i = 3 * dim - 1;
for (j = 2 * dim - 1;j < limit;)
{
	dst[j].red = (src[j].red + src[j - 1].red + src[m].red + src[m - 1].red + src[i].red + src[i - 1].red) / 6;
	dst[j].green = (src[j].green + src[j - 1].green + src[m].green + src[m - 1].green + src[i].green + src[i - 1].green) / 6;
	dst[j].blue = (src[j].blue + src[j - 1].blue + src[m].blue + src[m - 1].blue + src[i].blue + src[i - 1].blue) / 6;
	j += dim;
	m += dim;
	i += dim;
}
//left
limit = limit - dim;
m = 0;
i = dim + dim;

for (j = dim;j < limit;)
{
	dst[j].red = (src[j].red + src[j + 1].red + src[m].red + src[m + 1].red + src[i].red + src[i + 1].red) / 6;
	dst[j].green = (src[j].green + src[j + 1].green + src[m].green + src[m + 1].green + src[i].green + src[i + 1].green) / 6;
	dst[j].blue = (src[j].blue + src[j + 1].blue + src[m].blue + src[m + 1].blue + src[i].blue + src[i + 1].blue) / 6;
	j += dim;m += dim;i += dim;
}


int idx = dim + 1;
int idx_u = 1;
int idx_d = (dim<<1)+1;
int pre_r_1, pre_g_1, pre_b_1, pre_r_2, pre_g_2, pre_b_2, pre_r_3, pre_g_3, pre_b_3;
for (i = 1; i < dim - 1;i++)
{
	//prepare the first 3 line_sum
	pre_r_1 = src[idx_u - 1].red + src[idx - 1].red + src[idx_d - 1].red;
	pre_g_1 = src[idx_u - 1].green + src[idx - 1].green + src[idx_d - 1].green;
	pre_b_1 = src[idx_u - 1].blue + src[idx - 1].blue + src[idx_d - 1].blue;

	pre_r_2 = src[idx_u].red + src[idx].red + src[idx_d].red;
	pre_g_2 = src[idx_u].green + src[idx].green + src[idx_d].green;
	pre_b_2 = src[idx_u].blue + src[idx].blue + src[idx_d].blue;

	pre_r_3 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;
	pre_g_3 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;
	pre_b_3 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;


	dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
	dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
	dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
	idx++;idx_u++;idx_d++;

	for (j = 2; j < dim - 12;j+=12)
	{  
	//block1
		//update pre_1
		pre_r_1 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;
		dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_1 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;
		dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_1 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;
		dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

		//update pre_2
		pre_r_2 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_2 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_2 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;
		
		//update pre_3
		pre_r_3 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_3 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_3 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

	//block2
		//update pre_1
		pre_r_1 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_1 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_1 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

		//update pre_2
		pre_r_2 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_2 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_2 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

		//update pre_3
		pre_r_3 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_3 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_3 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

	//block3
		//update pre_1
		pre_r_1 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_1 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_1 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

		//update pre_2
		pre_r_2 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_2 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_2 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

		//update pre_3
		pre_r_3 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_3 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_3 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

	//block4
		//update pre_1
		pre_r_1 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_1 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_1 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

		//update pre_2
		pre_r_2 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_2 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_2 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

		//update pre_3
		pre_r_3 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_3 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_3 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

	

	}
	//deal with the rest pixels of the line
	for (;j < dim - 3;j += 3) {
	
		//update pre_1
		pre_r_1 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_1 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_1 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

		//update pre_2
		pre_r_2 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_2 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_2 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

		//update pre_3
		pre_r_3 = src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red;dst[idx].red = (pre_r_1 + pre_r_2 + pre_r_3) / 9;
		pre_g_3 = src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green;dst[idx].green = (pre_g_1 + pre_g_2 + pre_g_3) / 9;
		pre_b_3 = src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue;dst[idx].blue = (pre_b_1 + pre_b_2 + pre_b_3) / 9;
		idx++;idx_u++;idx_d++;

	}
	for (;j < dim - 1;j++)
	{
		dst[idx].red = (src[idx_u - 1].red + src[idx - 1].red + src[idx_d - 1].red + src[idx_u].red + src[idx].red + src[idx_d].red + src[idx_u + 1].red + src[idx + 1].red + src[idx_d + 1].red) / 9;
		dst[idx].green = (src[idx_u - 1].green + src[idx - 1].green + src[idx_d - 1].green + src[idx_u].green + src[idx].green + src[idx_d].green + src[idx_u + 1].green + src[idx + 1].green + src[idx_d + 1].green) / 9;
		dst[idx].blue = (src[idx_u - 1].blue + src[idx - 1].blue + src[idx_d - 1].blue + src[idx_u].blue + src[idx].blue + src[idx_d].blue + src[idx_u + 1].blue + src[idx + 1].blue + src[idx_d + 1].blue) / 9;
		idx++;idx_u++;idx_d++;
		
	}
	idx+=2;
	idx_u+=2;
	idx_d+=2;
}


}

/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}

