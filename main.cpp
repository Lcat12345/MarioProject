#include <windows.h>		//--- 윈도우 헤더 파일
#include <tchar.h>
#include <atlimage.h>

typedef struct grid {
	int x, y;
}Grid;

bool jump_downck1 = false;
bool jump_downck2 = false;

typedef struct charcter {
	Grid currPos;
	Grid hitBox;
	int pose;
}Character;

float Velocity = 300.f;				//	점프하는 힘
float Gravity = 300.f;				//	낙하힘으로 상수값
BOOL bJumpKeyPressed = FALSE;
float JumpHeight = 400.0f;

void Jump(void)
{
	if (!bJumpKeyPressed)		return;


	if (Velocity == 0.f)
	{
		jump_downck1 = true;
	}

	//	착지하면 변수들 모두 초기화
	if (Velocity <= -300.f)
	{
		Velocity = 300.f;
		bJumpKeyPressed = FALSE;
		JumpHeight = 400.0f;
		jump_downck2 = true;
	}

	JumpHeight -= Velocity * 0.04f;
	Velocity -= Gravity * 0.04f;
};

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Lio vs Easy";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);		// 윈도우 크기 변경 가능

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, memdc, bufferdc;
	PAINTSTRUCT ps;
	static CImage lio, walk, Easy_stand, Easy_run, Easy_attack_base, bg;
	static Character Lio, Easy;
	static HBITMAP hBitmap;
	static int offset;
	static BOOL jump, Attack;

	switch (iMessage) {
	case WM_CREATE:
		lio.Load(L"res\\Lio_stand.png");
		walk.Load(L"res\\Lio_walk.png");
		Easy_stand.Load(L"res\\Easy_stand.png");	Easy_run.Load(L"res\\Easy_run.png");
		Easy_attack_base.Load(L"res\\Easy_attack_base.png");
		bg.Load(L"res\\BG.jpg");
		Lio.currPos.x = 200;	Easy.currPos.x = 400;
		jump = FALSE;
		Attack = FALSE;
		Lio.pose = 0;	Easy.pose = 0;

		SetTimer(hWnd, 1, 80, NULL);
		SetTimer(hWnd, 2, 1, NULL);
		SetTimer(hWnd, 3, 100, NULL);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hBitmap = CreateCompatibleBitmap(hdc, 800, 600);
		memdc = CreateCompatibleDC(hdc);

		Jump();

		SelectObject(memdc, hBitmap);

		bg.Draw(memdc, 0, 0, 800, 600, 0, 0, 1200, 581);

		//Easy
		if (Easy.pose == 0) {	//stand
			Easy_stand.Draw(memdc, Easy.currPos.x, JumpHeight, 23 * 4, 39 * 4, 22*offset, 0, 23, 40);
		}
		else if (Easy.pose == 1) {	//run
			Easy_run.Draw(memdc, Easy.currPos.x, JumpHeight, 35 * 4, 39 * 4, offset * 35, 0, 35, 40);
		}
		else if (Easy.pose == 2) {	//attack
			Easy_attack_base.Draw(memdc, Easy.currPos.x, JumpHeight, 35 * 4, 39 * 4, offset * 91, 0, 90, 96);
		}
		else if (Easy.pose == 3) {	//jump

		}

		BitBlt(hdc, 0, 0, 800, 600, memdc, 0, 0, SRCCOPY);
		DeleteObject(hBitmap);
		DeleteDC(memdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			if (Easy.pose == 0) {
				offset++;
				offset = offset % 5;
			}
			else if (Easy.pose == 1) {
				offset++;
				offset = offset % 7;
			}
			break;
		case 2:
			if (Attack == FALSE) {
				if (GetAsyncKeyState(0x41) & 0x8000) {
					if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
						Easy.pose = 0;
						bJumpKeyPressed = TRUE;
					}
					Easy.pose = 1;
					Easy.currPos.x -= 3;
				}
				else if (GetAsyncKeyState(0x44))
				{
					if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
						Easy.pose = 0;
						bJumpKeyPressed = TRUE;
					}
					Easy.currPos.x += 3;
				}
				else if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
					Easy.pose = 0;
					bJumpKeyPressed = TRUE;
				}
				else if (GetAsyncKeyState(0x47)) {
					Attack = TRUE;
					Easy.pose = 2;
					offset = 0;
					SetTimer(hWnd, 3, 80, NULL);
				}
				else {
					Easy.pose = 0;
				}
			}	
			InvalidateRect(hWnd, NULL, false);
			break;
		case 3:	//attack
			offset++;
			if (offset == 4) {
				offset = 0;
				Attack = FALSE;
				KillTimer(hWnd, 3);
			}
			break;
		}
		break;
	case WM_DESTROY:
		lio.Destroy();
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		KillTimer(hWnd, 3);
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

