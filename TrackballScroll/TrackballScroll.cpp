///////////////////////////////////////////////////////////////////////
// @brief Low level mouse hook to allow scrolling with a trackball without a scroll wheel.
// Converts XBUTTONDOWN + MOUSEMOVE into MOUSEEVENTF_WHEEL.
// Converts XBUTTONDOWN + XBUTTONUP into MOUSEEVENTF_MIDDLEDOWN + MOUSEEVENTF_MIDDLEUP.
// @author Martin Seelge
// @date 2014-07-12
///////////////////////////////////////////////////////////////////////
//
// Edits by D'Artagnan Palmer
// Updated 2016-04-07
//
///////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <iostream>

const int Y_THRESHOLD = 10;   // threshold in pixels to trigger wheel event
const DWORD WHEEL_FACTOR = 1; // number of wheel events. The lines scrolled per wheel event are determined by the Microsoft Windows mouse wheel settings.

HHOOK g_Hook;           // callback function hook, called for every mouse event
POINT g_origin;         // cursor position when entering state x1DOWN
int g_ycount = 0;       // accumulated vertical movement while in state SCROLL

// If this program is expanded to include configurations, the mouse
//		states should probably kept with bitflags and bitmasks
BOOL bXB1 = FALSE;		// Is XButton1 down
BOOL bXB2 = FALSE;		// Is XButton2 down
BOOL bScrolling = FALSE;	// Are we in scrolling mode


LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode != HC_ACTION)
		return CallNextHookEx(g_Hook, nCode, wParam, lParam);
	INPUT inputDat[2];
	BOOL preventCallNextHookEx = FALSE;
	// Data about the mouse event
	const MSLLHOOKSTRUCT *p = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);

	// An extended mouse button was pressed
	if (wParam == WM_XBUTTONDOWN) {
		// XButton1
		//WORD w = HIWORD(p->mouseData);
		if (HIWORD(p->mouseData) & XBUTTON1) {
#if defined(_DEBUG)
		std::cout << "XB1 down\n";
#endif
			preventCallNextHookEx = TRUE;
			// Save cursor pos at time of "scroll-click"
			g_origin = p->pt;
			bXB1 = TRUE;	// Remember that XButton1 is down
		}
		// XButton2
		else if (HIWORD(p->mouseData) & XBUTTON2) {
#if defined(_DEBUG)
			std::cout << "XB2 down\n";
#endif
			// Send a MiddleButton Up message
			inputDat[0].type = INPUT_MOUSE;
			inputDat[0].mi.dx = p->pt.x;
			inputDat[0].mi.dy = p->pt.y;
			inputDat[0].mi.mouseData = (DWORD)0x0;
			inputDat[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN; // middle button down
			inputDat[0].mi.time = (DWORD)0x0;
			inputDat[0].mi.dwExtraInfo = (ULONG_PTR)NULL;
			SendInput(1, inputDat, sizeof(INPUT));

			bXB2 = TRUE;	// Remember that XButton2 is down
			preventCallNextHookEx = TRUE;
		}
	}
	// An extended mouse button was released
	else if (wParam == WM_XBUTTONUP) {
		// XButton1
		if (HIWORD(p->mouseData) & XBUTTON1) {
#if defined(_DEBUG)
			std::cout << "XB1 up\n";
#endif
			preventCallNextHookEx = TRUE;
			bScrolling = FALSE;
			bXB1 = FALSE;	// Forget it. XButton1 is no longer down
		}
		// XButton2
		else if (HIWORD(p->mouseData) & XBUTTON2) {
#if defined(_DEBUG)
			std::cout << "XB2 up\n";
#endif
			// Send a MiddleButton Up message
			inputDat[0].type = INPUT_MOUSE;
			inputDat[0].mi.dx = p->pt.x;
			inputDat[0].mi.dy = p->pt.y;
			inputDat[0].mi.mouseData = (DWORD)0x0;
			inputDat[0].mi.dwFlags = MOUSEEVENTF_MIDDLEUP; // middle button up
			inputDat[0].mi.time = (DWORD)0x0;
			inputDat[0].mi.dwExtraInfo = (ULONG_PTR)NULL;
			SendInput(1, inputDat, sizeof(INPUT));

			bXB2 = FALSE;	// Forget it. XButton1 is no longer down
			preventCallNextHookEx = TRUE;
		}
	}
	else if (wParam == WM_MOUSEMOVE) {
		// Is XButton1 being held
		if (bXB1) {
			// If we haven't started scrolling...
			if (!bScrolling) {
#if defined(_DEBUG)
				std::cout << "First scroll\n";
#endif
				preventCallNextHookEx = TRUE;
				// Remember that we are now scrolling
				bScrolling = TRUE;
				g_ycount = 0;	// Reset our record of verticle movement
				SetCursorPos(g_origin.x, g_origin.y);
			}
			// If we are in the middle of scrolling...
			else {
#if defined(_DEBUG)
				std::cout << " Continuing scroll\n";
#endif
				preventCallNextHookEx = TRUE;
				// Keep track of how much we moved vertically
				g_ycount += p->pt.y - g_origin.y;
				// Reset the cursor to its original position
				SetCursorPos(g_origin.x, g_origin.y);
				// If we have moved more than the threshold...
				if (g_ycount < -Y_THRESHOLD || g_ycount > Y_THRESHOLD){
					DWORD mouseData = (g_ycount > 0 ? -WHEEL_DELTA : +WHEEL_DELTA); // scroll direction
					g_ycount = 0;

					// Send <WHEEL_FACTOR> number of scroll events
					INPUT input[WHEEL_FACTOR];
					for (size_t i = 0; i < WHEEL_FACTOR; ++i) {
						input[i].type = INPUT_MOUSE;
						// INFO: Should these be set to g_origin pos instead?
						input[i].mi.dx = p->pt.x;
						input[i].mi.dy = p->pt.y;
						input[i].mi.mouseData = mouseData;
						input[i].mi.dwFlags = MOUSEEVENTF_WHEEL; // wheel
						input[i].mi.time = (DWORD)0x0;
						input[i].mi.dwExtraInfo = (ULONG_PTR)NULL;
					}
#if defined(_DEBUG)
					std::cout << "Sending Scrollwheel event\n";
#endif
					SendInput(WHEEL_FACTOR, input, sizeof(INPUT));
				}
			}
		}
	}

	// Decide if we should let other programs detect the mouse events
	return (preventCallNextHookEx ? 1 : CallNextHookEx(g_Hook, nCode, wParam, lParam));
}

int main() {
	std::cout << "TrackballScroll v2016.04.07\r\nOriginal Copyright (c) 2014 Martin Seelge, License: The MIT License (MIT)\r\nOriginal Project URL: https://github.com/Seelge/TrackballScroll/\r\n\r\n";
	std::cout << "Edits by D'Artagnan Palmer on 2016-04-07.\r\nModified Project URL: https://github.com/othermusketeer/TrackballScroll/\r\n\r\n";
	std::cout << "See README.md for information on how to use this program.\r\n\r\n";

	g_Hook = SetWindowsHookEx(WH_MOUSE_LL, &LowLevelMouseProc, GetModuleHandle(0), 0);
	if (!g_Hook) return 1; // hook failed
	std::cout << "Mouse hook registered, ready to scroll.\r\nClose this window to terminate scroll emulation.\r\n";
	MSG message;
	while (GetMessage(&message, NULL, 0, 0) > 0) // message pump, currently no exit event exists
		DispatchMessage(&message);
	UnhookWindowsHookEx(g_Hook); // remove hook
	return 0;
}
