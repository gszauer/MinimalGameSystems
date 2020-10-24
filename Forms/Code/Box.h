#ifndef _H_BOX_
#define _H_BOX_

#include "Rect.h"
#include "Offset.h"
#include <algorithm>

#define CONCAT(a, b) CONCAT_(a, b)
#define CONCAT_(a, b) a ## b

#define Box_ApplyDeltaToStyle(_side, _delta) \
for (int _style_i = 0; _style_i < 3; ++_style_i) { \
	if (_delta <= 0) { break; } \
	int min = std::min<int>(CONCAT(result.mStyle[_style_i]., _side), _delta); \
	_delta -= min; \
	CONCAT(result.mStyle[_style_i]., _side) -= min; \
}

namespace Forms {
	struct Box {
		Rect mContentRect;

		union {
			struct {
				Offset mMargin; // TODO: Un-cammel case
				Offset mBorder;
				Offset mPadding;
			};
			Offset mStyle[3];
		};

		inline Box(const Rect& content = Rect(0, 0, 0, 0), const Offset& margin = Offset(0, 0, 0, 0),
			const Offset& border = Offset(0, 0, 0, 0), const Offset& padding = Offset(0, 0, 0, 0)) : 
			mContentRect(content) {
			mMargin = margin;
			mBorder = border;
			mPadding = padding;
		}

		inline Rect GetLayoutRect() const {
			Rect layoutRect = mContentRect;

			int left = mMargin.left + mBorder.left + mPadding.left;
			int top = mMargin.top + mBorder.top + mPadding.top;
			int right = mMargin.right + mBorder.right + mPadding.right;
			int bottom = mMargin.bottom + mBorder.bottom + mPadding.bottom;

			layoutRect.x -= left;
			layoutRect.y -= top;
			layoutRect.width += left + right;
			layoutRect.height += top + bottom;

			return layoutRect;
		}

		inline Box ClipTo(const Rect& rect) const {
			Box result = *this;

			Rect layout = result.GetLayoutRect();
			if (layout.width > 0 && layout.GetLeft() < rect.GetLeft()) {
				int delta = rect.GetLeft() - layout.GetLeft();
				
				Box_ApplyDeltaToStyle(left, delta);

				int min = std::min<int>((int)result.mContentRect.width, delta);
				result.mContentRect.x += min;
				result.mContentRect.width -= min;
				delta -= min;

				Box_ApplyDeltaToStyle(right, delta);

				layout = result.GetLayoutRect();
			}


			if (layout.width > 0 && layout.GetRight() > rect.GetRight()) {
				int delta = layout.GetRight() - rect.GetRight();

				Box_ApplyDeltaToStyle(right, delta);

				int min = std::min<int>((int)result.mContentRect.width, delta);
				result.mContentRect.width -= min;
				delta -= min;

				Box_ApplyDeltaToStyle(left, delta);

				layout = result.GetLayoutRect();
			}


			if (layout.height > 0 && layout.GetTop() < rect.GetTop()) {
				int delta = rect.GetTop() - layout.GetTop();

				Box_ApplyDeltaToStyle(top, delta);

				int min = std::min<int>((int)result.mContentRect.height, delta);
				result.mContentRect.y += min;
				result.mContentRect.height -= min;
				delta -= min;

				Box_ApplyDeltaToStyle(bottom, delta);

				layout = result.GetLayoutRect();
			}

			if (layout.height > 0 && layout.GetBottom() > rect.GetBottom()) {
				int delta = layout.GetBottom() - rect.GetBottom();

				Box_ApplyDeltaToStyle(bottom, delta);

				int min = std::min<int>((int)result.mContentRect.height, delta);
				result.mContentRect.height -= min;
				delta -= min;

				Box_ApplyDeltaToStyle(top, delta);
			}

			return result;
		}
	};
}

#endif