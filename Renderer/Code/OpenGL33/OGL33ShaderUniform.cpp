#include "OGL33ShaderUniform.h"
#include "OGL33Shader.h"

Renderer::OGL33ShaderUniform::OGL33ShaderUniform(const IShader& shader, const char* name, ShaderUniformType type) {
	mOwner = (const OGL33Shader *)&shader;
	mType = type;
	mName = name; //This string lives in OGL33Shader. It's the key to a dictionary. The Shader uniform does not own it.
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


const char* Renderer::OGL33ShaderUniform::GetName() const {
	return mName;
}

const Renderer::IShader* Renderer::OGL33ShaderUniform::GetOwner() const {
	return mOwner;
}