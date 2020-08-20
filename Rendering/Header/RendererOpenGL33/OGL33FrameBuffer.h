#ifndef _H_OGL33FRAMEBUFFER_
#define _H_OGL33FRAMEBUFFER_

#include "../Renderer/IFrameBuffer.h"

namespace Renderer {
	class OGL33FrameBuffer : IFrameBuffer {
		friend class OGL33GraphicsDevice;
	protected:
		inline OGL33FrameBuffer(const ITexture* color, const ITexture* depth, const ITexture* stencil) : IFrameBuffer(color, depth, stencil) { }
		virtual inline ~OGL33FrameBuffer() {}
	public:
		inline void SetColorTarget(const ITexture* target) { }
		inline const ITexture* GetColorTarget() const { return 0; }
		
		inline void SetDepthTarget(const ITexture* target) { }
		inline const ITexture* GetDepthTarget() const { return 0; }

		inline void SetStencilTarget(const ITexture* target) { }
		inline const ITexture* GetStencilTarget() const { return 0; }
	};
}

#endif