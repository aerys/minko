//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/08/2009
//
// Filename: src-IL/src/il_ilbm.cpp
//
// Description: IFF ILBM file (.iff, .ilbm, .lbm) functions
//   IFF ILBM loader, ported from SDL_Image library (IMG_lbm.c)
//   http://www.libsdl.org/cgi/viewvc.cgi/trunk/SDL_image/IMG_lbm.c?view=markup
//
//   Handles Amiga ILBM and PBM images (including .lbm files saved by the PC
//   version of dpaint)
//   Handles ExtraHalfBright and HAM images.
// 
//   Adapted from SDL_image by Ben Campbell (http://scumways.com) 2009-02-23
//
//-----------------------------------------------------------------------------


// TODO: sort out the .iff extension confusion: .iff is currently handled by
// Maya IFF/CIMG handler (il_iff.c), but it should defer to this one if
// fileturns out to be an ILBM. I think the best solution would be to
// rename the IFF handler to CIMG, and create a new iff handler to act as
// a front end, passing off to either il_ilbm or il_cimg...
// For now, this handler only handles .lbm and .ilbm extenstions (but
// traditionally, .iff is more common).

#include "il_internal.h"
#ifndef IL_NO_ILBM
#include <stdlib.h>

ILboolean   iIsValidIlbm(void);
ILboolean   iLoadIlbmInternal(void);
static ILboolean load_ilbm(void);
static int isLBM(void );


ILboolean ilIsValidIlbm(ILconst_string FileName)
{
    ILHANDLE    f;
    ILboolean   bIlbm = IL_FALSE;

    if (!iCheckExtension(FileName, IL_TEXT("iff")) &&
        !iCheckExtension(FileName, IL_TEXT("ilbm")) &&
        !iCheckExtension(FileName, IL_TEXT("lbm")) &&
        !iCheckExtension(FileName, IL_TEXT("ham")) ) {
        ilSetError(IL_INVALID_EXTENSION);
        return bIlbm;
    }

    f = iopenr(FileName);
    if (f == NULL) {
        ilSetError(IL_COULD_NOT_OPEN_FILE);
        return bIlbm;
    }

    bIlbm = ilIsValidIlbmF(f);
    icloser(f);

    return bIlbm;
}


ILboolean ilIsValidIlbmF(ILHANDLE File)
{
    ILuint      FirstPos;
    ILboolean   bRet;

    iSetInputFile(File);
    FirstPos = itell();
    bRet = iIsValidIlbm();
    iseek(FirstPos, IL_SEEK_SET);

    return bRet;
}


ILboolean ilIsValidIlbmL(const void *Lump, ILuint Size)
{
    iSetInputLump(Lump, Size);
    return iIsValidIlbm();
}


ILboolean iIsValidIlbm()
{
    return isLBM() ? IL_TRUE:IL_FALSE;
}


// Reads a file
ILboolean ilLoadIlbm(ILconst_string FileName)
{
    ILHANDLE    IlbmFile;
    ILboolean   bIlbm = IL_FALSE;

    IlbmFile = iopenr(FileName);
    if (IlbmFile == NULL) {
        ilSetError(IL_COULD_NOT_OPEN_FILE);
        return bIlbm;
    }

    bIlbm = ilLoadIlbmF(IlbmFile);
    icloser(IlbmFile);

    return bIlbm;
}


// Reads an already-opened file
ILboolean ilLoadIlbmF(ILHANDLE File)
{
    ILuint      FirstPos;
    ILboolean   bRet;

    iSetInputFile(File);
    FirstPos = itell();
    bRet = iLoadIlbmInternal();
    iseek(FirstPos, IL_SEEK_SET);

    return bRet;
}


// Reads from a memory "lump"
ILboolean ilLoadIlbmL(const void *Lump, ILuint Size)
{
    iSetInputLump(Lump, Size);
    return iLoadIlbmInternal();
}


ILboolean iLoadIlbmInternal()
{
    if (iCurImage == NULL) {
        ilSetError(IL_ILLEGAL_OPERATION);
        return IL_FALSE;
    }
    if (!iIsValidIlbm()) {
        ilSetError(IL_INVALID_VALUE);
        return IL_FALSE;
    }

    if (!load_ilbm() )
    {
        return IL_FALSE;
    }

    return ilFixImage();
}


/* some defines to allow us to use the SDL_image source in as unmodified form
 * as possible, so it'll be easy to diff. Yes, it does look evil, but it's
 * really just some local syntactic sugar - most things correspond directly
 * to DevIL calls. It's not that bad, honest ;-)
 * This will make it _much_ easier to track changes to SDL_Image.
 * - BenC
 */

#define Sint8 ILbyte
#define Sint16 ILshort
#define Sint32 ILint
#define Uint8 ILubyte
#define Uint16 ILushort
#define Uint32 ILuint

#define SDL_RWops void
#define SDL_RWtell(s) itell()
#define SDL_RWread(s,ptr,size,nmemb) iread(ptr,size,nmemb)
#define SDL_RWseek(s,offset,whence) iseek(offset, IL_ ## whence)


/* use different function names to avoid any possible symbol contamination
 * (user might also be linking with libSDL) */
#define SDL_SwapBE32(i) iSDL_SwapBE32(i)
#define SDL_SwapBE16(s) iSDL_SwapBE16(s)
static Uint16 iSDL_SwapBE16( Uint16 s )
    { Uint16 foo = s; iSwapUShort(&foo); return foo; }
static Uint32 iSDL_SwapBE32( Uint32 i )
    { Uint32 foo = i; iSwapUInt(&foo); return foo; }

/* rest of this file is as unchanged as possible from IMG_lbm.c - BenC */
/***************************************************************************/

/* This is a ILBM image file loading framework
   Load IFF pictures, PBM & ILBM packing methods, with or without stencil
   Written by Daniel Morais ( Daniel AT Morais DOT com ) in September 2001.
   24 bits ILBM files support added by Marc Le Douarain (http://www.multimania.com/mavati)
   in December 2002.
   EHB and HAM (specific Amiga graphic chip modes) support added by Marc Le Douarain
   (http://www.multimania.com/mavati) in December 2003.
   Stencil and colorkey fixes by David Raulo (david.raulo AT free DOT fr) in February 2004.
   Buffer overflow fix in RLE decompression by David Raulo in January 2008.
*/


#define MAXCOLORS 256

/* Structure for an IFF picture ( BMHD = Bitmap Header ) */

/* TODO: SDL_Image seems to get away without any struct
 * packing... should it be added? */
typedef struct
{
    Uint16 w, h;        /* width & height of the bitmap in pixels */
    Sint16 x, y;        /* screen coordinates of the bitmap */
    Uint8 planes;       /* number of planes of the bitmap */
    Uint8 mask;         /* mask type ( 0 => no mask ) */
    Uint8 tcomp;        /* compression type */
    Uint8 pad1;         /* dummy value, for padding */
    Uint16 tcolor;      /* transparent color */
    Uint8 xAspect,      /* pixel aspect ratio */
         yAspect;
    Sint16  Lpage;      /* width of the screen in pixels */
    Sint16  Hpage;      /* height of the screen in pixels */
} BMHD;

static int isLBM()
{
    SDL_RWops* src = 0;
    int start;
    int   is_LBM;
    Uint8 magic[4+4+4];

    start = SDL_RWtell(src);
    is_LBM = 0;
    if ( SDL_RWread( src, magic, sizeof(magic), 1 ) )
    {
        if ( !memcmp( magic, "FORM", 4 ) &&
            ( !memcmp( magic + 8, "PBM ", 4 ) ||
              !memcmp( magic + 8, "ILBM", 4 ) ) )
        {
            is_LBM = 1;
        }
    }
    SDL_RWseek(src, start, SEEK_SET);
    return( is_LBM );
}

static ILboolean load_ilbm(void)
{
    SDL_RWops* src = 0;
    struct { Uint8 r; Uint8 g; Uint8 b; } scratch_pal[MAXCOLORS];
    ILenum      format; /* IL_RGB (ham or 24bit) or IL_COLOUR_INDEX */

    int start;
    Uint8       id[4], pbm, colormap[MAXCOLORS*3], *MiniBuf, *ptr, count, color, msk;
    Uint32      size, bytesloaded, nbcolors;
    Uint32      i, j, bytesperline, nbplanes, plane, h;
    Uint32      remainingbytes;
    Uint32      width;
    BMHD          bmhd;
    char        *error;
    Uint8       flagHAM,flagEHB;

    error   = NULL;
    MiniBuf = NULL;

    start = SDL_RWtell(src);
    if ( !SDL_RWread( src, id, 4, 1 ) )
    {
        error=(char*)"error reading IFF chunk";
        goto done;
    }

    /* Should be the size of the file minus 4+4 ( 'FORM'+size ) */
    if ( !SDL_RWread( src, &size, 4, 1 ) )
    {
        error=(char*)"error reading IFF chunk size";
        goto done;
    }

    /* As size is not used here, no need to swap it */

    if ( memcmp( id, "FORM", 4 ) != 0 )
    {
        ilSetError(IL_INVALID_FILE_HEADER);
        error=(char*)"not a IFF file";
        goto done;
    }

    if ( !SDL_RWread( src, id, 4, 1 ) )
    {
        error=(char*)"error reading IFF chunk";
        goto done;
    }

    pbm = 0;

    /* File format : PBM=Packed Bitmap, ILBM=Interleaved Bitmap */
    if ( !memcmp( id, "PBM ", 4 ) ) pbm = 1;
    else if ( memcmp( id, "ILBM", 4 ) )
    {
        ilSetError(IL_INVALID_FILE_HEADER);
        error=(char*)"not a IFF picture";
        goto done;
    }

    nbcolors = 0;

    memset( &bmhd, 0, sizeof( BMHD ) );
    flagHAM = 0;
    flagEHB = 0;

    while ( memcmp( id, "BODY", 4 ) != 0 )
    {
        if ( !SDL_RWread( src, id, 4, 1 ) ) 
        {
            error=(char*)"error reading IFF chunk";
            goto done;
        }

        if ( !SDL_RWread( src, &size, 4, 1 ) )
        {
            error=(char*)"error reading IFF chunk size";
            goto done;
        }

        bytesloaded = 0;

        size = SDL_SwapBE32( size );

        if ( !memcmp( id, "BMHD", 4 ) ) /* Bitmap header */
        {
            if ( !SDL_RWread( src, &bmhd, sizeof( BMHD ), 1 ) )
            {
                error=(char*)"error reading BMHD chunk";
                goto done;
            }

            bytesloaded = sizeof( BMHD );

            bmhd.w      = SDL_SwapBE16( bmhd.w );
            bmhd.h      = SDL_SwapBE16( bmhd.h );
            bmhd.x      = SDL_SwapBE16( bmhd.x );
            bmhd.y      = SDL_SwapBE16( bmhd.y );
            bmhd.tcolor = SDL_SwapBE16( bmhd.tcolor );
            bmhd.Lpage  = SDL_SwapBE16( bmhd.Lpage );
            bmhd.Hpage  = SDL_SwapBE16( bmhd.Hpage );
        }

        if ( !memcmp( id, "CMAP", 4 ) ) /* palette ( Color Map ) */
        {
            if ( !SDL_RWread( src, &colormap, size, 1 ) )
            {
                error=(char*)"error reading CMAP chunk";
                goto done;
            }

            bytesloaded = size;
            nbcolors = size / 3;
        }

        if ( !memcmp( id, "CAMG", 4 ) ) /* Amiga ViewMode  */
        {
            Uint32 viewmodes;
            if ( !SDL_RWread( src, &viewmodes, sizeof(viewmodes), 1 ) )
            {
                error=(char*)"error reading CAMG chunk";
                goto done;
            }

            bytesloaded = size;
            viewmodes = SDL_SwapBE32( viewmodes );
            if ( viewmodes & 0x0800 )
                flagHAM = 1;
            if ( viewmodes & 0x0080 )
                flagEHB = 1;
        }

        if ( memcmp( id, "BODY", 4 ) )
        {
            if ( size & 1 ) ++size;     /* padding ! */
            size -= bytesloaded;
            /* skip the remaining bytes of this chunk */
            if ( size ) SDL_RWseek( src, size, SEEK_CUR );
        }
    }

    /* compute some usefull values, based on the bitmap header */

    width = ( bmhd.w + 15 ) & 0xFFFFFFF0;  /* Width in pixels modulo 16 */

    bytesperline = ( ( bmhd.w + 15 ) / 16 ) * 2;

    nbplanes = bmhd.planes;

    if ( pbm )                         /* File format : 'Packed Bitmap' */
    {
        bytesperline *= 8;
        nbplanes = 1;
    }

    if ( bmhd.mask & 1 ) ++nbplanes;   /* There is a mask ( 'stencil' ) */

    /* Allocate memory for a temporary buffer ( used for
           decompression/deinterleaving ) */

	//@TODO: Why do we have a malloc here instead of ialloc?
    if ( ( MiniBuf = (ILubyte*)malloc( bytesperline * nbplanes ) ) == NULL )
    {
        ilSetError( IL_OUT_OF_MEMORY );
        error=(char*)"not enough memory for temporary buffer";
        goto done;
    }

    if( bmhd.planes==24 || flagHAM==1 ) {
        format = IL_BGR;
    } else {
        format = IL_COLOUR_INDEX;
    }
    if( !ilTexImage( width, bmhd.h, 1, (format==IL_COLOUR_INDEX)?1:3, format, IL_UNSIGNED_BYTE, NULL ) )
        goto done;
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;

#if 0 /*  No transparent colour support in DevIL? (TODO: confirm) */
    if ( bmhd.mask & 2 )               /* There is a transparent color */
        SDL_SetColorKey( Image, SDL_SRCCOLORKEY, bmhd.tcolor );
#endif

    /* Update palette informations */

    /* There is no palette in 24 bits ILBM file */
    if ( nbcolors>0 && flagHAM==0 )
    {
        int nbrcolorsfinal = 1 << nbplanes;
        ptr = &colormap[0];

        for ( i=0; i<nbcolors; i++ )
        {
            scratch_pal[i].r = *ptr++;
            scratch_pal[i].g = *ptr++;
            scratch_pal[i].b = *ptr++;
        }

        /* Amiga EHB mode (Extra-Half-Bright) */
        /* 6 bitplanes mode with a 32 colors palette */
        /* The 32 last colors are the same but divided by 2 */
        /* Some Amiga pictures save 64 colors with 32 last wrong colors, */
        /* they shouldn't !, and here we overwrite these 32 bad colors. */
        if ( (nbcolors==32 || flagEHB ) && (1<<bmhd.planes)==64 )
        {
            nbcolors = 64;
            ptr = &colormap[0];
            for ( i=32; i<64; i++ )
            {
                scratch_pal[i].r = (*ptr++)/2;
                scratch_pal[i].g = (*ptr++)/2;
                scratch_pal[i].b = (*ptr++)/2;
            }
        }

        /* If nbcolors < 2^nbplanes, repeat the colormap */
        /* This happens when pictures have a stencil mask */
        if ( nbrcolorsfinal > (1<<bmhd.planes) ) {
            nbrcolorsfinal = (1<<bmhd.planes);
        }
        for ( i=nbcolors; i < (Uint32)nbrcolorsfinal; i++ )
        {
            scratch_pal[i].r = scratch_pal[i%nbcolors].r;
            scratch_pal[i].g = scratch_pal[i%nbcolors].g;
            scratch_pal[i].b = scratch_pal[i%nbcolors].b;
        }

        if ( !pbm )
            ilRegisterPal( scratch_pal, 3*nbrcolorsfinal, IL_PAL_RGB24 );

    }

    /* Get the bitmap */

    for ( h=0; h < bmhd.h; h++ )
    {
        /* uncompress the datas of each planes */

        for ( plane=0; plane < nbplanes; plane++ )
        {
            ptr = MiniBuf + ( plane * bytesperline );

            remainingbytes = bytesperline;

            if ( bmhd.tcomp == 1 )      /* Datas are compressed */
            {
                do
                {
                    if ( !SDL_RWread( src, &count, 1, 1 ) )
                    {
                        error=(char*)"error reading BODY chunk";
                        goto done;
                    }

                    if ( count & 0x80 )
                    {
                        count ^= 0xFF;
                        count += 2; /* now it */

                        if ( ( count > remainingbytes ) || !SDL_RWread( src, &color, 1, 1 ) )
                        {
                            if( count>remainingbytes)
                                ilSetError(IL_ILLEGAL_FILE_VALUE );
                           error=(char*)"error reading BODY chunk";
                            goto done;
                        }
                        memset( ptr, color, count );
                    }
                    else
                    {
                        ++count;

                        if ( ( count > remainingbytes ) || !SDL_RWread( src, ptr, count, 1 ) )
                        {
                            if( count>remainingbytes)
                                ilSetError(IL_ILLEGAL_FILE_VALUE );
                           error=(char*)"error reading BODY chunk";
                            goto done;
                        }
                    }

                    ptr += count;
                    remainingbytes -= count;

                } while ( remainingbytes > 0 );
            }
            else
            {
                if ( !SDL_RWread( src, ptr, bytesperline, 1 ) )
                {
                    error=(char*)"error reading BODY chunk";
                    goto done;
                }
            }
        }

        /* One line has been read, store it ! */

        ptr = ilGetData();
        if ( nbplanes==24 || flagHAM==1 )
            ptr += h * width * 3;
        else
            ptr += h * width;

        if ( pbm )                 /* File format : 'Packed Bitmap' */
        {
           memcpy( ptr, MiniBuf, width );
        }
        else        /* We have to un-interlace the bits ! */
        {
            if ( nbplanes!=24 && flagHAM==0 )
            {
                size = ( width + 7 ) / 8;

                for ( i=0; i < size; i++ )
                {
                    memset( ptr, 0, 8 );

                    for ( plane=0; plane < nbplanes; plane++ )
                    {
                        color = *( MiniBuf + i + ( plane * bytesperline ) );
                        msk = 0x80;

                        for ( j=0; j<8; j++ )
                        {
                            if ( ( plane + j ) <= 7 ) ptr[j] |= (Uint8)( color & msk ) >> ( 7 - plane - j );
                            else                        ptr[j] |= (Uint8)( color & msk ) << ( plane + j - 7 );

                            msk >>= 1;
                        }
                    }
                    ptr += 8;
                }
            }
            else
            {
                Uint32 finalcolor = 0;
                size = ( width + 7 ) / 8;
                /* 24 bitplanes ILBM : R0...R7,G0...G7,B0...B7 */
                /* or HAM (6 bitplanes) or HAM8 (8 bitplanes) modes */
                for ( i=0; i<width; i=i+8 )
                {
                    Uint8 maskBit = 0x80;
                    for ( j=0; j<8; j++ )
                    {
                        Uint32 pixelcolor = 0;
                        Uint32 maskColor = 1;
                        Uint8 dataBody;
                        for ( plane=0; plane < nbplanes; plane++ )
                        {
                            dataBody = MiniBuf[ plane*size+i/8 ];
                            if ( dataBody&maskBit )
                                pixelcolor = pixelcolor | maskColor;
                            maskColor = maskColor<<1;
                        }
                        /* HAM : 12 bits RGB image (4 bits per color component) */
                        /* HAM8 : 18 bits RGB image (6 bits per color component) */
                        if ( flagHAM )
                        {
                            switch( pixelcolor>>(nbplanes-2) )
                            {
                                case 0: /* take direct color from palette */
                                    finalcolor = colormap[ pixelcolor*3 ] + (colormap[ pixelcolor*3+1 ]<<8) + (colormap[ pixelcolor*3+2 ]<<16);
                                    break;
                                case 1: /* modify only blue component */
                                    finalcolor = finalcolor&0x00FFFF;
                                    finalcolor = finalcolor | (pixelcolor<<(16+(10-nbplanes)));
                                    break;
                                case 2: /* modify only red component */
                                    finalcolor = finalcolor&0xFFFF00;
                                    finalcolor = finalcolor | pixelcolor<<(10-nbplanes);
                                    break;
                                case 3: /* modify only green component */
                                    finalcolor = finalcolor&0xFF00FF;
                                    finalcolor = finalcolor | (pixelcolor<<(8+(10-nbplanes)));
                                    break;
                            }
                        }
                        else
                        {
                            finalcolor = pixelcolor;
                        }
#if defined( __LITTLE_ENDIAN__ )
                        {
                            *ptr++ = (Uint8)(finalcolor>>16);
                            *ptr++ = (Uint8)(finalcolor>>8);
                            *ptr++ = (Uint8)(finalcolor);
                        }
#else
                        {
                            *ptr++ = (Uint8)(finalcolor);
                            *ptr++ = (Uint8)(finalcolor>>8);
                            *ptr++ = (Uint8)(finalcolor>>16);
                        }
#endif
                        maskBit = maskBit>>1;
                    }
                }
            }
        }
    }

done:

    if ( MiniBuf ) free( MiniBuf );

    if ( error )
    {
        /* fprintf(stderr,"il_ilbm.c: '%s'\n",error); */
        return IL_FALSE;
    }

    return IL_TRUE;
}



#endif//IL_NO_ILBM

