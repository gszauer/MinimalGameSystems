#ifndef _H_ANIMATIONHELPERS_
#define _H_ANIMATIONHELPERS_

namespace Animation {
    void CombineTransforms(float* outPos, float* outRot, float* outScale, const float* posA, const float* rotA, const float* sclA, const float* posB, const float* rotB, const float* sclB);
    void TransformToMatrix(float* outMatrix, const float* position, const float* rot, const float* scale);
    void MultiplyMatrices(float* out, const float* a, const float* b);
   
    unsigned int UIntStringLength(unsigned int v);
    unsigned int IntStringLength(int v);
    unsigned int FloatStringLength(float v); // Will always have 5 decimals
    
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
    float FastInvSqrt(float number);

    void* Allocate(unsigned int bytes);
    void Free(void* memory);
}

#endif