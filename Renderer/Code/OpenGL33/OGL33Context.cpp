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

Renderer::OGL33Context::OGL33Context() {
	// TODO
}

Renderer::OGL33Context::~OGL33Context() {
	// TODO
}

const char* Renderer::OGL33Context::GetName() const {
	// TODO
}

const Renderer::IRasterState& Renderer::OGL33Context::GetDefaultRasterState() const {
	// TODO
}

Renderer::IFrameBuffer* Renderer::OGL33Context::CreateFrameBuffer(ITexture* colorAttachment = 0, ITexture* depthAttachment = 0) const {
	// TODO
}

void Renderer::OGL33Context::DestroyFrameBuffer(const IFrameBuffer* buffer) const {
	// TODO
}

Renderer::IBufferData* Renderer::OGL33Context::CreateBuffer() const {
	// TODO
}

void Renderer::OGL33Context::DestroyBuffer(const IBufferData* buffer) const {
	// TODO
}

Renderer::ITexture* Renderer::OGL33Context::CreateTexture() const {
	// TODO
}

void Renderer::OGL33Context::DestroyTexture(const Renderer::ITexture* texture) const {
	// TODO
}

Renderer::IShader* Renderer::OGL33Context::CreateShader(const char* vertex, const char* fragment) const {
	// TODO
}

void Renderer::OGL33Context::DestroyShader(const IShader* shader) const {
	// TODO
}

Renderer::IRasterState* Renderer::OGL33Context::CreateRasterState() const {
	// TODO
}

void Renderer::OGL33Context::DestroyRasterState(const IRasterState* state) const {
	// TODO
}

void Renderer::OGL33Context::SetFrameBuffer(const IFrameBuffer* buffer) {
	// TODO
}

void Renderer::OGL33Context::SetShader(const IShader* shader) {
	// TODO
}

void Renderer::OGL33Context::SetAttribute(const IShaderAttribute* attrib, const IBufferView* buffer) {
	// TODO
}

void Renderer::OGL33Context::SetTexture(const IShaderUniform* uniform, const ITextureSampler* sampler) {
	// TODO
}

void Renderer::OGL33Context::SetUniform(const IShaderUniform* uniform, void* data, unsigned int count) {
	// TODO
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