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
		Box mRelativeLayout;

		Box mLayoutUnClipped; // Absolute (Rename to CachedAbsoluteLayout)
		Box mLayoutClipped; // Absolute (Rename to CachedAbsoluteLayoutClipped)
		// TODO: If layout is dirty, need to re-layout? (mAbsoluteLayoutDirty)

		Control* mParent;
		std::vector<Control*> mChildren;
		
		Overflow mOverflow;
		
		Docking mDocking;
		Size mMinSize;
		Size mMaxSize;
	public:
		inline Control(Control* parent = 0, const Box& box = Box()) : 
			mRelativeLayout(box), mOverflow(Overflow::Visible), mDocking(Docking::None) {
			mParent = 0;

			if (parent != 0) {
				SetParent(parent);
			}

		}

		inline void SetDocking(Docking dock) {
			mDocking = dock;
		}

		inline void SetOverflow(Overflow overflow) {
			mOverflow = overflow;
		}

		inline void SetMinLayoutSize(const Size& size) {
			mMinSize = size;
		}

		inline void SetMaxLayoutSize(const Size& size) {
			mMaxSize = size;
		}

		inline void SetRelativeLayout(const Box& box) {
			// TODO: Mark dirty
			mRelativeLayout = box;
		}

		inline Size GetPreferedLayoutSize() const {
			Size prefered = mRelativeLayout.GetLayoutRect().size;
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

		inline void UpdateLayout(const Rect& layoutRect) {
			mLayoutUnClipped = mRelativeLayout; // Copies margin and padding info
			if (mDocking == Docking::Fill) {
				mLayoutUnClipped.margin = Offset(0, 0, 0, 0);
			}
			else if (mDocking == Docking::Left) {
				mLayoutUnClipped.margin.left = 0;
				mLayoutUnClipped.margin.top = 0;
				mLayoutUnClipped.margin.bottom = 0;
			}
			else if (mDocking == Docking::Top) {
				mLayoutUnClipped.margin.left = 0;
				mLayoutUnClipped.margin.top = 0;
				mLayoutUnClipped.margin.right = 0;
			}
			else if (mDocking == Docking::Right) {
				mLayoutUnClipped.margin.right = 0;
				mLayoutUnClipped.margin.top = 0;
				mLayoutUnClipped.margin.bottom = 0;
			}
			else if (mDocking == Docking::Bottom) {
				mLayoutUnClipped.margin.left = 0;
				mLayoutUnClipped.margin.bottom = 0;
				mLayoutUnClipped.margin.right = 0;
			}
			mLayoutUnClipped.AdjustToLayoutRect(layoutRect);

			Rect contentRect = mLayoutUnClipped.GetContentRect();
			int dockLeft = contentRect.x;
			int dockTop = contentRect.y;
			int dockRight = contentRect.GetRight();
			int dockBottom = contentRect.GetBottom();

			unsigned int numChildren = mChildren.size();
			for (unsigned int c = 0; c < numChildren; ++c) {
				Docking childDock = mChildren[c]->mDocking;
				Size childSize = mChildren[c]->GetPreferedLayoutSize();

				// Apply layout
				Rect childLayout = mChildren[c]->mRelativeLayout.GetLayoutRect();
				childLayout.position.x += contentRect.x;
				childLayout.position.y += contentRect.y;

				if (childDock != Docking::None) {
					if (childDock == Docking::Left) {
						childLayout.position.x = dockLeft;
						childLayout.position.y = dockTop;
						childLayout.size.width = childSize.width;
						childLayout.size.height = dockBottom - dockTop;

						dockLeft += childSize.width;
					}
					else if (childDock == Docking::Top) {
						childLayout.position.x = dockLeft;
						childLayout.position.y = dockTop;
						childLayout.size.width = dockRight - dockLeft;
						childLayout.size.height = childSize.height;

						dockTop += childSize.height;
					}
					else if (childDock == Docking::Right) {
						childLayout.position.x = dockRight - childSize.width;
						childLayout.position.y = dockTop;
						childLayout.size.height = dockBottom - dockTop;
						childLayout.size.width = childSize.width;

						dockRight -= childSize.width;
					}
					else if (childDock == Docking::Bottom) {
						childLayout.position.x = dockLeft;
						childLayout.position.y = dockBottom - childSize.height;
						childLayout.size.width = dockRight - dockLeft;
						childLayout.size.height = childSize.height;

						dockBottom -= childSize.height;
					}
					else if (childDock == Docking::Fill) {
						childLayout.position.x = dockLeft;
						childLayout.position.y = dockTop;
						childLayout.size.width = dockRight - dockLeft;
						childLayout.size.height = dockBottom - dockTop;
					}
				}

				mChildren[c]->UpdateLayout(childLayout);
			}
		}

		inline void Clip(const Rect& clipRect, bool shouldClip = false) {
			// TODO: Update layout only if dirty

			mLayoutClipped = mLayoutUnClipped;
			if (shouldClip) {
				mLayoutClipped = mLayoutClipped.ClipTo(clipRect);
			}
			if (mOverflow == Overflow::Hidden) {
				shouldClip = true;
			}

			Rect clipTo = mLayoutClipped.GetContentRect();

			unsigned int numChildren = mChildren.size();
			for (unsigned int c = 0; c < numChildren; ++c) {
				mChildren[c]->Clip(clipTo, shouldClip);
			}
		}

		// TODO: The mark dirty functions, call them invalidate!

		inline Box GetRelativeLayout() const {
			return mRelativeLayout;
		}

		inline Box GetAbsoluteLayout() const {
			// TODO: Layout if dirty
			return mLayoutUnClipped;
		}

		inline Box GetAbsoluteLayoutClipped() const {
			// TODO: Layout if dirty
			// TODO: Clip if dirty
			return mLayoutClipped;
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
			return mChildren[index];
		}
	};
}

#endif