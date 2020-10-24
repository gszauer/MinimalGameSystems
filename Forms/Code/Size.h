#ifndef _H_SIZE_
#define _H_SIZE_

namespace Forms {
	struct Size {
		union {
			struct {
				unsigned int width;
				unsigned int height;
			};
			unsigned int size[2];
		};

		inline Size(unsigned int w, unsigned int h) : width(w), height(h) { }
	};
}


#endif // !_H_SIZE_
