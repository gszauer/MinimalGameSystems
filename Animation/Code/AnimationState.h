#ifndef _H_ANIMATION_STATE_
#define _H_ANIMATION_STATE_

namespace Animation {
    class State {
    protected:
        // Each transform in the mTransforms array has the following stride of 10 floats: position[3], rotation[4], scale[3]
        float* mTransforms;
        int* mHierarchy;
        unsigned int mSize;
    public:
        State();
        State(const State& other);
        State& operator=(const State& other);
        ~State();

        unsigned int Size() const;
        void Resize(unsigned int size);

        int GetParent(unsigned int index) const;
        void SetParent(unsigned int index, int parent);

        void GetRelativePosition(unsigned int index, float* outVec3) const;
        void GetRelativeRotation(unsigned int index, float* outQuat) const;
        void GetRelativeScale(unsigned int index, float* outVec3) const;

        void SetRelativePosition(unsigned int index, const float* pos);
        void SetRelativeRotation(unsigned int index, const float* rot);
        void SetRelativeScale(unsigned int index, const float* scl);

        void GetAbsolutePosition(unsigned int index, float* outVec3) const;
        void GetAbsoluteRotation(unsigned int index, float* outQuat) const;
        void GetAbsoluteScale(unsigned int index, float* outVec3) const;

        void GetRelativeTransform(unsigned int index, float* outPos, float* outRot, float* outScl) const;
        void GetAbsoluteTransform(unsigned int index, float* outPos, float* outRot, float* outScl) const;

        void SerializeToString(char* output) const;
        void DeSerializeFromString(const char* input);
        unsigned int SerializedStringLength() const;
    };

    void ToMatrixPalette(float* outArray, const State& state);
}

#endif