#ifndef _H_COLOR_
#define _H_COLOR_

namespace Forms {
	struct Color {
		unsigned char r;
		unsigned char g;
		unsigned char b;

		inline Color(unsigned char _r = 0, unsigned char _g = 0, unsigned char _b = 0) : r(_r), g(_g), b(_b) { }
	};
}

#endif