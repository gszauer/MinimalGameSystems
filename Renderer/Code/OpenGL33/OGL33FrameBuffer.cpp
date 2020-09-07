#include "OGL33FrameBuffer.h"

const Renderer::IContext* Renderer::OGL33FrameBuffer::GetOwner() const {
	return mOwner;
}

Renderer::OGL33FrameBuffer::OGL33FrameBuffer(const IContext& owner) {
	OGL33GraphicsDevice* mOwner = &owner;
	mColorAttachment = 0;
	mDepthAttachment = 0;
	mHasStancil = false;
	glGenFramebuffers(1, &mFrameBuffer);
}

Renderer::OGL33FrameBuffer::~OGL33FrameBuffer() {
	mOwner = 0;
	glDeleteFramebuffers(1, &mFrameBuffer);
}

void Renderer::OGL33FrameBuffer::SetColorTarget(const ITexture* target) {
	mColorAttachment = target;
}

const Renderer::ITexture* Renderer::OGL33FrameBuffer::GetColorTarget() const {
	return mColorAttachment;
}

void Renderer::OGL33FrameBuffer::SetDepthTarget(const ITexture* target) {
	mDepthAttachment = target;
	mHasStancil = false;
}

const Renderer::ITexture* Renderer::OGL33FrameBuffer::GetDepthTarget() const {
	if (!mHasStancil) {
		return mDepthAttachment;
	}
	return 0;
}

void Renderer::OGL33FrameBuffer::SetDepthStencilTarget(const ITexture* target) {
	mDepthAttachment = target;
	mHasStancil = true;
}

const Renderer::ITexture* Renderer::OGL33FrameBuffer::GetDepthStencilTarget() const {
	if (mHasStancil) {
		return mDepthAttachment;
	}
	return 0;
}