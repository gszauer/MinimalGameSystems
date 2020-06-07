#include "AnimationData.h"
#include "AnimationState.h"

#if 0
#include <ostream>
std::ostream& operator<<(std::ostream& os, const Animation::Data& data) {
	const char* label = data.GetLabel();
	float startTime = data.GetStartTime();
	float endTime = data.GetEndtime();
	const unsigned int* trackData = data.GetTrackData();
	unsigned int trackDataSize = data.TrackDataSize();
	const float* frameData = data.GetFrameData();

	unsigned int trackStride = 4;
	unsigned int numTracks = trackDataSize / trackStride;

	os << "Animation: " << label << "\n";
	os << "\tNumber of tracks: " << numTracks << "\n";
	os << "\tStart time: " << startTime << "\n";
	os << "\tEnd time: " << endTime << "\n";
	os << "\tTracks:\n";

	// Loop trough all tracks in the animation clip
	for (unsigned int t = 0; t < numTracks; ++t) {
		os << "\tTrack " << t << ":\n";

		// Current track data
		unsigned int trackId = trackData[t * trackStride + 0];
		unsigned int trackComponent = trackData[t * trackStride + 1];
		unsigned int trackOffset = trackData[t * trackStride + 2];
		unsigned int trackSize = trackData[t * trackStride + 3];

		os << "\t\tTarget: " << trackId << "\n";
		os << "\t\tComponent: ";
		if (trackComponent == 0) { // 0 = position
			os << "position\n";
		}
		else if (trackComponent == 1) { // 1 - rotation
			os << "rotation\n";
		}
		else { // 2 - scale
			os << "scale\n";
		}
		os << "\t\tOffset: " << trackOffset << "\n";
		os << "\t\tSize: " << trackSize << "\n";

		unsigned int frameStride = 3; // position and scale are vec3
		if (trackComponent = 1) { // rotation is quat
			frameStride = 4;
		}

		unsigned int numFrames = trackSize / frameStride;
		os << "\t\tNumber of frames:" << numFrames << "\n";
		os << "\t\tFrames:\n";

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

			os << "\t\tFrame " << f << ":\n";
			os << "\t\t\tTime: " << frameTime << "\n";
			for (int c = 0; c < frameStride; ++c) {
				os << "\t\t\tIn[" << c << "]: " << frameIn[c];
				os << (c == frameStride - 1) ? "\n" : ", ";
			}
			for (int c = 0; c < frameStride; ++c) {
				os << "\t\t\tValue[" << c << "]: " << frameValue[c];
				os << (c == frameStride - 1) ? "\n" : ", ";
			}
			for (int c = 0; c < frameStride; ++c) {
				os << "\t\t\tValue[" << c << "]: " << frameOut[c];
				os << (c == frameStride - 1) ? "\n" : ", ";
			}
		}
	}
}
#endif

namespace Animation {
	bool FloatCompare(float a, float b) {
		return (a == b); // TODO
	}
	
	float FastInvSqrt(float number) { // 1 / sqrt(number)
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *(long*)&y;                       // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1);               // what the fuck?
		y = *(float*)&i;
		y = y * (threehalfs - (x2 * y * y));   // 1st iteration

		return y;
	}
}

Animation::Data::Data() {
	mFrameData = 0;
	mFrameDataSize = 0;
	mTrackData = 0;
	mTrackDataSize = 0;
	mStartTime = 0.0f;
	mEndTime = 0.0f;
	mLabel = 0;
}

Animation::Data::Data(const Data& other) {
	mFrameData = 0;
	mFrameDataSize = 0;
	mTrackData = 0;
	mTrackDataSize = 0;
	mStartTime = 0.0f;
	mEndTime = 0.0f;
	mLabel = 0;
	*this = other;
}

Animation::Data& Animation::Data::operator=(const Data& other) {
	if (this == &other) {
		return *this;
	}

	Set(other.mFrameData, other.mFrameDataSize, other.mTrackData, other.mTrackDataSize);
	SetLabel(other.GetLabel());
}

Animation::Data::~Data() {
	if (mTrackData != 0) {
		delete[] mTrackData;
	}

	if (mFrameData != 0) {
		delete[] mFrameData;
	}

	if (mLabel != 0) {
		delete[] mLabel;
	}
}

const float* Animation::Data::GetFrameData() const {
	return mFrameData;
}

const unsigned int* Animation::Data::GetTrackData() const {
	return mTrackData;
}

unsigned int Animation::Data::FrameDataSize() const {
	return mFrameDataSize;
}

unsigned int Animation::Data::TrackDataSize() const {
	return mTrackDataSize;
}

float Animation::Data::GetStartTime() const {
	return mStartTime;
}

float Animation::Data::GetEndtime() const {
	return mEndTime;
}

bool Animation::Data::IsValid() const {
	return mEndTime > mStartTime && mFrameDataSize > 0 && mTrackDataSize > 0;
}

float Animation::Data::GetDuration() const {
	return mEndTime - mStartTime;
}

const char* Animation::Data::GetLabel() const {
	return mLabel;
}

void Animation::Data::SetLabel(const char* label) {
	if (mLabel != 0) {
		delete[] mLabel;
		mLabel = 0;
	}

	if (label == 0) {
		return;
	}
	
	const char* iterator;
	for (iterator = label; *iterator; ++iterator);
	unsigned int length = iterator - label;

	if (length == 0) {
		return;
	}
	mLabel = new char[length + 1];
	// StrCpy
	for (; *mLabel = *label; ++label, ++mLabel);
	mLabel[length] = '\0';
}

float Animation::Data::Sample(State& out, float time, bool looping) const {
	// Adjust time to fit valid range for clip
	if (looping) {
		float duration = mEndTime - mStartTime;
		if (duration <= 0) {
			return 0.0f;
		}

		// time = fmodf(time - mStartTime, mEndTime - mStartTime);
		float fmodA = time - mStartTime;
		float fmodB = mEndTime - mStartTime;
		int truncated = (int)(fmodA / fmodB);
		time = fmodA - ((float)truncated * fmodB);
		// End fmod

		while (time < mStartTime) {
			time += mEndTime - mStartTime;
		}
	}
	else {
		if (time < mStartTime) {
			time = mStartTime;
		}
		if (time > mEndTime) {
			time = mEndTime;
		}
	}
	
	// Loop trough all tracks
	unsigned int trackStride = 4;
	for (unsigned int i = 0; i < mTrackDataSize; i += trackStride) {
		// Gather info to access the right frames
		unsigned int offset = mTrackData[i + 2]; 
		unsigned int size = mTrackData[i + 3];
		unsigned int frameStride = 3 * 3 + 1;
		if (mTrackData[i + 1] == (unsigned int)Component::Rotation) {
			frameStride = 3 * 4 + 1;
		}
		unsigned int numFrames = size / frameStride;
		
		// The joint that is being animated
		unsigned int target = mTrackData[i];

		// Find current and next frames
		float trackStartTime = mFrameData[offset];
		float trackEndTime = mFrameData[offset + size - 1];
		unsigned int thisFrame = 0;
		
		float fmodA = time - trackStartTime;
		float fmodB = trackEndTime - trackStartTime;
		// t = fmodf(time - startTime, endTime - startTime); i didn't want to use the standard library here, it's just an fmod
		int trunc = (int)(fmodA / fmodB);
		float trackTime = fmodA - ((float)trunc * fmodB); // Time normalized to this track, not the animation
		while (trackTime < fmodA) {
			trackTime += fmodB - fmodA;
		}

		if (looping) {
			for (unsigned int j = offset; j < offset + size; j += trackStride) {
				if (trackTime >= mFrameData[j]) {
					thisFrame = j;
					break;
				}
			}
		}
		else {
			if (time < trackStartTime) {
				thisFrame = 0;
				trackTime = trackStartTime;
			}
			if (time > trackEndTime) {
				thisFrame = numFrames - 2; // -2 so thisFrame + 1 is a valid index
				trackTime = trackEndTime;
			}
		}
		unsigned int nextFrame = thisFrame + 1;

		// Find frame delta and interpolation t
		float frameDelta = mFrameData[thisFrame * frameStride] - mFrameData[nextFrame * frameStride];
		float t = 0.0f;
		if (frameDelta > 0.0f) {
			t = (trackTime - mFrameData[thisFrame * frameStride]) / frameDelta;
		}

		const float* component = 0;
		unsigned int componentSize = 3;
		if (mTrackData[i + 1] == (unsigned int)Component::Position) {
			component = out.GetRelativePosition(target);
		}
		else if (mTrackData[i + 1] == (unsigned int)Component::Rotation) {
			component = out.GetRelativeRotation(target);
			componentSize = 4;
		}
		else {
			component = out.GetRelativeScale(target);
		}

		// Interpolate between the two frames
		float result[4] = {};
		float* a = &mFrameData[thisFrame * frameStride + 1 + frameStride];
		float* b = &mFrameData[nextFrame * frameStride + 1 + frameStride];
		for (unsigned int j = 0; j < componentSize; ++j) {
			float outTangent = mFrameData[thisFrame * frameStride + 1 + j];
			float inTangent = mFrameData[nextFrame * frameStride + frameStride + frameStride + 1 + j];

			float y = mFrameData[nextFrame * frameStride + 1 + frameStride + j] - mFrameData[thisFrame * frameStride + 1 + frameStride + j]; // Delta on y axis (track value)
			float x = mFrameData[nextFrame * frameStride] - mFrameData[thisFrame * frameStride]; // Delta on x axis (track time)
			if (x < 0.0f) { x *= -1.0f; }
			float linearTangent = FloatCompare(x, 0)? 0.0f : y / x;

			if (outTangent >= 10000000.0f || outTangent <= -10000000.0f || inTangent >= 10000000.0f || inTangent <= -10000000.0f) { // Constant Interpolation
				result[j] = a[j];
			}
			else if (FloatCompare(inTangent, linearTangent) && FloatCompare(outTangent, linearTangent)) { // Linear interpolation
				float from = a[j];
				float to = b[j];

				if (mTrackData[i + 1] == (unsigned int)Component::Rotation) {
					if (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3] < 0.0f) {
						to = to * -1.0f; // Neighborhood
					}
				}
				result[j] = from + (to - from) * t;
			}
			else { //  Cubic interpolation
				float p1 = a[j];
				float s1 = outTangent * frameDelta;

				float p2 = b[j];
				float s2 = inTangent * frameDelta;

				if (mTrackData[i + 1] == (unsigned int)Component::Rotation) { // Quaternion neighborhood
					if (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3] < 0.0f) {
						p2 = -p2;
					}
				}

				// hermite
				float tt = t * t;
				float ttt = tt * t;

				float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
				float h2 = -2.0f * ttt + 3.0f * tt;
				float h3 = ttt - 2.0f * tt + t;
				float h4 = ttt - tt;

				result[j] = p1 * h1 + p2 * h2 + s1 * h3 + s2 * h4;
			}
		}

		// If this is a quaternion (rotation), it was lerped so far, not nlerped. Need to normalize.
		// This normalization should take care of both linear and quadratic curves. Constants don't need it, but get it too.
		if (mTrackData[i + 1] == (unsigned int)Component::Rotation) {
			float lenSq = result[0] * result[0] + result[1] * result[1] + result[2] * result[2] + result[3] * result[3];

			if (FloatCompare(lenSq, 0.0f)) { // Length of 0, invalid!
				result[0] = 0.0f;
				result[1] = 0.0f;
				result[2] = 0.0f;
				result[3] = 1.0f;
			}
			else {
				float inverseLength = FastInvSqrt(lenSq);
				result[0] *= inverseLength;
				result[1] *= inverseLength;
				result[2] *= inverseLength;
				result[3] *= inverseLength;
			}
		}

		// Set interpolation result back into output data
		if (mTrackData[i + 1] == (unsigned int)Component::Position) {
			out.SetRelativePosition(target, result);
		}
		else if (mTrackData[i + 1] == (unsigned int)Component::Rotation) {
			out.SetRelativeRotation(target, result);
		}
		else { // Component::Scale
			out.SetRelativeScale(target, result);
		}
	}

	// Return time relative to valid play range
	return time;
}

#pragma warning(push)
#pragma warning(disable:6385) // If you manage to overflow the track buffer the animation was huge
void Animation::Data::Set(float* frameData, unsigned int frameSize, unsigned int* trackData, unsigned int trackSize) {
	// Set frame data
	if (frameData == 0 || frameSize == 0) {
		if (mFrameData != 0) {
			delete[] mFrameData;
		}
		mFrameData = 0;
		mFrameDataSize = 0;
		return;
	}

	if (frameSize != mFrameDataSize) {
		if (mFrameData != 0) {
			delete[] mFrameData;
		}
		mFrameData = new float[frameSize];
		mFrameDataSize = frameSize;
	}

	for (unsigned int i = 0; i < frameSize; ++i) {
		mFrameData[i] = frameData[i];
	}

	// Set track data
	if (trackData == 0 || trackSize == 0) {
		if (mTrackData != 0) {
			delete[] mTrackData;
		}
		mTrackData = 0;
		mTrackDataSize = 0;
		return;
	}

	if (trackSize != mTrackDataSize) {
		if (mTrackData != 0) {
			delete[] mTrackData;
		}
		mTrackData = new unsigned int[trackSize];
		mTrackDataSize = trackSize;
	}

	for (unsigned int i = 0; i < trackSize; ++i) {
		mTrackData[i] = trackData[i];
	}

	// Find start and end times
	mStartTime = mFrameData[0];
	mEndTime = mFrameData[0];
	unsigned int trackStride = 4;
	// Loop trough all tracks
	for (unsigned int i = 0; i < mTrackDataSize; i += trackStride) {
		unsigned int offset = mTrackData[i + 2];
		unsigned int size = mTrackData[i + 3];

		float trackStartTime = mFrameData[offset];
		float trackEndTime = mFrameData[offset + size - 1];

		if (trackStartTime < mStartTime) {
			mStartTime = trackStartTime;
		}
		if (trackEndTime > mEndTime) {
			mEndTime = trackEndTime;
		}
	}
}
#pragma warning(pop)

unsigned int Animation::Data::Serialize(char* output, unsigned int outputSize) const {
	return 0;
	// TODO
}

void Animation::Data::DeSerialize(char* input, unsigned int inputSize) {
	// TODO
}

unsigned int Animation::Data::SerializedSize() const {
	// TODO
	return 0;
}