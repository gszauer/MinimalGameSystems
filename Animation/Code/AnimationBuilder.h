#ifndef _H_ANIMATIONDATABUILDER_
#define _H_ANIMATIONDATABUILDER_

#include "AnimationData.h"
#include "AnimationState.h"

namespace Animation {
	// TODO: Const-correct all of these!

	namespace Builder {
		struct Frame {
			float time;
			float in[4];
			float value[4];
			float out[4];

			Frame();
			Frame& operator=(const Frame& other);
		};

		class Clip;

		class Track {
			friend class Clip; // TODO: Can / should i undo this?
		protected:
			unsigned int mJointID;
			Animation::Data::Component mComponent;
			Frame* mFrameData;
			unsigned int mFrameCount;
			unsigned int mFrameCapacity;
		public:
			Track();
			Track(const Track&);
			Track& operator=(const Track&);
			Track(unsigned int jointID);
			Track(unsigned int jointID, Animation::Data::Component component);
			~Track();

			unsigned int GetJointID();
			void SetJointID(unsigned int id);

			Animation::Data::Component GetTarget();
			void SetTarget(Animation::Data::Component target);

			unsigned int Size();
			unsigned int Capacity();

			void Reserve(unsigned int numToReserve);
			void Resize(unsigned int newSize);

			Frame& operator[](unsigned int index);
			void PushFrame(const Frame& input);

			void ForceLinear(); // TODO: This is bad. Need to fix it
			void ForceStep();
		};

		class Clip {
		protected:
			char* mName;
			Track* mTracks;
			unsigned int mTrackCount;
			unsigned int mTrackCapacity;
		public:
			Clip();
			Clip(const char* name);
			Clip(const Clip&);
			Clip& operator=(const Clip&);
			~Clip();

			const char* GetName();
			void SetName(const char* newName);

			unsigned int Size();
			unsigned int Capacity();
			void Reserve(unsigned int numToReserve);
			void Resize(unsigned int newSize);

			//unsigned int GetNumUniqueTracks();

			Track& operator[](unsigned int index);
			void PushTrack(const Track& input);
		};

#if 0
		class Node {
		public:
			float position[3];
			float rotation[4];
			float scale[3];
		protected:
			Node* mParent;
			Node** mChildren;
			unsigned int mChildrenCount;
			unsigned int mChildrenCapacity;
		public:
			Node();
			Node(Node* parent);
			Node(float* pos, float* rot, float* scl);
			Node(Node* parent, float* pos, float* rot, float* scl);
			Node(const Node& other);
			Node& operator=(const Node& other);
			~Node();

			Node* GetParent();
			void SetParent(Node* parent);

			unsigned int Size();
			unsigned int Capacity();
			void Reserve(unsigned int numToReserve);

			Node* operator[](unsigned int index);
			void AddChild(Node* child);
			void RemoveChild(Node* child);
			void RemoveChild(unsigned int index);

			unsigned int SizeOfGraph();
		};
		Animation::State Convert(Animation::Builder::Node* nodes, unsigned int numNodes);
#endif
		Animation::Data Convert(Animation::Builder::Clip& clip);
	}
}

#endif