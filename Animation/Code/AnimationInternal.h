#ifndef _H_ANIMATIONINTERNAL_
#define _H_ANIMATIONINTERNAL_

namespace Animation {
    namespace Internal {
        bool FloatCompare(float a, float b);
        float InvSqrt(float number);
        float FMod(float x, float y);
        void* Allocate(unsigned int bytes); 
        void Free(void* memory);
    }
}

#endif