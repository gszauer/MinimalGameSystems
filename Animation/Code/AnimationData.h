#ifndef _H_ANIMATION_DATA_
#define _H_ANIMATION_DATA_

namespace Animation {
    class State;

	class Data {
    public:
        static float StepLimit;
        enum class Component {
            Unset = 0,
            Position = 1,
            Rotation = 2,
            Scale = 4,
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
        // unsigned int: offset in frame data (assuming mFrameData is just an array of floats)
        // unsigned int: number of frames (assuming mFrameData is just an array of floats)
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

        void Set(const float* frameData, unsigned int frameSize, const unsigned int* trackData, unsigned int trackSize);
        void SetPointers(float* frameData, unsigned int frameSize, unsigned int* trackData, unsigned int trackSize);

        const float* GetFrameData() const;
        unsigned int FrameDataSize() const;
        
        const unsigned int* GetTrackData() const;
        unsigned int TrackDataSize() const;
        unsigned int GetNumTracks() const;

        float GetStartTime() const;
        float GetEndtime() const;
        float GetDuration() const;
        bool IsValid() const;

        const char* GetLabel() const;
        void SetLabel(const char* label);

        float Sample(State& out, float time, bool looping) const;
        float SampleTrack(float* out, unsigned int trackIndex, float time, bool looping) const;
    };
}

#endif