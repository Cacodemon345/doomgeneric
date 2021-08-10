

#include <stdio.h>
#include <math.h>

#include <Windows.h>
#include "doomkeys.h"

#include "doomgeneric.h"
static BITMAPINFO s_Bmi = { sizeof(BITMAPINFOHEADER), DOOMGENERIC_RESX, -DOOMGENERIC_RESY, 1, 32 };
static HWND s_Hwnd = 0;
static HDC s_Hdc = 0;


#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

static unsigned char convertToDoomKey(unsigned char key)
{
	switch (key)
	{
	case VK_RETURN:
		key = KEY_ENTER;
		break;
	case VK_ESCAPE:
		key = KEY_ESCAPE;
		break;
	case VK_LEFT:
		key = KEY_LEFTARROW;
		break;
	case VK_RIGHT:
		key = KEY_RIGHTARROW;
		break;
	case VK_UP:
		key = KEY_UPARROW;
		break;
	case VK_DOWN:
		key = KEY_DOWNARROW;
		break;
	case VK_CONTROL:
		key = KEY_FIRE;
		break;
	case VK_SPACE:
		key = KEY_USE;
		break;
	case VK_SHIFT:
		key = KEY_RSHIFT;
		break;
	default:
		key = tolower(key);
		break;
	}

	return key;
}

static void addKeyToQueue(int pressed, unsigned char keyCode)
{
	unsigned char key = convertToDoomKey(keyCode);

	unsigned short keyData = (pressed << 8) | key;

	s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
	s_KeyQueueWriteIndex++;
	s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		ExitProcess(0);
		break;
	case WM_KEYDOWN:
		addKeyToQueue(1, wParam);
		break;
	case WM_KEYUP:
		addKeyToQueue(0, wParam);
		break;
	default:
		return DefWindowProcA(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void DG_Init()
{
	// window creation
	const char windowClassName[] = "DoomWindowClass";
	const char windowTitle[] = "Doom";
	WNDCLASSEXA wc;

	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.style = 0;
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = 0;
	wc.hIcon = 0;
	wc.hCursor = 0;
	wc.hbrBackground = 0;
	wc.lpszMenuName = 0;
	wc.lpszClassName = windowClassName;
	wc.hIconSm = 0;

	if (!RegisterClassExA(&wc))
	{
		printf("Window Registration Failed!");

		exit(-1);
	}

	RECT rect;
	rect.left = rect.top = 0;
	rect.right = DOOMGENERIC_RESX;
	rect.bottom = DOOMGENERIC_RESY;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindowExA(0, windowClassName, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, 0, 0, 0, 0);
	if (hwnd)
	{
		s_Hwnd = hwnd;

		s_Hdc = GetDC(hwnd);
		ShowWindow(hwnd, SW_SHOW);
	}
	else
	{
		printf("Window Creation Failed!");

		exit(-1);
	}

	memset(s_KeyQueue, 0, KEYQUEUE_SIZE * sizeof(unsigned short));
}

#define LO						 85
#define MD						170
#define HI						255
static HPEN invisiblePen = NULL;
static HBRUSH textModeBrushes[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const RGBQUAD TextModePalette[16] =
{
	{  0,  0,  0, 0 },		// 0 black
	{ MD,  0,  0, 0 },		// 1 blue
	{  0, MD,  0, 0 },		// 2 green
	{ MD, MD,  0, 0 },		// 3 cyan
	{  0,  0, MD, 0 },		// 4 red
	{ MD,  0, MD, 0 },		// 5 magenta
	{  0, LO, MD, 0 },		// 6 brown
	{ MD, MD, MD, 0 },		// 7 light gray

	{ LO, LO, LO, 0 },		// 8 dark gray
	{ HI, LO, LO, 0 },		// 9 light blue
	{ LO, HI, LO, 0 },		// A light green
	{ HI, HI, LO, 0 },		// B light cyan
	{ LO, LO, HI, 0 },		// C light red
	{ HI, LO, HI, 0 },		// D light magenta
	{ LO, HI, HI, 0 },		// E yellow
	{ HI, HI, HI, 0 },		// F white
};
struct ColFillParams
{
	RECT rect;
	ULONG color;
};

static void RGBtoHSV(float r, float g, float b, float* h, float* s, float* v)
{
	float min, max, delta, foo;

	if (r == g && g == b)
	{
		*h = 0;
		*s = 0;
		*v = r;
		return;
	}

	foo = r < g ? r : g;
	min = (foo < b) ? foo : b;
	foo = r > g ? r : g;
	max = (foo > b) ? foo : b;

	*v = max;									// v

	delta = max - min;

	*s = delta / max;							// s

	if (r == max)
		*h = (g - b) / delta;					// between yellow & magenta
	else if (g == max)
		*h = 2 + (b - r) / delta;				// between cyan & yellow
	else
		*h = 4 + (r - g) / delta;				// between magenta & cyan

	*h *= 60;									// degrees
	if (*h < 0)
		*h += 360;
}

void HSVtoRGB(float* r, float* g, float* b, float h, float s, float v)
{
	int i;
	float f, p, q, t;

	if (s == 0)
	{ // achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;									// sector 0 to 5
	i = (int)floor(h);
	f = h - i;									// factorial part of h
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));

	switch (i)
	{
	case 0:		*r = v; *g = t; *b = p; break;
	case 1:		*r = q; *g = v; *b = p; break;
	case 2:		*r = p; *g = v; *b = t; break;
	case 3:		*r = p; *g = q; *b = v; break;
	case 4:		*r = t; *g = p; *b = v; break;
	default:	*r = v; *g = p; *b = q; break;
	}
}
typedef struct color
{
	uint32_t b : 8;
	uint32_t g : 8;
	uint32_t r : 8;
	uint32_t a : 8;
} color;
void DG_DrawFrame()
{
	MSG msg;
	memset(&msg, 0, sizeof(msg));
	while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	if (M_CheckParm("-4bit"))
	{
		UINT32 x = 0, y = 0, i = 0;
		color* dgPixBuf = (void*)DG_ScreenBuffer;
		for (y = 0; y < DOOMGENERIC_RESY; y += 4)
		{
			for (x = 0; x < DOOMGENERIC_RESX; x += 4)
			{
				float h = 0, s = 0, v = 0;
				float r = dgPixBuf[y * DOOMGENERIC_RESX + x].r / 255.f;
				float g = dgPixBuf[y * DOOMGENERIC_RESX + x].g / 255.f;
				float b = dgPixBuf[y * DOOMGENERIC_RESX + x].b / 255.f;
				struct ColFillParams fillcol;
				uint8_t attrib = 0;

				RGBtoHSV(r, g, b, &h, &s, &v);
				if (s != 0)
				{ // color
					HSVtoRGB(&r, &g, &b, h, 1, 1);
					if (r == 1)  attrib = 0x4;
					if (g == 1)  attrib |= 0x2;
					if (b == 1)  attrib |= 0x1;
					if (v > 0.6) attrib |= 0x8;
				}
				else
				{ // gray
					if (v < 0.33) attrib = 0x8;
					else if (v < 0.90) attrib = 0x7;
					else			   attrib = 0xF;
				}
				fillcol.rect.left = fillcol.rect.right = x;
				fillcol.rect.top = fillcol.rect.bottom = y;
				fillcol.color = attrib & 0xF;
				fillcol.rect.right += 4;
				fillcol.rect.bottom += 4;
				if (!textModeBrushes[0])
				{
					for (i = 0; i < 16; i++)
					{
						textModeBrushes[i] = CreateSolidBrush(RGB(TextModePalette[i].rgbRed, TextModePalette[i].rgbGreen, TextModePalette[i].rgbBlue));
					}
				}
				FillRect(s_Hdc, &fillcol.rect, textModeBrushes[fillcol.color & 0xF]);
			}
		}
	}
	else StretchDIBits(s_Hdc, 0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, 0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, DG_ScreenBuffer, &s_Bmi, 0, SRCCOPY);

	SwapBuffers(s_Hdc);
}

void DG_SleepMs(uint32_t ms)
{
	Sleep(ms);
}

uint32_t DG_GetTicksMs()
{
	return GetTickCount();
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
	if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
	{
		//key queue is empty

		return 0;
	}
	else
	{
		unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
		s_KeyQueueReadIndex++;
		s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

		*pressed = keyData >> 8;
		*doomKey = keyData & 0xFF;

		return 1;
	}
}

void DG_SetWindowTitle(const char * title)
{
	if (s_Hwnd)
	{
		SetWindowTextA(s_Hwnd, title);
	}
}