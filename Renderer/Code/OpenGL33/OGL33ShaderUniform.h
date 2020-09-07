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
		const char* mName;
		unsigned int mIndex;
		unsigned int mSize;
		ShaderUniformType mType;
		bool mIsArray;
		unsigned int mLength;
	protected:
		OGL33ShaderUniform(); // Disabled
		OGL33ShaderUniform(const OGL33ShaderUniform&); // Disabled
		OGL33ShaderUniform& operator=(const OGL33ShaderUniform&); // Disabled
		OGL33ShaderUniform(const IShader& shader, const char* name, unsigned int index, unsigned int size, ShaderUniformType type, bool isArray, unsigned int length);
	public:
		~OGL33ShaderUniform();
		unsigned int GetIndex() const;

		unsigned int Size() const;
		ShaderUniformType GetType() const;

		bool IsArray() const;
		unsigned int Length() const;

		const IShader* GetOwner() const;
		const char* GetName() const;
	};
}

#endif 