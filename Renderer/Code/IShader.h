#ifndef _H_ISHADER_
#define _H_ISHADER_

namespace Renderer {
	class IGraphicsDevice;
	class IShaderAttribute;
	class IShaderUniform;

	class IShader {
	private:
		IShader(); // Disabled
		IShader(const IShader&); // Disabled
		virtual IShader& operator=(const IShader&); // Disabled
		inline IShader(const IGraphicsDevice& device, const char* vertex, const char* fragment) { }
	public:
		virtual inline ~IShader() { }

		virtual bool IsValid() const = 0;
		virtual const char* GetError() const = 0;

		virtual const IShaderAttribute* GetAttribute(const char* name) const = 0;
		virtual const IShaderUniform* GetUniform(const char* name) const = 0;

		virtual unsigned int AttributesSize() const = 0;
		virtual unsigned int UniformsSize() const = 0;

		virtual const IShaderAttribute* GetAttribute(unsigned int index) const = 0;
		virtual const IShaderUniform* GetUniform(unsigned int index) const = 0;

		virtual const IGraphicsDevice* GetOwner() const = 0;
	};
}

#endif