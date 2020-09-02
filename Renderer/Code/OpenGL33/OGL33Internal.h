#ifndef _H_OGL33INTERNAL_
#define _H_OGL33INTERNAL_

#include <new>

namespace Renderer {
	namespace OGL33Internal {
		void* Alloc(unsigned int numBytes);
		void Free(void* mem);
		int StrCmp(const char* str1, const char* str2);
	}
}

#endif