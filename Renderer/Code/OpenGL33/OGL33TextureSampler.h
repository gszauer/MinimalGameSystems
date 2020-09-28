#ifndef _H_OGL33TEXTURESAMPLER_
#define _H_OGL33TEXTURESAMPLER_

#include "../ITextureSampler.h"

namespace Renderer {
	class OGL33Texture;

	class OGL33TextureSampler : public ITextureSampler {
		friend class OGL33Texture;
	protected:
		const OGL33Texture* mOwner;
		TextureWrapMode mWrapS;
		TextureWrapMode mWrapT;
		MinFilterType mMinFilter;
		MagFilterType mMagFilter;
	protected:
		OGL33TextureSampler(); // Disabled
		OGL33TextureSampler(const OGL33TextureSampler&); // Disabled
		OGL33TextureSampler& operator=(const OGL33TextureSampler&); // Disabled
		OGL33TextureSampler(const ITexture& owner);
	public:
		~OGL33TextureSampler();

		TextureWrapMode GetWrapS() const;
		TextureWrapMode GetWrapT() const;
		MinFilterType GetMinFilter() const;
		MagFilterType GetMagFilter() const;

		void SetWrapS(TextureWrapMode mode);
		void SetWrapT(TextureWrapMode mode);
		void SetWrapST(TextureWrapMode mode);
		void SetMinFilter(MinFilterType filter);
		void SetMagFilter(MagFilterType filter);

		const ITexture* GetOwner() const;
	};
}

#endif 