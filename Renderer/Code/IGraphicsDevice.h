#ifndef _H_IGRAPHICSDEVICE_
#define _H_IGRAPHICSDEVICE_

#include "IBufferData.h"
#include "ITexture.h"

namespace Renderer {
	class IFrameBuffer;
	class IGPUTimer;
	class ITexture;
	class IRasterState;
	class IBufferData;
	class IShader;
	class ISampler;

	enum class Clear {
		Color = (1 << 1),
		Depth = (1 << 2),
		Stencil = (1 << 3),
		ColorDepth = (1 << 1) | (1 << 2),
		All = (1 << 1) | (1 << 2) | (1 << 3)
	};

	enum class DrawMode {
		Points = 0,
		LineStrip = 1,
		LineLoop = 2,
		Lines = 3, 
		TriangleStrip = 4,
		TriangleFan = 5,
		Triangles = 6
	};

	enum class NDCComponent {
		Near = 0,
		Far = 1,
		Left = 2,
		Right = 3,
		Top = 4,
		Bottom = 5
	};

	class IGraphicsDevice {
	private:
		IGraphicsDevice(const IGraphicsDevice&);
		IGraphicsDevice& operator=(const IGraphicsDevice&);
	protected:
		inline IGraphicsDevice() { }
		virtual inline ~IGraphicsDevice() { }
	public:
		// State getters and setters
		virtual int GetMajorVersion() const = 0;
		virtual int GetMinorVersion() const = 0;
		virtual const char* GetDescriptionString() const = 0;

		virtual const IRasterState* GetDefaultRasterState() const = 0;
		virtual void SetDefaultRasterState(const IRasterState* state) = 0;

		// Create / Destroy Resources
		virtual IFrameBuffer* MakeFrameBuffer(ITexture* colorAttachment = 0, ITexture* depthAttachment = 0, ITexture* stencilAttachment = 0) = 0;
		virtual void DestroyFrameBuffer(IFrameBuffer* buffer) = 0;

		virtual IBufferData* MakeBuffer(BufferUsageType usageType = BufferUsageType::Static) = 0;
		virtual void DestroyBuffer(IBufferData* buffer) = 0;

		virtual ITexture* MakeTexture() = 0;
		virtual ITexture* MakeTexture(unsigned char* data, unsigned int width, unsigned int height, TextureType type) = 0;
		virtual void DestroyTexture(ITexture* texture) = 0;

		virtual ISampler* MakeSampler() = 0;
		virtual void DestroySampler(ISampler* sampler) = 0;

		virtual IShader* MakeShader() = 0;
		virtual IShader* MakeNativeShader(const char* vertex, const char* fragment) = 0;
		virtual IShader* MakeGenericShader(const char* vertex, const char* fragment) = 0;
		virtual void DestroyShader(IShader* shader) = 0;

		virtual IRasterState* MakeRasterState() = 0;
		virtual void DestroyRasterState(IRasterState* state) = 0;

		// Bind / unbind resources
		virtual void BindDefaultFrameBuffer() = 0;
		virtual void BindFrameBuffer(const IFrameBuffer* buffer) = 0;
		virtual void UnbindFrameBuffer() = 0;

		virtual void BindShader(const IShader* shader) = 0;
		virtual void UnbindShader() = 0;

		virtual void ApplyRasterState(const IRasterState* state) = 0;
		virtual void Clear(Clear clear) = 0;

		// Drawing geometry
		virtual void Draw(DrawMode mode, const IIndexBufferView* buffer) const = 0;
		virtual void Draw(DrawMode mode, unsigned int first, unsigned int numVerts) const = 0;
		virtual void Draw(DrawMode mode, const IIndexBufferView* buffer, unsigned int first, unsigned int numVerts) const = 0;

		virtual void DrawInstanced(DrawMode mode, const IIndexBufferView* buffer, unsigned int instanceCount) const = 0;
		virtual void DrawInstanced(DrawMode mode, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const = 0;
		virtual void DrawInstanced(DrawMode mode, const IIndexBufferView* buffer, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const = 0;

		// Timers
		virtual IGPUTimer* CreateTimer() = 0;
		virtual void DestroyTimer(IGPUTimer* timer) = 0;

		// Viewport
		virtual const float* GetViewportDimensions() const = 0;
		virtual float GetViewportX() const = 0;
		virtual float GetViewportY() const = 0;
		virtual float GetViewportWidth() const = 0;
		virtual float GetViewportHeight() const = 0;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;

		// NDC Info
		virtual const float* GetNDCDimensions() const = 0;
		virtual float GetNDCComponent(NDCComponent component) const = 0;

		// Vsynch
		virtual void SetVSynch(bool val) = 0;
		virtual bool GetVSynch() const = 0;

		// Debug Info
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual float GetCPURenderTime() const = 0; // Milliseconds
		virtual float GetGPURenderTime() const = 0; // Milliseconds
		virtual unsigned int GetNumDrawCalls() const = 0;
		virtual unsigned int GetNumInstancedDrawCalls() const = 0;
		virtual unsigned int GetNumUnInstancedDrawCalls() const = 0;

		virtual unsigned int GetFrameBufferCount() const = 0;
		virtual unsigned int GetBufferDataCount() const = 0;
		virtual unsigned int GetIndexViewCount() const = 0;
		virtual unsigned int GetVertexViewCount() const = 0;
		virtual unsigned int GetTextureCount() const = 0;
		virtual unsigned int GetSamplerCuont() const = 0;
		virtual unsigned int GetShaderCount() const = 0;
		virtual unsigned int GetRasterStateCount() const = 0;

		virtual unsigned int GetRasterSwitchCount() const = 0;
		virtual unsigned int GetFrameBufferSwitchCount() const = 0;
		virtual unsigned int GetShaderSwitchCount() const = 0;
	};
}

#endif