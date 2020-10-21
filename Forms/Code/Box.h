#ifndef _H_BOX_
#define _H_BOX_

#include "Rect.h"
#include "Offset.h"

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

		inline Box(const Rect& content = Rect(0, 0, 0, 0)) : mContentRect(content) {}

		inline Rect GetLayoutRect() const {
			Rect layoutRect = mContentRect;

			unsigned int left = mMargin.left + mBorder.left + mPadding.left;
			unsigned int top = mMargin.top + mBorder.top + mPadding.top;
			unsigned int right = mMargin.right + mBorder.right + mPadding.right;
			unsigned int bottom = mMargin.bottom + mBorder.bottom + mPadding.bottom;

			layoutRect.x -= left;
			layoutRect.y -= top;
			layoutRect.width += left + right;
			layoutRect.height += top + bottom;

			return layoutRect;
		}
	};
}

#endif