#include "../AnimationInternal.h"

extern unsigned char __heap_base;
unsigned int bump_pointer = (unsigned int)&__heap_base;

void* Animation::Internal::Allocate(unsigned int bytes) {
    unsigned int r = bump_pointer;
    bump_pointer += bytes;
    return (void *)r;
}

void Animation::Internal::Free(void* memory) {
    
}

bool Animation::Internal::FloatCompare(float a, float b) {
    float delta = a - b;
    return delta < 0.000001f && delta > -0.000001f;
}

float Animation::Internal::InvSqrt(float number) {
#if 1
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long*)&y;                
    i = 0x5f3759df - (i >> 1);              
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y));  

    return y;
#else
    return 1.0f / sqrtf(number);
#endif
}

float Animation::Internal::FMod(float x, float y) {
    // fmod(x, y) = x - y * floor(x / y)

    if (y == 0.0f) { // Avoid div by 0
       return 0.0f;
    }
    
    // This is naive, there is a real chance for overflow.
    // Most game animations are not long enough to run into this problem.
    // The quick fix is to use the std implementation of fmod instead
    float floor_target = x / y;
    if (floor_target < 0.0f) { // floor
        floor_target = (float)((int)(floor_target - 1.0f));
    }
    else { 
        floor_target = (float)((int)floor_target);
    }

    float result = x - y * floor_target;

    return result;
}
