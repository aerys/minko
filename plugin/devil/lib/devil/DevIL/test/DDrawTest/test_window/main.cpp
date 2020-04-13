//============================================================//
// File:		main.cpp
// 
// Date:		11-21-2000
//===========================================================//
#include "../header/Render2DCore.h"
#include "resource.h"
#include <math.h>
#define ILUT_USE_WIN32
#include <il/ilut.h>


HWND						m_MainHWND;
HINSTANCE					hInst;
beScreen					m_MainDDScreen;
beSurface*					m_pMainSurface;
beSurface*					m_pMouseCursorSurface;
beSurface*					m_pBackgroundSurface;
DDBLTFX						ddbltfx;

int							m_iWindowWidth	= 640;
int							m_iWindowHeight = 480;
int							m_iCurrMouseX;
int							m_iCurrMouseY;
beScreenParams				InitDisplayStruct;

#define BORDER_W	8
#define MENU_H		46
#define MIN_W		205  // Accomodate the menu bar.
#define	MAX_W		400
#define	MAX_H		300
#define	TITLE		"DevIL Direct Draw Test"

ILuint	NumUndosAllowed = 4, UndoSize = 0;
ILuint	Undos[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
ILuint	Width, Height, Depth;  // Main image
ILuint	MainImage = 0;  // Main image
HBITMAP	hBitmap;  // Main bitmap

ILuint	FilterType;
ILuint	FilterParamInt;
ILfloat	FilterParamFloat;


BOOL APIENTRY AboutDlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam);
BOOL APIENTRY FilterDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);
BOOL APIENTRY ResizeDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);
void GenFilterString(char *Out, char **Strings);

//===========================================================//
// WindowProc()
//===========================================================//
long FAR PASCAL WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int x, y;
	static HDC hDC;

	static char NewTitle[2048];
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

    switch (message)
    {
        case WM_ACTIVATE:
            return 0L;

		case WM_CREATE:
			GenFilterString(OpenFilter, OFilter);
			GenFilterString(SaveFilter, SFilter);

			// Store the device context
			hDC = GetDC(hWnd);
			return 0L;

        case WM_DESTROY:
            // Clean up and close the app
			m_MainDDScreen.Delete();
            PostQuitMessage(0);
            return 0L;

        case WM_KEYDOWN:
            // Handle any non-accelerated key commands
            switch (wParam)
            {
                case VK_ESCAPE:
                case VK_F12:
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0L;
            }
            break;

		case WM_MOVE:
			m_MainDDScreen.Move();
			break;

		case WM_SIZE:
            m_MainDDScreen.Move();
			break;

		case WM_MOUSEMOVE:
			m_iCurrMouseX = LOWORD(lParam); 
			m_iCurrMouseY = HIWORD(lParam); 

		case WM_PAINT:
			if (!m_pBackgroundSurface)
				break;

			// we don't need a colorfill...cause we're using a bitmap to clear the surface
			m_pMainSurface->GetSurface()->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);
			m_pMainSurface->BltFast(0, 0, m_pBackgroundSurface, NULL, DDBLTFAST_NOCOLORKEY);

			/*RECT rcRect;
			rcRect.left = 0;
			rcRect.right = 128;
			rcRect.top = 0;
			rcRect.bottom = 128;*/

			DDSURFACEDESC2 ddsd;
			if (!m_pMouseCursorSurface->Lock(ddsd))
				return -1;

			// pixel-plotting test
			for (x = 0; x < 128; x++) {
				for (y = 0; y < 128; y++) {	
					m_pMouseCursorSurface->PlotPixel(x, y, x*2%255, 0, y*2%255, &ddsd);
				}
			}
			if (!m_pMouseCursorSurface->Unlock())
				return -1;

			//m_pMainSurface->BltFast(0, 0, m_pMouseCursorSurface,
			//                   &rcRect, DDBLTFAST_NOCOLORKEY);

			m_MainDDScreen.GetBackBufferSurface()->BltFast(0,0,m_pMainSurface->GetSurface(),NULL,DDBLTFAST_NOCOLORKEY);
			m_MainDDScreen.Flip();

			ValidateRect(hWnd, NULL);
			break;

		case WM_COMMAND:
			FilterType = LOWORD(wParam);

			switch (LOWORD(wParam))
			{
				case ID_FILE_EXIT:
					DestroyWindow(hWnd);
					return (0L);

				case ID_HELP_ABOUT:
					DialogBox (hInst,
						MAKEINTRESOURCE(IDD_DIALOG_ABOUT),
						hWnd,
						AboutDlgProc);
					return (0L);

				case ID_EFFECTS_COUNTCOLORS:
					int Colours;
					Colours = iluColoursUsed();
					char ColourString[255];
					sprintf(ColourString, "The number of colours in this image is:  %d", Colours);
					MessageBox(NULL, ColourString, "Colour Count", MB_OK);
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

					ilDeleteImages(UndoSize, Undos);
					UndoSize = 0;

					ilGenImages(1, &MainImage);
					ilBindImage(MainImage);
					Undos[0] = MainImage;
					ilutGetWinClipboard();

					ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
					ilutRenderer(ILUT_WIN32);

					Width = ilGetInteger(IL_IMAGE_WIDTH);
					Height = ilGetInteger(IL_IMAGE_HEIGHT);
					Depth = ilGetInteger(IL_IMAGE_DEPTH);

					hBitmap = ilutConvertToHBitmap(hDC);
					m_pBackgroundSurface->LoadImage(hBitmap, false);
					DeleteObject(hBitmap);

					RECT Rect;
					GetWindowRect(hWnd, &Rect);
					SetWindowPos(hWnd, HWND_TOP, Rect.left, Rect.top,
						Width < MIN_W ? MIN_W + BORDER_W : Width + BORDER_W,
						Height + MENU_H, SWP_SHOWWINDOW);
					return (0L);

				case ID_FILE_LOAD:
					sprintf(OpenFileName, "*.*");
					Ofn.lpstrFilter = OpenFilter;
					Ofn.lpstrFile = OpenFileName;
					Ofn.lpstrTitle = "Open File";
					Ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

					if (!GetOpenFileName(&Ofn))
						return (0L);

					ilDeleteImages(UndoSize, Undos);
					UndoSize = 0;

					ilGenImages(1, &MainImage);
					ilBindImage(MainImage);
					Undos[0] = MainImage;
					if (!ilLoadImage(OpenFileName))
						return (0L);
					ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
					ilutRenderer(ILUT_WIN32);

					Width = ilGetInteger(IL_IMAGE_WIDTH);
					Height = ilGetInteger(IL_IMAGE_HEIGHT);
					Depth = ilGetInteger(IL_IMAGE_DEPTH);

					hBitmap = ilutConvertToHBitmap(hDC);
					m_pBackgroundSurface->LoadImage(hBitmap, false);
					DeleteObject(hBitmap);

					GetWindowRect(hWnd, &Rect);
					SetWindowPos(hWnd, HWND_TOP, Rect.left, Rect.top,
						Width < MIN_W ? MIN_W + BORDER_W : Width + BORDER_W,
						Height + MENU_H, SWP_SHOWWINDOW);

					sprintf(NewTitle, "%s - %s", TITLE, OpenFileName);
					SetWindowText(hWnd, NewTitle);

					return (0L);

				case ID_FILE_SAVE:
					sprintf(SaveFileName, "monkey.tga");
					Ofn.lpstrFilter = SaveFilter;
					Ofn.lpstrFile = SaveFileName;
					Ofn.lpstrTitle = "Save File";
					Ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

					if (!GetSaveFileName(&Ofn))
						return (0L);

					ilEnable(IL_FILE_OVERWRITE);
					ilSaveImage(SaveFileName);

					sprintf(NewTitle, "%s - %s", TITLE, SaveFileName);
					SetWindowText(hWnd, NewTitle);

					return (0L);

				case ID_EDIT_UNDOLEVEL:
					if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						NumUndosAllowed = FilterParamInt <= 10 ? FilterParamInt : 10;
					}
					return (0L);

				case ID_EDIT_UNDO:
					if (UndoSize && NumUndosAllowed) {
						ilDeleteImages(1, &Undos[UndoSize]);
						ilBindImage(Undos[--UndoSize]);
						hBitmap = ilutConvertToHBitmap(hDC);
						m_pBackgroundSurface->LoadImage(hBitmap, false);
						DeleteObject(hBitmap);

						Width = ilGetInteger(IL_IMAGE_WIDTH);
						Height = ilGetInteger(IL_IMAGE_HEIGHT);
						Depth = ilGetInteger(IL_IMAGE_DEPTH);

						GetWindowRect(hWnd, &Rect);
						SetWindowPos(hWnd, HWND_TOP, Rect.left, Rect.top,
							Width < MIN_W ? MIN_W + BORDER_W : Width + BORDER_W,
							Height + MENU_H, SWP_SHOWWINDOW);

						InvalidateRect(hWnd, NULL, FALSE);
					}
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
				ilGenImages(1, &Undos[UndoSize]);
				ilBindImage(Undos[UndoSize]);
				ilCopyImage(Undos[UndoSize-1]);
				MainImage = Undos[UndoSize];  // ???
			}


			switch (LOWORD(wParam))
			{
				case ID_CONVERT_RGB:
					ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
					break;

				case ID_CONVERT_RGBA:
					ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
					break;

				case ID_CONVERT_BGR:
					ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
					break;

				case ID_CONVERT_BGRA:
					ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
					break;

				case ID_CONVERT_LUMINANCE:
					ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
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
					if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluNoisify(FilterParamFloat);
					}
					break;

				case ID_FILTER_PIXELIZE:
					if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluPixelize(FilterParamInt);
					}
					break;

				case ID_FILTERS_BLUR_AVERAGE:
					if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluBlurAvg(FilterParamInt);
					}
					break;

				case ID_FILTERS_BLUR_GAUSSIAN:
					if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluBlurGaussian(FilterParamInt);
					}
					break;

				case ID_FILTER_GAMMACORRECT:
					if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluGammaCorrect(FilterParamFloat);
					}
					break;

				case ID_FILTER_SHARPEN:
					if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluSharpen(FilterParamFloat, 1);
					}
					break;

				case ID_EFFECTS_FILTERS_ROTATE:
					if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluRotate(FilterParamFloat);

						Width = ilGetInteger(IL_IMAGE_WIDTH);
						Height = ilGetInteger(IL_IMAGE_HEIGHT);
						RECT Rect;
						GetWindowRect(hWnd, &Rect);
						SetWindowPos(hWnd, HWND_TOP, Rect.left, Rect.top,
							Width < MIN_W ? MIN_W + BORDER_W : Width + BORDER_W,
							Height + MENU_H, SWP_SHOWWINDOW);
					}
					break;

				case ID_EFFECTS_FILTERS_SCALE:
					iluImageParameter(ILU_FILTER, ILU_BILINEAR);
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_RESIZE), hWnd, ResizeDlgProc);
					break;
			}

			ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
			hBitmap = ilutConvertToHBitmap(hDC);
			m_pBackgroundSurface->LoadImage(hBitmap, false);
			DeleteObject(hBitmap);
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);

    }


    return 0L;
}


//===========================================================//
// InitApp()
//===========================================================//
static HRESULT InitApp(HINSTANCE hInstance, int nCmdShow)
{
    HWND		hWnd;
    WNDCLASSEX	wcex;

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
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON10);

	RegisterClassEx(&wcex);


    // Create a window
    hWnd = CreateWindow(TITLE, TITLE, WS_OVERLAPPEDWINDOW, 50, 50, 400, 300,
						NULL, NULL, hInstance, NULL);
    if (!hWnd)
        return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    SetFocus(hWnd);

	m_MainHWND = hWnd;

	InitDisplayStruct.m_Flags = 0;
	InitDisplayStruct.m_Flags |= BE_SCREENFLAGS_WINDOWED;
	InitDisplayStruct.m_OwnerWnd = hWnd;

	InitDisplayStruct.m_iScreenWidth = m_iWindowWidth;
	InitDisplayStruct.m_iScreenHeight = m_iWindowHeight;
	InitDisplayStruct.m_iScreenBitCount = 16;

	if (!m_MainDDScreen.Initialize(InitDisplayStruct))
		MessageBox(NULL,"ah!","ahhhh",MB_OK);
	
	m_pMouseCursorSurface	= new beSurface(&m_MainDDScreen);
	m_pBackgroundSurface	= new beSurface(&m_MainDDScreen);
	m_pMainSurface			= new beSurface(&m_MainDDScreen);
	m_pMainSurface->CreateSurface(InitDisplayStruct.m_iScreenWidth,InitDisplayStruct.m_iScreenHeight,false);

	ilEnable(IL_FORMAT_SET);
	ilEnable(IL_ORIGIN_SET);
	ilEnable(IL_TYPE_SET);

	ilFormatFunc(IL_BGRA);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilTypeFunc(IL_UNSIGNED_BYTE);

	hBitmap = ilutConvertToHBitmap(GetDC(hWnd));
	m_pBackgroundSurface->LoadImage(hBitmap, false);

	m_pMouseCursorSurface->CreateSurface(128, 128, true);

	InvalidateRect(hWnd, NULL, FALSE);

	return TRUE;
}

//===========================================================//
// WinMain()
//===========================================================//
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG		msg;
	HACCEL	hAccelTable;

	hInst = hInstance;

    InitApp(hInstance, nCmdShow);
	ZeroMemory(&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = RGB(128, 128, 128);

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_MENU1);
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete m_pMainSurface;
	delete m_pMouseCursorSurface;
	delete m_pBackgroundSurface;

    return msg.wParam;
}


BOOL APIENTRY AboutDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
    switch (message)
	{
		// Initialize the dialog box
	    case WM_INITDIALOG:
		{
			int i;
			ILenum ilError;
			char VersionNum[256];

			sprintf(VersionNum, "Num:  %d", ilGetInteger(IL_VERSION_NUM));

			// ilGetString demo
			SetDlgItemText(hDlg, IDC_ABOUT_VENDOR, ilGetString(IL_VENDOR));
			SetDlgItemText(hDlg, IDC_ABOUT_VER_STRING, ilGetString(IL_VERSION));
			SetDlgItemText(hDlg, IDC_ABOUT_VER_NUM, VersionNum);

			// Display any recent error messages
			for (i = 0; i < 6; i++) {
				ilError = ilGetError();
				if (ilError == IL_NO_ERROR)
					break;
				SetDlgItemText(hDlg, IDC_ERROR1+i, iluErrorString(ilError));
			}

			return (TRUE);
		}
		break;

		// Process command messages
	    case WM_COMMAND:      
		{
			// Validate and Make the changes
			if (LOWORD(wParam) == IDOK)
				EndDialog(hDlg, TRUE);
			if (LOWORD(wParam) == IDCANCEL)
				EndDialog(hDlg, FALSE);
	    }
		break;

		// Closed from sysbox
		case WM_CLOSE:
			EndDialog(hDlg,TRUE);
		break;
	}

	return FALSE;
}


// Dialog procedure.
BOOL APIENTRY FilterDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
	char EditString[255];

    switch (message)
	{
		// Initialize the dialog box
	    case WM_INITDIALOG:
		{
			switch (FilterType)
			{
				case ID_FILTER_PIXELIZE:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, "Width of pixelized block:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, "1");
					break;
				case ID_FILTER_NOISE:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, "Amount of noise threshold:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, "1");
					break;
				case ID_FILTERS_BLUR_AVERAGE:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, "Number of iterations:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, "1");
					break;
				case ID_FILTERS_BLUR_GAUSSIAN:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, "Number of iterations:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, "1");
					break;
				case ID_FILTER_GAMMACORRECT:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, "Amount of gamma correction:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, "1.0");
					break;
				case ID_FILTER_SHARPEN:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, "Sharpening factor:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, "1.0");
					break;
				case ID_EFFECTS_FILTERS_ROTATE:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, "Number of degress to rotate:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, "0.0");
					break;

				case ID_EDIT_UNDOLEVEL:
					SetDlgItemText(hDlg, IDC_FILTER_DESC_TEXT, "Set level of undo:");
					SetDlgItemText(hDlg, IDC_FILTER_EDIT, "4");
					break;

			}

			return TRUE;
		}
		break;

		// Process command messages
	    case WM_COMMAND:
		{
			// Validate and Make the changes
			if (LOWORD(wParam) == IDOK) {
				GetDlgItemText(hDlg, IDC_FILTER_EDIT, EditString, 255);
				FilterParamInt = atoi(EditString);
				FilterParamFloat = (float)atof(EditString);
				EndDialog(hDlg, TRUE);
			}
			if (LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, FALSE);
			}
	    }
		break;

		// Closed from sysbox
		case WM_CLOSE:
			EndDialog(hDlg,TRUE);
		break;
	}

	return FALSE;
}


// Dialog procedure.
BOOL APIENTRY ResizeDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
	static char x[255], y[255], z[255];
	static ILuint xsize, ysize, zsize;
	static RECT Rect;

    switch (message)
	{
		// Initialize the dialog box
	    case WM_INITDIALOG:
		{
			sprintf(x, "%d", Width);
			sprintf(y, "%d", Height);
			sprintf(z, "%d", Depth);
			SetDlgItemText(hDlg, IDC_EDIT_RESIZE_X, x);
			SetDlgItemText(hDlg, IDC_EDIT_RESIZE_Y, y);
			SetDlgItemText(hDlg, IDC_EDIT_RESIZE_Z, z);
			return TRUE;
		}
		break;

		// Process command messages
	    case WM_COMMAND:
		{
			// Validate and Make the changes
			if (LOWORD(wParam) == IDOK) {
				GetDlgItemText(hDlg, IDC_EDIT_RESIZE_X, x, 255);
				GetDlgItemText(hDlg, IDC_EDIT_RESIZE_Y, y, 255);
				GetDlgItemText(hDlg, IDC_EDIT_RESIZE_Z, z, 255);
				xsize = atoi(x);
				ysize = atoi(y);
				zsize = atoi(z);
				if (xsize && ysize && zsize) {
					iluScale(xsize, ysize, zsize);

					Width = ilGetInteger(IL_IMAGE_WIDTH);
					Height = ilGetInteger(IL_IMAGE_HEIGHT);
					Depth = ilGetInteger(IL_IMAGE_DEPTH);

					GetWindowRect(m_MainHWND, &Rect);
					SetWindowPos(m_MainHWND, HWND_TOP, Rect.left, Rect.top,
						Width < MIN_W ? MIN_W + BORDER_W : Width + BORDER_W,
						Height + MENU_H, SWP_SHOWWINDOW);


					InvalidateRect(m_MainHWND, NULL, FALSE);
				}
				EndDialog(hDlg, TRUE);
			}
			if (LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, FALSE);
			}
	    }
		break;

		// Closed from sysbox
		case WM_CLOSE:
			EndDialog(hDlg,TRUE);
		break;
	}

	return FALSE;
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
