//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 02/26/2009
//
// Filename: src-IL/include/il_dpx.h
//
// Description: Reads from a Digital Picture Exchange (.dpx) file.
//				Specifications for this format were	found at
//				http://www.cineon.com/ff_draft.php and
//				http://www.fileformat.info/format/dpx/.
//
//-----------------------------------------------------------------------------


#ifndef DPX_H
#define DPX_H

#include "il_internal.h"


#ifdef _WIN32
#pragma pack(push, packed_struct, 1)
#endif
typedef struct R32
{
	ILubyte	r, g, b, a;
} IL_PACKSTRUCT R32;
#ifdef _WIN32
#pragma pack(pop, packed_struct)
#endif


typedef struct DPX_FILE_INFO
{
    ILuint	MagicNum;        /* magic number 0x53445058 (SDPX) or 0x58504453 (XPDS) */
    ILuint	Offset;           /* offset to image data in bytes */
    ILbyte	Vers[8];          /* which header format version is being used (v1.0)*/
    ILuint	FileSize;        /* file size in bytes */
    ILuint	DittoKey;        /* read time short cut - 0 = same, 1 = new */
    ILuint	GenHdrSize;     /* generic header length in bytes */
    ILuint	IndHdrSize;     /* industry header length in bytes */
    ILuint	UserDataSize;   /* user-defined data length in bytes */
    ILbyte	FileName[100];   /* image file name */
    ILbyte	CreateTime[24];  /* file creation date "yyyy:mm:dd:hh:mm:ss:LTZ" */
    ILbyte	Creator[100];     /* file creator's name */
    ILbyte	Project[200];     /* project name */
    ILbyte	Copyright[200];   /* right to use or copyright info */
    ILuint	Key;              /* encryption ( FFFFFFFF = unencrypted ) */
    ILbyte	Reserved[104];    /* reserved field TBD (need to pad) */
} DPX_FILE_INFO;

typedef struct DPX_IMAGE_ELEMENT
{
    ILuint    DataSign;			/* data sign (0 = unsigned, 1 = signed ) */
								/* "Core set images are unsigned" */
    ILuint		RefLowData;		/* reference low data code value */
    R32			RefLowQuantity;	/* reference low quantity represented */
    ILuint		RefHighData;	/* reference high data code value */
    R32			RefHighQuantity;/* reference high quantity represented */
    ILubyte		Descriptor;		/* descriptor for image element */
    ILubyte		Transfer;		/* transfer characteristics for element */
    ILubyte		Colorimetric;	/* colormetric specification for element */
    ILubyte		BitSize;		/* bit size for element */
	ILushort	Packing;		/* packing for element */
    ILushort	Encoding;		/* encoding for element */
    ILuint		DataOffset;		/* offset to data of element */
    ILuint		EolPadding;		/* end of line padding used in element */
    ILuint		EoImagePadding;	/* end of image padding used in element */
    ILbyte		Description[32];/* description of element */
} DPX_IMAGE_ELEMENT;  /* NOTE THERE ARE EIGHT OF THESE */


typedef struct DPX_IMAGE_INFO
{
    ILushort	Orientation;          /* image orientation */
    ILushort	NumElements;       /* number of image elements */
    ILuint		Width;      /* or x value */
    ILuint		Height;  /* or y value, per element */
	DPX_IMAGE_ELEMENT	ImageElement[8];
    ILubyte		reserved[52];             /* reserved for future use (padding) */
} DPX_IMAGE_INFO;


typedef struct DPX_IMAGE_ORIENT
{
    ILuint		XOffset;               /* X offset */
    ILuint		YOffset;               /* Y offset */
    R32			XCenter;               /* X center */
    R32			YCenter;               /* Y center */
    ILuint		XOrigSize;            /* X original size */
    ILuint		YOrigSize;            /* Y original size */
    ILbyte		FileName[100];         /* source image file name */
    ILbyte		CreationTime[24];      /* source image creation date and time */
    ILbyte		InputDev[32];          /* input device name */
    ILbyte		InputSerial[32];       /* input device serial number */
    ILushort	Border[4];              /* border validity (XL, XR, YT, YB) */
    ILuint		PixelAspect[2];        /* pixel aspect ratio (H:V) */
    ILubyte		Reserved[28];           /* reserved for future use (padding) */
} DPX_IMAGE_ORIENT;


typedef struct DPX_MOTION_PICTURE_HEAD
{
    ILbyte film_mfg_id[2];    /* film manufacturer ID code (2 digits from film edge code) */
    ILbyte film_type[2];      /* file type (2 digits from film edge code) */
    ILbyte offset[2];         /* offset in perfs (2 digits from film edge code)*/
    ILbyte prefix[6];         /* prefix (6 digits from film edge code) */
    ILbyte count[4];          /* count (4 digits from film edge code)*/
    ILbyte format[32];        /* format (i.e. academy) */
    ILuint   frame_position;    /* frame position in sequence */
    ILuint   sequence_len;      /* sequence length in frames */
    ILuint   held_count;        /* held count (1 = default) */
    R32   frame_rate;        /* frame rate of original in frames/sec */
    R32   shutter_angle;     /* shutter angle of camera in degrees */
    ILbyte frame_id[32];      /* frame identification (i.e. keyframe) */
    ILbyte slate_info[100];   /* slate information */
    ILubyte    reserved[56];      /* reserved for future use (padding) */
} DPX_MOTION_PICTURE_HEAD;


typedef struct DPX_TELEVISION_HEAD
{
    ILuint tim_code;            /* SMPTE time code */
    ILuint userBits;            /* SMPTE user bits */
    ILubyte  interlace;           /* interlace ( 0 = noninterlaced, 1 = 2:1 interlace*/
    ILubyte  field_num;           /* field number */
    ILubyte  video_signal;        /* video signal standard (table 4)*/
    ILubyte  unused;              /* used for byte alignment only */
    R32 hor_sample_rate;     /* horizontal sampling rate in Hz */
    R32 ver_sample_rate;     /* vertical sampling rate in Hz */
    R32 frame_rate;          /* temporal sampling rate or frame rate in Hz */
    R32 time_offset;         /* time offset from sync to first pixel */
    R32 gamma;               /* gamma value */
    R32 black_level;         /* black level code value */
    R32 black_gain;          /* black gain */
    R32 break_point;         /* breakpoint */
    R32 white_level;         /* reference white level code value */
    R32 integration_times;   /* integration time(s) */
    ILubyte  reserved[76];        /* reserved for future use (padding) */
} DPX_TELEVISION_HEAD;


// For checking and reading
ILboolean iIsValidDpx(void);
//ILboolean iCheckDpx(DPXHEAD *Header);
ILboolean iLoadDpxInternal(void);

#endif//PCX_H
