#ifndef _H_OGL33FRAMEBUFFER_
#define _H_OGL33FRAMEBUFFER_

#include "../IFrameBuffer.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33GraphicsDevice;
	class OGL33FrameBuffer : IFrameBuffer {
		friend class OGL33GraphicsDevice;
	protected:
		const OGL33GraphicsDevice* mOwner;
		const ITexture* mColorAttachment;
		const ITexture* mDepthAttachment;
		bool mHasStancil;
		GLuint mFrameBuffer;
	protected:
		OGL33FrameBuffer(); // Disabled
		OGL33FrameBuffer(const OGL33FrameBuffer&); // Disabled
		OGL33FrameBuffer& operator=(const OGL33FrameBuffer&); // Disabled
		OGL33FrameBuffer(const IContext& owner);
	public:
		~OGL33FrameBuffer();

		void SetColorTarget(const ITexture* target);
		const ITexture* GetColorTarget() const;

		void SetDepthTarget(const ITexture* target);
		const ITexture* GetDepthTarget() const;

		void SetDepthStencilTarget(const ITexture* target);
		const ITexture* GetDepthStencilTarget() const;

		const IContext* GetOwner() const;
	};
}

#endif