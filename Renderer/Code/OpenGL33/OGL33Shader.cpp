#include "OGL33Shader.h"
#include "OGL33Internal.h"
#include "OGL33Loader.h"
#include "OGL33ShaderAttribute.h"
#include "OGL33ShaderUniform.h"
#include "OGL33Context.h"

#include <new>

namespace Renderer {
	namespace OGL33Internal {
		GLuint CreateVertexShader(const char* shader, char** error);
		GLuint CreateFragmentShader(const char* shader, char** error);
		void LinkShaders(GLuint program, GLuint vertex, GLuint fragment, char** error);
		ShaderAttributeType EnumToAttrib(GLenum enumeration);
		ShaderUniformType EnumToUniform(GLenum enumeration);
	}
}

const Renderer::IContext* Renderer::OGL33Shader::GetOwner() const {
	return mOwner;
}

Renderer::OGL33Shader::OGL33Shader(const IContext& device, const char* vertex, const char* fragment) {
	//////////////////////////////////////////////////////////////
	// Assign trivial variables
	mOwner = (const OGL33Context*)&device;
	mError = 0;

	//////////////////////////////////////////////////////////////
	// Create shader program
	mProgram = glCreateProgram();
	if (mProgram == 0) {
		const char* error = "Renderer::OGL33Shader::OGL33Shader, glCreateProgram failed.\n";
		unsigned int bytes = sizeof(char) * (Renderer::OGL33Internal::StrLen(error) + 1);
		mError = (char*)Renderer::OGL33Internal::Alloc(bytes);
		Renderer::OGL33Internal::MemSet(mError, 0, bytes);
		Renderer::OGL33Internal::StrCpy(mError, error);
		return;
	}
	glUseProgram(mProgram);

	//////////////////////////////////////////////////////////////
	// Compile vertex shader
	GLuint vertexShader = Renderer::OGL33Internal::CreateVertexShader(vertex, &mError);
	if (mError != 0) {
		glDeleteProgram(mProgram);
		return;
	}

	//////////////////////////////////////////////////////////////
	// Compile fragment shader
	GLuint fragmentShader = Renderer::OGL33Internal::CreateVertexShader(fragment, &mError);
	if (mError != 0) {
		glDeleteShader(vertexShader);
		glDeleteProgram(mProgram);
		return;
	}

	//////////////////////////////////////////////////////////////
	// Link program and delete shaders
	Renderer::OGL33Internal::LinkShaders(mProgram, vertexShader, fragmentShader, &mError);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (mError != 0) {
		glDeleteProgram(mProgram);
		return;
	}

	//////////////////////////////////////////////////////////////
	// Helper variables
	GLchar  nameString[512];
	GLsizei nameLength;
	GLint size;
	GLenum type;

	//////////////////////////////////////////////////////////////
	// Populate Uniforms
	GLint numUniforms = 0;
	glGetProgramiv(mProgram, GL_ACTIVE_UNIFORMS, &numUniforms);
	if (numUniforms < 0) {
		numUniforms = 0;
	}
	mUniformCount = (unsigned int)numUniforms;

	mUniformNames = (char**)Renderer::OGL33Internal::Alloc(sizeof(char*) * mUniformCount);
	mUniforms = (OGL33ShaderUniform*)Renderer::OGL33Internal::Alloc(sizeof(OGL33ShaderUniform) * mUniformCount);

	for (unsigned int uniform = 0; uniform < numUniforms; ++uniform) {
		Renderer::OGL33Internal::MemSet(nameString, 0, sizeof(char) * 512);
		glGetActiveUniform(mProgram, uniform, 512, &nameLength, &size, &type, nameString);

		bool isArray = false;
		for (unsigned int i = 0; i < nameLength; ++i) {
			if (nameString[i] == '[') {
				nameString[i] = '\0';
				nameLength = i;
				isArray = true;
				break;
			}
		}

		unsigned int bytesToAlloc = nameLength * sizeof(char) + sizeof(char);
		mUniformNames[uniform] = (char*)Renderer::OGL33Internal::Alloc(bytesToAlloc);
		mUniformNames[uniform][nameLength] = '\0';
		Renderer::OGL33Internal::StrCpy(mUniformNames[uniform], nameString);

		// Placement new to init the new array
		new (&mUniforms[uniform])OGL33ShaderUniform(*this, nameString, Renderer::OGL33Internal::EnumToUniform(type));
		mUniforms[uniform].mIndex = uniform;
		mUniforms[uniform].mSize = (unsigned int)size;
		mUniforms[uniform].mIsArray = isArray;

		const OGL33Shader* mOwner;
		const char* mName;
		unsigned int mIndex;
		unsigned int mSize;
		ShaderUniformType mType;
		bool mIsArray;
		unsigned int mLength;
	}

	//////////////////////////////////////////////////////////////
	// Populate Attributes
	GLint numAttribs = 0;
	glGetProgramiv(mProgram, GL_ACTIVE_ATTRIBUTES, &numAttribs);
	if (numAttribs < 0) {
		numAttribs = 0;
	}
	mAttribCount = (unsigned int)numAttribs;

	mAttribNames = (char**)Renderer::OGL33Internal::Alloc(sizeof(char*) * mAttribCount);
	mAttribs = (OGL33ShaderAttribute*)Renderer::OGL33Internal::Alloc(sizeof(OGL33ShaderAttribute) * mAttribCount);
	
	for (unsigned int attrib = 0; attrib < mAttribCount; ++attrib) {
		Renderer::OGL33Internal::MemSet(nameString, 0, sizeof(char) * 512);
		glGetActiveAttrib(mProgram, attrib, 512, &nameLength, &size, &type, nameString);

		unsigned int bytesToAlloc = nameLength * sizeof(char) + sizeof(char);
		mAttribNames[attrib] = (char*)Renderer::OGL33Internal::Alloc(bytesToAlloc);
		mAttribNames[attrib][nameLength] = '\0';
		Renderer::OGL33Internal::StrCpy(mAttribNames[attrib], nameString);

		// Placement new to init the new array
		new(&mAttribs[attrib])OGL33ShaderAttribute(*this, nameString, Renderer::OGL33Internal::EnumToAttrib(type));
		mAttribs[attrib].mIndex = (unsigned int)attrib;
		mAttribs[attrib].mSize = (unsigned int)size;
	}


	//////////////////////////////////////////////////////////////
	// Done
	glUseProgram(mOwner->GetBoundShader());
}

Renderer::OGL33Shader::~OGL33Shader() {
	glDeleteProgram(mProgram);
	if (mError != 0) {
		Renderer:OGL33Internal::Free(mError);
		mError = 0;
	}
	if (mAttribNames != 0) {
		for (unsigned int attrib = 0; attrib < mAttribCount; ++attrib) {
			Renderer::OGL33Internal::Free(mAttribNames[attrib]);
		}
		Renderer::OGL33Internal::Free(mAttribNames);
		mAttribNames = 0;
	}
	if (mAttribs != 0) {
		for (unsigned int attrib = 0; attrib < mAttribCount; ++attrib) {
			mAttribs[attrib].~OGL33ShaderAttribute();
		}
		Renderer::OGL33Internal::Free(mAttribs);
		mAttribs = 0;
	}
	if (mUniformNames != 0) {
		for (unsigned int uniform = 0; uniform < mUniformCount; ++uniform) {
			Renderer::OGL33Internal::Free(mUniformNames[uniform]);
		}
		Renderer::OGL33Internal::Free(mUniformNames);
		mUniformNames = 0;
	}
	if (mUniforms != 0) {
		for (unsigned int uniform = 0; uniform < mUniformCount; ++uniform) {
			mUniforms[uniform].~OGL33ShaderUniform();
		}
		Renderer::OGL33Internal::Free(mUniforms);
		mUniforms = 0;
	}
	mOwner = 0;
}

bool Renderer::OGL33Shader::IsValid() const {
	return mError == 0;
}

const char* Renderer::OGL33Shader::GetError() const {
	return mError;
}

const Renderer::IShaderAttribute* Renderer::OGL33Shader::GetAttribute(const char* name) const {
	for (unsigned int attrib = 0; attrib < mAttribCount; ++attrib) {
		const char* attribName = mAttribNames[attrib];
		if (Renderer::OGL33Internal::StrCmp(name, attribName) == 0) {
			return &mAttribs[attrib];
		}
	}

	return 0;
}

const Renderer::IShaderUniform* Renderer::OGL33Shader::GetUniform(const char* name) const {
	for (unsigned int uniform = 0; uniform < mUniformCount; ++uniform) {
		const char* uniformName = mUniformNames[uniform];
		if (Renderer::OGL33Internal::StrCmp(name, uniformName) == 0) {
			return &mUniforms[uniform];
		}
	}

	return 0;
}

unsigned int Renderer::OGL33Shader::AttributesSize() const {
	return mAttribCount;
}

unsigned int Renderer::OGL33Shader::UniformsSize() const {
	return mUniformCount;
}

const Renderer::IShaderAttribute* Renderer::OGL33Shader::GetAttribute(unsigned int index) const {
	return &mAttribs[index];
}

const Renderer::IShaderUniform* Renderer::OGL33Shader::GetUniform(unsigned int index) const {
	return &mUniforms[index];
}

Renderer::ShaderAttributeType Renderer::OGL33Internal::EnumToAttrib(GLenum enumeration) {
	Renderer::ShaderAttributeType result = ShaderAttributeType::Error;

	if (enumeration == GL_FLOAT) {
		result = ShaderAttributeType::Float;
	}
	else if (enumeration == GL_INT) {
		result = ShaderAttributeType::Int;
	}
	else if (enumeration == GL_UNSIGNED_INT) {
		result = ShaderAttributeType::UnsignedInt;
	}
	else if (enumeration == GL_FLOAT_VEC2) {
		result = ShaderAttributeType::Vec2F;
	}
	else if (enumeration == GL_FLOAT_VEC3) {
		result = ShaderAttributeType::Vec3F;
	}
	else if (enumeration == GL_FLOAT_VEC4) {
		result = ShaderAttributeType::Vec4F;
	}
	else if (enumeration == GL_INT_VEC2) {
		result = ShaderAttributeType::Vec2I;
	}
	else if (enumeration == GL_INT_VEC3) {
		result = ShaderAttributeType::Vec3I;
	}
	else if (enumeration == GL_INT_VEC4) {
		result = ShaderAttributeType::Vec4I;
	}
	else if (enumeration == GL_UNSIGNED_INT_VEC2) {
		result = ShaderAttributeType::Vec2U;
	}
	else if (enumeration == GL_UNSIGNED_INT_VEC3) {
		result = ShaderAttributeType::Vec3U;
	}
	else if (enumeration == GL_UNSIGNED_INT_VEC4) {
		result = ShaderAttributeType::Vec4U;
	}

	return result;
}

Renderer::ShaderUniformType Renderer::OGL33Internal::EnumToUniform(GLenum enumeration) {
	Renderer::ShaderUniformType result = ShaderUniformType::Error;

	if (enumeration == GL_FLOAT) {
		result = ShaderUniformType::Float;
	}
	else if (enumeration == GL_INT) {
		result = ShaderUniformType::Int;
	}
	else if (enumeration == GL_UNSIGNED_INT) {
		result = ShaderUniformType::UnsignedInt;
	}
	else if (enumeration == GL_FLOAT_VEC2) {
		result = ShaderUniformType::Vec2F;
	}
	else if (enumeration == GL_FLOAT_VEC3) {
		result = ShaderUniformType::Vec3F;
	}
	else if (enumeration == GL_FLOAT_VEC4) {
		result = ShaderUniformType::Vec4F;
	}
	else if (enumeration == GL_INT_VEC2) {
		result = ShaderUniformType::Vec2I;
	}
	else if (enumeration == GL_INT_VEC3) {
		result = ShaderUniformType::Vec3I;
	}
	else if (enumeration == GL_INT_VEC4) {
		result = ShaderUniformType::Vec4I;
	}
	else if (enumeration == GL_UNSIGNED_INT_VEC2) {
		result = ShaderUniformType::Vec2U;
	}
	else if (enumeration == GL_UNSIGNED_INT_VEC3) {
		result = ShaderUniformType::Vec3U;
	}
	else if (enumeration == GL_UNSIGNED_INT_VEC4) {
		result = ShaderUniformType::Vec4U;
	}
	else if (enumeration == GL_FLOAT_MAT2) {
		result = ShaderUniformType::Mat2F;
	}
	else if (enumeration == GL_FLOAT_MAT3) {
		result = ShaderUniformType::Mat3F;
	}
	else if (enumeration == GL_FLOAT_MAT4) {
		result = ShaderUniformType::Mat4F;
	}
	else if (enumeration == GL_FLOAT_MAT2x3) {
		result = ShaderUniformType::Mat2x3F;
	}
	else if (enumeration == GL_FLOAT_MAT2x4) {
		result = ShaderUniformType::Mat2x4F;
	}
	else if (enumeration == GL_FLOAT_MAT3x2) {
		result = ShaderUniformType::Mat3x2F;
	}
	else if (enumeration == GL_FLOAT_MAT3x4) {
		result = ShaderUniformType::Mat3x4F;
	}
	else if (enumeration == GL_FLOAT_MAT4x2) {
		result = ShaderUniformType::Mat4X2F;
	}
	else if (enumeration == GL_FLOAT_MAT4x3) {
		result = ShaderUniformType::Mat4X3F;
	}
	else if (enumeration == GL_SAMPLER_1D) {
		result = ShaderUniformType::Sampler;
	}
	else if (enumeration == GL_SAMPLER_2D) {
		result = ShaderUniformType::Sampler;
	}
	else if (enumeration == GL_SAMPLER_3D) {
		result = ShaderUniformType::Sampler;
	}

	return result;
}

GLuint Renderer::OGL33Internal::CreateVertexShader(const char* shader, char** error) {
	GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v_shader, 1, &shader, NULL);
	glCompileShader(v_shader);

	int success = 0;
	glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(v_shader, 512, NULL, infoLog);

		unsigned int bytes = sizeof(char) * (Renderer::OGL33Internal::StrLen(infoLog) + 1);
		*error = (char*)Renderer::OGL33Internal::Alloc(bytes);
		Renderer::OGL33Internal::MemSet(*error, 0, bytes);
		Renderer::OGL33Internal::StrCpy(*error, infoLog);

		return 0;
	};
	return v_shader;
}

GLuint Renderer::OGL33Internal::CreateFragmentShader(const char* shader, char** error) {
	unsigned int f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f_shader, 1, &shader, NULL);
	glCompileShader(f_shader);

	int success = 0;
	glGetShaderiv(f_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(f_shader, 512, NULL, infoLog);
		
		unsigned int bytes = sizeof(char) * (Renderer::OGL33Internal::StrLen(infoLog) + 1);
		*error = (char*)Renderer::OGL33Internal::Alloc(bytes);
		Renderer::OGL33Internal::MemSet(*error, 0, bytes);
		Renderer::OGL33Internal::StrCpy(*error, infoLog);

		return 0;
	};
	return f_shader;
}

void Renderer::OGL33Internal::LinkShaders(GLuint program, GLuint vertex, GLuint fragment, char** error) {
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	int success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		
		unsigned int bytes = sizeof(char) * (Renderer::OGL33Internal::StrLen(infoLog) + 1);
		*error = (char*)Renderer::OGL33Internal::Alloc(bytes);
		Renderer::OGL33Internal::MemSet(*error, 0, bytes);
		Renderer::OGL33Internal::StrCpy(*error, infoLog);

		return;
	}


}