#ifndef _H_ISHADERATTRIBUTE_
#define _H_ISHADERATTRIBUTE_

namespace Renderer {
	enum class ShaderAttributeType {
		FLOAT, 
		FLOAT_VEC2, 
		FLOAT_VEC3, 
		FLOAT_VEC4, 
		FLOAT_MAT2, 
		FLOAT_MAT3, 
		FLOAT_MAT4, 
		FLOAT_MAT2x3, 
		FLOAT_MAT2x4, 
		FLOAT_MAT3x2, 
		FLOAT_MAT3x4, 
		FLOAT_MAT4x2, 
		FLOAT_MAT4x3, 
		INT,
		INT_VEC2, 
		INT_VEC3, 
		INT_VEC4, 
		UNSIGNED_INT, 
		UNSIGNED_INT_VEC2, 
		UNSIGNED_INT_VEC3, 
		UNSIGNED_INT_VEC4, 
		DOUBLE, 
		DOUBLE_VEC2, 
		DOUBLE_VEC3, 
		DOUBLE_VEC4, 
		DOUBLE_MAT2, 
		DOUBLE_MAT3, 
		DOUBLE_MAT4, 
		DOUBLE_MAT2x3, 
		DOUBLE_MAT2x4, 
		DOUBLE_MAT3x2, 
		DOUBLE_MAT3x4, 
		DOUBLE_MAT4x2,
		DOUBLE_MAT4x3
	};

	class IShader;

	class IShaderAttribute {
	protected:
		IShaderAttribute(); // Disabled
		IShaderAttribute(const IShaderAttribute&); // Disabled
		virtual IShaderAttribute& operator=(const IShaderAttribute&); // Disabled
		inline IShaderAttribute(const IShader& shader, unsigned int index, unsigned int size, ShaderAttributeType type, bool isArray, unsigned int length) { }
	public:
		virtual inline ~IShaderAttribute() { }
		virtual unsigned int GetIndex() const = 0;

		virtual unsigned int Size() const = 0;
		virtual ShaderAttributeType GetType() const = 0;

		virtual bool IsArray() const = 0;
		virtual unsigned int Length() const = 0;

		virtual const IShader* GetOwner() const = 0;
	};
}

#endif 