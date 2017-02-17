#include "winfuncs.h"

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define MAX_SCREENS 100

HWND hwnd;

void snapshot(int windowWidth, int windowHeight)
{
	char* bmpBuffer = (char*)malloc(windowWidth*windowHeight * 3);
	if (!bmpBuffer)
		return;

	glReadPixels((GLint)0, (GLint)0, (GLint)windowWidth - 1, (GLint)windowHeight - 1, GL_BGR, GL_UNSIGNED_BYTE, bmpBuffer);
	//glBindTexture(GL_TEXTURE_2D, 22);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, bmpBuffer);

	char *filename = new char[100];
	char *buf = new char[100];
	filename[0] = 0;
	int i;
	for (i = 0; i < MAX_SCREENS; i++)
	{
		buf[0] = 0;
		sprintf(buf, "%s%d%s", "screenshots/screen", i, ".bmp");
		FILE *F = fopen(buf, "r");
		if (!F)
			break;
		fclose(F);
	}
	FILE *filePtr = fopen(buf, "wb");
	if (!filePtr)
		return;

	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	bitmapFileHeader.bfType = 0x4D42;
	bitmapFileHeader.bfSize = windowWidth*windowHeight * 3;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits =
		sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = windowWidth - 1;
	bitmapInfoHeader.biHeight = windowHeight - 1;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB;
	bitmapInfoHeader.biSizeImage = 0;
	bitmapInfoHeader.biXPelsPerMeter = 0;
	bitmapInfoHeader.biYPelsPerMeter = 0;
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	fwrite(bmpBuffer, windowWidth*windowHeight * 3, 1, filePtr);
	fclose(filePtr);

	free(bmpBuffer);
}

void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBitmap)
{
	HBITMAP hOldbm;
	HDC hMemDC;
	BITMAP bm;
	POINT ptSize, ptOrg;

	hMemDC = CreateCompatibleDC(hdc);
	hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmap);
	if (hOldbm)
	{
		SetMapMode(hMemDC, GetMapMode(hdc));
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
		ptSize.x = bm.bmWidth;
		ptSize.y = bm.bmHeight;

		DPtoLP(hdc, &ptSize, 1);

		ptOrg.x = 0;
		ptOrg.y = 0;

		DPtoLP(hMemDC, &ptOrg, 1);

		BitBlt(hdc, x, y, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, SRCCOPY);

		SelectObject(hMemDC, hOldbm);
	}

	DeleteDC(hMemDC);
}

HBITMAP hBitmap;
BITMAP bm;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
					 //PAINTSTRUCT ps;
					 //HDC hdc = BeginPaint(hwnd, &ps);
					 //DrawBitmap(hdc, 0, 0, hBitmap);
					 //EndPaint(hwnd, &ps);
	}
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void load_screen_on(HINSTANCE hInstance, int nCmdShow)
{
	hBitmap = (HBITMAP)LoadImage(NULL,
		L"logo.bmp",
		IMAGE_BITMAP, 891, 215, LR_LOADFROMFILE);
	if (!hBitmap){
		MessageBox(NULL, L"File not found!", L"Error", MB_OK);
		exit(0);
	}

	const wchar_t CLASS_NAME[] = L"Loading";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0, CLASS_NAME, L"Loading", WS_OVERLAPPEDWINDOW,
		270, 270, 910, 240, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	DrawBitmap(hdc, 0, 0, hBitmap);
	EndPaint(hwnd, &ps);

	int i = 3;
	return;
}

void load_screen_off()
{
	CloseWindow(hwnd);
}

#include <io.h>
#include <fcntl.h>

bool CreateConsole()
{
	FreeConsole();
	if (AllocConsole())
	{
		//int hCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		freopen("CONOUT$", "w", stdout);
		/**stdout = *(::_fdopen(hCrt, "w"));
		::setvbuf(stdout, NULL, _IONBF, 0);
		*stderr = *(::_fdopen(hCrt, "w"));
<<<<<<< HEAD
		::setvbuf(stderr, NULL, _IONBF, 0);
		freopen("CONOUT$", "w", stdout);
=======
		::setvbuf(stderr, NULL, _IONBF, 0);*/
>>>>>>> origin/master
		return TRUE;
	}
	return FALSE;
}