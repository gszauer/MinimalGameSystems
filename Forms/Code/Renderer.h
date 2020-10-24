#ifndef _H_RENDERER_
#define _H_RENDERER_

#include "Rect.h"
#include "Color.h"
#include "Box.h"
#include "Control.h"
#include <vector>
#include <list>
#include <unordered_map>

#undef APIENTRY
#include <windows.h>

#define RENDERER_BATCH 50

namespace Forms {
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
		inline Renderer(HWND hWnd) : mHwnd(hWnd), mCacheLength(0) {
			mHdc = GetDC(hWnd);
			mMemoryDc = CreateCompatibleDC(mHdc);

			RECT clientRect;
			GetClientRect(mHwnd, &clientRect);
			mBuffWidth = clientRect.right - clientRect.left;
			mBuffHeight = clientRect.bottom - clientRect.top;
			mBackBuffer = CreateCompatibleBitmap(mHdc, mBuffWidth, mBuffHeight);
		}

		inline ~Renderer() {
			for (auto it = mBrushes.begin(); it != mBrushes.end(); it++) {
				DeleteObject(it->second);
			}
			mBrushes.clear();
			
			DeleteObject(mBackBuffer);
			DeleteDC(mMemoryDc);
			ReleaseDC(mHwnd, mHdc);
		}

		inline void Clear(const Color& color) {
			// Resize DC if needed
			RECT clientRect;
			GetClientRect(mHwnd, &clientRect);
			unsigned int clientWidth = clientRect.right - clientRect.left;
			unsigned int clientHeight = clientRect.bottom - clientRect.top;

			if (clientWidth > mBuffWidth || clientHeight > mBuffHeight) {
				DeleteObject(mBackBuffer);
				mBackBuffer = CreateCompatibleBitmap(mHdc, clientWidth, clientHeight);
				mBuffWidth = clientWidth;
				mBuffHeight = clientHeight;
			}

			// Clear the screen
			Draw(Rect(0, 0, clientWidth, clientHeight), color);
		}

		inline void Present() {
			Flush();

			PAINTSTRUCT ps;
			BeginPaint(mHwnd, &ps);

			RECT clientRect;
			GetClientRect(mHwnd, &clientRect);
			unsigned int clientWidth = clientRect.right - clientRect.left;
			unsigned int clientHeight = clientRect.bottom - clientRect.top;

			HBITMAP defaultBackBuffer = (HBITMAP)SelectObject(mMemoryDc, mBackBuffer);
			BitBlt(mHdc, 0, 0, clientWidth, clientHeight, mMemoryDc, 0, 0, SRCCOPY);
			SelectObject(mMemoryDc, defaultBackBuffer);

			EndPaint(mHwnd, &ps);
		}

		inline void Flush() {
			if (mCacheLength == 0) {
				return;
			}

			HBITMAP defaultBackBuffer = (HBITMAP)SelectObject(mMemoryDc, mBackBuffer);
			for (unsigned int i = 0; i < mCacheLength; ++i) {
				COLORREF color = mColors[i];
				if (mBrushes.count(color) == 0) {
					mBrushes[color] = CreateSolidBrush(color);
				}

				FillRect(mMemoryDc, &mRectangles[i], mBrushes[color]);
			}
			SelectObject(mMemoryDc, defaultBackBuffer);

			mCacheLength = 0;
		}
		
		inline void Draw(const Rect& rect, const Color& color) {
			if (mCacheLength == RENDERER_BATCH) {
				Flush();
			}

			mRectangles[mCacheLength].left = rect.x;
			mRectangles[mCacheLength].top = rect.y;
			mRectangles[mCacheLength].right = rect.x + rect.width;
			mRectangles[mCacheLength].bottom = rect.y + rect.height;
			mColors[mCacheLength] = RGB(color.r, color.g, color.b);
			mCacheLength += 1;
		}

		inline void Draw(const Box& box, const Color& marginColor = Color(185, 155, 25), const Color& borderolor = Color(55, 55, 55), const Color& paddingColor = Color(125, 65, 185), const Color& contentColor = Color(35, 115, 145)) { // TODO: Default values for colors
			Rect layout = box.GetLayoutRect();

			Color colors[3] = {
				marginColor,
				borderolor,
				paddingColor
			};

			for (int i = 0; i < 3; ++i) {
				Offset style = box.mStyle[i];
				if (style.Total() != 0) {
					Rect top = layout;
					top.height = style.top;
					Draw(top, colors[i]);

					Rect bottom = layout;
					bottom.y = (bottom.y + bottom.height) - style.bottom;
					bottom.height = style.bottom;
					Draw(bottom, colors[i]);

					Rect left = layout;
					left.width = style.left;
					left.y += style.top;
					left.height -= style.top + style.bottom;
					Draw(left, colors[i]);

					Rect right = layout;
					right.x = (right.x + right.width) - style.right;
					right.width = style.right;
					right.y += style.top;
					right.height -= style.top + style.bottom;
					Draw(right, colors[i]);

					layout.x += style.left;
					layout.y += style.top;
					layout.width -= style.left + style.right;
					layout.height -= style.top + style.bottom;
				}
			}

			if (layout.Area() > 0) {
				Draw(layout, contentColor);
			}
		}

		inline void Draw(const Control& control, const Color& marginColor = Color(185, 155, 25), const Color& borderolor = Color(55, 55, 55), const Color& paddingColor = Color(125, 65, 185), const Color& contentColor = Color(35, 115, 145)) { // TODO: Default values for colors
			Box absoluteBox = control.GetAbsoluteBox();
			if (absoluteBox.GetLayoutRect().Area() > 0) {
				Draw(absoluteBox, marginColor, borderolor, paddingColor, contentColor);

				// Push kids
				unsigned int numKids = (unsigned int)control.GetNumChildren();
				for (unsigned int kid = 0; kid < numKids; ++kid) {
					Control* child = control.GetChild(kid);
					if (child != 0) {
						Draw(*child, marginColor, borderolor, paddingColor, contentColor);
					}
				}
			}
		}
	};
}

#endif