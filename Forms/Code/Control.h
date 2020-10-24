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
			// Later: Scroll
			// Later: Auto
		};
		enum class Docking {
			None,
			Top,
			Left,
			Bottom,
			Right,
			Fill
		};
	protected:
		Box mRelative;
		Control* mParent;
		std::vector<Control*> mChildren;
		Overflow mOverflow;
		Docking mDocking;
		Size mMinSize;
		Size mMaxSize;
		// TODO: Anchoring
	public:
		inline Control(Control* parent = 0, const Box& box = Box()) : mRelative(box), mOverflow(Overflow::Visible), mDocking(Docking::None) {
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

		inline Docking GetDocking() const {
			return mDocking;
		}

		inline Size GetPreferedLayoutSize() const {
			Size prefered = mRelative.GetLayoutRect().size;
			if (prefered.width < mMinSize.width) {
				prefered.width = mMinSize.width;
			}
			if (prefered.height < mMinSize.height) {
				prefered.height = mMinSize.height;
			}
			if (mMaxSize.width > 0 && prefered.width > mMaxSize.width) {
				prefered.width = mMaxSize.width;
			}
			if (mMaxSize.height > 0 && prefered.height > mMaxSize.height) {
				prefered.height = mMaxSize.height;
			}
			return prefered;
		}

		inline void SetMinLayoutSize(const Size& size) {
			mMinSize = size;
		}

		inline void SetMaxLayoutSize(const Size& size) {
			mMaxSize = size;
		}

		inline Rect GetDockedChildRelativeLayoutRect(const Control* target) const {
			Offset mDockingOffsets(0, 0, mRelative.mContentRect.width, mRelative.mContentRect.height);
			unsigned int numChildren = mChildren.size();
			for (unsigned int child = 0; child < numChildren; ++child) {
				Docking childDock = mChildren[child]->GetDocking();
				Size childSize = mChildren[child]->GetPreferedLayoutSize();
				Rect result = mChildren[child]->GetRelativeBox().GetLayoutRect();

				if (childDock == Docking::Left) {
					result.x = mDockingOffsets.left;
					result.y = mDockingOffsets.top;
					result.height = mDockingOffsets.bottom - mDockingOffsets.top;
					result.width = childSize.width;
					mDockingOffsets.left += childSize.width;
				}
				else if (childDock == Docking::Top) {
					result.x = mDockingOffsets.left;
					result.y = mDockingOffsets.top;
					result.width = mDockingOffsets.right - mDockingOffsets.left;
					result.height = childSize.height;
					mDockingOffsets.top += childSize.height;
				}
				else if (childDock == Docking::Right) {
					result.x = mDockingOffsets.right - childSize.width;
					result.y = mDockingOffsets.top;
					result.height = mDockingOffsets.bottom - mDockingOffsets.top;
					result.width = childSize.width;
					mDockingOffsets.right -= childSize.width;
				}
				else if (childDock == Docking::Bottom) {
					result.x = mDockingOffsets.left;
					result.y = mDockingOffsets.bottom - childSize.height;
					result.width = mDockingOffsets.right - mDockingOffsets.left;
					result.height = childSize.height;
					mDockingOffsets.bottom -= childSize.height;
				}
				else if (childDock == Docking::Fill) {
					result.x = mDockingOffsets.left;
					result.y = mDockingOffsets.top;
					result.width = mDockingOffsets.right - mDockingOffsets.left;
					result.height = mDockingOffsets.bottom - mDockingOffsets.top;
				}

				if (mChildren[child] == target) {
					return result;
				}
			}
			return Rect(0, 0, 0, 0);
		}

		inline void SetDocking(Docking dock) {
			mDocking = dock;
		}

		inline void ClearChildren() {
			unsigned int size = mChildren.size();
			for (unsigned int i = 0; i < size; ++i) {
				mChildren[i]->mParent = 0;
			}
			mChildren.clear();
		}
		
		inline Box GetRelativeBox() const {
			return mRelative;
		}

		inline Box GetAbsoluteBox() const {
			Box absoluteBox = GetRelativeBox();
			Box parentAbsoluteBox; // Zero by default
			if (mParent != 0) {
				parentAbsoluteBox = mParent->GetAbsoluteBox();
			}

			// Handle sizing pt1, anchoring

			// Handle sizing pt2, docking
			if (mParent != 0 && mDocking != Docking::None) {
				Rect dockedLayout = mParent->GetDockedChildRelativeLayoutRect(this);
				absoluteBox.mMargin = Offset(0, 0, 0, 0);
				absoluteBox.mContentRect.x = dockedLayout.GetLeft() + absoluteBox.mBorder.left + absoluteBox.mPadding.left;
				absoluteBox.mContentRect.y = dockedLayout.GetTop() + absoluteBox.mBorder.top + absoluteBox.mPadding.top;
				absoluteBox.mContentRect.width = dockedLayout.width - (absoluteBox.mBorder.left + absoluteBox.mPadding.left + absoluteBox.mBorder.right + absoluteBox.mPadding.right);
				absoluteBox.mContentRect.height = dockedLayout.height - (absoluteBox.mBorder.top + absoluteBox.mPadding.top + absoluteBox.mBorder.bottom + absoluteBox.mPadding.bottom);
			}

			// Move relative to parent
			absoluteBox.mContentRect.x += parentAbsoluteBox.mContentRect.x;
			absoluteBox.mContentRect.y += parentAbsoluteBox.mContentRect.y;

			// Handle clipping
			bool shouldClip = false;
			for (Control* it = mParent; it != 0; it = it->mParent) {
				if (it->GetOverflow() == Forms::Control::Overflow::Hidden) {
					shouldClip = true;
					break;
				}
			}

			if (mParent != 0 && shouldClip) {
				absoluteBox = absoluteBox.ClipTo(parentAbsoluteBox.mContentRect);
			}

			return absoluteBox;
		}

		inline void SetParent(Control* parent) {
			if (mParent != 0) {
				mParent->RemoveChild(this);
			}
			mParent = 0;
			if (parent != 0) {
				parent->AddChild(this);
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