#ifndef _H_OGL33GRAPHICSDEVICE_
#define _H_OGL33GRAPHICSDEVICE_

#include "../Renderer/IGraphicsDevice.h"
#include "../RendererOpenGL33/OGL33RasterState.h"

#include "glad.h"
#undef APIENTRY
#include <windows.h>

namespace Renderer {
	class OGL33GraphicsDevice : public IGraphicsDevice {
			friend OGL33GraphicsDevice* CreateOpenGL33GraphicsDevice(HWND hwnd);
			friend void DestroyOpenGL33GraphicsDevice(OGL33GraphicsDevice* device);
	protected:
		HWND mOwner;
		HDC mDeviceContext;
		GLuint mVertexArrayObject;
		GLint mMajorVersion;
		GLint mMinorVersion;
		const GLubyte* mVersionString;
		bool mVSynchEnabled;
		OGL33RasterState mDefaultState;
		OGL33RasterState mLastRasterState;

		int mNumRasterStatesMade;
		int mNumFrameBuffersMade;
		int mNumIndexBuffersMade;
		int mNumVertexBuffersMade;
		int mNumTexturesMade;
		int mNumSamplersMade;
		int mNumShadersMade;
	protected:
		OGL33GraphicsDevice(HWND hwnd, HDC hdc, bool vsynch);
		~OGL33GraphicsDevice();
	public:
		HWND GetHwnd() const;
		HDC GetHdc() const;

		// State getters and setters
		int GetMajorVersion() const;
		int GetMinorVersion() const;
		const char* GetDescriptionString() const;

		const IRasterState* GetDefaultRasterState() const;
		void SetDefaultRasterState(const IRasterState* state);

		IFrameBuffer* MakeFrameBuffer(ITexture* colorAttachment = 0, ITexture* depthAttachment = 0, ITexture* stencilAttachment = 0); // TODO: Maybe instead of make, use Create???
		void DestroyFrameBuffer(IFrameBuffer* buffer);
		IIndexBuffer* MakeIndexBuffer(BufferUsageType usageType = BufferUsageType::Static);
		void DestroyIndexBuffer(IIndexBuffer* buffer);
		IVertexBuffer* MakeVertexBuffer(BufferUsageType usageType = BufferUsageType::Static);
		void DestroyVertexBuffer(IVertexBuffer* buffer);
		ITexture* MakeTexture();
		ITexture* MakeTexture(unsigned char* data, unsigned int width, unsigned int height, TextureType type);
		void DestroyTexture(ITexture* texture);
		ISampler* MakeSampler();
		void DestroySampler(ISampler* sampler);
		IShader* MakeShader();
		IShader* MakeNativeShader(const char* vertex, const char* fragment);
		IShader* MakeGenericShader(const char* vertex, const char* fragment);
		void DestroyShader(IShader* shader);
		IRasterState* MakeRasterState();
		void DestroyRasterState(IRasterState* state);
		void Clear(Renderer::Clear clear);

		void ApplyRasterState(const IRasterState* state);
		inline void BindDefaultFrameBuffer() {}
		inline void BindFrameBuffer(const IFrameBuffer* buffer) {}
		inline void UnbindFrameBuffer() {}
		inline void BindShader(IShader* shader) {}
		inline void UnbindShader() {}

		inline void Draw(DrawMode mode, const IIndexBuffer* buffer) const {}
		inline void Draw(DrawMode mode, unsigned int first, unsigned int numVerts) const {}
		inline void Draw(DrawMode mode, const IIndexBuffer* buffer, unsigned int first, unsigned int numVerts) const {}
		
		inline void DrawInstanced(DrawMode mode, const IIndexBuffer* buffer, unsigned int instanceCount) const {}
		inline void DrawInstanced(DrawMode mode, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const {}
		inline void DrawInstanced(DrawMode mode, const IIndexBuffer* buffer, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const {}

		inline IGPUTimer* CreateTimer() { return 0; }
		inline void DestroyTimer(IGPUTimer* timer) {}
		inline const float* GetViewportDimensions() const { return 0; }
		inline float GetViewportX() const { return 0; }
		inline float GetViewportY() const { return 0; }
		inline float GetViewportWidth() const { return 0; }
		inline float GetViewportHeight() const { return 0; }
		inline void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {}

		inline const float* GetNDCDimensions() const { return 0; }
		inline float GetNDCComponent(NDCComponent component) const { return 0; }
		inline void SetVSynch(bool val) {}
		inline bool GetVSynch() const { return 0; }
		void BeginFrame();
		void EndFrame();

		inline float GetCPURenderTime() const { return 0; }
		inline float GetGPURenderTime() const { return 0; }
		inline unsigned int GetNumDrawCalls() const { return 0; }
		inline unsigned int GetNumInstancedDrawCalls() const { return 0; }
		inline unsigned int GetNumUnInstancedDrawCalls() const { return 0; }

		inline unsigned int GetFrameBufferCount() const { return 0; }
		inline unsigned int GetIndexBufferCount() const { return 0; }
		inline unsigned int GetVertexBufferCount() const { return 0; }
		inline unsigned int GetTextureCount() const { return 0; }
		inline unsigned int GetSamplerCuont() const { return 0; }
		inline unsigned int GetShaderCount() const { return 0; }
		inline unsigned int GetRasterStateCount() const { return 0; }

		inline unsigned int GetRasterSwitchCount() const { return 0; }
		inline unsigned int GetFrameBufferSwitchCount()const { return 0; }
		inline unsigned int GetShaderSwitchCount()const { return 0; }
	};
}

#endif