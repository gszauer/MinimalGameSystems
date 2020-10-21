#ifndef _H_SIZE_
#define _H_SIZE_

namespace Forms {
	struct Size {
		unsigned int width;
		unsigned int height;

		inline Size(unsigned int w, unsigned int h) : width(w), height(h) { }
	};
}


#endif // !_H_SIZE_
