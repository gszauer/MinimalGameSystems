#ifndef _H_IGPUTIMER_
#define _H_IGPUTIMER_

namespace Renderer {
	class IGPUTimer {
	private:
		IGPUTimer(const IGPUTimer&);
		IGPUTimer& operator=(const IGPUTimer&);
	protected:
		inline IGPUTimer() { }
		virtual inline ~IGPUTimer() { }
	public:
		virtual void Start() = 0;
		virtual void End() = 0;
		virtual float GetMilliseconds() const = 0;
	};
}

#endif