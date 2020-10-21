#ifndef _H_FORM_
#define _H_FORM_

#include "Control.h"

namespace Forms {
	class Form {
	protected:
		Control* mRoot;
	public:

	};
}

#if 0
#include "Control.h"

namespace Forms {
	class Form {
	protected:
		Control* mRootControl;
	public:

		void Draw();
	};
};
#endif
#endif