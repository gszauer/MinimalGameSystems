#ifndef _H_ITEXTURESAMPLER_
#define _H_ITEXTURESAMPLER_

// https://www.geeks3d.com/20110908/opengl-3-3-sampler-objects-control-your-texture-units/]

namespace Renderer {
	enum class MinFilterType {
		Nearest = 0,
		Linear = 1,
		NearestMipMapNearest = 2,
		LinearMipMapNearest = 3,
		NearestMipMapLinear = 4,
		LinearMipMapLinear = 5
	};

	enum class MagFilterType {
		Nearest = 6,
		Linear = 7
	};

	enum class TextureWrapMode {
		Repeat = 8,
		ClampToEdge = 9,
		MirroredRepeat = 10,
		MirroredClampToEdge = 11
	};

	class ITexture;
	
	class ITextureSampler {
	protected:
		ITextureSampler(); // Disabled
		ITextureSampler(const ITextureSampler&); // Disabled
		virtual ITextureSampler& operator=(const ITextureSampler&); // Disabled
		inline ITextureSampler(const ITexture& owner) { }
	public:
		virtual inline ~ITextureSampler() { };
		
		virtual TextureWrapMode GetWrapS() const = 0;
		virtual TextureWrapMode GetWrapT() const = 0;
		virtual MinFilterType GetMinFilter() const = 0;
		virtual MagFilterType GetMagFilter() const = 0;

		virtual void SetWrapS(TextureWrapMode mode) = 0;
		virtual void SetWrapT(TextureWrapMode mode) = 0;
		virtual void SetWrapST(TextureWrapMode mode) = 0;
		virtual void SetMinFilter(MinFilterType filter) = 0;
		virtual void SetMagFilter(MagFilterType filter) = 0;

		virtual const ITexture* GetOwner() const = 0;
	};
}

#endif