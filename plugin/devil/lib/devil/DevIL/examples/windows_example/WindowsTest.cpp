//--------------------------------------------------------------------------------
//
// ImageLib Windows (GDI) Test Source
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 02/14/2009
//
// Filename: testil/windowstest/windowstest.c
//
// Description: Full GDI test application for DevIL.
//
// Note:  This requires FluidStudio's Colour Picker library to compile
//			properly (available at http://www.fluidstudios.com/publications.html).
//
//--------------------------------------------------------------------------------

#ifdef  _DEBUG
#define IL_DEBUG
#endif//_DEBUG

#define _UNICODE

// Memory leak detection
#ifdef _DEBUG 
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

//#define ILUT_USE_WIN32
#include <windows.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#ifdef ILUT_USE_SDL
	#include <sdl.h>
#endif
#include "resource.h"
#include <stdlib.h>
#include <wchar.h>

//#pragma comment(lib, "sdl.lib")
//#pragma comment(lib, "sdlmain.lib")
//#pragma comment(lib, "colorpicker.lib")


// Evil globals!
HINSTANCE hInstance;
HBITMAP hBitmap;
BITMAPINFOHEADER BmpInfo;
HDC hDC = 0, hMemDC = 0;
HWND HWnd;
HBRUSH BackBrush;

#define BORDER_W	16 //8
#define MENU_H		58 //54
#define MIN_W		450  // Accomodate the menu bar.
//#define MAX_W		400
//#define MAX_H		400
#define TITLE		L"DevIL Windows Test"
ILuint	NumUndosAllowed = 4, UndoSize = 0;
ILuint	Undos[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
ILuint	Width, Height, Depth, Size;  // Main image
ILint	CurImage;
TCHAR	CurFileName[2048];

ILint	XOff, YOff;

ILdouble last_elapsed, cur_elapsed, elapsed;

ILuint	FilterType;
ILuint	FilterParamInt;
ILfloat	FilterParamFloat;
TCHAR	FilterEditString[255];
TCHAR	OpenFileName[2048];
TCHAR	SaveFileName[2048];
TCHAR	NewTitle[512];

TCHAR *ExtList[] = {
	L"pix", L"cut", L"dcx", L"gif", L"mdl", L"lif", L"jpe", L"jpg", L"jpeg", L"lif", L"bmp",
	L"ico", L"pbm", L"pgm", L"ppm", L"png", L"bw", L"rgb", L"rgba", L"sgi", L"tga", L"tif",
	L"tiff", L"xpm", L"psp", L"psd", L"iwi", L"exr", L"blp", L"tpl", L"wdp", L"pcx", L"dcm",
	L"rot", L"iwi", L"ftx", L"dds", L"dpx", L"vtf",
	NULL
};


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR APIENTRY AboutDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR APIENTRY PropertiesDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR APIENTRY FilterDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR APIENTRY ResizeDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR APIENTRY BatchDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void BatchConv(TCHAR *Directory, TCHAR *ExtList, TCHAR *ConvExt, bool Recurse);
void GenFilterString(TCHAR *Out, TCHAR **Strings);
void ResizeWin(void);
void CreateGDI(void);
bool IsOpenable(TCHAR *FileName);
bool GetPrevImage(void);
bool GetNextImage(void);
TCHAR *GetExtension(const TCHAR *FileName);
bool CheckExtension(TCHAR *Arg, TCHAR *Ext);

//extern "C"
//// Colour picker export
//__declspec( dllimport ) bool WINAPI FSColorPickerDoModal(unsigned int * currentColor, const bool currentColorIsDefault, unsigned int * originalColor, const bool originalColorIsDefault, const int initialExpansionState);


ILAPI ILubyte* ILAPIENTRY ilNVidiaCompressDXT(ILubyte *Data, ILuint Width, ILuint Height, ILenum DxtType);


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG			msg;
	WNDCLASSEX	wcex;
	HACCEL		hAccelTable;

	hInstance = hInst;
	memset(OpenFileName, 0, 2048 * sizeof(TCHAR));

	BackBrush = CreateSolidBrush(RGB(128,128,128));

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= BackBrush;
	wcex.lpszMenuName	= (LPCWSTR)IDR_MENU1; //@TODO: (LPCSTR)IDR_MENU1;
	wcex.lpszClassName	= TITLE;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);

	RegisterClassEx(&wcex);

	HWnd = CreateWindow(TITLE, TITLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						50, 50, 400, 300, NULL, NULL, hInstance, NULL);
	if (HWnd == NULL)
		return FALSE;

	//if (SDL_Init(SDL_INIT_TIMER) < 0)
	//	return FALSE;
	//atexit(SDL_Quit);

	// Display the window
	ShowWindow(HWnd, nCmdShow);
	UpdateWindow(HWnd);

	// Initialize DevIL
	ilInit();
	iluInit();
	ilutRenderer(ILUT_WIN32);

	// Is there a file to load from the command-line?
	if (__argc > 1) {
		ilGenImages(1, Undos);
		ilBindImage(Undos[0]);
		/*if (ilLoadImage(__argv[1])) {
			CurImage = 0;
			//ilConvertImage(IL_BGRA);
			ilutRenderer(ILUT_WIN32);
			ResizeWin();
			CreateGDI();
			sprintf(NewTitle, "%s - %s", TITLE, __argv[1]);
			SetWindowText(HWnd, NewTitle);
		}*/
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_MENU1);

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return (int)msg.wParam;
}


void CreateGDI()
{
	ILuint CopyName, CurName, CurImg, CurMip;

	hDC = GetDC(HWnd);
	hMemDC = CreateCompatibleDC(hDC);
	CurName = ilGetInteger(IL_CUR_IMAGE);
	CurImg = ilGetInteger(IL_ACTIVE_IMAGE);
	CurMip = ilGetInteger(IL_ACTIVE_MIPMAP);
	CopyName = ilCloneCurImage();
	ilBindImage(CopyName);
	//ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
	hBitmap = ilutConvertToHBitmap(hDC);
	ilutGetBmpInfo((BITMAPINFO*)&BmpInfo);
	DeleteObject(SelectObject(hMemDC, hBitmap));
	ilBindImage(CurName);
	if (CurImg)
		ilActiveImage(CurImg);//ilBindImage(Undos[0]);
	if (CurMip)
		ilActiveMipmap(CurMip);
	ilDeleteImages(1, &CopyName);

	return;
}


void DestroyGDI()
{
	if (hMemDC) {
		DeleteObject(hBitmap);
		DeleteDC(hMemDC);
	}
	if (hDC) {
		ReleaseDC(HWnd, hDC);
	}
	hBitmap = NULL;
	hMemDC = NULL;
	hDC = NULL;

	return;
}


void ResizeWin()
{
	static RECT Rect1, Rect2;
	static ILint NewW, NewH;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &Rect1, 0);

	GetWindowRect(HWnd, &Rect2);

	Width = ilGetInteger(IL_IMAGE_WIDTH);
	Height = ilGetInteger(IL_IMAGE_HEIGHT);
	Depth = ilGetInteger(IL_IMAGE_DEPTH);

	NewW = Width < MIN_W ? MIN_W : Width + BORDER_W;
	if (NewW + Rect2.left > Rect1.right)
		NewW = Rect1.right - Rect2.left;
	NewH = Height + MENU_H;
	if (NewH + Rect2.top > Rect1.bottom)
		NewH = Rect1.bottom - Rect2.top;

	SetWindowPos(HWnd, HWND_TOP, Rect2.left, Rect2.top, NewW, NewH, SWP_SHOWWINDOW);
	InvalidateRect(HWnd, NULL, FALSE);

	return;
}


// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HMENU		hMenu;
	static ILuint		Colours;
	static RECT			Rect;
	static PAINTSTRUCT	ps;
    static HDROP		hDrop;

	static TCHAR OpenFilter[2048];
	static TCHAR SaveFilter[2048];
	static TCHAR *OFilter[] = {
		L"All Files (*.*)", L"*.*",
		L"Alias|Wavefront Files (*.pix)", L"*.pix",
		L"Cut Files (*.cut)", L"*.cut",
		L"Dcx Files (*.dcx)", L"*.dcx",
		L"Graphics Interchange Format (*.gif)", L"*.gif",
		L"Half-Life Model Files (*.mdl)", L"*.mdl",
		L"Homeworld Image Files (*.lif)", L"*.lif",
		L"Image Files (All Supported Types)", L"*.jpe;*.jpg;*.jpeg;*.lif;*.bmp;*.ico;*.pbm;*.pgm;*.pnm;*.ppm;*.png;*.bw;*.rgb;*.rgba;*.sgi;*.tga;*.tif;*.tiff;*.pcx;*.xpm;*.psp;*.psd;*.pix;*.pxr;*.cut;*.dcx",
		L"Jpeg Files (*.jpe, *.jpg, *.jpeg)", L"*.jpe;*.jpg;*.jpeg",
		L"Kodak Photo CD Files (*.pcd)", L"*.pcd",
		L"Microsoft Bitmap Files (*.bmp)", L"*.bmp",
		L"Microsoft DirectDraw Surface (*.dds)", L"*.dds",
		L"Microsoft Icon Files (*.ico, *.cur)", L"*.ico, *.cur",
		L"Multiple Network Graphics Files (*.mng)", L"*.mng",
		L"Paint Shop Pro Files (*.psp)", L"*.psp",
		L"PhotoShop Files (*.psd)", L"*.psd",
		L"Pic Files (*.pic)", L"*.pic",
		L"Pixar Files (*.pix)", L"*.pix",
		L"Portable AnyMap Files (*.pbm, *.pgm, *.pnm, *.ppm)", L"*.pbm;*.pgm;*.pnm;*.ppm",
		L"Portable Network Graphics Files (*.png)", L"*.png",
		L"Sgi Files (*.sgi)", L"*.bw;*.rgb;*.rgba;*.sgi",
		L"Targa Files (*.tga, *.vda, *.icb, *.vst)", L"*.tga;*.vda;*.icb;*.vst",
		L"Tiff Files (*.tif)", L"*.tif;*.tiff",
		L"Valve Texture Files (*.vtf)", L"*.vtf",
		L"Quake Wal Files (*.wal)", L"*.wal",
		L"X PixelMap (*.xpm)", L"*.xpm",
		L"ZSoft Pcx Files (*.pcx)", L"*.pcx",
		L"\0\0"
	};
	static TCHAR *SFilter[] = {
		L"All Files (*.*)", L"*.*",
		L"C-Style Header (*.h)", L"*.h",
		L"Jpeg Files (*.jpe, *.jpg, *.jpeg)", L"*.jpe;*.jpg;*.jpeg",
		L"Microsoft Bitmap Files (*.bmp)", L"*.bmp",
		L"Microsoft DirectDraw Surface (*.dds)", L"*.dds",
		L"PhotoShop Files (*.psd)", L"*.psd",
		L"Portable AnyMap Files (*.pbm, *.pgm, *.ppm)", L"*.pbm;*.pgm;*.ppm",
		L"Portable Network Graphics Files (*.png)", L"*.png",
		L"Sgi Files (*.sgi)", L"*.bw;*.rgb;*.rgba;*.sgi",
		L"Targa Files (*.tga)", L"*.tga",
		L"Tiff Files (*.tif)", L"*.tif",
		L"ZSoft Pcx Files (*.pcx)", L"*.pcx",
		L"\0\0"
	};
	static OPENFILENAME Ofn = {
		sizeof(OPENFILENAME),
		hWnd,
		NULL,
		OpenFilter,
		NULL,
		0,
		0,
		OpenFileName,
		2048,
		NULL,
		0,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		0,
		0,
		NULL,
		NULL,
		NULL,
		NULL
	};

	POINT  CurMouse;
	static POINT		PrevMouse;
	static ILboolean	MouseDown = IL_FALSE;

	static RECT			WinSize;

	unsigned int		currentColor = 0x80000000;
	unsigned int		originalColor = 0x80000000;
	bool				userClickedOK;
	ILclampf			Red = 255, Green = 255, Blue = 255;
	ILubyte				*AlphaChannel;
	ILenum				Origin;

	switch (message)
	{
		case WM_CREATE:
			GenFilterString(OpenFilter, OFilter);
			GenFilterString(SaveFilter, SFilter);

			hDC = GetDC(hWnd);
			DragAcceptFiles(hWnd, TRUE);

			ReleaseDC(hWnd, hDC);
			break;

		case WM_CLOSE:

#ifdef _DEBUG
		_CrtDumpMemoryLeaks();
#endif
			DestroyGDI();
			DestroyWindow(hWnd);
			UnregisterClass(TITLE, hInstance);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_PAINT:
			GetWindowRect(HWnd, &WinSize);  // Shouldn't be here!
			hDC = BeginPaint(hWnd, &ps);
			//StretchBlt(hDC, 0, 0, WinSize.right - WinSize.left,
			//	WinSize.bottom - WinSize.top, BackHDC, 0, 0, 1, 1, SRCCOPY);
			WinSize.right -= WinSize.left;
			WinSize.bottom -= WinSize.top;
			WinSize.top = 0;
			WinSize.left = 0;
			FillRect(hDC, &WinSize, BackBrush);

            BitBlt(hDC, XOff, YOff, (WORD)BmpInfo.biWidth, (WORD)BmpInfo.biHeight, 
				  hMemDC, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
			ValidateRect(hWnd, NULL);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);

			// View the next image in the animation chain.
			if (wParam == VK_RIGHT) {
				ilBindImage(Undos[0]);  // @TODO: Implement undos better with this.
				CurImage++;
				if (CurImage > ilGetInteger(IL_NUM_IMAGES))
					CurImage = 0;  // Go back to the beginning of the animation.
				ilActiveImage(CurImage);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}

			if (wParam == '0') {
				ilBindImage(Undos[0]);  // @TODO: Implement undos better with this.
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}

			if (wParam == '1') {
				ilActiveMipmap(1);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}
			else if (wParam == '2') {
				ilActiveMipmap(2);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}
			else if (wParam == '3') {
				ilActiveMipmap(3);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}
			else if (wParam == '4') {
				ilActiveMipmap(4);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}
			else if (wParam == '5') {
				ilActiveMipmap(5);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}
			else if (wParam == '6') {
				ilActiveMipmap(6);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}
			else if (wParam == '7') {
				ilActiveMipmap(7);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}
			else if (wParam == '8') {
				ilActiveMipmap(8);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}
			else if (wParam == '9') {
				ilActiveMipmap(9);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}

			// View the previous image in the animation chain.
			if (wParam == VK_LEFT) {
				ilBindImage(Undos[0]);  // @TODO: Implement undos better with this.
				CurImage--;
				if (CurImage < 0)
					CurImage = 0;
				ilActiveImage(CurImage);
				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();
			}

			if (wParam == VK_PRIOR) {
				if (!GetPrevImage())
					break;

				DestroyGDI();
				if (UndoSize == 0)
					UndoSize = 1;
				ilDeleteImages(UndoSize, Undos);
				UndoSize = 0;
				XOff = 0;
				YOff = 0;

				ilGenImages(1, Undos);
				ilBindImage(Undos[0]);

				//last_elapsed = SDL_GetTicks();
				if (!ilLoadImage(OpenFileName)) {
					wsprintf(CurFileName, L"%s", OpenFileName);
					wsprintf(NewTitle, L"%s - Could not open %s", TITLE, OpenFileName);
					SetWindowText(hWnd, NewTitle);
					return (0L);
				}
				CurImage = 0;
				//cur_elapsed = SDL_GetTicks();
				elapsed = cur_elapsed - last_elapsed;
				last_elapsed = cur_elapsed;

				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();

				wsprintf(CurFileName, L"%s", OpenFileName);
				wsprintf(NewTitle, L"%s - %s:  %u ms", TITLE, OpenFileName, (unsigned int)elapsed);
				SetWindowText(hWnd, NewTitle);
			}

			if (wParam == VK_NEXT) {
				if (!GetNextImage())
					break;

				DestroyGDI();
				if (UndoSize == 0)
					UndoSize = 1;
				ilDeleteImages(UndoSize, Undos);
				UndoSize = 0;
				XOff = 0;
				YOff = 0;

				ilGenImages(1, Undos);
				ilBindImage(Undos[0]);

				//last_elapsed = SDL_GetTicks();
				if (!ilLoadImage(OpenFileName)) {
					wsprintf(CurFileName, L"%s", OpenFileName);
					wsprintf(NewTitle, L"%s - Could not open %s", TITLE, OpenFileName);
					SetWindowText(hWnd, NewTitle);
					return (0L);
				}
				CurImage = 0;
				//cur_elapsed = SDL_GetTicks();
				elapsed = cur_elapsed - last_elapsed;
				last_elapsed = cur_elapsed;

				ilutRenderer(ILUT_WIN32);
				ResizeWin();
				CreateGDI();

				wsprintf(CurFileName, L"%s", OpenFileName);
				wsprintf(NewTitle, L"%s - %s:  %u ms", TITLE, OpenFileName, (unsigned int)elapsed);
				SetWindowText(hWnd, NewTitle);
			}

			InvalidateRect(hWnd, NULL, FALSE);
			break;

		// Moves the "viewport"
		case WM_MOUSEMOVE:
			if (!MouseDown)
				break;
			GetCursorPos(&CurMouse);

			XOff += CurMouse.x - PrevMouse.x;
			YOff += CurMouse.y - PrevMouse.y;

			PrevMouse.x = CurMouse.x;
			PrevMouse.y = CurMouse.y;
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case WM_LBUTTONDOWN:
			MouseDown = IL_TRUE;
			GetCursorPos(&PrevMouse);
			break;

		case WM_LBUTTONUP:
			MouseDown = IL_FALSE;
			break;

		case WM_DROPFILES:
			hDrop = (HDROP)wParam;
			DragQueryFile(hDrop, 0, OpenFileName, 512);

			DestroyGDI();
			ilDeleteImages(UndoSize, Undos);
			UndoSize = 0;

			ilGenImages(1, Undos);
			ilBindImage(Undos[0]);
			ilLoadImage(OpenFileName);
			CurImage = 0;

			ilutRenderer(ILUT_WIN32);
			ResizeWin();
			CreateGDI();

			wsprintf(CurFileName, L"%s", OpenFileName);
			wsprintf(NewTitle, L"%s - %s", TITLE, OpenFileName);
			SetWindowText(hWnd, NewTitle);

			DragFinish(hDrop);
			return 0;

		case WM_COMMAND:
			FilterType = LOWORD(wParam);
	
			switch (LOWORD(wParam))
			{
				case ID_FILE_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					return (0L);

				case ID_HELP_ABOUT:
					DialogBox (hInstance,
						MAKEINTRESOURCE(IDD_DIALOG_ABOUT),
						hWnd,
						AboutDlgProc);
					return (0L);

				case ID_FILE_PROPERTIES:
					DialogBox (hInstance,
						MAKEINTRESOURCE(IDD_DIALOG_PROPERTIES),
						hWnd,
						PropertiesDlgProc);
					return (0L);

				case ID_BATCHCONVERT:
					DialogBox (hInstance,
						MAKEINTRESOURCE(IDD_DIALOG_BATCHCONV),
						hWnd,
						BatchDlgProc);
					return (0L);

				case ID_EFFECTS_COUNTCOLORS:
					Colours = iluColoursUsed();
					TCHAR ColourString[255];
					wsprintf(ColourString, L"The number of colours in this image is:  %d", Colours);
					MessageBox(NULL, ColourString, L"Colour Count", MB_OK);
					return (0L);

				case ID_EFFECTSTOOLS_BACKGROUNDCOLOUR:
					//userClickedOK = FSColorPickerDoModal(&currentColor, true, &originalColor, true, 0);
					userClickedOK = 0;

					if (userClickedOK) {
						Red = (ILfloat)((currentColor & 0xff0000) >> 16) / 255.0f;
						Green = (ILfloat)((currentColor & 0xff00) >> 8) / 255.0f;
						Blue = (ILfloat)(currentColor & 0xff) / 255.0f;

						ilClearColour(Red, Green, Blue, 1.0f);
					}

					return (0L);

				case ID_EDIT_COPY:
					ilutSetWinClipboard();
					return (0L);

				case ID_EDIT_PASTE:
					ILuint Test;
					ilGenImages(1, &Test);
					ilBindImage(Test);

					// Check if there's anything in the clipboard.
					if (!ilutGetWinClipboard()) {
						ilDeleteImages(1, &Test);
						return (0L);
					}
					ilDeleteImages(1, &Test);

					DestroyGDI();
					ilDeleteImages(UndoSize, Undos);
					UndoSize = 0;
					XOff = 0;
					YOff = 0;

					ilGenImages(1, Undos);
					ilBindImage(Undos[0]);
					ilutGetWinClipboard();

					wsprintf(CurFileName, L"Clipboard Paste");
					wsprintf(NewTitle, L"%s - Pasted from the Clipboard", TITLE);
					SetWindowText(hWnd, NewTitle);

					//ilConvertImage(IL_BGRA);
					ilutRenderer(ILUT_WIN32);
					ResizeWin();
					CreateGDI();
					return (0L);

				// @TODO:  Will probably fail if no image loaded!
				case ID_FILE_PRINT:
					/*PRINTDLG	Pd;
					DOCINFO		Di;
					//HDC			PrintDC;
					//HBITMAP		PrintReplace;

					memset(&Pd, 0, sizeof(PRINTDLG));
					Pd.lStructSize = sizeof(PRINTDLG);
					Pd.hwndOwner = hWnd;
					Pd.Flags = PD_RETURNDC;
					Pd.nCopies = 1;
					Pd.nFromPage = 0xFFFF;
					Pd.nToPage = 0xFFFF;
					Pd.nMinPage = 1;
					Pd.nMaxPage = 0xFFFF;

					if (!PrintDlg(&Pd))
						return (0L);

					Di.cbSize = sizeof(DOCINFO);
					Di.lpszDocName = L"DevIL Printing Test";
					Di.lpszOutput = NULL;
					Di.lpszDatatype = NULL;
					Di.fwType = 0;

					StartDoc(Pd.hDC, &Di);
					StartPage(Pd.hDC);

					//PrintDC = CreateCompatibleDC(Pd.hDC);
					//PrintReplace = (HBITMAP)SelectObject(PrintDC, hBitmap);
					StretchBlt(Pd.hDC, 0, 0, Width * 2, Height * 2, hMemDC, 0, 0, Width, Height, SRCCOPY);

					EndPage(Pd.hDC);
					EndDoc(Pd.hDC);
					//DeleteObject(PrintReplace);
					//DeleteDC(PrintDC);
					DeleteDC(Pd.hDC);*/

					ilutWinPrint(0, 0, ilGetInteger(IL_IMAGE_WIDTH) * 2, ilGetInteger(IL_IMAGE_HEIGHT) * 2, hDC);

					return (0L);

				case ID_FILE_LOAD:
					wsprintf(OpenFileName, L"*.*");
					Ofn.lpstrFilter = OpenFilter;
					Ofn.lpstrFile = OpenFileName;
					Ofn.lpstrTitle = L"Open File";
					Ofn.nFilterIndex = 1;
					Ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

					if (!GetOpenFileName(&Ofn))
						return (0L);

					DestroyGDI();
					if (UndoSize == 0)
						UndoSize = 1;
					ilDeleteImages(UndoSize, Undos);
					UndoSize = 0;
					XOff = 0;
					YOff = 0;

					ilGenImages(1, Undos);
					ilBindImage(Undos[0]);

					//last_elapsed = SDL_GetTicks();
					if (!ilLoadImage(OpenFileName))
						return (0L);
					CurImage = 0;
					//cur_elapsed = SDL_GetTicks();
					elapsed = cur_elapsed - last_elapsed;
					last_elapsed = cur_elapsed;

					//iluBuildMipmaps();

					//ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
					//ilEnable(IL_NVIDIA_COMPRESS);
					//ilEnable(IL_SQUISH_COMPRESS);
					//ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
					//free(ilCompressDXT(ilGetData(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_DXT5, &Size));
					//free(ilNVidiaCompressDXT(ilGetData(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_DXT5));

					ilutRenderer(ILUT_WIN32);
					ResizeWin();
					CreateGDI();

					wsprintf(CurFileName, L"%s", OpenFileName);
					wsprintf(NewTitle, L"%s - %s:  %u ms", TITLE, OpenFileName, (unsigned int)elapsed);
					SetWindowText(hWnd, NewTitle);

					return (0L);

				case ID_FILE_OPENURL:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) != TRUE) {
						return (0L);
					}

					DestroyGDI();
					ilDeleteImages(UndoSize, Undos);
					UndoSize = 0;
					XOff = 0;
					YOff = 0;

					ilGenImages(1, Undos);
					ilBindImage(Undos[0]);
					/*if (!ilutWinLoadUrl(FilterEditString))
						return (0L);*/

					ilutRenderer(ILUT_WIN32);
					ResizeWin();
					CreateGDI();
					
					wsprintf(NewTitle, L"%s - %s", TITLE, FilterEditString);
					SetWindowText(hWnd, NewTitle);

					return (0L);

				case ID_FILE_SAVE:
					wsprintf(SaveFileName, L"monkey.tga");
					Ofn.lpstrFilter = SaveFilter;
					Ofn.lpstrFile = SaveFileName;
					Ofn.lpstrTitle = L"Save File";
					Ofn.nFilterIndex = 1;
					Ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

					if (!GetSaveFileName(&Ofn))
						return (0L);

					ilEnable(IL_FILE_OVERWRITE);
					//ilBindImage(Undos[0]);  //@TODO: Do better here...

					//last_elapsed = SDL_GetTicks();
					ilSaveImage(SaveFileName);

					//cur_elapsed = SDL_GetTicks();
					elapsed = cur_elapsed - last_elapsed;
					last_elapsed = cur_elapsed;

					wsprintf(CurFileName, L"%s", SaveFileName);
					wsprintf(NewTitle, L"%s - %s:  %u ms", TITLE, SaveFileName, (unsigned int)elapsed);
					SetWindowText(hWnd, NewTitle);

					return (0L);

				case ID_EDIT_UNDOLEVEL:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						NumUndosAllowed = FilterParamInt <= 10 ? FilterParamInt : 10;
					}
					return (0L);

				case ID_EDIT_UNDO:
					if (UndoSize && NumUndosAllowed) {
						ilDeleteImages(1, &Undos[UndoSize]);
						ilBindImage(Undos[--UndoSize]);
						ResizeWin();
						CreateGDI();
					}
					return (0L);

				case ID_EDIT_VIEWIMAGENUM:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						ilBindImage(Undos[0]);  // @TODO: Implement undos better with this.
						ilActiveImage(FilterParamInt);
						ilutRenderer(ILUT_WIN32);
						ResizeWin();
						CreateGDI();
					}
					return (0L);

				case ID_EDIT_VIEWFACE:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						ilActiveFace(FilterParamInt);
						ilutRenderer(ILUT_WIN32);
						ResizeWin();
						CreateGDI();
					}
					return (0L);

				case ID_EDIT_VIEWMIPMAP:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						ilActiveMipmap(FilterParamInt);
						ilutRenderer(ILUT_WIN32);
						ResizeWin();
						CreateGDI();
					}
					return (0L);

				case ID_EDIT_NEXT:
					ilBindImage(Undos[0]);  // @TODO: Implement undos better with this.
					CurImage++;
					ilActiveImage(CurImage);
					ilutRenderer(ILUT_WIN32);
					ResizeWin();
					CreateGDI();
					return (0L);

				case ID_EDIT_PREV:
					ilBindImage(Undos[0]);  // @TODO: Implement undos better with this.
					CurImage--;
					ilActiveImage(CurImage);
					ilutRenderer(ILUT_WIN32);
					ResizeWin();
					CreateGDI();
					return (0L);
			}


			if (++UndoSize > NumUndosAllowed) {
				if (NumUndosAllowed > 0) {
					UndoSize = NumUndosAllowed;
					ilDeleteImages(1, &Undos[0]);
					memcpy(Undos, Undos+1, NumUndosAllowed * sizeof(ILuint));
					ilBindImage(Undos[UndoSize]);
				}
			}

			if (NumUndosAllowed > 0) {
				ilGetIntegerv(IL_ACTIVE_IMAGE, (ILint*)&Undos[UndoSize]);
				/*ilGenImages(1, &Undos[UndoSize]);
				ilBindImage(Undos[UndoSize]);
				ilCopyImage(Undos[UndoSize-1]);*/
				Undos[UndoSize] = ilCloneCurImage();
				ilBindImage(Undos[UndoSize]);
			}

			DestroyGDI();
			switch (LOWORD(wParam))
			{
				case ID_CONVERT_PALETTE:
					ilConvertImage(IL_COLOUR_INDEX, IL_UNSIGNED_BYTE);
					break;

				case ID_CONVERT_RGB:
					ilConvertImage(IL_RGB, ilGetInteger(IL_IMAGE_TYPE));
					break;

				case ID_CONVERT_RGBA:
					ilConvertImage(IL_RGBA, ilGetInteger(IL_IMAGE_TYPE));
					break;

				case ID_CONVERT_BGR:
					ilConvertImage(IL_BGR, ilGetInteger(IL_IMAGE_TYPE));
					break;

				case ID_CONVERT_BGRA:
					ilConvertImage(IL_BGRA, ilGetInteger(IL_IMAGE_TYPE));
					break;

				case ID_CONVERT_LUMINANCE:
					ilConvertImage(IL_LUMINANCE, ilGetInteger(IL_IMAGE_TYPE));
					break;

				case ID_CONVERT_LUMINANCEALPHA:
					ilConvertImage(IL_LUMINANCE_ALPHA, ilGetInteger(IL_IMAGE_TYPE));
					break;

				case ID_EDIT_VIEWALPHA:
					Origin = ilGetInteger(IL_ORIGIN_MODE);
					AlphaChannel = ilGetAlpha(IL_UNSIGNED_BYTE);
					ilTexImage(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
						ilGetInteger(IL_IMAGE_DEPTH), 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, AlphaChannel);
					free(AlphaChannel);
					ilRegisterOrigin(Origin);
					break;

				case ID_EFFECTS_FLIP:
					iluFlipImage();
					break;

				case ID_EFFECTS_MIRROR:
					iluMirror();
					break;

				case ID_FILTER_EMBOSS:
					iluEmboss();
					break;

				case ID_FILTER_EQUALIZE:
					iluEqualize();
					break;

				case ID_FILTER_ALIENIFY:
					iluAlienify();
					break;

				case ID_FILTER_NEGATIVE:
					iluNegative();
					break;

				case ID_EFFECTS_FILTERS_EDGEDETECT_EMBOSS:
					iluEdgeDetectE();
					break;

				case ID_EFFECTS_FILTERS_EDGEDETECT_SOBEL:
					iluEdgeDetectS();
					break;

				case ID_EFFECTS_FILTERS_EDGEDETECT_PREWITT:
					iluEdgeDetectP();
					break;

				case ID_FILTER_NOISE:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluNoisify(FilterParamFloat);
					}
					break;

				case ID_EFFECTS_FILTERS_WAVE:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluWave(FilterParamFloat);
					}
					break;

				case ID_FILTER_PIXELIZE:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluPixelize(FilterParamInt);
					}
					break;

				case ID_FILTERS_BLUR_AVERAGE:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluBlurAvg(FilterParamInt);
					}
					break;

				case ID_FILTERS_BLUR_GAUSSIAN:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluBlurGaussian(FilterParamInt);
						/*iluMatrixMode(ILU_CONVOLUTION_MATRIX);
						iluLoadFilter(ILU_FILTER_GAUSSIAN_5X5);
						iluApplyMatrix();*/
					}
					break;

				case ID_FILTER_GAMMACORRECT:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluGammaCorrect(FilterParamFloat);
					}
					break;

				case ID_FILTER_SHARPEN:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluSharpen(FilterParamFloat, 1);
					}
					break;

				case ID_EFFECTS_FILTERS_ROTATE:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluRotate(FilterParamFloat);
						ResizeWin();
					}
					break;

				case ID_EFFECTS_FILTERS_SCALE:
					HWnd = hWnd;
					iluImageParameter(ILU_FILTER, ILU_BILINEAR);
					DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_RESIZE), hWnd, ResizeDlgProc);
					break;

			}

			CreateGDI();
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		default:
		  return (DefWindowProc(hWnd, message, wParam, lParam));
	}

    return (0L);
}


void GenFilterString(TCHAR *Out, TCHAR **Strings)
{
	int OutPos = 0, StringPos = 0;

	while (Strings[StringPos][0] != 0) {
		wsprintf(Out + OutPos, Strings[StringPos]);
		OutPos += (int)wcslen(Strings[StringPos++]) + 1;
	}

	Out[OutPos++] = 0;
	Out[OutPos] = 0;

	return;
}


INT_PTR APIENTRY AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
	{
	    case WM_INITDIALOG:
		{
			int i;
			ILenum ilError;
			TCHAR VersionNum[256];

			wsprintf(VersionNum, L"Num:  %d", ilGetInteger(IL_VERSION_NUM));

			SetDlgItemText(hDlg, IDC_ABOUT_VENDOR, ilGetString(IL_VENDOR));
			SetDlgItemText(hDlg, IDC_ABOUT_VER_STRING, ilGetString(IL_VERSION_NUM));
			SetDlgItemText(hDlg, IDC_ABOUT_VER_NUM, VersionNum);

			for (i = 0; i < 6; i++) {
				ilError = ilGetError();
				if (ilError == IL_NO_ERROR)
					break;
				SetDlgItemText(hDlg, IDC_ERROR1+i, iluErrorString(ilError));
			}

			return (TRUE);
		}
		break;

	    case WM_COMMAND:      
		{
			if (LOWORD(wParam) == IDOK)
				EndDialog(hDlg, TRUE);
			if (LOWORD(wParam) == IDCANCEL)
				EndDialog(hDlg, FALSE);
	    }
		break;

		case WM_CLOSE:
			EndDialog(hDlg, TRUE);
			break;
	}

	return FALSE;
}


INT_PTR APIENTRY PropertiesDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
	{
	    case WM_INITDIALOG:
		{
			TCHAR Temp[256];

			SetDlgItemText(hDlg, IDC_PROP_FILENAME, CurFileName);
			wsprintf(Temp, L"%d", ilGetInteger(IL_IMAGE_WIDTH));
			SetDlgItemText(hDlg, IDC_PROP_WIDTH, Temp);
			wsprintf(Temp, L"%d", ilGetInteger(IL_IMAGE_HEIGHT));
			SetDlgItemText(hDlg, IDC_PROP_HEIGHT, Temp);
			wsprintf(Temp, L"%d", ilGetInteger(IL_IMAGE_DEPTH));
			SetDlgItemText(hDlg, IDC_PROP_DEPTH, Temp);
			wsprintf(Temp, L"%d", ilGetInteger(IL_IMAGE_SIZE_OF_DATA));
			SetDlgItemText(hDlg, IDC_PROP_SIZE, Temp);

			return (TRUE);
		}
		break;

	    case WM_COMMAND:      
		{
			if (LOWORD(wParam) == IDOK)
				EndDialog(hDlg, TRUE);
			if (LOWORD(wParam) == IDCANCEL)
				EndDialog(hDlg, FALSE);
	    }
		break;

		case WM_CLOSE:
			EndDialog(hDlg, TRUE);
			break;
	}

	return FALSE;
}


INT_PTR APIENTRY FilterDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
	{
	    case WM_INITDIALOG:
		{
			switch (FilterType)
			{
				case ID_FILTER_PIXELIZE:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Width of pixelized block:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"1");
					break;
				case ID_FILTER_NOISE:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Amount of noise threshold:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"1.0");
					break;
				case ID_EFFECTS_FILTERS_WAVE:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Angle of wave to apply:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"0.0");
					break;
				case ID_FILTERS_BLUR_AVERAGE:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Number of iterations:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"1");
					break;
				case ID_FILTERS_BLUR_GAUSSIAN:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Number of iterations:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"1");
					break;
				case ID_FILTER_GAMMACORRECT:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Amount of gamma correction:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"1.0");
					break;
				case ID_FILTER_SHARPEN:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Sharpening factor:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"1.0");
					break;
				case ID_EFFECTS_FILTERS_ROTATE:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Number of degress to rotate:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"0.0");
					break;

				case ID_EDIT_UNDOLEVEL:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Set level of undo:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"4");
					break;
				case ID_EDIT_VIEWIMAGENUM:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Enter image number:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"0");
					break;
				case ID_EDIT_VIEWMIPMAP:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Enter mipmap number:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"0");
					break;
				case ID_FILE_OPENURL:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, L"Enter url of image:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, L"");
					break;
			}

			return TRUE;
		}
		break;

	    case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK) {
				GetDlgItemText(hDlg, IDC_FILTER_EDIT, FilterEditString, 255);
				FilterParamInt = _wtoi(FilterEditString);
				FilterParamFloat = (float)_wtof(FilterEditString);
				EndDialog(hDlg, TRUE);
			}
			if (LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, FALSE);
			}
	    }
		break;

		case WM_CLOSE:
			EndDialog(hDlg, TRUE);
			break;
	}

	return FALSE;
}


INT_PTR APIENTRY ResizeDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TCHAR x[255], y[255], z[255];
	static ILuint xsize, ysize, zsize;
	static RECT Rect;

    switch (message)
	{
	    case WM_INITDIALOG:
		{
			wsprintf(x, L"%d", Width);
			wsprintf(y, L"%d", Height);
			wsprintf(z, L"%d", Depth);
			SetDlgItemText(hDlg, IDC_EDIT_RESIZE_X, x);
			SetDlgItemText(hDlg, IDC_EDIT_RESIZE_Y, y);
			SetDlgItemText(hDlg, IDC_EDIT_RESIZE_Z, z);
			return TRUE;
		}
		break;

	    case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK) {
				GetDlgItemText(hDlg, IDC_EDIT_RESIZE_X, x, 255);
				GetDlgItemText(hDlg, IDC_EDIT_RESIZE_Y, y, 255);
				GetDlgItemText(hDlg, IDC_EDIT_RESIZE_Z, z, 255);
				xsize = _wtoi(x);
				ysize = _wtoi(y);
				zsize = _wtoi(z);
				if (xsize && ysize && zsize) {
					iluScale(xsize, ysize, zsize);

					Width = ilGetInteger(IL_IMAGE_WIDTH);
					Height = ilGetInteger(IL_IMAGE_HEIGHT);
					Depth = ilGetInteger(IL_IMAGE_DEPTH);

					GetWindowRect(HWnd, &Rect);
					SetWindowPos(HWnd, HWND_TOP, Rect.left, Rect.top,
						Width < MIN_W ? MIN_W + BORDER_W : Width + BORDER_W,
						Height + MENU_H, SWP_SHOWWINDOW);

					InvalidateRect(HWnd, NULL, FALSE);
				}
				EndDialog(hDlg, TRUE);
			}
			if (LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, FALSE);
			}
	    }
		break;

		case WM_CLOSE:
			EndDialog(hDlg, TRUE);
			break;
	}

	return FALSE;
}


INT_PTR APIENTRY BatchDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TCHAR	Dir[255], NewExt[255];
	static bool		Recurse;
	static RECT		Rect;

    switch (message)
	{
	    case WM_INITDIALOG:
		{
			wsprintf(Dir, L"");
			wsprintf(NewExt, L"tga");
			SetDlgItemText(hDlg, IDC_BATCH_DIR, Dir);
			SetDlgItemText(hDlg, IDC_BATCH_NEWEXT, NewExt);
			return TRUE;
		}
		break;

	    case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK) {
				GetDlgItemText(hDlg, IDC_BATCH_DIR, Dir, 255);
				GetDlgItemText(hDlg, IDC_BATCH_NEWEXT, NewExt, 255);
				Recurse = IsDlgButtonChecked(hDlg, IDC_BATCH_CHECK1) == BST_CHECKED;

				// Do shit here.

				//BatchConv(Dir, NULL, NewExt, Recurse);

				EndDialog(hDlg, TRUE);
			}
			if (LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, FALSE);
			}
	    }
		break;

		case WM_CLOSE:
			EndDialog(hDlg, TRUE);
			break;
	}

	return FALSE;
}


bool GetPrevImage()
{
	HANDLE			Search;
	WIN32_FIND_DATA	FindData;
	int				i = 0, j, Total = 0, FileNamePos = -1;
	TCHAR			*Ext, *CurName;

	Search = FindFirstFile(L"*.*", &FindData);
	CurName = wcsrchr(CurFileName, '\\');
	if (CurName == NULL) {
		CurName = CurFileName;
	}
	else {
		CurName++;  // Skip the '\'
	}

	do {
		if (!_wcsicmp(FindData.cFileName, L".") || !_wcsicmp(FindData.cFileName, L".."))
			continue;
		Ext = GetExtension(FindData.cFileName);
		if (Ext == NULL)
			continue;
		for (j = 0; ExtList[j] != NULL; j++) {
			if (CheckExtension(FindData.cFileName, ExtList[j])) {
				if (!_wcsicmp(FindData.cFileName, CurName))
					FileNamePos = Total;
				Total++;
				break;
			}
		}
	} while (FindNextFile(Search, &FindData));

	FindClose(Search);
	Search = FindFirstFile(L"*.*", &FindData);

	if (Total == 0 || FileNamePos == -1)
		return false;

	if (FileNamePos == 0)
		FileNamePos = Total;

	do {
		if (!_wcsicmp(FindData.cFileName, L".") || !_wcsicmp(FindData.cFileName, L".."))
			continue;
		Ext = GetExtension(FindData.cFileName);
		if (Ext == NULL)
			continue;
		for (j = 0; ExtList[j] != NULL; j++) {
			if (CheckExtension(FindData.cFileName, ExtList[j])) {
				if (FileNamePos == i+1) {
					wcscpy(OpenFileName, FindData.cFileName);
					i++;
					break;
				}
				i++;
				break;
			}
		}
	} while (FindNextFile(Search, &FindData));


	FindClose(Search);
	return true;
}


bool GetNextImage()
{
	HANDLE			Search;
	WIN32_FIND_DATA	FindData;
	int				i = 0, j, Total = 0, FileNamePos = -1;
	TCHAR			*Ext, *CurName;

	Search = FindFirstFile(L"*.*", &FindData);
	CurName = wcsrchr(CurFileName, '\\');
	if (CurName == NULL) {
		CurName = CurFileName;
	}
	else {
		CurName++;  // Skip the '\'
	}

	do {
		if (!_wcsicmp(FindData.cFileName, L".") || !_wcsicmp(FindData.cFileName, L".."))
			continue;
		Ext = GetExtension(FindData.cFileName);
		if (Ext == NULL)
			continue;
		for (j = 0; ExtList[j] != NULL; j++) {
			if (CheckExtension(FindData.cFileName, ExtList[j])) {
				if (!_wcsicmp(FindData.cFileName, CurName))
					FileNamePos = Total;
				Total++;
				break;
			}
		}
	} while (FindNextFile(Search, &FindData));

	FindClose(Search);
	Search = FindFirstFile(L"*.*", &FindData);

	if (Total == 0 || FileNamePos == -1)
		return false;

	if (FileNamePos == Total-1)
		FileNamePos = -1;

	do {
		if (!_wcsicmp(FindData.cFileName, L".") || !_wcsicmp(FindData.cFileName, L".."))
			continue;
		Ext = GetExtension(FindData.cFileName);
		if (Ext == NULL)
			continue;
		for (j = 0; ExtList[j] != NULL; j++) {
			if (CheckExtension(FindData.cFileName, ExtList[j])) {
				if (FileNamePos == i-1) {
					wcscpy(OpenFileName, FindData.cFileName);
					i++;
					break;
				}
				i++;
				break;
			}
		}
	} while (FindNextFile(Search, &FindData));


	FindClose(Search);
	return true;
}



