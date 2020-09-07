#ifndef _H_OGL33SHADER_
#define _H_OGL33SHADER_

#include "../IShader.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33Context;
	class OGL33ShaderAttribute;
	class OGL33ShaderUniform;

	class OGL33Shader : public IShader {
		friend class OGL33Context;
	protected:
		const OGL33Context* mOwner;
		char* mError;
		GLuint mProgram;
		unsigned int mAttribCount;
		char** mAttribNames;
		OGL33ShaderAttribute* mAttribs;
		unsigned int mUniformCount;
		char** mUniformNames;
		OGL33ShaderUniform* mUniforms;
	protected:
		OGL33Shader(); // Disabled
		OGL33Shader(const OGL33Shader&); // Disabled
		virtual OGL33Shader& operator=(const OGL33Shader&); // Disabled
		OGL33Shader(const IContext& device, const char* vertex, const char* fragment);
	public:
		~OGL33Shader();

		bool IsValid() const;
		const char* GetError() const;

		const IShaderAttribute* GetAttribute(const char* name) const;
		const IShaderUniform* GetUniform(const char* name) const;

		unsigned int AttributesSize() const;
		unsigned int UniformsSize() const;

		const IShaderAttribute* GetAttribute(unsigned int index) const;
		const IShaderUniform* GetUniform(unsigned int index) const;

		const IContext* GetOwner() const;
	};
}

#endif