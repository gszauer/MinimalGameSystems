#ifndef _H_RECT_
#define _H_RECT_

namespace Forms {
	struct Rect {
		int x;
		int y;
		int width;
		int height;

		inline Rect(int _x = 0, int _y = 0, int _w = 0, int _h = 0) : x(_x), y(_y), width(_w), height(_h) { }
	};

#if 0
	struct CachedRect {
		Rect rect;
		bool dirty;
	};
#endif

}

#endif