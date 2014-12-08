#ifndef _WINFUNCS_H_
#define _WINFUNCS_H_

#include "disable_warnings.h"

#include <Windows.h>

extern HWND hwnd;

void snapshot(int windowWidth, int windowHeight);
void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBitmap);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void load_screen_on(HINSTANCE hInstance, int nCmdShow);
void load_screen_off();
bool CreateConsole();

#endif