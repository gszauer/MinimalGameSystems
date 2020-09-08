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
		inline IShaderAttribute(const IShader& shader, const char* name = 0, ShaderAttributeType type) { }
	public:
		virtual inline ~IShaderAttribute() { }
		virtual const char* GetName() const = 0;
		virtual ShaderAttributeType GetType() const = 0;

	};
}

#endif 