#ifndef _H_IGPUTIMER_
#define _H_IGPUTIMER_

namespace Renderer {
	class IGraphicsDevice;

	class IGPUTimer {
		friend class IGraphicsDevice;
	protected:
		IGPUTimer(); // Disabled
		IGPUTimer(const IGPUTimer&); // Disabled
		virtual IGPUTimer& operator=(const IGPUTimer&); // Disabled
		inline IGPUTimer(const IGraphicsDevice& owner) { }
	public:
		virtual inline ~IGPUTimer() { }
		
		virtual void Insert() = 0;
		virtual bool IsDone() = 0;
		
		virtual float operator-(const IGPUTimer& other) = 0;

		virtual const IGraphicsDevice* GetOwner() const = 0;
	};
}

#endif