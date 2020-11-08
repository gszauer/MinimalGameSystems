#include "Rect.h"
#include <algorithm>

Forms::Rect::Rect(int _x, int _y, unsigned int _w, unsigned int _h) : 
	x(_x), y(_y), width(_w), height(_h) { }

bool Forms::Rect::IsDegenerate() const {
	return width == 0 || height == 0;
}

int Forms::Rect::GetLeft() const {
	return x;
}

int Forms::Rect::GetTop() const {
	return y;
}

int Forms::Rect::GetRight() const {
	return x + (int)width;
}

int Forms::Rect::GetBottom() const {
	return y + (int)height;
}

unsigned int Forms::Rect::GetWidth() const {
	return width;
}

unsigned int Forms::Rect::GetHeight() const {
	return height;
}


Forms::Rect Forms::Rect::GetOverlap(const Rect& other) const {
	int left = std::max<int>(x, other.x);
	int top = std::max<int>(y, other.y);
	int right = std::min<int>(x + (int)width, other.x + (int)other.width);
	int bottom = std::min<int>(y + (int)height, other.y + (int)other.height);

	unsigned int width = (unsigned int)std::max<int>(0, right - left);
	unsigned int height = (unsigned int)std::max<int>(0, bottom - top);

	return Rect(left, top, width, height);
}

void Forms::Rect::SetPosition(int _x, int _y) {
	x = _x;
	y = _y;
}

void Forms::Rect::SetSize(unsigned int _w, unsigned int _h) {
	width = _w;
	height = _h;
}

Forms::Rect Forms::Rect::AdjustPosition(int deltaX, int deltaY) const {
	return Rect(x + deltaX, y + deltaY, width, height);
}

Forms::Rect Forms::Rect::AdjustSize(int deltaW, int deltaH) const {
	Rect result = *this;
	unsigned int absDeltaW = (unsigned int)std::abs(deltaW);
	unsigned int absDeltaH = (unsigned int)std::abs(deltaH);

	if (deltaW < 0) {
		result.width -= std::min<unsigned int>(width, absDeltaW);
	}
	else {
		result.width += absDeltaW;
	}

	if (deltaH < 0) {
		result.height -= std::min<unsigned int>(height, absDeltaH);
	}
	else {
		result.height += absDeltaH;
	}

	return result;
}

void Forms::Rect::SetSides(int left, int top, int right, int bottom) {
	if (bottom < top) {
		int temp = bottom;
		bottom = top;
		top = temp;
	}

	if (right < left) {
		int temp = right;
		right = left;
		left = temp;
	}

	x = left;
	y = top;
	width = (unsigned int)(right - left);
	height = (unsigned int)(bottom - top);
}