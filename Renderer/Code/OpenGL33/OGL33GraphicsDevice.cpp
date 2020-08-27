#include <RendererOpenGL33/OGL33GraphicsDevice.h>
#include <RendererOpenGL33/OGL33RasterState.h>
#include <RendererOpenGL33/OGL33FrameBuffer.h>
#include <RendererOpenGL33/OGL33IndexBuffer.h>
#include <RendererOpenGL33/OGL33VertexBuffer.h>
#include <RendererOpenGL33/OGL33Texture.h>
#include <RendererOpenGL33/OGL33Sampler.h>
#include <RendererOpenGL33/OGL33Shader.h>

Renderer::OGL33GraphicsDevice::OGL33GraphicsDevice(HWND hwnd, HDC hdc, bool vsynch) {
	mOwner = hwnd;
	mDeviceContext = hdc;
	mVSynchEnabled = vsynch;

	glGenVertexArrays(1, &mVertexArrayObject);
	glBindVertexArray(mVertexArrayObject);

	glGetIntegerv(GL_MAJOR_VERSION, &mMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &mMinorVersion);
	mVersionString = glGetString(GL_VERSION);

	mDefaultState = mLastRasterState = OGL33RasterState();
	OGL33RasterState::Apply(mDefaultState, mDefaultState, true);
	mNumRasterStatesMade = 0;
	mNumFrameBuffersMade = 0;
	mNumIndexBuffersMade = 0;
	mNumVertexBuffersMade = 0;
	mNumTexturesMade = 0;
	mNumSamplersMade = 0;
	mNumShadersMade = 0;
}

Renderer::OGL33GraphicsDevice::~OGL33GraphicsDevice() {
}

HWND Renderer::OGL33GraphicsDevice::GetHwnd() const { 
	return mOwner;
}

HDC Renderer::OGL33GraphicsDevice::GetHdc() const {
	return mDeviceContext;
}

void Renderer::OGL33GraphicsDevice::EndFrame() {
	SwapBuffers(mDeviceContext);
	if (mVSynchEnabled) {
		glFinish();
	}
}

void Renderer::OGL33GraphicsDevice::BeginFrame() {
	glBindVertexArray(mVertexArrayObject);
}

int Renderer::OGL33GraphicsDevice::GetMajorVersion() const { 
	return (int)mMajorVersion;
}

int Renderer::OGL33GraphicsDevice::GetMinorVersion() const { 
	return (int)mMinorVersion;
}

const char* Renderer::OGL33GraphicsDevice::GetDescriptionString() const { 
	return (const char*)mVersionString;
}

const Renderer::IRasterState* Renderer::OGL33GraphicsDevice::GetDefaultRasterState() const {
	return (const Renderer::IRasterState*)(&mDefaultState);
}

void Renderer::OGL33GraphicsDevice::SetDefaultRasterState(const IRasterState* state) {
	mDefaultState = *state;
}


Renderer::IRasterState* Renderer::OGL33GraphicsDevice::MakeRasterState() { 
	mNumRasterStatesMade += 1;
	return new OGL33RasterState();
}

void Renderer::OGL33GraphicsDevice::DestroyRasterState(IRasterState* state) {
	mNumRasterStatesMade -= 1;
	delete (OGL33RasterState*)state;
}

void Renderer::OGL33GraphicsDevice::ApplyRasterState(const Renderer::IRasterState* _state) {
	const OGL33RasterState* state = (const OGL33RasterState*)_state;
	OGL33RasterState::Apply(mLastRasterState, *state, false);
	mLastRasterState = *state;
}

Renderer::IFrameBuffer* Renderer::OGL33GraphicsDevice::MakeFrameBuffer(ITexture* colorAttachment, ITexture* depthAttachment, ITexture* stencilAttachment) {
	mNumFrameBuffersMade += 1;
	return new OGL33FrameBuffer(colorAttachment, depthAttachment, stencilAttachment);
}

void Renderer::OGL33GraphicsDevice::DestroyFrameBuffer(IFrameBuffer* buffer) {
	mNumFrameBuffersMade -= 1;
	delete (OGL33FrameBuffer*)buffer;
}

Renderer::IIndexBuffer* Renderer::OGL33GraphicsDevice::MakeIndexBuffer(BufferUsageType usageType) {
	mNumIndexBuffersMade += 1;
	return new OGL33IndexBuffer(usageType);
}

void Renderer::OGL33GraphicsDevice::DestroyIndexBuffer(IIndexBuffer* buffer) {
	mNumIndexBuffersMade -= 1;
	delete (OGL33IndexBuffer*)buffer;
}

Renderer::IVertexBuffer* Renderer::OGL33GraphicsDevice::MakeVertexBuffer(BufferUsageType usageType) {
	mNumVertexBuffersMade += 1;
	return new OGL33VertexBuffer(usageType);
}
void Renderer::OGL33GraphicsDevice::DestroyVertexBuffer(IVertexBuffer* buffer) {
	mNumVertexBuffersMade -= 1;
	delete (OGL33VertexBuffer*)buffer;
}

Renderer::ITexture* Renderer::OGL33GraphicsDevice::MakeTexture() {
	mNumTexturesMade += 1;
	return new OGL33Texture();
}

Renderer::ITexture* Renderer::OGL33GraphicsDevice::MakeTexture(unsigned char* data, unsigned int width, unsigned int height, TextureType type) {
	mNumTexturesMade += 1;
	OGL33Texture* result = new OGL33Texture();
	int channelCount = 0;
	if (type == TextureType::Depth || type == TextureType::Stencil) {
		channelCount = 1;
	}
	else if (type == TextureType::RGB) {
		channelCount = 3;
	}
	else if (type == TextureType::RGBA) {
		channelCount = 4;
	}
	result->Set(width, height, channelCount, data);
	return result;
}

void Renderer::OGL33GraphicsDevice::DestroyTexture(ITexture* texture) {
	mNumTexturesMade -= 1;
	delete (OGL33Texture*)texture;
}

Renderer::ISampler* Renderer::OGL33GraphicsDevice::MakeSampler() {
	mNumSamplersMade += 1;
	return new OGL33Sampler();
}

void Renderer::OGL33GraphicsDevice::DestroySampler(ISampler* sampler) {
	mNumSamplersMade -= 1;
	delete (OGL33Sampler*)sampler;
}

Renderer::IShader* Renderer::OGL33GraphicsDevice::MakeShader() { 
	mNumShadersMade += 1;
	return new OGL33Shader();
}

Renderer::IShader* Renderer::OGL33GraphicsDevice::MakeNativeShader(const char* vertex, const char* fragment) { 
	mNumShadersMade += 1;
	OGL33Shader* result = new OGL33Shader();
	result->CompileNative(vertex, fragment);
	return result;
}

Renderer::IShader* Renderer::OGL33GraphicsDevice::MakeGenericShader(const char* vertex, const char* fragment) { 
	mNumShadersMade += 1;
	OGL33Shader* result = new OGL33Shader();
	result->CompileGeneric(vertex, fragment);
	return result;
}

void Renderer::OGL33GraphicsDevice::DestroyShader(IShader* shader) {
	mNumShadersMade -= 1;
	delete (OGL33Shader*)shader;
}

enum class Clear {
	Color = 2,
	Depth = 4,
	Stencil = 8,
	All = 16
};

void Renderer::OGL33GraphicsDevice::Clear(Renderer::Clear clear) {
	GLbitfield mask = 0;
	unsigned int c = (unsigned int)clear;
	if ((c & (1 << 1)) == (1 << 1)) { // 2 = Color
		mask |= GL_COLOR_BUFFER_BIT;
	}
	if ((c & (1 << 2)) == (1 << 2)) { // 4 = Depth
		mask |= GL_DEPTH_BUFFER_BIT;
	}
	if ((c & (1 << 3)) == (1 << 3)) { // 8 = Stencil
		mask |= GL_STENCIL_BUFFER_BIT;
	}
	glClear(mask);
}