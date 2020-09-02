#ifndef _H_ISHADERATTRIBUTE_
#define _H_ISHADERATTRIBUTE_

namespace Renderer {
	enum class ShaderAttributeType {
		Float,
		Vec2F,
		Vec3F,
		Vec4F,
		Mat2F,
		Mat4F,
		Mat4F,
		Mat2x3F,
		Mat2x4F,
		Mat3x2F,
		Mat3x4F,
		Mat4X2F,
		Mat4X3F,
		Int,
		Vec2I,
		Vec3I,
		Vec4I,
		UnsignedInt,
		Vec2U,
		Vec3U,
		Vec4U,
		Double,
		Vec2D,
		Vec3D,
		Vec4D,
		Mat2D,
		Mat3D,
		Mat4D,
		Mat2x3D,
		Mat2x4D,
		Mat3x2D,
		Mat3x4D,
		Mat4x2D,
		Mat4x3D
	};

	class IShader;

	class IShaderAttribute {
	protected:
		IShaderAttribute(); // Disabled
		IShaderAttribute(const IShaderAttribute&); // Disabled
		virtual IShaderAttribute& operator=(const IShaderAttribute&); // Disabled
		inline IShaderAttribute(const IShader& shader, const char* name, unsigned int index, unsigned int size, ShaderAttributeType type, bool isArray, unsigned int length) { }
	public:
		virtual inline ~IShaderAttribute() { }
		virtual unsigned int GetIndex() const = 0;

		virtual unsigned int Size() const = 0;
		virtual ShaderAttributeType GetType() const = 0;

		virtual bool IsArray() const = 0;
		virtual unsigned int Length() const = 0;

		virtual const IShader* GetOwner() const = 0;

		virtual const char* GetName() const = 0;
	};
}

#endif 