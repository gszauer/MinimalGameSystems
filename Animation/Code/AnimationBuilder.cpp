#include "AnimationBuilder.h"
#include "AnimationHelpers.h"

Animation::Builder::Frame::Frame() {
	time = 0.0f;
	in[0] = in[1] = in[2] = in[3] = 0.0f;
	value[0] = value[1] = value[2] = 0.0f;
	value[3] = 1.0f;
	out[0] = out[1] = out[2] = out[3] = 0.0f;
}

Animation::Builder::Frame& Animation::Builder::Frame::operator=(const Frame& other) {
	if (this == &other) {
		return *this;
	}

	time = other.time;

	in[0] = other.in[0];
	in[1] = other.in[1];
	in[2] = other.in[2];
	in[3] = other.in[3];

	value[0] = other.value[0];
	value[1] = other.value[1];
	value[2] = other.value[2];
	value[3] = other.value[3];

	out[0] = other.out[0];
	out[1] = other.out[1];
	out[2] = other.out[2];
	out[3] = other.out[3];

	return *this;
}

Animation::Builder::Track::Track() {
	mJointID = 0;
	mComponent = Animation::Data::Component::Unset;
	mFrameData = 0;
	mFrameCount = 0;
	mFrameCapacity = 0;
}

Animation::Builder::Track::Track(const Track& other) {
	mJointID = 0;
	mComponent = Animation::Data::Component::Unset;
	mFrameData = 0;
	mFrameCount = 0;
	mFrameCapacity = 0;

	*this = other;
}

Animation::Builder::Track& Animation::Builder::Track::operator=(const Track& other) {
	if (this == &other) {
		return *this;
	}

	mJointID = other.mJointID;
	mComponent = other.mComponent;

	if (mFrameCapacity != other.mFrameCapacity) {
		Reserve(other.mFrameCapacity);
	}
	mFrameCapacity = other.mFrameCapacity;
	mFrameCount = other.mFrameCount;

	for (unsigned int i = 0; i < mFrameCount; ++i) {
		mFrameData[i] = other.mFrameData[i];
	}


	return *this;
}

Animation::Builder::Track::Track(unsigned int jointID) {
	mJointID = jointID;
	mComponent = Animation::Data::Component::Unset;
	mFrameData = 0;
	mFrameCount = 0;
	mFrameCapacity = 0;
}

Animation::Builder::Track::Track(unsigned int jointID, Animation::Data::Component component) {
	mJointID = jointID;
	mComponent = component;
	mFrameData = 0;
	mFrameCount = 0;
	mFrameCapacity = 0;
}

Animation::Builder::Track::~Track() {
	if (mFrameData != 0) {
		Animation::Free(mFrameData);
	}
}

unsigned int Animation::Builder::Track::GetJointID() {
	return mJointID;
}

void Animation::Builder::Track::SetJointID(unsigned int id) {
	mJointID = id;
}

Animation::Data::Component Animation::Builder::Track::GetTarget() {
	return mComponent;
}

void Animation::Builder::Track::SetTarget(Animation::Data::Component target) {
	mComponent = target;
}

unsigned int Animation::Builder::Track::Size() {
	return mFrameCount;
}

unsigned int Animation::Builder::Track::Capacity() {
	return mFrameCapacity;
}


void Animation::Builder::Track::Reserve(unsigned int numToReserve) {
	if (mFrameCapacity < numToReserve) {
		Frame* newFrames = (Frame*)Animation::Allocate(sizeof(Frame) * numToReserve);

		for (unsigned int i = 0; i < mFrameCount; ++i) {
			newFrames[i] = mFrameData[i];
		}
		for (unsigned int i = mFrameCount; i < numToReserve; ++i) {
			newFrames[i] = Frame();
		}

		if (mFrameData != 0) {
			Animation::Free(mFrameData);
		}
		mFrameData = newFrames;
		mFrameCapacity = numToReserve;
	}
}

void Animation::Builder::Track::Resize(unsigned int newSize) {
	Frame* newFrames = 0;
	if (newSize == 0) {
		if (mFrameData != 0) {
			Animation::Free(mFrameData);
		}
	}
	else if (newSize == mFrameCount && newSize == mFrameCapacity) {
		newFrames = mFrameData;
	}
	else if (newSize < mFrameCount) {
		newFrames = (Frame*)Animation::Allocate(sizeof(Frame) * newSize);
		for (unsigned int i = 0; i < newSize; ++i) {
			newFrames[i] = mFrameData[i];
		}
		if (mFrameData != 0) {
			Animation::Free(mFrameData);
		}
	}
	else { // newSize > mFrameCount
		newFrames = (Frame*)Animation::Allocate(sizeof(Frame) * newSize);
		for (unsigned int i = 0; i < mFrameCount; ++i) {
			newFrames[i] = mFrameData[i];
		}
		for (unsigned int i = mFrameCount; i < newSize; ++i) {
			newFrames[i] = Frame();
		}
		if (mFrameData != 0) {
			Animation::Free(mFrameData);
		}
	}
	mFrameCount = mFrameCapacity = newSize;
	mFrameData = newFrames;
}

Animation::Builder::Frame& Animation::Builder::Track::operator[](unsigned int index) {
	return mFrameData[index];
}

void Animation::Builder::Track::PushFrame(const Frame& input) {
	if (mFrameCount == mFrameCapacity) {
		Reserve(mFrameCount + (mFrameCount / 2) + 10);
	}

	mFrameData[mFrameCount++] = input;
}

void Animation::Builder::Track::ForceLinear() {
	for (unsigned int i = 0; i < mFrameCount - 1; ++i) {
		float thisX = mFrameData[i].time;
		float nextX = mFrameData[i + 1].time;

		for (unsigned int j = 0; j < 4; ++j) {
			float thisY = mFrameData[i].value[j];
			float nextY = mFrameData[i + 1].value[j];
			
			// this.outTangent = normalized(next - this).x / normalized(next - this).y
			// next.inTangent = -outTangent

			float deltaX = nextX - thisX;
			float deltaY = nextY - thisY;

			float lenSq = deltaX * deltaX + deltaY * deltaY;
			if (!FloatCompare(lenSq, 0.0f)) {
				float invLen = InvSqrt(lenSq);
				deltaX *= invLen;
				deltaY *= invLen;
			}

			float tangent = 0.0f;
			if (!FloatCompare(deltaY, 0.0f)) {
				tangent = deltaX / deltaY;
			}

			mFrameData[i].out[j] = tangent;
			mFrameData[i].in[j] = -tangent;
		}
	}
}

void Animation::Builder::Track::ForceStep() {
	for (unsigned int i = 0; i < mFrameCount; ++i) {
		mFrameData[i].in[0] = 2000000.0f;
		mFrameData[i].in[1] = 2000000.0f;
		mFrameData[i].in[2] = 2000000.0f;
		mFrameData[i].in[3] = 2000000.0f;

		mFrameData[i].out[0] = 2000000.0f;
		mFrameData[i].out[1] = 2000000.0f;
		mFrameData[i].out[2] = 2000000.0f;
		mFrameData[i].out[3] = 2000000.0f;
	}
}

Animation::Builder::Clip::Clip() {
	mName = 0;
	mTracks = 0;
	mTrackCount = 0;
	mTrackCapacity = 0;
}

Animation::Builder::Clip::Clip(const char* name) {
	mName = 0;
	mTracks = 0;
	mTrackCount = 0;
	mTrackCapacity = 0;

	SetName(name);
}

Animation::Builder::Clip::Clip(const Clip& other) {
	mName = 0;
	mTracks = 0;
	mTrackCount = 0;
	mTrackCapacity = 0;

	*this = other;
}

Animation::Builder::Clip::~Clip() {
	if (mName != 0) {
		Animation::Free(mName);
	}

	if (mTracks != 0) {
		Animation::Free(mTracks);
	}
}

const char* Animation::Builder::Clip::GetName() {
	return mName;
}

unsigned int Animation::Builder::Clip::Size() {
	return mTrackCount;
}

unsigned int Animation::Builder::Clip::Capacity() {
	return mTrackCapacity;
}

Animation::Builder::Track& Animation::Builder::Clip::operator[](unsigned int index) {
	return mTracks[index];
}

void Animation::Builder::Clip::PushTrack(const Track& input) {
	if (mTrackCount == mTrackCapacity) {
		Reserve(mTrackCount + (mTrackCapacity / 2) + 10);
	}

	mTracks[mTrackCount++] = input;
}

Animation::Builder::Clip& Animation::Builder::Clip::operator=(const Clip& other) {
	if (this == &other) {
		return *this;
	}

	SetName(other.mName);

	if (mTrackCapacity != other.mTrackCapacity) {
		Reserve(other.mTrackCapacity);
	}
	mTrackCapacity = other.mTrackCapacity;
	mTrackCount = other.mTrackCount;

	for (unsigned int i = 0; i < mTrackCount; ++i) {
		mTracks[i] = other.mTracks[i];
	}

	return *this;
}

void Animation::Builder::Clip::SetName(const char* newName) {
	if (mName != 0) {
		Animation::Free(mName);
		mName = 0;
	}

	if (newName != 0) {
		unsigned int len = 0;
		for (const char* c = newName; *c != '\0'; c += 1, len += 1);
		if (len != 0) {
			mName = (char*)Allocate((unsigned int)(sizeof(char)) * (len + 1));
			for (unsigned int i = 0; i < len; ++i) {
				mName[i] = newName[i];
			}
			mName[len] = '\0';
		}
	}
}

void Animation::Builder::Clip::Reserve(unsigned int numToReserve) {
	if (mTrackCapacity < numToReserve) {
		Track* newTracks = (Track*)Animation::Allocate(sizeof(Track) * numToReserve);

		for (unsigned int i = 0; i < mTrackCount; ++i) {
			newTracks[i] = mTracks[i];
		}
		for (unsigned int i = mTrackCount; i < numToReserve; ++i) {
			newTracks[i] = Track();
		}

		if (mTracks != 0) {
			Animation::Free(mTracks);
		}
		mTracks = newTracks;
		mTrackCapacity = numToReserve;
	}
}

void Animation::Builder::Clip::Resize(unsigned int newSize) {
	Track* newTracks = 0;
	if (newSize == 0) {
		if (mTracks != 0) {
			Animation::Free(mTracks);
		}
	}
	else if (newSize == mTrackCount && newSize == mTrackCapacity) {
		newTracks = mTracks;
	}
	else if (newSize < mTrackCount) {
		newTracks = (Track*)Animation::Allocate(sizeof(Track) * newSize);
		for (unsigned int i = 0; i < newSize; ++i) {
			newTracks[i] = mTracks[i];
		}
		if (mTracks != 0) {
			Animation::Free(mTracks);
		}
	}
	else { // newSize > mTrackCount
		newTracks = (Track*)Animation::Allocate(sizeof(Track) * newSize);
		for (unsigned int i = 0; i < mTrackCount; ++i) {
			newTracks[i] = mTracks[i];
		}
		for (unsigned int i = mTrackCount; i < newSize; ++i) {
			newTracks[i] = Track();
		}
		if (mTracks != 0) {
			Animation::Free(mTracks);
		}
	}
	mTrackCount = mTrackCapacity = newSize;
	mTracks = newTracks;
}

Animation::Data Animation::Builder::Convert(Animation::Builder::Clip& clip) {
	Animation::Data result;

	const char* label = clip.GetName();
	result.SetLabel(label);

	unsigned int* trackData = 0;
	unsigned int trackDataSize = 0;
	unsigned int frameDataSize = 0;

	trackDataSize = clip.Size() * 4;
	trackData = (unsigned int*)Animation::Allocate(((unsigned int)sizeof(unsigned int)) * trackDataSize);
	for (unsigned int i = 0; i < clip.Size(); ++i) {
		Animation::Builder::Track& track = clip[i];
		trackData[i * 4 + 0] = track.GetJointID();
		trackData[i * 4 + 1] = (unsigned int)track.GetTarget();
		trackData[i * 4 + 2] = frameDataSize;
		trackData[i * 4 + 3] = track.Size() * (trackData[i * 4 + 1] == 2? 13 : 10);
		frameDataSize += trackData[i * 4 + 3];
	}

	float* frameData = (float*)Animation::Allocate(((unsigned int)sizeof(float)) * frameDataSize);
	unsigned int writeIndex = 0;

	for (unsigned int trackIndex = 0; trackIndex < clip.Size(); ++trackIndex) {
		Animation::Builder::Track& track = clip[trackIndex];

		for (unsigned int frameIndex = 0; frameIndex < track.Size(); ++frameIndex) {
			Animation::Builder::Frame& frame = track[frameIndex];

			frameData[writeIndex++] = frame.time;
			frameData[writeIndex++] = frame.in[0];
			frameData[writeIndex++] = frame.in[1];
			frameData[writeIndex++] = frame.in[2];
			if (track.GetTarget() == Animation::Data::Component::Rotation) {
				frameData[writeIndex++] = frame.in[3];
			}
			frameData[writeIndex++] = frame.value[0];
			frameData[writeIndex++] = frame.value[1];
			frameData[writeIndex++] = frame.value[2];
			if (track.GetTarget() == Animation::Data::Component::Rotation) {
				frameData[writeIndex++] = frame.value[3];
			}
			frameData[writeIndex++] = frame.out[0];
			frameData[writeIndex++] = frame.out[1];
			frameData[writeIndex++] = frame.out[2];
			if (track.GetTarget() == Animation::Data::Component::Rotation) {
				frameData[writeIndex++] = frame.out[3];
			}
		}
	}
	
	result.SetRawData(frameData, frameDataSize, trackData, trackDataSize);
	return result;
}

#if 0
Animation::Builder::Node::Node() {
	position[0] = position[1] = position[2] = 0.0f;
	rotation[0] = rotation[1] = rotation[2] = 0.0f;
	rotation[3] = 1.0f;
	scale[0] = scale[1] = scale[2] = 1.0f;

	mParent = 0;
	mChildren = 0;
	mChildrenCapacity = 0;
	mChildrenCapacity = 0;
}

Animation::Builder::Node::Node(Node* parent) {
	position[0] = position[1] = position[2] = 0.0f;
	rotation[0] = rotation[1] = rotation[2] = 0.0f;
	rotation[3] = 1.0f;
	scale[0] = scale[1] = scale[2] = 1.0f;

	mParent = parent;
	mChildren = 0;
	mChildrenCapacity = 0;
	mChildrenCapacity = 0;
}

Animation::Builder::Node::Node(Node* parent, float* pos, float* rot, float* scl) {
	position[0] = pos[0];
	position[1] = pos[1];
	position[2] = pos[2];
	rotation[0] = rot[0];
	rotation[1] = rot[1];
	rotation[2] = rot[2];
	rotation[3] = rot[3];
	scale[0] = scl[0];
	scale[1] = scl[1];
	scale[2] = scl[2];

	mParent = parent;
	mChildren = 0;
	mChildrenCapacity = 0;
	mChildrenCapacity = 0;
}

Animation::Builder::Node::Node(float* pos, float* rot, float* scl) {
	position[0] = pos[0];
	position[1] = pos[1];
	position[2] = pos[2];
	rotation[0] = rot[0];
	rotation[1] = rot[1];
	rotation[2] = rot[2];
	rotation[3] = rot[3];
	scale[0] = scl[0];
	scale[1] = scl[1];
	scale[2] = scl[2];

	mParent = 0;
	mChildren = 0;
	mChildrenCapacity = 0;
	mChildrenCapacity = 0;
}

Animation::Builder::Node::Node(const Node& other) {
	position[0] = position[1] = position[2] = 0.0f;
	rotation[0] = rotation[1] = rotation[2] = 0.0f;
	rotation[3] = 1.0f;
	scale[0] = scale[1] = scale[2] = 1.0f;

	mParent = 0;
	mChildren = 0;
	mChildrenCapacity = 0;
	mChildrenCapacity = 0;

	*this = other;
}

Animation::Builder::Node& Animation::Builder::Node::operator=(const Node& other) {
	if (this == &other) {
		return *this;
	}

	if (mChildren != 0) {
		Animation::Free(mChildren);
		mChildren = 0;
	}

	position[0] = other.position[0];
	position[1] = other.position[1];
	position[2] = other.position[2];

	rotation[0] = other.rotation[0];
	rotation[1] = other.rotation[1];
	rotation[2] = other.rotation[2];
	rotation[3] = other.rotation[3];

	scale[0] = other.scale[0];
	scale[1] = other.scale[1];
	scale[2] = other.scale[2];

	mParent = other.mParent;

	mChildrenCapacity = 0;
	mChildrenCount = 0;
	Reserve(other.mChildrenCapacity);
	mChildrenCount = other.mChildrenCount;

	for (unsigned int i = 0; i < mChildrenCount; ++i) {
		mChildren[i] = other.mChildren[i];
	}
	for (unsigned int i = mChildrenCount; i < mChildrenCapacity; ++i) {
		mChildren[i] = 0;
	}

	return *this;
}

Animation::Builder::Node::~Node() {
	if (mChildren != 0) {
		Animation::Free(mChildren);
	}
}

Animation::Builder::Node* Animation::Builder::Node::GetParent() {
	return mParent;
}

void Animation::Builder::Node::SetParent(Node* parent) {
	mParent = parent;
}

unsigned int Animation::Builder::Node::Size() {
	return mChildrenCount;
}

unsigned int Animation::Builder::Node::Capacity() {
	return mChildrenCapacity;
}

void Animation::Builder::Node::Reserve(unsigned int numToReserve) {
	if (mChildrenCapacity < numToReserve) {
		Node** newChildren = (Node**)Animation::Allocate(sizeof(Node*) * numToReserve);

		for (unsigned int i = 0; i < mChildrenCount; ++i) {
			newChildren[i] = mChildren[i];
		}
		for (unsigned int i = mChildrenCount; i < numToReserve; ++i) {
			newChildren[i] = 0;
		}

		if (mChildren != 0) {
			Animation::Free(mChildren);
		}
		mChildren = newChildren;
		mChildrenCapacity = numToReserve;
	}
}

Animation::Builder::Node* Animation::Builder::Node::operator[](unsigned int index) {
	return mChildren[index];
}

void Animation::Builder::Node::AddChild(Node* child) {
	if (child->mParent != 0) {
		child->mParent->RemoveChild(child);
	}

	if (mChildrenCount == mChildrenCapacity) {
		Reserve(mChildrenCapacity + (mChildrenCapacity / 2) + 1);
	}

	mChildren[mChildrenCount++] = child;
	child->mParent = this;
}

void Animation::Builder::Node::RemoveChild(Node* child) {
	if (mChildren == 0 || mChildrenCount == 0) {
		return;
	}

	unsigned int index = 0;
	for (; index < mChildrenCount; ++index) {
		if (mChildren[index] == child) {
			break;
		}
	}

	if (index >= mChildrenCount) {
		return;
	}

	RemoveChild(index);
}

void Animation::Builder::Node::RemoveChild(unsigned int index) {
	for (int i = (int)mChildrenCount - 1; i >= (int)index; --i) {
		mChildren[i] = mChildren[i - 1];
	}
	mChildrenCount -= 1;
}

unsigned int Animation::Builder::Node::SizeOfGraph() {
	unsigned int size = 1;

	for (unsigned int childIndex = 0; childIndex < mChildrenCount; ++childIndex) {
		size += mChildren[childIndex]->SizeOfGraph();
	}

	return size;
}

Animation::State Animation::Builder::Convert(Animation::Builder::Node* nodes, unsigned int numNodes) {
	unsigned int totalNodes = 0;

	for (unsigned int nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex) {
		Node& node = nodes[nodeIndex];
		totalNodes += node.SizeOfGraph();
	}

	Animation::State result;
	result.Resize(totalNodes);

	Node** 
}
#endif