#ifndef _H_ANIMATION_DATA_
#define _H_ANIMATION_DATA_

namespace Animation {
    class State;

	class Data {
    public:
        enum class Component {
            Unset = 0,
            Position = 1,
            Rotation = 2,
            Scale = 4,
        };
        struct TrackID {
            unsigned int index;
        };
    protected:
        // Frame: x is 3 or 4 depending on component type (only support pos, rot and scale)
        // float time
        // float in[x]
        // float value[x]
        // float out[x]
        float* mFrameData;
        unsigned int mFrameDataSize; // Number of items in mFrameData array 

        // Each track data contains
        // unsigned int: id
        // unsigned int: component (position, rotation, scale)
        // unsigned int: offset in frame data
        // unsigned int: number of frames
        unsigned int* mTrackData;
        unsigned int mTrackDataSize;

        float mStartTime;
        float mEndTime;

        char* mLabel;
    public:
        Data();
        Data(const Data& other);
        Data& operator=(const Data& other);
        ~Data();

#if 0 // TODO: This is a more user friendly representation. Be sure to implement and test this path as well
        void Resize(unsigned int numTracks, unsigned int numFrames);
#endif
        void SetRawData(const float* frameData, unsigned int frameSize, const unsigned int* trackData, unsigned int trackSize);

        const float* GetFrameData() const;
        unsigned int FrameDataSize() const;
        
        const unsigned int* GetTrackData() const;
        unsigned int TrackDataSize() const;

        float GetStartTime() const;
        float GetEndtime() const;
        float GetDuration() const;
        bool IsValid() const;

        const char* GetLabel() const;
        void SetLabel(const char* label);

        float Sample(State& out, float time, bool looping) const;

        void SerializeToString(char* output) const;
        void DeSerializeFromString(const char* input);
        unsigned int SerializedStringLength() const;
    };
}

#endif