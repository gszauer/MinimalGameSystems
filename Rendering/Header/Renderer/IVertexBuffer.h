#ifndef _H_IVERTEXBUFFER_
#define _H_IVERTEXBUFFER_

namespace Renderer {
	enum class VertexBufferDataType {
		Float = 0,
		Int = 1,
		UInt = 2,
		Double = 3
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

	class IVertexBuffer {
	private:
		IVertexBuffer();
		IVertexBuffer(const IVertexBuffer&);
		IVertexBuffer& operator=(const IVertexBuffer&);
	protected:
		inline IVertexBuffer(BufferUsageType usage) {}
		virtual inline ~IVertexBuffer() { }
	public:
		virtual void SetFloat(unsigned int numComponents, float* inputArray, unsigned int arrayLength) = 0;
		virtual void SetDouble(unsigned int numComponents, double* inputArray, unsigned int arrayLength) = 0;
		virtual void SetInt(unsigned int numComponents, int* inputArray, unsigned int arrayLength) = 0;
		virtual void SetUInt(unsigned int numComponents, unsigned int* inputArray, unsigned int arrayLength) = 0;

		virtual BufferUsageType GetUsage() const = 0;
		virtual void SetUsage(BufferUsageType usage) = 0;

		virtual VertexBufferDataType GetDataType() const = 0;
		virtual unsigned int GetNumComponents() const = 0;
		virtual unsigned int Size() const = 0;

		virtual IBufferType GetBufferType() const = 0;
	};
}

#endif