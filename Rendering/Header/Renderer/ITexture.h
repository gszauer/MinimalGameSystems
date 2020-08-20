#ifndef _H_ITEXTURE_
#define _H_ITEXTURE_

namespace Renderer {

#ifndef _H_RENDERER_TEXTURETYPE_
#define _H_RENDERER_TEXTURETYPE_
	enum class TextureType {
		RGB = 0,
		RGBA = 1,
		Depth = 2,
		Stencil = 3
	};
#endif

	class ITexture {
	private:
		ITexture(const ITexture&);
		ITexture& operator=(const ITexture&);
	protected:
		inline ITexture() { }
		virtual inline ~ITexture() { }
	public:
		virtual void Set(unsigned int width, unsigned int height, int channelCount, unsigned char* data) = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual TextureType GetChannelCount() const = 0;
	};
}

#endif