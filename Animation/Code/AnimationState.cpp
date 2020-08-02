#include "AnimationState.h"
#include "AnimationHelpers.h"

Animation::State::State() {
    mTransforms = 0;
    mHierarchy = 0;
    mSize = 0;
}

Animation::State::State(const Animation::State& other) {
    mTransforms = 0;
    mHierarchy = 0;
    mSize = 0;

    *this = other;
}

Animation::State& Animation::State::operator=(const Animation::State& other) {
    if (this == &other) {
        return *this;
    }

    Resize(other.Size());
    for (unsigned int i = 0; i < mSize; ++i) {
        mHierarchy[i] = other.mHierarchy[i];
    }

    for (unsigned int i = 0; i < mSize * 10; ++i) {
        mTransforms[i] = other.mTransforms[i];
    }


    return *this;
}

Animation::State::~State() {
    Resize(0);
}

unsigned int Animation::State::Size() const {
    return mSize;
}

void Animation::State::Resize(unsigned int size) {
    if (mSize == size && mTransforms != 0 && mHierarchy != 0) {
        return;
    }

    if (mTransforms != 0) {
        Animation::Free(mTransforms);
        mTransforms = 0;
    }
    if (mHierarchy != 0) {
        Animation::Free(mHierarchy);
        mHierarchy = 0;
    }

    if (size == 0) {
        mHierarchy = 0;
        mTransforms = 0;
    }
    else {
        mTransforms = (float*)Animation::Allocate(sizeof(float) * size * 10);
        mHierarchy = (int*)Animation::Allocate(sizeof(int) * size);
    }
    mSize = size;
}

bool Animation::State::ToMatrixPalette(float* outArray, unsigned int arraySize) const {
    if ((arraySize == 0) || (outArray == 0) || (mSize * 16 < arraySize)) {
        return false;
    }

    float position[3] = { 0.0f };
    float rotation[4] = { 0.0f };
    float scale[3] = { 0.0f };

    unsigned int i = 0;
    for (unsigned int size = mSize; i < size; ++i) {
        int parent = mHierarchy[i]; // TODO: Fast path is untested because of bad model. Export an optimized / re-ordered data set and try again with that.
        if (parent > ((int)i)) {
            break;
        }

        GetRelativePosition(i, position);
        GetRelativeRotation(i, rotation);
        GetRelativeScale(i, scale);
        Animation::Internal::TransformToMatrix(&outArray[i * 16], position, rotation, scale);
        if (parent >= 0) {
            Animation::Internal::MultiplyMatrices(&outArray[i * 16], &outArray[(unsigned int)parent * 16], &outArray[i * 16]);
        }
    }

    for (unsigned int size = mSize; i < size; ++i) {
        GetAbsoluteTransform(i, position, rotation, scale);
        Animation::Internal::TransformToMatrix(&outArray[i * 16], position, rotation, scale);
    }

    return true;
}

int Animation::State::GetParent(unsigned int index) const {
    return mHierarchy[index];
}

void Animation::State::SetParent(unsigned int index, int parent) {
    mHierarchy[index] = parent;
}

void Animation::State::GetRelativePosition(unsigned int index, float* outVec3) const {
    float* position = &mTransforms[index * 10];
    outVec3[0] = position[0];
    outVec3[1] = position[1];
    outVec3[2] = position[2];
}

void Animation::State::GetRelativeRotation(unsigned int index, float* outQuat) const {
    float* rotation = &mTransforms[index * 10 + 3];
    outQuat[0] = rotation[0];
    outQuat[1] = rotation[1];
    outQuat[2] = rotation[2];
    outQuat[3] = rotation[3];
}

void Animation::State::GetRelativeScale(unsigned int index, float* outVec3) const {
    float* scale = &mTransforms[index * 10 + 7];
    outVec3[0] = scale[0];
    outVec3[1] = scale[1];
    outVec3[2] = scale[2];
}

void Animation::State::SetRelativeScale(unsigned int index, const float* scl) {
    float* scale = &mTransforms[index * 10 + 7];
    scale[0] = scl[0];
    scale[1] = scl[1];
    scale[2] = scl[2];
}

void Animation::State::SetRelativePosition(unsigned int index, const float* pos) {
    float* position = &mTransforms[index * 10];
    position[0] = pos[0];
    position[1] = pos[1];
    position[2] = pos[2];
}

void Animation::State::SetRelativeRotation(unsigned int index, const float* rot) {
    float* rotation = &mTransforms[index * 10 + 3];
    rotation[0] = rot[0];
    rotation[1] = rot[1];
    rotation[2] = rot[2];
    rotation[3] = rot[3];

    float rotLenSq = rot[0] * rot[0] + rot[1] * rot[1] + rot[2] * rot[2] + rot[3] * rot[3];
    if (!Animation::FloatCompare(rotLenSq, 1.0f)) {
        if (rotLenSq > 0.0f) {
            float invRotLen = Animation::InvSqrt(rotLenSq);
            rotation[0] *= rotLenSq;
            rotation[1] *= rotLenSq;
            rotation[2] *= rotLenSq;
            rotation[3] *= rotLenSq;
        }
    }
}

void Animation::State::GetRelativeTransform(unsigned int index, float* outPos, float* outRot, float* outScl) const {
    float* position = &mTransforms[index * 10];
    float* rotation = &mTransforms[index * 10 + 3];
    float* scale = &mTransforms[index * 10 + 7];
    
    outPos[0] = position[0];
    outPos[1] = position[1];
    outPos[2] = position[2];

    outRot[0] = rotation[0];
    outRot[1] = rotation[1];
    outRot[2] = rotation[2];
    outRot[3] = rotation[3];

    outScl[0] = scale[0];
    outScl[1] = scale[1];
    outScl[2] = scale[2];
}

void Animation::State::GetAbsoluteTransform(unsigned int index, float* outPos, float* outRot, float* outScl) const {
    float parentPos[3], parentRot[4], parentScl[3];

    GetRelativePosition(index, outPos);
    GetRelativeRotation(index, outRot);
    GetRelativeScale(index, outScl);

    for (int parent = GetParent(index); parent >= 0; parent = GetParent(parent)) {
        GetRelativePosition((unsigned int)parent, parentPos);
        GetRelativeRotation((unsigned int)parent, parentRot);
        GetRelativeScale((unsigned int)parent, parentScl);

        Animation::Internal::CombineTransforms(
            outPos, outRot, outScl,
            parentPos, parentRot, parentScl,
            outPos, outRot, outScl
        );
    }

    // It might be a good idea to normalize the resulting rotation here, depending on the depth of the 
    // skeletons hierarchy
}

void Animation::State::GetAbsolutePosition(unsigned int index, float* outVec3) const {
    float rotation[4];
    float scale[3];

    GetAbsoluteTransform(index, outVec3, rotation, scale);
}

void Animation::State::GetAbsoluteRotation(unsigned int index, float* outQuat) const {
    float position[3];
    float scale[3];

    GetAbsoluteTransform(index, position, outQuat, scale);
}

void Animation::State::GetAbsoluteScale(unsigned int index, float* outVec3) const {
    float position[3];
    float rotation[4];

    GetAbsoluteTransform(index, position, rotation, outVec3);
}

unsigned int Animation::State::SerializedStringLength() const {
    unsigned int space = 1;
    unsigned int lineBreak = 1;

    unsigned int stringLength = StringLengthUInt(mSize) + space + lineBreak;

    for (unsigned int i = 0; i < mSize; ++i) {
        stringLength += StringLengthInt(mHierarchy[i]) + space;
    }
    stringLength += lineBreak;

    for (unsigned int i = 0; i < mSize; ++i) {
        stringLength += StringLengthFloat(mTransforms[i * 10 + 0]) + space;
        stringLength += StringLengthFloat(mTransforms[i * 10 + 1]) + space;
        stringLength += StringLengthFloat(mTransforms[i * 10 + 2]) + space;
        stringLength += StringLengthFloat(mTransforms[i * 10 + 3]) + space;
        stringLength += StringLengthFloat(mTransforms[i * 10 + 4]) + space;
        stringLength += StringLengthFloat(mTransforms[i * 10 + 5]) + space;
        stringLength += StringLengthFloat(mTransforms[i * 10 + 6]) + space;
        stringLength += StringLengthFloat(mTransforms[i * 10 + 7]) + space;
        stringLength += StringLengthFloat(mTransforms[i * 10 + 8]) + space;
        stringLength += StringLengthFloat(mTransforms[i * 10 + 9]) + space;
    }
    stringLength += lineBreak;

    stringLength += 1; // null terminator

    return stringLength;
}

void Animation::State::SerializeToString(char* output) const {
    output = WriteUInt(output, mSize);

    for (unsigned int i = 0; i < mSize; ++i) {
        output = WriteInt(output, mHierarchy[i]);
    }
    output = WriteNewLine(output);

    for (unsigned int i = 0; i < mSize; ++i) {
        output = WriteFloat(output, mTransforms[i * 10 + 0]);
        output = WriteFloat(output, mTransforms[i * 10 + 1]);
        output = WriteFloat(output, mTransforms[i * 10 + 2]);
        output = WriteFloat(output, mTransforms[i * 10 + 3]);
        output = WriteFloat(output, mTransforms[i * 10 + 4]);
        output = WriteFloat(output, mTransforms[i * 10 + 5]);
        output = WriteFloat(output, mTransforms[i * 10 + 6]);
        output = WriteFloat(output, mTransforms[i * 10 + 7]);
        output = WriteFloat(output, mTransforms[i * 10 + 8]);
        output = WriteFloat(output, mTransforms[i * 10 + 9]);
    }
    output = WriteNewLine(output);
    *output = '\0';
}

void Animation::State::DeSerializeFromString(const char* input) {
    input = ReadUInt(input, mSize);
    Resize(mSize);

    for (unsigned int i = 0; i < mSize; ++i) {
        input = ReadInt(input, mHierarchy[i]);
    }

    for (unsigned int i = 0; i < mSize; ++i) {
        input = ReadFloat(input, mTransforms[i * 10 + 0]);
        input = ReadFloat(input, mTransforms[i * 10 + 1]);
        input = ReadFloat(input, mTransforms[i * 10 + 2]);
        input = ReadFloat(input, mTransforms[i * 10 + 3]);
        input = ReadFloat(input, mTransforms[i * 10 + 4]);
        input = ReadFloat(input, mTransforms[i * 10 + 5]);
        input = ReadFloat(input, mTransforms[i * 10 + 6]);
        input = ReadFloat(input, mTransforms[i * 10 + 7]);
        input = ReadFloat(input, mTransforms[i * 10 + 8]);
        input = ReadFloat(input, mTransforms[i * 10 + 9]);

        float* rot = &mTransforms[i * 10 + 3];
        float rotLenSq = rot[0] * rot[0] + rot[1] * rot[1] + rot[2] * rot[2] + rot[3] * rot[3];
        if (!Animation::FloatCompare(rotLenSq, 1.0f)) {
            if (rotLenSq > 0.0f) {
                float invRotLen = Animation::InvSqrt(rotLenSq);
                mTransforms[i * 10 + 3] *= rotLenSq;
                mTransforms[i * 10 + 4] *= rotLenSq;
                mTransforms[i * 10 + 5] *= rotLenSq;
                mTransforms[i * 10 + 6] *= rotLenSq;
            }
        }
    }

    // Ignore null terminator
}