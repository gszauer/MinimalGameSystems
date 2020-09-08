#ifndef _H_OGL33SHADERATTRIBUTE_
#define _H_OGL33SHADERATTRIBUTE_

#include "../IShaderAttribute.h"

namespace Renderer {
	class OGL33Shader;
	class OGL33ShaderAttribute : public IShaderAttribute {
		friend class OGL33Shader;
	protected:
		const OGL33Shader* mOwner;
		const char* mName;
		unsigned int mIndex;
		unsigned int mSize;
		ShaderAttributeType mType;
	public:
		OGL33ShaderAttribute(); // Disabled
		OGL33ShaderAttribute(const OGL33ShaderAttribute&); // Disabled
		OGL33ShaderAttribute& operator=(const OGL33ShaderAttribute&); // Disabled
		OGL33ShaderAttribute(const IShader& shader, const char* name, ShaderAttributeType type);
	public:
		~OGL33ShaderAttribute();
		unsigned int GetIndex() const;

		unsigned int Size() const;
		ShaderAttributeType GetType() const;

		const IShader* GetOwner() const;
		const char* GetName() const;

	};
}

#endif