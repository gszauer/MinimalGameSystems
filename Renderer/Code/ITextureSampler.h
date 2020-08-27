#ifndef _H_ISAMPLER_
#define _H_ISAMPLER_

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
	
	class ITextureSampler {
	private:
		ITextureSampler(const ITextureSampler&);
		ITextureSampler& operator=(const ITextureSampler&);
	protected:
		inline ITextureSampler() { }
		virtual inline ~ITextureSampler() { };
	public:
		virtual TextureWrapMode GetWrapS() const = 0;
		virtual TextureWrapMode GetWrapT() const = 0;
		virtual MinFilterType GetMinFilter() const = 0;
		virtual MagFilterType GetMagFilter() const = 0;
		virtual const float* GetBorderColor() const = 0;
		virtual float GetMinLod() const = 0;
		virtual float GetMaxLod() const = 0;

		virtual void SetWrapS(TextureWrapMode mode) = 0;
		virtual void SetWrapT(TextureWrapMode mode) = 0;
		virtual void SetWrapST(TextureWrapMode mode) = 0;
		virtual void SetMinFilter(MinFilterType filter) = 0;
		virtual void SetMagFilter(MagFilterType filter) = 0;
		virtual void SetBorderColor(float r, float g, float b, float a) = 0;
		virtual void SetBorderColor(float* rgba) = 0;
		virtual void SetMinLod(float value) = 0;
		virtual void SetMaxLod(float value) = 0;
	};
}

#endif