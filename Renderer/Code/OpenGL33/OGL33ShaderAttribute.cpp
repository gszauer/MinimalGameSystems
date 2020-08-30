#include "OGL33ShaderAttribute.h"

Renderer::OGL33ShaderAttribute::OGL33ShaderAttribute(const IShader& shader, unsigned int index, unsigned int size, ShaderAttributeType type, bool isArray, unsigned int length) {
	mOwner = &shader;
	mIndex = index;
	mSize = size;
	mType = type;
	mIsArray = isArray;
	mLengt = length;
}

Renderer::OGL33ShaderAttribute::~OGL33ShaderAttribute() {
	mOwner = 0;
}

unsigned int Renderer::OGL33ShaderAttribute::GetIndex() const {
	return mIndex;
}

unsigned int Renderer::OGL33ShaderAttribute::Size() const {
	return mSize;
}

Renderer::ShaderAttributeType Renderer::OGL33ShaderAttribute::GetType() const {
	return mType;
}

bool Renderer::OGL33ShaderAttribute::IsArray() const {
	return mIsArray;
}

unsigned int Renderer::OGL33ShaderAttribute::Length() const {
	return mLengt;
}

const Renderer::IShader* Renderer::OGL33ShaderAttribute::GetOwner() const {
	return mOwner;
}