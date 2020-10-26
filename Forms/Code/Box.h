#ifndef _H_BOX_
#define _H_BOX_

#include "Rect.h"
#include "Offset.h"
#include <algorithm>

namespace Forms {
	struct Box {
		Rect content;

		union {
			struct {
				Offset margin;
				Offset border;
				Offset padding;
			};
			Offset offsets[3];
		};

		inline Box(const Rect& c = Rect(0,0), const Offset& _margin = Offset(0, 0, 0, 0),
			const Offset& _border = Offset(0, 0, 0, 0), const Offset& _padding = Offset(0, 0, 0, 0)) {
			content = c;
			margin = _margin;
			border = _border;
			padding = _padding;
		}

		inline Rect GetContentRect() const {
			return content;
		}

		inline Rect GetLayoutRect() const {
			int left = margin.left + border.left + padding.left;
			int right = margin.right + border.right + padding.right;
			int top = margin.top + border.top + padding.top;
			int bottom = margin.bottom + border.bottom + padding.bottom;

			Rect layout = GetContentRect();
			layout.x -= left;
			layout.y -= top;
			layout.width += left + right;
			layout.height += top + bottom;

			return layout;
		}

		inline void AdjustToLayoutRect(const Rect& layout) {
			int left = margin.left + border.left + padding.left;
			int right = margin.right + border.right + padding.right;
			int top = margin.top + border.top + padding.top;
			int bottom = margin.bottom + border.bottom + padding.bottom;

			content = layout;
			content.x += left;
			content.y += top;
			content.width -= std::min<unsigned int>(content.width, left + right);
			content.height -= std::min<unsigned int>(content.height, top + bottom);
		}

		inline Box ClipTo(const Rect& rect) const {
			Box result = *this;

			if (rect.Area() == 0) {
				result = Box();
			}
			else {
				Rect layoutRect = result.GetLayoutRect(); // Left
				if (layoutRect.width > 0 && layoutRect.GetLeft() < rect.GetLeft()) {
					int delta = rect.GetLeft() - layoutRect.GetLeft();

					// Eat left margin
					int min = std::min<int>(result.margin.left, delta);
					result.margin.left -= min;
					delta -= min;

					// Eat left border
					min = std::min<int>(result.border.left, delta);
					result.border.left -= min;
					delta -= min;

					// Eat left padding
					min = std::min<int>(result.padding.left, delta);
					result.padding.left -= min;
					delta -= min;

					// Eat content
					min = std::min<int>((int)result.content.width, delta);
					result.content.x += min;
					result.content.width -= min;
					delta -= min;

					// By this point, the content has a width of 0 if clipping should still keep happening
					// To avoid having the clipped padding swimming, the content must be pushed to the right.
					// At this point, the push is safe, as the content is already clipped so the user can't see.

					// Eat right padding
					min = std::min<int>(result.padding.right, delta);
					result.padding.right -= min;
					result.content.x += min;
					delta -= min;

					// Eat right border
					min = std::min<int>(result.border.right, delta);
					result.border.right -= min;
					result.content.x += min;
					delta -= min;

					// Eat right margin
					min = std::min<int>(result.margin.right, delta);
					result.margin.right -= min;
					result.content.x += min;
					delta -= min;
				}

				layoutRect = result.GetLayoutRect(); // Right
				if (layoutRect.width > 0 && layoutRect.GetRight() > rect.GetRight()) {
					int delta = layoutRect.GetRight() - rect.GetRight();

					// Eat right margin
					int min = std::min<int>(result.margin.right, delta);
					result.margin.right -= min;
					delta -= min;

					// Eat right border 
					min = std::min<int>(result.border.right, delta);
					result.border.right -= min;
					delta -= min;

					// Eat right padding
					min = std::min<int>(result.padding.right, delta);
					result.padding.right -= min;
					delta -= min;

					// Eat content
					min = std::min<int>((int)result.content.width, delta);
					result.content.width -= min;
					delta -= min;

					// Same as above, the content here is actually pushed over a bit

					// Eat left padding
					min = std::min<int>(result.padding.left, delta);
					result.padding.left -= min;
					result.content.x -= min;
					delta -= min;

					// Eat left border
					min = std::min<int>(result.border.left, delta);
					result.border.left -= min;
					result.content.x -= min;
					delta -= min;

					// Eat left margin
					min = std::min<int>(result.margin.left, delta);
					result.margin.left -= min;
					result.content.x -= min;
					delta -= min;
				}

				layoutRect = result.GetLayoutRect(); // Top
				if (layoutRect.height > 0 && layoutRect.GetTop() < rect.GetTop()) {
					int delta = rect.GetTop() - layoutRect.GetTop();

					// Eat top margin
					int min = std::min<int>(result.margin.top, delta);
					result.margin.top -= min;
					delta -= min;

					// Eat top border
					min = std::min<int>(result.border.top, delta);
					result.border.top -= min;
					delta -= min;

					// Eat top padding
					min = std::min<int>(result.padding.top, delta);
					result.padding.top -= min;
					delta -= min;

					// Eat content
					min = std::min<int>((int)result.content.height, delta);
					result.content.y += min;
					result.content.height -= min;
					delta -= min;

					// Same as above, the content here is actually pushed over a bit

					// Eat bottom padding
					min = std::min<int>(result.padding.bottom, delta);
					result.padding.bottom -= min;
					result.content.y += min;
					delta -= min;

					// Eat bottom border
					min = std::min<int>(result.border.bottom, delta);
					result.border.bottom -= min;
					result.content.y += min;
					delta -= min;

					// Eat bottom margin
					min = std::min<int>(result.margin.bottom, delta);
					result.margin.bottom -= min;
					result.content.y += min;
					delta -= min;
				}

				layoutRect = result.GetLayoutRect(); // Bottom
				if (layoutRect.width > 0 && layoutRect.GetBottom() > rect.GetBottom()) {
					int delta = layoutRect.GetBottom() - rect.GetBottom();

					// Eat bottom margin
					int min = std::min<int>(result.margin.bottom, delta);
					result.margin.bottom -= min;
					delta -= min;

					// Eat bottom border 
					min = std::min<int>(result.border.bottom, delta);
					result.border.bottom -= min;
					delta -= min;

					// Eat bottom padding
					min = std::min<int>(result.padding.bottom, delta);
					result.padding.bottom -= min;
					delta -= min;

					// Eat content
					min = std::min<int>((int)result.content.height, delta);
					result.content.height -= min;
					delta -= min;

					// Same as above, the content here is actually pushed over a bit

					// Eat top padding
					min = std::min<int>(result.padding.top, delta);
					result.padding.top -= min;
					result.content.y -= min;
					delta -= min;

					// Eat top border
					min = std::min<int>(result.border.top, delta);
					result.border.top -= min;
					result.content.y -= min;
					delta -= min;

					// Eat top margin
					min = std::min<int>(result.margin.top, delta);
					result.margin.top -= min;
					result.content.y -= min;
					delta -= min;
				}

			}

			return result;
		}
	};
}

#endif