#include "OGL33ShaderAttribute.h"
#include "OGL33Shader.h"

Renderer::OGL33ShaderAttribute::OGL33ShaderAttribute(const IShader& shader, const char* name,  ShaderAttributeType type) {
	mOwner = (const OGL33Shader*)&shader;
	mIndex = 0;
	mSize = 0;
	mType = type;
	mName = name; // This string lives in OGL33Shader. It's the key to a dictionary. The Shader attribute does not own it.
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

const char* Renderer::OGL33ShaderAttribute::GetName() const {
	return mName;
}


const Renderer::IShader* Renderer::OGL33ShaderAttribute::GetOwner() const {
	return mOwner;
}