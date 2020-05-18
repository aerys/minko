//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 10/12/2001
//
// Filename: src-ILU/src/ilu_filter_rcg.cpp
//
// Description: Scales an image.  Based on the Graphic Gems III source.
//
//-----------------------------------------------------------------------------


/*
 *		Filtered Image Rescaling
 *
 *		  by Dale Schumacher
 *
 */

/*
	Additional changes by Ray Gardener, Daylon Graphics Ltd.
	December 4, 1999

	Summary:

		- Horizontal filter contributions are calculated on the fly,
		  as each column is mapped from src to dst image. This lets 
		  us omit having to allocate a temporary full horizontal stretch 
		  of the src image.

		- If none of the src pixels within a sampling region differ, 
		  then the output pixel is forced to equal (any of) the source pixel.
		  This ensures that filters do not corrupt areas of constant color.

		- Filter weight contribution results, after summing, are 
		  rounded to the nearest pixel color value instead of 
		  being casted to ILubyte (usually an int or char). Otherwise, 
		  artifacting occurs. 

		- All memory allocations checked for failure; zoom() returns 
		  error code. new_image() returns NULL if unable to allocate 
		  pixel storage, even if Image struct can be allocated.
		  Some assertions added.

		- load_image(), save_image() take filenames, not file handles.

		- TGA bitmap format available. If you want to add a filetype, 
		  extend the gImageHandlers array, and factor in your load_image_xxx()
		  and save_image_xxx() functions. Search for string 'add your' 
		  to find appropriate code locations.

		- The 'input' and 'output' command-line arguments do not have 
		  to specify .bm files; any supported filetype is okay.

		- Added implementation of getopt() if compiling under Windows.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>
#include <math.h>

#include "ilu_internal.h"
#include "ilu_filter.h"
#include "ilu_states.h"

#define	filter_support		(1.0)
double filter( double t) {
	/* f(t) = 2|t|^3 - 3|t|^2 + 1, -1 <= t <= 1 */
	if(t < 0.0) t = -t;
	if(t < 1.0) return((2.0 * t - 3.0) * t * t + 1.0);
	return(0.0);
}

#define	box_support		(0.5)

double box_filter( double t) {
	if((t > -0.5) && (t <= 0.5)) return(1.0);
	return(0.0);
}

#define	triangle_support	(1.0)
double triangle_filter( double t ) {
	if(t < 0.0) t = -t;
	if(t < 1.0) return(1.0 - t);
	return(0.0);
}

#define	bell_support		(1.5)

double bell_filter( double t ) {
	if(t < 0) t = -t;
	if(t < .5) return(.75 - (t * t));
	if(t < 1.5) {
		t = (t - 1.5);
		return(.5 * (t * t));
	}
	return(0.0);
}

#define	B_spline_support	(2.0)

#define FRAC_2_3 0.6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666667
#define FRAC_1_6 0.1666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666667

double B_spline_filter( double t ) { /* box (*) box (*) box (*) box */
	double tt;

	if(t < 0) t = -t;
	if(t < 1) {
		tt = t * t;
		return((.5 * tt * t) - tt + (2.0 / 3.0));
	} else if(t < 2) {
		t = 2 - t;
		return((1.0 / 6.0) * (t * t * t));
	}
	return(0.0);
}

double sinc( double x) {
	x *= IL_PI;
	if(x != 0) return(sin(x) / x);
	return(1.0);
}

#define	Lanczos3_support	(3.0)
double Lanczos3_filter( double t ) {
	if(t < 0) t = -t;
	if(t < 3.0) return(sinc(t) * sinc(t/3.0));
	return(0.0);
}

#define	Mitchell_support	(2.0)

#define	B	(1.0 / 3.0)
#define	C	(1.0 / 3.0)

double Mitchell_filter( double t ) {
	double tt;

	tt = t * t;
	if(t < 0) t = -t;
	if(t < 1.0) {
		t = (((12.0 - 9.0 * B - 6.0 * C) * (t * tt))
		   + ((-18.0 + 12.0 * B + 6.0 * C) * tt)
		   + (6.0 - 2 * B));
		return(t / 6.0);
	} else if(t < 2.0) {
		t = (((-1.0 * B - 6.0 * C) * (t * tt))
		   + ((6.0 * B + 30.0 * C) * tt)
		   + ((-12.0 * B - 48.0 * C) * t)
		   + (8.0 * B + 24 * C));
		return(t / 6.0);
	}
	return(0.0);
}


int roundcloser(double d) {
	int n = (int) d;
	double diff = d - (double)n;
	if(diff < 0)
		diff = -diff;
	if(diff >= 0.5)
	{
		if(d < 0)
			n--;
		else
			n++;
	}
	return n;
}

int wrap_filter_sample(int i, int size) {
	int j;
	j = i % (size * 2);
	if (j < 0)
		j += (size * 2);
	if (j < size)
		return j;
	return 2 * size - j - 1;
}

/*char	_Program[] = "fzoom";
char	_Version[] = "0.30";
char	_Copyright[] = "Public Domain 1991 by Dale Schumacher. Mods by Ray Gardener";*/


/* Note: if you define ILubyte to something bigger than char, 
		you may need to add more support in bitmap file I/O functions.
*/

#define	WHITE_PIXEL	(255)
#define	BLACK_PIXEL	(0)

#define CLAMP(v,l,h)    ((v)<(l) ? (l) : (v) > (h) ? (h) : v)

static ILuint c;  // Current colour plane offset

typedef struct {
	int	pixel;
	double	weight;
} CONTRIB;

typedef struct {
	int	n;		/* number of contributors */
	CONTRIB	*p;		/* pointer to list of contributions */
} CLIST;

CLIST	*contrib;		/* array of contribution lists */

/*
 *	generic image access and i/o support routines
 */

/* 
	calc_x_contrib()
	
	Calculates the filter weights for a single target column.
	contribX->p must be freed afterwards.

	Returns -1 if error, 0 otherwise.
*/
int calc_x_contrib( CLIST *contribX, double xscale, double fwidth, int dstwidth, int srcwidth, double (*filterf)(double), int i) {
	double width;
	double fscale;
	double center, left, right;
	double weight;
	int j, k, n;

	if(xscale < 1.0)
	{
		/* Shrinking image */
		width = fwidth / xscale;
		fscale = 1.0 / xscale;

		contribX->n = 0;
		contribX->p = (CONTRIB *)icalloc((int) (width * 2 + 1),
				sizeof(CONTRIB));
		if (contribX->p == NULL) {
			return -1;
		}

		center = (double) i / xscale;
		left = ceil(center - width);
		right = floor(center + width);
		for(j = (int)left; j <= right; ++j)
		{
			weight = center - (double) j;
			weight = (*filterf)(weight / fscale) / fscale;
			n = wrap_filter_sample(j, srcwidth);
			
			k = contribX->n++;
			contribX->p[k].pixel = n;
			contribX->p[k].weight = weight;
		}
	
	}
	else
	{
		/* Expanding image */
		contribX->n = 0;
		contribX->p = (CONTRIB*)icalloc((int) (fwidth * 2 + 1),
				sizeof(CONTRIB));
		if (contribX->p == NULL) {
			return -1;
		}
		center = (double) i / xscale;
		left = ceil(center - fwidth);
		right = floor(center + fwidth);

		for(j = (int)left; j <= right; ++j)
		{
			weight = center - (double) j;
			weight = (*filterf)(weight);
			n = wrap_filter_sample(j, srcwidth);
			k = contribX->n++;
			contribX->p[k].pixel = n;
			contribX->p[k].weight = weight;
		}
	}
	return 0;
} /* calc_x_contrib */


/*
	zoom()

	Resizes bitmaps while resampling them.
	Returns -1 if error, 0 if success.
*/
int zoom( ILimage *dst, ILimage *src, double (*filterf)(double), double fwidth) {
	ILubyte* tmp;
	double xscale, yscale;		/* zoom scale factors */
	int xx;
	int i, j, k;			/* loop variables */
	int n;				/* pixel number */
	double center, left, right;	/* filter calculation variables */
	double width, fscale, weight;	/* filter calculation variables */
	ILubyte pel, pel2;
	int bPelDelta;
	CLIST	*contribY;		/* array of contribution lists */
	CLIST	contribX;
	int		nRet = -1;

	/* create intermediate column to hold horizontal dst column zoom */
	tmp = (ILubyte*)ialloc(src->Height * sizeof(ILubyte));
	if (tmp == NULL) {
		return 0;
	}

	xscale = (double) dst->Width / (double) src->Width;

	/* Build y weights */
	/* pre-calculate filter contributions for a column */
	contribY = (CLIST*)icalloc(dst->Height, sizeof(CLIST));
	if (contribY == NULL) {
		ifree(tmp);
		return -1;
	}

	yscale = (double) dst->Height / (double) src->Height;

	if(yscale < 1.0)
	{
		width = fwidth / yscale;
		fscale = 1.0 / yscale;
		for(i = 0; i < (ILint)dst->Height; ++i)
		{
			contribY[i].n = 0;
			contribY[i].p = (CONTRIB*)icalloc((int) (width * 2 + 1),
					sizeof(CONTRIB));
			if(contribY[i].p == NULL) {
				ifree(tmp);
				ifree(contribY);
				return -1;
			}
			center = (double) i / yscale;
			left = ceil(center - width);
			right = floor(center + width);
			for(j = (int)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight / fscale) / fscale;
				n = wrap_filter_sample(j, src->Height);
				k = contribY[i].n++;
				contribY[i].p[k].pixel = n;
				contribY[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < (ILint)dst->Height; ++i) {
			contribY[i].n = 0;
			contribY[i].p = (CONTRIB*)icalloc((int) (fwidth * 2 + 1),
					sizeof(CONTRIB));
			if (contribY[i].p == NULL) {
				ifree(tmp);
				ifree(contribY);
				return -1;
			}
			center = (double) i / yscale;
			left = ceil(center - fwidth);
			right = floor(center + fwidth);
			for(j = (int)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight);
				n = wrap_filter_sample(j, src->Height);
				k = contribY[i].n++;
				contribY[i].p[k].pixel = n;
				contribY[i].p[k].weight = weight;
			}
		}
	}


	for(xx = 0; xx < (ILint)dst->Width; xx++)
	{
		if(0 != calc_x_contrib(&contribX, xscale, fwidth, 
								dst->Width, src->Width, filterf, xx))
		{
			goto __zoom_cleanup;
		}
		/* Apply horz filter to make dst column in tmp. */
		for(k = 0; k < (ILint)src->Height; ++k)
		{
			weight = 0.0;
			bPelDelta = IL_FALSE;
			// Denton:  Put get_pixel source here
			//pel = get_pixel(src, contribX.p[0].pixel, k);
			pel = src->Data[k * src->Bps + contribX.p[0].pixel * src->Bpp + c];
			for(j = 0; j < contribX.n; ++j)
			{
				// Denton:  Put get_pixel source here
				//pel2 = get_pixel(src, contribX.p[j].pixel, k);
				pel2 = src->Data[k * src->Bps + contribX.p[j].pixel * src->Bpp + c];
				if(pel2 != pel)
					bPelDelta = IL_TRUE;
				weight += pel2 * contribX.p[j].weight;
			}
			weight = bPelDelta ? roundcloser(weight) : pel;

			tmp[k] = (ILubyte)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL);
		} /* next row in temp column */

		ifree(contribX.p);

		/* The temp column has been built. Now stretch it 
		 vertically into dst column. */
		for(i = 0; i < (ILint)dst->Height; ++i)
		{
			weight = 0.0;
			bPelDelta = IL_FALSE;
			pel = tmp[contribY[i].p[0].pixel];

			for(j = 0; j < contribY[i].n; ++j)
			{
				pel2 = tmp[contribY[i].p[j].pixel];
				if(pel2 != pel)
					bPelDelta = IL_TRUE;
				weight += pel2 * contribY[i].p[j].weight;
			}
			weight = bPelDelta ? roundcloser(weight) : pel;
			// Denton:  Put set_pixel source here
			//put_pixel(dst, xx, i, (ILubyte)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL));
			dst->Data[i * dst->Bps + xx * dst->Bpp + c] =
				(ILubyte)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL);
		} /* next dst row */
	} /* next dst column */
	nRet = 0; /* success */

__zoom_cleanup:
	ifree(tmp);

	// Free the memory allocated for vertical filter weights
	for (i = 0; i < (ILint)dst->Height; ++i)
		ifree(contribY[i].p);
	ifree(contribY);

	return nRet;
} /* zoom */


ILuint iluScaleAdvanced(ILuint Width, ILuint Height, ILenum Filter)
{
	double (*f)(double) = filter;
	double s = filter_support;
	ILimage *Dest;

	iluCurImage = ilGetCurImage();
	if (iluCurImage == NULL) {
		ilSetError(ILU_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	// Not supported yet.
	if (iluCurImage->Type != IL_UNSIGNED_BYTE ||
		iluCurImage->Format == IL_COLOUR_INDEX ||
		iluCurImage->Depth > 1) {
			ilSetError(ILU_ILLEGAL_OPERATION);
			return IL_FALSE;
	}

	switch (Filter)
	{
		case ILU_SCALE_BOX: f=box_filter; s=box_support; break;
		case ILU_SCALE_TRIANGLE: f=triangle_filter; s=triangle_support; break;
		case ILU_SCALE_BELL: f=bell_filter; s=bell_support; break;
		case ILU_SCALE_BSPLINE: f=B_spline_filter; s=B_spline_support; break;
		case ILU_SCALE_LANCZOS3: f=Lanczos3_filter; s=Lanczos3_support; break;
		case ILU_SCALE_MITCHELL: f=Mitchell_filter; s=Mitchell_support; break;
		//case 'h': f=filter; s=filter_support; break;
	}

	Dest = ilNewImage(Width, Height, 1, iluCurImage->Bpp, 1);
	Dest->Origin = iluCurImage->Origin;
	Dest->Duration = iluCurImage->Duration;
	for (c = 0; c < (ILuint)iluCurImage->Bpp; c++) {
		if (zoom(Dest, iluCurImage, f, s) != 0) {
			return IL_FALSE;
		}
	}

	ilTexImage(Width, Height, 1, iluCurImage->Bpp, iluCurImage->Format, iluCurImage->Type, Dest->Data);
	iluCurImage->Origin = Dest->Origin;
	iluCurImage->Duration = Dest->Duration;
	ilCloseImage(Dest);

	return IL_TRUE;
}

