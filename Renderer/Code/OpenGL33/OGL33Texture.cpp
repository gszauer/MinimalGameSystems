#include "OGL33Texture.h"
#include "OGL33Internal.h"

Renderer::OGL33Texture::OGL33Texture(const IGraphicsDevice& owner) {
	mOwner = &owner;
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

void Renderer::OGL33Texture::Update(unsigned int x, unsigned int y, unsigned int width, unsigned int height, TextureType type, const void* data) {
	// TODO: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexSubImage2D.xhtml
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

const Renderer::ITextureSampler* Renderer::OGL33Texture::CreateSampler() const {
	// TODO
}

void Renderer::OGL33Texture::DestroySampler(const ITextureSampler* sampler) const {
	// TODO
}

const Renderer::IGraphicsDevice* Renderer::OGL33Texture::GetOwner() const {
	return mOwner;
}