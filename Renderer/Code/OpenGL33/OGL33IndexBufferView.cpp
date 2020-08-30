#include "OGL33IndexBufferView.h"
#include "OGL33IndexBufferData.h"

Renderer::OGL33IndexBufferView::OGL33IndexBufferView(const IIndexBufferData& data, unsigned int count, IndexBufferDataType type, unsigned int offset) {
	mOwner = (const OGL33IndexBufferData*)&data;;
	mCount = count;
	mType = type;
	mOffset = offset;
}

Renderer::OGL33IndexBufferView::~OGL33IndexBufferView() {
	mOwner = 0;
}

const Renderer::IIndexBufferData* Renderer::OGL33IndexBufferView::GetBuffer() const {
	return mOwner;
}

unsigned int Renderer::OGL33IndexBufferView::GetCount() const {
	return mCount;
}

Renderer::IndexBufferDataType Renderer::OGL33IndexBufferView::GetType() const {
	return mType;
}

unsigned int Renderer::OGL33IndexBufferView::GetOffset() const {
	return mOffset;
}