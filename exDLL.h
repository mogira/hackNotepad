#include "forcl.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <shlwapi.h>
#include <commctrl.h>

#define ID_CONFIG 1000
#define ID_WRITECFG 1001
#define ID_LOADCFG 1002
#define ID_FOREGROUND 1003
#define ID_ESCAPE_ADD 1004
#define ID_REDESCRIBE 1005

#define ID_LEFT 2000
#define ID_CENTER 2001
#define ID_RIGHT 2002

#define ID_BG_SS 3000
#define ID_BG_NOTUSE 3001

#define ID_TAB_TXCOLOR 0
#define ID_TAB_BGCOLOR 1
#define ID_TAB_LAYER 2
#define ID_TAB_TABS 3
#define ID_TAB_MARGIN_X 4
#define ID_TAB_MARGIN_Y 5

#define ID_BUTTON_OK 0xE5
#define ID_BUTTON_APPLY 0xE6
#define ID_BUTTON_CANCEL 0xE7
#define ID_LABEL 0xE8


#define MAX_LENGTH 9

typedef struct {
	COLORREF txColor, bgColor;
	unsigned int tabs, layer, margin_x, margin_y;
	} sConfig;
typedef enum {SES_LEFT, SES_CENTER, SES_RIGHT} ESES;

#ifdef MAIN_SOURCE
	HINSTANCE ghInstance;
	WNDPROC g_WndProc, g_EditProc, g_TextProc;
	HWND ghWnd = NULL, ghEdit, ghConfig = NULL, ghText, ghBack = NULL, ghTab;
	TCHAR path[MAX_PATH];
	sConfig cfg;
	char esc_add = 0, f = 0, sh = 0;
	HDC hmdc[2];
	HBITMAP hsBmp = NULL, hfBmp = NULL;
	RECT grc;
#else
	extern HINSTANCE ghInstance;
	extern WNDPROC g_TextProc;
	extern HWND ghWnd, ghEdit, ghConfig, ghText, ghTab;
	extern TCHAR path[MAX_PATH];
	extern sConfig cfg;
	extern char f, sh;
	extern HDC hmdc[2];
	extern HBITMAP hsBmp, hfBmp;
	extern RECT grc;
#endif


void ComposeMenu (HMENU *hMenu, HMENU *align, HMENU *hmBack);
UINT ToggleMenuItem (HMENU hMenu, UINT ID);
void LoadConfig (void);
void WriteConfig (void);
HWND CreateConfig (RECT *rc);
HWND CreateBack (int x, int y, int w, int h);
LONG SetEditStyle (LONG src, ESES pos);
void GetValue (sConfig *pcfg, char tab_id);
void SetValue (sConfig *pcfg, char tab_id);

LRESULT CALLBACK Main_Proc (HWND shWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK Edit_Proc (HWND shEdit, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK Config_Proc (HWND shConfig, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK Text_Proc (HWND shText, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK BackProc (HWND shBack, UINT msg, WPARAM wp, LPARAM lp);

void printError (const char *fname, int err);