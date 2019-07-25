#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")

#define MAIN_SOURCE
#include "exDLL.h"
#include "forcl.h"

BOOL WINAPI DllMain (HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
			ghInstance = hInstance;
			GetModuleFileName(hInstance, path, sizeof(path));
			PathRenameExtension(path, TEXT(".inf"));
			ghWnd = FindWindow(TEXT("Notepad"), NULL);
			g_WndProc = (WNDPROC)SetWindowLongPtr(ghWnd, GWLP_WNDPROC, (LONG_PTR)Main_Proc);
			SetWindowLongPtr(ghWnd, GWL_EXSTYLE, GetWindowLongPtr(ghWnd, GWL_EXSTYLE)|WS_EX_LAYERED);
			SetActiveWindow(ghWnd);
			break;
		}
	return TRUE;
	}


LRESULT CALLBACK Main_Proc (HWND shWnd, UINT msg, WPARAM wp, LPARAM lp) {
	static HBRUSH hBrush;
	static HMENU hMenu, hAlign, hmBack;
	static char s = 0;
	static HBITMAP def = NULL;
	HBITMAP pre;
	RECT rc;
	HDC hdc;

	switch (msg) {
		case WM_ERASEBKGND:
			return FALSE;

		case WM_CTLCOLOREDIT:
			if (s==0) { //WM_CREATEの代わりに色々最初にやること
				LoadConfig();
				ComposeMenu(&hMenu, &hAlign, &hmBack);
				DrawMenuBar(ghWnd);
				CheckMenuRadioItem(hAlign, 0, 2, 0, MF_BYPOSITION);
				esc_add = ToggleMenuItem(hMenu, ID_ESCAPE_ADD)?1:0;
				ghEdit = (HWND)lp;
				g_EditProc = (WNDPROC)SetWindowLongPtr(ghEdit, GWLP_WNDPROC, (LONG_PTR)Edit_Proc);
				GetClientRect(ghEdit, &rc);
				rc.left = 5;
				rc.top = 5;
				rc.right -= 5;
				rc.bottom -= 5;
				SendMessage(ghEdit, EM_SETRECT, (WPARAM)0, (LPARAM)&rc);

				GetWindowRect(ghEdit, &grc);
				grc.right -= grc.left;
				grc.bottom -= grc.top;
				ghBack = CreateBack(grc.left, grc.top, grc.right, grc.bottom);
				SetLayeredWindowAttributes(ghBack, 0, 0x00, LWA_ALPHA);
				SetForegroundWindow(shWnd);
				hdc = GetDC(ghBack);
				hmdc[0] = CreateCompatibleDC(hdc);
				ReleaseDC(ghBack, hdc);
				hdc = GetDC(NULL);
				hmdc[1] = CreateCompatibleDC(hdc);
				ReleaseDC(NULL, hdc);
				s = 1;
				}
			SetBkMode((HDC)wp, OPAQUE); //塗るモードを選択
			SetTextColor((HDC)wp, cfg.txColor);
			SetBkColor((HDC)wp, cfg.bgColor);
			if (f!=1) {
				if (f==-1) DeleteObject(hBrush);
				hBrush = CreateSolidBrush(cfg.bgColor);
				SendMessage(ghEdit, EM_SETTABSTOPS, (WPARAM)1, (LPARAM)&(cfg.tabs));
				SetLayeredWindowAttributes(ghWnd, 0, cfg.layer, LWA_ALPHA);
				GetClientRect(ghEdit, &rc);
				rc.left = cfg.margin_x;
				rc.right -= cfg.margin_x;
				rc.top = cfg.margin_y;
				rc.bottom -= cfg.margin_y;
				SendMessage(ghEdit, EM_SETRECT, (WPARAM)0, (LPARAM)&rc);
				f = 1;
				}
			return (LRESULT)hBrush; //全体の背景色を設定

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case ID_CONFIG:
					if (ghConfig==NULL) {
						GetWindowRect(ghWnd, &rc);
						ghConfig = CreateConfig(&rc);
						}
					if (ghConfig==NULL)
						ShellExecute(NULL, TEXT("open"), path, NULL, NULL, SW_SHOW);
					else
						ShowWindow(ghConfig, SW_SHOW);
					break;

				case ID_WRITECFG:
					WriteConfig();
					break;

					case ID_LOADCFG:
					LoadConfig();
					f = -1;
					InvalidateRect(ghEdit, NULL, TRUE);
					break;

				case ID_BG_SS:
					sh = 1;
					GetWindowRect(ghEdit, &grc);
					grc.right -= grc.left;
					grc.bottom -= grc.top;
					SetLayeredWindowAttributes(shWnd, 0, 0x00, LWA_ALPHA);
					SetLayeredWindowAttributes(ghBack, 0, 0x00, LWA_ALPHA);
					if (hsBmp!=NULL) {
						SelectObject(hmdc[0], def);
						DeleteObject(hsBmp);
						hsBmp = NULL;
						}
					hdc = GetDC(NULL);
					hsBmp = CreateCompatibleBitmap(hdc, grc.right, grc.bottom);
					pre = (HBITMAP)SelectObject(hmdc[1], hsBmp);
					BitBlt(hmdc[1], 0, 0, grc.right, grc.bottom, hdc, grc.left, grc.top, SRCCOPY);
					ReleaseDC(NULL, hdc);
					SelectObject(hmdc[1], pre);
					if (def==NULL) def = SelectObject(hmdc[0], hsBmp);
					else SelectObject(hmdc[0], hsBmp);
					SetLayeredWindowAttributes(ghBack, 0, 0xFF, LWA_ALPHA);
					SetLayeredWindowAttributes(shWnd, 0, cfg.layer, LWA_ALPHA);
					InvalidateRect(ghBack, NULL, TRUE);
					CheckMenuRadioItem(hmBack, 0, 2, 0, MF_BYPOSITION);
					break;

				case ID_BG_NOTUSE:
					SetLayeredWindowAttributes(ghBack, 0, 0x00, LWA_ALPHA);
					CheckMenuRadioItem(hmBack, 0, 2, 2, MF_BYPOSITION);
					sh = 0;
					break;

				case ID_LEFT:
					CheckMenuRadioItem(hAlign, 0, 2, 0, MF_BYPOSITION);
					SetWindowLongPtr(ghEdit, GWL_STYLE, SetEditStyle(GetWindowLongPtr(ghEdit, GWL_STYLE), SES_LEFT));
					InvalidateRect(ghEdit, NULL, TRUE);
					break;

				case ID_CENTER:
					CheckMenuRadioItem(hAlign, 0, 2, 1, MF_BYPOSITION);
					SetWindowLongPtr(ghEdit, GWL_STYLE, SetEditStyle(GetWindowLongPtr(ghEdit, GWL_STYLE), SES_CENTER));
					InvalidateRect(ghEdit, NULL, TRUE);
					break;

				case ID_RIGHT:
					CheckMenuRadioItem(hAlign, 0, 2, 2, MF_BYPOSITION);
					SetWindowLongPtr(ghEdit, GWL_STYLE, SetEditStyle(GetWindowLongPtr(ghEdit, GWL_STYLE), SES_RIGHT));
					InvalidateRect(ghEdit, NULL, TRUE);
					break;

				case ID_FOREGROUND:
					SetWindowPos(ghWnd, ToggleMenuItem(hMenu, ID_FOREGROUND)?HWND_TOPMOST:HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
					break;

				case ID_ESCAPE_ADD:
					esc_add = ToggleMenuItem(hMenu, ID_ESCAPE_ADD)?1:0;
					break;

				case ID_REDESCRIBE:
					f = -1;
					InvalidateRect(ghEdit, NULL, TRUE);
					break;
					}
			break;

		case WM_MOVE:
		case WM_SIZE:
			if (sh==1) {
				GetWindowRect(ghEdit, &grc);
				grc.right -= grc.left;
				grc.bottom -= grc.top;
				SetWindowPos(ghBack, ghWnd, grc.left, grc.top, grc.right, grc.bottom, SWP_NOACTIVATE);
				}
			break;

		case WM_SIZING:
			GetClientRect(ghEdit, &rc);
			rc.left = 5;
			rc.top = 5;
			rc.right -= 5;
			rc.bottom -= 5;
			SendMessage(ghEdit, EM_SETRECT, (WPARAM)0, (LPARAM)&rc);
			break;

		case WM_ACTIVATEAPP:
			if (wp==TRUE)
				SetWindowPos(ghBack, shWnd, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
			break;

		case WM_DESTROY:
			if (hsBmp!=NULL) DeleteObject(hsBmp);
			if (hfBmp!=NULL) DeleteObject(hfBmp);
			DeleteDC(hmdc[1]);
			DeleteDC(hmdc[0]);
			DestroyWindow(ghBack);
			DestroyMenu(hAlign);
			DestroyMenu(hmBack);
			DestroyMenu(hMenu);
			DeleteObject(hBrush);
			break;
			}

	return CallWindowProc(g_WndProc, shWnd, msg, wp, lp);
	}

//EM_GETSEL,_SETSEL wpを先頭、lpを末尾として選択行を取得・設定する。_SETSELでwpを-1にすると選択状態を解除
//EM_LINEFROMCHAR 先頭からの距離wp(-1で現在キャリッジ)を含む行の番号
//EM_LINEINDEX 行数wp(-1で現在行)の行頭までのコントロール先頭からの距離を取得

#define swap(a, b) {a+=b;b=a-b;a-=b;}

LRESULT CALLBACK Edit_Proc (HWND shEdit, UINT msg, WPARAM wp, LPARAM lp) {
	DWORD line_top, dwCur[3], nLine[2];
	RECT rc;
	unsigned int i;

	switch (msg) {
		case WM_KEYDOWN:
				if (wp==VK_F12) PostMessage(ghWnd, WM_COMMAND, (WPARAM)ID_CONFIG, (LPARAM)0);
				break;
		case WM_CHAR:
			if (wp=='\t' && GetKeyState(VK_SHIFT)<0) {
				SendMessage(ghEdit, EM_GETSEL, (WPARAM)&dwCur[0], (LPARAM)&dwCur[1]); //dwCur[0]に選択状態の先頭、dwCur[1]に末尾までのコントロール先頭からの距離
				SendMessage(ghEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)0); //選択状態解除
				SendMessage(ghEdit, EM_GETSEL, (WPARAM)&dwCur[2], (LPARAM)NULL); //dwCur[2]に現在の位置までのコントロール先頭からの距離
	
				nLine[0] = SendMessage(ghEdit, EM_LINEFROMCHAR, (WPARAM)dwCur[0], (LPARAM)0);
				nLine[1] = SendMessage(ghEdit, EM_LINEFROMCHAR, (WPARAM)dwCur[1], (LPARAM)0);
				//nLine[0]にdwCur[0]の行数
				//nLine[1]にdwCur[1]の行数
				if (nLine[0]>nLine[1]) swap(nLine[0], nLine[1]);
	
				for (i = nLine[0]; i<=nLine[1]; ++i) {
					line_top = SendMessage(ghEdit, EM_LINEINDEX, (WPARAM)i, (LPARAM)0);
					SendMessage(ghEdit, EM_SETSEL, line_top, line_top);
 					SendMessage(ghEdit, EM_REPLACESEL, (WPARAM)1, (LPARAM)TEXT("\t"));
					}

				dwCur[1] += nLine[1]-nLine[0]+1;
				SendMessage(ghEdit, EM_SETSEL, (WPARAM)(dwCur[0]==dwCur[2]?dwCur[1]:dwCur[0]), (LPARAM)(dwCur[0]==dwCur[2]?dwCur[0]:dwCur[1]));
				return 0;
				}
			else if (wp==0x1B && esc_add) {
				PostMessage(ghWnd, WM_CLOSE, 0, 0);
				return 0;
				}
			break;
		}

	return CallWindowProc(g_EditProc, shEdit, msg, wp, lp);
	}