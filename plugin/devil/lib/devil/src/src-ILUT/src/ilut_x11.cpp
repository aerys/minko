//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2002 by Denton Woods
// Copyright (C) 2002 Nelson Rush.
// Last modified: 05/18/2002
//
// Filename: src-ILUT/src/ilut_x11.cpp
//
// Description: X11 Pixmap and XImage binding functions (with XShm)
//
//-----------------------------------------------------------------------------

/*
** This file was created by Jesse Maurais Wed April 4, 2007
** Contact: www.jessemaurais@gmail.com
**
**
** This patch to the Devil Tookit binds to the X Windows System Version 11,
** using client-side XImages and server-side Pixmaps, with support for both
** ZPixmaps and the insane XYPixmap format. (Depends on the X server)
**
** If the XShm extension to X11 is present at the time of ./configure then
** support for shared memory XImages and Pixmaps is also compiled in. Note
** that "shared" does not mean Devil and X are sharing the same memory space.
** This is not possible, because both libraries make byte-for-byte copies
** of their data. It means that memory is part of an inter-process memory
** segment (see XShm spec).
**
** TODO
** 1) Assumed the display depth is 24 bits (the most common) but there should
** be a check, and iXConvertImage should handle this properly. I don't think
** this should be difficult to modify from whats here.
** 2) It would be nice to convert from an XImage back to a Devil image for
** saving changes. Would be seful for an interactive image editor.
** 3) Possibly some additional OpenGL bindings for GLX Pbuffers.
**
** FYI
** It was a long night figuring out the under-documented XYPixmap format.
*/


#include "ilut_internal.h"
#ifdef ILUT_USE_X11


int bits;	// bits per pixel
int field;	// bits per channel
int bytes;	// bytes per pixel
int grain;	// bytes per line
int width;	// pixels per line
int height;	// number of lines
ILpal* palette;	// for indexed colors
char* data;	// pointer to pixels



void iXGrabImage( ILimage * img )
{
	bits     = img->Bpp*8;	// bits per pixel
	field    = img->Bpc;	// bits per channel
	bytes    = img->Bpp;	// bytes per pixel
	grain    = img->Bps;	// bytes per line
	width    = img->Width;
	height   = img->Height;
	palette  = &img->Pal;
	data     = (char*)img->Data;
}


Bool iXGrabCurrentImage(void)
{
	ilutCurImage = ilGetCurImage();
	if (!ilutCurImage) {
		return False;
	}
	iXGrabImage(ilutCurImage);
	return True;
}


void iXConvertImage( Display * dpy, XImage * img )
{
	int x,y,z;
	int sX,dX;
	int sY,dY;
 	int sZ,dZ;
	int plane;


	ILimage * tmp;

	switch ( img->byte_order )
	{
	 case LSBFirst:
		tmp = iConvertImage( ilutCurImage,IL_BGR,IL_UNSIGNED_BYTE );
		break;
	 case MSBFirst:
		tmp = iConvertImage( ilutCurImage,IL_RGB,IL_UNSIGNED_BYTE );
		break;
	 default:
		return;
	}

	if ( !tmp ) return;
	iXGrabImage( tmp );


	switch ( img->format )
	{
	 case ZPixmap:

		for ( y = 0; y < height; y ++ )
		{
		 dY = y * img->bytes_per_line;
		 sY = y * grain;

		 for ( x = 0; x < width; x ++ )
		 {
		  dX = x * img->bits_per_pixel / 8;
		  sX = x * bytes;

		  for ( z = 0; z < bytes; z ++ )
		  {
		   img->data[dX+dY+z] = data[sX+sY+z];
		  }
		 }
		}

	 break;

	 case XYPixmap:

		for ( y = 0; y < height; y ++ )
		{
		 sY = y * grain;

		 for ( x = 0; x < width; x ++ )
		 {
		  sX = x * bytes;

		  for ( z = 0; z < bits; z ++ )
		  {
		   sZ = z / 8;
		   dZ = z % 8;

		   if ( data[sY+sX+sZ] & ( 1 << dZ ) )
		   {
		    plane = bits - z - 1;

		    sZ = x % 8;
		    dX = x / 8;
		    dY = y * img->bytes_per_line;
		    dZ = plane * img->bytes_per_line * height;

		    img->data[dZ+dY+dX] |= 1 << sZ;
		   }
		  }
		 }
		}

	 break;

	 default:
		ilSetError( ILUT_NOT_SUPPORTED );
	}

	ilCloseImage( tmp );
}

ILboolean ilutXInit(void) {
	return IL_TRUE;
}







XImage * ILAPIENTRY ilutXCreateImage( Display * dpy )
{
	Visual * vis;
	XImage * img;
	char * buffer;

	if (!iXGrabCurrentImage()) {
		return NULL;
	}

	buffer = malloc( width * height * 4 );
	if (!buffer) {
		return NULL;
	}

	vis = CopyFromParent;
	img = XCreateImage( dpy,vis, 24,ZPixmap,0,buffer,width,height,8,0 );
	if (!img) {
		free(buffer);
		return NULL;
	}

	iXConvertImage( dpy,img );

	return img;
}



Pixmap ILAPIENTRY ilutXCreatePixmap( Display * dpy, Drawable draw )
{
	XImage * img;
	GC gc;
	Pixmap pix;

	img = ilutXCreateImage( dpy );
	if (!img) {
		return None;
	}

	gc = DefaultGC(dpy,DefaultScreen(dpy));
	if (!gc) {
		XDestroyImage( img );
		return None;
	}

	pix = XCreatePixmap( dpy,draw, width,height,24 );
	if (!pix ) {
		XDestroyImage( img );
		return None;
	}

	XPutImage( dpy,pix,gc,img, 0,0,0,0,width,height );

	XDestroyImage( img );
	
	return pix;
}



XImage * ILAPIENTRY ilutXLoadImage( Display * dpy, char * filename )
{
	iBindImageTemp();
	if (!ilLoadImage(filename)) {
		return NULL;
	}
	return ilutXCreateImage( dpy );
}



Pixmap ILAPIENTRY ilutXLoadPixmap( Display * dpy, Drawable draw, char * filename )
{
	iBindImageTemp();
	if (!ilLoadImage(filename)) {
		return None;
	}
	return ilutXCreatePixmap( dpy,draw );
}






#ifdef ILUT_USE_XSHM
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>


XImage * ILAPIENTRY ilutXShmCreateImage( Display * dpy, XShmSegmentInfo * info )
{
	Visual * vis;
	XImage * img;

	// Get server supported format

	int size,format = XShmPixmapFormat( dpy );

	// Grab the current image

	if (!iXGrabCurrentImage()) {
		return NULL;
	}

	// Create a shared image

	vis = CopyFromParent;
	img = XShmCreateImage( dpy,vis, 24,format,NULL,info,width,height );

	if (!img) {
		return NULL;
	}

	// Create shared memory

	size = img->bytes_per_line * img->height;

	info->shmid = shmget( IPC_PRIVATE, size, IPC_CREAT | 0666 );
	info->shmaddr = img->data = shmat( info->shmid, 0, 0 );
	info->readOnly = False;

	// Attach to server

	XShmAttach( dpy,info );

	// Copy image pixels to shared memory

	iXConvertImage( dpy,img );

	return img;
}



void ILAPIENTRY ilutXShmDestroyImage( Display * dpy, XImage * img, XShmSegmentInfo * info )
{
	XShmDetach( dpy,info );
	XDestroyImage( img );
	XFlush( dpy );

	shmdt( info->shmaddr );
	shmctl( info->shmid, IPC_RMID, 0 );
}



Pixmap ILAPIENTRY ilutXShmCreatePixmap( Display * dpy, Drawable draw, XShmSegmentInfo * info )
{
	Pixmap pix;
	XImage*img;

	// Create a dumby image

	img = ilutXShmCreateImage( dpy,info );
	if (!img) {
		return None;
	}

	// Use the same memory segment in the pixmap

	pix = XShmCreatePixmap( dpy,draw, info->shmaddr,info,width,height,24 );
	if (!pix) {
		ilutXShmDestroyImage( dpy,img,info );
		return None;
	}

	// Riddance to the image

	XDestroyImage( img );

	return pix;
}



void ILAPIENTRY ilutXShmFreePixmap( Display * dpy, Pixmap pix, XShmSegmentInfo * info )
{
	XShmDetach( dpy,info );
	XFreePixmap( dpy,pix );
	XFlush( dpy );

	shmdt( info->shmaddr );
	shmctl( info->shmid, IPC_RMID, 0 );
}



XImage * ILAPIENTRY ilutXShmLoadImage( Display * dpy, char* filename, XShmSegmentInfo * info )
{
	iBindImageTemp();
	if (!ilLoadImage(filename)) {
		return NULL;
	}
	return ilutXShmCreateImage( dpy,info );
}



Pixmap ILAPIENTRY ilutXShmLoadPixmap( Display * dpy, Drawable draw, char* filename, XShmSegmentInfo * info )
{
	iBindImageTemp();
	if (!ilLoadImage(filename)) {
		return None;
	}
	return ilutXShmCreatePixmap( dpy,draw,info );
}





#endif//ILUT_USE_XSHM


#endif//ILUT_USE_X11

