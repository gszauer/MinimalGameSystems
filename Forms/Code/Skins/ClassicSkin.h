#ifndef _H_CLASSICSKIN_
#define _H_CLASSICSKIN_

#include "../Skin.h"

namespace Forms {
	class ClassicSkin : public Skin {
	protected:
		Color mBorderOuterLT = Color(255, 255, 255);
		Color mBorderInnerLT = Color(223, 223, 223);
		Color mBorderOuterRB = Color(10, 10, 10);
		Color mBorderInnerRB = Color(128, 128, 128);

		Color mContentColor = Color(192, 192, 192);

		unsigned int mBorderSize = 1;
	public:
		ClassicSkin(Renderer& r);

		void SetBorderSize(unsigned int border);
		unsigned int GetBorderSize() const;

		virtual unsigned int GetPadding() const;
		virtual void DrawPanel(const Rect& layout, const Rect& clip) const;
	};
}

#endif