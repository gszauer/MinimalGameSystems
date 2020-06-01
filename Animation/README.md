# Animation

This library is intended to be a simple, easy to use animation library that's easy to integrate into any project. The code does not provide anything outside of animation, there is no file loading or dependancy on any external libraries, etc... There is no dependancy on any of the standard C headers. There are a few instances of ```new``` and ```delete```, but they should be easy to replace if needed.

The goal of this library is simplicity, to provide a minimal surface API for animation. The code here aims to be easy and intuitive by providing a minimal API to interact with. For basic animations, you only need two class, ```Animation::State``` and ```Animation::Data```. Aside from the core classes, utilities for skinning and animation blending are also provided.

The readability of the logic for many of these functions suffers from trying to keep the API as small as possible. For an in depth guide to animation programming that covers all of the techniques implemented here check out my book [Hands On Game Animation Programming](TODO). The following files are provided

* ```AnimationData[.h, .cpp]``` - **required** - Contains ```Animation::Data```, analogous to an animation clip.
* ```AnimationState[.h, .cpp]``` - **required** - Contains ```Aniamtion::State```, analogous to an animated pose. 
* ```AnimationBlend[.h, .cpp]``` - **optional** - Contains code to blend animations smoothly.
* ```AnimationSkin[.h, .cpp]``` - **optional** - Contains code to skin meshes using ```Animation::State```.

# State And Data

This library breaks animation down into two key concepts, **animation state** and **animation data**. Data is static, it does not change. State is driven by data, it's volatile and changes. 

Animation data is analogous to an animation clip. It contains frames, the frames make tracks and the tracks move components of transforms. 

Animation state is analogous to the Pose of an animation. An animation clip is typically sampled into a pose. Animation state is that, it holds the same information as a pose.

Troughout the rest of this document, animation data will be used interchangably with animation clip and animation state will be used interchangably with pose.

## Animation::Data

In a more straight forward animation system, animation data is represented by several classes. In my [book](TODO) I break this down into the following classes:

* __[Frame](TODO)__ TODO: Describe
* __[Track](TODO)__ TODO: Describe
* __[TransformTrack](TODO)__ TODO: Describe
* __[Clip](TODO)__ TODO: Describe

In this animation system, all of that data still exists, it's just all packed into one compact class. The ```Animation::Data``` class contains the following members:


```
class Data {
    protected:
        float* mFrameData;
        unsigned int mFrameDataSize;

        unsigned int* mTrackData;
        unsigned int mTrackDataSize;

        float mStartTime;
        float mEndTime;

        char* mLabel;
```

The ```mFrameData``` variable contains all of the frames that are inside of the animation clip. The ```mFrameDataSize``` variable contains the number of elements are inside the array. All of the frames for one track are laid out linearly. This makes the ```mFrameData``` array segmented on a high level like this:

```
+----------------------+----------------------------------+----------------------------+
| Track 1 (24 frames)  | Track 2 (48 frames)              | Track 3 (18 frames)| etc.. |
+----------------------+----------------------------------+----------------------------+
```

An animation track usually drives every component of a vector, not just a single one. To make a track work with higher order data, animation systems often have different classes like ```ScalarTrack```, ```VectorTrack```, ```QuaternionTrack```. In the [Hands On Game Animation Programming](TODO) book i chose to template the __[Track](TODO)__ class.

This is important, because the size of a single frame (in bytes) changes based on how much data a frame represents. Consider the following frame structures

```
struct ScalarFrame {
    float time;
    float in;
    float value;
    float out;
}

struct Vec3Frame {
    float time;
    vec3 in;
    vec3 value;
    vec3 out;
}

// The vec3 frame could be expressed as:
struct Float3Frame {
    float time;
    float in[3];
    float value[3];
    float out[3];
}
```

This makes the diagram of how ```mFrameData``` is segmented harder to read. Two segments both containing 5 frames could have a different size (in bytes) if one segment could hold a vector track while another segment could hold a quaternion track. 



```
+----------------------+----------------------------------+----------------------------+
| Track 1 (24 frames)  | Track 2 (48 frames)              | Track 3 (18 frames)| etc.. |
+----------------------+----------------------------------+----------------------------+
  |
  |   +----------------------------------+
  |   | Assuming Track 1 is a vec3 track |
  |   +-------------------------------+--+----------------------------+----------------+
  +-> | time, in[3], value[3], out[3] | time, in[3], value[3], out[3] | time, in[3] ...|
      +-------------------------------+-------------------------------+----------------+

```


There is no context for where one track begins and the other ends. There is also no context for where a frame in this array might be located. The ```mFrameData``` array is just a large chunk of memory. The ```mTrackData``` array provides context for interpreting the ```mFrameData``` array.

The ```mTrackData``` contains information needed to tell where a track begins, how many frames the track has and how large each element of a frame is. Even tough it's stored as an array of unsigned integers, ```mTrackData``` should be interpreted as if it consisted of this struct:

```
struct Track {
    unsigned int id;
    unsigned int component;
    unsigned int offset;
    unsigned int size;
};
```

So, each "Track" in ```mTrackData``` consists of 4 unsigned integers. Another way to think about it is that the  ```mTrackData``` array has a stride of 4. Here is a breakdown of what each integer represetns.

* ```id``` TODO
* ```component``` TODO
* ```offset``` TODO
* ```size``` TODO

The ```mLabel``` member of the ```Animation::Data``` class is an optional, null terminated string. Typically you will store the name of the animation in this string. This variable isn't used by the animation system, it can be used for anything, like a user data pointer.

The start and end time of an animation clip depend on the tracks. The track that starts with the lowest time dictates the start time of the clip. Finding the correct start and end times involve looping trough all the tracks in the clip, to avoid doing this the times are cached in ```mStartTime``` and ```mEndTime``` whenever the animation data is set.

### Interpreting Animation::Data, sample

The following code prints out the ```Animation::Data``` class. It's not valid code, but it should serve to demonstrate how to evaluate the contents of the ```Data``` class.


```
void Print(const Animation::Data& data) {
    unsigned int trackStride = 4;
    unsigned int numTracks = data.mTrackDataSize / trackStride;

    std::cout << "Animation: " << data.mLabel << "\n";
    std::cout << "\tNumber of tracks: " << numTracks << "\n";
    std::cout << "\tStart time: " << mStartTime << "\n";
    std::cout << "\tEnd time: " << mEndTime << "\n";
    std::cout << "\tTracks:\n";

    // Loop trough all tracks in the animation clip
    for (unsigned int t = 0; t < numTracks; ++t) {
        std::cout << "\tTrack " << t << ":\n";

        // Current track data
        unsigned int trackId = data.mTrackData[t * trackStride + 0];
        unsigned int trackComponent = data.mTrackData[t * trackStride + 1];
        unsigned int trackOffset = data.mTrackData[t * trackStride + 2];
        unsigned int trackSize = data.mTrackData[t * trackStride + 3];

        std::cout << "\t\tTarget: " << trackId << "\n";
        std::cout << "\t\tComponent: ";
        if (trackComponent == COMPONENT_POSITION) {
            std::cout << "position\n";
        }
        else if (trackComponent == COMPONENT_ROTATION) {
            std::cout << "rotation\n";
        }
        else {
            std::cout << "scale\n";
        }
        std::cout << "\t\tOffset: " << trackOffset << "\n";
        std::cout << "\t\tSize: " << trackSize << "\n";
        
        unsigned int frameStride = 3;
        if (trackComponent = COMPONENT_ROTATION) {
            frameStride = 4;
        }
        
        unsigned int numFrames = trackSize / frameStride;
        std::cout << "\t\tNumber of frames:" << numFrames << "\n";
        std::cout << "\t\tFrames:\n";

        // Loop trough all frames in current track
        for (unsigned int f = 0; f < numFrames; ++f) {
            int frameIndex = trackOffset + f * frameStride;

            // Current frame data
            float frameTime = data.mFrameData[frameIndex++];
            float frameIn[4] = {
                data.mFrameData[frameIndex++],
                data.mFrameData[frameIndex++],
                data.mFrameData[frameIndex++],
                (frameStride == 4)? data.mFrameData[frameIndex++] : 0
            };
            float frameValue[4] = {
                data.mFrameData[frameIndex++],
                data.mFrameData[frameIndex++],
                data.mFrameData[frameIndex++],
                (frameStride == 4)? data.mFrameData[frameIndex++] : 0
            };
            float frameOut[4] = {
                data.mFrameData[frameIndex++],
                data.mFrameData[frameIndex++],
                data.mFrameData[frameIndex++],
                (frameStride == 4)? data.mFrameData[frameIndex++] : 0
            };

            std::cout << "\t\tFrame " << f << ":\n";
            std::cout << "\t\t\tTime: " << frameTime << "\n";
            for (int c = 0; c < frameStride; ++c) {
                std::cout << "\t\t\tIn[" << c << "]: " << frameIn[c];
                std::cout << (c == frameStride - 1)? "\n" : ", ";
            }
            for (int c = 0; c < frameStride; ++c) {
                std::cout << "\t\t\tValue[" << c << "]: " << frameValue[c];
                std::cout << (c == frameStride - 1)? "\n" : ", ";
            }
            for (int c = 0; c < frameStride; ++c) {
                std::cout << "\t\t\tValue[" << c << "]: " << frameOut[c];
                std::cout << (c == frameStride - 1)? "\n" : ", ";
            }
        }
    }
}
```

### Loading Animation::Data

It's up to the user to load and populate data. The ```Animation::Data``` class provides no functionality for loading anything from disk or from unknown formats. Animation data can be assigned using the ```Set``` function of ```Animation::Data```. The class does provide a ```Serialize``` and ```Deserialize``` function to quickly load and save animation data.

The sample files provided in this repo where generated offline using the [glTF loader code](TODO) written in [Hands On Game Animation Programming](TODO). I do have plans to add a minimal glTF loader with no external dependancies later. 

### Animation::Data functions

* ```Data();``` - Default constructor, trivial.
* ```Data(const Data& other);``` - Copy constructor, trivial
* ```Data& operator=(const Data& other);``` - Assignment operator, trivial
* ```~Data();``` - Destructor, trivial
* ```void Set(float* frameData, unsigned int frameSize, unsigned int* trackData, unsigned int trackSize);``` - Sets the frame and track data of the ```Animation::Data``` object. This function is the only way to set the data. It also calculates  ```mStartTime``` and ```mEndTime```
* ```const float* GetFrameData() const;``` - Getter to immutable ```mFrameData```. This is intended to give a "debug view", not for direct modification. That being said, direct modification _should_ be safe so long as the pointer is fresh
* ```unsigned int FrameDataSize() const;``` - Geter to ```mFrameDataSize```, trivial
* ```const unsigned int* GetTrackData() const;``` - TODO
* ```unsigned int TrackDataSize() const;``` - TODO
* ```float GetStartTime() const;``` - TODO
* ```void SetStartTime(float time);``` - TODO
* ```float GetEndtime() const;``` - TODO
* ```void SetEndTime(float time);``` - TODO
* ```bool IsValid() const;``` - TODO
* ```float GetDuration() const;``` - TODO
* ```const char* GetLabel() const;``` - TODO
* ```void SetLabel(const char* label);``` - TODO
* ```unsigned int Serialize(char* output, unsigned int outputSize) const;``` - TODO
* ```void DeSerialize(char* input);``` - TODO
* ```unsigned int SerializedSize() const;``` - TODO
* ```float Sample(State& out, float time, bool looping) const;``` - TODO

## Animation::State

TODO

# Usage

TODO

## The sample code

Where did the sample data come from

What should the user see

Link to WebGL sample