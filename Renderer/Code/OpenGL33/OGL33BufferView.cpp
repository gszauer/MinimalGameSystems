#include "OGL33BufferView.h"
#include "OGL33BufferData.h"

const Renderer::IBufferData* Renderer::OGL33BufferView::GetOwner() const {
	return mOwner;
}

Renderer::OGL33BufferView::OGL33BufferView(const IBufferData& data, unsigned int numComponents = 1, BufferDataType type, unsigned int stride, unsigned int offset) {
	mOwner = (const OGL33BufferData*)&data;
	mNumComponents = numComponents;
	mType = type;
	mStride = stride;;
	mOffset = offset;
}

Renderer::OGL33BufferView::~OGL33BufferView() {
	mOwner = 0;
}

unsigned int Renderer::OGL33BufferView::GetNumComponents() const {
	return mNumComponents;
}

Renderer::BufferDataType Renderer::OGL33BufferView::GetType() const {
	return mType;
}

unsigned int Renderer::OGL33BufferView::GetStride() const {
	return mStride;
}

unsigned int Renderer::OGL33BufferView::GetOffset() const {
	return mOffset;
}