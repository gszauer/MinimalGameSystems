#ifndef _H_OGL33CONTEXT_
#define _H_OGL33CONTEXT_

#include "../IContext.h"
#include "OGL33Loader.h"
#include "OGL33RasterState.h"

#include <vector>
#include <utility>

namespace Renderer {
	class OGL33Context : public IContext {
	protected:
		GLuint mVAO;
		const char* mRendererName;

		GLuint mBoundFrameBuffer;
		GLuint mBoundShader;

		std::vector<std::pair<unsigned int, unsigned int>> mBoundAttribs;
		unsigned int mNumBoundAttribs;
		unsigned int mNumBoundTextures;

		OGL33RasterState mCurrentState;
		OGL33RasterState mDefaultState;

		bool mCullingEnabled;
		bool mBlendingEnabled;

		unsigned int mViewPort[4];

		bool mVSynchSupported;
		bool mVSynchOn;
	protected:
		OGL33Context(const OGL33Context&); // Disabled
		OGL33Context& operator=(const OGL33Context&); // Disabled
		OGL33Context();
	public:
		~OGL33Context();
		const char* GetName() const;

		// Create / Destroy Resources
		IFrameBuffer* CreateFrameBuffer(ITexture* colorAttachment = 0, ITexture* depthAttachment = 0, DepthUsage usage = DepthUsage::Depth) const;
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
		void BindFrameBuffer(const IFrameBuffer* buffer); // Calling with 0 will bind to default FBO
		void BindShader(const IShader* shader); // Calling with 0 will unbind shader
		void BindAttribute(const IShaderAttribute* attrib, const IBufferView* buffer); // Call with 0 to unbind
		void BindTexture(const IShaderUniform* uniform, const ITextureSampler* sampler); // No need to unbind
		void SetUniform(const IShaderUniform* uniform, const void* data, unsigned int count = 1);

		void Unbind(UnbindTarget target);

		// Modofy various states
		void Clear(Renderer::ClearTarget clear = Renderer::ClearTarget::All);
		void ApplyRasterState(const IRasterState* state);

		// Viewport
		void GetViewport(unsigned int& x, unsigned int& y, unsigned int& w, unsigned int& h) const;
		void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h);

		// Drawing geometry
		void Draw(DrawMode mode, const IBufferView* buffer, unsigned int instanceCount = 1) const;
		void Draw(DrawMode mode, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const;

		// NDC Info
		void GetNDC(float& left, float& right, float& top, float& bottom, float& near, float& far) const;

		// Vsynch
		void SetVSynch(bool val);
		bool GetVSynch() const;

		// Helpers
		GLuint GetBoundFBO() const;
		GLuint GetBoundShader() const;
		
		void PullNativeState();
	};
}

#endif