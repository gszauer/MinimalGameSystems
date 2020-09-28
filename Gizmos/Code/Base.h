#ifndef _H_GIZMOBASE_
#define _H_GIZMOBASE_

namespace Gizmos {
	namespace Internal {
		extern float gParallelEpsilon;
		extern float gTranslationInteractionThreshold;

		class Base {
		protected:
			float mOrientation[4]; // For local / global mode, even position and scale need to know the orientation
			bool mMouseWasDown; // = false
			
			float mTarget[4]; // Either an axis or a plane
			bool mTargetIsAxis; // = false

			bool mIsActive; // = false, true only if the gizmo is engaged

			// Camera params are cached at Update time and re-used at render time
			float mCameraPosition[3];
			float mCameraForward[3];
			float mCameraUp[3];
		protected:
			virtual bool OnClick(const float* rayOrigin, const float* rayDirection) = 0;
			virtual void OnDrag(const float* rayOrigin, const float* rayDirection) = 0;
			virtual void OnRelease(const float* rayOrigin, const float* rayDirection) = 0;
		public:
			inline void Update(unsigned int mouseX, unsigned int mouseY, bool mouseDown, const float* mvpMat) {
				float pickRayOrigin[3]; // TODO
				float pickRayDirection[3]; // TODO

				// TODO: Make sure pickRayDirection is normalized!

				if (!mMouseWasDown && mouseDown) { // Click
					mIsActive = OnClick(pickRayOrigin, pickRayDirection);
				}
				else if (mMouseWasDown && mouseDown) { // Drag
					if (mIsActive) {
						OnDrag(pickRayOrigin, pickRayDirection);
					}
				}
				else if (mMouseWasDown && !mouseDown) { // Release
					if (mIsActive) {
						OnRelease(pickRayOrigin, pickRayDirection);
					}
					mIsActive = false;
				}
				mMouseWasDown = mouseDown;
			}

			virtual void Render(float aspect) const = 0;
		};
	}
}

#endif