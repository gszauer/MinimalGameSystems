#include "OGL33FrameBuffer.h"
#include "OGL33Context.h"
#include "OGL33Texture.h"

const Renderer::IContext* Renderer::OGL33FrameBuffer::GetOwner() const {
	return mOwner;
}

bool Renderer::OGL33FrameBuffer::IsComplete() const {
	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}


Renderer::OGL33FrameBuffer::OGL33FrameBuffer(const IContext& owner) {
	mOwner = (const OGL33Context*)&owner;
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
	const OGL33Texture* glTex = (const OGL33Texture*)target;
	mColorAttachment = target;

	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glTex->GetHandle(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, mOwner->GetBoundFBO());
}

const Renderer::ITexture* Renderer::OGL33FrameBuffer::GetColorTarget() const {
	return mColorAttachment;
}

void Renderer::OGL33FrameBuffer::SetDepthTarget(const ITexture* target) {
	const OGL33Texture* glTex = (const OGL33Texture*)target;
	mDepthAttachment = target;
	mHasStancil = false;

	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glTex->GetHandle(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, mOwner->GetBoundFBO());
}

const Renderer::ITexture* Renderer::OGL33FrameBuffer::GetDepthTarget() const {
	if (!mHasStancil) {
		return mDepthAttachment;
	}
	return 0;
}

void Renderer::OGL33FrameBuffer::SetDepthStencilTarget(const ITexture* target) {
	const OGL33Texture* glTex = (const OGL33Texture*)target;
	mDepthAttachment = target;
	mHasStancil = true;

	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, glTex->GetHandle(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, mOwner->GetBoundFBO());
}

const Renderer::ITexture* Renderer::OGL33FrameBuffer::GetDepthStencilTarget() const {
	if (mHasStancil) {
		return mDepthAttachment;
	}
	return 0;
}