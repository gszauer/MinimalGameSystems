#include "OGL33TextureSampler.h"
#include "OGL33Texture.h"
#include "../ITexture.h"

const Renderer::ITexture* Renderer::OGL33TextureSampler::GetOwner() const {
	return (const Renderer::ITexture*)mOwner;
}

Renderer::OGL33TextureSampler::OGL33TextureSampler(const ITexture& owner) {
	mOwner = (const Renderer::OGL33Texture*)&owner;
	mWrapS = TextureWrapMode::Repeat;
	mWrapT = TextureWrapMode::Repeat;
	mMinFilter = MinFilterType::NearestMipMapLinear;
	mMagFilter = MagFilterType::Linear;
}

Renderer::OGL33TextureSampler::~OGL33TextureSampler() {
	mOwner = 0;
}

Renderer::TextureWrapMode Renderer::OGL33TextureSampler::GetWrapS() const {
	return mWrapS;
}

Renderer::TextureWrapMode Renderer::OGL33TextureSampler::GetWrapT() const {
	return mWrapT;
}

Renderer::MinFilterType Renderer::OGL33TextureSampler::GetMinFilter() const {
	return mMinFilter;
}

Renderer::MagFilterType Renderer::OGL33TextureSampler::GetMagFilter() const {
	return mMagFilter;
}

void Renderer::OGL33TextureSampler::SetWrapS(TextureWrapMode mode) {
	mWrapS = mode;
}

void Renderer::OGL33TextureSampler::SetWrapT(TextureWrapMode mode) {
	mWrapT = mode;
}

void Renderer::OGL33TextureSampler::SetWrapST(TextureWrapMode mode) {
	mWrapS = mWrapT = mode;
}

void Renderer::OGL33TextureSampler::SetMinFilter(MinFilterType filter) {
	mMinFilter = filter;
}

void Renderer::OGL33TextureSampler::SetMagFilter(MagFilterType filter) {
	mMagFilter = filter;
}