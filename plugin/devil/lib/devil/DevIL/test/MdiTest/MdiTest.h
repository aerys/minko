#ifndef MDITEST_H
#define MDITEST_H

#include <windows.h>
#include <il\il.h>
#include <list>
#include <vector>
using namespace std;



HINSTANCE hInstance;
int CmdShow;
HWND MainHWnd, CurChildWnd;


#define TITLE		"OpenIL Windows Test"
#define CHILD		"OpenIL Child Window"


class cWindow
{
public:
	cWindow()
	{
		Width = 0;
		Height = 0;
		WinWidth = 0;
		WinHeight = 0;
		hWnd = 0;
		Bitmap = 0;
		hDC = 0;
		hMemDC = 0;
		sprintf(Title, "OpenIL Windows Test");
		return;
	}

	~cWindow()
	{
		return;
	}

	bool LoadImage(char *FileName)
	{
		Images.resize(1);

		ilGenImages(1, Images.begin());
		ilBindImage(Images[0]);
		if (!ilLoadImage(FileName)) {
			ilDeleteImages(1, Images.begin());
			return false;
		}

		Width = ilGetInteger(IL_IMAGE_WIDTH);
		Height = ilGetInteger(IL_IMAGE_HEIGHT);

		Create(FileName, 100, 100, Width + 8, Height + 23, MainHWnd);

		ILuint i = ilCloneCurImage();
		ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
		Bitmap = ilutConvertToHBitmap(hDC);
		ilDeleteImages(1, &i);
		SelectObject(hMemDC, Bitmap);

		sprintf(Title, FileName);
		SetWindowText(hWnd, Title);

		return true;
	}


	bool Create(char *Name, int x, int y, int width, int height, HWND Parent)
	{
		if (Parent) {
			hWnd = CreateWindow(CHILD, Name, WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS,
						x, y, width, height, Parent, NULL, hInstance, NULL);
		}
		else {
			hWnd = CreateWindow(TITLE, Name, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
						x, y, width, height, Parent, NULL, hInstance, NULL);
		}

		if (Parent == NULL)
			MainHWnd = hWnd;
		if (hWnd == NULL)
			return false;

		WinWidth = Width;
		WinHeight = Height;
		XPos = x;
		YPos = y;

		ShowWindow(hWnd, CmdShow);
		UpdateWindow(hWnd);
		InvalidateRect(hWnd, NULL, false);

		SetForegroundWindow(hWnd);
		SetFocus(hWnd);
		BringWindowToTop(hWnd);
		ShowWindow(hWnd, SW_SHOWNORMAL);

		CurChildWnd = hWnd;

		hDC = GetDC(hWnd);
		hMemDC = CreateCompatibleDC(hDC);

		return true;
	}

	void Destroy(void)
	{
		Width = 0;
		Height = 0;
		WinWidth = 0;
		WinHeight = 0;
		if (Bitmap)
			DeleteObject(Bitmap);
		if (hMemDC)
			DeleteDC(hMemDC);
		if (hDC && hWnd) {
			ReleaseDC(hWnd, hDC);
			DeleteDC(hDC);
		}
		if (hWnd)
			DestroyWindow(hWnd);
		if (Images.size()) {
			ilDeleteImages(Images.size(), Images.begin());
			Images.clear();
		}
		return;
	}

	HWND GetHWnd(void)
	{
		return hWnd;
	}

	void Paint(void)
	{
		if (!hWnd)
			return;
		PAINTSTRUCT ps;

		/*hDC = */BeginPaint(hWnd, &ps);
		BitBlt(hDC, 0, 0, (WORD)Width, (WORD)Height, hMemDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		ValidateRect(hWnd, NULL);
		ShowWindow(hWnd, SW_SHOW);

		return;
	}

	void BindImage()
	{
		if (Images.size()) {
			ilBindImage(*(Images.begin() + Images.size() - 1));
		}
		return;
	}

	void SetTitle(char *Name)
	{
		sprintf(Title, Name);
		SetWindowText(hWnd, Title);
	}


protected:
	ILuint	XPos, YPos, Width, Height;
	ILuint	WinWidth, WinHeight;
	char	Title[512];

	vector <ILuint> Images;
	ILuint	UndoSize;

	HWND	hWnd;
	HBITMAP	Bitmap;
	HDC		hDC, hMemDC;

};


cWindow	MainWindow;
list <cWindow> WindowList;


cWindow *GetWinClass(HWND hWnd);
void AddNewWindow(char *FileName);
void DestroyWindows(void);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY AboutDlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam);
BOOL APIENTRY FilterDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);
BOOL APIENTRY ResizeDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);
void GenFilterString(char *Out, char **Strings);
void ResizeWin(void);
void CreateGDI(void);
bool IsOpenable(char *FileName);


#endif//MDITEST_H
