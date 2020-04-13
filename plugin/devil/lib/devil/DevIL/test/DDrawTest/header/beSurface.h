//============================================================//
// File:		beScreen.h
// 
// Date:		10-19-2000
//============================================================//
#ifndef BESURFACE_H
#define BESURFACE_H

class beSurface
{

public:

	beSurface(beScreen *pScreen);
	~beSurface();

	void Release();

    bool Lock(DDSURFACEDESC2& ddsd);
    bool Unlock();

    bool CreateSurface(int iWidth, int iHeight, bool bSystemMemory);
	bool LoadImage(HBITMAP image, bool bSystemMemory);

	beDirectDrawSurface *			GetSurface()		{ return m_pSurface; };
	int								GetWidth()			{ return m_iWidth; };
	int								GetHeight()			{ return m_iHeight; };

	bool BltFast(DWORD dwX,DWORD dwY,beSurface* pSurface,LPRECT lpSrcRect,DWORD dwTrans);
	bool Blt(LPRECT lpDestRect,beSurface* pSurface,LPRECT lpSrcRect,DWORD dwFlags,LPDDBLTFX lpDDBltFx);

	void PlotPixel( int x, int y, int r, int g, int b, DDSURFACEDESC2 *ddsd);

protected:
	void PlotPixel16( int x, int y, int r, int g, int b, DDSURFACEDESC2 *ddsd);
	void PlotPixel32( int x, int y, int r, int g, int b, DDSURFACEDESC2 *ddsd);

private:
	int								m_iWidth;
	int								m_iHeight;

	beScreen *						m_pScreen;
	beDirectDrawSurface *			m_pSurface;
	

};

#endif