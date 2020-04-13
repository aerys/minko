//============================================================//
// File:		beScreen.cpp
// 
// Date:		10-19-2000
//============================================================//
#include "../header/Render2DCore.h"

//============================================================//
// beScreen()
//============================================================//
beScreen::beScreen()
{
	m_pDDraw			= NULL;
	m_pddsBackBuffer	= NULL;
	m_pddsPrimary		= NULL;
}

//============================================================//
// ~beScreen()
//============================================================//
beScreen::~beScreen()
{
	beSafeRelease(m_pddsPrimary);
	beSafeRelease(m_pDDraw);
}

//============================================================//
// Initialize()
//============================================================//
bool beScreen::Initialize(beScreenParams sparam)
{
	// setup class
	m_ScreenParams = sparam;

    // Create the main DirectDraw object
    HRESULT hRet = DirectDrawCreateEx(NULL, (VOID**)&m_pDDraw, IID_IDirectDraw7, NULL);
    if (FAILED(hRet))
        return false;

	// Initialize surfaces
	if (!InitializeSurfaces())
		return false;	

	InitializeMask();

	return true;
}

//============================================================//
// Delete()
//============================================================//
void beScreen::Delete()
{
    if (m_pDDraw != NULL)
    {
        m_pDDraw->SetCooperativeLevel(m_ScreenParams.m_OwnerWnd, DDSCL_NORMAL);
        if (m_pddsBackBuffer != NULL)
        {
            m_pddsBackBuffer->Release();
            m_pddsBackBuffer = NULL;
        }
        if (m_pddsPrimary != NULL)
        {
            m_pddsPrimary->Release();
            m_pddsPrimary = NULL;
        }
    }
    return;
}

//============================================================//
// InitializeSurfaces()
//============================================================//
bool beScreen::InitializeSurfaces()
{
    HRESULT		        hRet;
    DDSURFACEDESC2      ddsd;
    DDSCAPS2            ddscaps;
    LPDIRECTDRAWCLIPPER pClipper;

    if ( m_ScreenParams.m_Flags & BE_SCREENFLAGS_WINDOWED )
    {
        // Get normal windowed mode
        hRet = m_pDDraw->SetCooperativeLevel(m_ScreenParams.m_OwnerWnd, DDSCL_NORMAL);
        if (hRet != DD_OK)
            return false;

    	// Get the dimensions of the viewport and screen bounds
    	GetClientRect(m_ScreenParams.m_OwnerWnd, &m_rcViewport);
    	GetClientRect(m_ScreenParams.m_OwnerWnd, &m_rcScreen);
    	ClientToScreen(m_ScreenParams.m_OwnerWnd, (POINT*)&m_rcScreen.left);
    	ClientToScreen(m_ScreenParams.m_OwnerWnd, (POINT*)&m_rcScreen.right);

        // Create the primary surface
        ZeroMemory(&ddsd,sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        hRet = m_pDDraw->CreateSurface(&ddsd, &this->m_pddsPrimary, NULL);
        if (hRet != DD_OK)
            return false;

        // Create a clipper object since this is for a Windowed render
        hRet = m_pDDraw->CreateClipper(0, &pClipper, NULL);
        if (hRet != DD_OK)
            return false;

        // Associate the clipper with the window
        
		pClipper->SetHWnd(0, m_ScreenParams.m_OwnerWnd);
        m_pddsPrimary->SetClipper(pClipper);
        pClipper->Release();
        pClipper = NULL;

        // Get the backbuffer. For fullscreen mode, the backbuffer was created
        // along with the primary, but windowed mode still needs to create one.
        ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        ddsd.dwWidth        = m_ScreenParams.m_iScreenWidth;
        ddsd.dwHeight       = m_ScreenParams.m_iScreenHeight;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        hRet = m_pDDraw->CreateSurface(&ddsd, &m_pddsBackBuffer, NULL);
        if (hRet != DD_OK)
            return false;

	//	MessageBox(NULL,"AAAAAAAAH","ASH",MB_OK);
	} else {

        // Get exclusive mode
        hRet = m_pDDraw->SetCooperativeLevel(m_ScreenParams.m_OwnerWnd, DDSCL_EXCLUSIVE |
                                                DDSCL_FULLSCREEN);
        if (hRet != DD_OK)
            return false;

        // Set the video mode 
        hRet = m_pDDraw->SetDisplayMode( m_ScreenParams.m_iScreenWidth, m_ScreenParams.m_iScreenHeight, m_ScreenParams.m_iScreenBitCount, 0, 0);
        if (hRet != DD_OK)
            return false;

    	// Get the dimensions of the viewport and screen bounds
    	// Store the rectangle which contains the renderer
    	SetRect(&m_rcViewport, 0, 0, m_ScreenParams.m_iScreenWidth, m_ScreenParams.m_iScreenHeight );
    	memcpy(&m_rcScreen, &m_rcViewport, sizeof(RECT) );

        // Create the primary surface with 1 back buffer
        ZeroMemory(&ddsd,sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags = DDSD_CAPS |
                       DDSD_BACKBUFFERCOUNT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                			  DDSCAPS_FLIP |
                			  DDSCAPS_COMPLEX;
        ddsd.dwBackBufferCount = 1;
        hRet = m_pDDraw->CreateSurface( &ddsd, &m_pddsPrimary, NULL);
        if (hRet != DD_OK)
            return false;

        ZeroMemory(&ddscaps, sizeof(ddscaps));
        ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
        hRet = m_pddsPrimary->GetAttachedSurface(&ddscaps, &m_pddsBackBuffer);
        if (hRet != DD_OK)
            return false;
	}

	return true;
}

//============================================================//
// InitializeMask()
//============================================================//
WORD GetNumberOfBits( DWORD dwMask )
{
    WORD wBits = 0;
    while( dwMask )
    {
        dwMask = dwMask & ( dwMask - 1 );
        wBits++;
    }
    return wBits;
}

WORD GetMaskPos( DWORD dwMask )
{
    WORD wPos = 0;
    //while( !(dwMask & (1 << wPos)) ) wPos++;

	while(!(dwMask & 1))
	{
		dwMask >>= 1;
		wPos++;
	}

    return wPos;
}


void beScreen::InitializeMask()
{
    DDPIXELFORMAT ddpf;
    ZeroMemory(&ddpf, sizeof(ddpf));
    ddpf.dwSize = sizeof(ddpf);
    this->GetPrimarySurface()->GetPixelFormat(&ddpf);

	m_wRBits	= GetNumberOfBits(ddpf.dwRBitMask);
	m_wGBits	= GetNumberOfBits(ddpf.dwGBitMask);
	m_wBBits	= GetNumberOfBits(ddpf.dwBBitMask);
	m_wRPos		= GetMaskPos(ddpf.dwRBitMask);
	m_wGPos		= GetMaskPos(ddpf.dwGBitMask);
	m_wBPos		= GetMaskPos(ddpf.dwBBitMask);
}

//============================================================//
// Flip()
//============================================================//
bool beScreen::Flip()
{
    HRESULT result;

    if ( m_ScreenParams.m_Flags & BE_SCREENFLAGS_WINDOWED )
    {
		result = m_pddsPrimary->Blt(&m_rcScreen, m_pddsBackBuffer,
                              NULL, DDBLT_WAIT,
                              NULL);
	} else 
		result = m_pddsPrimary->Flip(NULL, DDFLIP_WAIT);

    if (result == DDERR_SURFACELOST) result = m_pddsPrimary->Restore();
    if (FAILED(result)) return false;

    // success
    return true;
}

//============================================================//
// Move()
//============================================================//
void beScreen::Move()
{
//    GetWindowRect(m_ScreenParams.m_OwnerWnd , &m_rcWindow);
    GetClientRect(m_ScreenParams.m_OwnerWnd , &m_rcViewport);
    GetClientRect(m_ScreenParams.m_OwnerWnd , &m_rcScreen);
    ClientToScreen(m_ScreenParams.m_OwnerWnd , (POINT*)&m_rcScreen.left);
    ClientToScreen(m_ScreenParams.m_OwnerWnd , (POINT*)&m_rcScreen.right);
}
