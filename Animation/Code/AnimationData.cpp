#include "AnimationData.h"
#include "AnimationState.h"
#include "AnimationHelpers.h"

namespace Animation {
	namespace Internal {
		class FrameView {
		protected:
			float* mBuffer;
			unsigned int mComponentSize;
			unsigned int mFrameSize;
			unsigned int mNumFrames;
		protected:
			inline void StepVec3(float* out, unsigned int left, unsigned int right, float t) const; // TODO
			inline void LinearVec3(float* out, unsigned int left, unsigned int right, float t) const {
				const float* valueLeft = &mBuffer[left * mFrameSize + 1 + mComponentSize];
				const float* valueRight = &mBuffer[right * mFrameSize + 1 + mComponentSize];

				if (mComponentSize >= 1) {
					out[0] = valueLeft[0] + (valueRight[0] - valueLeft[0]) * t;
				}
				if (mComponentSize >= 2) {
					out[1] = valueLeft[1] + (valueRight[1] - valueLeft[1]) * t;
				}
				if (mComponentSize >= 3) {
					out[2] = valueLeft[2] + (valueRight[2] - valueLeft[2]) * t;
				}
				if (mComponentSize >= 4) {
					out[3] = valueLeft[3] + (valueRight[3] - valueLeft[3]) * t;
				}
			}
			inline void CubicVec3(float* out, unsigned int left, unsigned int right, float t) const; // TODO

			inline void StepQuat(float* out, unsigned int left, unsigned int right, float t) const; // TODO

			inline void LinearQuat(float* out, unsigned int left, unsigned int right, float t) const {
				// Neighborhood
				const float* valueRight = &mBuffer[right * mFrameSize + 1 + mComponentSize];
				float valueLeft[4] = { 0.0f };
				float delta[4] = { 0.0f };

				float neighborhood = 0.0f;
				const float* originalLeft = &mBuffer[left * mFrameSize + 1 + mComponentSize];

				if (mComponentSize >= 1) {
					neighborhood = originalLeft[0] * originalLeft[0];
					valueLeft[0] = originalLeft[0];
					delta[0] = originalLeft[0] - valueRight[0];
				}
				if (mComponentSize >= 2) {
					neighborhood = originalLeft[0] * originalLeft[0] + originalLeft[1] * originalLeft[1];
					valueLeft[1] = originalLeft[1];
					delta[1] = originalLeft[1] - valueRight[1];
				}
				if (mComponentSize >= 3) {
					neighborhood = originalLeft[0] * originalLeft[0] + originalLeft[1] * originalLeft[1] + originalLeft[2] * originalLeft[2];
					valueLeft[2] = originalLeft[2];
					delta[2] = originalLeft[1] - valueRight[2];
				}
				if (mComponentSize >= 4) {
					neighborhood = originalLeft[0] * originalLeft[0] + originalLeft[1] * originalLeft[1] + originalLeft[2] * originalLeft[2] + originalLeft[3] * originalLeft[3];
					valueLeft[3] = originalLeft[3];
					delta[3] = originalLeft[3] - valueRight[3];
				}

				if (neighborhood < 0.0f) {
					valueLeft[0] = -valueLeft[0];
					valueLeft[1] = -valueLeft[1];
					valueLeft[2] = -valueLeft[2];
					valueLeft[3] = -valueLeft[3];
				}

				// Iterplate
				if (mComponentSize >= 1) {
					out[0] = valueLeft[0] + (valueRight[0] - valueLeft[0]) * t;
				}
				if (mComponentSize >= 2) {
					out[1] = valueLeft[1] + (valueRight[1] - valueLeft[1]) * t;
				}
				if (mComponentSize >= 3) {
					out[2] = valueLeft[2] + (valueRight[2] - valueLeft[2]) * t;
				}
				if (mComponentSize >= 4) {
					out[3] = valueLeft[3] + (valueRight[3] - valueLeft[3]) * t;
				}

				// Normalize
				float lenSq = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2] + delta[3] * delta[3];
				if (lenSq > 0.0f) {
					float invLen = Animation::InvSqrt(lenSq);
					if (mComponentSize >= 1) {
						out[0] *= invLen;
					}
					if (mComponentSize >= 2) {
						out[1] *= invLen;
					}
					if (mComponentSize >= 3) {
						out[2] *= invLen;
					}
					if (mComponentSize >= 4) {
						out[3] *= invLen;
					}
				}
			}
			inline void CubicQuat(float* out, unsigned int left, unsigned int right, float t) const; // TODO
		private:
			FrameView();
			FrameView(const FrameView&);
			FrameView& operator=(const FrameView&);
		public:
			inline FrameView(float* buffer, unsigned int component, unsigned int size) {
				mBuffer = buffer;
				mComponentSize = component;
				mNumFrames = size;

				mFrameSize = 1; // time
				mFrameSize += mComponentSize; // in
				mFrameSize += mComponentSize; // value
				mFrameSize += mComponentSize; // out
			}

			inline ~FrameView() { }

			inline const float* operator[](unsigned int index) const {
				return &mBuffer[index * mFrameSize];
			}

			inline float GetTime(unsigned int index) const {
				return mBuffer[index * mFrameSize];
			}

			inline const float* GetIn(unsigned int index) const {
				return &mBuffer[index * mFrameSize + 1];

			}
			
			inline const float* GetValue(unsigned int index) const {
				return &mBuffer[index * mFrameSize + 1 + mComponentSize];
			}

			inline const float* GetOut(unsigned int index) const {
				return &mBuffer[index * mFrameSize + 1 + mComponentSize + mComponentSize];
			}

			inline unsigned int GetNumComponents() const {
				return mComponentSize;
			}

			inline unsigned int GetNumFrames() const {
				return mNumFrames;
			}

			inline const float* GetBuffer() const {
				return mBuffer;
			}

			inline float GetStartTime() const {
				return mBuffer[0];
			}

			inline float GetEndTime() const {
				return mBuffer[mNumFrames * mFrameSize - mFrameSize];
			}

			inline void Vec3Interpolate(float* out, unsigned int left, unsigned int right, float t) const {
				// TODO: Figure out proper interpolation
				return LinearVec3(out, left, right, t);
			}

			inline void QuatInterpolate(float* out, unsigned int left, unsigned int right, float t) const {
				// TODO: Figure out proper interpolation
				return LinearQuat(out, left, right, t);
			}
		};
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

		time = startTime + Animation::FMod(time - startTime, endTime - startTime);

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
	for (unsigned int trackIndex = 0; trackIndex < mTrackDataSize; trackIndex += trackStride) {
		unsigned int targetJointId = mTrackData[trackIndex + 0];
		unsigned int frameStride = 3 * 3 + 1;
		unsigned int frameSize = 3;
		Animation::Data::Component trackComponent = (Animation::Data::Component)mTrackData[trackIndex + 1];
		if (mTrackData[trackIndex + 1] == (unsigned int)Component::Rotation) {
			frameStride = 3 * 4 + 1;
			frameSize = 4;
		}
		unsigned int offset = mTrackData[trackIndex + 2];
		unsigned int size = mTrackData[trackIndex + 3];
		unsigned int numFrames = size / frameStride;

		Animation::Internal::FrameView frameView(&mFrameData[offset], frameSize, numFrames);
		
		// Find current and next frames
		float trackStartTime = frameView.GetStartTime();
		float trackEndTime = frameView.GetEndTime();
		float trackTime = trackStartTime + Animation::FMod(time - trackStartTime, trackEndTime - trackStartTime);

		unsigned int thisFrame = 0;
		if (looping) { // TODO: Linear search is not optimal here. Replace with binary search, since time only ever grows
			for (int frameIndex = frameView.GetNumFrames() - 1; frameIndex >= 0; frameIndex -= 1) {
				if (trackTime >= frameView.GetTime(frameIndex)) {
					thisFrame = frameIndex;
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
		float frameDelta = frameView.GetTime(nextFrame) - frameView.GetTime(thisFrame);
		float t = 0.0f;
		if (frameDelta > 0.0f) {
			t = (trackTime - frameView.GetTime(thisFrame)) / frameDelta;
		}

		float result[4] = { 0.0f };
		if (trackComponent == Component::Position) {
			frameView.Vec3Interpolate(result, thisFrame, nextFrame, t);
			out.SetRelativePosition(targetJointId, result);
		}
		else if (trackComponent == Component::Rotation) {
			frameView.QuatInterpolate(result, thisFrame, nextFrame, t);
			out.SetRelativeRotation(targetJointId, result);
		}
		else {
			frameView.Vec3Interpolate(result, thisFrame, nextFrame, t);
			out.SetRelativeScale(targetJointId, result);
		}
	}

	// Return time relative to valid play range
	return time;
}

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
			mFrameData = (float*)Animation::Allocate(((unsigned int)sizeof(float)) * frameSize);
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
			mTrackData = (unsigned int*)Animation::Allocate(((unsigned int)sizeof(unsigned int)) * trackSize);
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
			unsigned int component = mTrackData[i + 1];
			unsigned int offset = mTrackData[i + 2];
			unsigned int size = mTrackData[i + 3];

			float trackStartTime = mFrameData[offset];
			unsigned int frameDataStride = (component == (unsigned int)Animation::Data::Component::Rotation) ? 13 : 10;
			float trackEndTime = mFrameData[offset + size - frameDataStride];

			if (trackStartTime < mStartTime) {
				mStartTime = trackStartTime;
			}
			if (trackEndTime > mEndTime) {
				mEndTime = trackEndTime;
			}
		}
	}
}

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
