#include "OGL33ShaderUniform.h"

Renderer::OGL33ShaderUniform::OGL33ShaderUniform(const IShader& shader, unsigned int index, unsigned int size, ShaderUniformType type, bool isArray, unsigned int length) {
	mOwner = &shader;
	mIndex = index;
	mSize = size;
	mType = type;
	mIsArray = isArray;
	mLengt = length;
}

Renderer::OGL33ShaderUniform::~OGL33ShaderUniform() {
	mOwner = 0;
}

unsigned int Renderer::OGL33ShaderUniform::GetIndex() const {
	return mIndex;
}

unsigned int Renderer::OGL33ShaderUniform::Size() const {
	return mSize;
}

Renderer::ShaderUniformType Renderer::OGL33ShaderUniform::GetType() const {
	return mType;
}

bool Renderer::OGL33ShaderUniform::IsArray() const {
	return mIsArray;
}

unsigned int Renderer::OGL33ShaderUniform::Length() const {
	return mLengt;
}

const Renderer::IShader* Renderer::OGL33ShaderUniform::GetOwner() const {
	return mOwner;
}