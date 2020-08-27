#ifndef _H_OGL33TIMER_
#define _H_OGL33TIMER_

#include "../Renderer/IGPUTimer.h"

namespace Renderer {
	class OGL33GPUTimer : public IGPUTimer {
	public:
		inline void Initialize() { }
		inline void Destroy() { }
		inline bool IsValid() { return true; }

		inline void SampleStart() { }
		inline void SampleEnd() { }
		inline bool IsDone() { return true; }
		inline float GetMilliseconds() { return 0.0f; }
	};
}

#endif