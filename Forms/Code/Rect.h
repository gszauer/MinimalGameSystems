#ifndef _H_RECT_
#define _H_RECT_

namespace Forms {
	class Rect {
	protected:
		int x;
		int y;
		unsigned int width;
		unsigned int height;
	public:
		Rect(int _x = 0, int _y = 0, unsigned int _w = 0, unsigned int _h = 0);
		bool IsDegenerate() const;

		int GetLeft() const;
		int GetTop() const;
		
		int GetRight() const;
		int GetBottom() const;

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;

		void SetPosition(int _x = 0, int _y = 0);
		void SetSize(unsigned int _w = 0, unsigned int _h = 0);
		void SetSides(int left, int top, int right, int bottom);

		Rect AdjustPosition(int deltaX = 0, int deltaY = 0) const;
		Rect AdjustSize(int deltaW = 0, int deltaH = 0) const;
		Rect GetOverlap(const Rect& other) const;

	};
}

#endif