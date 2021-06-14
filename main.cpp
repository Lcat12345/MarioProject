#include <windows.h>		//--- 윈도우 헤더 파일
#include <tchar.h>
#include <atlimage.h>

typedef struct grid {
	int x, y;
}Grid;

bool Lio_jump_downck1 = false;
bool Lio_jump_downck2 = false;

bool Easy_jump_downck1 = false;
bool Easy_jump_downck2 = false;

typedef struct charcter {
	Grid currPos;
	Grid hitBox;
	int pose;
	int hit_count_standard;
	BOOL hit;
}Character;

float Lio_Velocity = 300.f;				//	점프하는 힘
float Lio_Gravity = 300.f;				//	낙하힘으로 상수값
BOOL Lio_bJumpKeyPressed = FALSE;
float Lio_JumpHeight = 400.0f;

float Easy_Velocity = 300.f;				//	점프하는 힘
float Easy_Gravity = 300.f;				//	낙하힘으로 상수값
BOOL Easy_bJumpKeyPressed = FALSE;
float Easy_JumpHeight = 400.0f;

void Lio_Jump(void)
{
	if (!Lio_bJumpKeyPressed)		return;


	if (Lio_Velocity == 0.f)
	{
		Lio_jump_downck1 = true;
	}

	//	착지하면 변수들 모두 초기화
	if (Lio_Velocity <= -300.f)
	{
		Lio_Velocity = 300.f;
		Lio_bJumpKeyPressed = FALSE;
		Lio_JumpHeight = 400.0f;
		Lio_jump_downck2 = true;
	}

	Lio_JumpHeight -= Lio_Velocity * 0.04f;
	Lio_Velocity -= Lio_Gravity * 0.04f;
}

void Easy_Jump(void)
{
	if (!Easy_bJumpKeyPressed)		return;

	if (Easy_Velocity == 0.f)
	{
		Easy_jump_downck1 = true;
	}

	//	착지하면 변수들 모두 초기화
	if (Easy_Velocity <= -300.f)
	{
		Easy_Velocity = 300.f;
		Easy_bJumpKeyPressed = FALSE;
		Easy_JumpHeight = 400.0f;
		Easy_jump_downck2 = true;
	}

	Easy_JumpHeight -= Easy_Velocity * 0.04f;
	Easy_Velocity -= Easy_Gravity * 0.04f;
}

BOOL collide(int Lio, int Easy) {
	if (Easy - Lio < 90) {
		return FALSE;
	}
	else {
		return TRUE;
	}
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
	HFONT font, oldfont;
	static CImage lio, walk, bg, riohpbar, riohp, hurt, Easy_win, easyhp, attack, attack2, jump;
	static CImage easy, Easy_walk, Easy_jump, Easy_attack, Easy_hurt;
	static Character Lio, Easy;
	static HBITMAP hBitmap;
	static int Lio_offset, Easy_offset;
	static int game_win;
	static BOOL Attack, Easy_Attack;

	static int rio_hp;
	static int easy_hp;
	static int time;

	static int Easy_hit_timer;

	TCHAR str[] = L"90";

	switch (iMessage) {
	case WM_CREATE:
		lio.Load(L"mario\\stand.png");	easy.Load(L"res\\Easy_stand.png");
		walk.Load(L"mario\\run.png");	Easy_walk.Load(L"res\\Easy_run.png");
		hurt.Load(L"mario\\hurt.png");	Easy_jump.Load(L"res\\Easy_jump.png");
		Easy_win.Load(L"mario\\Easy_win.png");	Easy_hurt.Load(L"res\\Easy_hurt.png");
		bg.Load(L"BG.jpg");
		riohpbar.Load(L"mario\\hp_bar.png");
		riohp.Load(L"mario\\hp.png");
		easyhp.Load(L"mario\\hp.png");
		attack.Load(L"mario\\attack.png");
		attack2.Load(L"mario\\attack2.png");
		jump.Load(L"mario\\jump.png");

		Lio.currPos.x = 200;	Easy.currPos.x = 600;
		Lio.pose = 0;	Easy.pose = 0;
		Lio_offset = 0;
		Lio.hit = FALSE;	Easy.hit = FALSE;
		Lio.hit_count_standard = 0;	Easy.hit_count_standard = 0;

		game_win = 0;
		time = 90;

		rio_hp = 350;
		easy_hp = 350;

		Easy_hit_timer = 0;

		SetTimer(hWnd, 1, 80, NULL);
		SetTimer(hWnd, 2, 1, NULL);
		SetTimer(hWnd, 3, 1000, NULL);
		SetTimer(hWnd, 4, 0.5, NULL);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hBitmap = CreateCompatibleBitmap(hdc, 800, 600);
		memdc = CreateCompatibleDC(hdc);
		Lio_Jump();
		Easy_Jump();

		SelectObject(memdc, hBitmap);

		//bg
		bg.Draw(memdc, 0, 0, 800, 600, 0, 0, 1200, 581);

		Rectangle(memdc, 365, 25, 419, 66);

		wsprintf(str, TEXT("%d"), time);

		font = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"휴먼옛체");
		oldfont = (HFONT)SelectObject(memdc, font);
		SetBkMode(memdc, TRANSPARENT);
		TextOut(memdc, 367, 20, str, lstrlen(str));
		SelectObject(memdc, oldfont);
		DeleteObject(font);

		riohpbar.Draw(memdc, 5, 25, 356, 39, 0, 0, 124, 14);

		if (rio_hp > 0) {
			riohp.Draw(memdc, 8, 28, rio_hp, 33, 0, 0, 120, 10);
		}

		riohpbar.Draw(memdc, 423, 25, 356, 39, 0, 0, 124, 14); //이지 막대바, 삭제 필요

		if (easy_hp > 0) {
			easyhp.Draw(memdc, 426, 28, easy_hp, 33, 0, 0, 120, 10);
		}

		//Lio
		if (Lio.pose == 0) {
			//lio
			lio.Draw(memdc, Lio.currPos.x, Lio_JumpHeight, 25 * 4, 36 * 4, Lio_offset * 26, 0, 25, 36);
		}
		else if (Lio.pose == 1) {
			//walk
			walk.Draw(memdc, Lio.currPos.x, Lio_JumpHeight, 35 * 4, 36 * 4, Lio_offset * 35, 0, 35, 36);
		}
		else if (Lio.pose == 5) {
			//hult
			hurt.Draw(memdc, Lio.currPos.x, Lio_JumpHeight, 35 * 4, 36 * 4, 0, 0, 30, 38);
		}
		else if (Lio.pose == 6) {
			//hult
			hurt.Draw(memdc, Lio.currPos.x, Lio_JumpHeight, 35 * 4, 36 * 4, 0, 0, 30, 38);
		}
		else if (Lio.pose == 7) {
			//attack
			attack.Draw(memdc, Lio.currPos.x, Lio_JumpHeight, 40 * 4, 36 * 4, Lio_offset * 40, 0, 40, 36);
		}
		else if (Lio.pose == 8) {
			//jump1
			jump.Draw(memdc, Lio.currPos.x, Lio_JumpHeight, 25 * 4, 36 * 4, 0, 0, 25, 39);
		}
		else if (Lio.pose == 9) {
			//jump2
			jump.Draw(memdc, Lio.currPos.x, Lio_JumpHeight, 25 * 4, 36 * 4, 25, 0, 25, 39);
		}
		else if (Lio.pose == 10) {
			//attack2
			attack2.Draw(memdc, Lio.currPos.x, Lio_JumpHeight, 40 * 4, 36 * 4, Lio_offset * 40, 0, 40, 36);
		}

		//Easy
		if (Easy.pose == 0) {	//stand
			easy.Draw(memdc, Easy.currPos.x, Easy_JumpHeight, 25 * 4, 36 * 4, Easy_offset * 22, 0, 22, 42);
		}
		else if (Easy.pose == 1) {	//walk
			Easy_walk.Draw(memdc, Easy.currPos.x, Easy_JumpHeight, 35 * 4, 36 * 4, Easy_offset * 35, 0, 35, 40);
		}
		else if (Easy.pose == 5) {	//hurt
			Easy_hurt.Draw(memdc, Easy.currPos.x, Easy_JumpHeight, 35 * 4, 36 * 4, 0, 0, 35, 40);
		}
		else if (Easy.pose == 8) {
			Easy_jump.Draw(memdc, Easy.currPos.x, Easy_JumpHeight, 30 * 4, 36 * 4, 0, 0, 33, 43);
		}
		else if (Easy.pose == 9) {
			Easy_jump.Draw(memdc, Easy.currPos.x, Easy_JumpHeight, 30 * 4, 36 * 4, 33, 0, 33, 43);
		}

		if (game_win == 1) {
			Easy_win.Draw(memdc, 285, 250, 230, 100, 0, 0, 230, 100);
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);
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
			if (Lio.pose == 0) {
				Lio_offset++;
				Lio_offset = Lio_offset % 7;
			}
			else if (Lio.pose == 1) {
				Lio_offset++;
				Lio_offset = Lio_offset % 8;
			}

			if (Easy.pose == 0) {
				Easy_offset++;
				Easy_offset = Easy_offset % 6;
			}
			else if (Easy.pose == 1) {
				Easy_offset++;
				Easy_offset = Easy_offset % 7;
			}

			break;
		case 2:
			//Lio
			if (Attack == FALSE) {
				if (GetAsyncKeyState(0x44) & 0x8000) {
					if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
						Lio.pose = 8;
						Lio_bJumpKeyPressed = TRUE;
					}

					if (Lio_bJumpKeyPressed == FALSE) {
						Lio.pose = 1;
					}
					if (Lio.currPos.x < 680 && collide(Lio.currPos.x,Easy.currPos.x)) {
						Lio.currPos.x += 2;
					}
				}
				else if (GetAsyncKeyState(0x41)) {
					if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
						Lio.pose = 8;
						Lio_bJumpKeyPressed = TRUE;
					}

					if (Lio_bJumpKeyPressed == FALSE) {
						Lio.pose = 1;
					}

					if (Lio.currPos.x > 0) {
						Lio.currPos.x -= 2;
					}
				}
				else if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
					Lio.pose = 8; //jump
					Lio_bJumpKeyPressed = TRUE;
				}
				else if (GetAsyncKeyState(0x47)) {	//attak
					Attack = TRUE;
					//Easy 피격판정
					if (!collide(Lio.currPos.x + 25, Easy.currPos.x)) {
						easy_hp -= 10;
						Easy.hit = TRUE;
						Easy.pose = 5;
						SetTimer(hWnd, 5, 500, NULL);
					}
					if (Lio.hit_count_standard == 0) {
						//기본공격 1
						Lio.pose = 7;
						Lio_offset = 0;
						SetTimer(hWnd, 4, 80, NULL);
					}
					else if (Lio.hit_count_standard == 1) {
						//기본 공격 2

					}
				}
				else {
					if (Lio_bJumpKeyPressed != TRUE) {
						Lio.pose = 0;
						//Lio_offset = 0;
					}
				}
			}

			if (Lio_jump_downck1 == true) {
				Lio.pose = 9;
				Lio_jump_downck1 = false;
			}

			if (Lio_jump_downck2 == true) {
				Lio.pose = 0;
				Lio_jump_downck2 = false;
			}

			if (rio_hp < 0) {
				Lio.pose = 6;
				game_win = 1;
			}
			//end of lio

			//Easy
			if (Easy_Attack == FALSE && Easy.hit==FALSE) {
				if (GetAsyncKeyState(VK_LEFT) & 0x8000) {	//walk Left
					if (GetAsyncKeyState(0x4B) & 0x8000) {
						Easy.pose = 8;
						Easy_bJumpKeyPressed = TRUE;
					}

					if (Easy_bJumpKeyPressed == FALSE) {
						Easy.pose = 1;
					}

					if (Easy.currPos.x > 0 && collide(Lio.currPos.x,Easy.currPos.x)) {
						Easy.currPos.x -= 2;
					}
				}
				else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {	//walk right
					if (GetAsyncKeyState(0x4B) & 0x8000) {
						Easy.pose = 8;
						Easy_bJumpKeyPressed = TRUE;
					}

					if (Easy_bJumpKeyPressed == FALSE) {
						Easy.pose = 1;
					}
					if (Easy.currPos.x < 680 && collide(Lio.currPos.x, Easy.currPos.x)) {
						Easy.currPos.x += 2;
					}
				}
				else if (GetAsyncKeyState(0x4B) & 0x8000) {
					Easy.pose = 8; //jump
					Easy_bJumpKeyPressed = TRUE;
				}
				else {
					if (Easy_bJumpKeyPressed != TRUE) {
						Easy.pose = 0;
						//Lio_offset = 0;
					}
				}
			}

			if (Easy_jump_downck1 == true) {
				Easy.pose = 9;
				Easy_jump_downck1 = false;
			}

			if (Easy_jump_downck2 == true) {
				Easy.pose = 0;
				Easy_jump_downck2 = false;
			}

			//end of Easy
			InvalidateRect(hWnd, NULL, false);
			break;
		case 3:
			--time;

			break;
		case 4:	//attack
			Lio_offset++;
			if (Lio_offset == 5) {
				Lio_offset = 0;
				Attack = FALSE;
				Lio.pose = 0;
				KillTimer(hWnd, 4);
			}
			break;
		case 5:
			if (Easy_hit_timer == 2) {
				Easy.hit = FALSE;
				Easy_hit_timer = 0;
				Easy.pose = 0;
				KillTimer(hWnd, 5);
			}
			Easy_hit_timer++;
			break;
		}
		break;
	case WM_DESTROY:
		lio.Destroy();
		KillTimer(hWnd, 1);	// 애니메이션 오프셋 움직이는 타이머
		KillTimer(hWnd, 2);	//가장 빠른 타이머
		KillTimer(hWnd, 3);	//게임 시간 세는 타이머
		KillTimer(hWnd, 4);	//마리오 기본 공격 타이머 1
		KillTimer(hWnd, 5);	//루이지 피격 회복 타이머
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
