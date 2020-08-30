#ifndef _H_OGL33VERTEXBUFFERDATA_
#define _H_OGL33VERTEXBUFFERDATA_

#include "../IVertexBufferData.h"
#include "OGL33Win32Loader.h"

namespace Renderer {
	class OGL33VertexBufferData : public IVertexBufferData {
		friend class OGL33GraphicsDevice;
	protected:
		VertexBufferUsageType mUsageType;
		unsigned int mSize;
		GLuint mBuffer;
	protected:
		OGL33VertexBufferData();
		OGL33VertexBufferData(const OGL33VertexBufferData&); // Disabled
		OGL33VertexBufferData& operator=(const OGL33VertexBufferData&); // Disabled
	public:
		~OGL33VertexBufferData();

		void Set(unsigned int bytes, const void* data, VertexBufferUsageType type = VertexBufferUsageType::Static);
		void Update(unsigned int offset, unsigned int bytes, const void* data);

		VertexBufferUsageType GetUsage() const;
		unsigned int Size() const;
		const IVertexBufferView* CreateView(unsigned int numComponents = 1, VertexBufferDataType type = VertexBufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0) const;
		void DestroyView(const IVertexBufferView* view);
	};
}

#endif