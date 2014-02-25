//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2001-2002 by Denton Woods
// Last modified: 05/25/2001 <--Y2K Compliant! =]
//
// Filename: src-IL/include/il_manip.h
//
// Description: Image manipulation
//
//-----------------------------------------------------------------------------

#ifndef MANIP_H
#define MANIP_H

#ifdef _cplusplus
extern "C" {
#endif

ILboolean ilFlipImage(void);
ILboolean ilMirrorImage(void);	//@JASON New routine created 03/28/2001

//-----------------------------------------------
// Overflow handler for float-to-half conversion;
// generates a hardware floating-point overflow,
// which may be trapped by the operating system.
//-----------------------------------------------
#ifndef NOINLINE

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable : 4756)  // Disables 'named type definition in parentheses' warning
#endif
INLINE ILfloat /*ILAPIENTRY*/ ilFloatToHalfOverflow() {
	ILfloat f = 1e10;
	ILint j;
	for (j = 0; j < 10; j++)
		f *= f;				// this will overflow before
	// the for loop terminates
	return f;
}
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

//-----------------------------------------------------
// Float-to-half conversion -- general case, including
// zeroes, denormalized numbers and exponent overflows.
//-----------------------------------------------------
INLINE ILushort ILAPIENTRY ilFloatToHalf(ILuint i) {
	//
	// Our floating point number, f, is represented by the bit
	// pattern in integer i.  Disassemble that bit pattern into
	// the sign, s, the exponent, e, and the significand, m.
	// Shift s into the position where it will go in in the
	// resulting half number.
	// Adjust e, accounting for the different exponent bias
	// of float and half (127 versus 15).
	//

	register int s =  (i >> 16) & 0x00008000;
	register int e = ((i >> 23) & 0x000000ff) - (127 - 15);
	register int m =   i        & 0x007fffff;

	//
	// Now reassemble s, e and m into a half:
	//

	if (e <= 0)
	{
		if (e < -10)
		{
			//
			// E is less than -10.  The absolute value of f is
			// less than HALF_MIN (f may be a small normalized
			// float, a denormalized float or a zero).
			//
			// We convert f to a half zero.
			//

			return 0;
		}

		//
		// E is between -10 and 0.  F is a normalized float,
		// whose magnitude is less than HALF_NRM_MIN.
		//
		// We convert f to a denormalized half.
		// 

		m = (m | 0x00800000) >> (1 - e);

		//
		// Round to nearest, round "0.5" up.
		//
		// Rounding may cause the significand to overflow and make
		// our number normalized.  Because of the way a half's bits
		// are laid out, we don't have to treat this case separately;
		// the code below will handle it correctly.
		// 

		if (m &  0x00001000)
			m += 0x00002000;

		//
		// Assemble the half from s, e (zero) and m.
		//

		return s | (m >> 13);
	}
	else if (e == 0xff - (127 - 15))
	{
		if (m == 0)
		{
			//
			// F is an infinity; convert f to a half
			// infinity with the same sign as f.
			//

			return s | 0x7c00;
		}
		else
		{
			//
			// F is a NAN; we produce a half NAN that preserves
			// the sign bit and the 10 leftmost bits of the
			// significand of f, with one exception: If the 10
			// leftmost bits are all zero, the NAN would turn 
			// into an infinity, so we have to set at least one
			// bit in the significand.
			//

			m >>= 13;
			return s | 0x7c00 | m | (m == 0);
		}
	}
	else
	{
		//
		// E is greater than zero.  F is a normalized float.
		// We try to convert f to a normalized half.
		//

		//
		// Round to nearest, round "0.5" up
		//

		if (m &  0x00001000)
		{
			m += 0x00002000;

			if (m & 0x00800000)
			{
				m =  0;		// overflow in significand,
				e += 1;		// adjust exponent
			}
		}

		//
		// Handle exponent overflow
		//

		if (e > 30)
		{
			ilFloatToHalfOverflow();	// Cause a hardware floating point overflow;
			return s | 0x7c00;	// if this returns, the half becomes an
		}   			// infinity with the same sign as f.

		//
		// Assemble the half from s, e and m.
		//

		return s | (e << 10) | (m >> 13);
	}
}

// Taken from OpenEXR
INLINE ILuint ILAPIENTRY ilHalfToFloat (ILushort y) {

	int s = (y >> 15) & 0x00000001;
	int e = (y >> 10) & 0x0000001f;
	int m =  y		  & 0x000003ff;

	if (e == 0)
	{
		if (m == 0)
		{
			//
			// Plus or minus zero
			//

			return s << 31;
		}
		else
		{
			//
			// Denormalized number -- renormalize it
			//

			while (!(m & 0x00000400))
			{
				m <<= 1;
				e -=  1;
			}

			e += 1;
			m &= ~0x00000400;
		}
	}
	else if (e == 31)
	{
		if (m == 0)
		{
			//
			// Positive or negative infinity
			//

			return (s << 31) | 0x7f800000;
		}
		else
		{
			//
			// Nan -- preserve sign and significand bits
			//

			return (s << 31) | 0x7f800000 | (m << 13);
		}
	}

	//
	// Normalized number
	//

	e = e + (127 - 15);
	m = m << 13;

	//
	// Assemble s, e and m.
	//

	return (s << 31) | (e << 23) | m;
}
#endif //NOINLINE

#ifdef _cplusplus
}
#endif

#endif//MANIP_H
