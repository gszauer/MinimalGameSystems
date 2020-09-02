#include "OGL33Shader.h"
#include "OGL33Internal.h"
#include "OGL33ShaderAttribute.h"
#include "OGL33ShaderUniform.h"

Renderer::OGL33Shader::OGL33Shader(const IGraphicsDevice& device, const char* vertex, const char* fragment) {
	/* TODO
	const OGL33GraphicsDevice* mOwner;
	char* mError;
	GLuint mProgram;
	unsigned int mAttribCount;
	char** mAttribNames;
	OGL33ShaderAttribute* mAttribs;
	unsigned int mUniformCount;
	char** mUniformNames;
	OGL33ShaderUniform* mUniforms;
	*/
}

Renderer::OGL33Shader::~OGL33Shader() {
	// TODO
}

bool Renderer::OGL33Shader::IsValid() const {
	return mError == 0;
}

const char* Renderer::OGL33Shader::GetError() const {
	return mError;
}

const Renderer::IShaderAttribute* Renderer::OGL33Shader::GetAttribute(const char* name) const {
	for (unsigned int attrib = 0; attrib < mAttribCount; ++attrib) {
		const char* attribName = mAttribNames[attrib];
		if (Renderer::OGL33Internal::StrCmp(name, attribName) == 0) {
			return &mAttribs[attrib];
		}
	}

	return 0;
}

const Renderer::IShaderUniform* Renderer::OGL33Shader::GetUniform(const char* name) const {
	for (unsigned int uniform = 0; uniform < mUniformCount; ++uniform) {
		const char* uniformName = mUniformNames[uniform];
		if (Renderer::OGL33Internal::StrCmp(name, uniformName) == 0) {
			return &mUniforms[uniform];
		}
	}

	return 0;
}

unsigned int Renderer::OGL33Shader::AttributesSize() const {
	return mAttribCount;
}

unsigned int Renderer::OGL33Shader::UniformsSize() const {
	return mUniformCount;
}

const Renderer::IShaderAttribute* Renderer::OGL33Shader::GetAttribute(unsigned int index) const {
	return &mAttribs[index];
}

const Renderer::IShaderUniform* Renderer::OGL33Shader::GetUniform(unsigned int index) const {
	return &mUniforms[index];
}

const Renderer::IGraphicsDevice* Renderer::OGL33Shader::GetOwner() const {
	return mOwner;
}