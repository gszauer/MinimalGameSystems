#ifndef _H_ITEXTURE_
#define _H_ITEXTURE_

#include "ITextureSampler.h"

namespace Renderer {
	enum class TextureType {
		RGB = 0,
		RGBA = 1,
		RGB32 = 2,
		RGBA32 = 3,
		Depth = 4,
		Stencil = 5,
		DepthStencil = 6
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
		virtual void Set(unsigned int width, unsigned int height, TextureType type, void* data) = 0;
		
		virtual bool HasMips() const = 0;
		virtual void GenerateMips() const = 0;
		virtual void ClearMips() const = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual TextureType GetType() const = 0;

		virtual const ITextureSampler* CreateSampler() const = 0;
		virtual void DestroySampler(const ITextureSampler* sampler) const = 0;

		virtual const IGraphicsDevice* GetOwner() const = 0;
	};
}

#endif