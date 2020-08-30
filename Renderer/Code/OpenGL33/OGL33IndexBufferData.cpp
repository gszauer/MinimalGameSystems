#include "OGL33IndexBufferData.h"
#include "OGL33IndexBufferView.h"
#include "OGL33Internal.h"

Renderer::OGL33IndexBufferData::OGL33IndexBufferData(const IGraphicsDevice& owner) {
	mUsageType = IndexBufferUsageType::Static;
	mSize = 0;
	glGenBuffers(1, &mBuffer);
	mOwner = &owner;
}

Renderer::OGL33IndexBufferData::~OGL33IndexBufferData() {
	glDeleteBuffers(1, &mBuffer);
	mOwner = 0;
}

void Renderer::OGL33IndexBufferData::Set(unsigned int bytes, const void* data, IndexBufferUsageType type = IndexBufferUsageType::Static) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes, data, type ==IndexBufferUsageType::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	mUsageType = type;
	mSize = bytes;
}

void Renderer::OGL33IndexBufferData::Update(unsigned int offset, unsigned int bytes, const void* data) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, bytes, data);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	mSize = bytes;
}

Renderer::IndexBufferUsageType Renderer::OGL33IndexBufferData::GetUsage() const {
	return mUsageType;
}

unsigned int Renderer::OGL33IndexBufferData::Size() const {
	return mSize;
}

const Renderer::IIndexBufferView* Renderer::OGL33IndexBufferData::CreateView(unsigned int count, IndexBufferDataType type, unsigned int offset) const {
	OGL33IndexBufferView* view = (OGL33IndexBufferView*)Renderer::Internal::Alloc(sizeof(OGL33IndexBufferView));
	new(view)OGL33IndexBufferView(*this, count, type, offset);
	return view;
}

void Renderer::OGL33IndexBufferData::DestroyView(const IIndexBufferView* view) const {
	if (view != 0) {
		const OGL33IndexBufferView* oglView = (const OGL33IndexBufferView*)view;
		oglView->~OGL33IndexBufferView();
	}
}

const Renderer::IGraphicsDevice* Renderer::OGL33IndexBufferData::GetOwner() const {
	return mOwner;
}