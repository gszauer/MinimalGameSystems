#ifndef _H_TRANSLATION_
#define _H_TRANSLATION_

#include "Base.h"

namespace Gizmos {
	namespace Internal {
		void QuatMulVec3(float* result, const float* quat, const float* vec);
		float Vec3Dot(const float* v1, const float* v2);
		void Vec3Normalize(float* v);
		float Abs(float x);

		enum class TargetAxis {
			Forward = 1,
			Up = 2,
			Right = 4
		};
	}

	class Translation : public Internal::Base {
	protected:
		float mTranslation[3];
	protected:
		inline bool OnClick(const float* rayOrigin, const float* rayDirection) {
			float forward[3] = { 0.0f, 0.0f, 1.0f };
			float up[3] = { 0.0f, 1.0f, 0.0f };
			float right[3] = { 1.0f, 0.0f, 0.0f };

			Internal::QuatMulVec3(forward, mOrientation, forward);
			Internal::QuatMulVec3(up, mOrientation, up);
			Internal::QuatMulVec3(right, mOrientation, right);

			Internal::Vec3Normalize(forward);
			Internal::Vec3Normalize(up);
			Internal::Vec3Normalize(right);
			

			// TODO: View plane / all axis (and return early if it's that)


			/* Use this logic to figure out which axis to skip when finding ClosestAxis
			if (Internal::Abs(Internal::Vec3Dot(forward, mCameraForward)) > 1.0f - Internal::gParallelEpsilon) {
				mDisabled |= (unsigned int)Internal::TargetAxis::Forward;
			}
			if (Internal::Abs(Internal::Vec3Dot(up, mCameraForward)) > 1.0f - Internal::gParallelEpsilon) {
				mDisabled |= (unsigned int)Internal::TargetAxis::Up;
			}
			if (Internal::Abs(Internal::Vec3Dot(right, mCameraForward)) > 1.0f - Internal::gParallelEpsilon) {
				mDisabled |= (unsigned int)Internal::TargetAxis::Right;
			}
			*/

			float closestAxis[3]; // TODO: Get closest points on origin and axis. Turn those into segments. Find the shortest segment, it's the shortest axis. Respect disabled!
			float lineSegment[6]; // Get the line segment between ray and closest axis
			float lineSegmentLength = 0; // TODO

			if (lineSegmentLength < Internal::gTranslationInteractionThreshold) { // Interacting with closest Axis!
				mTargetIsAxis = true;
				mTarget[0] = closestAxis[0];
				mTarget[1] = closestAxis[1];
				mTarget[2] = closestAxis[2];
				mTarget[3] = 0.0f;
				return true;
			}

			// TODO: Interacting with a plane

			return false;
		}

		inline void OnDrag(const float* rayOrigin, const float* rayDirection) {

		}

		inline void OnRelease(const float* rayOrigin, const float* rayDirection) {

		}
	public:
		inline void Render(float aspect) const {

		}

	};
}

#endif