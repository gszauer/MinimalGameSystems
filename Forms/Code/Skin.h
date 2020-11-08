#ifndef _H_SKIN_
#define _H_SKIN_

#include "Control.h"
#include "Renderer.h" // For Color

namespace Forms {
	class Skin {
	protected:
		Color mNormalMinOuter = Color(255, 255, 255);
		Color mNormalMinInner = Color(223, 223, 223);
		Color mNormalMaxOuter = Color(10, 10, 10);
		Color mNormalMaxInner = Color(128, 128, 128);

		Color mActiveMinOuter = Color(10, 10, 10);
		Color mActiveMinInner = Color(128, 128, 128);
		Color mActiveMaxOuter = Color(255, 255, 255);
		Color mActiveMaxInner = Color(223, 223, 223);

		unsigned int mBorderSize = 1;

		Color mContentColor = Color(192, 192, 192);

		Renderer* mRenderer;
	public:
		Skin(Renderer& r);

		virtual unsigned int GetPadding() const;
		virtual void DrawPanel(const Rect& layout, const Rect& clip, ControlState state);
	};
}

#endif