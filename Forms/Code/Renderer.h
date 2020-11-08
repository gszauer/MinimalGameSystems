#ifndef _H_RENDERER_
#define _H_RENDERER_

#include <vector>
#include <list>
#include <unordered_map>

#undef APIENTRY
#include <windows.h>

#define RENDERER_BATCH 50

namespace Forms {
	struct Color {
		unsigned char r;
		unsigned char g;
		unsigned char b;

		inline Color(unsigned char _r = 0, unsigned char _g = 0, unsigned char _b = 0) : r(_r), g(_g), b(_b) { }
	};

	class Rect;

	class Renderer {
	protected:
		HWND mHwnd;
		HDC mHdc;

		RECT mRectangles[RENDERER_BATCH];
		COLORREF mColors[RENDERER_BATCH];
		unsigned int mCacheLength;
		std::unordered_map<COLORREF, HBRUSH> mBrushes;

		HDC mMemoryDc;
		HBITMAP mBackBuffer;
		unsigned int mBuffWidth;
		unsigned int mBuffHeight;
	public:
		Renderer(HWND hWnd);
		~Renderer();
		void Clear(const Color& color);
		void Present();
		void Flush();
		void DrawRect(const Rect& rect, const Rect& clip, const Color& color);
	};
}

#endif