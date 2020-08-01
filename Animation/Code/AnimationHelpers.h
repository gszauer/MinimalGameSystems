#ifndef _H_ANIMATIONHELPERS_
#define _H_ANIMATIONHELPERS_

namespace Animation {
    namespace Internal {
        void CombineTransforms(float* outPos, float* outRot, float* outScale, const float* posA, const float* rotA, const float* sclA, const float* posB, const float* rotB, const float* sclB); // TODO: Only called once in AnimationState.cpp, consider inlining
        void TransformToMatrix(float* outMatrix, const float* position, const float* rot, const float* scale); // TODO: Only called in 2 places in AnimationState.cpp, consider inlining
        void MultiplyMatrices(float* out, const float* a, const float* b); // TODO: This is mostly used in samples, move it to ISample and inline it
        void InvertMatrix(float* out, const float* in); // TODO: This is only used for generating the inv-bind-pose. Inline it.
        void MultiplyMat4Vec4(float* out, const float* mat, const float* vec); // TODO: Only used in AnimationSkin.cpp, consider inlining it

    }
    
    // TODO: These should not be global functions. Instead, create AnimationSerializer.h/cpp that will serialize and deserialize files.
    // This will be optional.

    unsigned int StringLengthUInt(unsigned int v);
    unsigned int StringLengthInt(int v);
    unsigned int StringLengthFloat(float v); // Will always have 5 decimals
    
    const char* ReadInt(const char* target, int& v);
    const char* ReadUInt(const char* target, unsigned int& v);
    const char* ReadFloat(const char* target, float& v); 
    
    char* WriteUInt(char* target, unsigned int v);
    char* WriteInt(char* target, int v);
    char* WriteFloat(char* target, float v); // Will always have 5 decimals
    
    unsigned int CountDigits(unsigned int n);
    const char* IgnoreUntilNumber(const char* target);
    char* WriteNewLine(char* target);

    bool FloatCompare(float a, float b);
    float InvSqrt(float number);
    float FMod(float x, float y);

    void* Allocate(unsigned int bytes); // TODO: I can figure out how to drop the rest of this file, but not how to drop the Alloc/Free calls here. Maybe a global function pointer?!?
    void Free(void* memory);
}

#endif