#include "Control.h"
#include "Skin.h"
#include <algorithm>

#define ANCHOR_LEFT mAnchor[0]
#define ANCHOR_TOP mAnchor[1]
#define ANCHOR_RIGHT mAnchor[2]
#define ANCHOR_BOTTOM mAnchor[3]

Forms::Control::Control(Control* parent, const Rect& rect, const Skin* skin) :
	mRelativeLayout(rect), mCustomSkin(skin), mAnchoring(Anchor::None) {

	ANCHOR_LEFT = 0;
	ANCHOR_TOP = 0;
	ANCHOR_RIGHT = 0;
	ANCHOR_BOTTOM = 0;

	mParent = 0;
	if (parent != 0) {
		SetParent(parent);
	}
}

Forms::Control::~Control() { }

Forms::Anchor Forms::Control::GetAnchoring() const {
	return mAnchoring;
}

void Forms::Control::SetAnchoring(Anchor anchor) {
	mAnchoring = anchor;
}

unsigned int Forms::Control::GetAnchor(Anchor anchor) const {
	if (anchor == Anchor::Left) {
		return ANCHOR_LEFT;
	}
	else if (anchor == Anchor::Top) {
		return ANCHOR_TOP;
	}
	else if (anchor == Anchor::Right) {
		return ANCHOR_RIGHT;
	}
	else if (anchor == Anchor::Bottom) {
		return ANCHOR_BOTTOM;
	}
	return 0;
}

void Forms::Control::SetAnchor(Anchor anchor, unsigned int value) {
	if (((unsigned int)anchor & (unsigned int)Anchor::Left) == (unsigned int)Anchor::Left) {
		ANCHOR_LEFT = value;
	}

	if (((unsigned int)anchor & (unsigned int)Anchor::Top) == (unsigned int)Anchor::Top) {
		ANCHOR_TOP = value;
	}

	if (((unsigned int)anchor & (unsigned int)Anchor::Right) == (unsigned int)Anchor::Right) {
		ANCHOR_RIGHT = value;
	}

	if (((unsigned int)anchor & (unsigned int)Anchor::Bottom) == (unsigned int)Anchor::Bottom) {
		ANCHOR_BOTTOM = value;
	}
}

Forms::Rect Forms::Control::GetAbsoluteLayout(const Skin& defaultSkin) const {
	Rect absoluteLayout = mRelativeLayout;

	if (mParent != 0) {
		Rect parentLayout = mParent->GetAbsoluteLayout(defaultSkin);
		absoluteLayout = mParent->GetChildLayout(defaultSkin, parentLayout, *this);
	}

	return absoluteLayout;
}

const Forms::Skin* Forms::Control::GetSkin() const {
	return mCustomSkin;
}

void Forms::Control::SetSkin(const Skin* skin) {
	mCustomSkin = skin;
}

Forms::Rect Forms::Control::GetRelativeLayout() const {
	return mRelativeLayout;
}

void Forms::Control::SetRelativeLayout(const Rect& layout) {
	mRelativeLayout = layout;
}

Forms::Control* Forms::Control::GetParent() const {
	return mParent;
}

void Forms::Control::SetParent(Control* parent) {
	if (mParent != 0) {
		mParent->RemoveChild(*this);
	}
	mParent = 0;
	if (parent != 0) {
		parent->AddChild(*this);
		mParent = parent;
	}
}

bool Forms::Control::RemoveChild(Control& child) {
	unsigned int numChildren = (unsigned int)mChildren.size();
	for (unsigned int c = 0; c < numChildren; ++c) {
		if (mChildren[c] == &child) {
			return RemoveChild(c);
		}
	}
	return false;
}

bool Forms::Control::RemoveChild(unsigned int child) {
	unsigned int numChildren = (unsigned int)mChildren.size();
	if (child >= numChildren) {
		return false;
	}

	mChildren[child]->mParent = 0;
	mChildren.erase(mChildren.begin() + child);
	return true;
}

bool Forms::Control::AddChild(Control& child) {
	unsigned int numChildren = (unsigned int)mChildren.size();
	for (unsigned int c = 0; c < numChildren; ++c) {
		if (mChildren[c] == &child) {
			return false;
		}
	}
	
	if (child.mParent != 0) {
		child.mParent->RemoveChild(child);
		child.mParent = 0;
	}
	
	child.mParent = this;
	mChildren.push_back(&child);

	return true;
}

unsigned int Forms::Control::GetNumChildren() const {
	return mChildren.size();
}

Forms::Control* Forms::Control::GetChild(unsigned int index) const {
	return mChildren[index];
}

Forms::Rect Forms::Control::GetChildLayout(const Skin& defaultSkin, const Rect& selfLayout, const Control& child) const {
	const Skin* skin = &defaultSkin;
	if (mCustomSkin != 0) {
		skin = mCustomSkin;
	}

	// Child layout in local space
	Rect childLocal = child.mRelativeLayout;
	
	// Move into parent space
	Rect childRelative = childLocal.AdjustPosition(selfLayout.GetLeft(), selfLayout.GetTop());

	// Apply parent padding
	int padding = (int)skin->GetPadding();
	Rect padded = childRelative.AdjustPosition(padding, padding);

	if (mAnchoring == Anchor::None) {
		// Apply padding, this will push all content so that 0, 0 is in an appropriate position
		// We have to apply the padding here because some controls might choose not to pad
		return padded;
	}

	// Now for anchoring 
	// TODO: Actually find the valuees of this
	bool leftSet = false;
	bool topSet = false;
	bool rightSet = false;
	bool bottomSet = false;

	Rect result = padded;

	if (leftSet && rightSet) {
		// TODO: Change position and size
	}
	else if (leftSet) {
		int xPos = selfLayout.GetLeft();
		result.SetPosition(xPos, 0);
		result = result.AdjustPosition(padding, 0);
		result = result.AdjustPosition((int)ANCHOR_LEFT, 0);
	}
	else if (rightSet) {
		// TODO: Change position only
	}

	if (topSet && bottomSet) {
		// TODO: Change position and size
	}
	else if (topSet) {
		// TODO: Change position only
	}
	else if (bottomSet) {
		// TODO: Change position only
	}

	return padded;
}

Forms::Rect Forms::Control::GetAbsoluteContentClip(const Skin& defaultSkin) const {
	const Skin* skin = &defaultSkin;
	if (mCustomSkin != 0) {
		skin = mCustomSkin;
	}

	Rect absoluteClipping = GetAbsoluteLayout(defaultSkin);
	absoluteClipping = absoluteClipping.AdjustPosition((int)skin->GetPadding(), (int)skin->GetPadding());
	absoluteClipping = absoluteClipping.AdjustSize(-(int)skin->GetPadding() * 2, -(int)skin->GetPadding() * 2);

	if (mParent != 0) {
		Rect parentClip = mParent->GetAbsoluteContentClip(defaultSkin);
		absoluteClipping = absoluteClipping.GetOverlap(parentClip);
	}

	return absoluteClipping;
}

