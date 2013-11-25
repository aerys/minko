//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Last modified: 17/04/2005
// by Meloni Dario
// 
// Description: Common altivec function.
//
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif //HAVE_CONFIG_H

#ifdef ALTIVEC_GCC
#include "altivec_common.h"

// from http://developer.apple.com/hardware/ve/alignment.html
/*vector unsigned char load_unaligned( unsigned char *buffer ) {
	vector unsigned char MSQ, LSQ;
	vector unsigned char mask;
	MSQ = vec_ld(0, buffer); // most significant quadword
	LSQ = vec_ld(15, buffer); // least significant quadword
	mask = vec_lvsl(0, buffer); // create the permute mask
	return vec_perm(MSQ, LSQ, mask);// align the data
}*/

vector float fill_vector_f( float value ) {
	vector_t vec;
	vec.sf[0] = value;
	vector float temp = vec_ld(0,vec.sf);
	return vec_splat(temp,0);
}

inline unsigned int round16( unsigned int v ) {
	return ((int)((v/16)*10)%10) > 0 ? (v/16) : (v/16)+1;
}


#endif
