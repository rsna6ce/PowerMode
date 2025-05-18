// This file is part of PowerMode.
// 
// Copyright (C)2016 Justin Dailey <dail8859@yahoo.com>
// 
// PowerMode is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <algorithm>
#include "PluginDefinition.h"
#include "AboutDialog.h"
#include "resource.h"

#include <vector>
#include <Commctrl.h>
#pragma comment(lib, "comctl32.lib")

static HANDLE _hModule;
static NppData nppData;
static void showAbout();

static bool lastUndoState = false;
static int lastTextLength = 0;
static bool isInitialized = false;

FuncItem funcItem[] = { { TEXT("About..."), showAbout, 0, false, nullptr } };

typedef struct Particle {
	double x, y;
	double vx, vy;
	int life;
	int size;
	COLORREF color;
} Particle;

std::vector<Particle> particles;

static void showAbout() {
	HWND hSelf = CreateDialogParam((HINSTANCE)_hModule, MAKEINTRESOURCE(IDD_ABOUTDLG), nppData._nppHandle, abtDlgProc, NULL);

	// Go to center
	RECT rc;
	::GetClientRect(nppData._nppHandle, &rc);
	POINT center;
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;
	center.x = rc.left + w / 2;
	center.y = rc.top + h / 2;
	ClientToScreen(nppData._nppHandle, &center);

	RECT dlgRect;
	GetClientRect(hSelf, &dlgRect);
	int x = center.x - (dlgRect.right - dlgRect.left) / 2;
	int y = center.y - (dlgRect.bottom - dlgRect.top) / 2;

	SetWindowPos(hSelf, HWND_TOP, x, y, (dlgRect.right - dlgRect.left), (dlgRect.bottom - dlgRect.top), SWP_SHOWWINDOW);
}

template<class T>
static T randRange(T min_number, T max_number) {
	double f = ((double)rand() / RAND_MAX);
	return (T)(min_number + f * (max_number - min_number));
}

static void updateIt() {
	particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle &p){ return p.life == 0; }), particles.end());
	for (auto &p : particles) {
		p.x = p.x + (p.vx * 1);
		p.y = p.y + (p.vy * 1);
		p.vy += 0.5;
		p.life--;
		// Since I have no idea how to do alpha...
		if (p.life == 10) p.size = 4;
		if (p.life == 5) p.size = 3;
		if (p.life == 0) p.size = 0;
	}
}

static LRESULT CALLBACK doIt(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	static bool actuallyDoIt = false;

	auto ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);

	switch (uMsg) {
	case WM_PAINT: {
		if (actuallyDoIt) updateIt();
		if (particles.size() > 0) {
			auto hdc = GetDC(hWnd);
			for (auto &p : particles) {
				auto pen = CreatePen(PS_SOLID, p.size, p.color);
				SelectObject(hdc, pen);
				MoveToEx(hdc, (int)p.x, (int)p.y, 0);
				LineTo(hdc, (int)p.x, (int)p.y);
				DeleteObject(pen);
			}
			ReleaseDC(hWnd, hdc);
		}
		actuallyDoIt = false;
		return TRUE;
	}
	case WM_TIMER:
		if (wParam == 133) {
			if (particles.size() > 0) {
				actuallyDoIt = true;
				InvalidateRect(hWnd, NULL, TRUE);
			}
			return FALSE;
		}
	}
	return ret;
}

static void powerModeOnRange(int start, int end, COLORREF c, int size = 5) {
	// Put some sort of limit on it
	if (particles.size() > 1000) return;

	int startLine = SendMessage(nppData._scintillaMainHandle, SCI_LINEFROMPOSITION,start, 0);
	int endLine = SendMessage(nppData._scintillaMainHandle, SCI_LINEFROMPOSITION, end, 0);

	// Do it based on lines since they can be different lengths
	for (int line = startLine; line <= endLine; ++line) {
		int startPos = SendMessage(nppData._scintillaMainHandle, SCI_POSITIONFROMLINE, line, 0);
		int endPos = SendMessage(nppData._scintillaMainHandle, SCI_GETLINEENDPOSITION, line, 0);
		startPos = max(start, startPos);
		endPos = min(end, endPos);
		int num = min(max(3, (endPos - startPos) / 2), 1000);

		int sx = SendMessage(nppData._scintillaMainHandle, SCI_POINTXFROMPOSITION, 0, startPos);
		int ex = SendMessage(nppData._scintillaMainHandle, SCI_POINTXFROMPOSITION, 0, endPos);
		int sy = SendMessage(nppData._scintillaMainHandle, SCI_POINTYFROMPOSITION, 0, startPos);
		int ey = SendMessage(nppData._scintillaMainHandle, SCI_POINTYFROMPOSITION, 0, endPos);

		for (int i = 0; i < num; ++i) {
			particles.emplace_back(Particle{ static_cast<double>(randRange(sx, ex)), static_cast<double>(randRange(sy, ey) + 5), randRange(-2.0, 2.0), randRange(-8.0, -2.0), randRange(30, 32), size, c });
		}
	}
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD reasonForCall, LPVOID lpReserved) {
	if (reasonForCall == DLL_PROCESS_ATTACH) _hModule = hModule;
	return TRUE;
}

extern "C" __declspec(dllexport) void setInfo(NppData notepadPlusData) { nppData = notepadPlusData; }
extern "C" __declspec(dllexport) const wchar_t *getName() { return NPP_PLUGIN_NAME; }
extern "C" __declspec(dllexport) FuncItem *getFuncsArray(int *nbF) { *nbF = 1; return funcItem; }
extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam) { return TRUE; }
extern "C" __declspec(dllexport) BOOL isUnicode() { return TRUE; }

extern "C" __declspec(dllexport) void beNotified(const SCNotification* notify) {
	static int prev_size = -1;
	static int prev_pos = 0;

	// Somehow we are getting notifications from other scintilla handles at times
	if (notify->nmhdr.hwndFrom != nppData._nppHandle &&
		notify->nmhdr.hwndFrom != nppData._scintillaMainHandle)
		return;

	switch (notify->nmhdr.code) {
	case SCN_MODIFIED: {
		// not supported event, disabled
		break;
	}
	case SCN_CHARADDED: {
		int pos = SendMessage(nppData._scintillaMainHandle, SCI_GETCURRENTPOS, 0, 0);
		powerModeOnRange(pos - 1, pos, RGB(88, 88, 88));
		break;
	}
	case SCN_KEY: {
		if (notify->ch == VK_BACK || notify->ch == VK_DELETE) {
			int pos = SendMessage(nppData._scintillaMainHandle, SCI_GETCURRENTPOS, 0, 0);
			powerModeOnRange(pos, pos + 1, RGB(88, 88, 88));
		}
		break;
	}
	case SCN_UPDATEUI: {
		if (!isInitialized) {
			lastTextLength = SendMessage(nppData._scintillaMainHandle, SCI_GETTEXTLENGTH, 0, 0);
			lastUndoState = SendMessage(nppData._scintillaMainHandle, SCI_GETUNDOCOLLECTION, 0, 0);
			break;
		}

		int currentTextLength = SendMessage(nppData._scintillaMainHandle, SCI_GETTEXTLENGTH, 0, 0);
		if (lastTextLength != currentTextLength) {
			int pos = SendMessage(nppData._scintillaMainHandle, SCI_GETCURRENTPOS, 0, 0);
			powerModeOnRange(pos - 1, pos, RGB(88, 88, 88));
		}
		lastTextLength = currentTextLength;

		// Undo/RedoÇÃåüèo
		bool currentUndoState = SendMessage(nppData._scintillaMainHandle, SCI_GETUNDOCOLLECTION, 0, 0);
		if (lastUndoState != currentUndoState) {
			int pos = SendMessage(nppData._scintillaMainHandle, SCI_GETCURRENTPOS, 0, 0);
			powerModeOnRange(pos - 1, pos, RGB(88, 88, 88));
		}
		lastUndoState = currentUndoState;

		if (prev_size != -1) {
			COLORREF c = SendMessage(nppData._scintillaMainHandle, SCI_STYLEGETFORE, SendMessage(nppData._scintillaMainHandle, SCI_GETSTYLEAT, prev_pos, 0), 0);
			if (c != RGB(0, 0, 0))
				for (unsigned int i = prev_size; i < particles.size(); ++i)
					particles[i].color = c;
			prev_size = -1;
		}
		break;
	}
	case NPPN_READY: {
		SetWindowSubclass(nppData._scintillaMainHandle, doIt, 0, 0);
		SetTimer(nppData._scintillaMainHandle, 133, 50, NULL);
		isInitialized = true;
		lastTextLength = SendMessage(nppData._scintillaMainHandle, SCI_GETTEXTLENGTH, 0, 0);
		lastUndoState = SendMessage(nppData._scintillaMainHandle, SCI_GETUNDOCOLLECTION, 0, 0);
		break;
	}
	case NPPN_SHUTDOWN: {
		KillTimer(nppData._scintillaMainHandle, 133);
		RemoveWindowSubclass(nppData._scintillaMainHandle, doIt, 0);
		isInitialized = false;
		break;
	}
	}
	return;
}
