#ifndef _H_IGPUTIMER_
#define _H_IGPUTIMER_

namespace Renderer {
	class IGPUTimer { // TODO: Rename this to sample timer, and also somehow make the API better. Right now it's unclear how the timers work.
	private:
		IGPUTimer(const IGPUTimer&);
		IGPUTimer& operator=(const IGPUTimer&);
	protected:
		inline IGPUTimer() { }
		virtual inline ~IGPUTimer() { }
	public:
		virtual void StartSample() = 0;
		virtual void End() = 0;
		virtual float GetMilliseconds() const = 0;
	};
}

#endif