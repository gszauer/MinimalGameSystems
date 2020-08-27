#ifndef _H_OGL33_VERTEXBUFFERVIEW_
#define _H_OGL33_VERTEXBUFFERVIEW_

#include "../IVertexBufferView.h"

namespace Renderer {
	class OGL33BufferData;

	class OGL33VertexBufferView {
		friend class OGL33BufferData;
	protected:
		const OGL33BufferData* mOwner;
		unsigned int mNumComponents;
		VertexBufferDataType mType;
		unsigned int mStride;
		unsigned int mOffset;
	protected:
		OGL33VertexBufferView();
	public:
		OGL33VertexBufferView(const IBufferData& data, , unsigned int numComponents = 1, VertexBufferDataType type = VertexBufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0);
		OGL33VertexBufferView(const OGL33VertexBufferView& other);
		OGL33VertexBufferView& operator=(const OGL33VertexBufferView& other);
		~OGL33VertexBufferView();
		
		const IBufferData* GetBuffer() const;
		unsigned int GetNumComponents() const;
		VertexBufferDataType GetType() const;
		unsigned int GetStride() const;
		unsigned int GetOffset() const;
	};
}

#endif