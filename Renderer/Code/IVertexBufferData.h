#ifndef _H_IVERTEXBUFFERDATA_
#define _H_IVERTEXBUFFERDATA_

#include "IVertexBufferView.h"

namespace Renderer {
	enum class VertexBufferUsageType { // TODO: Refactor to VertexBufferUsageType
		Static = 0,
		Dynamic = 1
	};

	class IGraphicsDevice;

	class IVertexBufferData {
	protected:
		IVertexBufferData(); // Disabled
		IVertexBufferData(const IVertexBufferData&); // Disabled
		IVertexBufferData& operator=(const IVertexBufferData&); // Disabled
		inline IVertexBufferData(const IGraphicsDevice&) { }
	public:
		virtual inline ~IVertexBufferData() { }

		virtual void Set(unsigned int bytes, const void* data, VertexBufferUsageType type = VertexBufferUsageType::Static) = 0;
		virtual void Update(unsigned int offset, unsigned int bytes, const void* data) = 0;
		
		virtual VertexBufferUsageType GetUsage() const = 0;
		virtual unsigned int Size() const = 0;

		virtual const IVertexBufferView* CreateView(unsigned int numComponents = 1, VertexBufferDataType type = VertexBufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0) const = 0;
		virtual void DestroyView(const IVertexBufferView* view) const = 0;

		virtual const IGraphicsDevice* GetOwner() const = 0;
	};
}

#endif