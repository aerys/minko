//============================================================//
// File:		beSurface.cpp
// 
// Date:		10-19-2000
//============================================================//
#include "../header/Render2DCore.h"

//============================================================//
// beScreen()
//============================================================//
beSurface::beSurface(beScreen *pScreen)
{
	m_pScreen = pScreen;
	m_pSurface = NULL;
}

//============================================================//
// ~beScreen()
//============================================================//
beSurface::~beSurface()
{
	m_pScreen = NULL;
	beSafeRelease( m_pSurface );
}

//============================================================//
// Release()
//============================================================//
void beSurface::Release()
{
	beSafeRelease( m_pSurface );
}

//============================================================//
// Lock()
//============================================================//
bool beSurface::Lock(DDSURFACEDESC2& ddsd)
{
	if (!m_pSurface)
		return false;

    dxutil_InitStructure(ddsd);
    m_pSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	return true;
}

//============================================================//
// Unlock()
//============================================================//
bool beSurface::Unlock()
{
    if (m_pSurface == NULL) return false;
	
	m_pSurface->Unlock(NULL);

	return true;
}

//============================================================//
// CreateSurface()
//============================================================//
bool beSurface::CreateSurface(int iWidth, int iHeight, bool bSystemMemory)
{
	HRESULT result;

	// release old surface
	beSafeRelease(m_pSurface);

	// get DirectDraw interface
	if (m_pScreen == NULL) return false;

	beDirectDraw * p_dd = m_pScreen->GetDirectDraw();
	if (p_dd == NULL) return false;

	// create surface
	DDSURFACEDESC2 ddsd;
	dxutil_InitStructure(ddsd);
	ddsd.dwFlags            = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps     = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth            = iWidth;
	ddsd.dwHeight           = iHeight;
	if (bSystemMemory) ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	result = p_dd->CreateSurface(&ddsd, &m_pSurface, NULL);
	if (!FAILED(result))
		return true;

	return false;
}

//============================================================//
// LoadImage()
//============================================================//
bool beSurface::LoadImage(HBITMAP image, bool bSystemMemory)
{
    HRESULT result;
	HBITMAP oldBitmap;

    // release old surface
    beSafeRelease(m_pSurface);

	// get DirectDraw interface
	if (m_pScreen == NULL) return false;

	beDirectDraw * p_dd = m_pScreen->GetDirectDraw();
	if (p_dd == NULL) return false;

    HDC             srcDC  = NULL;
    HDC             destDC = NULL;

    if (image == NULL)
		goto RELEASE;

    // get bitmap description
    BITMAP bmpDesc;
    ::GetObject(image, sizeof(bmpDesc), &bmpDesc);

    // create surface
    if (!CreateSurface(bmpDesc.bmWidth, bmpDesc.bmHeight, bSystemMemory))
		goto RELEASE;
    
    // copy image into surface
    result = m_pSurface->GetDC(&destDC);
    if (FAILED(result)) goto RELEASE;

    // create GDI DC
    srcDC = CreateCompatibleDC(NULL);
    if (srcDC == NULL) goto RELEASE;

    // blit image into surface
    oldBitmap = (HBITMAP) SelectObject(srcDC, image);
    BitBlt(destDC, 0, 0, bmpDesc.bmWidth, bmpDesc.bmHeight, srcDC, 0, 0, SRCCOPY);
    SelectObject(srcDC, oldBitmap);

	m_iWidth = bmpDesc.bmWidth;
	m_iHeight = bmpDesc.bmHeight;

    // delete GDI DC
    DeleteDC(srcDC);
    srcDC = NULL;

    // release surface DC
    m_pSurface->ReleaseDC(destDC);
    destDC = NULL;

    // delete DIB section
    //::DeleteObject(image);

	return true;

RELEASE:
    if (m_pSurface != NULL)
    {
            if (destDC != NULL) m_pSurface->ReleaseDC(destDC);
            beSafeRelease(m_pSurface);
    }

    if (srcDC != NULL) ::DeleteDC(srcDC);

    if (image != NULL) ::DeleteObject(image);

	return false;
}

//============================================================//
// BltFast()
//============================================================//
bool beSurface::BltFast(DWORD dwX,DWORD dwY,beSurface* pSurface,LPRECT lpSrcRect,DWORD dwTrans)
{
	if (lpSrcRect!=NULL)
	{
		// check the bounds
		if (dwX + lpSrcRect->right > (DWORD)m_iWidth)
			lpSrcRect->right = m_iWidth - dwX;

		if (dwY + lpSrcRect->bottom > (DWORD)m_iHeight)
			lpSrcRect->bottom = m_iHeight - dwY;
	}

	if (!FAILED(m_pSurface->BltFast(dwX,dwY,pSurface->GetSurface(),lpSrcRect,dwTrans)))
		return true;

	return false;
}

//============================================================//
// Blt()
//============================================================//
bool beSurface::Blt(LPRECT lpDestRect,beSurface* pSurface,LPRECT lpSrcRect,DWORD dwFlags,LPDDBLTFX lpDDBltFx)
{
	if (!FAILED(m_pSurface->Blt(lpDestRect,pSurface->GetSurface(),lpSrcRect,dwFlags,lpDDBltFx)))
		return true;

	return false;
}

//============================================================//
// PlotPixel32()
//============================================================//
void beSurface::PlotPixel32( int x, int y, int r, int g, int b, DDSURFACEDESC2 *ddsd)
{
/*	DWORD  *destPixel = (DWORD *)(((BYTE *)ddsd->lpSurface) + ddsd->lPitch * y);

	destPixel[x]  = (((r)) & ddsd->ddpfPixelFormat.dwRBitMask);
    destPixel[x] |= (((g)) & ddsd->ddpfPixelFormat.dwGBitMask);
    destPixel[x] |= (((b)) & ddsd->ddpfPixelFormat.dwBBitMask);*/

}

//============================================================//
// PlotPixel16()
//============================================================//
void beSurface::PlotPixel16( int x, int y, int r, int g, int b, DDSURFACEDESC2 *ddsd)
{
	WORD  *destPixel = (WORD *)(((BYTE *)ddsd->lpSurface) + ddsd->lPitch * y);

	/*destPixel[x]  = (((r)) & ddsd->ddpfPixelFormat.dwRBitMask);
    destPixel[x] |= (((g)) & ddsd->ddpfPixelFormat.dwGBitMask);
    destPixel[x] |= (((b)) & ddsd->ddpfPixelFormat.dwBBitMask);
*/
	destPixel[x]  = (r << (BYTE)m_pScreen->GetRPos()) | (g << (BYTE)m_pScreen->GetGPos()) | b ;

}

//============================================================// 
// PlotPixel()
//============================================================//
void beSurface::PlotPixel( int x, int y, int r, int g, int b, DDSURFACEDESC2 *ddsd)
{
/*	switch(ddsd->ddpfPixelFormat.dwRGBBitCount)
	{
		case 16:
			PlotPixel16(x,y,r,g,b,ddsd);
			break;
		case 32:
			PlotPixel32(x,y,r,g,b,ddsd);
			break;
	};*/
	DWORD Offset = y * ddsd->lPitch + x * (ddsd->ddpfPixelFormat.dwRGBBitCount >> 3);
	DWORD Pixel;
	Pixel = *((LPDWORD)((DWORD)ddsd->lpSurface+Offset));
    Pixel = (Pixel & ~ddsd->ddpfPixelFormat.dwRBitMask) |
        ((r >> (8-m_pScreen->GetRBits())) << m_pScreen->GetRPos());
    Pixel = (Pixel & ~ddsd->ddpfPixelFormat.dwGBitMask) |
        ((g >> (8-m_pScreen->GetGBits())) << m_pScreen->GetGPos());
    Pixel = (Pixel & ~ddsd->ddpfPixelFormat.dwBBitMask) |
        ((b >> (8-m_pScreen->GetBBits())) << m_pScreen->GetBPos());
    *((LPDWORD)((DWORD)ddsd->lpSurface+Offset)) = Pixel;

}
