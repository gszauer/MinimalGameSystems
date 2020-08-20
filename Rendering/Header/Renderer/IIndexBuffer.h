#ifndef _H_IINDEXBUFFER_
#define _H_IINDEXBUFFER_

namespace Renderer {
	enum class IndexBufferDataType {
		UShort = 0,
		UInt = 1,
	};

#ifndef _H_RENDERER_IBUFFERTYPE_
#define _H_RENDERER_IBUFFERTYPE_
	enum class IBufferType {
		Array = 0,
		ElementArray = 1
	};
#endif
#ifndef _H_RENDERER_BUFFERUSAGE_
#define _H_RENDERER_BUFFERUSAGE_
	enum class BufferUsageType {
		Static = 0,
		Dynamic = 1
	};
#endif

	class IIndexBuffer {
	private:
		IIndexBuffer();
		IIndexBuffer(const IIndexBuffer&);
		IIndexBuffer& operator=(const IIndexBuffer&);
	protected:
		inline IIndexBuffer(BufferUsageType usage) { }
		inline virtual ~IIndexBuffer() { }
	public:
		virtual void SetUInt(unsigned int* inputArray, unsigned int arrayLength) = 0;
		virtual void SetUShort(unsigned short* inputArray, unsigned int arrayLength) = 0;

		virtual BufferUsageType GetUsage() const = 0;
		virtual void SetUsage(BufferUsageType usage) = 0;

		virtual IndexBufferDataType GetDataType() const = 0;
		virtual unsigned int Size() = 0;

		virtual IBufferType GetBufferType() const = 0;
	};
}

#endif