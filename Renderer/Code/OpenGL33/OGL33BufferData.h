#ifndef _H_OGL33BUFFERDATA_
#define _H_OGL33BUFFERDATA_

#include "../IBufferData.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33Context;

	class OGL33VertexBufferData : public IBufferData {
		friend class OGL33Context;
	protected:
		BufferDataUsage mUsageType;
		unsigned int mSize;
		GLuint mBuffer;
		const OGL33Context* mOwner;
	protected:
		OGL33VertexBufferData(); // Disabled
		OGL33VertexBufferData(const OGL33VertexBufferData&); // Disabled
		OGL33VertexBufferData& operator=(const OGL33VertexBufferData&); // Disabled
		OGL33VertexBufferData(const IContext&);
	public:
		~OGL33VertexBufferData();

		void Set(BufferDataIntent intent, unsigned int bytes, const void* data, BufferDataUsage type = BufferDataUsage::Static);
		void Update(BufferDataIntent intent, unsigned int offset, unsigned int bytes, const void* data);

		BufferDataUsage GetUsage() const;
		unsigned int Size() const;

		const IBufferView* CreateView(unsigned int numComponents = 1, BufferDataType type = BufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0) const;
		void DestroyView(const IBufferView* view) const;
		
		const IContext* GetOwner() const;
	};
}

#endif