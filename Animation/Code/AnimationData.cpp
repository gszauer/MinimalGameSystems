#include "AnimationData.h"
#include "AnimationState.h"
#include "AnimationHelpers.h"

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

	SetRawData(other.mFrameData, other.mFrameDataSize, other.mTrackData, other.mTrackDataSize);
	SetLabel(other.GetLabel());

	return *this;
}

Animation::Data::~Data() {
	if (mFrameData != 0) {
		Animation::Free(mFrameData);
	}
	if (mTrackData != 0) {
		Animation::Free(mTrackData);
	}
	if (mLabel != 0) {
		Animation::Free(mLabel);
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
	return GetEndtime() > GetStartTime() && mFrameDataSize > 0 && mTrackDataSize > 0;
}

float Animation::Data::GetDuration() const {
	return GetEndtime() - GetStartTime();
}

const char* Animation::Data::GetLabel() const {
	return mLabel;
}

void Animation::Data::SetLabel(const char* label) {
	if (mLabel != 0) {
		Animation::Free(mLabel);
		mLabel = 0;
	}

	if (label == 0) {
		return;
	}
	
	const char* iterator;
	for (iterator = label; *iterator; ++iterator);
	unsigned int length = (unsigned int)(iterator - label);

	if (length == 0) {
		return;
	}
	mLabel = (char*)Animation::Allocate(sizeof(char) * (length + 1));
	for (char* cpy = mLabel; *cpy = *label; ++label, ++cpy); // StrCpy
	mLabel[length] = '\0';
}

float Animation::Data::Sample(State& out, float time, bool looping) const {
	// Adjust time to fit valid range for clip
	float startTime = GetStartTime();
	float endTime = GetEndtime();
	
	// Time is normalized to the animation duration, not an indevidual track
	if (looping) {
		float duration = GetDuration();
		if (duration <= 0) {
			return 0.0f;
		}

		// time = fmodf(time - startTime, endTime - startTime);
		float fmodA = time - startTime;
		float fmodB = endTime - startTime;
		int truncated = (int)(fmodA / fmodB);
		time = fmodA - ((float)truncated * fmodB);
		// End fmod

		while (time < startTime) {
			time += endTime - startTime;
		}
	}
	else {
		if (time < startTime) {
			time = startTime;
		}
		if (time > endTime) {
			time = endTime;
		}
	}
	
	// Loop trough all tracks
	unsigned int trackStride = 4;
	for (unsigned int i = 0; i < mTrackDataSize; i += trackStride) {
		unsigned int target = mTrackData[i + 0];
		unsigned int frameStride = 3 * 3 + 1;
		unsigned int frameSize = 3;
		if (mTrackData[i + 1] == (unsigned int)Component::Rotation) {
			frameStride = 3 * 4 + 1;
			frameSize = 4;
		}
		unsigned int offset = mTrackData[i + 2]; 
		unsigned int size = mTrackData[i + 3];
		
		unsigned int numFrames = size / frameSize;
		if (size % frameSize != 0) { // TODO: Remove
			int error = 1; // TODO: Remove
		}

		// Find current and next frames
		float trackStartTime = mFrameData[offset];
		float trackEndTime = mFrameData[offset + size - 1];
		unsigned int thisFrame = 0;
		
		float fmodA = time - trackStartTime;
		float fmodB = trackEndTime - trackStartTime;
		// t = fmodf(time - startTime, endTime - startTime); i didn't want to use the standard library here, it's just an fmod
		int trunc = (int)(fmodA / fmodB);
		float trackTime = fmodA - ((float)trunc * fmodB); // Time normalized to this track, not the animation
		
		float lookForError = trackTime; // TODO: Remove
		while (trackTime < fmodA) {
			trackTime += fmodB - fmodA;
			if (trackTime < lookForError) { // TODO: Remove
				lookForError = 9.0f; // Just here to set a breakpoint
			}
		}

		if (looping) {
			// TODO: Linear search is not optimal here. Replace with binary search, since time only ever grows
			for (unsigned int j = offset + size - 1; j >= offset; j -= frameStride) {
				if (trackTime >= mFrameData[j]) {
					thisFrame = j / frameStride;
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
		float* a = &mFrameData[thisFrame * frameStride + 1 + frameStride]; // TODO: Is this right? FrameStride or FrameSize? Also, cache?
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
void Animation::Data::SetRawData(const float* frameData, unsigned int frameSize, const unsigned int* trackData, unsigned int trackSize) {
	// Set frame data
	if (frameData == 0 || frameSize == 0) {
		if (mFrameData != 0) {
			Animation::Free(mFrameData);
		}
		mFrameData = 0;
		mFrameDataSize = 0;
	}
	else {
		if (frameSize != mFrameDataSize) {
			if (mFrameData != 0) {
				Animation::Free(mFrameData);
			}
			mFrameData = (float*)Animation::Allocate(sizeof(float) * frameSize);
			mFrameDataSize = frameSize;
		}

		for (unsigned int i = 0; i < frameSize; ++i) {
			mFrameData[i] = frameData[i];
		}
	}

	// Set track data
	if (trackData == 0 || trackSize == 0) {
		if (mTrackData != 0) {
			Animation::Free(mTrackData);
		}
		mTrackData = 0;
		mTrackDataSize = 0;
		return;
	}
	else {
		if (trackSize != mTrackDataSize) {
			if (mTrackData != 0) {
				Animation::Free(mTrackData);
			}
			mTrackData = (unsigned int*)Animation::Allocate(sizeof(unsigned int) * trackSize);
			mTrackDataSize = trackSize;
		}

		for (unsigned int i = 0; i < trackSize; ++i) {
			mTrackData[i] = trackData[i];
		}
	}

	// Figure out start and end times
	if (mFrameData != 0 && mTrackData != 0) {
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
}
#pragma warning(pop)

void Animation::Data::SerializeToString(char* output) const {
	char* last = output;
	unsigned int written = 0;
	unsigned int delta = 0;

	output = WriteUInt(output, mFrameDataSize);
	output = WriteNewLine(output);

	for (unsigned int i = 0; i < mFrameDataSize; ++i) {
		output = WriteFloat(output, mFrameData[i]);
	}
	output = WriteNewLine(output);

	output = WriteUInt(output, mTrackDataSize);
	output = WriteNewLine(output);
	for (unsigned int i = 0; i < mTrackDataSize; ++i) {
		output = WriteUInt(output, mTrackData[i]);
	}
	output = WriteNewLine(output);

	output = WriteFloat(output, mStartTime);
	output = WriteFloat(output, mEndTime);
	output = WriteNewLine(output);

	unsigned int labelLength = 0;
	if (mLabel != 0) {
		char* it = mLabel;
		while (*it != '\0') {
			labelLength += 1;
			it += 1;
		}
	}
	output = WriteUInt(output, labelLength);

	for (unsigned int i = 0; i < labelLength; ++i) {
		*output = mLabel[i];
		output += 1;
	}

	*output = '\0';
}

unsigned int Animation::Data::SerializedStringLength() const {
	unsigned int size = 0;

	size += UIntStringLength(mFrameDataSize) + 1;
	size += 1;

	for (unsigned int i = 0; i < mFrameDataSize; ++i) {
		size += FloatStringLength(mFrameData[i]) + 1;
	}
	size += 1;

	size += UIntStringLength(mTrackDataSize) + 1;
	size += 1;
	for (unsigned int i = 0; i < mTrackDataSize; ++i) {
		size += UIntStringLength(mTrackData[i]) + 1;
	}
	size += 1;

	size += FloatStringLength(mStartTime) + 1;
	size += FloatStringLength(mEndTime) + 1;
	size += 1;

	unsigned int labelLength = 0;
	if (mLabel != 0) {
		char* it = mLabel;
		while (*it != '\0') {
			labelLength += 1;
			it += 1;
		}
	}
	size += UIntStringLength(labelLength) + 1;

	for (unsigned int i = 0; i < labelLength; ++i) {
		size += 1;
	}

	size += 1;

	return size;
}

void Animation::Data::DeSerializeFromString(const char* input) {
	input = IgnoreUntilNumber(input);
	input = ReadUInt(input, mFrameDataSize);
	if (mFrameData != 0) {
		Free(mFrameData);
	}
	mFrameData = (float*)Allocate(sizeof(float) * mFrameDataSize);

	for (unsigned int i = 0; i < mFrameDataSize; ++i) {
		input = ReadFloat(input, mFrameData[i]);
	}

	input = ReadUInt(input, mTrackDataSize);
	if (mTrackData != 0) {
		Free(mTrackData);
	}
	mTrackData = (unsigned int*)Allocate(sizeof(unsigned int) * mTrackDataSize);
	for (unsigned int i = 0; i < mTrackDataSize; ++i) {
		input = ReadUInt(input, mTrackData[i]);
	}

	input = ReadFloat(input, mStartTime);
	input = ReadFloat(input, mEndTime);

	unsigned int labelLength = 0;
	input = ReadUInt(input, labelLength);
	mLabel = (char*)Allocate(sizeof(char) * labelLength + 1);
	mLabel[labelLength] = '\0';

	for (unsigned int i = 0; i < labelLength; ++i) {
		mLabel[i] = *input;
		input += 1;
	}
}
