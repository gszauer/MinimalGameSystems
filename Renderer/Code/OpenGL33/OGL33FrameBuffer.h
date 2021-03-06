#ifndef _H_OGL33FRAMEBUFFER_
#define _H_OGL33FRAMEBUFFER_

#include "../IFrameBuffer.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33Context;
	class OGL33FrameBuffer : IFrameBuffer {
		friend class OGL33Context;
	protected:
		const OGL33Context* mOwner;
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

		bool IsComplete() const;
		GLuint GetHandle() const;
	};
}

#endif