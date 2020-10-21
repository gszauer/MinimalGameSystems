#ifndef _H_OFFSET_
#define _H_OFFSET_

namespace Forms {
	struct Offset {
		unsigned int left;
		unsigned int top;
		unsigned int right;
		unsigned int bottom;

		inline Offset(unsigned int l = 0, unsigned int t = 0, unsigned int r = 0, unsigned int b = 0) :
			left(l), top(t), right(r), bottom(b) { }

		inline unsigned int Total() const {
			return left + top + right + bottom;
		}
	};
}

#endif // !_H_OFFSET_
