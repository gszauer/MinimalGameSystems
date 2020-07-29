#include "AnimationHelpers.h"
#include <cmath> // for sqrt. Can use fast inv sqrt as well

void Animation::CombineTransforms(float* outPos, float* outRot, float* outScale, const float* posA, const float* rotA, const float* sclA, const float* posB, const float* rotB, const float* sclB) {
    float resultScale[3] = { 1.0f };
    float resultPos[3] = { 0.0f };
    float resultRot[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    // vec * vec
    resultScale[0] = sclA[0] * sclB[0];
    resultScale[1] = sclA[1] * sclB[1];
    resultScale[2] = sclA[2] * sclB[2];

    // quat * quat
    resultRot[0] = rotB[0] * rotA[3] + rotB[1] * rotA[2] - rotB[2] * rotA[1] + rotB[3] * rotA[0];
    resultRot[1] = -rotB[0] * rotA[2] + rotB[1] * rotA[3] + rotB[2] * rotA[0] + rotB[3] * rotA[1];
    resultRot[2] = rotB[0] * rotA[1] - rotB[1] * rotA[0] + rotB[2] * rotA[3] + rotB[3] * rotA[2];
    resultRot[3] = -rotB[0] * rotA[0] - rotB[1] * rotA[1] - rotB[2] * rotA[2] + rotB[3] * rotA[3];

    // vec * vec
    resultPos[0] = sclA[0] * posB[0];
    resultPos[1] = sclA[1] * posB[1];
    resultPos[2] = sclA[2] * posB[2];

    // quat * vec
    float v[3] = { resultPos[0], resultPos[1], resultPos[2] };
    float d1 = rotA[0] * v[0] + rotA[1] * v[1] + rotA[2] * v[2];
    float d2 = rotA[0] * rotA[0] + rotA[1] * rotA[1] + rotA[2] * rotA[2];

    resultPos[0] = (rotA[0] * 2.0f * d1) + (v[0] * (rotA[3] * rotA[3] - d2)) + ((rotA[1] * v[2] - rotA[2] * v[1]) * 2.0f * rotA[3]);
    resultPos[1] = (rotA[1] * 2.0f * d1) + (v[1] * (rotA[3] * rotA[3] - d2)) + ((rotA[2] * v[0] - rotA[0] * v[2]) * 2.0f * rotA[3]);
    resultPos[2] = (rotA[2] * 2.0f * d1) + (v[2] * (rotA[3] * rotA[3] - d2)) + ((rotA[0] * v[1] - rotA[1] * v[0]) * 2.0f * rotA[3]);

    // vec + vec
    resultPos[0] = posA[0] + resultPos[0];
    resultPos[1] = posA[1] + resultPos[1];
    resultPos[2] = posA[2] + resultPos[2];

    // Copy data out
    for (int i = 0; i < 3; ++i) {
        outScale[i] = resultScale[i];
        outPos[i] = resultPos[i];
        outRot[i] = resultRot[i];
    }
    outRot[3] = resultRot[3];
}

void Animation::TransformToMatrix(float* outMatrix, const float* position, const float* rot, const float* scale) {
    // First, extract the rotation basis of the transform
    float v[3] = { 1.0f, 0.0f, 0.0f };
    float d1 = rot[0] * v[0] + rot[1] * v[1] + rot[2] * v[2];
    float d2 = rot[0] * rot[0] + rot[1] * rot[1] + rot[2] * rot[2];
    float x[3] = {
         (rot[0] * 2.0f * d1) + (v[0] * (rot[3] * rot[3] - d2)) + ((rot[1] * v[2] - rot[2] * v[1]) * 2.0f * rot[3]),
         (rot[1] * 2.0f * d1) + (v[1] * (rot[3] * rot[3] - d2)) + ((rot[2] * v[0] - rot[0] * v[2]) * 2.0f * rot[3]),
         (rot[2] * 2.0f * d1) + (v[2] * (rot[3] * rot[3] - d2)) + ((rot[0] * v[1] - rot[1] * v[0]) * 2.0f * rot[3])
    };

    v[0] = 0.0f; v[1] = 1.0f;
    d1 = rot[0] * v[0] + rot[1] * v[1] + rot[2] * v[2];
    float y[3] = {
        (rot[0] * 2.0f * d1) + (v[0] * (rot[3] * rot[3] - d2)) + ((rot[1] * v[2] - rot[2] * v[1]) * 2.0f * rot[3]),
        (rot[1] * 2.0f * d1) + (v[1] * (rot[3] * rot[3] - d2)) + ((rot[2] * v[0] - rot[0] * v[2]) * 2.0f * rot[3]),
        (rot[2] * 2.0f * d1) + (v[2] * (rot[3] * rot[3] - d2)) + ((rot[0] * v[1] - rot[1] * v[0]) * 2.0f * rot[3])
    };

    v[1] = 0.0f; v[2] = 1.0f;
    d1 = rot[0] * v[0] + rot[1] * v[1] + rot[2] * v[2];
    float z[3] = {
        (rot[0] * 2.0f * d1) + (v[0] * (rot[3] * rot[3] - d2)) + ((rot[1] * v[2] - rot[2] * v[1]) * 2.0f * rot[3]),
        (rot[1] * 2.0f * d1) + (v[1] * (rot[3] * rot[3] - d2)) + ((rot[2] * v[0] - rot[0] * v[2]) * 2.0f * rot[3]),
        (rot[2] * 2.0f * d1) + (v[2] * (rot[3] * rot[3] - d2)) + ((rot[0] * v[1] - rot[1] * v[0]) * 2.0f * rot[3])
    };

    // Next, scale the basis vectors
    x[0] *= scale[0];
    x[1] *= scale[0];
    x[2] *= scale[0];

    y[0] *= scale[1];
    y[1] *= scale[1];
    y[2] *= scale[1];

    z[0] *= scale[2];
    z[1] *= scale[2];
    z[2] *= scale[2];

    // Create matrix
    outMatrix[0] = x[0];
    outMatrix[1] = x[1];
    outMatrix[2] = x[2];
    outMatrix[3] = 0.0f;

    outMatrix[4] = y[0];
    outMatrix[5] = y[1];
    outMatrix[6] = y[2];
    outMatrix[7] = 0.0f;

    outMatrix[8] = z[0];
    outMatrix[9] = z[1];
    outMatrix[10] = z[2];
    outMatrix[11] = 0.0f;

    outMatrix[12] = position[0];
    outMatrix[13] = position[1];
    outMatrix[14] = position[2];
    outMatrix[15] = 1.0f;
}

void Animation::MultiplyMatrices(float* out, const float* a, const float* b) {
    float result[16] = { 0.0f };

    // Column 0
    result[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
    result[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
    result[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
    result[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];

    // Column 1
    result[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
    result[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
    result[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
    result[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];

    // Column 2
    result[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
    result[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
    result[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
    result[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];

    // Column 3
    result[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
    result[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
    result[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
    result[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];

    for (unsigned int i = 0; i < 16; ++i) {
        out[i] = result[i];
    }
}

unsigned int Animation::CountDigits(unsigned int n) {
    int length = 0;
    if (n == 0) {
        length = 1;
    }
    while (n != 0) {
        n = n / 10;
        ++length;
    }
    return length;
}

unsigned int Animation::UIntStringLength(unsigned int v) {
    return CountDigits(v);
}

unsigned int Animation::IntStringLength(int v) {
    unsigned int length = 0;
    if (v < 0) {
        length = 1;
        v = -v;
    }
    unsigned int uv = (unsigned int)v;

    return CountDigits(uv) + length;;
}

unsigned int Animation::FloatStringLength(float v) {
    // Floating point numbers will always have 5 decimal places, hard coding that
    unsigned int length = 0;
    if (v < 0.0f) {
        length = 1;
        v = -v;
    }
    unsigned int uv = (unsigned int)v;
    length += CountDigits(uv) + 1 + 5;

    return length;
}

char* Animation::WriteUInt(char* target, unsigned int v) {
    if (v == 0) {
        *target = '0';
        target += 1;
    }
    else {
        unsigned int numDigits = CountDigits(v);
        char* write = target + numDigits - 1;

        while (v > 10) {
            unsigned int quotient = v / 10;
            unsigned int remainder = v % 10;

            *write = '0' + (char)remainder;
            write -= 1;

            v = quotient;
        }

        if (v == 10) {
            *write = '0';
            write -= 1;
            *write = '1';
        }
        else if (v != 0) {
            *write = '0' + (char)v;
        }
        target += numDigits;
    }

    *target = ' ';
    target += 1;

    return target;
}

char* Animation::WriteInt(char* target, int v) {
    if (v < 0) {
        *target = '-';
        target += 1;
        v = -v;
    }
    unsigned int uv = (unsigned int)v;

    return WriteUInt(target, uv);
}

char* Animation::WriteFloat(char* target, float v) {
    if (v < 0) {
        *target = '-';
        target += 1;
        v = -v;
    }

    unsigned int whole = (unsigned int)v;
    target = WriteUInt(target, whole);

    // WriteUInt leaves a space at the end of the string
    // Go back one and replace it with the decimal
    target -= 1;
    *target = '.';
    target += 1;

    float part = v - (float)whole;

    for (unsigned int i = 0; i < 5; ++i) {
        part = part * 10.0f; // Move up one
        while (part >= 10.0f) { part -= 10.0f; }
        whole = (unsigned int)part; // Get Digit
        //if (whole <= 9) {
            *target = '0' + whole; // Since it's a single digit
            target += 1;
        //}
    }
    *target = ' ';
    target += 1;

    return target;
}

char* Animation::WriteNewLine(char* target) {
    *target = '\n';
    target += 1;
    return target;
}

const char* Animation::IgnoreUntilNumber(const char* target) {
    char current = *target;

    while ((current < '0' || current > '9') && current != '-' && current != 0) {
        target += 1;
        current = *target;
    }

    return target;
}

const char* Animation::ReadInt(const char* target, int& v) {
    target = IgnoreUntilNumber(target);
    bool negative = *target == '-';
    if (negative) {
        target += 1;
    }

    v = 0;
    char current = *target;
    while (current >= '0' && current <= '9') {
        v = (v * 10) + (current - '0');
        target += 1;
        current = *target;
    }

    if (negative) {
        v = -v;
    }

    return target;
}

const char* Animation::ReadUInt(const char* target, unsigned int& v) {
    target = IgnoreUntilNumber(target);
    if (*target == '-') {
        target += 1;
    }
    
    v = 0;
    char current = *target;
    while (current >= '0' && current <= '9') {
        v = (v * 10) + (current - '0');
        target += 1;
        current = *target;
    }

    return target;
}

const char* Animation::ReadFloat(const char* target, float& f) {
    target = IgnoreUntilNumber(target);
    bool negative = *target == '-';
    if (negative) {
        target += 1;
    }

    unsigned int whole = 0;
    target = ReadUInt(target, whole);
    float part = 0.0f;

    float shift[] = { 0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f, 0.000001f };
    if (*target == '.') {
        target += 1;

        { // Start ReadUInt, but only 5 numbers.
            // First, make sure the next token is valid
            char current = *target;
            if (current >= '0' && current <= '9') {
                // Read up to 5 tokens
                for (unsigned int i = 0; i < 5; ++i) {
                    char current = *target;
                    // If token is valid, process it
                    if (current >= '0' && current <= '9') { 
                        part += shift[i] * (current - '0');
                        target += 1; // increment target
                    }
                    else { // otherwise break
                        break;
                    }
                }
                // We read in 5 tokens, if there are more numbers, ignore them
                current = *target;
                while (current >= '0' && current <= '9') {
                    target += 1;
                    current = *target;
                }
            }
        } // End ReadUInt, but only 5 numbers.
    }

    f = (float)whole + part;
    if (negative) {
        f = -f;
    }

    return target;
}

void* Animation::Allocate(unsigned int bytes) {
    char* result = new char[bytes];
    for (unsigned int i = 0; i < bytes; ++i) {
        result[i] = 0;
    }
    return result;
}

void Animation::Free(void* memory) {
    delete[] ((char*)memory);
}

bool Animation::FloatCompare(float a, float b) {
    float delta = a - b;
    return delta < 0.000001f && delta > -0.000001f;
}

float Animation::FastInvSqrt(float number) { // 1 / sqrt(number)
#if 0
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long*)&y;                       // evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);               // what the fuck?
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y));   // 1st iteration

    return y;
#else
    return 1.0f / sqrtf(number);
#endif
}

float Animation::FMod(float x, float y) {
    return fmodf(x, y);
}

#define ANIMHELPER_M4_3X3MINOR(c0, c1, c2, r0, r1, r2) \
    (in[c0 * 4 + r0] * (in[c1 * 4 + r1] * in[c2 * 4 + r2] - in[c1 * 4 + r2] * in[c2 * 4 + r1]) - \
     in[c1 * 4 + r0] * (in[c0 * 4 + r1] * in[c2 * 4 + r2] - in[c0 * 4 + r2] * in[c2 * 4 + r1]) + \
     in[c2 * 4 + r0] * (in[c0 * 4 + r1] * in[c1 * 4 + r2] - in[c0 * 4 + r2] * in[c1 * 4 + r1]))

void Animation::InvertMatrix(float* out, const float* in) {
    float determinant =   in[0] * ANIMHELPER_M4_3X3MINOR(1, 2, 3, 1, 2, 3)
                        - in[4] * ANIMHELPER_M4_3X3MINOR(0, 2, 3, 1, 2, 3)
                        + in[8] * ANIMHELPER_M4_3X3MINOR(0, 1, 3, 1, 2, 3)
                        - in[12] * ANIMHELPER_M4_3X3MINOR(0, 1, 2, 1, 2, 3);

    if (determinant == 0.0f) {
        for (int i = 0; i < 16; ++i) {
            out[i] = 0.0f;
        }
        out[0] = out[5] = out[10] = out[15] = 1.0f;
        return;
    }

    float cofactor[16] = { 0.0f };

    cofactor[0] = ANIMHELPER_M4_3X3MINOR(1, 2, 3, 1, 2, 3);
    cofactor[1] = -ANIMHELPER_M4_3X3MINOR(1, 2, 3, 0, 2, 3);
    cofactor[2] = ANIMHELPER_M4_3X3MINOR(1, 2, 3, 0, 1, 3);
    cofactor[3] = -ANIMHELPER_M4_3X3MINOR(1, 2, 3, 0, 1, 2);

    cofactor[4] = -ANIMHELPER_M4_3X3MINOR(0, 2, 3, 1, 2, 3);
    cofactor[5] = ANIMHELPER_M4_3X3MINOR(0, 2, 3, 0, 2, 3);
    cofactor[6] = -ANIMHELPER_M4_3X3MINOR(0, 2, 3, 0, 1, 3);
    cofactor[7] = ANIMHELPER_M4_3X3MINOR(0, 2, 3, 0, 1, 2);

    cofactor[8] = ANIMHELPER_M4_3X3MINOR(0, 1, 3, 1, 2, 3);
    cofactor[9] = -ANIMHELPER_M4_3X3MINOR(0, 1, 3, 0, 2, 3);
    cofactor[10] = ANIMHELPER_M4_3X3MINOR(0, 1, 3, 0, 1, 3);
    cofactor[11] = -ANIMHELPER_M4_3X3MINOR(0, 1, 3, 0, 1, 2);

    cofactor[12] = -ANIMHELPER_M4_3X3MINOR(0, 1, 2, 1, 2, 3);
    cofactor[13] = ANIMHELPER_M4_3X3MINOR(0, 1, 2, 0, 2, 3);
    cofactor[14] = -ANIMHELPER_M4_3X3MINOR(0, 1, 2, 0, 1, 3);
    cofactor[15] = ANIMHELPER_M4_3X3MINOR(0, 1, 2, 0, 1, 2);

    float adjugate[16] = {
        cofactor[0], cofactor[4], cofactor[8], cofactor[12],
        cofactor[1], cofactor[5], cofactor[9], cofactor[13],
        cofactor[2], cofactor[6], cofactor[10], cofactor[14],
        cofactor[3], cofactor[7], cofactor[11], cofactor[15]
    };

    float invDet = 1.0f / determinant;

    for (int i = 0; i < 16; ++i) {
        out[i] = adjugate[i] * invDet;
    }
}

void Animation::FastInvertMatrix(float* out, const float* in) {
    float result[16] = { 0.0f };

    result[0] = in[0];
    result[5] = in[5];
    result[10] = in[10];
    result[15] = in[15];

    result[3] = in[0];
    result[7] = in[5];
    result[11] = in[11];

    result[1] = in[4];
    result[4] = in[1];
    result[2] = in[8];
    result[8] = in[2];
    result[6] = in[9];
    result[9] = in[6];

    result[12] = -(in[12] * result[0] + in[13] + result[4] + in[14] * result[8]);
    result[13] = -(in[12] * result[1] + in[13] + result[5] + in[14] * result[9]);
    result[14] = -(in[12] * result[2] + in[13] + result[6] + in[14] * result[10]);

    for (int i = 0; i < 16; ++i) {
        out[i] = result[i];
    }
}


void Animation::ScaleMatrix(float* out, const float* in, float s) {
    float result[16] = { 0.0f };
   
    result[0] = in[0] * s;
    result[1] = in[1] * s;
    result[2] = in[2] * s;
    result[3] = in[3] * s;
    result[4] = in[4] * s;
    result[5] = in[5] * s;
    result[6] = in[6] * s;
    result[7] = in[7] * s;
    result[8] = in[8] * s;
    result[9] = in[9] * s;
    result[10] = in[10] * s;
    result[11] = in[11] * s;
    result[12] = in[12] * s;
    result[13] = in[13] * s;
    result[14] = in[14] * s;
    result[15] = in[15] * s;

    for (int i = 0; i < 16; ++i) {
        out[i] = result[i];
    }
}

void Animation::AddMatrices(float* out, const float* a, const float* b) {
    float result[16] = { 0.0f };
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
    result[3] = a[3] + b[3];
    result[4] = a[4] + b[4];
    result[5] = a[5] + b[5];
    result[6] = a[6] + b[6];
    result[7] = a[7] + b[7];
    result[8] = a[8] + b[8];
    result[9] = a[9] + b[9];
    result[10] = a[10] + b[10];
    result[11] = a[11] + b[11];
    result[12] = a[12] + b[12];
    result[13] = a[13] + b[13];
    result[14] = a[14] + b[14];
    result[15] = a[15] + b[15];

    for (int i = 0; i < 16; ++i) {
        out[i] = result[i];
    }
}

void Animation::MultiplyMat4Vec4(float* out, const float* mat, const float* vec) {
    float result[4] = { 0.0f };
    result[0] = vec[0] * mat[0] + vec[1] * mat[4] + vec[2] * mat[8] + vec[3] * mat[12];
    result[1] = vec[0] * mat[1] + vec[1] * mat[5] + vec[2] * mat[9] + vec[3] * mat[13];
    result[2] = vec[0] * mat[2] + vec[1] * mat[6] + vec[2] * mat[10] + vec[3] * mat[14];
    result[3] = vec[0] * mat[3] + vec[1] * mat[7] + vec[2] * mat[11] + vec[3] * mat[15];

    for (int i = 0; i < 4; ++i) {
        out[i] = result[i];
    }
}