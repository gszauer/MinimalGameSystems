#ifndef _H_OGL33SHADERATTRIBUTE_
#define _H_OGL33SHADERATTRIBUTE_

#include "../IShaderAttribute.h"

namespace Renderer {
	class OGL33Shader;
	class OGL33ShaderAttribute : public IShaderAttribute {
		friend class OGL33Shader;
	protected:
		const OGL33Shader* mOwner;
		unsigned int mIndex;
		unsigned int mSize;
		ShaderAttributeType mType;
		bool mIsArray;
		unsigned int mLengt;
	public:
		OGL33ShaderAttribute(); // Disabled
		OGL33ShaderAttribute(const OGL33ShaderAttribute&); // Disabled
		virtual OGL33ShaderAttribute& operator=(const OGL33ShaderAttribute&); // Disabled
		OGL33ShaderAttribute(const IShader& shader, unsigned int index, unsigned int size, ShaderAttributeType type, bool isArray, unsigned int length);
	public:
		~OGL33ShaderAttribute();
		unsigned int GetIndex() const;

		unsigned int Size() const;
		ShaderAttributeType GetType() const;

		bool IsArray() const;
		unsigned int Length() const;

		const IShader* GetOwner() const;
	};
}

#endif