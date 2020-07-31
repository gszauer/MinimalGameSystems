//#pragma warning(disable : 28251)
//#pragma warning(disable : 28159)
//#pragma warning(disable : 26451)

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include "glad.h"
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

#include "AnimationState.h"
#include "AnimationData.h"
#include "AnimationHelpers.h"
#include "AnimationSkin.h"
#include "AnimationBlend.h"

#include "Samples/ISample.h"
#include "Samples/SkinnedSample.h"
#include "Samples/SkeletonSample.h"
#include "Samples/CurvesSample.h"
#include "Samples/microui.h"

#include <vector>
#include <fstream>
#include <sstream>

using std::vector;

///////////////////////////////////////////////////////////////////////////////////////
// Everything below this line is just WinMain + OpenGL loader. Nothing too interesting.
///////////////////////////////////////////////////////////////////////////////////////

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

ISample* gCurveSample = 0; // TODO: Move these into an array
ISample* gSkeletonSample = 0;
ISample* gSkinSample = 0;
ISample* gBlendSample = 0;

mu_Context* gUIContext = 0;
int gShowCurveSample = 0;
int gShowSkeletonSample = 0;
int gShowSkinnedSample = 1;
int gShowBlendingSample = 0;
bool gRendererRunning = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
	gCurveSample = new CurvesSample();
	gSkeletonSample = new SkeletonSample();
	gSkinSample = new SkinnedSample();
	gBlendSample = 0;
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

	DWORD style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX); // WS_THICKFRAME to resize
	AdjustWindowRectEx(&windowRect, style, FALSE, 0);
	HWND hwnd = CreateWindowEx(0, wndclass.lpszClassName, L"Game Window", style, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, szCmdLine);
	HDC hdc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);

	HGLRC tempRC = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempRC);
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	const int attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0,
	};

	HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, attribList);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempRC);
	wglMakeCurrent(hdc, hglrc);

	if (!gladLoadGL()) {
		std::cout << "Could not initialize GLAD\n";
	}
	else {
		std::cout << "OpenGL Version " << GLVersion.major << "." << GLVersion.minor << " loaded\n";
	}

	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
	bool swapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;

	int vsynch = 0;
	if (swapControlSupported) {
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

		if (wglSwapIntervalEXT(1)) {
			std::cout << "Enabled vsynch\n";
			vsynch = wglGetSwapIntervalEXT();
		}
		else {
			std::cout << "Could not enable vsynch\n";
		}
	}
	else { // !swapControlSupported
		std::cout << "WGL_EXT_swap_control not supported\n";
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	r_init(800, 600);
	gRendererRunning = true;
	gUIContext = (mu_Context*)malloc(sizeof(mu_Context));
	mu_init(gUIContext);

	if (gCurveSample != 0) {
		gCurveSample->Initialize();
	}
	if (gSkeletonSample != 0) {
		gSkeletonSample->Initialize();
	}
	if (gSkinSample != 0) {
		gSkinSample->Initialize();
	}
	if (gBlendSample != 0) {
		gBlendSample->Initialize();
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glPointSize(5.0f);

	DWORD lastTick = GetTickCount();
	MSG msg;
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		DWORD thisTick = GetTickCount();
		float deltaTime = float(thisTick - lastTick) * 0.001f;
		lastTick = thisTick;
		if (gCurveSample != 0 && gShowCurveSample) {
			gCurveSample->Update(deltaTime);
		}
		if (gSkeletonSample != 0 && gShowSkeletonSample) {
			gSkeletonSample->Update(deltaTime);
		}
		if (gSkinSample != 0 && gShowSkinnedSample) {
			gSkinSample->Update(deltaTime);
		}
		if (gBlendSample != 0 && gShowBlendingSample) {
			gBlendSample->Update(deltaTime);
		}
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		clientWidth = clientRect.right - clientRect.left;
		clientHeight = clientRect.bottom - clientRect.top;
		glViewport(0, 0, clientWidth, clientHeight);
		glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		float aspect = (float)clientWidth / (float)clientHeight;
		if (gCurveSample != 0 && gShowCurveSample) {
			gCurveSample->Render(aspect);
		}
		if (gSkinSample != 0 && gShowSkinnedSample) {
			gSkinSample->Render(aspect);
		}
		if (gSkeletonSample != 0 && gShowSkeletonSample) {
			gSkeletonSample->Render(aspect);
		}
		if (gBlendSample != 0 && gShowBlendingSample) {
			gBlendSample->Render(aspect);
		}

		{
			// TODO: handle mu input
			mu_begin(gUIContext, clientWidth, clientHeight);
			
			if (mu_begin_window_ex(gUIContext, "Animation Demo", mu_rect(clientWidth - 10 - 125, 10, 125, 130), MU_OPT_NOCLOSE)) {
				mu_Container* win = mu_get_current_container(gUIContext);

				{
					static int layout[] = { -1 };
					mu_layout_row(gUIContext, 1, layout, 0);
					mu_checkbox(gUIContext, "Curve Sample", &gShowCurveSample);
					mu_checkbox(gUIContext, "Skeleton Sample", &gShowSkeletonSample);
					mu_checkbox(gUIContext, "Skinning Sample", &gShowSkinnedSample);
					mu_checkbox(gUIContext, "Blending Sample", &gShowBlendingSample);
				}

				mu_end_window(gUIContext);
			}

			mu_end(gUIContext);

			mu_Command* cmd = NULL;
			while (mu_next_command(gUIContext, &cmd)) {
				switch (cmd->type) {
					case MU_COMMAND_TEXT: r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color); break;
					case MU_COMMAND_RECT: r_draw_rect(cmd->rect.rect, cmd->rect.color); break;
					case MU_COMMAND_ICON: r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
					case MU_COMMAND_CLIP: r_set_clip_rect(cmd->clip.rect); break;
				}
			}
			r_present();
		}

		SwapBuffers(hdc);
		if (vsynch != 0) {
			glFinish();
		}
	} // End of game loop

	if (gCurveSample != 0) {
		std::cout << "Expected gCurveSample to be null on exit\n";
		delete gCurveSample;
		gCurveSample = 0;
	}
	if (gSkeletonSample != 0) {
		std::cout << "Expected gSkeletonSample to be null on exit\n";
		delete gSkeletonSample;
		gSkeletonSample = 0;
	}
	if (gSkinSample != 0) {
		std::cout << "Expected gSkinSample to be null on exit\n";
		delete gSkinSample;
		gSkinSample = 0;
	}
	if (gBlendSample != 0) {
		std::cout << "Expected gBlendSample to be null on exit\n";
		delete gBlendSample;
		gBlendSample = 0;
	}

	return (int)msg.wParam;
}

#define LEFT_MOUSE_POS (short)((float)LOWORD(lParam))
#define RIGHT_MOUSE_POS (short)((float)HIWORD(lParam))

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	switch (iMsg) {
	case WM_CLOSE:
		if (gCurveSample != 0) {
			gCurveSample->Shutdown();
			gCurveSample = 0;

		}
		if (gSkeletonSample != 0) {
			gSkeletonSample->Shutdown();
			delete gSkeletonSample;
			gSkeletonSample = 0;
		}
		if (gSkinSample != 0) {
			gSkinSample->Shutdown();
			delete gSkinSample;
			gSkinSample = 0;
		}
		if (gBlendSample != 0) {
			gBlendSample->Shutdown();
			delete gBlendSample;
			gBlendSample = 0;
		}
		if (gRendererRunning) {
			r_shutdown();

			HDC hdc = GetDC(hwnd);
			HGLRC hglrc = wglGetCurrentContext();

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc); // TODO: If i do a decent amount of show hides and exit i can trigger a breakpoint here. Why?
			ReleaseDC(hwnd, hdc);
			DestroyWindow(hwnd);
			gRendererRunning = false;
		}
		return 1;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 1;
	case WM_LBUTTONDOWN:
		mu_input_mousedown(gUIContext, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 1);
		return 1;
	case WM_LBUTTONUP:
		mu_input_mouseup(gUIContext, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 1);
		return 1;
	case WM_RBUTTONDOWN:
		mu_input_mousedown(gUIContext, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 0);
		return 1;
	case WM_RBUTTONUP:
		mu_input_mouseup(gUIContext, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 0);
		return 1;
	case WM_MBUTTONDOWN:
		mu_input_mousedown(gUIContext, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 2);
		return 1;
	case WM_MBUTTONUP:
		mu_input_mouseup(gUIContext, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 2);
		return 1;
	case WM_MOUSEMOVE:
		mu_input_mousemove(gUIContext, LEFT_MOUSE_POS, RIGHT_MOUSE_POS);
		return 1;
	case WM_MOUSEWHEEL:
		mu_input_scroll(gUIContext,0, (short)HIWORD(wParam) / WHEEL_DELTA);
		return 1;
	case WM_PAINT:
	case WM_ERASEBKGND:
		return 0;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}