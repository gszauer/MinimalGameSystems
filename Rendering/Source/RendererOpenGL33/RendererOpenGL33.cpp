#include <RendererOpenGL33.h>
#include <iostream>

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

Renderer::OGL33GraphicsDevice* Renderer::CreateOpenGL33GraphicsDevice(HWND hwnd) {
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
	0, };

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

	OGL33GraphicsDevice* device = new OGL33GraphicsDevice(hwnd, hdc, vsynch);

	return device;
}

void Renderer::DestroyOpenGL33GraphicsDevice(Renderer::OGL33GraphicsDevice* device) {
	HWND hwnd = device->GetHwnd();
	HDC hdc = device->GetHdc();
	delete device;

	HGLRC hglrc = wglGetCurrentContext();
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
	ReleaseDC(hwnd, hdc);
}