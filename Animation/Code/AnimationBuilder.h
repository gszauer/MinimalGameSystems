#ifndef _H_ANIMATIONDATABUILDER_
#define _H_ANIMATIONDATABUILDER_

#include "AnimationData.h"
#include "AnimationState.h"

namespace Animation {
	namespace Builder {
		struct Frame {
			float time;
			float in[4];
			float value[4];
			float out[4];

			Frame();
			Frame& operator=(const Frame& other);
		};

		class Track {
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

			unsigned int GetJointID() const;
			void SetJointID(unsigned int id);

			Animation::Data::Component GetTarget() const;
			void SetTarget(Animation::Data::Component target);

			unsigned int Size() const;
			unsigned int Capacity() const;

			void Reserve(unsigned int numToReserve);
			void Resize(unsigned int newSize);

			Frame& operator[](unsigned int index);
			const Frame& operator[](unsigned int index) const;
			void PushFrame(const Frame& input);

			void ForceLinear();
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

			const char* GetName() const;
			void SetName(const char* newName);

			unsigned int Size() const;
			unsigned int Capacity() const;
			void Reserve(unsigned int numToReserve);
			void Resize(unsigned int newSize);

			Track& operator[](unsigned int index);
			const Track& operator[](unsigned int index) const;
			void PushTrack(const Track& input);
		};

		Animation::Data Convert(const Animation::Builder::Clip& clip);
	}
}

#endif