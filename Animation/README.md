# Animation

This library is intended to be a simple, easy to use animation library that's easy to integrate into any project. The library was designed for, but is not limited to character animation. The code does not provide anything outside of animation. There is no file loading, dependancy on any external libraries, external headers, etc... There are a few instances of ```new``` and ```delete```, but they should be easy to replace if needed.

The goal of this library is simplicity, to provide a minimal surface API for animation. The code here aims to be easy and intuitive by providing a minimal API to interact with. For basic animations, you only need two class, ```Animation::State``` and ```Animation::Data```. Aside from the core classes, utilities for skinning and animation blending are also provided.

The readability of the logic for many of these functions suffers from trying to keep the API as small as possible. For an in depth guide to animation programming that covers all of the techniques implemented here check out my book [Hands-On C++ Game Animation Programming](https://animationprogramming.com). The following files are provided

* ```AnimationData[.h, .cpp]``` - **required** - Contains ```Animation::Data```, analogous to an animation clip.
* ```AnimationState[.h, .cpp]``` - **required** - Contains ```Aniamtion::State```, analogous to an animated pose. 
* ```AnimationHelpers[.h, .cpp]``` - **required** - Contains helper functions that can be used as is or replaced by more specialized implementations.
* ```AnimationBuilder[.h, .cpp]``` - **optional** - Contains verbose classes and conversion functions that make authoring ```Animation::Data``` and ```Animation::State``` intuitive. Intended to be used by conversion tools, not the runtime.
* ```AnimationBlend[.h, .cpp]``` - **optional** - Contains code to blend animations smoothly.
* ```AnimationSkin[.h, .cpp]``` - **optional** - Contains code to skin meshes using ```Animation::State```.

# Helpers

The ```AnimationState[.h, .cpp]``` files contains helper function. These helper functions wrap functionality that is already available (and probably better implemented) in the standard library. The intention with this was to provide a central place for all standard library functions so they can be re-implemented with more appropriate versions.

# State And Data

This library breaks animation down into two key concepts, **animation state** and **animation data**. Data is static, it does not change. State is driven by data, it's volatile and changes. 

Animation data is analogous to an animation clip. It contains frames, the frames make tracks and the tracks move components of transforms. 

Animation state is analogous to the Pose of an animation. An animation clip is typically sampled into a pose. Animation state is that, it holds the same information as a pose.

Troughout the rest of this document, animation data will be used interchangably with animation clip and animation state will be used interchangably with pose.

## Animation::Data

In a more straight forward animation system, animation data is represented by several classes. In my [book](https://animationprogramming.com) I break this down into the following classes:

* __[Frame](https://github.com/gszauer/GameAnimationProgramming/blob/master/AllChapters/Code/Frame.h)__ Encodes a value at a given time. Also contains tangents for cubic interpolation. You can interpolate between two frames
* __[Track](https://github.com/gszauer/GameAnimationProgramming/blob/master/AllChapters/Code/Track.h)__ A track is made up of multiple frames. You can have different types of tracks like scalar, vector or quaternion tracks. Sampling a track results in the data type of the track
* __[TransformTrack](https://github.com/gszauer/GameAnimationProgramming/blob/master/AllChapters/Code/TransformTrack.h)__ A transform track is made up of multiple Tracks. It maps tracks to joints. Each joint can have three tracks: position, rotation and scale. Sampling a TransformTrack will always result in a Transform object.
* __[Clip](https://github.com/gszauer/GameAnimationProgramming/blob/master/AllChapters/Code/Clip.h)__ A clip, such as "waling" is made up of multiple transform tracks. Each TransformTrack describes the motion of one joint over time. An animation clip describes the motion of multiple joints, a skeleton over time. A clip samples into a Pose.

In this animation system, all of that data still exists, it's just all packed into one compact class. The ```Animation::Data``` class contains the following members:


```
namespace Animation {
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

An animation track usually drives every component of a vector, not just a single one. To make a track work with higher order data, animation systems often have different classes like ```ScalarTrack```, ```VectorTrack```, ```QuaternionTrack```. In my [book](https://animationprogramming.com) book i chose to template the __[Track](https://github.com/gszauer/GameAnimationProgramming/blob/master/AllChapters/Code/Track.h)__ class, this library takes a different approac..

Conceptually, each type of track has to have it's own type of frame. This is important because the size of a single frame (in bytes) changes based on how much data a frame represents. Consider the following frame structures

```
struct ScalarFrame {
    float time;
    float in;
    float value;
    float out;
}

struct VectorFrame {
    float time;
    vec3 in;
    vec3 value;
    vec3 out;
}

// QuaternionFrame, etc...

// Higher order frames like VectorFrame could be expressed with arrays:
struct Float3Frame {
    float time;
    float in[3];
    float value[3];
    float out[3];
}
```

This makes the diagram of how ```mFrameData``` is segmented harder to read. Two segments both containing 5 frames could have a different size (in bytes) if one segment could hold a vector track while another segment could hold a quaternion track. The ```mFrameData``` is already segmented into tracks. Each track is also segmented into frames, like so:

```
+----------------------+----------------------------------+----------------------------+
| Track 1 (24 frames)  | Track 2 (48 frames)              | Track 3 (18 frames)| etc.. |
+----------------------+---+------------------------------+----------------------------+
  |                        |
  |   +----------------------------------+
  |   | Assuming Track 1 is a vec3 track |
  |   +-------------------------------+--+----------------------------+----------------+
  +-> | time, in[3], value[3], out[3] | time, in[3], value[3], out[3] | time, in[3] ...|
      +-------------------------------+-------------------------------+----------------+
                           |
                           |    +----------------------------------+
                           |    | Assuming Track 2 is a quat track |
                           |    +-------------------------------+--+--------------------+
                            +-> | time, in[4], value[4], out[4] | time, in[4],  etc...  |
                                +-------------------------------+-----------------------+
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

* __```id```__ The index of the joint in ```Animation::State``` whose transform is driven by this track.
* __```component```__ Represents if the track is targeting the position, rotation or scale component of the joints transform. The size of each frame depends on this. Tracks that target position and scale are vector tracks, tracks that target rotation are quaternion tracks.
* __```offset```__ Where in the ```mFrameData``` array this track starts. This is an index.
* __```size```__ How many elements the ```mFrameData``` array contains make up this track. Counts how many floating point numbers make up the track.

The ```mLabel``` member of the ```Animation::Data``` class is an optional, null terminated string. It is indended to store the name of the animation clip. 

The start and end time of an animation clip depend on the tracks. The track that starts with the lowest time dictates the start time of the clip. Finding the correct start and end times involve looping trough all the tracks in the clip, to avoid doing this the times are cached in ```mStartTime``` and ```mEndTime``` whenever the animation data is set.

### Interpreting Animation::Data

The only thing that's non trivial about <code>Animtion::Data</code> is the granularity at which interpolation types can be defined. Interpolation can change on a per frame basis. This means frames 1 and 2 might interpolate lineraly, frames 2 and 3 might use constant interpolation and frames 3 and 4 might use cubic interpolation. This isn't uncommon, Unity's Animation window works the same way. The following criteria is used to determine how two frames should be interpolated.

TODO: Show interpolation selection logic

The following code prints out the ```Animation::Data``` class. It demonstrates how to access all important elements inside the ```Animation::Data``` class.


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

The sample files provided in this repo where generated offline using the [glTF loader code](https://github.com/gszauer/GameAnimationProgramming/blob/master/AllChapters/Code/GLTFLoader.h) written in [Hands-On C++ Game Animation Programming](https://animationprogramming.com). I modified the loading code to convert glTF files to the serialized format expected by this library instead.

> I do have plans to add a minimal glTF loader with no external dependancies later, until that happens the utility of this library is limited.

### Animation::Data functions

* ```Data();``` - Default constructor, trivial.
* ```Data(const Data& other);``` - Copy constructor, trivial
* ```Data& operator=(const Data& other);``` - Assignment operator, trivial
* ```~Data();``` - Destructor, trivial
* ```void Set(float* frameData, unsigned int frameSize, unsigned int* trackData, unsigned int trackSize);``` - Sets the frame and track data of the ```Animation::Data``` object. This function is the only way to set the data. It also calculates  ```mStartTime``` and ```mEndTime```
* ```const float* GetFrameData() const;``` - Getter to immutable ```mFrameData```. This is intended to give a "debug view", not for direct modification. That being said, direct modification _should_ be safe so long as the pointer is fresh
* ```unsigned int FrameDataSize() const;``` - Geter to ```mFrameDataSize```, trivial
* ```const unsigned int* GetTrackData() const;``` - Getter to ```mTrackData```, trivial
* ```unsigned int TrackDataSize() const;``` - Getter to ```mTrackDataSize```, trivial
* ```float GetStartTime() const;``` - Getter for the cached start time of this animation, trivial. Cached start time is set by the ```Set``` function.
* ```float GetEndtime() const;``` - Getter for the cahced end time of this animation, trivial. Cached end time is set by the ```Set``` function.
* ```float GetDuration() const;``` - Getter for the cached duration of this animation, trivial. Cached duration is set by the ```Set``` function.
* ```bool IsValid() const;``` - Returns false if the animation has a duration of 0, there is no frame data, or if there is no track data.
* ```const char* GetLabel() const;``` - Getter for the string intended to store the animation name, trivial.
* ```void SetLabel(const char* label);``` - Setter for the string intended to store the animation name, trivial.
* ```unsigned int Serialize(char* output, unsigned int outputSize) const;``` - Serializes the class into the output buffer. Size is expected in bytes
* ```void DeSerialize(char* input, unsigned int inputSize);``` - De-Serializes class from the input buffer. Size is expected in bytes
* ```unsigned int SerializedSize() const;``` - Returns how many bytes are needed to serialize this class.
* ```float Sample(State& out, float time, bool looping) const;``` - Samples this ```Animation::Data``` object into an ```Animation::State``` given a specific time. The floating point value that is returned is the time, adjusted to be in the valid playback range of the animation.

## Animation::State

The ```Animation::State``` object is analogous to a ```Pose``` class or something similar. The ```Aniamtion::State``` class is a wrapper for a simple hierarchy. This hierarchy is the animated scene, or in the case of character animation, the animates pose or skeleton.

> I've played around with the idea of removing ```Animation::State``` in favor of a descriptor type of scheme. Where ```Animation::Data::Sample``` would take an ```Animation::HierarchyDescriptor``` object that provides a mapping to where the transform data should be stored. This would make the animation code presented here much easier to integrate into existing code bases, and it would eliminate two spots in code that do memory allocation. I'm not sure how the descriptor would store the hierarchy relationship.

The ```Animation::State``` class encodes a hierarchy using the following member variables:

```
namespace Animation {
    class State {
    protected:
        float* mTransforms;
        int* mHierarchy;
        unsigned int mSize;
```

The ```mTransforms``` array contain only floating point numbers. Conceptually, the data is stored as if it where an array of transform structures, like so:

```
struct Transform {
    float position[3];
    float rotation[4];
    float scale[3];
}
```

This means that each transform has a stride of 10 elements. Accessing the transform data of a bone, for example, bone five, can be done like this:

```
int transformStride = 10;
int index = 5 * transformStride;

float position[3] = {
    mTransforms[index + 0],
    mTransforms[index + 1],
    mTransforms[index + 2]
}

float position[4] = {
    mTransforms[index + 3],
    mTransforms[index + 4],
    mTransforms[index + 5],
    mTransforms[index + 6]
}

float scale[3] = {
    mTransforms[index + 7],
    mTransforms[index + 8],
    mTransforms[index + 9]
}

// Alternateley, if you just want pointers:
float* position = &mTransforms[index * 10];
float* rotation = &mTransforms[index * 10 + 3];
float* scale = &mTransforms[index * 10 + 7];
```

The data in ```mTransforms``` is segmented twice. On the top level, there is one transform located every 10 floats. Below that, each transform contains three floats for position, four for rotation, and five for scale.

```
+--------------+-------------+-------------+-------------+
| Transform 0  | Transform 1 | Transform 2 | Transform 3 |
+--------------+-----------+-+-------+-----+-------------+
  |                        |         |  
  |   +-------------+      |         |   +-------------+      
  |   | Transform 0 |      |         |   | Transform 2 | 
  |   +-------------+----------+     |   +-------------+----------+ 
  +-> | pos[3], rot[4], scl[3] |     +-> | pos[3], rot[4], scl[3] | 
      +------------------------+         +------------------------+
                           |
                           |    +-------------+
                           |    | Transform 1 |
                           |    +-------------+----------+
                            +-> | pos[3], rot[4], scl[3] |
                                +------------------------+
```

The ```mHierarchy``` array has the same size as the ```mTransforms``` array. The size for both arrays is stored in the ```mSize``` variable. The index of each joint in the ```mTransforms``` array is the ID of that joint. Joint, bone and transform are used interchangeably here. The ```mHierarchy``` and ```mTransforms``` arrays are parallel. For each joint, the index of the joints parent is stored in the ```mHierarchy``` array.

If a joint is a root node, it's parent in ```mHierarchy``` will be negative. There can be more than one root node. The joints are not stored in any particular order, it's possible to have a root node randomly in the middle of the array with children on both sides.  

### Interpreting Animation::State

The following code prints out the ```Animation::State``` class. It demonstrates how to access all important elements inside the ```Animation::State``` class.


```
void Print(const Animation::State& state) {
    std::cout << "Number of nodes in scene graph: " << state.mSize << "\n";
    std::cout << "Parent child relationships: " << "\n";
    for (unsigned int i = 0; i < state.mSize; ++i) {
        std::cout << "\tParent: " << state.mHierarchy[i] << ", Child: " << i << "\n";
    }

    unsigned int trackStride = 10;
    std::cout << "Local transforms: \n";
    for (unsigned int i = 0; i < state.mSize; ++i) {
        unsigned int index = i * trackStride;
        std::cout << "\t" << i << ": position {";
        std::cout << state.mTransforms[index + 0] << ", ";
        std::cout << state.mTransforms[index + 1] << ", ";
        std::cout << state.mTransforms[index + 2] << "}, rotation {";
        std::cout << state.mTransforms[index + 3] << ", ";
        std::cout << state.mTransforms[index + 4] << ", ";
        std::cout << state.mTransforms[index + 5] << ", ";
        std::cout << state.mTransforms[index + 6] << "}, scale {";
        std::cout << state.mTransforms[index + 7] << ", ";
        std::cout << state.mTransforms[index + 8] << ", ";
        std::cout << state.mTransforms[index + 9] << "}\n";
    }

    std::cout << "Global transforms: \n";
    for (unsigned int i = 0; i < state.mSize; ++i) {
        float position[3], rotation[4], scale[3] = { 0 };
        state.GetAbsoluteTransform(i, position, rotation, scale);

        std::cout << "\t" << i << ": position {";
        std::cout << position[0] << ", ";
        std::cout << position[1] << ", ";
        std::cout << position[2] << "}, rotation {";
        std::cout << rotation[0] << ", ";
        std::cout << rotation[1] << ", ";
        std::cout << rotation[2] << ", ";
        std::cout << rotation[3] << "}, scale {";
        std::cout << scale[0] << ", ";
        std::cout << scale[1] << ", ";
        std::cout << scale[2] << "}\n";
    }
}
```

### Loading Animation::State

Animation state is volatile, it is the result of sampling animation data at some point in tieme. It doesn't make much sense to save volatile state. But, there are two special states for each animation:

* Bind State
* Rest State

The two are defined seperateley. The **bind state** is whatever the pose the skeleton was in when the mesh was skinned to it. The **rest state** is the default / reference state of the hierarchy. The bind and rest pose are often the same, but not always. Some file formats or 3DCC tools might not export the two poses as the same thing. 

The ```Serialize``` and ```Deserialize``` functions are intended to save and load the bind and rest poses. 

TODO: Function signatores are now off here and with data. Fix that.

### Animation::State functions

* ```State();``` - Default constructor, trivial
* ```State(const State& other);``` - Copy constructor, trivial
* ```State& operator=(const State& other);``` - ASsignment operator, trivial
* ```~State();``` - Destructor - Trivial
* ```bool ToMatrixPalette(float* outArray, unsigned int arraySize) const;``` - Write the global transform of each node in the hierarchy into a flat array of 4x4 matrices.
* ```unsigned int Size() const;``` - Returns the number of transforms in the hierarchy
* ```void Resize(unsigned int size);``` - Adjusts the total number of transforms in the hierarchy
* ```int GetParent(unsigned int index) const;``` - Gets the parent index of a joint, -1 if joint has no parent.
* ```void SetParent(unsigned int index, int parent);``` - Sets the parent index of a joint
* ```const float* GetRelativePosition(unsigned int index) const;``` - Returns the local position (x, y, z) of a joint. Do not modify the target of the constant pointer.
* ```const float* GetRelativeRotation(unsigned int index) const;``` - Returns the local rotation (x, y, z, w) of a joint. Do not modify the target of the constant pointer.
* ```const float* GetRelativeScale(unsigned int index) const;``` - Returns the local scale (x, y, z) of a joint. Do not modify the target of the constant pointer.
* ```void GetRelativePosition(unsigned int index, float* outVec3) const;``` - Writes the local position (x, y, z) of a joint into the provided array
* ```void GetRelativeRotation(unsigned int index, float* outQuat) const;``` - Writes the local rotation (x, y, z, w) of a joint into the provided array
* ```void GetRelativeScale(unsigned int index, float* outVec3) const;``` - Writes the local scale (x, y, z) of a joint into the provided array.
* ```void SetRelativePosition(unsigned int index, const float* pos);``` - Sets the local position (x, y, z) of a joint.
* ```void SetRelativeRotation(unsigned int index, const float* rot);``` - Sets the local rotation (x, y, z, w) of a joint.
* ```void SetRelativeScale(unsigned int index, const float* scl);``` - Sets the local scale (x, y, z) of a joint.
* ```void GetAbsolutePosition(unsigned int index, float* outVec3) const;``` - Writes the global position (x, y, z) of a joint into the provided array
* ```void GetAbsoluteRotation(unsigned int index, float* outQuat) const;``` - Writes the global rotation (x, y, z, w) of a joint into the provided array
* ```void GetAbsoluteScale(unsigned int index, float* outVec3) const;``` - Writes the global scale (x, y, z) of a joint into the provided array
* ```void GetRelativeTransform(unsigned int index, float* outPos, float* outRot, float* outScl) const;``` - Writes the local position (x, y, z), rotation (x, y, z, w) and scale (x, y, z) of a joint into the provided arrays.
* ```void GetAbsoluteTransform(unsigned int index, float* outPos, float* outRot, float* outScl) const;``` - Writes the global position (x, y, z), rotation (x, y, z, w) and scale (x, y, z) of a joint into the provided arrays.
* ```unsigned int Serialize(char* output, unsigned int outputSize) const;``` - Serializes the class into the output buffer. Size is expected in bytes
* ```void DeSerialize(char* input, unsigned int inputSize);``` - De-Serializes class from the input buffer. Size is expected in bytes
* ```unsigned int SerializedSize() const;``` - Returns how many bytes are needed to serialize this class.

# Skinning

TODO

# Blending

TODO

# Helpers

TODO

# Usage

TODO

## The sample code

Where did the sample data come from

What should the user see

Link to WebGL sample

# What went wrong

* The data storage of ```Animation::Data``` is overly complex. Packing the data is hard to get right, I spent three (weekend) days writing and debugging a converter. Most of the difficuly is in the conversions between vectors of touples (```std::vector<vec3>```) and arrays of scalars (```float*```). 
* ```Animation::Helpers``` was not planned. I didn't plan on including this module, but the shared code between state and data started to grow and they needed a common place. I like that this namespace contains functions that can be better implemented based on the context that the animation system is used in.
* The serialized file format is hacky at best. I only created these functions to import existing data that can be used to test the system. A more robust, preferably binary format is needed.
* Both ```Animation::Data``` and ```Animation::State``` need the ability to load from a standard format, then serialize to something faster to work with. 
* The ```Animation::Data``` API for setting data requires intimate knowledge of teh data layout. This makes the class complicated and error prone. I have not been able to figure out a better API, primarily beause both Animation Clip and Animation Track data is stored in the same class.

# What went right

* While the internal data storage of the animation classes are complicated, using the classes is simple. I feel like the goal of having a low barrier to entry worked.
* Since the users of the animation API typically don't work with track data, rolling track and clip data into one class (```Animation::Data```) greatly simplified the API.
* Making ```Animation::Skin``` work with data views allows for mapping the skinning data to just about any input format. It's a cache nightmare, but the API is easy to use.

## Intended for characters

What makes this a _character animation library_ and not a _generic animation_ library? This library makes the assumption that the target of animation data is a transform hierarchy. Tracks have targets expressed as enum values that only makes sens in the context of animating a hierarchy. Generally, when animating a hierarchy we're animating characters.

A more general porpuse animation system would just be a collection of tracks. Each track would need a generic way to bind to a value, like a pointer to the tracks target. With a setup like this, it wouldn't matter what is being animated, tracks are bound to the generic attributes of a hierarchy and don't need context for what they are animating.
