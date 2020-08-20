#ifndef _H_IFRAMEBUFFER_
#define _H_IFRAMEBUFFER_

namespace Renderer {
	class ITexture;

	class IFrameBuffer {
	private:
		IFrameBuffer();
		IFrameBuffer(const IFrameBuffer&);
		IFrameBuffer& operator=(const IFrameBuffer&);
	protected:
		inline IFrameBuffer(const ITexture* color, const ITexture* depth, const ITexture* stencil) { }
		virtual inline ~IFrameBuffer() {}
	public:
		virtual void SetColorTarget(const ITexture* target) = 0;
		virtual const ITexture* GetColorTarget() const = 0;

		virtual void SetDepthTarget(const ITexture* target) = 0;
		virtual const ITexture* GetDepthTarget() const = 0;

		virtual void SetStencilTarget(const ITexture* target) = 0;
		virtual const ITexture* GetStencilTarget() const = 0;
	};
}

#endif