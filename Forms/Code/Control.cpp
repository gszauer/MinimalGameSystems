#include "Control.h"
#include "Skin.h"
#include <algorithm>

Forms::Control::Control(Control* parent, const Rect& rect, const Skin* skin) :
	mRelativeLayout(rect), mCustomSkin(skin) {
	mParent = 0;
	if (parent != 0) {
		SetParent(parent);
	}
}

Forms::Control::~Control() { }

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

	// Apply padding, this will push all content so that 0, 0 is in an appropriate position
	// We have to apply the padding here because some controls might choose not to pad
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