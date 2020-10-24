#ifndef _H_RECT_
#define _H_RECT_

#include "Position.h"
#include "Size.h"

namespace Forms {
	struct Rect {
		union  {
			struct {
				int x;
				int y;
			};
			Position position;
		};
		
		union {
			struct {
				unsigned int width;
				unsigned int height;
			};
			Size size;
		};
		
		inline Rect(int _x = 0, int _y = 0, unsigned int _w = 0, unsigned int _h = 0) : x(_x), y(_y), width(_w), height(_h) { }
		inline Rect(const Position& pos, const Size& siz = Size(0, 0)) : position(pos), size(siz) { }

		inline unsigned int Area() const {
			return size.width * size.height;
		}

		inline int GetLeft() const {
			return x;
		}

		inline int GetTop() const {
			return y;
		}

		inline int GetRight() const {
			return x + (int)width;
		}

		inline int GetBottom() const {
			return y + (int)height;
		}
	};

#if 0
	struct CachedRect {
		Rect rect;
		bool dirty;
	};
#endif

}

#endif