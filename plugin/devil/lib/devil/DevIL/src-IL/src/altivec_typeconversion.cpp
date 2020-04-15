#ifdef HAVE_CONFIG_H
#include <config.h>
#endif //HAVE_CONFIG_H

#ifdef ALTIVEC_GCC
#include "altivec_typeconversion.h"

static inline void abc2cba_internal( register const vector unsigned char p[4], unsigned char *data, register unsigned int length, unsigned char *newdata ) {
	register vector unsigned char d0,d1,d2,t0,t1,t2;

	length = eround16(length);
		
	if( length >= 3 ) {
		length -= 3;
		
		d2 = vec_ld(32,data);
		d1 = vec_ld(16,data);
		d0 = vec_ld(0,data);
		
		while( length >= 3 ) {
			t0 = vec_perm(d0,d1,p[0]);
			t1 = vec_perm(d1,d0,p[1]);
			t2 = vec_perm(d2,d1,p[2]);
			t1 = vec_perm(t1,d2,p[3]);
			
			vec_st(t0,0,newdata);
			vec_st(t1,16,newdata);
			vec_st(t2,32,newdata);
			
			length -= 3;
			data += 16*3;
			newdata += 16*3;
			
			d2 = vec_ld(32,data);
			d1 = vec_ld(16,data);
			d0 = vec_ld(0,data);
		}
		t0 = vec_perm(d0,d1,p[0]);
		t1 = vec_perm(d1,d0,p[1]);
		t2 = vec_perm(d2,d1,p[2]);
		t1 = vec_perm(t1,d2,p[3]);
		
		vec_st(t0,0,newdata);
		vec_st(t1,16,newdata);
		vec_st(t2,32,newdata);
	}
	
	if( length == 2 ) {
		d0 = vec_ld(0,data);
		d1 = vec_ld(16,data);
		
		t0 = vec_perm(d0,d1,p[0]);
		t1 = vec_perm(d1,d0,p[1]);
		
		vec_st(t0,0,newdata);
		vec_st(t1,16,newdata);
	} else if( length == 1 ) {
		d0 = vec_ld(0,data);
		t0 = vec_perm(d0,d0,p[0]);
		vec_st(t0,0,newdata);
	}
}

static inline void abcd2cbad_internal( register const vector unsigned char p, unsigned char *data, unsigned int length, unsigned char *newdata ) {
	register vector unsigned char d0,d1,d2,z;
	z = vec_splat_u8(0);
	
	length = eround16(length);
	
	if( length >= 3 ) {
		length -= 3;
		
		d2 = vec_ld(32,data);
		d1 = vec_ld(16,data);
		d0 = vec_ld(0,data);
		
		while( length >= 3 ) {
			d0 = vec_perm(d0,z,p);
			d1 = vec_perm(d1,z,p);
			d2 = vec_perm(d2,z,p);
			
			vec_st(d0,0,newdata);
			vec_st(d1,16,newdata);
			vec_st(d2,32,newdata);
			
			length -= 3;
			data += 16*3;
			newdata += 16*3;
			
			d2 = vec_ld(32,data);
			d1 = vec_ld(16,data);
			d0 = vec_ld(0,data);
		}
		d0 = vec_perm(d0,z,p);
		d1 = vec_perm(d1,z,p);
		d2 = vec_perm(d2,z,p);
		
		vec_st(d0,0,newdata);
		vec_st(d1,16,newdata);
		vec_st(d2,32,newdata);
	}
	
	if( length == 2 ) {
		d0 = vec_ld(0,data);
		d1 = vec_ld(16,data);
		
		d0 = vec_perm(d0,z,p);
		d1 = vec_perm(d1,z,p);
		
		vec_st(d0,0,newdata);
		vec_st(d1,16,newdata);
	} else if( length == 1 ) {
		d0 = vec_ld(0,data);
		d0 = vec_perm(d0,d0,z);
		vec_st(d0,0,newdata);
	}
}

// Format conversion function
void abc2cba_byte( ILubyte *data, ILuint length, ILubyte *newdata ) {
	const vector unsigned char p[4] =
{	(vector unsigned char)(0x02,0x01,0x00,0x05,0x04,0x03,0x08,0x07,0x06,0x0B,0x0A,0x09,0x0E,0x0D,0x0C,0x11),
	(vector unsigned char)(0x00,0x10,0x04,0x03,0x02,0x07,0x06,0x05,0x0A,0x09,0x08,0x0D,0x0C,0x0B,0x0E,0x0F),
	(vector unsigned char)(0x1E,0x03,0x02,0x01,0x06,0x05,0x04,0x09,0x08,0x07,0x0C,0x0B,0x0A,0x0F,0x0E,0x0D),
	(vector unsigned char)(0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x10,0x0F)};
	abc2cba_internal(p,data,length,newdata);
}

void abc2cba_short( ILushort *data, ILuint length, ILushort *newdata ) {
	const vector unsigned char p[4] =
{	(vector unsigned char)(0x04,0x05,0x02,0x03,0x00,0x01,0x0A,0x0B,0x08,0x09,0x06,0x07,0x10,0x11,0x0E,0x0F),
	(vector unsigned char)(0x1C,0x1D,0x06,0x07,0x04,0x05,0x02,0x03,0x0C,0x0D,0x0A,0x0B,0x08,0x09,0x0E,0x0F),
	(vector unsigned char)(0x00,0x01,0x1E,0x1F,0x08,0x09,0x06,0x07,0x04,0x05,0x0E,0x0F,0x0C,0x0D,0x0A,0x0B),
	(vector unsigned char)(0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x12,0x13)};
	abc2cba_internal(p,(ILubyte*)data,length,(ILubyte*)newdata);
}

void abc2cba_int( ILuint *data, ILuint length, ILuint *newdata ) {
	const vector unsigned char p[4] =
{	(vector unsigned char)(0x08,0x09,0x0A,0x0B,0x04,0x05,0x06,0x07,0x00,0x01,0x02,0x03,0x14,0x15,0x16,0x17),
	(vector unsigned char)(0x00,0x01,0x02,0x03,0x1C,0x1D,0x1E,0x1F,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F),
	(vector unsigned char)(0x18,0x19,0x1A,0x1B,0x0C,0x0D,0x0E,0x0F,0x08,0x09,0x0A,0x0B,0x04,0x05,0x06,0x07),
	(vector unsigned char)(0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x10,0x11,0x12,0x13,0x0C,0x0D,0x0E,0x0F)};
	abc2cba_internal(p,(ILubyte*)data,length,(ILubyte*)newdata);
}

void abc2cba_double( ILdouble *data, ILuint length, ILdouble *newdata ) {
	const vector unsigned char p[4] =
{	(vector unsigned char)(0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F),
	(vector unsigned char)(0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F),
	(vector unsigned char)(0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F),
	(vector unsigned char)(0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F)};
	abc2cba_internal(p,(ILubyte*)data,length,(ILubyte*)newdata);
}

void abcd2cbad_byte( ILubyte *data, ILuint length, ILubyte *newdata ) {
	const vector unsigned char p = (vector unsigned char)(0x02,0x01,0x00,0x03,0x06,0x05,0x04,0x07,0x0A,0x09,0x08,0x0B, 0x0E,0x0D,0x0C,0x0F);
	abcd2cbad_internal(p,data,length,newdata);
}

void abcd2cbad_short( ILushort *data, ILuint length, ILushort *newdata ) {
	const vector unsigned char p = (vector unsigned char)(0x04,0x05,0x02,0x03,0x00,0x01,0x06,0x07,0x0C,0x0D,0x0A,0x0B,0x08,0x09,0x0E,0x0F);
	abcd2cbad_internal(p,(ILubyte*)data,length,(ILubyte*)newdata);
}

void abcd2cbad_int( ILuint *data, ILuint length, ILuint *newdata ) {
	const vector unsigned char p = (vector unsigned char)(0x08,0x09,0x0A,0x0B,0x04,0x05,0x06,0x07,0x00,0x01,0x02,0x03,0x0C,0x0D,0x0E,0x0F);
	abcd2cbad_internal(p,(ILubyte*)data,length,(ILubyte*)newdata);
}

void abcd2cbad_double( ILdouble *tdata, ILuint length, ILdouble *tnewdata ) {
	register ILubyte *data = (ILubyte*)tdata;
	register ILubyte *newdata = (ILubyte*)tnewdata;
	const vector unsigned char p = (vector unsigned char)(0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F);
	register vector unsigned char d0,d1,d2,d3,t0,t1,t2,t3;
	
	length = eround16(length);
	
	if( length >= 4 ) {
		length -= 4;
		
		d3 = vec_ld(48,data);
		d2 = vec_ld(32,data);
		d1 = vec_ld(16,data);
		d0 = vec_ld(0,data);
		
		while( length >= 4 ) {
			t0 = vec_perm(d0,d1,p);
			t1 = vec_perm(d1,d0,p);
			t2 = vec_perm(d2,d3,p);
			t3 = vec_perm(d3,d2,p);
			
			vec_st(t0,0,newdata);
			vec_st(t1,16,newdata);
			vec_st(t2,32,newdata);
			vec_st(t3,48,newdata);
			
			length -= 4;
			data += 16*4;
			newdata += 16*4;
			
			d3 = vec_ld(48,data);
			d2 = vec_ld(32,data);
			d1 = vec_ld(16,data);
			d0 = vec_ld(0,data);
		}
		t0 = vec_perm(d0,d1,p);
		t1 = vec_perm(d1,d0,p);
		t2 = vec_perm(d2,d3,p);
		t3 = vec_perm(d3,d2,p);
		
		vec_st(d0,0,newdata);
		vec_st(d1,16,newdata);
		vec_st(d2,32,newdata);
		vec_st(d3,48,newdata);
	}
	
	if( length == 2 ) {
		d0 = vec_ld(0,data);
		d1 = vec_ld(16,data);
		
		t0 = vec_perm(d0,d1,p);
		t1 = vec_perm(d1,d0,p);
		
		vec_st(t0,0,newdata);
		vec_st(t1,16,newdata);
	}
}

#endif
