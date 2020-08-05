# Minimal Game Animation

This is intended to be a simple, easy to use character animation library. What makes this a _character animation library_ and not a _generic animation_ library? The data structures make an assumption that the target of animation data is a transform hierarchy. Tracks don't have a generic way to bind to values, they are always assumed to be bound to transoform objects.

The library was designed to have as few files as possible, the runtime consists of only three files. The code is self contained, support functions like matrix multiplications are written in-line when possible. The runtime API is designed to be small and easy to digest, the classes present the smallest set of functions needed to work. 

The following files make up the minimal animation library:

* ```AnimationData[.h, .cpp]``` - **required** - Contains ```Animation::Data```, analogous to an animation clip.
* ```AnimationState[.h, .cpp]``` - **required** - Contains ```Aniamtion::State```, analogous to an animated pose. 
* ```AnimationInternal[.h, .cpp]``` - **required** - Contains helper functions that can be used as is or replaced by more specialized implementations.
* ```AnimationBuilder[.h, .cpp]``` - **optional** - Contains verbose classes and conversion functions that make authoring ```Animation::Data``` intuitive. Intended to be used by conversion tools, not the runtime.
* ```AnimationBlend[.h, .cpp]``` - **optional** - Contains code to blend animations smoothly.
* ```AnimationSkin[.h, .cpp]``` - **optional** - Contains code to skin meshes using descriptors and an ```Animation::State```.
* ```AnimationSerializer[.h, .cpp]``` - **optional** - Contains code to serialie and deserialise ```Animation::State``` and ```Aniamtion::Data```

For basic animations, you only need two class, ```Animation::State``` and ```Animation::Data```. Both of these classes have a dependancy on ```Animation::Helpers```, a namespace which contains helper functions for math and memory allocation. 

Animations clips and tracks are both represented by the ```Aniamtion::Data``` class, which makes authoring animations clips difficult. To address this, the ```Animation::Builder``` namespace contains easy to use classes for _Clips_ and _Tracks_ which are convertable to ```Animation::Data``` objects. Animation data and animation state can be serialized or deserialized using functions in the ```Animation::Serializer``` namespace.

A reference skinning implementation is provided in the ```Aniamtion::Skin``` namespace. This is a software skinning implementation which maps vertex data using descriptors or views. Using these descriptors allows the skinning function to deform a mesh regardless of how it's data is stored. Animation blendin is also provided trough the ```Animation::Blend``` function which does linear blending between two poses in local space.

Much of the provided code's readability suffers from trying to keep the API as small as possible. For an in depth guide to animation programming that covers all the techniques implemented here, check out my book [Hands-On C++ Game Animation Programming](https://animationprogramming.com). 

# Animation::Internal

The internal namespace contains math and memory related functions. If you want to use a custom allocator, re-implement ```Animation::Internal::Allocate``` and ```Animation::Internal::Free```.  The ```Animation::Internall``` namespace contains the following math related functions:

* ```Animation::Internal::Fmod``` - A simple implementation of ```float fmod(float x, float y)```, the c library version is not used.
* ```Animation::Internal::FloatCompare``` - Uses a small epsilon value to compare two floating point numbers. The default epsilon value is ```0.000001f```.
* ```Animation::Internal::InvSqrt``` - A [fast inverse square root](https://en.wikipedia.org/wiki/Fast_inverse_square_root) method used to normalize vectors and quaternions. The implementation could be replaced with ```1/sqrt(x)```

# Animation::State And Animation::Data

This library breaks animation down into two key concepts, **animation state** and **animation data**. Data is static, it does not change. State is driven by data, it's volatile and changes. 

Animation data is analogous to an animation clip. It contains tracks, the tracks contain frames and when sampled this data animates the components of transforms. 

Animation state is analogous to the Pose of an animation. An animation clip is typically sampled into a pose. Animation state holds the same information as a pose, think of animation state as a snapshot of animation data at some point in time.

Troughout the rest of this document, animation data will be used interchangably with animation clip and animation state will be used interchangably with pose.

## Animation::Data

In a more straight forward animation system, animation data is represented by several classes. In my [book](https://animationprogramming.com) I break animation data down into the following classes:

* __[```Frame```](https://github.com/gszauer/GameAnimationProgramming/blob/master/AllChapters/Code/Frame.h)__ Encodes a value at a given time. Also contains tangents for cubic interpolation. You can interpolate between two frames
* __[```Track```](https://github.com/gszauer/GameAnimationProgramming/blob/master/AllChapters/Code/Track.h)__ A track is made up of multiple frames. You can have different types of tracks like scalar, vector or quaternion tracks. Sampling a track results in the data type of the track
* __[```TransformTrack```](https://github.com/gszauer/GameAnimationProgramming/blob/master/AllChapters/Code/TransformTrack.h)__ A transform track is made up of multiple Tracks. It maps tracks to joints. Each joint can have three tracks: position, rotation and scale. Sampling a TransformTrack will always result in a Transform object.
* __[```Clip```](https://github.com/gszauer/GameAnimationProgramming/blob/master/AllChapters/Code/Clip.h)__ A clip, such as "waling" is made up of multiple transform tracks. Each TransformTrack describes the motion of one joint over time. An animation clip describes the motion of multiple joints, a skeleton over time. A clip samples into a Pose.

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

The ```mFrameData``` variable contains all of the frames that are inside of the animation clip. The ```mFrameDataSize``` variable contains the number of elements are inside the array. All of the frames for one track are laid out linearly. On a high level, this makes the ```mFrameData``` array segmented like so:

```
+----------------------+----------------------------------+----------------------------+
| Track 1 (24 frames)  | Track 2 (48 frames)              | Track 3 (18 frames)| etc.. |
+----------------------+----------------------------------+----------------------------+
```

An animation track usually drives every component of a vector, not just a single one. To make a track work with higher order data, animation systems often have different classes like ```ScalarTrack```, ```VectorTrack```, ```QuaternionTrack```. In this animation library, the concept of a track doesn't really exist, let's explore how tracks of vectors and quaternions are implemented.

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

// Higher order frames like VectorFrame or QuaternionFrame could be expressed with arrays:
struct QuaternionFrame {
    float time;
    float in[4];
    float value[4];
    float out[4];
}
```

This makes the diagram of how ```mFrameData``` is segmented harder to read. Two segments both containing 5 frames could have a different size (in bytes) if one segment is a vector track while another segment is a quaternion track. The ```mFrameData``` is already segmented into tracks. Each track is also segmented into frames, like so:

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


So far, there is no context for where one track begins and the other ends. There is also no context for where a frame in this array might be located. The ```mFrameData``` array is just a large chunk of memory. The ```mTrackData``` array provides context for interpreting the ```mFrameData``` array.

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
* __```component```__ Represents if the track is targeting the position, rotation or scale component of the joints transform. The size of each frame depends on this. Tracks that target position or scale are vector tracks, tracks that target rotation are quaternion tracks. The ```Animation::Data::Component``` enum should be used for the value of this int.
* __```offset```__ Where in the ```mFrameData``` array this track starts. This is an index.
* __```size```__ How many elements the ```mFrameData``` array contains make up this track. Counts how many floating point numbers make up the track.

The ```mLabel``` member of the ```Animation::Data``` class is an optional, null terminated string. It is indended to store the name of the animation clip. 

The start and end time of an animation clip depend on the tracks. The track that starts with the lowest time dictates the start time of the clip. Finding the correct start and end times involve looping trough all the tracks in the clip, to avoid doing this the times are cached in ```mStartTime``` and ```mEndTime``` whenever the animation data is set.

### Interpreting Animation::Data

The only non-trivial part of  ```Animtion::Data``` is the granularity at which interpolation types can be defined. Interpolation can change on a per frame basis. This means frames 1 and 2 might interpolate lineraly, frames 2 and 3 might use constant interpolation and frames 3 and 4 might use cubic interpolation. 

This interpolation granularity isn't uncommon, Unity's Animation window works the same way. This single curve uses step, linear and cubic interpolation.

![Unity Animation Curves](images/curve_reference.png)

The following criteria is used to determine how two frames should be interpolated.  The condition flow checks if the interpolation is constant first, if it's linear next and does cubic interpolation by default.

* If the tangent of either frame is over the step threshold (```Animation::Data::StepLimit```, ``` 1000000.0f```), the frames will have **constant interpolation**.
* If both tangents are linear, the frames will have **linear interpolation**. The linear tangent between two frames can be expressed as ```value delta / time delta```.
* If the frame interpolation is not constant or linear, it will be **cubic interpolation** by default.

The following code demonstrates how to access the values inside of the ```Animation::Data``` class in a meaningful way by printing out the contents of the class. 

```
void Print(const Animation::Data& data) {
	const float* frameData = data.GetFrameData();
	unsigned int frameDataSize = data.FrameDataSize();
	const unsigned int* trackData = data.GetTrackData();
	unsigned int trackDataSize = data.TrackDataSize();

	unsigned int trackStride = 4;
	unsigned int numTracks = trackDataSize / trackStride;

	std::cout << "Animation: " << data.GetLabel() << "\n";
	std::cout << "\tNumber of tracks: " << numTracks << "\n";
	std::cout << "\tStart time: " << data.GetStartTime() << "\n";
	std::cout << "\tEnd time: " << data.GetEndtime() << "\n";
	std::cout << "\tTracks:\n";

	// Loop trough all tracks in the animation clip
	for (unsigned int t = 0; t < numTracks; ++t) {
		std::cout << "\tTrack " << t << ":\n";

		// Current track data
		unsigned int trackId = trackData[t * trackStride + 0];
		unsigned int trackComponent = trackData[t * trackStride + 1];
		unsigned int trackOffset = trackData[t * trackStride + 2];
		unsigned int trackSize = trackData[t * trackStride + 3];

		std::cout << "\t\tTarget: " << trackId << "\n";
		std::cout << "\t\tComponent: ";
		if (trackComponent == (unsigned int)Animation::Data::Component::Position) {
			std::cout << "position\n";
		}
		else if (trackComponent == (unsigned int)Animation::Data::Component::Rotation) {
			std::cout << "rotation\n";
		}
		else {
			std::cout << "scale\n";
		}
		std::cout << "\t\tOffset: " << trackOffset << "\n";
		std::cout << "\t\tSize: " << trackSize << "\n";

		unsigned int frameStride = 3; // Vec3
		if (trackComponent == (unsigned int)Animation::Data::Component::Rotation) {
			frameStride = 4; // Quaternion
		}

		unsigned int numFrames = trackSize / frameStride;
		std::cout << "\t\tNumber of frames:" << numFrames << "\n";
		std::cout << "\t\tFrames:\n";

		// Loop trough all frames in current track
		for (unsigned int f = 0; f < numFrames; ++f) {
			int frameIndex = trackOffset + f * frameStride;

			// Current frame data
			float frameTime = frameData[frameIndex++];
			float frameIn[4] = {
				frameData[frameIndex++],
				frameData[frameIndex++],
				frameData[frameIndex++],
				(frameStride == 4) ? frameData[frameIndex++] : 0
			};
			float frameValue[4] = {
				frameData[frameIndex++],
				frameData[frameIndex++],
				frameData[frameIndex++],
				(frameStride == 4) ? frameData[frameIndex++] : 0
			};
			float frameOut[4] = {
				frameData[frameIndex++],
				frameData[frameIndex++],
				frameData[frameIndex++],
				(frameStride == 4) ? frameData[frameIndex++] : 0
			};

			std::cout << "\t\tFrame " << f << ":\n";
			std::cout << "\t\t\tTime: " << frameTime << "\n";
			for (int c = 0; c < frameStride; ++c) {
				std::cout << "\t\t\tIn[" << c << "]: " << frameIn[c];
				std::cout << (c == frameStride - 1) ? "\n" : ", ";
			}
			for (int c = 0; c < frameStride; ++c) {
				std::cout << "\t\t\tValue[" << c << "]: " << frameValue[c];
				std::cout << (c == frameStride - 1) ? "\n" : ", ";
			}
			for (int c = 0; c < frameStride; ++c) {
				std::cout << "\t\t\tValue[" << c << "]: " << frameOut[c];
				std::cout << (c == frameStride - 1) ? "\n" : ", ";
			}
		}
	}
}
```

### Creating Animation::Data

TODO: How to create animation::data with builder classes

The ```Animation::Builder``` namespace is provided to create an intuitive interface for building animation clips. This namespace contains more traditional class definitions for ```Frame```, ```Track``` and ```Clip```, where an animation clips is made up of tracks, which are in turn made up of frames.

TODO: Sample usage

Ideally the ```Animation::Builder``` namespace will be used to create offline tools that convert the desired aniamtion to ```Animation::Data``` objects, which will then be serialized. The **Converter** project that is provided uses [cgltf](https://github.com/jkuhlmann/cgltf) to convert [gltf](https://www.khronos.org/gltf/) files to runtime appropriate data.

### Loading and saving Animation::Data

Animation data can be serialized and de-serialized using the ```Animation::Serializer``` namespace, located in the ```AnimationSerializer[.h, .cpp]``` files. The serialization functions will not do any file handing, they will serialize the data into a provided area of memory. The following functions are important to serialize or de-serialize data:

* ```Animation::Serializer::SerializedDataSize``` - Returns how many bytes are needed to serialize an ```Animation::Data``` object. Usage: ```char* data = new char[SerializedDataSize(data)];```
* ```Animation::Serializer::SerializeData``` - Serializes an ```Aniamtion::Data``` object into the provided memory. It's assumed that hte provided memory is large enough to hold the ```Aniamtion::data``` object.
* ```Animation::Serializer::DeserializeData``` - Deserializes a given chunk of data into an ```Aniamtion::Data``` object.

### Sampling Animation::Data

TODO

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

# Samples

TODO

# Converter

TODO