#include "OGL33Texture.h"
#include "OGL33Internal.h"
#include "OGL33TextureSampler.h"
#include "OGL33Context.h"
#include "../ITexture.h"

Renderer::OGL33Texture::OGL33Texture(const IContext& owner) {
	mOwner = (const OGL33Context*)&owner;
	mWidth = 0;
	mHeight = 0;
	mType = TextureType::RGB;
	GLuint mTexture;
	mHasMips = false;
	glGenTextures(1, &mTexture);
}

Renderer::OGL33Texture::~OGL33Texture() {
	glDeleteTextures(1, &mTexture);
	mOwner = 0;
}

void Renderer::OGL33Texture::Set(unsigned int width, unsigned int height, TextureType type, const void* data, bool mips) {
	glBindTexture(GL_TEXTURE_2D, mTexture);
	GLint internalFormat = 0;
	GLenum format = 0;
	GLenum dataType = GL_UNSIGNED_BYTE;
	if (type == TextureType::RGB) {
		internalFormat = GL_RGB;
		format = GL_RGB;
	}
	else if (type == TextureType::RGB32) {
		internalFormat = GL_RGB32F;
		format = GL_RGB32F;
		dataType = GL_FLOAT;
	}
	else if (type == TextureType::RGBA) {
		internalFormat = GL_RGBA;
		format = GL_RGBA;
	}
	else if (type == TextureType::RGBA32) {
		internalFormat = GL_RGBA32F;
		format = GL_RGBA32F;
		dataType = GL_FLOAT;
	}
	else if (type == TextureType::R) {
		internalFormat = GL_RED;
		format = GL_RED;
	}
	else if (type == TextureType::R32) {
		internalFormat = GL_R32F;
		format = GL_R32F;
		dataType = GL_FLOAT;
	}
	else if (type == TextureType::Depth) {
		internalFormat = GL_DEPTH_COMPONENT;
		format = GL_DEPTH_COMPONENT;
	}
	else { // type == TextureType::DepthStencil
		internalFormat = GL_DEPTH_STENCIL;
		format = GL_DEPTH_STENCIL;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, data);

	mWidth = width;
	mHeight = height;
	mType = type;
	mHasMips = mips;

	if (mips) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::OGL33Texture::Update(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const void* data) {
	GLenum format = 0;
	GLenum dataType = GL_UNSIGNED_BYTE;
	
	if (mType == TextureType::RGB) {
		format = GL_RGB;
	}
	else if (mType == TextureType::RGB32) {
		format = GL_RGB32F;
	}
	else if (mType == TextureType::RGBA) {
		format = GL_RGBA;
	}
	else if (mType == TextureType::RGBA32) {
		format = GL_RGBA32F;
	}
	else if (mType == TextureType::R) {
		format = GL_RED;
	}
	else if (mType == TextureType::R32) {
		format = GL_R32F;
	}
	else if (mType == TextureType::Depth) {
		format = GL_DEPTH_COMPONENT;
	}
	else { // type == TextureType::DepthStencil
		format = GL_DEPTH_STENCIL;
	}
	
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, format, dataType, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool Renderer::OGL33Texture::HasMips() const {
	return mHasMips;
}

void Renderer::OGL33Texture::GenerateMips() {
	if (!mHasMips) {
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		mHasMips = true;
	}
}

unsigned int Renderer::OGL33Texture::GetWidth() const {
	return mWidth;
}

unsigned int Renderer::OGL33Texture::GetHeight() const {
	return mHeight;
}

Renderer::TextureType Renderer::OGL33Texture::GetType() const {
	return mType;
}

const Renderer::ITextureSampler* Renderer::OGL33Texture::CreateSampler(TextureWrapMode s, TextureWrapMode t, MinFilterType min, MagFilterType mag) const {
	const ITexture& iTexture = *this;
	OGL33TextureSampler* sampler = (OGL33TextureSampler*)Renderer::OGL33Internal::Alloc(sizeof(OGL33TextureSampler));
	new(sampler)OGL33TextureSampler(iTexture);
	sampler->SetWrapS(s);
	sampler->SetWrapT(t);
	sampler->SetMinFilter(min);
	sampler->SetMagFilter(mag);
	return sampler;
}

void Renderer::OGL33Texture::DestroySampler(const ITextureSampler* sampler) const {
	if (sampler != 0) {
		const OGL33TextureSampler* oglSampler = (const OGL33TextureSampler*)sampler;
		oglSampler->~OGL33TextureSampler();
	}
}

const Renderer::IContext* Renderer::OGL33Texture::GetOwner() const {
	return mOwner;
}

GLuint Renderer::OGL33Texture::GetHandle() const {
	return mTexture;
}

unsigned int Renderer::OGL33Texture::GetSizeInBytes() const {
	unsigned int bytesPerPixel = 0;
	if (mType == TextureType::RGB) {
		bytesPerPixel = 3;
	}
	else if (mType == TextureType::RGB32) {
		bytesPerPixel = 3 * 4;
	}
	else if (mType == TextureType::RGBA) {
		bytesPerPixel = 4;
	}
	else if (mType == TextureType::RGBA32) {
		bytesPerPixel = 4 * 4;
	}
	else if (mType == TextureType::R) {
		bytesPerPixel = 1;
	}
	else if (mType == TextureType::R32) {
		bytesPerPixel = 1 * 4;
	}
	else if (mType == TextureType::Depth) {
		bytesPerPixel = 1 * 4;
	}
	else { // type == TextureType::DepthStencil
		bytesPerPixel = 1 * 4;
	}

	return mWidth * mHeight * bytesPerPixel;
}

void Renderer::OGL33Texture::GetData(void* target, unsigned int level) const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	GLenum format = 0;
	GLenum dataType = GL_UNSIGNED_BYTE;

	if (mType == TextureType::RGB) {
		format = GL_RGB;
	}
	else if (mType == TextureType::RGB32) {
		format = GL_RGB32F;
	}
	else if (mType == TextureType::RGBA) {
		format = GL_RGBA;
	}
	else if (mType == TextureType::RGBA32) {
		format = GL_RGBA32F;
	}
	else if (mType == TextureType::R) {
		format = GL_RED;
	}
	else if (mType == TextureType::R32) {
		format = GL_R32F;
	}
	else if (mType == TextureType::Depth) {
		format = GL_DEPTH_COMPONENT;
	}
	else { // type == TextureType::DepthStencil
		format = GL_DEPTH_STENCIL;
	}

	glGetTexImage(GL_TEXTURE_2D, level, format, dataType, target);
}