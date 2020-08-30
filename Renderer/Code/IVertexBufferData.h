#ifndef _H_IVERTEXBUFFERDATA_
#define _H_IVERTEXBUFFERDATA_

#include "IVertexBufferView.h"

namespace Renderer {
	enum class VertexBufferUsageType { // TODO: Refactor to VertexBufferUsageType
		Static = 0,
		Dynamic = 1
	};

	class IVertexBufferData {
	protected:
		inline IVertexBufferData() { }
		IVertexBufferData(const IVertexBufferData&); // Disabled
		IVertexBufferData& operator=(const IVertexBufferData&); // Disabled
	public:
		virtual inline ~IVertexBufferData() { }

		virtual void Set(unsigned int bytes, const void* data, VertexBufferUsageType type = BufferUsageType::Static) = 0;
		virtual void Update(unsigned int offset, unsigned int bytes, const void* data) = 0;
		
		virtual VertexBufferUsageType GetUsage() const = 0;
		virtual unsigned int Size() const = 0;

		virtual const IVertexBufferView* CreateView(unsigned int numComponents = 1, VertexBufferDataType type = VertexBufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0) const = 0;
	};
}

#endif