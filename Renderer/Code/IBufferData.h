#ifndef _H_IVERTEXBUFFER_
#define _H_IVERTEXBUFFER_

namespace Renderer {
	enum class BufferUsageType {
		Static = 0,
		Dynamic = 1
	};

	class IBufferData {
	protected:
		inline IBufferData() { } // Intended to be called by friend class
		IBufferData(const IBufferData&); // Disabled
		IBufferData& operator=(const IBufferData&); // Disabled
	public:
		virtual inline ~IBufferData() { }

		virtual void Set(unsigned int bytes, void* data, BufferUsageType type = BufferUsageType::Static) = 0;
		virtual void Update(unsigned int offset, unsigned int bytes, void* data) = 0;
		
		virtual BufferUsageType GetUsage() const = 0;
		virtual unsigned int Size() const = 0;
	};
}

#endif