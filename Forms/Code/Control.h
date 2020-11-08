#ifndef _H_CONTROL_
#define _H_CONTROL_

#include "Rect.h"
#include <vector>

namespace Forms {
	class Skin;

	enum class ControlState {
		Normal,
		Active,
		Focused,
		Disabled
	};

	class Control {
	private:
		Rect mRelativeLayout;

		Control* mParent;
		std::vector<Control*> mChildren;
		
		Skin* mCustomSkin;
		bool mEnabled;
	public:
		Control(Control* parent = 0, const Rect& rect = Rect(), Skin* skin = 0, bool enabled = true);
		
		Skin* GetSkin();
		void SetSkin(Skin* skin);
		
		Rect GetRelativeLayout();
		void SetRelativeLayout(const Rect& layout);
		
		bool GetEnabled() const;
		void SetEnabled(bool enabled);

		Control* GetParent();
		void SetParent(Control* parent);

		bool RemoveChild(Control* child);
		bool AddChild(Control* child);
		
		unsigned int GetNumChildren() const;
		Control* GetChild(unsigned int index) const;
		Rect GetAbsoluteLayout(const Skin& defaultSkin) const;

		// These need to be over-written for each control
		void Draw(Skin& defaultSkin);

		// Skin is needed for all of these, since the padding in the skin
		// might change the display properties of these controls
		Rect GetChildLayout(const Skin& defaultSkin, const Rect& selfLayout, const Control& child) const;
		Rect GetAbsoluteContentClip(const Skin& defaultSkin) const;
	};
}

#endif