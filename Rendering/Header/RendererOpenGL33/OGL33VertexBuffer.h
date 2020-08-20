#ifndef _H_OGL33VERTEXBUFFER_
#define _H_OGL33VERTEXBUFFER_

#include "../Renderer/IVertexBuffer.h"

namespace Renderer {
	class OGL33VertexBuffer : public IVertexBuffer {
		friend class OGL33GraphicsDevice;
	protected:
		inline OGL33VertexBuffer(BufferUsageType usage) : IVertexBuffer(usage) {}
		inline ~OGL33VertexBuffer() { }
	public:
		// TODO: Make an overloaded set of Set functions that works more like OpenGL (IE allows for stride and shit)

		inline void SetFloat(unsigned int numComponents, float* inputArray, unsigned int arrayLength) {}
		inline void SetDouble(unsigned int numComponents, double* inputArray, unsigned int arrayLength) {}
		inline void SetInt(unsigned int numComponents, int* inputArray, unsigned int arrayLength) {}
		inline void SetUInt(unsigned int numComponents, unsigned int* inputArray, unsigned int arrayLength) {}

		inline BufferUsageType GetUsage() const { return BufferUsageType::Static; }
		inline void SetUsage(BufferUsageType usage) {}
		inline VertexBufferDataType GetDataType() const { return VertexBufferDataType::Float; }
		inline unsigned int GetNumComponents() const { return 0; }
		inline unsigned int Size() const { return 0; }
		inline IBufferType GetBufferType() const { return IBufferType::Array; }
	};
}

#endif