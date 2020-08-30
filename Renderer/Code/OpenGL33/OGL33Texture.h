#ifndef _H_OGL33TEXTURE_
#define _H_OGL33TEXTURE_

#include "../ITexture.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33GraphicsDevice;

	class OGL33Texture : ITexture {
		friend class OGL33GraphicsDevice;
	protected:
		OGL33GraphicsDevice* mOwner;
		unsigned int mWidth;
		unsigned int mHeight;
		TextureType mType;
		GLuint mTexture;
		bool mHasMips;
	protected:
		OGL33Texture(); // Disabled
		OGL33Texture(const OGL33Texture&); // Disabled
		OGL33Texture& operator=(const OGL33Texture&); // Disabled
		OGL33Texture(const IGraphicsDevice&);
	public:
		~OGL33Texture();
		void Set(unsigned int width, unsigned int height, TextureType type, const void* data, bool mips = false);
		void Update(unsigned int x, unsigned int y, unsigned int width, unsigned int height, TextureType type, const void* data);

		bool HasMips() const;
		void GenerateMips();

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		TextureType GetType() const;

		const ITextureSampler* CreateSampler() const;
		void DestroySampler(const ITextureSampler* sampler) const;

		const IGraphicsDevice* GetOwner() const;
	};
}

#endif