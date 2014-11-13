/************************************************************************************

Filename    :   Util_ImageWindow.cpp
Content     :   An output object for windows that can display raw images for testing
Created     :   March 13, 2014
Authors     :   Dean Beeler

Copyright   :   Copyright 2014 Oculus VR, Inc. All Rights reserved.

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
#include "../../Include/OVR_Kernel.h"

#include "Util_ImageWindow.h"

#if defined(OVR_OS_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "DWrite.h"

typedef HRESULT (WINAPI *D2D1CreateFactoryFn)(
	_In_      D2D1_FACTORY_TYPE,
	_In_      REFIID,
	_In_opt_  const D2D1_FACTORY_OPTIONS*,
	_Out_     ID2D1Factory **
	);

typedef HRESULT (WINAPI *DWriteCreateFactoryFn)(
	_In_   DWRITE_FACTORY_TYPE factoryType,
	_In_   REFIID iid,
	_Out_  IUnknown **factory
	);


namespace OVR { namespace Util {
	
ID2D1Factory* ImageWindow::pD2DFactory = NULL;
IDWriteFactory* ImageWindow::pDWriteFactory = NULL;


// TODO(review): This appears to be (at present) necessary, the global list is accessed by the
// render loop in Samples.  In the current version, windows will just be lost when windowCount
// exceeds MaxWindows; I've left that in place, since this is unfamiliar code. I'm not sure what
// thread-safety guarantees this portion of the code needs to satisfy, so I don't want to
// change it to a list or whatever.  Asserts added to catch the error.
ImageWindow* ImageWindow::globalWindow[ImageWindow::MaxWindows];
int ImageWindow::windowCount = 0;

LRESULT CALLBACK MainWndProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg) 
	{ 
	case WM_CREATE: 
		return 0; 

	case WM_PAINT: 
		{
			LONG_PTR ptr = GetWindowLongPtr( hwnd, GWLP_USERDATA );
			if( ptr )
			{
				ImageWindow* iw = (ImageWindow*)ptr;
				iw->OnPaint();
			}
		}
		
		return 0; 

	case WM_SIZE: 
		// Set the size and position of the window. 
		return 0; 

	case WM_DESTROY: 
		// Clean up window-specific data objects. 
		return 0; 

		// 
		// Process other messages. 
		// 

	default: 
		return DefWindowProc(hwnd, uMsg, wParam, lParam); 
	} 
	//return 0; 
}

ImageWindow::ImageWindow( uint32_t width, uint32_t height ) :
	frontBufferMutex( new Mutex() )
{

	HINSTANCE hInst = LoadLibrary( L"d2d1.dll" );
	HINSTANCE hInstWrite = LoadLibrary( L"Dwrite.dll" );

	D2D1CreateFactoryFn createFactory = NULL;
	DWriteCreateFactoryFn writeFactory = NULL;

	if( hInst )
	{
		createFactory = (D2D1CreateFactoryFn)GetProcAddress( hInst, "D2D1CreateFactory" );
	}

	if( hInstWrite )
	{
		writeFactory = (DWriteCreateFactoryFn)GetProcAddress( hInstWrite, "DWriteCreateFactory" );
	}

    // TODO: see note where globalWindow is declared.
	globalWindow[windowCount++ % MaxWindows] = this;
    OVR_ASSERT(windowCount < MaxWindows);

	if( pD2DFactory == NULL && createFactory && writeFactory )
	{
		createFactory( 
			D2D1_FACTORY_TYPE_MULTI_THREADED,
			__uuidof(ID2D1Factory),
			NULL,
			&pD2DFactory
			);

		// Create a DirectWrite factory.
		writeFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pDWriteFactory),
			reinterpret_cast<IUnknown **>(&pDWriteFactory)
			);

	}

	resolution = D2D1::SizeU( width, height );

	SetWindowLongPtr( hWindow, GWLP_USERDATA, (LONG_PTR)this );

	pRT = NULL;
	greyBitmap = NULL;
	colorBitmap = NULL;
}

ImageWindow::~ImageWindow()
{
	for( int i = 0; i < MaxWindows; ++i )
	{
		if( globalWindow[i] == this )
		{
			globalWindow[i] = NULL;
			break;
		}
    }

	if( greyBitmap )
		greyBitmap->Release();

	if( colorBitmap )
		colorBitmap->Release();

	if( pRT )
		pRT->Release();

	{
		Mutex::Locker locker( frontBufferMutex  );

		while( frames.GetSize() )
		{
			Ptr<Frame> aFrame = frames.PopBack();
		}
	}

	delete frontBufferMutex;

	ShowWindow( hWindow, SW_HIDE );
	DestroyWindow( hWindow );
}

void ImageWindow::AssociateSurface( void* surface )
{
	// Assume an IUnknown
	IUnknown* unknown = (IUnknown*)surface;

	IDXGISurface *pDxgiSurface = NULL;
	HRESULT hr = unknown->QueryInterface(&pDxgiSurface);
	if( hr == S_OK )
	{
		D2D1_RENDER_TARGET_PROPERTIES props =
			D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96,
			96
			);


		pRT = NULL;			
		ID2D1RenderTarget* tmpTarget;

		hr = pD2DFactory->CreateDxgiSurfaceRenderTarget( pDxgiSurface, &props, &tmpTarget );

		if( hr == S_OK )
		{
			DXGI_SURFACE_DESC desc = {0};
			pDxgiSurface->GetDesc( &desc );
			int width = desc.Width;
			int height = desc.Height;

			D2D1_SIZE_U size = D2D1::SizeU( width, height );

			D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(
				DXGI_FORMAT_A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED
				);

			D2D1_PIXEL_FORMAT colorPixelFormat = D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED
				);

			D2D1_BITMAP_PROPERTIES bitmapProps;
			bitmapProps.dpiX = 96;
			bitmapProps.dpiY = 96;
			bitmapProps.pixelFormat = pixelFormat;

			D2D1_BITMAP_PROPERTIES colorBitmapProps;
			colorBitmapProps.dpiX = 96;
			colorBitmapProps.dpiY = 96;
			colorBitmapProps.pixelFormat = colorPixelFormat;

			HRESULT result = tmpTarget->CreateBitmap( size, bitmapProps, &greyBitmap );
			if( result != S_OK )
			{
				tmpTarget->Release();
				tmpTarget = NULL;
			}

			if (tmpTarget)
			{
				result = tmpTarget->CreateBitmap(size, colorBitmapProps, &colorBitmap);
				if (result != S_OK)
				{
					tmpTarget->Release();
					tmpTarget = NULL;
				}
			}
			pRT = tmpTarget;
		}
	}
}

void ImageWindow::Process()
{
	if( pRT && greyBitmap )
	{
		OnPaint();

		pRT->Flush();
	}
}

void ImageWindow::Complete()
{
	Mutex::Locker locker( frontBufferMutex  );

	if( frames.IsEmpty() )
		return;

	if( frames.PeekBack(0)->ready )
		return;

	Ptr<Frame> frame = frames.PeekBack(0);

	frame->ready = true;
}

void ImageWindow::OnPaint()
{
	Mutex::Locker locker( frontBufferMutex  );

	// Nothing to do
	if( frames.IsEmpty() )
		return;

	if( !frames.PeekFront(0)->ready )
		return;

	Ptr<Frame> currentFrame = frames.PopFront();

	Ptr<Frame> nextFrame = NULL;

	if( !frames.IsEmpty() )
		nextFrame = frames.PeekFront(0);
	
	while( nextFrame && nextFrame->ready )
	{
		// Free up the current frame since it's been removed from the deque
		currentFrame = frames.PopFront();

		if( frames.IsEmpty() )
			break;

		nextFrame = frames.PeekFront(0);
	}

	if( currentFrame->imageData )
		greyBitmap->CopyFromMemory( NULL, currentFrame->imageData, currentFrame->width );

	if( currentFrame->colorImageData )
		colorBitmap->CopyFromMemory( NULL, currentFrame->colorImageData, currentFrame->colorPitch );

	pRT->BeginDraw();

	pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED); 

	pRT->Clear( D2D1::ColorF(D2D1::ColorF::Black) );

	// This will mirror our image
	D2D1_MATRIX_3X2_F m;
	m._11 = -1; m._12 = 0;
	m._21 = 0; m._22 = 1;
	m._31 = 0; m._32 = 0;
	pRT->SetTransform( m );

	ID2D1SolidColorBrush* whiteBrush;

	pRT->CreateSolidColorBrush( D2D1::ColorF(D2D1::ColorF::White, 1.0f), &whiteBrush );

	if( currentFrame->imageData )
	{
		pRT->FillOpacityMask( greyBitmap, whiteBrush, 
			D2D1_OPACITY_MASK_CONTENT_TEXT_NATURAL, 
			D2D1::RectF( -(FLOAT)resolution.width, 0.0f, (FLOAT)0.0f, (FLOAT)resolution.height ), 
			//D2D1::RectF( 0.0f, 0.0f, (FLOAT)0.0f, (FLOAT)resolution.height ), 
			D2D1::RectF( 0.0f, 0.0f, (FLOAT)resolution.width, (FLOAT)resolution.height ) );
	}
	else if( currentFrame->colorImageData )
	{
		pRT->DrawBitmap( colorBitmap,
			D2D1::RectF( -(FLOAT)resolution.width, 0.0f, (FLOAT)0.0f, (FLOAT)resolution.height ) );

	}

	pRT->SetTransform(D2D1::Matrix3x2F::Identity());

	whiteBrush->Release();

	Array<CirclePlot>::Iterator it;

	for( it = currentFrame->plots.Begin(); it != currentFrame->plots.End(); ++it )
	{
		ID2D1SolidColorBrush* aBrush;

		pRT->CreateSolidColorBrush( D2D1::ColorF( it->r, it->g, it->b), &aBrush );

		D2D1_ELLIPSE ellipse;
		ellipse.point.x = it->x;
		ellipse.point.y = it->y;
		ellipse.radiusX = it->radius;
		ellipse.radiusY = it->radius;

		if( it->fill )
			pRT->FillEllipse( &ellipse, aBrush );
		else
			pRT->DrawEllipse( &ellipse, aBrush );

		aBrush->Release();
	}

	static const WCHAR msc_fontName[] = L"Verdana";
	static const FLOAT msc_fontSize = 20;

	IDWriteTextFormat* textFormat = NULL;

	// Create a DirectWrite text format object.
	pDWriteFactory->CreateTextFormat(
		msc_fontName,
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		msc_fontSize,
		L"", //locale
		&textFormat
		);

	D2D1_SIZE_F renderTargetSize = pRT->GetSize();

	Array<TextPlot>::Iterator textIt;
	for( textIt = currentFrame->textLines.Begin(); textIt != currentFrame->textLines.End(); ++textIt )
	{
		ID2D1SolidColorBrush* aBrush;

		pRT->CreateSolidColorBrush( D2D1::ColorF( textIt->r, textIt->g, textIt->b), &aBrush );

		WCHAR* tmpString = (WCHAR*)calloc( textIt->text.GetLength(),  sizeof( WCHAR ) );
		for( unsigned i = 0; i < textIt->text.GetLength(); ++i )
		{
			tmpString[i] = (WCHAR)textIt->text.GetCharAt( i );
		}
					
		pRT->DrawTextW( tmpString, (UINT32)textIt->text.GetLength(), textFormat,
			D2D1::RectF(textIt->x, textIt->y, renderTargetSize.width, renderTargetSize.height), aBrush );

		free( tmpString );

		aBrush->Release();
	}

	if( textFormat )
		textFormat->Release();

	pRT->EndDraw();

	pRT->Flush();
}

Ptr<Frame> ImageWindow::lastUnreadyFrame()
{
	static int framenumber = 0;

	if( frames.GetSize() && !frames.PeekBack( 0 )->ready )
		return frames.PeekBack( 0 );

	// Create a new frame if an unready one doesn't already exist
	Ptr<Frame> tmpFrame = *new Frame( framenumber );
	frames.PushBack( tmpFrame );

	++framenumber;

	return tmpFrame;
}

void ImageWindow::UpdateImageBW( const uint8_t* imageData, uint32_t width, uint32_t height )
{
	if( pRT && greyBitmap )
	{
		Mutex::Locker locker( frontBufferMutex );

		Ptr<Frame> frame = lastUnreadyFrame();
		frame->imageData = malloc( width * height );
		frame->width = width;
		frame->height = height;
		memcpy( frame->imageData, imageData, width * height );
	}
}

void ImageWindow::UpdateImageRGBA( const uint8_t* imageData, uint32_t width, uint32_t height, uint32_t pitch )
{
	if( pRT && colorBitmap )
	{
		Mutex::Locker locker( frontBufferMutex );

		Ptr<Frame> frame = lastUnreadyFrame();
		frame->colorImageData = malloc( pitch * height );
		frame->width = width;
		frame->height = height;
		frame->colorPitch = pitch;
		memcpy( frame->colorImageData, imageData, pitch * height );
	}
}

void ImageWindow::addCircle( float x, float y, float radius, float r, float g, float b, bool fill )
{
	if( pRT )
	{
		CirclePlot cp;

		cp.x = x;
		cp.y = y;
		cp.radius = radius;
		cp.r = r;
		cp.g = g;
		cp.b = b;
		cp.fill = fill;

		Mutex::Locker locker( frontBufferMutex );

		Ptr<Frame> frame = lastUnreadyFrame();
		frame->plots.PushBack( cp );
	}

}

void ImageWindow::addText( float x, float y, float r, float g, float b, OVR::String text )
{
	if( pRT )
	{
		TextPlot tp;

		tp.x = x;
		tp.y = y;
		tp.r = r;
		tp.g = g;
		tp.b = b;
		tp.text = text;

		Mutex::Locker locker( frontBufferMutex );
		Ptr<Frame> frame = lastUnreadyFrame();
		frame->textLines.PushBack( tp );
	}
}

}}

#else //defined(OVR_OS_WIN32)

namespace OVR { namespace Util {

ImageWindow* ImageWindow::globalWindow[4];
int ImageWindow::windowCount = 0;

}}

#endif //#else //defined(OVR_OS_WIN32)

