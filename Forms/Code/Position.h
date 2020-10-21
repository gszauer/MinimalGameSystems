#ifndef _H_POSITION_
#define _H_POSITION_

namespace Forms {
	struct Position {
		int x;
		int y;

		inline Position(int _x = 0, int _y = 0) : x(_x), y(_y) { }
	};
}

#endif // !_H_POSITION_
