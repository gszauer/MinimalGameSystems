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

#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")

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
	//HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

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

void AnchoringTest(HWND hwnd) {
	if (gRenderer == 0) {
		return;
	}

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	Forms::ClassicSkin noBorder(*gRenderer);
	noBorder.SetBorderSize(0);

	gRenderer->Clear(Forms::Color(128, 158, 178)); // I call RGB(.5, .6, .7) as "Gabor Blue"

	int root_pad = 5; // Should root touch the edges, or respect some padding?
	Forms::Panel  root = Forms::Panel(0, Forms::Rect(root_pad, root_pad, clientRect.right - clientRect.left - root_pad * 2, clientRect.bottom - clientRect.top - root_pad * 2));
	
	Forms::Panel pa_test_2 = Forms::Panel(&root, Forms::Rect(515 - 2, 15 - 2, 250 + 4, 175 + 4)); // Test left and right anchoring
	Forms::Panel pa_test = Forms::Panel(&root, Forms::Rect(515, 15, 250, 175), &noBorder); // Test left and right anchoring

	Forms::Panel pa_a = Forms::Panel(&pa_test, Forms::Rect(100, 15, 100, 20)); // Anchored to the left
	pa_a.SetAnchoring(Forms::Anchor::Left);
	pa_a.SetAnchor(Forms::Anchor::Left, 0);

	Forms::Panel pa_b = Forms::Panel(&pa_test, Forms::Rect(100, 45, 100, 20)); // Anchored to the left
	pa_b.SetAnchoring(Forms::Anchor::Left);
	pa_b.SetAnchor(Forms::Anchor::Left, 10);

	Forms::Panel pa_c = Forms::Panel(&pa_test, Forms::Rect(100, 15, 100, 20)); // Anchored to the right
	pa_c.SetAnchoring(Forms::Anchor::Right);
	pa_c.SetAnchor(Forms::Anchor::Right, 0);

	Forms::Panel pa_d = Forms::Panel(&pa_test, Forms::Rect(100, 45, 100, 20)); // Anchored to the right
	pa_d.SetAnchoring(Forms::Anchor::Right);
	pa_d.SetAnchor(Forms::Anchor::Right, 10);

	Forms::Panel pa_e = Forms::Panel(&pa_test, Forms::Rect(100, 75, 100, 20)); // Anchored to both
	pa_e.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right);
	pa_e.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right, 0);

	Forms::Panel pa_f = Forms::Panel(&pa_test, Forms::Rect(100, 105, 100, 20)); // Anchored to both
	pa_f.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right);
	pa_f.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right, 10);

	Forms::Panel pa_g = Forms::Panel(&pa_test, Forms::Rect(100, 135, 100, 20)); // Anchored to both
	pa_g.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right);
	pa_g.SetAnchor(Forms::Anchor::Right, 5);
	pa_g.SetAnchor(Forms::Anchor::Left, 30);

	Forms::Panel lr_test = Forms::Panel(&root, Forms::Rect(15, 15, 300, 175)); // Test left and right anchoring

	Forms::Panel lr_a = Forms::Panel(&lr_test, Forms::Rect(100, 15, 100, 20)); // Anchored to the left
	lr_a.SetAnchoring(Forms::Anchor::Left);
	lr_a.SetAnchor(Forms::Anchor::Left, 0);

	Forms::Panel lr_b = Forms::Panel(&lr_test, Forms::Rect(100, 45, 100, 20)); // Anchored to the left
	lr_b.SetAnchoring(Forms::Anchor::Left);
	lr_b.SetAnchor(Forms::Anchor::Left, 10);

	Forms::Panel lr_c = Forms::Panel(&lr_test, Forms::Rect(100, 15, 100, 20)); // Anchored to the right
	lr_c.SetAnchoring(Forms::Anchor::Right);
	lr_c.SetAnchor(Forms::Anchor::Right, 0);

	Forms::Panel lr_d = Forms::Panel(&lr_test, Forms::Rect(100, 45, 100, 20)); // Anchored to the right
	lr_d.SetAnchoring(Forms::Anchor::Right);
	lr_d.SetAnchor(Forms::Anchor::Right, 10);

	Forms::Panel lr_e = Forms::Panel(&lr_test, Forms::Rect(100, 75, 100, 20)); // Anchored to both
	lr_e.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right);
	lr_e.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right, 0);

	Forms::Panel lr_f = Forms::Panel(&lr_test, Forms::Rect(100, 105, 100, 20)); // Anchored to both
	lr_f.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right);
	lr_f.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right, 10);

	Forms::Panel lr_g = Forms::Panel(&lr_test, Forms::Rect(100, 135, 100, 20)); // Anchored to both
	lr_g.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right);
	lr_g.SetAnchor(Forms::Anchor::Right, 5);
	lr_g.SetAnchor(Forms::Anchor::Left, 30);

	Forms::Panel ud_test = Forms::Panel(&root, Forms::Rect(330, 15, 170, 350)); // Test top and bottom anchoring

	Forms::Panel ud_a = Forms::Panel(&ud_test, Forms::Rect(10, 50, 20, 80)); // anchored to top
	ud_a.SetAnchoring(Forms::Anchor::Top);
	ud_a.SetAnchor(Forms::Anchor::Top, 0);

	Forms::Panel ud_b = Forms::Panel(&ud_test, Forms::Rect(40, 50, 20, 80)); // anchored to top
	ud_b.SetAnchoring(Forms::Anchor::Top);
	ud_b.SetAnchor(Forms::Anchor::Top, 10);

	Forms::Panel ud_c = Forms::Panel(&ud_test, Forms::Rect(10, 50, 20, 80)); // anchored to bottom
	ud_c.SetAnchoring(Forms::Anchor::Bottom);
	ud_c.SetAnchor(Forms::Anchor::Bottom, 0);

	Forms::Panel ud_d = Forms::Panel(&ud_test, Forms::Rect(40, 50, 20, 80)); // anchored to bottom
	ud_d.SetAnchoring(Forms::Anchor::Bottom);
	ud_d.SetAnchor(Forms::Anchor::Bottom, 10);

	Forms::Panel ud_e = Forms::Panel(&ud_test, Forms::Rect(70, 50, 20, 80)); // anchored to top and bottom
	ud_e.SetAnchoring(Forms::Anchor::Bottom | Forms::Anchor::Top);
	ud_e.SetAnchor(Forms::Anchor::Bottom | Forms::Anchor::Top, 0);

	Forms::Panel ud_f = Forms::Panel(&ud_test, Forms::Rect(100, 50, 20, 80)); // anchored to top and bottom
	ud_f.SetAnchoring(Forms::Anchor::Bottom | Forms::Anchor::Top);
	ud_f.SetAnchor(Forms::Anchor::Bottom | Forms::Anchor::Top, 10);

	Forms::Panel ud_g = Forms::Panel(&ud_test, Forms::Rect(130, 50, 20, 80)); // anchored to top and bottom
	ud_g.SetAnchoring(Forms::Anchor::Bottom | Forms::Anchor::Top);
	ud_g.SetAnchor(Forms::Anchor::Bottom, 5);
	ud_g.SetAnchor(Forms::Anchor::Top, 20);

	Forms::Panel nesting_test_a = Forms::Panel(&root, Forms::Rect(15, 200, 300, 100)); // Test left and right anchoring

	Forms::Panel na_a = Forms::Panel(&nesting_test_a, Forms::Rect(20, 20, 30, 30)); // Anchored to both
	na_a.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	na_a.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 5);

	Forms::Panel na_b = Forms::Panel(&na_a, Forms::Rect(20, 20, 30, 30)); // Anchored to both
	na_b.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	na_b.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 5);

	Forms::Panel na_c = Forms::Panel(&na_b, Forms::Rect(20, 20, 30, 30)); // Anchored to both
	na_c.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	na_c.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 0);

	Forms::Panel na_d = Forms::Panel(&na_c, Forms::Rect(20, 20, 30, 30)); // Anchored to both
	na_d.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	na_d.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 0);

	Forms::Panel na_e = Forms::Panel(&na_d, Forms::Rect(20, 20, 30, 30)); // Anchored to both
	na_e.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	na_e.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 10);
	
	Forms::Panel nesting_test_b = Forms::Panel(&root, Forms::Rect(15, 310, 300, 100)); // Test left and right anchoring
	
	Forms::Panel nb_a = Forms::Panel(&nesting_test_b, Forms::Rect(20, 20, 30, 30), &noBorder); // Anchored to both
	nb_a.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	nb_a.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 5);

	Forms::Panel nb_b = Forms::Panel(&nb_a, Forms::Rect(20, 20, 30, 30)); // Anchored to both
	nb_b.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	nb_b.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 5);

	Forms::Panel nb_c = Forms::Panel(&nb_b, Forms::Rect(20, 20, 30, 30)); // Anchored to both
	nb_c.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	nb_c.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 0);

	Forms::Panel nb_d = Forms::Panel(&nb_c, Forms::Rect(20, 20, 30, 30)); // Anchored to both
	nb_d.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	nb_d.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 0);

	Forms::Panel nb_e = Forms::Panel(&nb_d, Forms::Rect(20, 20, 30, 30)); // Anchored to both
	nb_e.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	nb_e.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 10);

	Forms::Panel nesting_test_c = Forms::Panel(&root, Forms::Rect(15, 420, 300, 100)); // Test left and right anchoring

	Forms::Panel nc_a = Forms::Panel(&nesting_test_c, Forms::Rect(20, 20, 30, 30), &noBorder); // Anchored to both
	nc_a.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	nc_a.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 5);

	Forms::Panel nc_b = Forms::Panel(&nc_a, Forms::Rect(20, 20, 30, 30), &noBorder); // Anchored to both
	nc_b.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	nc_b.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 5);

	Forms::Panel nc_c = Forms::Panel(&nc_b, Forms::Rect(20, 20, 30, 30), &noBorder); // Anchored to both
	nc_c.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	nc_c.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 0);

	Forms::Panel nc_d = Forms::Panel(&nc_c, Forms::Rect(20, 20, 30, 30), &noBorder); // Anchored to both
	nc_d.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	nc_d.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 0);

	Forms::Panel nc_e = Forms::Panel(&nc_d, Forms::Rect(20, 20, 30, 30)); // Anchored to both
	nc_e.SetAnchoring(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom);
	nc_e.SetAnchor(Forms::Anchor::Left | Forms::Anchor::Right | Forms::Anchor::Top | Forms::Anchor::Bottom, 10);

	root.Draw(*gClassicSkin);

	Forms::Rect noClip(0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	Forms::Rect a_layout = na_e.GetAbsoluteLayout(*gClassicSkin);
	Forms::Rect b_layout = nb_e.GetAbsoluteLayout(*gClassicSkin);
	Forms::Rect c_layout = nc_e.GetAbsoluteLayout(*gClassicSkin);

	gRenderer->DrawRect(a_layout.AdjustSize(-((int)a_layout.GetWidth() - 2), 400).AdjustPosition(0, 15), noClip, Forms::Color(255, 0, 0));
	gRenderer->DrawRect(a_layout.AdjustSize(-((int)a_layout.GetWidth() - 2), 400).AdjustPosition(a_layout.GetWidth() - 2, 15), noClip, Forms::Color(255, 0, 0));

	gRenderer->DrawRect(b_layout.AdjustSize(-((int)b_layout.GetWidth() - 2), 400).AdjustPosition(0, 15), noClip, Forms::Color(0, 255, 0));
	gRenderer->DrawRect(b_layout.AdjustSize(-((int)b_layout.GetWidth() - 2), 400).AdjustPosition(b_layout.GetWidth() - 2, 15), noClip, Forms::Color(0, 255, 0));

	gRenderer->DrawRect(c_layout.AdjustSize(-((int)c_layout.GetWidth() - 2), 400).AdjustPosition(0, 15), noClip, Forms::Color(0, 0, 255));
	gRenderer->DrawRect(c_layout.AdjustSize(-((int)c_layout.GetWidth() - 2), 400).AdjustPosition(c_layout.GetWidth() - 2, 15), noClip, Forms::Color(0, 0, 255));


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
			//ClippingTest(hwnd);
			AnchoringTest(hwnd);
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
