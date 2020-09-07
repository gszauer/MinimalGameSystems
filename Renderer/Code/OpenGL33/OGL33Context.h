#ifndef _H_OGL33CONTEXT_
#define _H_OGL33CONTEXT_

#include "../IContext.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33Context : public IContext {
	protected:
		OGL33Context(const OGL33Context&); // Disabled
		OGL33Context& operator=(const OGL33Context&); // Disabled
		OGL33Context();
	public:
		~OGL33Context();
		const char* GetName() const;
		const IRasterState& GetDefaultRasterState() const;

		// Create / Destroy Resources
		IFrameBuffer* CreateFrameBuffer(ITexture* colorAttachment = 0, ITexture* depthAttachment = 0) const;
		void DestroyFrameBuffer(const IFrameBuffer* buffer) const;

		IBufferData* CreateBuffer() const;
		void DestroyBuffer(const IBufferData* buffer) const;

		ITexture* CreateTexture() const;
		void DestroyTexture(const ITexture* texture) const;

		IShader* CreateShader(const char* vertex, const char* fragment) const;
		void DestroyShader(const IShader* shader) const;

		IRasterState* CreateRasterState() const;
		void DestroyRasterState(const IRasterState* state) const;

		// Bind / unbind resources
		void SetFrameBuffer(const IFrameBuffer* buffer); // Calling with 0 will bind to default FBO
		void SetShader(const IShader* shader); // Calling with 0 will unbind shader
		void SetAttribute(const IShaderAttribute* attrib, const IBufferView* buffer); // Call with 0 to unbind
		void SetTexture(const IShaderUniform* uniform, const ITextureSampler* sampler); // No need to unbind
		void SetUniform(const IShaderUniform* uniform, void* data, unsigned int count = 1);

		// Modofy various states
		void Clear(Renderer::Clear clear = Renderer::Clear::All);
		void ApplyRasterState(const IRasterState* state);

		// Viewport
		void GetViewport(unsigned int& x, unsigned int& y, unsigned int& w, unsigned int& h) const;
		void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h);

		// Drawing geometry
		void Draw(DrawMode mode, const IBufferView* buffer, unsigned int instanceCount = 1) const;
		void Draw(DrawMode mode, const IBufferView* buffer, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const;
		void Draw(DrawMode mode, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const;

		// NDC Info
		void GetNDC(float& left, float& right, float& top, float& bottom, float& near, float& far) const;

		// Vsynch
		void SetVSynch(bool val);
		bool GetVSynch() const;
	};
}

#endif