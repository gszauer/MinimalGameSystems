#ifndef _H_OGL33INTERNAL_
#define _H_OGL33INTERNAL_

#include <new>

namespace Renderer {
	namespace OGL33Internal {
		void* Alloc(unsigned int numBytes);
		void Free(void* mem);
	}
}

#endif