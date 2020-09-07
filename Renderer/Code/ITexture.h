#ifndef _H_ITEXTURE_
#define _H_ITEXTURE_

#include "ITextureSampler.h"

namespace Renderer {
	// Internal and external formats are always the same.
	// Other types, like BGR are not supported.
	enum class TextureType {
		R = 0,
		R32 = 1,
		RGB = 2,
		RGBA = 3,
		RGB32 = 4,
		RGBA32 = 5,
		Depth = 6,
		DepthStencil = 7
	};

	class IGraphicsDevice;

	class ITexture {
	protected:
		ITexture(); // Disabled
		ITexture(const ITexture&); // Disabled
		virtual ITexture& operator=(const ITexture&); // Disabled
		inline ITexture(const IGraphicsDevice&) {}
	public:
		virtual inline ~ITexture() { }
		virtual void Set(unsigned int width, unsigned int height, TextureType type, const void* data = 0, bool mips = false) = 0; // Data CAN be 0 to make empty texture
		virtual void Update(unsigned int x, unsigned int y, unsigned int width, unsigned int height, TextureType type, const void* data) = 0;
		
		virtual bool HasMips() const = 0;
		virtual void GenerateMips() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual TextureType GetType() const = 0;

		virtual const ITextureSampler* CreateSampler(TextureWrapMode s = TextureWrapMode::Repeat, TextureWrapMode t = TextureWrapMode::Repeat, MinFilterType min = MinFilterType::NearestMipMapLinear, MagFilterType mag = MagFilterType::Linear) const = 0;
		virtual void DestroySampler(const ITextureSampler* sampler) const = 0;

		virtual const IGraphicsDevice* GetOwner() const = 0;
	};
}

#endif