#include <windows.h>
#include <il\il.h>
#include <il\ilu.h>
#include <il\ilut.h>
#include "resource.h"


#pragma comment(lib, "opengl32.lib")


LPCTSTR lpszAppName = "OpenIL Windows Test";
HINSTANCE hInstance;
HWND HWnd;


#define MAX_W 400
#define MAX_H 300
ILuint NumUndosAllowed = 4, UndoSize = 0;
ILuint Undos[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
ILuint MainImage = 0;
ILuint Width, Height, Depth;  // Main image


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY AboutDlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam);
BOOL APIENTRY FilterDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);
BOOL APIENTRY ResizeDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);
void GenFilterString(char *Out, char **Strings);


// Entry point of all Windows programs
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG			msg;	// Windows message structure
	WNDCLASSEX	wcex;
	HACCEL		hAccelTable;
	HWND		hWnd;	// Storeage for window handle

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
	wcex.lpszClassName	= lpszAppName;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);

	RegisterClassEx(&wcex);


	// Create the main application window
	hWnd = CreateWindow(lpszAppName, lpszAppName, WS_OVERLAPPEDWINDOW, 50, 50, 400, 300,
						NULL, NULL, hInstance, NULL);
	if (hWnd == NULL)
		return FALSE;

	// Display the window
	//ShowWindow(hWnd, SW_SHOW);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_MENU1);

	// Process application messages until the application closes
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnregisterClass(lpszAppName, hInstance);

	return msg.wParam;
}


ILuint FilterType;
ILuint FilterParamInt;
ILfloat	FilterParamFloat;


PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp, PBITMAPINFO pbmi)
{ 
    BITMAP bmp; 
    WORD    cClrBits; 

    // Retrieve the bitmap's color format, width, and height. 
    GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);

    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 

     /*if (cClrBits != 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (1<< cClrBits)); 

     // There is no RGBQUAD array for the 24-bit-per-pixel format. 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); */

    // Initialize the fields in the BITMAPINFO structure. 

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // Width must be DWORD aligned unless bitmap is RLE compressed.
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 15) /16 
                                  * pbmi->bmiHeader.biHeight 
                                  * cClrBits;
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
     pbmi->bmiHeader.biClrImportant = 0; 
     return pbmi; 
 } 

// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC hDC;                 // Private GDI Device context
	static HMENU hMenu;				// Handle to the menu
	static ILuint Colours;
	static RECT Rect;

	static char OpenFileName[512];
	static char OpenFilter[512];
	static char SaveFileName[512];
	static char SaveFilter[512];
	static char *OFilter[] = {
		"All Files (*.*)", "*.*",
		"Jpeg Files (*.jpe, *.jpg, *.jpeg)", "*.jpe;*.jpg;*.jpeg",
		"Microsoft Bitmap Files (*.bmp)", "*.bmp",
		"Microsoft Icon Files (*.ico)", "*.ico",
		"Portable AnyMap Files (*.pbm, *.pgm, *.ppm)", "*.pbm;*.pgm;*.ppm",
		"Portable Network Graphics Files (*.png)", "*.png",
		"Sgi Files (*.sgi)", "*.bw;*.rgb;*.rgba;*.sgi",
		"Targa Files (*.tga)", "*.tga",
		"Tiff Files (*.tif)", "*.tif;*.tiff",
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
		// Window creation, setup for OpenGL
		case WM_CREATE:
			GenFilterString(OpenFilter, OFilter);
			GenFilterString(SaveFilter, SFilter);

			// Store the device context
			hDC = GetDC(hWnd);

			break;

		// Window is being destroyed, cleanup
		case WM_DESTROY:
			// Tell the application to terminate after the window
			// is gone.
			PostQuitMessage(0);
			break;

		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
		case WM_PAINT:

	PAINTSTRUCT ps;
	hDC = BeginPaint(hWnd, &ps);

HBITMAP hbmp;
HDC hMemDC;
BITMAPINFOHEADER lpInfo;


	//SelectObject(hdc, hbmp);


hMemDC = CreateCompatibleDC(hDC);
//hbmp = (HBITMAP)LoadImage(NULL, "rgb32.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);


/*ILuint Id;
ilGenImages(1, &Id);
ilBindImage(Id);
ilLoadImage("rgb32.bmp");*/
hbmp = ilutConvertToHBitmap(hDC);
//ilDeleteImages(1, &Id);


/*CreateBitmapInfoStruct(hWnd, hbmp, (PBITMAPINFO)&lpInfo);
hbmp = (HBITMAP)SelectObject(hMemDC, hbmp);
BitBlt(hdc, 0, 0, (WORD)lpInfo.biWidth, (WORD)lpInfo.biHeight, 
      hMemDC, 0, 0, SRCCOPY);
DeleteObject(SelectObject(hMemDC, hbmp));
DeleteDC(hMemDC);*/


/*SetDIBitsToDevice(hDC, 0, 0, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
  0, 0, 0, ilGetInteger(IL_IMAGE_HEIGHT), ilGetData(), (BITMAPINFO*)&lpInfo, DIB_RGB_COLORS); */

ilBindImage(MainImage);
hbmp = ilutConvertToHBitmap(hDC);
CreateBitmapInfoStruct(hWnd, hbmp, (PBITMAPINFO)&lpInfo);
hbmp = (HBITMAP)SelectObject(hMemDC, hbmp);
BitBlt(hDC, 0, 0, (WORD)lpInfo.biWidth, (WORD)lpInfo.biHeight, 
      hMemDC, 0, 0, SRCCOPY);
DeleteObject(SelectObject(hMemDC, hbmp));
DeleteDC(hMemDC);



//ilDeleteImages(1, &Id);


//DeleteObject(hbmp);
			
			EndPaint(hWnd, &ps);
			ValidateRect(hWnd, NULL);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				//PostQuitMessage(0);

			InvalidateRect(hWnd, NULL, FALSE);
		break;

		case WM_COMMAND:
			FilterType = LOWORD(wParam);
	
			switch (LOWORD(wParam))
			{
				case ID_FILE_EXIT:
					DestroyWindow(hWnd);
					return (0L);

				case ID_HELP_ABOUT:
					DialogBox (hInstance,
						MAKEINTRESOURCE(IDD_DIALOG_ABOUT),
						hWnd,
						AboutDlgProc);
					return (0L);

				case ID_EFFECTS_COUNTCOLORS:
					Colours = iluColoursUsed();
					char ColourString[255];
					sprintf(ColourString, "The number of colours in this image is:  %d", Colours);
					MessageBox(NULL, ColourString, "Colour Count", MB_OK);
					return (0L);

				case ID_EDIT_COPY:
					ilutSetWinClipboard();
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

ilutGetWinClipboard();
					ilConvertImage(IL_BGRA);
					ilutRenderer(ILUT_DIRECTX);

					Width = ilGetInteger(IL_IMAGE_WIDTH);
					Height = ilGetInteger(IL_IMAGE_HEIGHT);
					Depth = ilGetInteger(IL_IMAGE_DEPTH);

					GetWindowRect(hWnd, &Rect);
					SetWindowPos(hWnd, HWND_TOP, Rect.left, Rect.top, Width, Height + 46, SWP_SHOWWINDOW);
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
					ilConvertImage(IL_RGB);
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_CONVERT_RGBA:
					ilConvertImage(IL_RGBA);
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_CONVERT_BGR:
					ilConvertImage(IL_BGR);
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_CONVERT_BGRA:
					ilConvertImage(IL_BGRA);
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_CONVERT_LUMINANCE:
					ilConvertImage(IL_LUMINANCE);
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_EFFECTS_FLIP:
					iluFlipImage();
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_EFFECTS_MIRROR:
					iluMirror();
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_FILTER_EMBOSS:
					iluEmboss();
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_FILTER_ALIENIFY:
					iluAlienify();
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_FILTER_BITFILTER1:
					iluBitFilter1();
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_FILTER_BITFILTER2:
					iluBitFilter2();
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_FILTER_BITFILTER3:
					iluBitFilter3();
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_FILTER_NEGATIVE:
					iluNegative();
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_EFFECTS_FILTERS_EDGEDETECT_SOBEL:
					iluEdgeDetectS();
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_EFFECTS_FILTERS_EDGEDETECT_PREWITT:
					iluEdgeDetectP();
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_FILTER_NOISE:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluNoisify(FilterParamInt);
						InvalidateRect(hWnd,NULL,FALSE);
					}
					break;

				case ID_FILTER_PIXELIZE:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluPixelize(FilterParamInt);
						InvalidateRect(hWnd,NULL,FALSE);
					}
					break;

				case ID_FILTERS_BLUR_AVERAGE:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluBlurAvg(FilterParamInt);
						InvalidateRect(hWnd,NULL,FALSE);
					}
					break;

				case ID_FILTERS_BLUR_GAUSSIAN:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluBlurGaussian(FilterParamInt);
						InvalidateRect(hWnd,NULL,FALSE);
					}
					break;

				case ID_FILTER_GAMMACORRECT:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluGammaCorrectScale(FilterParamFloat);
						InvalidateRect(hWnd,NULL,FALSE);
					}
					break;

				case ID_FILTER_SHARPEN:
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FILTER),
						hWnd, FilterDlgProc) == TRUE) {
						iluSharpen(FilterParamFloat, 1);
						InvalidateRect(hWnd,NULL,FALSE);
					}
					break;

				case ID_EFFECTS_FILTERS_SCALE:
					HWnd = hWnd;
					iluImageParameter(ILU_FILTER, ILU_BILINEAR);
					DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_RESIZE), hWnd, ResizeDlgProc);
					break;

		}
		break;



	default:   // Passes it on if unproccessed
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


// Dialog procedure.
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

					GetWindowRect(HWnd, &Rect);
					SetWindowPos(HWnd, HWND_TOP, Rect.left, Rect.top, Width, Height + 46, SWP_SHOWWINDOW);

					InvalidateRect(HWnd, NULL, FALSE);
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