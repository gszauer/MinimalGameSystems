#include "OGL33VertexBufferView.h"
#include "OGL33VertexBufferData.h"

Renderer::OGL33VertexBufferView::OGL33VertexBufferView(const IVertexBufferData& data, unsigned int numComponents = 1, VertexBufferDataType type = VertexBufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0) {
	mOwner = (const OGL33VertexBufferData*)&data;
	mNumComponents = numComponents;
	mType = type;
	mStride = stride;;
	mOffset = offset;
}

Renderer::OGL33VertexBufferView::~OGL33VertexBufferView() { 
	mOwner = 0;
}

const Renderer::IVertexBufferData* Renderer::OGL33VertexBufferView::GetBuffer() const {
	return mOwner;
}

unsigned int Renderer::OGL33VertexBufferView::GetNumComponents() const {
	return mNumComponents;
}

Renderer::VertexBufferDataType Renderer::OGL33VertexBufferView::GetType() const {
	return mType;
}

unsigned int Renderer::OGL33VertexBufferView::GetStride() const {
	return mStride;
}

unsigned int Renderer::OGL33VertexBufferView::GetOffset() const {
	return mOffset;
}