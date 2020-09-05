#ifndef _H_OGL33TIMER_
#define _H_OGL33TIMER_

#include "../IGPUTimer.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33GraphicsDevice;

	class OGL33GPUTimer : public IGPUTimer {
	protected:
		GLuint mCounter;
	protected:
		const OGL33GraphicsDevice* mOwner;
		OGL33GPUTimer(const IGraphicsDevice* owner);
	public:
		~OGL33GPUTimer();

		void Insert();
		bool IsDone();
		float operator-(const IGPUTimer& other);

		const IGraphicsDevice* GetOwner() const;
	};
}

#endif