/************************************************************************************

Filename    :   Util_ImageWindow.h
Content     :   An output object for windows that can display raw images for testing
Created     :   March 13, 2014
Authors     :   Dean Beeler

Copyright   :   Copyright 2014 Oculus, Inc. All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.1 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.1 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*************************************************************************************/

#ifndef UTIL_IMAGEWINDOW_H
#define UTIL_IMAGEWINDOW_H

#if defined(OVR_OS_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#endif

#include "../Kernel/OVR_Hash.h"
#include "../Kernel/OVR_Array.h"
#include "../Kernel/OVR_Threads.h"
#include "../Kernel/OVR_Deque.h"

#include <stdint.h>

namespace OVR { namespace Util {

	typedef struct 
	{
		float x;
		float y;
		float radius;
		float r;
		float g;
		float b;
		bool  fill;
	} CirclePlot;

	typedef struct  
	{
		float x;
		float y;
		float r;
		float g;
		float b;
	OVR::String text;
	} TextPlot;

class Frame : virtual public RefCountBaseV<Frame>
	{
public:

	Frame( int frame ) :
		frameNumber( frame ),
		imageData( NULL ),
		colorImageData( NULL ),
		plots(),
		textLines(),
		width( 0 ),
		height( 0 ),
		colorPitch( 0 ),
		ready( false )
	{

	}

	~Frame()
	{
		if( imageData )
			free( imageData );
		if( colorImageData )
			free( colorImageData );

		plots.ClearAndRelease();
		textLines.ClearAndRelease();
	}

	int						frameNumber;

		Array<CirclePlot> plots;
	Array<TextPlot>			textLines;
		void*			  imageData;
		void*			  colorImageData;
		int				  width;
		int				  height;
		int				  colorPitch;
		bool			  ready;
};

#if defined(OVR_OS_WIN32)
class ImageWindow
{
	HWND hWindow;
	ID2D1RenderTarget* pRT;
	D2D1_SIZE_U resolution;

	Mutex*						frontBufferMutex;

	InPlaceMutableDeque< Ptr<Frame> >	frames;

	ID2D1Bitmap*				greyBitmap;
	ID2D1Bitmap*				colorBitmap;
    
public:
	// constructors
	ImageWindow();
	ImageWindow( uint32_t width, uint32_t height );
	virtual ~ImageWindow();

	void GetResolution( size_t& width, size_t& height ) { width = resolution.width; height = resolution.height; }

	void OnPaint(); // Called by Windows when it receives a WM_PAINT message

	void UpdateImage( const uint8_t* imageData, uint32_t width, uint32_t height ) { UpdateImageBW( imageData, width, height ); }
	void UpdateImageBW( const uint8_t* imageData, uint32_t width, uint32_t height );
	void UpdateImageRGBA( const uint8_t* imageData, uint32_t width, uint32_t height, uint32_t pitch );
	void Complete(); // Called by drawing thread to submit a frame

	void Process(); // Called by rendering thread to do window processing

	void AssociateSurface( void* surface );

	void addCircle( float x , float y, float radius, float r, float g, float b, bool fill );
	void addText( float x, float y, float r, float g, float b, OVR::String text );

	static ImageWindow*			GlobalWindow( int window ) { return globalWindow[window]; }
	static int					WindowCount() { return windowCount; }

private:

	Ptr<Frame>					lastUnreadyFrame();

	static const int			MaxWindows = 4;
	static ImageWindow*			globalWindow[MaxWindows];
	static int					windowCount;
	static ID2D1Factory*		pD2DFactory;
	static IDWriteFactory*		pDWriteFactory;
};

#else

class ImageWindow
{
public:
	// constructors
	ImageWindow() {}
	ImageWindow( uint32_t width, uint32_t height ) { OVR_UNUSED( width ); OVR_UNUSED( height ); }
	virtual ~ImageWindow() { }

	void GetResolution( size_t& width, size_t& height ) { width = 0; height = 0; }

	void OnPaint() { }

	void UpdateImage( const uint8_t* imageData, uint32_t width, uint32_t height ) { UpdateImageBW( imageData, width, height ); }
	void UpdateImageBW( const uint8_t* imageData, uint32_t width, uint32_t height ) { OVR_UNUSED( imageData ); OVR_UNUSED( width ); OVR_UNUSED( height ); }
	void UpdateImageRGBA( const uint8_t* imageData, uint32_t width, uint32_t height, uint32_t pitch ) { OVR_UNUSED( imageData ); OVR_UNUSED( width ); OVR_UNUSED( height ); OVR_UNUSED( pitch ); }
	void Complete() { }

	void Process() { }

	void AssociateSurface( void* surface ) { OVR_UNUSED(surface); }

	void addCircle( float x , float y, float radius, float r, float g, float b, bool fill ) { OVR_UNUSED( x ); OVR_UNUSED( y ); OVR_UNUSED( radius ); OVR_UNUSED( r ); OVR_UNUSED( g ); OVR_UNUSED( b ); OVR_UNUSED( fill ); }
	void addText( float x, float y, float r, float g, float b, OVR::String text ) { OVR_UNUSED( x ); OVR_UNUSED( y ); OVR_UNUSED( r ); OVR_UNUSED( g ); OVR_UNUSED( b ); OVR_UNUSED( text ); }

	static ImageWindow*			GlobalWindow( int window ) { return globalWindow[window]; }
	static int					WindowCount() { return windowCount; }

private:

	static const int			MaxWindows = 4;
	static ImageWindow*			globalWindow[4];
	static int					windowCount;
};

#endif

}} // namespace OVR::Util


#endif