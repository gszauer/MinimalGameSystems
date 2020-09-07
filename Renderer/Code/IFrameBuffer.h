#ifndef _H_IFRAMEBUFFER_
#define _H_IFRAMEBUFFER_

namespace Renderer {
	class ITexture;
	class IContext;

	class IFrameBuffer {
	protected:
		IFrameBuffer(); // Disabled
		IFrameBuffer(const IFrameBuffer&); // Disabled
		virtual IFrameBuffer& operator=(const IFrameBuffer&); // Disabled
		inline IFrameBuffer(const IContext& owner) { }
	public:
		virtual inline ~IFrameBuffer() {}

		virtual void SetColorTarget(const ITexture* target) = 0;
		virtual const ITexture* GetColorTarget() const = 0;

		virtual void SetDepthTarget(const ITexture* target) = 0;
		virtual const ITexture* GetDepthTarget() const = 0;

		virtual void SetDepthStencilTarget(const ITexture* target) = 0;
		virtual const ITexture* GetDepthStencilTarget() const = 0;

		virtual const IContext* GetOwner() const = 0;
		virtual bool IsComplete() const = 0;
	};
}

#endif