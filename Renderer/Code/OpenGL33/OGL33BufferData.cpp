#include "OGL33BufferDAta.h"
#include "OGL33BufferView.h"
#include "OGL33Internal.h"
#include "OGL33Context.h"

const Renderer::IContext* Renderer::OGL33BufferData::GetOwner() const {
	return mOwner;
}

Renderer::OGL33BufferData::OGL33BufferData(const IContext& owner) {
	mUsageType = BufferDataUsage::Static;
	mSize = 0;
	glGenBuffers(1, &mBuffer);
	mOwner = (const OGL33Context*)&owner;
}

Renderer::OGL33BufferData::~OGL33BufferData() {
	glDeleteBuffers(1, &mBuffer);
	mOwner = 0;
}

void Renderer::OGL33BufferData::Set(BufferDataIntent intent, unsigned int bytes, const void* data, BufferDataUsage type) {
	GLenum target = GL_ARRAY_BUFFER;
	if (intent == BufferDataIntent::Element) {
		target = GL_ELEMENT_ARRAY_BUFFER;
	}
	glBindBuffer(target, mBuffer);
	glBufferData(target, bytes, data, type == BufferDataUsage::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	glBindBuffer(target, 0);
	mUsageType = type;
	mSize = bytes;
}

void Renderer::OGL33BufferData::Update(BufferDataIntent intent, unsigned int offset, unsigned int bytes, const void* data) {
	GLenum target = GL_ARRAY_BUFFER;
	if (intent == BufferDataIntent::Element) {
		target = GL_ELEMENT_ARRAY_BUFFER;
	}
	glBindBuffer(target, mBuffer);
	glBufferSubData(target, offset, bytes, data);
	glBindBuffer(target, 0);
	mSize = bytes;
}

Renderer::BufferDataUsage Renderer::OGL33BufferData::GetUsage() const {
	return mUsageType;
}

unsigned int Renderer::OGL33BufferData::Size() const {
	return mSize;
}

const Renderer::IBufferView* Renderer::OGL33BufferData::CreateView(unsigned int numComponents, BufferDataType type, unsigned int stride, unsigned int offset) const {
	OGL33BufferView* view = (OGL33BufferView * )Renderer::OGL33Internal::Alloc(sizeof(OGL33BufferView));
	new(view)OGL33BufferView(*this, numComponents, type, stride, offset);
	return view;
}

void Renderer::OGL33BufferData::DestroyView(const IBufferView* view) const {
	if (view != 0) {
		const OGL33BufferView* oglView = (const OGL33BufferView*)view;
		oglView->~OGL33BufferView();
	}
}
