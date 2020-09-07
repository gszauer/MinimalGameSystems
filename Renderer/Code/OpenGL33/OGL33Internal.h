#ifndef _H_OGL33INTERNAL_
#define _H_OGL33INTERNAL_

#include <new>

namespace Renderer {
	namespace OGL33Internal {
		void* Alloc(unsigned int numBytes);
		void Free(const void* mem);
		int StrCmp(const char* str1, const char* str2);
		unsigned int StrLen(const char* str);
		void MemSet(void* target, unsigned char value, unsigned int bytes);
		void StrCpy(char* destination, const char* source);
	}
}

#endif