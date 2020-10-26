#ifndef _H_STYLE_
#define _H_STYLE_

#include "Color.h"

namespace Forms {
	struct Style {
		union {
			struct {
				Color marginLeftColor;
				Color marginTopColor;
				Color marginRightColor;
				Color marginBottomColor;
			};
			Color marginColor[4];
		};

		union {
			struct {
				Color borderLeftColor;
				Color borderTopColor;
				Color borderRightColor;
				Color borderBottomColor;
			};
			Color borderColor[4];
		};

		union {
			struct {
				Color paddingLeftColor;
				Color paddingTopColor;
				Color paddingRightColor;
				Color paddingBottomColor;
			};
			Color paddingColor[4];
		};

		Color contentColor;

		inline Style(const Color& margin = Color(185, 155, 25), const Color& border = Color(55, 55, 55), const Color& padding = Color(125, 65, 185), const Color& content = Color(35, 115, 145)) {
			for (int i = 0; i < 4; ++i) {
				marginColor[i] = margin;
				borderColor[i] = border;
				paddingColor[i] = padding;
			}
			contentColor = content;
		}
	};
}

#endif // !_H_STYLE_
