#include "doomkeys.h"

#include "doomgeneric.h"

#include <stdio.h>

#include <Windows.h>

// for High precision sleep
#pragma comment( lib, "winmm.lib" )

static BITMAPINFO s_Bmi = { sizeof(BITMAPINFOHEADER), DOOMGENERIC_RESX, -DOOMGENERIC_RESY, 1, 32 };
static HWND s_Hwnd = 0;
static HDC s_Hdc = 0;


#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

// Aspect ratio 4:3 (640*480)
static unsigned int windowWidth = DOOMGENERIC_RESX;
static unsigned int windowHeight = DOOMGENERIC_RESY + 80;

static unsigned char convertToDoomKey(unsigned char key)
{
	switch (key)
	{
	case VK_BACK:
		key = KEY_BACKSPACE;
		break;
	case VK_RETURN:
		key = KEY_ENTER;
		break;
	case VK_SHIFT:
		key = KEY_RSHIFT;
		break;
	case VK_CONTROL:
		key = KEY_FIRE;
		break;
	case VK_MENU:
		key = KEY_RALT;
		break;
	case VK_PAUSE:
		key = KEY_PAUSE;
		break;
	case VK_ESCAPE:
		key = KEY_ESCAPE;
		break;
	case VK_SPACE:
		key = KEY_USE;
		break;
	case VK_LEFT:
		key = KEY_LEFTARROW;
		break;
	case VK_UP:
		key = KEY_UPARROW;
		break;
	case VK_RIGHT:
		key = KEY_RIGHTARROW;
		break;
	case VK_DOWN:
		key = KEY_DOWNARROW;
		break;
	case VK_DELETE:
		key = KEY_DEL;
		break;
	case VK_NUMPAD0:
		key = KEYP_0;
		break;
	case VK_NUMPAD1:
		key = KEYP_1;
		break;
	case VK_NUMPAD2:
		key = KEYP_2;
		break;
	case VK_NUMPAD3:
		key = KEYP_3;
		break;
	case VK_NUMPAD4:
		key = KEYP_4;
		break;
	case VK_NUMPAD5:
		key = KEYP_5;
		break;
	case VK_NUMPAD6:
		key = KEYP_6;
		break;
	case VK_NUMPAD7:
		key = KEYP_7;
		break;
	case VK_NUMPAD8:
		key = KEYP_8;
		break;
	case VK_NUMPAD9:
		key = KEYP_9;
		break;
	case VK_MULTIPLY:
		key = KEYP_MULTIPLY;
		break;
	case VK_ADD:
		key = KEYP_PLUS;
		break;
	case VK_SUBTRACT:
		key = KEYP_MINUS;
		break;
	case VK_DECIMAL:
		key = KEYP_PERIOD;
		break;
	case VK_DIVIDE:
		key = KEYP_DIVIDE;
		break;
	case VK_F1:
		key = KEY_F1;
		break;
	case VK_F2:
		key = KEY_F2;
		break;
	case VK_F3:
		key = KEY_F3;
		break;
	case VK_F4:
		key = KEY_F4;
		break;
	case VK_F5:
		key = KEY_F5;
		break;
	case VK_F6:
		key = KEY_F6;
		break;
	case VK_F7:
		key = KEY_F7;
		break;
	case VK_F8:
		key = KEY_F8;
		break;
	case VK_F9:
		key = KEY_F9;
		break;
	case VK_F10:
		key = KEY_F10;
		break;
	case VK_F11:
		key = KEY_F11;
		break;
	case VK_F12:
		key = KEY_F12;
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
	case WM_SYSKEYDOWN:
		addKeyToQueue(1, wParam);
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		addKeyToQueue(0, wParam);
		break;
	case WM_SIZE:
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam);
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
	rect.right = windowWidth;
	rect.bottom = windowHeight;
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

void DG_DrawFrame()
{
	StretchDIBits(s_Hdc, 0, 0, windowWidth, windowHeight, 0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, DG_ScreenBuffer, &s_Bmi, 0, SRCCOPY);
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

int main(int argc, char **argv)
{
	timeBeginPeriod(1);

	doomgeneric_Create(argc, argv);

	MSG msg;
	memset(&msg, 0, sizeof(msg));
	
	while (1)
	{
		if (PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            		if(GetMessage(&msg, NULL, 0, 0))
            		{
                		TranslateMessage(&msg);
                		DispatchMessageA(&msg);
            		}
			else
			{
				break;
			}
		}
		else
		{
			doomgeneric_Tick();
		}
	}

	timeEndPeriod(1);

    return 0;
}
