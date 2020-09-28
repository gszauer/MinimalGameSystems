#ifndef _H_OGL33SHADERUNIFORM_
#define _H_OGL33SHADERUNIFORM_

#include "../IShaderUniform.h"

namespace Renderer {
	class IShader;
	class OGL33Shader;

	class OGL33ShaderUniform : public IShaderUniform {
		friend class OGL33Shader;
	protected:
		const OGL33Shader* mOwner;
		const char* mName; // //This string lives in OGL33Shader. It's the key to a dictionary. The Shader uniform does not own it.
		unsigned int mIndex;
		unsigned int mSize;
		ShaderUniformType mType;
		bool mIsArray;
	protected:
		OGL33ShaderUniform(); // Disabled
		OGL33ShaderUniform(const OGL33ShaderUniform&); // Disabled
		OGL33ShaderUniform& operator=(const OGL33ShaderUniform&); // Disabled
		OGL33ShaderUniform(const IShader& shader, const char* name, ShaderUniformType type);
	public:
		~OGL33ShaderUniform();
		unsigned int GetIndex() const;

		unsigned int Size() const;
		ShaderUniformType GetType() const;

		bool IsArray() const;

		const IShader* GetOwner() const;
		const char* GetName() const;
	};
}

#endif 