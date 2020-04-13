//-----------------------------------------------------------------------------
//
// ImageLib Windows (GDI) Test Source
// Copyright (C) 2000 by Denton Woods
// Last modified:  01/24/2001 <--Y2K Compliant! =]
//
// Filename: testil/windowstest/windowstest.c
//
// Description:  Full MDI test application for OpenIL.
//
//-----------------------------------------------------------------------------

#include <windows.h>
#include <il\il.h>
#include <il\ilu.h>
#include <il\ilut.h>
#include "mditest.h"
#include "resource.h"


#define BORDER_W	8
#define MENU_H		46
#define MIN_W		205  // Accomodate the menu bar.
#define MAX_W		400
#define MAX_H		400
ILuint NumUndosAllowed = 4, UndoSize = 0;
ILuint Undos[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
ILuint Width, Height, Depth;  // Main image

ILuint	FilterType;
ILuint	FilterParamInt;
ILfloat	FilterParamFloat;
char	FilterEditString[255];


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG			msg;
	WNDCLASSEX	wcex;
	HACCEL		hAccelTable;

	hInstance = hInst;
	CmdShow = nCmdShow;

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

	wcex.lpszClassName	= CHILD;
	RegisterClassEx(&wcex);

	/*HWND h = CreateWindow(TITLE, TITLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						50, 50, 400, 300, NULL, NULL, hInstance, NULL);
	DestroyWindow(h);*/
	if (!MainWindow.Create(TITLE, 50, 50, 400, 300, NULL))
		return false;

	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilEnable(IL_TYPE_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilTypeFunc(IL_UNSIGNED_BYTE);

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_MENU1);

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	DestroyWindows();

	UnregisterClass(TITLE, hInstance);
	UnregisterClass(CHILD, hInstance);

	return msg.wParam;
}


cWindow *GetWinClass(HWND hWnd)
{
	if (MainWindow.GetHWnd() == hWnd)
		return &MainWindow;
	list <cWindow>::iterator i;

	for (i = WindowList.begin(); i != WindowList.end(); i++) {
		if (i->GetHWnd() == hWnd)
			return &(*i);
	}

	return NULL;
}


void AddNewWindow(char *FileName)
{
	cWindow Window;

	Window.LoadImage(FileName);
	WindowList.push_back(Window);

	return;
}


void DestroyWindows()
{
	list <cWindow>::iterator i;

	for (i = WindowList.begin(); i != WindowList.end(); i++) {
		i->Destroy();
	}

	WindowList.clear();

	return;
}


void DrawWindows()
{
	list <cWindow>::iterator i;

	for (i = WindowList.begin(); i != WindowList.end(); i++) {
		i->Paint();
	}

	return;
}



// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HMENU		hMenu;
	static ILuint		Colours;
	static RECT			Rect;
    static HDROP		hDrop;
	static cWindow		*Window;

	static char OpenFileName[2048];
	static char OpenFilter[2048];
	static char SaveFileName[2048];
	static char SaveFilter[2048];
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
	static char *SFilter[] = {
		"All Files (*.*)", "*.*",
		"C-Style Header (*.h)", "*.h",
		"Jpeg Files (*.jpe, *.jpg, *.jpeg)", "*.jpe;*.jpg;*.jpeg",
		"Microsoft Bitmap Files (*.bmp)", "*.bmp",
		"OpenIL Files (*.oil)", "*.oil",
		"Portable AnyMap Files (*.pbm, *.pgm, *.ppm)", "*.pbm;*.pgm;*.ppm",
		"Portable Network Graphics Files (*.png)", "*.png",
		"Sgi Files (*.sgi)", "*.bw;*.rgb;*.rgba;*.sgi",
		"Targa Files (*.tga)", "*.tga",
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

	Window = GetWinClass(hWnd);
	if (Window == NULL) {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	Window->BindImage();

	switch (message)
	{
		case WM_CREATE:
			GenFilterString(OpenFilter, OFilter);
			GenFilterString(SaveFilter, SFilter);
			break;

		case WM_CLOSE:
			Window->Destroy();
			break;

		case WM_DESTROY:
			if (Window == &MainWindow)
				PostQuitMessage(0);
			break;

		case WM_PAINT:
			Window->Paint();
			/*if (Window == &MainWindow)
				DrawWindows();*/
			break;

		case WM_ACTIVATE:
			if (hWnd != MainWindow.GetHWnd()) {
				if (wParam != WA_INACTIVE) {  // Switch back to window
					CurChildWnd = hWnd;
				}
			}
			return (DefWindowProc(hWnd, message, wParam, lParam));
			break;

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);

			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case WM_COMMAND:
			FilterType = LOWORD(wParam);
	
			switch (LOWORD(wParam))
			{
				case ID_FILE_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					return (0L);

				case ID_EDIT_COPY:
					ilutSetWinClipboard();
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

					AddNewWindow(OpenFileName);
					return (0L);

				case ID_FILE_SAVE:
					/*if (Window == &MainWindow)
						return (0L);*/

					if (CurChildWnd == NULL)
						return (0L);

					sprintf(SaveFileName, "monkey.tga");
					Ofn.lpstrFilter = SaveFilter;
					Ofn.lpstrFile = SaveFileName;
					Ofn.lpstrTitle = "Save File";
					Ofn.nFilterIndex = 1;
					Ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

					if (!GetSaveFileName(&Ofn))
						return (0L);

					ilEnable(IL_FILE_OVERWRITE);
					ilSaveImage(SaveFileName);

					GetWinClass(CurChildWnd)->SetTitle(SaveFileName);

					return (0L);
			}

			InvalidateRect(hWnd, NULL, FALSE);
			break;

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
