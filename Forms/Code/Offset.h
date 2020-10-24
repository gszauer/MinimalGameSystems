#ifndef _H_OFFSET_
#define _H_OFFSET_

namespace Forms {
	struct Offset {
		union {
			struct {
				int left;
				int top;
				int right;
				int bottom;
			};
			int offsets[4];
		};

		inline Offset(int l = 0, int t = 0, int r = 0, int b = 0) :
			left(l), top(t), right(r), bottom(b) { }

		inline int Total() const {
			return left + top + right + bottom;
		}
	};
}

#endif // !_H_OFFSET_
