#ifndef _H_ISHADERUNIFORM_
#define _H_ISHADERUNIFORM_

namespace Renderer {
	enum class ShaderUniformType {
		Error,
		
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

		Mat2F,
		Mat3F,
		Mat4F,

		Mat2x3F,
		Mat2x4F,
		Mat3x2F,
		Mat3x4F,
		Mat4X2F,
		Mat4X3F,

		Sampler1D,
		Sampler2D,
		Sampler3D
	};

	class IShader;

	class IShaderUniform {
	protected:
		IShaderUniform(); // Disabled
		IShaderUniform(const IShaderUniform&); // Disabled
		virtual IShaderUniform& operator=(const IShaderUniform&); // Disabled
		inline IShaderUniform(const IShader& shader, const char* name, unsigned int index, unsigned int size, ShaderUniformType type, bool isArray, unsigned int length) { }
	public:
		virtual inline ~IShaderUniform() { }
		virtual unsigned int GetIndex() const = 0;

		virtual unsigned int Size() const = 0;
		virtual ShaderUniformType GetType() const = 0;

		virtual bool IsArray() const = 0;
		virtual unsigned int Length() const = 0;

		virtual const IShader* GetOwner() const = 0;
		virtual const char* GetName() const = 0;
	};
}

#endif 