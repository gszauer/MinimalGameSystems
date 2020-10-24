#ifndef _H_CONTROL_
#define _H_CONTROL_

#include "Box.h"
#include <vector>

namespace Forms {
	class Control {
	public:
		enum class Overflow {
			Visible,
			Hidden,
			// Scroll
			// Auto
		};

	protected:
		Box mRelative;
		Control* mParent;
		std::vector<Control*> mChildren;
		Overflow mOverflow;
	public:
		inline Control(Control* parent = 0, const Box& box = Box()) : mRelative(box), mOverflow(Overflow::Visible) {
			mParent = 0;
			if (parent != 0) {
				SetParent(parent);
			}
		}

		inline Overflow GetOverflow() const {
			return mOverflow;
		}

		inline void SetOverflow(Overflow flow) {
			mOverflow = flow;
		}
		
		inline Box GetRelativeBox() const {
			return mRelative;
		}

		inline Box GetAbsoluteBox() const {
			Box absolute = GetRelativeBox();
			Control* iterator = mParent;
			bool shouldClip = false;

			while (iterator != 0) {
				if (iterator->GetOverflow() == Forms::Control::Overflow::Hidden) {
					shouldClip = true;
				}
				Box parent = iterator->GetRelativeBox();
				absolute.mContentRect.x += parent.mContentRect.x;
				absolute.mContentRect.y += parent.mContentRect.y;
				iterator = iterator->mParent;
			}

			if (mParent != 0 && shouldClip) {
				Box parentAbsoluteBox = mParent->GetAbsoluteBox();
				absolute = absolute.ClipTo(parentAbsoluteBox.mContentRect);
			}

			return absolute;
		}

		inline void SetParent(Control* parent) {
			if (mParent != 0) {
				mParent->RemoveChild(this);
			}
			mParent = parent;
			if (mParent != 0) {
				mParent->AddChild(this);
			}
		}

		inline Control* GetParent() const {
			return mParent;
		}

		inline bool RemoveChild(Control* child) {
			unsigned int numChildren = (unsigned int)mChildren.size();
			for (unsigned int c = 0; c < numChildren; ++c) {
				if (mChildren[c] == child) {
					mChildren[c]->mParent = 0;
					mChildren.erase(mChildren.begin() + c);
					return true;
				}
			}
			return false;
		}

		inline bool AddChild(Control* child) {
			unsigned int numChildren = (unsigned int)mChildren.size();
			for (unsigned int c = 0; c < numChildren; ++c) {
				if (mChildren[c] == child) {
					return false;
				}
			}
			if (child->mParent != 0) {
				child->mParent->RemoveChild(child);
			}
			child->mParent = this;
			mChildren.push_back(child);
			return true;
		}

		inline unsigned int GetNumChildren() const {
			return mChildren.size();
		}

		inline Control* GetChild(unsigned int index) const {
			if (index >= mChildren.size()) {
				return 0;
			}
			return mChildren[index];
		}
	};
}

#endif