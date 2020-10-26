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
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);

			gRenderer->Clear(Forms::Color(125, 175, 225));

			// Docking tests
			Forms::Control  root = Forms::Control(0,Forms::Box(Forms::Rect(50, 50  , 400, 300), Forms::Offset(10, 10, 10, 10), Forms::Offset(2, 2, 2, 2), Forms::Offset(5, 5, 5, 5)));
			Forms::Control  a = Forms::Control(0,	Forms::Box(Forms::Rect(150, 150, 100, 50), Forms::Offset(3, 3, 3, 3), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			Forms::Control  b = Forms::Control(0,	Forms::Box(Forms::Rect(150, 150, 100, 50), Forms::Offset(3, 3, 3, 3), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			Forms::Control  c = Forms::Control(0,	Forms::Box(Forms::Rect(150, 150, 100, 50), Forms::Offset(3, 3, 3, 3), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			Forms::Control  d = Forms::Control(0,	Forms::Box(Forms::Rect(150, 150, 50, 50),	Forms::Offset(3, 3, 3, 3), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			Forms::Control  e = Forms::Control(0,	Forms::Box(Forms::Rect(150, 150, 50, 50),	Forms::Offset(3, 3, 3, 3), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			Forms::Control  f = Forms::Control(0,	Forms::Box(Forms::Rect(150, 150, 100, 50), Forms::Offset(3, 3, 3, 3), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			Forms::Control  g = Forms::Control(0,	Forms::Box(Forms::Rect(150, 150, 100, 50), Forms::Offset(3, 3, 3, 3), Forms::Offset(1, 1, 1, 1), Forms::Offset(3, 3, 3, 3)));
			Forms::Control  j = Forms::Control(0,	Forms::Box(Forms::Rect(50, 50  , 200, 150),	Forms::Offset(10, 10, 10, 10), Forms::Offset(0,0,0,0), Forms::Offset(0,0,0,0)));
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

			//root.SetDocking(Forms::Control::Docking::Fill);
			root.SetParent(&j);
			j.SetOverflow(Forms::Control::Overflow::Hidden);

			int width = (clientRect.right - clientRect.left);
			int height = (clientRect.bottom - clientRect.top);
			int x = width / 2 - width / 2 / 2;
			int y = height / 2 - height / 2 / 2;
			
			int root_x = width / 2 - root.GetPreferedLayoutSize().width / 2;
			int root_y = height / 2 - root.GetPreferedLayoutSize().height / 2;
			root_x -= x;
			root_y -= y;
			Forms::Box rootLayout = root.GetRelativeLayout();
			rootLayout.content.x = root_x;
			rootLayout.content.y = root_y;
			root.SetRelativeLayout(rootLayout);

			Forms::Rect formRect(x, y, width / 2, height / 2);
			j.UpdateLayout(formRect);
			j.Clip(formRect);



			gRenderer->Draw(j);

			gRenderer->Present();
		}
		return 1;
	case WM_ERASEBKGND:
		return 1;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

#pragma warning(pop)
