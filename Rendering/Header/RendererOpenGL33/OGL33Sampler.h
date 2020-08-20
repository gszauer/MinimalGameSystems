#ifndef _H_OGL33SAMPLER_
#define _H_OGL33SAMPLER_

#include "../Renderer/ISampler.h"

namespace Renderer {
	class OGL33Sampler : public ISampler {
		friend class OGL33GraphicsDevice;
	protected:
		inline OGL33Sampler() {}
		inline ~OGL33Sampler() {}
	public:
		inline TextureWrapMode GetWrapS() const { return TextureWrapMode::Repeat; }
		inline TextureWrapMode GetWrapT() const { return TextureWrapMode::Repeat; }
		inline MinFilterType GetMinFilter() const { return MinFilterType::Nearest; }
		inline MagFilterType GetMagFilter() const { return MagFilterType::Nearest; }
		inline const float* GetBorderColor() const { return 0; }
		inline float GetMinLod() const { return 0; }
		inline float GetMaxLod() const { return 0; }
		inline bool IsValid() const { return false;  }

		inline void SetWrapS(TextureWrapMode mode) {}
		inline void SetWrapT(TextureWrapMode mode) {}
		inline void SetWrapST(TextureWrapMode mode) {}
		inline void SetMinFilter(MinFilterType filter) {}
		inline void SetMagFilter(MagFilterType filter) {}
		inline void SetBorderColor(float r, float g, float b, float a) {}
		inline void SetBorderColor(float* rgba) {}
		inline void SetMinLod(float value) {}
		inline void SetMaxLod(float value) {}
	};
}

#endif