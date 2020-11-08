#ifndef _H_CONTROL_
#define _H_CONTROL_

#include "Rect.h"
#include <vector>

namespace Forms {
	class Skin;

	enum class Anchor {
		None = 0,
		Left = 1,
		Top = 2,
		Right = 4,
		Bottom = 8
	};
	Anchor operator|(Anchor a, Anchor b);

	class Control {
	protected:
		Rect mRelativeLayout;

		Anchor mAnchoring;
		unsigned int mAnchor[4];

		Control* mParent;
		std::vector<Control*> mChildren;
		
		const Skin* mCustomSkin;
	public:
		Control(Control* parent = 0, const Rect& rect = Rect(), const Skin* skin = 0);
		virtual ~Control();
		
		const Skin* GetSkin() const;
		void SetSkin(const Skin* skin);

		Anchor GetAnchoring() const;
		void SetAnchoring(Anchor anchor);
		
		unsigned int GetAnchor(Anchor anchor) const;
		void SetAnchor(Anchor anchor, unsigned int value);

		virtual Rect GetRelativeLayout() const;
		virtual void SetRelativeLayout(const Rect& layout);
		
		virtual Control* GetParent() const;
		virtual void SetParent(Control* parent);

		virtual bool AddChild(Control& child);
		virtual unsigned int GetNumChildren() const;
		virtual Control* GetChild(unsigned int index) const;
		virtual bool RemoveChild(unsigned int index);
		virtual bool RemoveChild(Control& child);

		// Skin is needed for all of these, since the padding in the skin
		virtual Rect GetAbsoluteLayout(const Skin& defaultSkin) const;

		// might change the display properties of these controls
		virtual Rect GetChildLayout(const Skin& defaultSkin, const Rect& selfLayout, const Control& child) const;
		virtual Rect GetAbsoluteContentClip(const Skin& defaultSkin) const;

		// These need to be over-written for each control
		virtual void Draw(const Skin& defaultSkin) const = 0;
	};
}

#endif