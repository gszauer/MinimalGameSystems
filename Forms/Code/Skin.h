#ifndef _H_ISKIN_
#define _H_ISKIN_

#include "Rect.h"
#include "Renderer.h" // For Color (brings in Renderer too)

namespace Forms {
	class Skin {
	protected:
		Renderer* mRenderer;
	private:
		Skin(); // Renderer is required to create skin
		Skin(const Skin& other); // Disable
		Skin& operator=(const Skin& other); // Disable
	public:
		Skin(Renderer& r);
		virtual ~Skin();

		virtual unsigned int GetPadding() const = 0;

		virtual void DrawPanel(const Rect& layout, const Rect& clip) const = 0;
		// virtual void DrawLabel(const Rect& layout, const Rect& clip, const String& string); // Font, size, etc are a part of skin

		//virtual void DrawButton(const Rect& layout, const Rect& clip, State);
		//virtual void DrawLabelButton(const Rect& layout, const Rect& clip, State, const string& string);
		//virtual void DrawImageButton(const Rect& layout, const Rect& clip, State, const Image& image);

	};
}

#endif