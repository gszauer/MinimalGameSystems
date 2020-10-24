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

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	switch (iMsg) {
	case WM_CLOSE:
		if (gRenderer != 0) {
			delete gRenderer;
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
	case WM_PAINT:
		if (gRenderer != 0) {
			gRenderer->Clear(Forms::Color(125, 175, 225));

			gRenderer->Draw(Forms::Rect(10, 10, 150, 50), Forms::Color(255, 0, 0));
			gRenderer->Draw(Forms::Rect(110, 56, 50, 199), Forms::Color(0, 255, 0));
			gRenderer->Draw(Forms::Rect(3, 123, 300, 25), Forms::Color(0, 0, 255));
			gRenderer->Draw(Forms::Rect(310, 78, 50, 149), Forms::Color(255, 0, 255));

			gRenderer->Clear(Forms::Color(125, 175, 225));

			Forms::Box b1(Forms::Rect(400 - 150, 300 - 75, 300, 150));
			Forms::Box b2(Forms::Rect(800-100 - 20, 600 - 100 - 20, 100, 100));

			b1.mMargin = Forms::Offset(13, 13, 23, 23);
			b1.mBorder = Forms::Offset(1, 1, 1, 5);
			b1.mPadding = Forms::Offset(40, 40, 10, 10);

			b2.mMargin = Forms::Offset(15, 15, 15, 15);
			b2.mBorder = Forms::Offset(3, 3, 3, 3);
			b2.mPadding = Forms::Offset(10, 10, 10, 10);

			gRenderer->Draw(b1);
			gRenderer->Draw(b2);

			gRenderer->Clear(Forms::Color(125, 175, 225));

			// Clipping test
			Forms::Control root(0,	Forms::Box(Forms::Rect(50, 50, 500, 200),		Forms::Offset(5, 5, 5, 5),		Forms::Offset(1, 1, 1, 1),	Forms::Offset(5, 5, 5, 5)));
			Forms::Control a(&root, Forms::Box(Forms::Rect(100, 10, 300, 300),	Forms::Offset(0, 0, 0, 5),		Forms::Offset(1, 1, 1, 1),	Forms::Offset(3, 3, 3, 3)));
			Forms::Control b(&a,	Forms::Box(Forms::Rect(-150, 150, 500, 50),	Forms::Offset(5, 5, 5, 5),		Forms::Offset(1, 1, 1, 1),	Forms::Offset(4, 3, 2, 1)));
			Forms::Control c(&root, Forms::Box(Forms::Rect(300, 10, 400, 10),	Forms::Offset(10, 10, 10, 10),	Forms::Offset(1, 1, 1, 1),	Forms::Offset(3, 3, 3, 3)));
			Forms::Control d(&c,	Forms::Box(Forms::Rect(-150, 10, 50, 50),	Forms::Offset(5, 5, 5, 5),		Forms::Offset(1, 1, 1, 1),	Forms::Offset(10, 10, 10, 10)));
			Forms::Control e(&c, Forms::Box(Forms::Rect(0, 50, 5, 5), Forms::Offset(10, 10, 10, 10), Forms::Offset(1, 1, 1, 1), Forms::Offset(1, 2, 3, 4)));
			Forms::Control f(&root, Forms::Box(Forms::Rect(0, 30, 5, 5), Forms::Offset(5, 5, 5, 5), Forms::Offset(5, 5, 5, 5), Forms::Offset(5, 5, 5, 5)));
			Forms::Control g(&root, Forms::Box(Forms::Rect(5, 50, 5, 5), Forms::Offset(5, 5, 5, 5), Forms::Offset(5, 5, 5, 5), Forms::Offset(5, 5, 5, 5)));
			Forms::Control h(&root, Forms::Box(Forms::Rect(10, 70, 5, 5), Forms::Offset(5, 5, 5, 5), Forms::Offset(5, 5, 5, 5), Forms::Offset(5, 5, 5, 5)));
			Forms::Control i(&root, Forms::Box(Forms::Rect(15, 90, 5, 5), Forms::Offset(5, 5, 5, 5), Forms::Offset(5, 5, 5, 5), Forms::Offset(5, 5, 5, 5)));
			Forms::Control j(&a, Forms::Box(Forms::Rect(150, -150, 50, 500), Forms::Offset(5, 5, 5, 5), Forms::Offset(1, 1, 1, 1), Forms::Offset(4, 3, 2, 1)));

			root.SetOverflow(Forms::Control::Overflow::Hidden);
			a.SetOverflow(Forms::Control::Overflow::Hidden);

			gRenderer->Draw(root);

			gRenderer->Clear(Forms::Color(125, 175, 225));

			// Docking tests
			root = Forms::Control(0, Forms::Box(Forms::Rect(50, 50, 400, 300), Forms::Offset(10, 10, 10, 10), Forms::Offset(2, 2, 2, 2), Forms::Offset(5, 5, 5, 5)));
			a = Forms::Control(0, Forms::Box(Forms::Rect(150, 150, 100, 50), Forms::Offset(0, 0, 0, 0), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			b = Forms::Control(0, Forms::Box(Forms::Rect(150, 150, 100, 50), Forms::Offset(0, 0, 0, 0), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			c = Forms::Control(0, Forms::Box(Forms::Rect(150, 150, 100, 50), Forms::Offset(0, 0, 0, 0), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			d = Forms::Control(0, Forms::Box(Forms::Rect(150, 150, 50, 50), Forms::Offset(0, 0, 0, 0), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			e = Forms::Control(0, Forms::Box(Forms::Rect(150, 150, 50, 50), Forms::Offset(0, 0, 0, 0), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			f = Forms::Control(0, Forms::Box(Forms::Rect(150, 150, 100, 50), Forms::Offset(0, 0, 0, 0), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			g = Forms::Control(0, Forms::Box(Forms::Rect(150, 150, 100, 50), Forms::Offset(0, 0, 0, 0), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			a.SetParent(&root);
			a.SetDocking(Forms::Control::Docking::Left);
			b.SetParent(&root);
			b.SetDocking(Forms::Control::Docking::Bottom);
			c.SetParent(&root);
			f.SetDocking(Forms::Control::Docking::Top);
			f.SetParent(&root);
			c.SetDocking(Forms::Control::Docking::Top);
			d.SetParent(&root);
			d.SetDocking(Forms::Control::Docking::Right);
			g.SetParent(&root);
			g.SetDocking(Forms::Control::Docking::Bottom);
			e.SetParent(&root);
			e.SetDocking(Forms::Control::Docking::Fill);
			b.SetMaxLayoutSize(Forms::Size(22, 22));
			c.SetMaxLayoutSize(Forms::Size(22, 22));
			g.SetMaxLayoutSize(Forms::Size(22, 22));

			gRenderer->Draw(root);

			gRenderer->Present();
		}
		return 1;
	case WM_ERASEBKGND:
		return 1;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

#pragma warning(pop)
