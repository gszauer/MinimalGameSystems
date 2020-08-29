#include "OGL33VertexBufferData.h"

Renderer::OGL33VertexBufferData::OGL33VertexBufferData() {
	mUsageType = BufferUsageType::Static;
	mSize = 0;
	glGenBuffers(1, &mBuffer);
}

Renderer::OGL33VertexBufferData::~OGL33VertexBufferData() {
	glDeleteBuffers(1, &mBuffer);
}

void Renderer::OGL33VertexBufferData::Set(unsigned int bytes, const void* data, BufferUsageType type = BufferUsageType::Static) {
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
	glBufferData(GL_ARRAY_BUFFER, bytes, data, type == BufferUsageType::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
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

Renderer::BufferUsageType Renderer::OGL33VertexBufferData::GetUsage() const {
	return mUsageType;
}

unsigned int Renderer::OGL33VertexBufferData::Size() const {
	return mSize;
}

const Renderer::IVertexBufferView* Renderer::OGL33VertexBufferData::CreateView(unsigned int numComponents = 1, VertexBufferDataType type = VertexBufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0) const {
	// TODO: Alloc new OGL33VertexBufferView object and return address
}

void Renderer::OGL33VertexBufferData::DestroyView(const IVertexBufferView* view) {
	// TODO: Destroy vertex buffer view
}