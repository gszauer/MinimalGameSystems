#ifndef _H_OGL33TEXTURE_
#define _H_OGL33TEXTURE_

#include "../ITexture.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33Context;

	class OGL33Texture : public Renderer::ITexture {
		friend class OGL33Context;
	protected:
		const OGL33Context* mOwner;
		unsigned int mWidth;
		unsigned int mHeight;
		TextureType mType;
		GLuint mTexture;
		bool mHasMips;
	protected:
		OGL33Texture(); // Disabled
		OGL33Texture(const OGL33Texture&); // Disabled
		OGL33Texture& operator=(const OGL33Texture&); // Disabled
		OGL33Texture(const IContext& owner);
	public:
		~OGL33Texture();

		void Set(unsigned int width, unsigned int height, TextureType type, const void* data = 0, bool mips = false); // Data CAN be 0 to create an empty texture
		void Update(unsigned int x, unsigned int y, unsigned int width, unsigned int height, TextureType type, const void* data);

		// TODO: Maybe add a "Pull Data" option to like save screenshots or whatever?

		bool HasMips() const;
		void GenerateMips();

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		TextureType GetType() const;

		const ITextureSampler* CreateSampler(TextureWrapMode s = TextureWrapMode::Repeat, TextureWrapMode t = TextureWrapMode::Repeat, MinFilterType min = MinFilterType::NearestMipMapLinear, MagFilterType mag = MagFilterType::Linear) const;
		void DestroySampler(const ITextureSampler* sampler) const;

		const IContext* GetOwner() const;
		GLuint GetHandle() const; // TODO
	};
}

#endif