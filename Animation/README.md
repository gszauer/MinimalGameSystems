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

The ```Animation::Builder``` namespace is provided to create an intuitive interface for building animation clips. This namespace contains more traditional class definitions for ```Frame```, ```Track``` and ```Clip```, where an animation clips is made up of tracks, which are in turn made up of frames.

To create an animation clip, create an ```Animation::Builder::Clip``` object, which is made up of ```Animation::Builder::Track``` objects which are in turn made up of ```Animation::Builder::Frame``` objects. ```Samples/BlendSample[.h, .cpp]``` uses the animation builder to create an animation clip that is used to control blending what animation is playing.  The code below demonstrates how the ```Animation::Builder``` can be used to create a new animation.

```
Animation::Builder::Frame MakeFrame(float time, float in, float value, float out) {
    Animation::Builder::Frame result;

    result.time = time;
    result.in[0] = in;
    result.value[0] = value;
    result.out[0] = out;
    return result;
}
// ...
{
    Animation::Builder::Clip curve;
	blendCurve.SetName("New Curve");

    Animation::Builder::Track track;
	track.SetTarget(Animation::Data::Component::Position);
	track.SetJointID(0);

	track.PushFrame(MakeFrame(0, 0, 0, 0));
	track.PushFrame(MakeFrame(0.4f, 0, 0, 0));
	track.PushFrame(MakeFrame(0.6f, 0, 1.0f, 0));
	track.PushFrame(MakeFrame(1.4f, 0, 1.0f, 0));
	track.PushFrame(MakeFrame(1.6f, 0, 0, 0));
	track.PushFrame(MakeFrame(2, 0, 0, 0));
	
	curve.PushTrack(track);
	Animation::Data animationData = Animation::Builder::Convert(curve);
}
```


Ideally the ```Animation::Builder``` namespace will be used by offline tools that convert the desired aniamtion to ```Animation::Data``` objects, which will then be serialized. The **Converter** project that is provided uses [cgltf](https://github.com/jkuhlmann/cgltf) to convert [gltf](https://www.khronos.org/gltf/) files to convert and serialize gltf files.

### Loading and saving Animation::Data

Animation data can be serialized and de-serialized using the ```Animation::Serializer``` namespace. The serialization functions will not do any file handing, they will serialize the data into a provided area of memory. The following functions are important to serialize or de-serialize data:

* ```Animation::Serializer::SerializedDataSize``` - Returns how many bytes are needed to serialize an ```Animation::Data``` object. Usage: ```char* d = new char[SerializedDataSize(data)];```
* ```Animation::Serializer::SerializeData``` - Serializes an ```Aniamtion::Data``` object into the provided memory. It's assumed that the provided memory is large enough to hold the ```Aniamtion::Data``` object.
* ```Animation::Serializer::DeserializeData``` - Deserializes a given chunk of data into an ```Aniamtion::Data``` object.


The code below demonstrates how to deserialize animation data from a file

```
char* ReadFileContents(const char* filename) {
	char* buffer = 0;
	FILE* f = fopen(filename, "r");

	if (f != 0) {
		fseek(f, 0, SEEK_END);
		unsigned int length = (unsigned int)ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = (char*)malloc((unsigned int)sizeof(char) * (length + 1));
		if (buffer != 0) {
			memset(buffer, 0, (unsigned int)sizeof(char) * (length + 1));
			fread(buffer, 1, length, f);
			buffer[length] = '\0';
		}
		fclose(f);
	}

	return buffer;
}

// ...

Animation::Data runningAnimation;
char* input = ReadFileContents("Assets/runningData.txt");
Animation::Serializer::DeserializeData(runningAnimation, input);
free(input);
```

And the code below demonstrates how to serialize that same animation data

```
unsigned int size = Animation::Serializer::SerializedDataSize(runningAnimation);

char* outBuffer = outputBuffer = new char[size];
Animation::Serializer::SerializeData(outBuffer, runningAnimation);

std::ofstream out;
out.open("Assets/runningData.txt");
out << outBuffer;
out.close();
free[] outBuffer;
```

### Sampling Animation::Data

Animation data can be sampled either manually, or directly into an ```Animation::State```. Sampling directly into a state object is the intended way for animations to be sampled, this system was written for character animation. However, there are situations where being able to sample a track is useful, so a way to sample a track manually is provided as well. The following functions can be used to sample ```Animation::Data```:

```
float Animation::Data::Sample(State& out, float time, bool looping) const;
float Animation::Data::SampleTrack(float* out, unsigned int trackIndex, float time, bool looping) const;
```

Both functions take a ```time``` argument, the return value of the function is this input time, adjusted to be valid for this clip. For example, if the track is ```looping``` and ```time``` is greater than the clips ent time, ```time``` will be adjusted to loop with the clip.

The ```Sample``` function takes an ```Animation::State``` which is where the track data will be sampled into, it takes the current playback time and a boolean to indicate if the animation is looping or not. 

The ```SampleTrack``` function takes a float pointer, which is where the vector or quaternion, depending ont he track type will be written to. The second argument specifies track index, if a an animation was built using ```Animation::Builder```, the index is whatever index the track was pushed into the clip at. The last two arguments are time and looping, the same as the other sample function. The following code demonstrates how a ```Animation::Data``` can be sampled:

```
Animation::Data runningAnimation;
Animation::State animatedPose;
float animationTime;

Animation::Data customCurve;
float curveTime;

// ...

{
	animationTime = runningAnimation.Sample(animatedPose, animationTime + dt, true);

	float result[4] = { 0.0f }; // Store the blend result
	curveTime = customCurve.SampleTrack(result, 0, curveTime + dt, true);
	float value = result[0]; // We only care about the x track
}
```

## Animation::State

The ```Animation::State``` object is analogous to a ```Pose``` or ```Skeleton```. The ```Aniamtion::State``` class represents a hierarchy of ```Transform``` objects. A ```Transform``` object has a position, a rotation, and a scale. This hierarchy is an animated scene, or in the case of character animation, the animated pose or skeleton.

The ```Animation::State``` class encodes a transform hierarchy using the following member variables:

```
namespace Animation {
    class State {
    protected:
        float* mTransforms;
        int* mHierarchy;
        unsigned int mSize;
```

The ```mTransforms``` array contain only floating point numbers. Conceptually, the data is stored as if it where an array of transform structures, like this:

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

### Animation::State to matrix palette

TODO

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

The **bind state** is whatever the pose the skeleton was in when the mesh was skinned to it. The **rest state** is the default / reference state of the hierarchy. The bind and rest pose are often the same, but not always.

The ```Animation::Serialization``` namespace contains the following functions to serialize / deserialize ```Animation::State```.

* ```Animation::Serializer::SerializedStateSize``` - Returns how many bytes are needed to serialize an ```Animation::State``` object. Usage: ```char* p = new char[SerializedStateSize(data)];```
* ```Animation::Serializer::SerializeState``` - Serializes an ```Aniamtion::State``` object into the provided memory. It's assumed that the provided memory is large enough to hold the ```Aniamtion::State``` object.
* ```Animation::Serializer::DeserializeState``` - Deserializes a given chunk of data into an ```Aniamtion::State``` object.

The code sample below demonstrates how to serialize and de-serialize ```Animation::State``` objects.

```
Animation::State bindPose;

// Serialize
char* out = new char[Animation::Serializer::SerializedStateSize(bindPose)];
Animation::Serializer::SerializeState(out, bindPose);
out.open("bindState.txt");
out << out;
out.close();
delete[] out;

// De-Serialize
char* input = ReadFileContents("Assets/bindState.txt");
Animation::Serializer::DeserializeState(bindPose, input);
free(input);
```

# Skinning

A sample CPU Skinning implementation is provided, however the focus of this library is animating a hierarchy, not skinning. The skinning code has no context to the formating of the mesh that it is skinning. I'm not going to go into the skinning algorithm here, but i did write about it on the [Landing Page](https://animationprogramming.com/) for [Hands-On C++ Game Animation Programming](https://animationprogramming.com/)

To skin a mesh, create an ```Animation::Skin::Descriptor<float, 3>``` object for the position and normal of the vertices of a mesh. The ```Animation::Skin::Descriptor<float, 3>``` class describes a 3 dimensional vector that can be stored in memory either linearly or with a stride. The descriptor object describes how the position or normal data is laid out in memory.

The ```Animation::Skin::Apply``` function does the actual skinning. The function has these two overloads

```
void Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* animationMatrixPalette, const float* invBindPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights);

void Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* skinMatrixPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights);
```

The first three arguments are the same, a descriptor to write to, a descriptor to read from and a ```w``` component. The normals and vertices of a mesh are treated as three component vectors. To skin a mesh, these vectors are multiplied by the skin matrix. The ```w``` argument is used in this vector multiplication to controll if we're multiplying a position or a vector. So, vertex positions would have a ```w``` of 1 and vertex normals have a ```w``` of 0.

The next argument is where the two functions split. The first implementation takes two seperatie matrices ```animationMatrixPalette``` and ```invBindPalette```. These matrix arrays represent the animated hierarchy and inverse bind pose respectivley. The other implementation takes only a single matrix array ```skinMatrixPalette```, this is the same as the other two matrices, but pre-multiplied, ```skinMatrixPalette = animationMatrixPalette * invBindPalette```.

The last two arguments are again the same, a descriptor for the influences and weights of each vertex. These are expressed as ```float4``` and ```int4```, making the maximum number of bone influences per vertex 4. The code below shows a bare bones example of how the vertices of a mesh can be skinned using the provided skinning methods.

```
// Variables
Animation::Skin::Descriptor<float, 3> readPositions;
Animation::Skin::Descriptor<float, 3> readNormals;
Animation::Skin::Descriptor<float, 3> writePositions;
Animation::Skin::Descriptor<float, 3> writeNormals;
Animation::Skin::Descriptor<unsigned int, 4> readInfluences;
Animation::Skin::Descriptor<float, 4> readWeights;

// Initialize
void Initialize() {
    readPositions.Set(mVertices[0].v, numVertices * 3, 0, 0);
    readNormals.Set(mNormals[0].v, numVertices * 3, 0, 0);
    readInfluences.Set(mInfluences[0].v, numVertices * 4, 0, 0);
    readWeights.Set(mWeights[0].v, numVertices * 4, 0, 0);

    writePositions.Set(mSkinned[0].v, numVertices * 3, 6 * sizeof(float), 0);
    writeNormals.Set(mSkinned[0].v, numVertices * 3, 6 * sizeof(float), 3 * sizeof(float));
}

// Update
void SkinnedSample::Update(float dt) {
	playTime = aniamtionData.Sample(animatedPose, playTime + dt, true);
	Animation::ToMatrixPalette(animatedPosePalette[0].v, animatedPose);

	Animation::Skin::Apply(writePositions, readPositions, 1.0f, animatedPosePalette[0].v, invBindPosePalette[0].v, readInfluences, readWeights);

	Animation::Skin::Apply(writeNormals, readNormals, 0.0f, animatedPosePalette[0].v, invBindPosePalette[0].v, readInfluences, readWeights);
}
```

# Blending

TODO

# Samples

TODO

# Converter

TODO