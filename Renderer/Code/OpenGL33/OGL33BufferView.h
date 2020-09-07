#ifndef _H_OGL33BUFFERVIEW_
#define _H_OGL33BUFFERVIEW_

#include "../IBufferView.h"

namespace Renderer {
	class OGL33BufferData;

	class OGL33VertexBufferView : public IBufferView {
		friend class OGL33BufferData;
	protected:
		const OGL33BufferData* mOwner;
		unsigned int mNumComponents;
		BufferDataType mType;
		unsigned int mStride;
		unsigned int mOffset;
	protected:
		OGL33VertexBufferView(); // Disabled
		OGL33VertexBufferView(const OGL33VertexBufferView& other); // Disbled
		OGL33VertexBufferView& operator=(const OGL33VertexBufferView& other); // Disabled
		OGL33VertexBufferView(const IBufferData& data, unsigned int numComponents = 1, BufferDataType type = BufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0);
	public:
		~OGL33VertexBufferView();
		
		const IBufferData* GetOwner() const;
		unsigned int GetNumComponents() const;
		BufferDataType GetType() const;
		unsigned int GetStride() const;
		unsigned int GetOffset() const;
	};
}

#endif