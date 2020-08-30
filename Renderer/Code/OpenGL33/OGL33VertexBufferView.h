#ifndef _H_OGL33VERTEXBUFFERVIEW_
#define _H_OGL33VERTEXBUFFERVIEW_

#include "../IVertexBufferView.h"

namespace Renderer {
	class OGL33VertexBufferData;

	class OGL33VertexBufferView : public IVertexBufferView {
		friend class OGL33VertexBufferData;
	protected:
		const OGL33VertexBufferData* mOwner;
		unsigned int mNumComponents;
		VertexBufferDataType mType;
		unsigned int mStride;
		unsigned int mOffset;
	protected:
		OGL33VertexBufferView(); // Disabled
		OGL33VertexBufferView(const OGL33VertexBufferView& other); // Disbled
		OGL33VertexBufferView& operator=(const OGL33VertexBufferView& other); // Disabled
		OGL33VertexBufferView(const IVertexBufferData& data, unsigned int numComponents = 1, VertexBufferDataType type = VertexBufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0);
	public:
		~OGL33VertexBufferView();
		
		const IVertexBufferData* GetOwner() const;
		unsigned int GetNumComponents() const;
		VertexBufferDataType GetType() const;
		unsigned int GetStride() const;
		unsigned int GetOffset() const;
	};
}

#endif