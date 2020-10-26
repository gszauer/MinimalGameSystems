#ifndef _H_CONTROL_
#define _H_CONTROL_

#include "Box.h"
#include "Style.h"

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
	private:
		Box mRelativeLayout;

		Box mAbsoluteLayoutUnClipped;
		Box mAbsoluteLayoutClipped; 

		Control* mParent;
		std::vector<Control*> mChildren;
		
		Overflow mOverflow;
		
		Docking mDocking;
		Size mMinSize;
		Size mMaxSize;

		const Style* mStyle;
	public:
		inline Control(Control* parent = 0, const Box& box = Box(), Style* style = 0) :
			mRelativeLayout(box), mOverflow(Overflow::Visible), mDocking(Docking::None)  {
			mParent = 0;
			mStyle = style;

			if (parent != 0) {
				SetParent(parent);
			}

		}

		inline const Style* GetStyle() const {
			return mStyle;
		}

		inline void SetStyle(const Style* style) {
			mStyle = style;
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
			mRelativeLayout = box;
		}

		inline Size GetPreferedLayoutSize() const {
			Rect layoutRect = mRelativeLayout.GetLayoutRect();
			Size prefered(layoutRect.width, layoutRect.height);

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
			if (layoutRect.Area() == 0) {
				mAbsoluteLayoutUnClipped = Box();
				return;
			}

			// Copies margin and padding info
			mAbsoluteLayoutUnClipped = mRelativeLayout; 
			
			// No margin if docked
			if (mDocking != Docking::None) {
				mAbsoluteLayoutUnClipped.margin = Offset(0, 0, 0, 0);
			}
			
			// Adjust content rect to fit into layout
			mAbsoluteLayoutUnClipped.AdjustToLayoutRect(layoutRect);
			
			// Next, we will need to lay out all children. These are running numbers
			// for the content rect that are used to lay out any children that dock.
			Rect contentRect = mAbsoluteLayoutUnClipped.GetContentRect();
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
				childLayout.x += contentRect.x;
				childLayout.y += contentRect.y;

				if (childDock != Docking::None) {
					if (childDock == Docking::Left) {
						// Handle case where we might not have enough space
						int width = childSize.width;
						if (dockLeft + width > dockRight) {
							int delta = std::max<int>(dockRight - (dockLeft + width), 0);
							width -= std::min<int>(delta, width);;
						}

						childLayout.x = dockLeft;
						childLayout.y = dockTop;
						childLayout.width = width;
						childLayout.height = (dockBottom > dockTop)? dockBottom - dockTop : 0;

						dockLeft += width;
						if (dockLeft > dockRight) {
							dockLeft = dockRight;
						}
					}
					else if (childDock == Docking::Top) {
						childLayout.x = dockLeft;
						childLayout.y = dockTop;
						childLayout.width = (dockRight > dockLeft)? dockRight - dockLeft : 0;
						childLayout.height = childSize.height;

						dockTop += childSize.height;
						if (dockTop > dockBottom) {
							dockTop = dockBottom;
						}
					}
					else if (childDock == Docking::Right) {
						// Handle case where we might not have enough space
						int width = childSize.width;
						if (dockRight - width < dockLeft) {
							int delta = std::max<int>(dockLeft - (dockRight - width), 0);
							width -= std::min<int>(delta, width);;
						}

						childLayout.x = dockRight - width;
						childLayout.y = dockTop;
						childLayout.height = (dockBottom > dockTop)? dockBottom - dockTop : 0; // Avoid underflow
						childLayout.width = width;

						dockRight -= width;
						if (dockRight < dockLeft) {
							dockRight = dockLeft;
						}
					}
					else if (childDock == Docking::Bottom) {
						childLayout.x = dockLeft;
						childLayout.y = dockBottom - childSize.height;
						childLayout.width = (dockRight > dockLeft)? dockRight - dockLeft : 0;
						childLayout.height = childSize.height;

						dockBottom -= childSize.height;
						if (dockBottom < dockTop) {
							dockBottom = dockTop;
						}
					}
					else if (childDock == Docking::Fill) {
						childLayout.x = dockLeft;
						childLayout.y = dockTop;
						childLayout.width = (dockRight > dockLeft)? dockRight - dockLeft : 0;
						childLayout.height = (dockBottom > dockTop)? dockBottom - dockTop : 0;
					}
				}

				mChildren[c]->UpdateLayout(childLayout);
			}
		}

		inline void Clip(const Rect& clipRect, bool shouldClip = false) {
			mAbsoluteLayoutClipped = mAbsoluteLayoutUnClipped;
			if (shouldClip) {
				mAbsoluteLayoutClipped = mAbsoluteLayoutClipped.ClipTo(clipRect);
			}
			if (mOverflow == Overflow::Hidden) {
				shouldClip = true;
			}

			Rect clipTo = mAbsoluteLayoutClipped.GetContentRect();

			unsigned int numChildren = mChildren.size();
			for (unsigned int c = 0; c < numChildren; ++c) {
				mChildren[c]->Clip(clipTo, shouldClip);
			}
		}

		inline Box GetRelativeLayout() const {
			return mRelativeLayout;
		}

		inline Box GetAbsoluteLayout() const {
			return mAbsoluteLayoutUnClipped;
		}

		inline Box GetAbsoluteLayoutClipped() const {
			return mAbsoluteLayoutClipped;
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