#include "exDLL.h"

#define SetConfig(x) \
	cfg.txColor = x.txColor;\
	cfg.bgColor = x.bgColor;\
	cfg.tabs = x.tabs;\
	cfg.layer = x.layer;\
	cfg.margin_x = x.margin_x;\
	cfg.margin_y = x.margin_y;

HWND ghApply;

void SEND_SELCHANGE () {
	static NMHDR nm;
	nm.hwndFrom = ghConfig;
	nm.idFrom = 0;
	nm.code = TCN_SELCHANGE;
	PostMessage(ghConfig, WM_NOTIFY, (WPARAM)NULL, (LPARAM)&nm);
	return;
}

LRESULT CALLBACK Config_Proc (HWND shConfig, UINT msg, WPARAM wp, LPARAM lp) {
	static HWND hTab, hLabel, hButton[3];
	TCITEM tc_item;
	RECT rc;
	HGDIOBJ font;
	static char pre_tab = ID_TAB_LAYER;
	static sConfig tcfg;

	switch (msg) {
		case WM_CREATE:
			font = GetStockObject(DEFAULT_GUI_FONT);
			InitCommonControls();
			GetClientRect(shConfig ,&rc);
			ghTab = hTab = CreateWindowEx(
				0,
				WC_TABCONTROL,
				NULL,
				WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|TCS_FOCUSNEVER,
				0, 0,
				(int)rc.right, 25,
				shConfig,
				0,
				ghInstance,
				NULL
			);
			if (hTab==NULL) return -1;
			SendMessage(hTab, WM_SETFONT, (WPARAM)font, 0);
			tc_item.mask = TCIF_TEXT;
			tc_item.pszText = TEXT("文字色");
			TabCtrl_InsertItem(hTab, ID_TAB_TXCOLOR, &tc_item);
			tc_item.pszText = TEXT("背景色");
			TabCtrl_InsertItem(hTab, ID_TAB_BGCOLOR, &tc_item);
			tc_item.pszText = TEXT("透過率");
			TabCtrl_InsertItem(hTab, ID_TAB_LAYER, &tc_item);
			tc_item.pszText = TEXT("タブ幅");
			TabCtrl_InsertItem(hTab, ID_TAB_TABS, &tc_item);
			tc_item.pszText = TEXT("横マージン");
			TabCtrl_InsertItem(hTab, ID_TAB_MARGIN_X, &tc_item);
			tc_item.pszText = TEXT("縦マージン");
			TabCtrl_InsertItem(hTab, ID_TAB_MARGIN_Y, &tc_item);
			TabCtrl_SetCurSel(hTab, ID_TAB_LAYER);
			hButton[0] = CreateWindow(
				TEXT("BUTTON"),
				TEXT("OK"),
				WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|BS_NOTIFY,
				rc.right-(80+5), rc.bottom-(20+5),
				80, 20,
				shConfig,
				(HMENU)ID_BUTTON_OK,
				ghInstance,
				NULL
			);
			if (hButton[0]==NULL) return -1;
			SendMessage(hButton[0], WM_SETFONT, (WPARAM)font, 0);
			ghApply = hButton[1] = CreateWindow(
				TEXT("BUTTON"),
				TEXT("適用"),
				WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_NOTIFY,
				rc.right-(80+5+80+5), rc.bottom-(20+5),
				80, 20,
				shConfig,
				(HMENU)ID_BUTTON_APPLY,
				ghInstance,
				NULL
			);
			if (hButton[1]==NULL) return -1;
			SendMessage(hButton[1], WM_SETFONT, (WPARAM)font, 0);
			hButton[2] = CreateWindow(
				TEXT("BUTTON"),
				TEXT("キャンセル"),
				WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_NOTIFY,
				rc.right-(80+5+80+5+80+5), rc.bottom-(20+5),
				80, 20,
				shConfig,
				(HMENU)ID_BUTTON_CANCEL,
				ghInstance,
				NULL
			);
			if (hButton[2]==NULL) return -1;
			SendMessage(hButton[2], WM_SETFONT, (WPARAM)font, 0);
			ghText = CreateWindow(
				TEXT("EDIT"),
				NULL,
				WS_CHILD|WS_VISIBLE|ES_LEFT|WS_BORDER|ES_MULTILINE,
				rc.right-(80+30+5), rc.bottom-(20+80+5),
				22, 16,
				shConfig,
				(HMENU)ID_BUTTON_CANCEL,
				ghInstance,
				NULL
			);
			if (ghText==NULL) return -1;
			SendMessage(ghText, WM_SETFONT, (WPARAM)font, 0);
			SendMessage(ghText, EM_LIMITTEXT, (WPARAM)2, 0);
			g_TextProc = (WNDPROC)SetWindowLongPtr(ghText, GWLP_WNDPROC, (LONG_PTR)Text_Proc);
			hLabel = CreateWindow(
				TEXT("STATIC"),
				TEXT("0x"),
				WS_CHILD|WS_VISIBLE|SS_RIGHT,
				rc.right-(30+3+80+30+5), rc.bottom-(18+80+5),
				30, 16,
				shConfig,
				(HMENU)ID_LABEL,
				ghInstance,
				NULL
			);
			if (hLabel==NULL) return -1;
			SendMessage(hLabel, WM_SETFONT, (WPARAM)font, 0);
			break;
		case WM_SHOWWINDOW:
			if (wp==TRUE) {
				EnableWindow(ghWnd, FALSE);
				memcpy(&tcfg, &cfg, sizeof(sConfig));
				SetValue(&tcfg, pre_tab);
				SetFocus(ghText);
			}
			else {
				EnableWindow(ghWnd, TRUE);
				SetFocus(ghWnd);
			}
			break;
		case WM_NOTIFY:
			switch (((NMHDR*)lp)->code) {
			case TCN_SELCHANGE:
				GetValue(&tcfg, pre_tab);
				pre_tab = TabCtrl_GetCurSel(hTab);
				SetValue(&tcfg, pre_tab);

				switch (pre_tab) {
				case ID_TAB_TXCOLOR:
				case ID_TAB_BGCOLOR:
					SetWindowText(hLabel, TEXT("0x00"));
					SetWindowPos(ghText, 0, 0, 0, 55, 16, SWP_NOMOVE);
					SendMessage(ghText, EM_LIMITTEXT, (WPARAM)6, 0);
					//文字列
					break;
				case ID_TAB_TABS:
				case ID_TAB_LAYER:
				case ID_TAB_MARGIN_X:
				case ID_TAB_MARGIN_Y:
					SetWindowText(hLabel, TEXT("0x"));
					SetWindowPos(ghText, 0, 0, 0, 22, 16, SWP_NOMOVE);
					SendMessage(ghText,  EM_LIMITTEXT, (WPARAM)2, 0);
					//文字列
					break;
				}
				break;
			}
			break;
		case WM_COMMAND:
			if (HIWORD(wp)==BN_CLICKED) {
				SetFocus(ghConfig);

				switch (LOWORD(wp)) {
				case ID_BUTTON_OK:
					GetValue(&tcfg, pre_tab);
					SetConfig(tcfg);
					f = -1;
					InvalidateRect(ghEdit, NULL, TRUE);
					PostMessage(ghConfig, WM_CLOSE, 0, 0);
					break;
				case ID_BUTTON_APPLY:
					GetValue(&tcfg, pre_tab);
					SetConfig(tcfg);
					f = -1;
					InvalidateRect(ghEdit, NULL, TRUE);
					break;
				case ID_BUTTON_CANCEL:
					PostMessage(ghConfig, WM_CLOSE, 0, 0);
					break;
				}
			}
			break;
		case WM_KEYDOWN:
			if (wp!=VK_ESCAPE) break;
		case WM_CLOSE:
			ShowWindow(ghConfig, SW_HIDE);
			return 0;
		case WM_DESTROY:
			DestroyWindow(hTab);
			DestroyWindow(hButton[0]);
			DestroyWindow(hButton[1]);
			DestroyWindow(ghText);
			DestroyWindow(hLabel);
			break;
		}

	return DefWindowProc(shConfig, msg, wp, lp);
}


LRESULT CALLBACK Text_Proc (HWND shText, UINT msg, WPARAM wp, LPARAM lp) {
	int n;
	switch (msg) {
	case WM_KEYDOWN:
		if (wp==VK_ESCAPE) SendMessage(ghConfig, WM_CLOSE, 0, 0);
		else if (wp==VK_RETURN) {
			SendMessage(ghApply, BM_CLICK, 0, 0);
			SetFocus(ghText);
			return -1;
		}
		break;
	case WM_CHAR:
		if (wp>='a' && wp<='f') wp -= 0x20;
		else if (wp==0x1B) return -1;
		else if (wp==0x08) break;
		else if (wp==0x09) {
			n = TabCtrl_GetCurSel(ghTab);
			TabCtrl_SetCurSel(ghTab, n==ID_TAB_MARGIN_Y?ID_TAB_TXCOLOR:n+1);
			//TCN_SELCHANGEでちゃんと送るべし
			SEND_SELCHANGE();
		}
		else if (wp<'0' || (wp>'9' && wp<'A') || wp>'F') {
			MessageBeep(MB_OK);
			return -1;
		}
		break;
	}
	return CallWindowProc(g_TextProc, shText, msg, wp, lp);
}
