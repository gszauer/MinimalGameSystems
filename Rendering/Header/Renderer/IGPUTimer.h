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
		virtual void Initialize() = 0;
		virtual void Destroy() = 0;
		virtual bool IsValid() const = 0;

		virtual void SampleStart() = 0;
		virtual void SampleEnd() = 0;
		virtual bool IsDone() const = 0;
		virtual float GetMilliseconds() const = 0;
	};
}

#endif