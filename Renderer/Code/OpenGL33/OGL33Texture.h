#ifndef _H_OGL33TEXTURE_
#define _H_OGL33TEXTURE_

#include "../Renderer/ITexture.h"

namespace Renderer {
	class OGL33Texture : ITexture {
		friend class OGL33GraphicsDevice;
	protected:
		inline OGL33Texture() { }
		inline ~OGL33Texture() { }
	public:
		inline void Set(unsigned int width, unsigned int height, int channelCount, unsigned char* data) {}
		inline unsigned int GetWidth() const { return 0; }
		inline unsigned int GetHeight() const { return 0; }
		inline TextureType GetChannelCount() const { return TextureType::RGB; }
	};
}

#endif