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
#include "Control.h"
#include "Skin.h"

Forms::Skin* gClassicSkin;
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
	gClassicSkin = new Forms::Skin(*gRenderer);

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
	Forms::Control  root = Forms::Control(0, Forms::Rect(root_pad, root_pad, clientRect.right - clientRect.left - root_pad * 2, clientRect.bottom - clientRect.top - root_pad * 2));
	
	// Testing vertical clipping
	Forms::Control vertical_clipping = Forms::Control(&root, Forms::Rect(15, 15, 660, 300)); // Free floating inside root (not clipped by default)
	Forms::Control v_a = Forms::Control(&vertical_clipping, Forms::Rect(40, 20, 80, 260)); // Top and bottom are in frame
	Forms::Control v_b = Forms::Control(&vertical_clipping, Forms::Rect(140, 0, 80, 300 - 4)); // Top and bottom are at layout border
	Forms::Control v_c = Forms::Control(&vertical_clipping, Forms::Rect(240, -1, 80, 300 - 2)); // Top and bottom are clipped by 1px
	Forms::Control v_d = Forms::Control(&vertical_clipping, Forms::Rect(340, 20, 80, 350)); // Bottom is clipped only
	Forms::Control v_e = Forms::Control(&vertical_clipping, Forms::Rect(440, -20, 80, 300)); // Top is clipped only
	Forms::Control v_f = Forms::Control(&vertical_clipping, Forms::Rect(540, -20, 80, 360)); // Top and bottom are clipped

	// Testing horizontal clipping
	Forms::Control horizontal_clipping = Forms::Control(&root, Forms::Rect(15, 330, 300, 240), 0, false); // Free floating inside root (not clipped by default)
	Forms::Control h_a = Forms::Control(&horizontal_clipping, Forms::Rect(40, 5, 300 - 80, 25)); // Left and right are both in frame
	Forms::Control h_b = Forms::Control(&horizontal_clipping, Forms::Rect(0, 45, 300 - 4, 25)); // Left and right are at layout border
	Forms::Control h_c = Forms::Control(&horizontal_clipping, Forms::Rect(-1, 85, 300 - 2, 25)); // Top and bottom are clipped by 1px
	Forms::Control h_d = Forms::Control(&horizontal_clipping, Forms::Rect(40, 125, 300, 25)); // Right is clipped only
	Forms::Control h_e = Forms::Control(&horizontal_clipping, Forms::Rect(-40, 165, 300, 25)); // Left is clipped only
	Forms::Control h_f = Forms::Control(&horizontal_clipping, Forms::Rect(-40, 205, 500, 25)); // Right and left are clipped

	// Test multi clipping
	Forms::Control multi_clipping = Forms::Control(&root, Forms::Rect(340, 330, 120, 100)); // Free floating inside root (not clipped by default)
	Forms::Control m_a = Forms::Control(&multi_clipping, Forms::Rect(-2, -2, 124, 104)); // No borders should show at all
	Forms::Control m_b = Forms::Control(&multi_clipping, Forms::Rect(-1, -1, 118, 98)); // 1 pixel on the inset
	Forms::Control m_c = Forms::Control(&m_b, Forms::Rect(-10, -10, 30, 30)); // clip top left
	Forms::Control m_d = Forms::Control(&m_b, Forms::Rect(100, 80, 300, 300)); // clip bottom right
	Forms::Control m_e = Forms::Control(&m_b, Forms::Rect(100, -10, 30, 30)); // clip top right
	Forms::Control m_f = Forms::Control(&m_b, Forms::Rect(-10, 80, 50, 300)); // clip bottom left

	// Test complex clipping
	Forms::Control complex_clipping = Forms::Control(&root, Forms::Rect(340, 460, 120, 100)); // Free floating inside root (not clipped by default)
	Forms::Control c_a = Forms::Control(&complex_clipping, Forms::Rect(10 + 100, 10, 50, 50)); // Partially clipped on right
	Forms::Control c_b = Forms::Control(&complex_clipping, Forms::Rect(-50, 10, 10, 10)); // Far back enough to not be clipped (should be clipped tough)

	// Test flush inside, deep nesting
	Forms::Control no_clipping = Forms::Control(&root, Forms::Rect(490, 460, 120, 100)); // Free floating inside root (not clipped by default)
	Forms::Control n_a = Forms::Control(&no_clipping, Forms::Rect(0, 0, 120 - 4, 100 - 4)); // Should not clip
	Forms::Control n_b = Forms::Control(&n_a, Forms::Rect(0, 0, 120 - 8, 100 - 8)); // Should not clip
	Forms::Control n_c = Forms::Control(&n_b, Forms::Rect(2, 2, 120 - 12 - 4, 100 - 12 - 4)); // Should not clip
	Forms::Control n_d = Forms::Control(&n_c, Forms::Rect(0, 0, 120 - 16 - 4, 100 - 16 - 4), 0, false); // Should not clip
	Forms::Control n_e = Forms::Control(&n_d, Forms::Rect(n_d.GetRelativeLayout().AdjustPosition(30, -20).AdjustSize(-64, 20))); // Should clip top and bottom
	Forms::Control n_f = Forms::Control(&n_d, Forms::Rect(n_d.GetRelativeLayout().AdjustPosition(-20, 30).AdjustSize(20, -64))); // Should clip left and right
	Forms::Control n_g = Forms::Control(&n_d, Forms::Rect(n_d.GetRelativeLayout().AdjustPosition(30, 30).AdjustSize(-64, -64))); // Should not clip

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
