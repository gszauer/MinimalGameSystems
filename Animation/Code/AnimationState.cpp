#include "AnimationState.h"

#if 0
#include <ostream>
std::ostream& operator<<(std::ostream& os, const Animation::State& state) {
    unsigned int size = state.Size();

    os << "Number of nodes in scene graph: " << size << "\n";
    os << "Parent child relationships: " << "\n";
    for (unsigned int i = 0; i < size; ++i) {
        os << "\tParent: " << state.GetParent(i) << ", Child: " << i << "\n";
    }

    unsigned int trackStride = 10;
    os << "Local transforms: \n";
    for (unsigned int i = 0; i < size; ++i) {
        float position[3], rotation[4], scale[3] = { 0 };
        state.GetRelativeTransform(i, position, rotation, scale);

        os << "\t" << i << ": position {";
        os << position[0] << ", ";
        os << position[1] << ", ";
        os << position[2] << "}, rotation {";
        os << rotation[0] << ", ";
        os << rotation[1] << ", ";
        os << rotation[2] << ", ";
        os << rotation[3] << "}, scale {";
        os << scale[0] << ", ";
        os << scale[1] << ", ";
        os << scale[2] << "}\n";
    }

    os << "Global transforms: \n";
    for (unsigned int i = 0; i < size; ++i) {
        float position[3], rotation[4], scale[3] = { 0 };
        state.GetAbsoluteTransform(i, position, rotation, scale);

        os << "\t" << i << ": position {";
        os << position[0] << ", ";
        os << position[1] << ", ";
        os << position[2] << "}, rotation {";
        os << rotation[0] << ", ";
        os << rotation[1] << ", ";
        os << rotation[2] << ", ";
        os << rotation[3] << "}, scale {";
        os << scale[0] << ", ";
        os << scale[1] << ", ";
        os << scale[2] << "}\n";
    }
}
#endif

namespace Animation {
    static void CombineTransforms(float* outPos, float* outRot, float* outScale, const float* posA, const float* rotA, const float* sclA, const float* posB, const float* rotB, const float* sclB) {
        // vec * vec
        outScale[0] = sclA[0] * sclB[0];
        outScale[1] = sclA[1] * sclB[1];
        outScale[2] = sclA[2] * sclB[2];

        // quat * quat
        outRot[0] = rotB[0] * rotA[3] + rotB[1] * rotA[2] - rotB[2] * rotA[1] + rotB[3] * rotA[0];
        outRot[1] = -rotB[0] * rotA[2] + rotB[1] * rotA[3] + rotB[2] * rotA[0] + rotB[3] * rotA[1];
        outRot[2] = rotB[0] * rotA[1] - rotB[1] * rotA[0] + rotB[2] * rotA[3] + rotB[3] * rotA[2];
        outRot[3] = -rotB[0] * rotA[0] - rotB[1] * rotA[1] - rotB[2] * rotA[2] + rotB[3] * rotA[3];

        // vec * vec
        outPos[0] = sclA[0] * posB[0];
        outPos[1] = sclA[1] * posB[1];
        outPos[2] = sclA[2] * posB[2];

        // quat * vec
        float v[3] = { outPos[0], outPos[1], outPos[2] };
        float d1 = rotA[0] * v[0] + rotA[1] * v[1] + rotA[2] * v[2];
        float d2 = rotA[0] * rotA[0] + rotA[1] * rotA[1] + rotA[2] * rotA[2];

        outPos[0] = (rotA[0] * 2.0f * d1) + (v[0] * (rotA[3] * rotA[3] - d2)) + ((rotA[1] * v[2] - rotA[2] * v[1]) * 2.0f * rotA[3]);
        outPos[1] = (rotA[1] * 2.0f * d1) + (v[1] * (rotA[3] * rotA[3] - d2)) + ((rotA[2] * v[0] - rotA[0] * v[2]) * 2.0f * rotA[3]);
        outPos[2] = (rotA[2] * 2.0f * d1) + (v[2] * (rotA[3] * rotA[3] - d2)) + ((rotA[0] * v[1] - rotA[1] * v[0]) * 2.0f * rotA[3]);

        // vec + vec
        outPos[0] = posA[0] + outPos[0];
        outPos[1] = posA[1] + outPos[1];
        outPos[2] = posA[2] + outPos[2];
    }

    static void TransformToMatrix(float* outMatrix, const float* position, const float* rot, const float* scale) {
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

    static void MultiplyMatrices(float* out, const float* a, const float* b) {
        // Column 0
        out[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
        out[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
        out[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
        out[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];

        // Column 1
        out[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
        out[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
        out[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
        out[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];

        // Column 2
        out[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
        out[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
        out[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
        out[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];

        // Column 3
        out[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
        out[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
        out[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
        out[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
    }
}

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
        mTransforms[i] = other.mTransforms[i];
        mHierarchy[i] = other.mHierarchy[i];
    }

    return *this;
}

Animation::State::~State() {
    if (mTransforms != 0) {
        delete[] mTransforms;
    }
    if (mHierarchy != 0) {
        delete[] mHierarchy;
    }
}

unsigned int Animation::State::Size() const {
    return mSize;
}

void Animation::State::Resize(unsigned int size) {
    if (size == 0) {
        if (mTransforms != 0) {
            delete[] mTransforms;
        }
        if (mHierarchy != 0) {
            delete[] mHierarchy;
        }
        mHierarchy = 0;
        mTransforms = 0;
    }
    else {
        if (mTransforms != 0) {
            delete[] mTransforms;
        }
        if (mHierarchy != 0) {
            delete[] mHierarchy;
        }
        mTransforms = new float[size];
        mHierarchy = new int[size];
    }
    mSize = size;
}

bool Animation::State::ToMatrixPalette(float* outArray, unsigned int arraySize) const {
    if ((arraySize == 0) || (outArray == 0) || (mSize * 16 < arraySize)) {
        return false;
    }

    float position[3];
    float rotation[4];
    float scale[3];

    int i = 0;
    for (int size = (int)mSize; i < size; ++i) {
        int parent = mHierarchy[i];
        if (parent > i) {
            break;
        }

        GetRelativePosition((unsigned int)i, position);
        GetRelativeRotation((unsigned int)i, rotation);
        GetRelativeScale((unsigned int)i, scale);
        TransformToMatrix(&outArray[(unsigned int)i * 16], position, rotation, scale);
        if (parent >= 0) {
            MultiplyMatrices(&outArray[(unsigned int)i * 16], &outArray[(unsigned int)parent * 16], &outArray[(unsigned int)i * 16]);
        }
    }

    for (int size = (int)mSize; i < size; ++i) {
        GetAbsoluteTransform((unsigned int)i, position, rotation, scale);
        TransformToMatrix(&outArray[(unsigned int)i * 16], position, rotation, scale);
    }

    return true;
}

int Animation::State::GetParent(unsigned int index) const {
    return mHierarchy[index];
}

void Animation::State::SetParent(unsigned int index, int parent) {
    mHierarchy[index] = parent;
}

const float* Animation::State::GetRelativePosition(unsigned int index) const {
    return &mTransforms[index * 10];
}

const float* Animation::State::GetRelativeRotation(unsigned int index) const {
    return &mTransforms[index * 10 + 3];
}

const float* Animation::State::GetRelativeScale(unsigned int index) const {
    return &mTransforms[index * 10 + 7];
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
    float parentPos[3], parentRot[3], parentScl[3];

    GetRelativePosition(index, outPos);
    GetRelativeRotation(index, outRot);
    GetRelativeScale(index, outScl);

    for (int parent = GetParent(index); parent >= 0; parent = GetParent(parent)) {
        GetRelativePosition((unsigned int)parent, parentPos);
        GetRelativeRotation((unsigned int)parent, parentRot);
        GetRelativeScale((unsigned int)parent, parentScl);

        CombineTransforms(
            outPos, outRot, outScl,
            parentPos, parentRot, parentScl,
            outPos, outRot, outScl
        );
    }
}

void Animation::State::GetAbsolutePosition(unsigned int index, float* outVec3) const {
    float position[3];
    float rotation[4];
    float scale[3];

    GetAbsoluteTransform(index, outVec3, rotation, scale);
}

void Animation::State::GetAbsoluteRotation(unsigned int index, float* outQuat) const {
    float position[3];
    float rotation[4];
    float scale[3];

    GetAbsoluteTransform(index, position, outQuat, scale);
}

void Animation::State::GetAbsoluteScale(unsigned int index, float* outVec3) const {
    float position[3];
    float rotation[4];
    float scale[3];

    GetAbsoluteTransform(index, position, rotation, outVec3);
}

unsigned int Animation::State::SerializedSize() const {
    unsigned int transform = sizeof(float) * 10 * mSize;
    unsigned int hierarchy = sizeof(int) * mSize;
    unsigned int size = sizeof(unsigned int);
    unsigned int header = sizeof(char) * 4;

    return transform + hierarchy + size + header;
}

unsigned int Animation::State::Serialize(char* output, unsigned int outputSize) {
    output[0] = 'A';
    output[1] = 'N';
    output[2] = 'I';
    output[3] = 'M';

    // TODO

    return 0;
}

void Animation::State::DeSerialize(char* input, unsigned int inputSize) {
    // TODO
}