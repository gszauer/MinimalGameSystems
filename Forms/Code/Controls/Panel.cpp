#include "Panel.h"
#include "../Skin.h"

Forms::Panel::Panel(Control* parent, const Rect& rect, const Skin* skin) : Control(parent, rect, skin) { }

void Forms::Panel::Draw(const Skin& defaultSkin) const {
	const Skin* skin = &defaultSkin;
	if (mCustomSkin != 0) {
		skin = mCustomSkin;
	}

	Rect layoutRect = GetAbsoluteLayout(defaultSkin);
	Rect clipRect = layoutRect; // Don't clip root component

	if (mParent != 0) {
		clipRect = mParent->GetAbsoluteContentClip(defaultSkin);
	}

	skin->DrawPanel(layoutRect, clipRect);

	unsigned int numKids = GetNumChildren();
	for (unsigned int kid = 0; kid < numKids; ++kid) {
		Forms::Control* child = GetChild(kid);
		child->Draw(defaultSkin);
	}
}