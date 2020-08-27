#ifndef _H_OGL33SHADER_
#define _H_OGL33SHADER_

#include "../Renderer/IShader.h"

namespace Renderer {
	class OGL33Shader : public IShader {
		friend class OGL33GraphicsDevice;
	protected:
		inline OGL33Shader() { }
		inline ~OGL33Shader() { }
	public:
		inline void CompileNative(const char* vertex, const char* fragment) {}
		inline void CompileGeneric(const char* vertex, const char* fragment) {}
		inline void SetFloat(unsigned int numComponents, float* inputArray, unsigned int arrayLength) {}
		inline void SetInt(unsigned int numComponents, int* inputArray, unsigned int arrayLength) {}
		inline void SetUInt(unsigned int numComponents, unsigned int* inputArray, unsigned int arrayLength) {}
		inline void SetFloatMatrix(unsigned int numRows, unsigned int numCols, float* inputArray, unsigned int arrayLength) {}

		inline int GetAttribLocation(const char* name) const { return 0; }
		inline int GetUniformLocation(const char* name) const { return 0; }
		inline bool IsValid() const { return false; }
		inline ShaderCompileStatus GetcompileStatus() { return ShaderCompileStatus::NotStarted;  }
	};
}

#endif