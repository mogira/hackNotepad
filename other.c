#include "exDLL.h"

#define SETMIISTR(m, ID_X, ST) \
	mii.fMask = MIIM_STRING|MIIM_ID;\
	mii.fType = MFT_STRING;\
	m.wID = ID_X;\
	m.dwTypeData = TEXT(ST);\
	m.cch = _tcslen(m.dwTypeData);

#define SETMIISEP(m) \
	m.fMask = MIIM_FTYPE;\
	m.fType = MFT_SEPARATOR;

#define SETMIISUB(m, SUB, ST)\
	m.fMask = MIIM_SUBMENU|MIIM_STRING;\
	m.hSubMenu = SUB;\
	m.dwTypeData = TEXT(ST);

void ComposeMenu (HMENU *hMenu, HMENU *align, HMENU *back) {
	MENUITEMINFO mii = {
		sizeof(MENUITEMINFO)
	};

	*align = CreatePopupMenu();
		SETMIISTR(mii, ID_LEFT, "¶‘µ‚¦(&L)");
		InsertMenuItem(*align, 0, 1, &mii);

		SETMIISTR(mii, ID_CENTER, "’†‰›‘µ‚¦(&C)");
		InsertMenuItem(*align, 1, 1, &mii);

		SETMIISTR(mii, ID_RIGHT, "‰E‘µ‚¦(&R)");
		InsertMenuItem(*align, 2, 1, &mii);

	*back = CreatePopupMenu();
		SETMIISTR(mii, ID_BG_SS, "Œ»Ý‚Ì”wŒi(&C)");
		InsertMenuItem(*back, 0, 1, &mii);

		SETMIISEP(mii);
		InsertMenuItem(*back, 1, 2, &mii);

		SETMIISTR(mii, ID_BG_NOTUSE, "Žg‚í‚È‚¢(&N)");
		InsertMenuItem(*back, 2, 1, &mii);

	*hMenu = CreatePopupMenu();
		SETMIISTR(mii, ID_CONFIG, "Ý’è(&C)");
		InsertMenuItem(*hMenu, 0, 1, &mii);

		SETMIISTR(mii, ID_WRITECFG, "‘‚«ž‚Ý(&W)");
		InsertMenuItem(*hMenu, 1, 1, &mii);

		SETMIISTR(mii, ID_LOADCFG, "“Ç‚Ýž‚Ý(&L)");
		InsertMenuItem(*hMenu, 2, 1, &mii);

		SETMIISEP(mii);
		InsertMenuItem(*hMenu, 3, 1, &mii);

		SETMIISUB(mii, *back, "”wŒiƒEƒBƒ“ƒhƒE(&B)");
		InsertMenuItem(*hMenu, 4, 1, &mii);

		SETMIISUB(mii, *align, "ƒeƒLƒXƒgˆÊ’u(&A)");
		InsertMenuItem(*hMenu, 5, 1, &mii);

		SETMIISTR(mii, ID_FOREGROUND, "í‚ÉÅ‘O–Ê(&T)");
		InsertMenuItem(*hMenu, 6, 1, &mii);

		SETMIISTR(mii, ID_ESCAPE_ADD, "ESC‚Å•Â‚¶‚é(&E)");
		InsertMenuItem(*hMenu, 7, 1, &mii);

		SETMIISEP(mii);
		InsertMenuItem(*hMenu, 8, 1, &mii);

		SETMIISTR(mii, ID_REDESCRIBE, "Ä•`‰æ(&R)");
		InsertMenuItem(*hMenu, 9, 1, &mii);


	SETMIISUB(mii, *hMenu, "’Ç‰Á‹@”\(&A)");
	InsertMenuItem(GetMenu(ghWnd), 5, 1, &mii);

	return;
	}

UINT ToggleMenuItem (HMENU hMenu, UINT id) {
	MENUITEMINFO mii = {sizeof(MENUITEMINFO), MIIM_STATE};

	GetMenuItemInfo(hMenu, id, FALSE, &mii);
	mii.fState = (mii.fState==MFS_CHECKED) ? MFS_UNCHECKED : MFS_CHECKED;
	SetMenuItemInfo(hMenu, id, FALSE, &mii);

	return mii.fState;
}


void LoadConfig (void) {
	TCHAR str[MAX_LENGTH], *x;

	GetPrivateProfileString(
		TEXT("SETTING"), TEXT("TEXT_COLOR"),
		TEXT("000000"), str,
		MAX_LENGTH,
		path
	);
	cfg.txColor = (COLORREF)_tcstol(str, &x, 16);
	if (x!=str+_tcslen(str)) cfg.txColor = 0x00000000;

	GetPrivateProfileString(
		TEXT("SETTING"), TEXT("BG_COLOR"),
		TEXT("FFFFFF"), str,
		MAX_LENGTH,
		path
	);
	cfg.bgColor = (COLORREF)_tcstol(str, &x, 16);
	if (x!=str+_tcslen(str)) cfg.bgColor = 0x00FFFFFF;

	GetPrivateProfileString(
		TEXT("SETTING"), TEXT("TABS"),
		TEXT("08"), str,
		MAX_LENGTH,
		path
	);
	cfg.tabs = (unsigned int)_tcstol(str, &x, 16);
	if (x!=str+_tcslen(str)) cfg.tabs = 0x08;
	cfg.tabs *= 4;

	GetPrivateProfileString(
		TEXT("SETTING"), TEXT("LAYER"),
		TEXT("FF"), str,
		MAX_LENGTH,
		path
	);
	cfg.layer = (unsigned int)_tcstol(str, &x, 16);
	if (x!=str+_tcslen(str)) cfg.layer = 0xFF;

	GetPrivateProfileString(
		TEXT("SETTING"), TEXT("MARGIN_X"),
		TEXT("00"), str,
		MAX_LENGTH,
		path
	);
	cfg.margin_x = (unsigned int)_tcstol(str, &x, 16);
	if (x!=str+_tcslen(str)) cfg.margin_x = 0x00;

	GetPrivateProfileString(
		TEXT("SETTING"), TEXT("MARGIN_Y"),
		TEXT("00"), str,
		MAX_LENGTH,
		path
	);
	cfg.margin_y = (unsigned int)_tcstol(str, &x, 16);
	if (x!=str+_tcslen(str)) cfg.margin_y = 0x00;

	return;
}


void WriteConfig (void) {
	TCHAR str[MAX_LENGTH];

	_stprintf(str, TEXT("%06X"), cfg.txColor);
	WritePrivateProfileString(
		TEXT("SETTING"), TEXT("TEXT_COLOR"),
		str,
		path
	);

	_stprintf(str, TEXT("%06X"), cfg.bgColor);
	WritePrivateProfileString(
		TEXT("SETTING"), TEXT("BG_COLOR"),
		str,
		path
	);

	_stprintf(str, TEXT("%02X"), cfg.tabs/4);
	WritePrivateProfileString(
		TEXT("SETTING"), TEXT("TABS"),
		str,
		path
	);

	_stprintf(str, TEXT("%02X"), cfg.layer);
	WritePrivateProfileString(
		TEXT("SETTING"), TEXT("LAYER"),
		str,
		path
	);

	_stprintf(str, TEXT("%02X"), cfg.margin_x);
	WritePrivateProfileString(
		TEXT("SETTING"), TEXT("MARGIN_X"),
		str,
		path
	);

	_stprintf(str, TEXT("%02X"), cfg.margin_y);
	WritePrivateProfileString(
		TEXT("SETTING"), TEXT("MARGIN_Y"),
		str,
		path
	);

	return;
}


HWND CreateConfig (RECT *rc) {
	WNDCLASSEX wc = {0};
	RECT arc = {0};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = Config_Proc;
	wc.hInstance = ghInstance;
	wc.hIcon = LoadIcon(NULL, IDI_INFORMATION);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("Config");
	if (!RegisterClassEx(&wc)) return NULL;

	arc.right = 300;
	arc.bottom = 200;
	AdjustWindowRect(&arc, WS_SYSMENU|WS_CAPTION, FALSE);

	return CreateWindow(
		wc.lpszClassName,
		TEXT("Ý’è"),
		WS_CAPTION|WS_SYSMENU,
		((rc->right)-(rc->left)-300)/2+(rc->left), ((rc->bottom)-(rc->top)-200)/2+(rc->top),
		arc.right-arc.left, arc.bottom-arc.top,
		ghWnd,
		NULL,
		ghInstance,
		NULL
	);
}


HWND CreateBack (int x, int y, int w, int h) {
	WNDCLASSEX wc = {0};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = BackProc;
	wc.hInstance = ghInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("Back");
	if (!RegisterClassEx(&wc)) return NULL;

	return CreateWindowEx(
		WS_EX_TOOLWINDOW|WS_EX_LAYERED,
		wc.lpszClassName,
		TEXT("”wŒi"),
		WS_POPUP|WS_VISIBLE,
		x, y,
		w, h,
		NULL,
		NULL,
		ghInstance,
		NULL
	);
}


LONG SetEditStyle (LONG src, ESES pos) {
	switch (pos) {
		case SES_LEFT:
			src = src | ES_LEFT & ~(ES_CENTER | ES_RIGHT);
			break;
		case SES_CENTER:
			src = src | ES_CENTER & ~(ES_LEFT | ES_RIGHT);
			break;
		case SES_RIGHT:
			src = src | ES_RIGHT & ~(ES_LEFT | ES_CENTER);
			break;
	}
	return src;
}


void GetValue (sConfig *pcfg, char tab_id) {
	TCHAR str[7];

	if (GetWindowTextLength(ghText)==0) return;
	GetWindowText(ghText, str, sizeof(str));
	switch (tab_id) {
		case ID_TAB_TXCOLOR:
			pcfg->txColor = _tcstol(str, NULL, 16);
			break;
		case ID_TAB_BGCOLOR:
			pcfg->bgColor = _tcstol(str, NULL, 16);
			break;
		case ID_TAB_TABS:
			pcfg->tabs = _tcstol(str, NULL, 16)*4;
			break;
		case ID_TAB_LAYER:
			pcfg->layer = _tcstol(str, NULL, 16);
			break;
		case ID_TAB_MARGIN_X:
			pcfg->margin_x = _tcstol(str, NULL, 16);
			break;
		case ID_TAB_MARGIN_Y:
			pcfg->margin_y = _tcstol(str, NULL, 16);
			break;
	}

	return;
}

void SetValue (sConfig *pcfg, char tab_id) {
	TCHAR str[7];

	switch (tab_id) {
		case ID_TAB_TXCOLOR:
			_stprintf(str, TEXT("%06X") ,pcfg->txColor);
			break;
		case ID_TAB_BGCOLOR:
			_stprintf(str, TEXT("%06X"), pcfg->bgColor);
			break;
		case ID_TAB_TABS:
			_stprintf(str, TEXT("%02X"), pcfg->tabs/4);
			break;
		case ID_TAB_LAYER:
			_stprintf(str, TEXT("%02X"), pcfg->layer);
			break;
		case ID_TAB_MARGIN_X:
			_stprintf(str, TEXT("%02X"), pcfg->margin_x);
			break;
		case ID_TAB_MARGIN_Y:
			_stprintf(str, TEXT("%02X"), pcfg->margin_y);
			break;
	}
	SetWindowText(ghText, str);

	return;
}