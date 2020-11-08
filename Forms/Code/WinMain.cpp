#pragma warning(push)
#pragma warning(disable : 28251)
#pragma warning(disable : 28159)
#pragma warning(disable : 26451)

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#undef APIENTRY
#include <windows.h>
#include <iostream>

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#if _DEBUG
#include <crtdbg.h>
#pragma comment(linker, "/subsystem:console")
int main(int argc, const char** argv) {
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
	WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWDEFAULT);
	return 0;
}
#else 
#ifdef _WIN64
#pragma comment(linker, "/subsystem:windows,5.02")
#else
#pragma comment(linker, "/subsystem:windows,5.01")
#endif
#endif
#pragma comment(lib, "opengl32.lib")

#include "Renderer.h"
#include "Controls/Panel.h"
#include "Skins/ClassicSkin.h"

Forms::ClassicSkin* gClassicSkin;
Forms::Renderer* gRenderer;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = L"Win32 Game Window";
	RegisterClassEx(&wndclass);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int clientWidth = 800;
	int clientHeight = 600;
	RECT windowRect;
	SetRect(&windowRect, (screenWidth / 2) - (clientWidth / 2), (screenHeight / 2) - (clientHeight / 2), (screenWidth / 2) + (clientWidth / 2), (screenHeight / 2) + (clientHeight / 2));

	DWORD style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
	AdjustWindowRectEx(&windowRect, style, FALSE, 0);
	HWND hwnd = CreateWindowEx(0, wndclass.lpszClassName, L"Game Window", style, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, szCmdLine);
	HDC hdc = GetDC(hwnd);

	///////////////////////////////////////////////////////////////////////////////////////
	// Initialize Here
	///////////////////////////////////////////////////////////////////////////////////////
	gRenderer = new Forms::Renderer(hwnd);
	gClassicSkin = new Forms::ClassicSkin(*gRenderer);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
		

	return (int)msg.wParam;
}

#define LEFT_MOUSE_POS (short)((float)LOWORD(lParam))
#define RIGHT_MOUSE_POS (short)((float)HIWORD(lParam))

void ClippingTest(HWND hwnd) {
	if (gRenderer == 0) {
		return;
	}

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	gRenderer->Clear(Forms::Color(128, 158, 178)); // I call RGB(.5, .6, .7) as "Gabor Blue"

	int root_pad = 5; // Should root touch the edges, or respect some padding?
	Forms::Panel  root = Forms::Panel(0, Forms::Rect(root_pad, root_pad, clientRect.right - clientRect.left - root_pad * 2, clientRect.bottom - clientRect.top - root_pad * 2));
	Forms::Panel dummy;

	// Testing vertical clipping
	Forms::Panel vertical_clipping = Forms::Panel(&root, Forms::Rect(15, 15, 660, 300)); // Free floating inside root (not clipped by default)
	Forms::Panel v_a = Forms::Panel(&vertical_clipping, Forms::Rect(40, 20, 80, 260)); // Top and bottom are in frame
	Forms::Panel v_b = Forms::Panel(&vertical_clipping, Forms::Rect(140, 0, 80, 300 - 4)); // Top and bottom are at layout border
	Forms::Panel v_c = Forms::Panel(&vertical_clipping, Forms::Rect(240, -1, 80, 300 - 2)); // Top and bottom are clipped by 1px
	Forms::Panel v_d = Forms::Panel(&vertical_clipping, Forms::Rect(340, 20, 80, 350)); // Bottom is clipped only
	Forms::Panel v_e = Forms::Panel(&vertical_clipping, Forms::Rect(440, -20, 80, 300)); // Top is clipped only
	Forms::Panel v_f = Forms::Panel(&vertical_clipping, Forms::Rect(540, -20, 80, 360)); // Top and bottom are clipped

	// Testing horizontal clipping
	Forms::Panel horizontal_clipping = Forms::Panel(&root, Forms::Rect(15, 330, 300, 240), 0); // Free floating inside root (not clipped by default)
	Forms::Panel h_a = Forms::Panel(&horizontal_clipping, Forms::Rect(40, 5, 300 - 80, 25)); // Left and right are both in frame
	Forms::Panel h_b = Forms::Panel(&horizontal_clipping, Forms::Rect(0, 45, 300 - 4, 25)); // Left and right are at layout border
	Forms::Panel h_c = Forms::Panel(&horizontal_clipping, Forms::Rect(-1, 85, 300 - 2, 25)); // Top and bottom are clipped by 1px
	Forms::Panel h_d = Forms::Panel(&horizontal_clipping, Forms::Rect(40, 125, 300, 25)); // Right is clipped only
	Forms::Panel h_e = Forms::Panel(&horizontal_clipping, Forms::Rect(-40, 165, 300, 25)); // Left is clipped only
	Forms::Panel h_f = Forms::Panel(&horizontal_clipping, Forms::Rect(-40, 205, 500, 25)); // Right and left are clipped

	// Test multi clipping
	Forms::Panel multi_clipping = Forms::Panel(&root, Forms::Rect(340, 330, 120, 100)); // Free floating inside root (not clipped by default)
	Forms::Panel m_a = Forms::Panel(&multi_clipping, Forms::Rect(-2, -2, 124, 104)); // No borders should show at all
	Forms::Panel m_b = Forms::Panel(&multi_clipping, Forms::Rect(-1, -1, 118, 98)); // 1 pixel on the inset
	Forms::Panel m_c = Forms::Panel(&m_b, Forms::Rect(-10, -10, 30, 30)); // clip top left
	Forms::Panel m_d = Forms::Panel(&m_b, Forms::Rect(100, 80, 300, 300)); // clip bottom right
	Forms::Panel m_e = Forms::Panel(&m_b, Forms::Rect(100, -10, 30, 30)); // clip top right
	Forms::Panel m_f = Forms::Panel(&m_b, Forms::Rect(-10, 80, 50, 300)); // clip bottom left

	// Test complex clipping
	Forms::Panel complex_clipping = Forms::Panel(&root, Forms::Rect(340, 460, 120, 100)); // Free floating inside root (not clipped by default)
	Forms::Panel c_a = Forms::Panel(&complex_clipping, Forms::Rect(10 + 100, 10, 50, 50)); // Partially clipped on right
	Forms::Panel c_b = Forms::Panel(&complex_clipping, Forms::Rect(-50, 10, 10, 10)); // Far back enough to not be clipped (should be clipped tough)

	// Test flush inside, deep nesting
	Forms::Panel no_clipping = Forms::Panel(&root, Forms::Rect(490, 460, 120, 100)); // Free floating inside root (not clipped by default)
	Forms::Panel n_a = Forms::Panel(&no_clipping, Forms::Rect(0, 0, 120 - 4, 100 - 4)); // Should not clip
	Forms::Panel n_b = Forms::Panel(&n_a, Forms::Rect(0, 0, 120 - 8, 100 - 8)); // Should not clip
	Forms::Panel n_c = Forms::Panel(&n_b, Forms::Rect(2, 2, 120 - 12 - 4, 100 - 12 - 4)); // Should not clip
	Forms::Panel n_d = Forms::Panel(&n_c, Forms::Rect(0, 0, 120 - 16 - 4, 100 - 16 - 4), 0); // Should not clip
	Forms::Panel n_e = Forms::Panel(&n_d, Forms::Rect(n_d.GetRelativeLayout().AdjustPosition(30, -20).AdjustSize(-64, 20))); // Should clip top and bottom
	Forms::Panel n_f = Forms::Panel(&n_d, Forms::Rect(n_d.GetRelativeLayout().AdjustPosition(-20, 30).AdjustSize(20, -64))); // Should clip left and right
	Forms::Panel n_g = Forms::Panel(&n_d, Forms::Rect(n_d.GetRelativeLayout().AdjustPosition(30, 30).AdjustSize(-64, -64))); // Should not clip

	root.Draw(*gClassicSkin);

	gRenderer->Present();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	switch (iMsg) {
	case WM_CLOSE:
		if (gRenderer != 0) {
			delete gRenderer;
			gRenderer = 0;
		}
		if (gClassicSkin != 0) {
			delete gClassicSkin;
			gClassicSkin = 0;
		}
		DestroyWindow(hwnd);
		return 1;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 1;
	case WM_LBUTTONDOWN:
		return 1;
	case WM_LBUTTONUP:
		return 1;
	case WM_RBUTTONDOWN:
		return 1;
	case WM_RBUTTONUP:
		return 1;
	case WM_MBUTTONDOWN:
		return 1;
	case WM_MBUTTONUP:
		return 1;
	case WM_MOUSEMOVE:
		return 1;
	case WM_MOUSEWHEEL:
		return 1;
	case WM_CHAR:
		if (wParam == '1') {
			//currentTest = 0;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		if (wParam == '2') {
			//currentTest = 1;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		else if (wParam == '3') {
			//currentTest = 2;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		else if (wParam == '4') {
			//currentTest = 3;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 1;
	case WM_PAINT:
		if (gRenderer != 0) {
			ClippingTest(hwnd);
#if 0
			if (currentTest == 0) {
				GraphicsTest0(hwnd);
			}
			else if (currentTest == 1) {
				GraphicsTest1(hwnd, true);
			}
			else if (currentTest == 2) {
				GraphicsTest1(hwnd, false);
			}
			else if (currentTest == 3) {
				GraphicsTest2(hwnd);
			}
#endif
		}
		return 1;
	case WM_ERASEBKGND:
		return 1;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

#pragma warning(pop)
