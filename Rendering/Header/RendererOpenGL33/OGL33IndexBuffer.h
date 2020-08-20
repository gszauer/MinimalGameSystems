#ifndef _H_OGL33INDEXBUFFER_
#define _H_OGL33INDEXBUFFER_

#include "../Renderer/IIndexBuffer.h"

namespace Renderer {
	class OGL33IndexBuffer : public IIndexBuffer {
		friend class OGL33GraphicsDevice;
	protected:
		inline OGL33IndexBuffer(BufferUsageType usage) : IIndexBuffer(usage) {} 
		inline ~OGL33IndexBuffer() { }
	public:
		inline void SetUInt(unsigned int* inputArray, unsigned int arrayLength) {}
		inline void SetUShort(unsigned short* inputArray, unsigned int arrayLength) {}
		inline BufferUsageType GetUsage() const { return BufferUsageType::Static; }
		inline void SetUsage(BufferUsageType usage) {}
		inline IndexBufferDataType GetDataType() const { return IndexBufferDataType::UInt; }
		inline unsigned int Size() { return 0; }
		inline IBufferType GetBufferType() const { return IBufferType::ElementArray; }
	};
}

#endif