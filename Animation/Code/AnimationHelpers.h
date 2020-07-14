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

	static void ScaleMatrix(float* out, const float* in, float s) {
		out[0] = in[0] * s;
		out[1] = in[1] * s;
		out[2] = in[2] * s;
		out[3] = in[3] * s;
		out[4] = in[4] * s;
		out[5] = in[5] * s;
		out[6] = in[6] * s;
		out[7] = in[7] * s;
		out[8] = in[8] * s;
		out[9] = in[9] * s;
		out[10] = in[10] * s;
		out[11] = in[11] * s;
		out[12] = in[12] * s;
		out[13] = in[13] * s;
		out[14] = in[14] * s;
		out[15] = in[15] * s;
	}

	static void AddMatrices(float* out, const float* a, const float* b) {
		out[0] = a[0] + b[0];
		out[1] = a[1] + b[1];
		out[2] = a[2] + b[2];
		out[3] = a[3] + b[3];
		out[4] = a[4] + b[4];
		out[5] = a[5] + b[5];
		out[6] = a[6] + b[6];
		out[7] = a[7] + b[7];
		out[8] = a[8] + b[8];
		out[9] = a[9] + b[9];
		out[10] = a[10] + b[10];
		out[11] = a[11] + b[11];
		out[12] = a[12] + b[12];
		out[13] = a[13] + b[13];
		out[14] = a[14] + b[14];
		out[15] = a[15] + b[15];
	}

	static void MultiplyMat4Vec4(float* out, const float* mat, const float* vec) {
		out[0] = vec[0] * mat[0] + vec[1] * mat[4] + vec[2] * mat[8] + vec[3] * mat[12];
		out[1] = vec[0] * mat[1] + vec[1] * mat[5] + vec[2] * mat[9] + vec[3] * mat[13];
		out[2] = vec[0] * mat[2] + vec[1] * mat[6] + vec[2] * mat[10] + vec[3] * mat[14];
		out[3] = vec[0] * mat[3] + vec[1] * mat[7] + vec[2] * mat[11] + vec[3] * mat[15];
	}
}

#endif