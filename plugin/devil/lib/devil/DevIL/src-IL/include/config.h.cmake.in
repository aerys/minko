#ifndef __CONFIG_H__
#define __CONFIG_H__

/* this file is either config.h.cmake.in or generated from it by cmake */


/* Theses are present in original autoconf setup, but unused in the code:

AC_APPLE_UNIVERSAL_BUILD
HAVE_STDINT_H
HAVE_LIBM
HAVE_LIBZ
HAVE_DLFCN_H
HAVE_MEMORY_H
HAVE_INTTYPES_H
HAVE_GLUT_GLUT_H
HAVE_GL_GLUT_H
HAVE_GL_GLU_H
HAVE_GL_GL_H
HAVE_OPENGL_GLU_H
HAVE_OPENGL_GL_H
HAVE_PTHREAD
HAVE_STDLIB_H
HAVE_STRINGS_H
HAVE_STRING_H
HAVE_SYS_STAT_H
HAVE_SYS_TYPES_H
HAVE_UNISTD_H
HAVE_VARARGS_GLU_TESSCB
HAVE_WINDOWS_H
PTHREAD_CREATE_JOINABLE
STDC_HEADERS
X_DISPLAY_MISSING
IL_USE_JPEGLIB_UNMODIFIED
SSE
SSE2
SSE3

*/


/* compiler stuff */

/* Altivec extension found */
#undef ALTIVEC_GCC

/* "Enable debug code features" */
#undef DEBUG

/* PPC_ASM assembly found */
#undef GCC_PCC_ASM

/* X86_64_ASM assembly found */
#undef GCC_X86_64_ASM

/* X86_ASM assembly found */
#undef GCC_X86_ASM

/* restrict keyword available */
#undef RESTRICT_KEYWORD


#cmakedefine WORDS_BIGENDIAN @WORDS_BIGENDIAN@


/* assorted memory management stuff */

/* define if you have memalign memory allocation */
#undef MEMALIGN

/* define if you have posix_memalign memory allocation */
#undef POSIX_MEMALIGN

/* define if you have mm_malloc.h header */
#undef MM_MALLOC

/* define if you have valloc memory allocation */
#undef VALLOC

/* Memory must be vector aligned */
#undef VECTORMEM



/* Deal with these once the core library is building...

#undef ILUT_USE_ALLEGRO
#undef ILUT_USE_DIRECTX8
#undef ILUT_USE_DIRECTX9
#undef ILUT_USE_OPENGL
#undef ILUT_USE_SDL
#undef ILUT_USE_X11
#undef ILUT_USE_XRENDER
#undef ILUT_USE_XSHM
#undef ILU_ENABLED

*/

/* Define if you can support at least some ASM */
#undef IL_INLINE_ASM


/* File formats */

/* blp support (BLP is the texture format for Blizzard games.) */
#undef IL_NO_BLP

/* bmp support (BMP is a standard Windows bitmap.) */
#undef IL_NO_BMP

/* dcx support (DCX is a multi-page PCX file.) */
#undef IL_NO_DCX

/* dds support (DDS (DirectDraw Surface) is used by DirectX to load images
   (usually DXTC).) */
#undef IL_NO_DDS

/* dicom support (DICOM (Digital Imaging and Communications in Medicine) is
   used extensively in medical imaging.) */
#undef IL_NO_DICOM

/* exr support (EXR is a HDR image file format standardized by Industrial
   Light and Magic. OpenEXR powered) */
#cmakedefine IL_NO_EXR @IL_NO_EXR@

/* fits support (FITS (Flexible Image Transport System) is used for storing
   scientific images and is used extensively in astronomy.) */
#undef IL_NO_FITS

/* Define if you want to disable support for various game formats */
#undef IL_NO_GAMES

/* gif support */
#undef IL_NO_GIF

/* hdr support (HDR is the Radiance High Dynamic Range format, using more bits
   per channel than normal.) */
#undef IL_NO_HDR

/* icns support (ICNS is the icon format for the Mac OS X operation system.)
   */
#undef IL_NO_ICNS

/* icon support (ICO is the icon format for the Windows operating system.) */
#undef IL_NO_ICON

/* iff support (IFF (Interchange File Format) is used to transfer images
   between software from different companies.) */
#undef IL_NO_IFF

/* ilbm support (ILBM (Amiga Interleaved BitMap Format).) */
#undef IL_NO_ILBM

/* iwi support (IWI is the Infinity Ward Image format used in the Call of Duty
   games.) */
#undef IL_NO_IWI

/* jp2 support (JP2 (JPEG 2000) is a better, but not so widespread,
   alternative to JPEG. JasPer powered) */
#cmakedefine IL_NO_JP2 @IL_NO_JP2@

/* jpeg support (JPEG is most common lossy format. libjpeg powered) */
#cmakedefine IL_NO_JPG @IL_NO_JPG@

/* lcms support () */
#cmakedefine IL_NO_LCMS @IL_NO_LCMS@
/* LCMS: include lcms.h instead of lcms/lcms.h (implicit on windows) */
#cmakedefine LCMS_NODIRINCLUDE @LCMS_NODIRINCLUDE@

/* lif support (LIF is the image format of the game Homeworld.) */
#undef IL_NO_LIF

/* mdl support (MDL is the format for models in the game Half-Life.) */
#undef IL_NO_MDL

/* mng support (MNG is basically the animated version of PNG but is not
   widespread.) */
#cmakedefine IL_NO_MNG @IL_NO_MNG@

/* mp3 support (MP3 (MPEG-1 Audio Layer 3) sometimes have images in them, such
   as cover art.) */
#undef IL_NO_MP3

/* pcd support (PCD is the Kodak PhotoCD format.) */
#undef IL_NO_PCD

/* pcx support (PCX is one of the first widely accepted DOS imaging standards)
   */
#undef IL_NO_PCX

/* pic support (PIC is used by SoftImage software.) */
#undef IL_NO_PIC

/* pix support () */
#undef IL_NO_PIX

/* png support (PNG is a smart, loseless and even open format. libpng
   powered.) */
#cmakedefine IL_NO_PNG @IL_NO_PNG@

/* pnm support (PNM (Portable aNy Map format) is considered the
   least-common-denominator for converting pixmap, graymap, or bitmap files
   between different platforms.) */
#undef IL_NO_PNM

/* psd support (PSD is the PhotoShop native file format.) */
#undef IL_NO_PSD

/* psp support (PSP is the Paint Shop Pro native file format.) */
#undef IL_NO_PSP

/* pxr support (PXR is used by Pixar.) */
#undef IL_NO_PXR

/* raw support (RAW is simply raw data.) */
#undef IL_NO_RAW

/* rot support (ROT is the image format for the game Homeworld 2.) */
#undef IL_NO_ROT

/* sgi support (SGI is the native raster graphics file format for Silicon
   Graphics workstations) */
#undef IL_NO_SGI

/* sun support (SUN is the image format used on Sun's operating systems.) */
#undef IL_NO_SUN

/* texture support (TEXTURE is the format used in Medieval II: Total War
   (similar to DDS).) */
#undef IL_NO_TEXTURE

/* tga support (TGA (aka TARGA file format) has been used mainly in games. It
   is open and simple.) */
#undef IL_NO_TGA

/* tiff support (TIFF is a lossless format supporting greater color depths.
   libtiff powered) */
#cmakedefine IL_NO_TIF @IL_NO_TIF@

/* tpl support (TPL is the format used by many GameCube and Wii games.) */
#undef IL_NO_TPL

/* utx support (UTX is the format used in the Unreal Tournament game series.) */
#cmakedefine IL_NO_UTX @IL_NO_UTX@

/* vtf support (VTF (Valve Texture Format) is used by games based on Valve's
   Source engine.) */
#undef IL_NO_VTF

/* doom support (WAD is the format for graphics in the original DooM I/II.) */
#undef IL_NO_WAD

/* wal support (WAL is the image format used for the game Quake 2.) */
#undef IL_NO_WAL

/* wbmp support (WBMP (Wireless Application Protocol Bitmap) is a monochrome
   graphics file format optimized for mobile computing devices) */
#undef IL_NO_WBMP

/* wdp support (WDP is Microsoft's successor to JPEG, able to store many
   different image types either lossy or losslessly, also known as HD Photo.)
   */
#cmakedefine IL_NO_WDP %IL_NO_WDP%

/* xpm support (XPM (X PixMap) is an ASCII-based image used in X Windows.) */
#undef IL_NO_XPM

/* Define if you have nvidia texture tools library installed */
#cmakedefine IL_USE_DXTC_NVIDIA %IL_USE_DXTC_NVIDIA%

/* Define if you have libsquish installed */
#cmakedefine IL_USE_DXTC_SQUISH %IL_USE_DXTC_SQUISH%









/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#undef LT_OBJDIR

/* Building on Mac OS X */
#undef MAX_OS_X


/* Define to 1 if your C compiler doesn't accept -c and -o together. */
#undef NO_MINUS_C_MINUS_O

/* Name of package */
#undef PACKAGE

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the home page for this package. */
#undef PACKAGE_URL

/* Define to the version of this package. */
#undef PACKAGE_VERSION



/* Version number of package */
#undef VERSION



#endif //__CONFIG_H__

