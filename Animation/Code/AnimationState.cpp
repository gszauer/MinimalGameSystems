#include "AnimationState.h"
#include "AnimationInternal.h"

namespace Animation {
    namespace Internal {
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
        Animation::Internal::Free(mTransforms);
        mTransforms = 0;
    }
    if (mHierarchy != 0) {
        Animation::Internal::Free(mHierarchy);
        mHierarchy = 0;
    }

    if (size == 0) {
        mHierarchy = 0;
        mTransforms = 0;
    }
    else {
        mTransforms = (float*)Animation::Internal::Allocate(sizeof(float) * size * 10);
        mHierarchy = (int*)Animation::Internal::Allocate(sizeof(int) * size);
    }
    mSize = size;
}

void Animation::ToMatrixPalette(float* outArray, const State& state) {
    float position[3] = { 0.0f };
    float rotation[4] = { 0.0f };
    float scale[3] = { 0.0f };

    unsigned int i = 0;
    for (unsigned int size = state.Size(); i < size; ++i) {
        int parent = state.GetParent(i); // TODO: Fast path is untested because of bad model. Export an optimized / re-ordered data set and try again with that.
        if (parent > ((int)i)) {
            break;
        }

        state.GetRelativePosition(i, position);
        state.GetRelativeRotation(i, rotation);
        state.GetRelativeScale(i, scale);
        Animation::Internal::TransformToMatrix(&outArray[i * 16], position, rotation, scale);
        if (parent >= 0) {

            // multiply matrices
            const float* a = &outArray[(unsigned int)parent * 16];
            const float* b = &outArray[i * 16];
            float* out = &outArray[i * 16];
            {
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

                for (unsigned int j = 0; j < 16; ++j) {
                    out[j] = result[j];
                }
            }
        }
    }

    for (unsigned int size = state.Size(); i < size; ++i) {
        state.GetAbsoluteTransform(i, position, rotation, scale);
        Animation::Internal::TransformToMatrix(&outArray[i * 16], position, rotation, scale);
    }
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
    if (!Animation::Internal::FloatCompare(rotLenSq, 1.0f)) {
        if (rotLenSq > 0.0f) {
            float invRotLen = Animation::Internal::InvSqrt(rotLenSq);
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

        // Combine the two transforms, store result in out
        {
            float resultScale[3] = { 1.0f };
            float resultPos[3] = { 0.0f };
            float resultRot[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

            // vec * vec
            resultScale[0] = parentScl[0] * outScl[0];
            resultScale[1] = parentScl[1] * outScl[1];
            resultScale[2] = parentScl[2] * outScl[2];

            // quat * quat
            resultRot[0] = parentRot[0] * outRot[3] + parentRot[1] * outRot[2] - parentRot[2] * outRot[1] + parentRot[3] * outRot[0];
            resultRot[1] = -parentRot[0] * outRot[2] + parentRot[1] * outRot[3] + parentRot[2] * outRot[0] + parentRot[3] * outRot[1];
            resultRot[2] = parentRot[0] * outRot[1] - parentRot[1] * outRot[0] + parentRot[2] * outRot[3] + parentRot[3] * outRot[2];
            resultRot[3] = -parentRot[0] * outRot[0] - parentRot[1] * outRot[1] - parentRot[2] * outRot[2] + parentRot[3] * outRot[3];

            // vec * vec
            resultPos[0] = parentScl[0] * outPos[0];
            resultPos[1] = parentScl[1] * outPos[1];
            resultPos[2] = parentScl[2] * outPos[2];

            // quat * vec
            float v[3] = { resultPos[0], resultPos[1], resultPos[2] };
            float d1 = parentRot[0] * v[0] + parentRot[1] * v[1] + parentRot[2] * v[2];
            float d2 = parentRot[0] * parentRot[0] + parentRot[1] * parentRot[1] + parentRot[2] * parentRot[2];

            resultPos[0] = (parentRot[0] * 2.0f * d1) + (v[0] * (parentRot[3] * parentRot[3] - d2)) + ((parentRot[1] * v[2] - parentRot[2] * v[1]) * 2.0f * parentRot[3]);
            resultPos[1] = (parentRot[1] * 2.0f * d1) + (v[1] * (parentRot[3] * parentRot[3] - d2)) + ((parentRot[2] * v[0] - parentRot[0] * v[2]) * 2.0f * parentRot[3]);
            resultPos[2] = (parentRot[2] * 2.0f * d1) + (v[2] * (parentRot[3] * parentRot[3] - d2)) + ((parentRot[0] * v[1] - parentRot[1] * v[0]) * 2.0f * parentRot[3]);

            // vec + vec
            resultPos[0] = parentPos[0] + resultPos[0];
            resultPos[1] = parentPos[1] + resultPos[1];
            resultPos[2] = parentPos[2] + resultPos[2];

            // Copy data out
            for (int i = 0; i < 3; ++i) {
                outScl[i] = resultScale[i];
                outPos[i] = resultPos[i];
                outRot[i] = resultRot[i];
            }
            outRot[3] = resultRot[3];
        }
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