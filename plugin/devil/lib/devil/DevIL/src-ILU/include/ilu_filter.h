//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 05/28/2001 <--Y2K Compliant! =]
//
// Filename: src-ILU/include/ilu_filter.h
//
// Description: Applies filters to an image.
//
//-----------------------------------------------------------------------------


#ifndef FILTER_H
#define FILTER_H

#include "ilu_internal.h"


static const ILint filter_average_scale = 9;
static const ILint filter_average_bias  = 1;
static const ILint filter_average[] =
{	1, 1, 1,
	1, 1, 1,
	1, 1, 1 };


static const ILint filter_gaussian_scale = 16;
static const ILint filter_gaussian_bias  = 1;
static const ILint filter_gaussian[] =
{	1, 2, 1,
	2, 4, 2,
	1, 2, 1 };


static const ILint filter_h_sobel_scale = 1;
static const ILint filter_h_sobel_bias  = 0;
static const ILint filter_h_sobel[] =
{	1,  2,  1,
	0,  0,  0,
   -1, -2, -1 };


static const ILint filter_v_sobel_scale = 1;
static const ILint filter_v_sobel_bias  = 0;
static const ILint filter_v_sobel[] =
{	1,  0, -1,
	2,  0, -2,
    1,  0, -1 };


static const ILint filter_h_prewitt_scale = 1;
static const ILint filter_h_prewitt_bias  = 0;
static const ILint filter_h_prewitt[] =
{	1,  1,  1,
	0,  0,  0,
   -1, -1, -1 };


static const ILint filter_v_prewitt_scale = 1;
static const ILint filter_v_prewitt_bias  = 0;
static const ILint filter_v_prewitt[] =
{	1,  0, -1,
	1,  0, -1,
    1,  0, -1 };

static const ILint filter_emboss_scale = 1;
static const ILint filter_emboss_bias  = 128;
static const ILint filter_emboss[] =
{	-1,  0, 1,
	-1,  0, 1,
    -1,  0, 1 };

static const ILint filter_embossedge_scale = 1;
static const ILint filter_embossedge_bias  = 0;
static const ILint filter_embossedge[] =
{	-1,  0, 1,
	-1,  0, 1,
    -1,  0, 1 };

#endif//FILTER_H
