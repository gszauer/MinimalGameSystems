#include "Renderer.h"
#include "Rect.h"

Forms::Renderer::Renderer(HWND hWnd) : mHwnd(hWnd), mCacheLength(0) {
	mHdc = GetDC(hWnd);
	mMemoryDc = CreateCompatibleDC(mHdc);

	RECT clientRect;
	GetClientRect(mHwnd, &clientRect);
	mBuffWidth = clientRect.right - clientRect.left;
	mBuffHeight = clientRect.bottom - clientRect.top;
	mBackBuffer = CreateCompatibleBitmap(mHdc, mBuffWidth, mBuffHeight);
}

Forms::Renderer::~Renderer() {
	for (auto it = mBrushes.begin(); it != mBrushes.end(); it++) {
		DeleteObject(it->second);
	}
	mBrushes.clear();

	DeleteObject(mBackBuffer);
	DeleteDC(mMemoryDc);
	ReleaseDC(mHwnd, mHdc);
}

void Forms::Renderer::Clear(const Color& color) {
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
	DrawRect(Rect(0, 0, clientWidth, clientHeight), Rect(0, 0, clientWidth, clientHeight), color);
}

void Forms::Renderer::Present() {
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

void Forms::Renderer::Flush() {
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

void Forms::Renderer::DrawRect(const Rect& rect, const Rect& clip, const Color& color) {
	Rect draw = rect.GetOverlap(clip);
	if (rect.IsDegenerate() || clip.IsDegenerate() || draw.IsDegenerate()) {
		return;
	}

	if (mCacheLength == RENDERER_BATCH) {
		Flush();
	}

	// So long as the root component is from [0, 0] to [screen w, screen h]
	// we should never get an invalid control. The rectangle clipping
	// at the start of this function should take care of it.

	mRectangles[mCacheLength].left = draw.GetLeft();// < 0 ? 0 : draw.GetLeft();
	mRectangles[mCacheLength].top = draw.GetTop();// < 0 ? 0 : draw.GetTop();
	mRectangles[mCacheLength].right = draw.GetRight();
	mRectangles[mCacheLength].bottom = draw.GetBottom();
	mColors[mCacheLength] = RGB(color.r, color.g, color.b);
	mCacheLength += 1;
}