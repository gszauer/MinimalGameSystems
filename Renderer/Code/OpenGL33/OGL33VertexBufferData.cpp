#include "OGL33VertexBufferData.h"
#include "OGL33VertexBufferView.h"
#include "OGL33Internal.h"

Renderer::OGL33VertexBufferData::OGL33VertexBufferData(const IGraphicsDevice& owner) {
	mUsageType = VertexBufferUsageType::Static;
	mSize = 0;
	glGenBuffers(1, &mBuffer);
	mOwner = &owner;
}

Renderer::OGL33VertexBufferData::~OGL33VertexBufferData() {
	glDeleteBuffers(1, &mBuffer);
	mOwner = 0;
}

void Renderer::OGL33VertexBufferData::Set(unsigned int bytes, const void* data, VertexBufferUsageType type) {
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
	glBufferData(GL_ARRAY_BUFFER, bytes, data, type == VertexBufferUsageType::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	mUsageType = type;
	mSize = bytes;
}

void Renderer::OGL33VertexBufferData::Update(unsigned int offset, unsigned int bytes, const void* data) {
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, offset, bytes, data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	mSize = bytes;
}

Renderer::VertexBufferUsageType Renderer::OGL33VertexBufferData::GetUsage() const {
	return mUsageType;
}

unsigned int Renderer::OGL33VertexBufferData::Size() const {
	return mSize;
}

const Renderer::IVertexBufferView* Renderer::OGL33VertexBufferData::CreateView(unsigned int numComponents, VertexBufferDataType type, unsigned int stride, unsigned int offset) const {
	OGL33VertexBufferView* view = (OGL33VertexBufferView * )Renderer::Internal::Alloc(sizeof(OGL33VertexBufferView));
	new(view)OGL33VertexBufferView(*this, numComponents, type, stride, offset);
	return view;
}

void Renderer::OGL33VertexBufferData::DestroyView(const IVertexBufferView* view) const {
	if (view != 0) {
		const OGL33VertexBufferView* oglView = (const OGL33VertexBufferView*)view;
		oglView->~OGL33VertexBufferView();
	}
}

const Renderer::IGraphicsDevice* Renderer::OGL33VertexBufferData::GetOwner() const {
	return mOwner;
}
