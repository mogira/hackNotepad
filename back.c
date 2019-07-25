#include "exDLL.h"

LRESULT CALLBACK BackProc (HWND shBack, UINT msg, WPARAM wp, LPARAM lp) {
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg) {
		case WM_PAINT:
			if (sh==1) {
				hdc = BeginPaint(shBack, &ps);
				BitBlt(hdc, 0, 0, grc.right, grc.bottom, hmdc[0], 0, 0, SRCCOPY);
				EndPaint(shBack, &ps);
			}
		break;
	}

	return DefWindowProc(shBack, msg, wp, lp);
}