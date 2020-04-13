//============================================================//
// File:		beScreen.h
// 
// Date:		10-19-2000
//============================================================//
#ifndef BESCREEN_H
#define BESCREEN_H

#define BE_SCREENFLAGS_FULLSCREEN               0x00000000
#define BE_SCREENFLAGS_WINDOWED                 0x00000001
#define BE_SCREENFLAGS_BACKBUFFERS              0x00000002

struct beScreenParams
{
        long			m_Flags;                // BE_SCREENFLAGS_xxxx

        unsigned int	m_iScreenWidth;          // x-resolution
        unsigned int	m_iScreenHeight;         // y-resolution
        unsigned int	m_iScreenBitCount;       // color resolution (ignored in windowed mode)

        unsigned int	m_iBackBufferCount;      // number of back-buffers (max 1 in windowed mode)

        HWND            m_OwnerWnd;              // handle of owner window

        long			m_sclFlags;              // additional cooperative flags
};

class beScreen
{

public:

	beScreen();
	~beScreen();

	bool Initialize(beScreenParams sparam);
	void Delete();

	bool Flip();

	void Move();

	beDirectDrawSurface *			GetPrimarySurface()		{ return m_pddsPrimary; };
	beDirectDrawSurface *			GetBackBufferSurface()	{ return m_pddsBackBuffer; };
	beDirectDraw *					GetDirectDraw()			{ return m_pDDraw; };
	beScreenParams					GetScreenParams()		{ return m_ScreenParams; };
	
	RECT							GetScreenRect()			{ return m_rcScreen; };
	RECT							GetViewportRect()		{ return m_rcViewport; };
	
	// Pixel-Plotting
	WORD							GetRBits()				{ return m_wRBits; };
	WORD							GetGBits()				{ return m_wGBits; };
	WORD							GetBBits()				{ return m_wBBits; };
	WORD							GetRPos()				{ return m_wRPos; };
	WORD							GetGPos()				{ return m_wGPos; };
	WORD							GetBPos()				{ return m_wBPos; };

protected:

	bool InitializeSurfaces();
	void InitializeMask();

private:

	RECT							m_rcViewport;           // Pos. & size to blt from
	RECT							m_rcScreen;             // Screen pos. for blt

    beDirectDraw *                  m_pDDraw;
    beDirectDrawSurface *			m_pddsPrimary;
    beDirectDrawSurface *			m_pddsBackBuffer;

	beScreenParams					m_ScreenParams;

	// Pixel-Plotting
	WORD							m_wRBits;
	WORD							m_wGBits;
	WORD							m_wBBits;
	WORD							m_wRPos;
	WORD							m_wGPos;
	WORD							m_wBPos;
};

#endif