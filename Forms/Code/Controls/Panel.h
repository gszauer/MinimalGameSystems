#ifndef _H_PANEL_
#define _H_PANEL_

#include "../Control.h"

namespace Forms {
	class Panel : public Control {
	public:
		Panel(Control* parent = 0, const Rect& rect = Rect(), const Skin* skin = 0);
		virtual void Draw(const Skin& defaultSkin) const;
	};
}

#endif