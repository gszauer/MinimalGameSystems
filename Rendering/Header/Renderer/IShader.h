#ifndef _H_ISHADER_
#define _H_ISHADER_

namespace Renderer {
	enum class ShaderCompileStatus {
		NotStarted = 0,
		Success = 1,
		FailedVertex = 2,
		FailedFragment = 3,
		FailedLinking = 4
	};

	class IShader {
	private:
		IShader(const IShader&);
		IShader& operator=(const IShader&);
	protected:
		inline IShader() { }
		virtual inline ~IShader() { }
	public:
		virtual bool IsValid() const = 0;

		virtual void SetFloat(unsigned int numComponents, float* inputArray, unsigned int arrayLength) = 0; // with 9 and 16 do mat 3 and mat 4
		virtual void SetInt(unsigned int numComponents, int* inputArray, unsigned int arrayLength) = 0;
		virtual void SetUInt(unsigned int numComponents, unsigned int* inputArray, unsigned int arrayLength) = 0;
		virtual void SetFloatMatrix(unsigned int numRows, unsigned int numCols, float* inputArray, unsigned int arrayLength) = 0;

		virtual int GetAttribLocation(const char* name) const = 0;
		virtual int GetUniformLocation(const char* name) const = 0;

		virtual void CompileNative(const char* vertex, const char* fragment) = 0;
		virtual void CompileGeneric(const char* vertex, const char* fragment) = 0;
		virtual ShaderCompileStatus GetcompileStatus() = 0;
	};
}

#endif