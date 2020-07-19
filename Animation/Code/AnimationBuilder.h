#ifndef _H_ANIMATIONDATABUILDER_
#define _H_ANIMATIONDATABUILDER_

#include "AnimationData.h"

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

		class Clip;

		class Track {
			friend class Clip;
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

			const char* GetName();
			void SetName(const char* newName);

			unsigned int Size();
			unsigned int Capacity();
			void Reserve(unsigned int numToReserve);
			void Resize(unsigned int newSize);

			Track& operator[](unsigned int index);
			void PushTrack(const Track& input);
		};

		Animation::Data Convert(Animation::Builder::Clip& clip);
		Animation::Builder::Clip Convert(Animation::Data& data);
	}
}

#endif