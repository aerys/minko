/*
 *  altivec_typeconversion.h
 *  DevIL
 *
 *  Created by Meloni Dario on 17/04/05.
 *
 */

#include "il_internal.h"

#ifdef ALTIVEC_GCC
#ifndef ALTIVEC_COMMON
#define ALTIVEC_COMMON

typedef union {
	vector unsigned int vuint;
	unsigned int suint[4];
	vector unsigned char vuchar;
	unsigned char suchar[4];
	vector float vf;
	float sf[4];
} vector_t;

// Loads 16 byte from the specified address, aligned or not
//vector unsigned char load_unaligned( unsigned char *buffer );

// Fills a vector with the specified value
vector float fill_vector_f( float value );

#define eround(v,x) (((int)((v/x)*10)%10) > 0 ? (v/x) : (v/x)+1)
#define eround16(v) eround(v,16)

#endif
#endif
