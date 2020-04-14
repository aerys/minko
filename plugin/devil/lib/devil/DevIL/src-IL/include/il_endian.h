//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 01/29/2009
//
// Filename: src-IL/include/il_endian.h
//
// Description: Handles Endian-ness
//
//-----------------------------------------------------------------------------

#ifndef IL_ENDIAN_H
#define IL_ENDIAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "il_internal.h"

#ifdef WORDS_BIGENDIAN  // This is defined by ./configure.
#ifndef __BIG_ENDIAN__
#define __BIG_ENDIAN__ 1
#endif
#endif

#if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __BIG_ENDIAN__) \
  || (defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__))
#undef __LITTLE_ENDIAN__
#define Short(s) iSwapShort(s)
#define UShort(s) iSwapUShort(s)
#define Int(i) iSwapInt(i)
#define UInt(i) iSwapUInt(i)
#define Float(f) iSwapFloat(f)
#define Double(d) iSwapDouble(d)

#define BigShort(s)  
#define BigUShort(s)  
#define BigInt(i)  
#define BigUInt(i)  
#define BigFloat(f)  
#define BigDouble(d)  
#else
#undef __BIG_ENDIAN__
#if !defined(__LITTLE_ENDIAN__) 
#undef __LITTLE_ENDIAN__  // Not sure if it's defined by any compiler...
#define __LITTLE_ENDIAN__
#endif
#define Short(s)  
#define UShort(s)  
#define Int(i)  
#define UInt(i)  
#define Float(f)  
#define Double(d)  

#define BigShort(s) iSwapShort(s)
#define BigUShort(s) iSwapUShort(s)
#define BigInt(i) iSwapInt(i)
#define BigUInt(i) iSwapUInt(i)
#define BigFloat(f) iSwapFloat(f)
#define BigDouble(d) iSwapDouble(d)
#endif

#ifdef IL_ENDIAN_C
#undef NOINLINE
#undef INLINE
#define INLINE
#endif

void   iSwapUShort(ILushort *s);
void   iSwapShort(ILshort *s);
void   iSwapUInt(ILuint *i);
void   iSwapInt(ILint *i);
void   iSwapFloat(ILfloat *f);
void   iSwapDouble(ILdouble *d);
ILushort GetLittleUShort();
ILshort  GetLittleShort();
ILuint   GetLittleUInt();
ILint    GetLittleInt();
ILfloat  GetLittleFloat();
ILdouble GetLittleDouble();
ILushort GetBigUShort();
ILshort  GetBigShort();
ILuint   GetBigUInt();
ILint    GetBigInt();
ILfloat  GetBigFloat();
ILdouble GetBigDouble();
ILubyte SaveLittleUShort(ILushort s);
ILubyte SaveLittleShort(ILshort s);
ILubyte SaveLittleUInt(ILuint i);
ILubyte SaveLittleInt(ILint i);
ILubyte SaveLittleFloat(ILfloat f);
ILubyte SaveLittleDouble(ILdouble d);
ILubyte SaveBigUShort(ILushort s);
ILubyte SaveBigShort(ILshort s);
ILubyte SaveBigUInt(ILuint i);
ILubyte SaveBigInt(ILint i);
ILubyte SaveBigFloat(ILfloat f);
ILubyte SaveBigDouble(ILdouble d);

void EndianSwapData(void *_Image);

#ifdef __cplusplus
}
#endif

#endif//ENDIAN_H