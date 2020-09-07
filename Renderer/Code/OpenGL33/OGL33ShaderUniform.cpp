#include "OGL33ShaderUniform.h"
#include "OGL33Shader.h"

Renderer::OGL33ShaderUniform::OGL33ShaderUniform(const IShader& shader, const char* name, unsigned int index, unsigned int size, ShaderUniformType type, bool isArray, unsigned int length) {
	mOwner = (const OGL33Shader *)&shader;
	mIndex = index;
	mSize = size;
	mType = type;
	mIsArray = isArray;
	mName = name;
	mLength = length;
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
	return mLength;
}

const char* Renderer::OGL33ShaderUniform::GetName() const {
	return mName;
}

const Renderer::IShader* Renderer::OGL33ShaderUniform::GetOwner() const {
	return mOwner;
}