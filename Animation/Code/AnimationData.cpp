#include "AnimationData.h"
#include "AnimationState.h"
#include "AnimationInternal.h"

namespace Animation {
	float Data::StepLimit = 1000000.0f;
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

	return *this;
}

Animation::Data::~Data() {
	if (mFrameData != 0) {
		Animation::Internal::Free(mFrameData);
	}
	if (mTrackData != 0) {
		Animation::Internal::Free(mTrackData);
	}
	if (mLabel != 0) {
		Animation::Internal::Free(mLabel);
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

unsigned int Animation::Data::GetNumTracks() const {
	return mTrackDataSize / 4;
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
		Animation::Internal::Free(mLabel);
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
	mLabel = (char*)Animation::Internal::Allocate((unsigned int)sizeof(char) * (length + 1));
	for (char* cpy = mLabel; (*cpy = *label); ++label, ++cpy); // StrCpy
	mLabel[length] = '\0';
}

float Animation::Data::Sample(State& out, float time, bool looping) const {
	// Adjust time to fit valid range for clip
	float clipStartTime = GetStartTime();
	float clipEndTime = GetEndtime();
	float clipDuration = GetDuration();
	if (clipDuration <= 0) {
		return 0.0f;
	}
	
	// Time is normalized to the animation duration, not an indevidual track 
	// clipCurrentTime will be >= the smallest start time of all tracks AND
	// clipCurrentTime will be <= the largest end time of all tracks
	float clipTime = time;
	{
		if (looping) {
			clipTime = Animation::Internal::FMod(clipTime - clipStartTime, clipDuration) + clipStartTime;
			if (clipTime < clipStartTime) {
				clipTime += clipDuration;
			}
		}
		else {
			if (clipTime < clipStartTime) {
				clipTime = clipStartTime;
			}
			if (clipTime > clipEndTime) {
				clipTime = clipEndTime;
			}
		}
	}
	
	// Loop trough all tracks
	unsigned int numTracks = mTrackDataSize / 4; // 4 = track stride
	for (unsigned int trackIndex = 0; trackIndex < numTracks; ++trackIndex) {
		float result[4];
		
		SampleTrack(result, trackIndex, clipTime, looping);

		unsigned int targetJointId = mTrackData[trackIndex * 4 + 0];
		Animation::Data::Component trackComponent = (Animation::Data::Component)mTrackData[trackIndex * 4 + 1];
		if (trackComponent == Component::Position) {
			out.SetRelativePosition(targetJointId, result);
		}
		else if (trackComponent == Component::Rotation) {
			out.SetRelativeRotation(targetJointId, result);
		}
		else {
			out.SetRelativeScale(targetJointId, result);
		}
	}

	// Return time relative to valid play range
	return clipTime;
}

float Animation::Data::SampleTrack(float* out, unsigned int trackIndex, float time, bool looping) const {
	float trackTime = time;
	trackIndex = trackIndex * 4; // 4 = track stride

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
	float* frames = &mFrameData[offset];
	{
		// Find current and next frames
		float trackStartTime = frames[0];
		float trackEndTime = frames[numFrames * frameStride - frameStride];
		float trackDuration = trackEndTime - trackStartTime;
		if (trackDuration <= 0.0f) {
			return 0.0f;
		}

		if (looping) {
			trackTime = Animation::Internal::FMod(trackTime - trackStartTime, trackDuration) + trackStartTime;
			if (trackTime < trackStartTime) {
				trackTime += trackDuration;
			}
		}
		else {
			if (trackTime < trackStartTime) {
				trackTime = trackStartTime;
			}
			if (trackTime > trackEndTime) {
				trackTime = trackEndTime;
			}
		}

		// Given a time, find the current and next frame indices
		unsigned int thisFrame = 0;
		unsigned int nextFrame = 0;
		{
#if 0 // Not optimal linear search
			for (int frameIndex = frameView.GetNumFrames() - 1; frameIndex >= 0; frameIndex -= 1) {
				if (trackTime >= frameView.GetTime(frameIndex)) {
					thisFrame = frameIndex;
					break;
				}
			}
#else //Since time only ever increases, use a binary search
			int l = 0;
			int r = (int)numFrames;
			while (l <= r) {
				int m = l + (r - l) / 2;
				float thisFrameTime = frames[m * frameStride];

				// Found at last frame
				if (m == numFrames - 1) {
					if (trackTime >= thisFrameTime) {
						thisFrame = numFrames - 1;
						break;
					}
				}
				else {
					if (trackTime >= thisFrameTime && trackTime <= frames[(m + 1) * frameStride]) {
						thisFrame = m;
						break;
					}

				}

				// target is greater, ignore left half
				if (thisFrameTime < trackTime) {
					l = m + 1;
				}
				// target is greater, ignore right half
				else {
					r = m - 1;
				}
			}
#endif
			if (!looping) {
				if (trackTime <= trackStartTime) {
					thisFrame = 0;
					trackTime = trackStartTime;
				}
				if (trackTime >= trackEndTime) {
					thisFrame = numFrames - 1;
					trackTime = trackEndTime;
				}
			}

			// Keep nextFrame valid
			if (thisFrame >= numFrames - 1) {
				thisFrame = numFrames - 2; // -2 so thisFrame + 1 is a valid index
			}
			nextFrame = thisFrame + 1;
		}

		// Find frame delta and interpolation t
		float frameDelta = frames[nextFrame * frameStride] - frames[thisFrame * frameStride];
		float t = 0.0f;
		if (frameDelta > 0.0f) {
			t = (trackTime - frames[thisFrame * frameStride]) / frameDelta;
		}

		bool isQuaternion = trackComponent == Component::Rotation;
		{ // Interpolate
			const float* originalLeft = &frames[thisFrame * frameStride + 1 + frameSize];
			const float* originalRight = &frames[nextFrame * frameStride + 1 + frameSize];

			float valueLeft[4] = { originalLeft[0], originalLeft[1], originalLeft[2], originalLeft[3] };
			float valueRight[4] = { originalRight[0], originalRight[1], originalRight[2], originalRight[3] };

			float leftTime = frames[thisFrame * frameStride];
			float rightTime = frames[nextFrame * frameStride];

			const float* outTangentLeft = &frames[thisFrame * frameStride + 1 + frameSize + frameSize];
			const float* inTangentRight = &frames[nextFrame * frameStride + 1];

			if (isQuaternion) {
				float neighborhood = valueRight[0] * valueLeft[0] + valueRight[1] * valueLeft[1] + valueRight[2] * valueLeft[2] + valueRight[3] * valueLeft[3];

				if (neighborhood < 0.0f) {
					valueRight[0] = -valueRight[0];
					valueRight[1] = -valueRight[1];
					valueRight[2] = -valueRight[2];
					valueRight[3] = -valueRight[3];
				}
			}

			// Interpolate the actual frames
			{
				// For linear (and hermite)
				float frameDelta = rightTime - leftTime;

				// For hermite
				float tt = t * t;
				float ttt = tt * t;
				float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
				float h2 = -2.0f * ttt + 3.0f * tt;
				float h3 = ttt - 2.0f * tt + t;
				float h4 = ttt - tt;

				for (unsigned int component = 0; component < frameSize; ++component) {
					float valueDelta = valueRight[component] - valueLeft[component];
					float linearTangent = valueDelta / frameDelta;

					float outTangent = outTangentLeft[component];
					float inTangent = inTangentRight[component];

					bool stepOut = false;
					if (outTangent > 0.0f && outTangent >= Animation::Data::StepLimit) {
						stepOut = true;
					}
					if (outTangent < 0.0f && outTangent <= -Animation::Data::StepLimit) {
						stepOut = true;
					}

					bool stepIn = false;
					if (inTangent > 0.0f && inTangent >= Animation::Data::StepLimit) {
						stepIn = true;
					}
					if (inTangent < 0.0f && inTangent <= -Animation::Data::StepLimit) {
						stepIn = true;
					}

					if (stepOut || stepIn) {
						out[component] = valueLeft[component];
					}
					else if (Animation::Internal::FloatCompare(outTangent, linearTangent) && Animation::Internal::FloatCompare(inTangent, linearTangent)) {
						out[component] = valueLeft[component] + (valueRight[component] - valueLeft[component]) * t;
					}
					else { // Cubic
						float s1 = outTangent * frameDelta;
						float s2 = inTangent * frameDelta;
						out[component] = (valueLeft[component] * h1) + (valueRight[component] * h2) + (s1 * h3) + (s2 * h4);
					}
				}
			}

			if (isQuaternion) { // Normalize
				float lenSq = out[0] * out[0] + out[1] * out[1] + out[2] * out[2] + out[3] * out[3];
				if (!Animation::Internal::FloatCompare(lenSq, 1.0f)) {
					if (lenSq > 0.0f) {
						float invLen = Animation::Internal::InvSqrt(lenSq);
						out[0] *= invLen;
						out[1] *= invLen;
						out[2] *= invLen;
						out[3] *= invLen;
					}
				}
			}
		}
	}

	return trackTime;
}

void Animation::Data::SetPointers(float* frameData, unsigned int frameSize, unsigned int* trackData, unsigned int trackSize) {
	if (mFrameData != 0 && mFrameData != frameData) {
		Animation::Internal::Free(mFrameData);
	}
	mFrameData = frameData;
	mFrameDataSize = frameSize;

	if (mTrackData != 0 && mTrackData != trackData) {
		Animation::Internal::Free(mTrackData);
	}
	mTrackData = trackData;
	mTrackDataSize = trackSize;

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

	// Normalize quaternions
	if (mFrameData != 0 && mTrackData != 0) {
		unsigned int trackStride = 4;
		// Loop trough all tracks
		for (unsigned int i = 0; i < mTrackDataSize; i += trackStride) {
			unsigned int component = mTrackData[i + 1];
			if (component == (unsigned int)Animation::Data::Component::Rotation) {
				unsigned int offset = mTrackData[i + 2];
				unsigned int size = mTrackData[i + 3];

				unsigned int frameDataStride = 13;

				for (unsigned int j = 0; j < size / frameDataStride; ++j) {
					float* rot = &mFrameData[offset + (j * 13) + 1 + 4];

					float rotLenSq = rot[0] * rot[0] + rot[1] * rot[1] + rot[2] * rot[2] + rot[3] * rot[3];
					if (!Animation::Internal::FloatCompare(rotLenSq, 1.0f)) {
						if (rotLenSq > 0.0f) {
							float invRotLen = Animation::Internal::InvSqrt(rotLenSq);
							rot[0] *= rotLenSq;
							rot[1] *= rotLenSq;
							rot[2] *= rotLenSq;
							rot[3] *= rotLenSq;
						}
					}
				}
			}
		}
	}
}

void Animation::Data::Set(const float* frameData, unsigned int frameSize, const unsigned int* trackData, unsigned int trackSize) {
	// Set frame data
	if (frameData == 0 || frameSize == 0) {
		if (mFrameData != 0) {
			Animation::Internal::Free(mFrameData);
		}
		mFrameData = 0;
		mFrameDataSize = 0;
	}
	else {
		if (frameSize != mFrameDataSize) {
			if (mFrameData != 0) {
				Animation::Internal::Free(mFrameData);
			}
			mFrameData = (float*)Animation::Internal::Allocate(((unsigned int)sizeof(float)) * frameSize);
			mFrameDataSize = frameSize;
		}

		for (unsigned int i = 0; i < frameSize; ++i) {
			mFrameData[i] = frameData[i];
		}
	}

	// Set track data
	if (trackData == 0 || trackSize == 0) {
		if (mTrackData != 0) {
			Animation::Internal::Free(mTrackData);
		}
		mTrackData = 0;
		mTrackDataSize = 0;
		return;
	}
	else {
		if (trackSize != mTrackDataSize) {
			if (mTrackData != 0) {
				Animation::Internal::Free(mTrackData);
			}
			mTrackData = (unsigned int*)Animation::Internal::Allocate(((unsigned int)sizeof(unsigned int)) * trackSize);
			mTrackDataSize = trackSize;
		}

		for (unsigned int i = 0; i < trackSize; ++i) {
			mTrackData[i] = trackData[i];
		}
	}

	// Figure out start and end times and normalize quaternions in the set pointer function
	SetPointers(mFrameData, mFrameDataSize, mTrackData, mTrackDataSize);
}