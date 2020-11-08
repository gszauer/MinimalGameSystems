#include "ClassicSkin.h"
#include "../Renderer.h"
#include "../Control.h"

Forms::ClassicSkin::ClassicSkin(Renderer& r) : Skin(r) { }

unsigned int Forms::ClassicSkin::GetPadding() const {
	return mBorderSize * 2;
}

void Forms::ClassicSkin::DrawPanel(const Rect& layout, const Rect& clip) const {
	if (layout.IsDegenerate() || clip.IsDegenerate()) {
		return;
	}

	Color minOuter = mBorderOuterLT;
	Color minInner = mBorderInnerLT;
	Color maxOuter = mBorderOuterRB;
	Color maxInner = mBorderInnerRB;

	int borderSize = (int)mBorderSize;

	// Draw the background rectangle
	Rect content = layout;
	content = content.AdjustPosition(borderSize * 2, borderSize * 2);
	content = content.AdjustSize(-(borderSize * 4), -(borderSize * 4));
	mRenderer->DrawRect(content, clip, mContentColor);

	// Draw bottom border
	Rect border = layout; // Same size as content

	border.SetPosition(layout.GetLeft(), layout.GetBottom() - borderSize); // Move to 1 above the bottom
	border.SetSize(border.GetWidth(), mBorderSize); // Set height to be 1
	mRenderer->DrawRect(border, clip, maxOuter); // Draw outer border

	border = border.AdjustPosition(borderSize, -borderSize); // Move left and up by 1
	border = border.AdjustSize(-(borderSize * 2), 0); // Reduce width by 1
	mRenderer->DrawRect(border, clip, maxInner); // Draw inner border

	// Draw top border
	border = layout; // Same size as content

	border.SetSize(border.GetWidth(), mBorderSize); // Set the height of the border to be border size
	border = border.AdjustSize(-borderSize, 0);  // Reduce the width to not overdraw right border
	mRenderer->DrawRect(border, clip, minOuter); // Draw the outer border
	
	border = border.AdjustPosition(mBorderSize, mBorderSize); // Step the top left and right down by one
	border = border.AdjustSize(-(borderSize * 2), 0); // Reduce the width by two
	mRenderer->DrawRect(border, clip, minInner); // Draw the inner rectangle
	
	// Draw left border
	border = layout; // Same size as content
	
	border.SetSize(mBorderSize, border.GetHeight()); // Set the width of the border to be border size
	border = border.AdjustPosition(0, borderSize); // Move down by 1
	border = border.AdjustSize(0, -(borderSize * 2)); // Reduce height by 2
	mRenderer->DrawRect(border, clip, minOuter); // Draw the outer border

	border = border.AdjustPosition(borderSize, borderSize); // Move in and down by 1
	border = border.AdjustSize(0, -(borderSize * 2)); // Reduce height by 2
	mRenderer->DrawRect(border, clip, minInner); // Draw inner border

	// Draw right border
	border = layout; // Same size as content

	border.SetSize(mBorderSize, border.GetHeight()); // Set the width of the border to be 1
	border.SetPosition(layout.GetRight() - borderSize, border.GetTop()); // Move to the right and in
	border = border.AdjustSize(0, -borderSize); // Reduce height by 1
	mRenderer->DrawRect(border, clip, maxOuter); // Draw outer

	border = border.AdjustPosition(-(int)mBorderSize, mBorderSize); // Move in by 1 and down by 1
	border = border.AdjustSize(0, -(borderSize * 2)); // Reduce height by 2
	mRenderer->DrawRect(border, clip, maxInner); // Draw inner
}