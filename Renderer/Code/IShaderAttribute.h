#ifndef _H_ISHADERATTRIBUTE_
#define _H_ISHADERATTRIBUTE_

namespace Renderer {
	enum class ShaderAttributeType {
		Error = 0,
		Float,
		Vec2F,
		Vec3F,
		Vec4F,
		Int,
		Vec2I,
		Vec3I,
		Vec4I,
		UnsignedInt,
		Vec2U,
		Vec3U,
		Vec4U,
	};

	class IShader;

	class IShaderAttribute {
	protected:
		IShaderAttribute(); // Disabled
		IShaderAttribute(const IShaderAttribute&); // Disabled
		virtual IShaderAttribute& operator=(const IShaderAttribute&); // Disabled
		inline IShaderAttribute(const IShader& shader, const char* name = 0, unsigned int index = 0, unsigned int size = 0, ShaderAttributeType type = ShaderAttributeType::Float) { }
	public:
		virtual inline ~IShaderAttribute() { }
		virtual unsigned int GetIndex() const = 0;

		virtual unsigned int Size() const = 0;
		virtual ShaderAttributeType GetType() const = 0;

		virtual const IShader* GetOwner() const = 0;

		virtual const char* GetName() const = 0;
	};
}

#endif 