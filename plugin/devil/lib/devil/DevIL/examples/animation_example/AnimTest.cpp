//-----------------------------------------------------------------------------
//
// ImageLib Windows (GDI) Test Source
// Copyright (C) 2000 by Denton Woods
// Last modified:  08/26/2001 <--Y2K Compliant! =]
//
// Filename: testil/animtest/animtest.c
//
// Description:  Animation test application for DevIL.
//
//-----------------------------------------------------------------------------

#include <windows.h>
#ifdef _DEBUG
#define IL_DEBUG
#endif
#include <il/il.h>
#include <il/ilu.h>
#include <il/ilut.h>
#include "resource.h"


// Evil globals!
HINSTANCE hInstance;
HDC hDC, hMemDC = NULL;
HWND HWnd;

#define	BORDER_W	8
#define	MENU_H		46
#define	MIN_W		205  // Accomodate the menu bar.
#define	MAX_W		400
#define	MAX_H		400
#define	TITLE		"DevIL Animation Test"

ILuint	FilterType;
ILuint	FilterParamInt;
ILfloat	FilterParamFloat;
char	FilterEditString[255];
char	NewTitle[512];

BITMAPINFOHEADER *BmpInfo = NULL;
HBITMAP	*Bitmaps = NULL;
ILuint	*Durations = NULL;
ILuint	NumImages = 0, CurImage = 0;

__int64	StartTime, TimerFreq;
double	TimerRes;
bool	IsPaused = false;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void GenFilterString(char *Out, char **Strings);
void DisplayImage(void);
void LoadImages(char *FileName);


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG			msg;
	WNDCLASSEX	wcex;
	HACCEL		hAccelTable;

	hInstance = hInst;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDR_MENU1;
	wcex.lpszClassName	= TITLE;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);

	RegisterClassEx(&wcex);

	HWnd = CreateWindow(TITLE, TITLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						50, 50, 400, 300, NULL, NULL, hInstance, NULL);
	if (HWnd == NULL)
		return FALSE;

	// Display the window
	ShowWindow(HWnd, nCmdShow);
	UpdateWindow(HWnd);

	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilEnable(IL_TYPE_SET);
	ilEnable(IL_FORMAT_SET);

	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilTypeFunc(IL_UNSIGNED_BYTE);
	ilFormatFunc(IL_BGR);

	// Is there a file to load from the command-line?
	if (__argc > 1) {
		LoadImages(__argv[1]);
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_MENU1);

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}


void LoadImages(char *FileName)
{
	ILuint Image, i;

	hDC = GetDC(HWnd);
	hMemDC = CreateCompatibleDC(hDC);

	ilGenImages(1, &Image);
	ilBindImage(Image);
	if (!ilLoadImage(FileName)) {
		ilDeleteImages(1, &Image);
		return;
	}

	ilEnable(IL_ORIGIN_SET);
	ilEnable(IL_FORMAT_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	//ilFormatFunc(IL_BGRA);
	ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
	ilutRenderer(ILUT_WIN32);

	CurImage = 0;
	NumImages = ilGetInteger(IL_NUM_IMAGES) + 1;
	Bitmaps = new HBITMAP[NumImages];
	BmpInfo = new BITMAPINFOHEADER[NumImages];
	Durations = new ILuint[NumImages];
	if (Bitmaps == NULL || BmpInfo == NULL || Durations == NULL) {
		ilDeleteImages(1, &Image);
		return;
	}

	for (i = 0; i < NumImages; i++) {
		ilActiveImage(0);
		ilActiveImage(i);
		Durations[i] = ilGetInteger(IL_IMAGE_DURATION);
		*(Bitmaps + i) = ilutConvertToHBitmap(hDC);
		ilutGetBmpInfo((BITMAPINFO*)(BmpInfo + i));
	}

	SelectObject(hMemDC, Bitmaps[0]);

	ilDeleteImages(1, &Image);

	sprintf(NewTitle, "%s - %s", TITLE, FileName);
	SetWindowText(HWnd, NewTitle);

	QueryPerformanceFrequency((LARGE_INTEGER*)&TimerFreq);
	TimerRes = 1.0 / TimerFreq;
	QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);

	return;
}


void DestroyGDI()
{
	ILuint i;

	if (Bitmaps) {
		for (i = 0; i < NumImages; i++) {
			DeleteObject(*(Bitmaps + i));
		}
	}

	if (hMemDC)
		DeleteDC(hMemDC);

	if (Bitmaps)
		delete []Bitmaps;
	if (BmpInfo)
		delete []BmpInfo;

	Bitmaps = NULL;
	BmpInfo = NULL;
	hMemDC = NULL;

	return;
}


void DisplayImage()
{
	static PAINTSTRUCT	ps;
	static __int64		CurTime;
	static double		TimeElapsed;

	// Not created yet.
	if (Durations == NULL || BmpInfo == NULL || Bitmaps == NULL)
		return;

	if (!IsPaused) {
		QueryPerformanceCounter((LARGE_INTEGER*)&CurTime);
		TimeElapsed = (CurTime - StartTime) * TimerRes;
		if (TimeElapsed * 1000 > Durations[CurImage]) {
			StartTime = CurTime;
			CurImage++;
			if (CurImage >= NumImages) {
				CurImage = 0;
			}
			SelectObject(hMemDC, Bitmaps[CurImage]);
		}
	}

	hDC = BeginPaint(HWnd, &ps);
	BitBlt(hDC, 0, 0, (WORD)BmpInfo[CurImage].biWidth, (WORD)BmpInfo[CurImage].biHeight, 
		  hMemDC, 0, 0, SRCCOPY);
	EndPaint(HWnd, &ps);

	return;
}


// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HMENU	hMenu;
	static ILuint	Colours;
	static RECT		Rect;
    static HDROP	hDrop;

	static char OpenFileName[2048];
	static char OpenFilter[2048];
	static char *OFilter[] = {
		"All Files (*.*)", "*.*",
		"Half-Life Model Files (*.mdl)", "*.mdl",
		"Homeworld Image Files (*.lif)", "*.lif",
		"Image Files (All Supported Types)", "*.jpe;*.jpg;*.jpeg;*.lif;*.bmp;*.ico;*.pbm;*.pgm;*.pnm;*.ppm;*.png;*.bw;*.rgb;*.rgba;*.sgi;*.tga;*.tif;*.tiff;*.pcx",
		"Jpeg Files (*.jpe, *.jpg, *.jpeg)", "*.jpe;*.jpg;*.jpeg",
		"Microsoft Bitmap Files (*.bmp)", "*.bmp",
		"Microsoft Icon Files (*.ico)", "*.ico",
		"OpenIL Files (*.oil)", "*.oil",
		"Portable AnyMap Files (*.pbm, *.pgm, *.pnm, *.ppm)", "*.pbm;*.pgm;*.pnm;*.ppm",
		"Portable Network Graphics Files (*.png)", "*.png",
		"Sgi Files (*.sgi)", "*.bw;*.rgb;*.rgba;*.sgi",
		"Targa Files (*.tga)", "*.tga",
		"Tiff Files (*.tif)", "*.tif;*.tiff",
		"Quake Wal Files (*.wal)", "*.wal",
		"ZSoft Pcx Files (*.pcx)", "*.pcx",
		"\0\0"
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
		512,
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

	switch (message)
	{
		case WM_CREATE:
			GenFilterString(OpenFilter, OFilter);

			hDC = GetDC(hWnd);
			DragAcceptFiles(hWnd, TRUE);

			break;

		case WM_CLOSE:
			DestroyGDI();
			ReleaseDC(hWnd, hDC);
			DestroyWindow(hWnd);
			UnregisterClass(TITLE, hInstance);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_PAINT:
			DisplayImage();
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case WM_DROPFILES:
			hDrop = (HDROP)wParam;
			DragQueryFile(hDrop, 0, OpenFileName, 512);

			DestroyGDI();
			LoadImages(OpenFileName);

			DragFinish (hDrop);
			return 0;

		case WM_COMMAND:
			FilterType = LOWORD(wParam);
	
			switch (LOWORD(wParam))
			{
				case ID_FILE_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					return (0L);

				case ID_FILE_LOAD:
					sprintf(OpenFileName, "*.*");
					Ofn.lpstrFilter = OpenFilter;
					Ofn.lpstrFile = OpenFileName;
					Ofn.lpstrTitle = "Open File";
					Ofn.nFilterIndex = 1;
					Ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

					if (!GetOpenFileName(&Ofn))
						return (0L);

					DestroyGDI();
					LoadImages(OpenFileName);

					return (0L);

				case ID_EDIT_PAUSE:
					IsPaused = true;
					return 0;

				case ID_EDIT_RESUME:
					IsPaused = false;
					QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);
					return 0;
			}

		default:
		  return (DefWindowProc(hWnd, message, wParam, lParam));
	}

    return (0L);
}


void GenFilterString(char *Out, char **Strings)
{
	int OutPos = 0, StringPos = 0;

	while (Strings[StringPos][0] != 0) {
		sprintf(Out + OutPos, Strings[StringPos]);
		OutPos += strlen(Strings[StringPos++]) + 1;
	}

	Out[OutPos++] = 0;
	Out[OutPos] = 0;

	return;
}
