#ifndef _H_OGL33SHADER_
#define _H_OGL33SHADER_

#include "../IShader.h"
#include "OGL33Loader.h"

#include <map>
#include<string>

namespace Renderer {
	class OGL33GraphicsDevice;
	class OGL33ShaderAttribute;

	class OGL33Shader : public IShader {
		friend class OGL33GraphicsDevice;
	protected:
		const OGL33GraphicsDevice* mOwner;
		char* mError;
		GLuint mProgram;
		std::map<std::string, OGL33ShaderAttribute*> mAttributes;
		std::map<std::string, > mUniforms;
	protected:
		OGL33Shader(); // Disabled
		OGL33Shader(const OGL33Shader&); // Disabled
		virtual OGL33Shader& operator=(const OGL33Shader&); // Disabled
		OGL33Shader(const IGraphicsDevice& device, const char* vertex, const char* fragment);
	public:
		~OGL33Shader();

		bool IsValid() const = 0;
		const char* GetError() const = 0;

		const IShaderAttribute* GetAttribute(const char* name) const;
		const IShaderUniform* GetUniform(const char* name) const;

		const IGraphicsDevice* GetOwner() const;
	};
}

#endif