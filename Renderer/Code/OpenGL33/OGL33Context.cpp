#include "OGL33BufferData.h"
#include "OGL33BufferView.h"
#include "OGL33Context.h"
#include "OGL33FrameBuffer.h"
#include "OGL33Internal.h"
#include "OGL33Loader.h"
#include "OGL33RasterState.h"
#include "OGL33Shader.h"
#include "OGL33ShaderAttribute.h"
#include "OGL33ShaderUniform.h"
#include "OGL33Texture.h"
#include "OGL33TextureSampler.h"


namespace Renderer {
	namespace OGL33Internal {
		GLenum BufferDataTypeToGlEnum(BufferDataType type) {
			// TODO
		}
	}
}

Renderer::OGL33Context::OGL33Context() {
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	mRendererName = "OpenGL 3.3 Core";

	mBoundFrameBuffer = 0;
	mBoundShader = 0;
	mCurrentTextureIndex = 0;

	// TODO: Finish function (as new stuff gets added)
}

Renderer::OGL33Context::~OGL33Context() {
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &mVAO);
	mBoundAttribs.clear();

	// TODO: Finish function (as new stuff gets added)
}

const char* Renderer::OGL33Context::GetName() const {
	return mRendererName;
}

Renderer::IFrameBuffer* Renderer::OGL33Context::CreateFrameBuffer(ITexture* colorAttachment, ITexture* depthAttachment, DepthUsage usage) const {
	if (colorAttachment == 0 && depthAttachment == 0) {
		return 0;
	}

	OGL33Texture* glColor = (OGL33Texture*)colorAttachment;
	OGL33Texture* glDepth = (OGL33Texture*)depthAttachment;

	Renderer::OGL33FrameBuffer* result = (Renderer::OGL33FrameBuffer*)Renderer::OGL33Internal::Alloc(sizeof(Renderer::OGL33FrameBuffer));
	new (result) Renderer::OGL33FrameBuffer(*this);

	result->SetColorTarget(glColor);

	if (usage == DepthUsage::Depth) {
		result->SetDepthTarget(glDepth);
	}
	else { //usage == DepthUsage::DepthStencil
		result->SetDepthStencilTarget(glDepth);
	}

	return result;
}

void Renderer::OGL33Context::DestroyFrameBuffer(const IFrameBuffer* buffer) const {
	const Renderer::OGL33FrameBuffer* glBuffer = (const Renderer::OGL33FrameBuffer*)buffer;
	glBuffer->~OGL33FrameBuffer();
	Renderer::OGL33Internal::Free(buffer);
}

Renderer::IBufferData* Renderer::OGL33Context::CreateBuffer() const {
	OGL33BufferData* result = (OGL33BufferData*)Renderer::OGL33Internal::Alloc(sizeof(Renderer::OGL33BufferData));
	new (result)OGL33BufferData(*this);
	return result;
}

void Renderer::OGL33Context::DestroyBuffer(const IBufferData* buffer) const {
	const OGL33BufferData* glBuffer = (const OGL33BufferData*)buffer;
	glBuffer->~OGL33BufferData();
	Renderer::OGL33Internal::Free(buffer);
}

Renderer::ITexture* Renderer::OGL33Context::CreateTexture() const {
	OGL33Texture* result = (OGL33Texture*)Renderer::OGL33Internal::Alloc(sizeof(OGL33Texture));
	new(result)OGL33Texture(*this);
	return result;
}

void Renderer::OGL33Context::DestroyTexture(const Renderer::ITexture* texture) const {
	const OGL33Texture* glTex = (const OGL33Texture*)texture;
	glTex->~OGL33Texture();
	Renderer::OGL33Internal::Free(texture);
}

Renderer::IShader* Renderer::OGL33Context::CreateShader(const char* vertex, const char* fragment) const {
	OGL33Shader* result = (OGL33Shader*)Renderer::OGL33Internal::Alloc(sizeof(OGL33Shader));
	new(result)OGL33Shader(*this, vertex, fragment);
	return result;
}

void Renderer::OGL33Context::DestroyShader(const IShader* shader) const {
	const OGL33Shader* glShader = (const OGL33Shader*)shader;
	glShader->~OGL33Shader();
	Renderer::OGL33Internal::Free(shader);
}

Renderer::IRasterState* Renderer::OGL33Context::CreateRasterState() const {
	OGL33RasterState* result = (Renderer::OGL33RasterState*)Renderer::OGL33Internal::Alloc(sizeof(Renderer::OGL33RasterState));
	new(result)Renderer::OGL33RasterState(*this);
	return result;
}

void Renderer::OGL33Context::DestroyRasterState(const IRasterState* state) const {
	const OGL33RasterState* glState = (const OGL33RasterState*)state;
	glState->~OGL33RasterState();
	Renderer::OGL33Internal::Free(state);
}

void Renderer::OGL33Context::BindFrameBuffer(const IFrameBuffer* buffer) {
	const OGL33FrameBuffer* glBuffer = (const OGL33FrameBuffer*)buffer;

	if (buffer == 0) {
		if (mBoundFrameBuffer != 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			mBoundFrameBuffer = 0;
		}
	}
	else {
		if (mBoundFrameBuffer != glBuffer->mFrameBuffer) {
			glBindFramebuffer(GL_FRAMEBUFFER, glBuffer->mFrameBuffer);
			mBoundFrameBuffer = glBuffer->mFrameBuffer;
		}
	}
}

void Renderer::OGL33Context::BindShader(const IShader* shader) {
	const OGL33Shader* glShader = (const OGL33Shader*)shader;

	if (shader == 0) {
		if (mBoundShader != 0) {
			glUseProgram(0);
			mBoundShader = 0;
		}
	}
	else {
		if (mBoundShader != glShader->GetHandle()) {
			glUseProgram(glShader->GetHandle());
			mBoundShader = glShader->GetHandle();
		}
	}
}

void Renderer::OGL33Context::BindAttribute(const IShaderAttribute* attrib, const IBufferView* buffer) {
	const OGL33BufferData* glBufferData = (const OGL33BufferData*)buffer->GetOwner();
	const OGL33ShaderAttribute* glAttrib = (const OGL33ShaderAttribute*)attrib;

	GLuint index = glAttrib->GetIndex();
	GLuint bufferHandle = glBufferData->GetHandle();

	glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
	glEnableVertexAttribArray(index);

	BufferDataType bufferType = buffer->GetType();
	if (bufferType == BufferDataType::Float) {
		glVertexAttribPointer(index, buffer->GetNumComponents(), Renderer::OGL33Internal::BufferDataTypeToGlEnum(bufferType), GL_FALSE, buffer->GetStride(), (const void*)buffer->GetOffset());
	}
	else {
		glVertexAttribIPointer(index, buffer->GetNumComponents(), Renderer::OGL33Internal::BufferDataTypeToGlEnum(bufferType), buffer->GetStride(), (const void*)buffer->GetOffset());
	}

	mBoundAttribs.push_back(std::pair<unsigned int, unsigned int>(bufferHandle, index));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::OGL33Context::BindTexture(const IShaderUniform* uniform, const ITextureSampler* sampler) {
	const OGL33Texture* glTexture = (const OGL33Texture*)sampler->GetOwner();
	const OGL33ShaderUniform* glUniform = (const OGL33ShaderUniform*)uniform;

	glActiveTexture(GL_TEXTURE0 + mCurrentTextureIndex);
	glBindTexture(GL_TEXTURE_2D, glTexture->GetHandle());
	glUniform1i(glUniform->GetIndex(), mCurrentTextureIndex); // Bind uniform X to sampler Y
	glActiveTexture(GL_TEXTURE0);

	mCurrentTextureIndex += 1;
}

void Renderer::OGL33Context::SetUniform(const IShaderUniform* uniform, void* data, unsigned int count) {
	// TODO: this one is gonna be long
}

void Renderer::OGL33Context::Unbind(UnbindTarget target) {
	// TODO: Frame Buffer
	// TODO: Shader
	// TODO: Attribute
	// Texture
}

void Renderer::OGL33Context::Clear(Renderer::Clear clear) {
	// TODO
}

void Renderer::OGL33Context::ApplyRasterState(const IRasterState* state) {
	// TODO
}

void Renderer::OGL33Context::GetViewport(unsigned int& x, unsigned int& y, unsigned int& w, unsigned int& h) const {
	// TODO
}

void Renderer::OGL33Context::SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
	// TODO
}

void Renderer::OGL33Context::Draw(DrawMode mode, const IBufferView* buffer, unsigned int instanceCount) const {
	// TODO
}

void Renderer::OGL33Context::Draw(DrawMode mode, const IBufferView* buffer, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const {
	// TODO
}

void Renderer::OGL33Context::Draw(DrawMode mode, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const {
	// TODO
}

void Renderer::OGL33Context::GetNDC(float& left, float& right, float& top, float& bottom, float& near, float& far) const {
	// TODO
}

void Renderer::OGL33Context::SetVSynch(bool val) {
	// TODO
}

bool Renderer::OGL33Context::GetVSynch() const {
	// TODO
}