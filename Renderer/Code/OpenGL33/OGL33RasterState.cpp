#include "OGL33RasterState.h"
#include "OGL33Context.h"

const Renderer::IContext* Renderer::OGL33RasterState::GetOwner() const {
	return mOwner;
}

Renderer::OGL33RasterState::OGL33RasterState(const IContext& owner) {
	mOwner = (const OGL33Context*)&owner;
	mFillMode = FillMode::Fill;
	mFillFace = FillFace::Front;
	mCullMode = CullMode::Back;
	mWindingOrder = WindingOrder::CounterClockwise;
	mScissorState = ScissorState::Disabled;
	mScissorRect[0] = mScissorRect[1] = mScissorRect[2] = mScissorRect[3] = 0;
	mSrcBlend = BlendFactor::One;
	mDstBlend = BlendFactor::Zero;
	mLineWidth = 1.0f;
	mPointSize = 1.0f;
}

Renderer::OGL33RasterState::OGL33RasterState(const OGL33RasterState& other) {
	*this = other;
}

Renderer::IRasterState& Renderer::OGL33RasterState::operator=(const OGL33RasterState& other) {
	mOwner = other.mOwner;
	mFillMode = other.mFillMode;
	mFillFace = other.mFillFace;
	mCullMode = other.mCullMode;
	mWindingOrder = other.mWindingOrder;
	mScissorState = other.mScissorState;
	mScissorRect[0] = other.mScissorRect[0];
	mScissorRect[1] = other.mScissorRect[1];
	mScissorRect[2] = other.mScissorRect[2];
	mScissorRect[3] = other.mScissorRect[3];
	mSrcBlend = other.mSrcBlend;
	mDstBlend = other.mDstBlend;
	mLineWidth = other.mLineWidth;
	mPointSize = other.mPointSize;
}

Renderer::OGL33RasterState::~OGL33RasterState() {
	mOwner = 0;
}

Renderer::FillMode Renderer::OGL33RasterState::GetFillMode() const {
	return mFillMode;
}

void Renderer::OGL33RasterState::SetFillMode(FillMode mode) {
	mFillMode = mode;
}

Renderer::FillFace Renderer::OGL33RasterState::GetFillFace() const {
	return mFillFace;
}

void Renderer::OGL33RasterState::SetFillFace(FillFace face) {
	mFillFace = face;
}

Renderer::CullMode Renderer::OGL33RasterState::GetCullMode() const {
	return mCullMode;
}

void Renderer::OGL33RasterState::SetCullMode(CullMode mode) {
	mCullMode = mode;
}

Renderer::WindingOrder Renderer::OGL33RasterState::GetWindingOrder() const {
	return mWindingOrder;
}

void Renderer::OGL33RasterState::WindingOrder(Renderer::WindingOrder order) {
	mWindingOrder = order;
}

Renderer::ScissorState Renderer::OGL33RasterState::GetScissorState() const {
	return mScissorState;
}

void Renderer::OGL33RasterState::SetScissorState(ScissorState state) {
	mScissorState = state;
}

void Renderer::OGL33RasterState::GetScissorRect(unsigned int& x, unsigned int& y, unsigned int& w, unsigned int& h) const {
	x = (unsigned int)mScissorRect[0];
	y = (unsigned int)mScissorRect[1];
	w = (unsigned int)mScissorRect[2];
	h = (unsigned int)mScissorRect[3];
}

void Renderer::OGL33RasterState::SetScissorRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
	 mScissorRect[0] = (GLuint)x;
	 mScissorRect[1] = (GLuint)y;
	 mScissorRect[2] = (GLuint)w;
	 mScissorRect[3] = (GLuint)h;
}

Renderer::BlendFactor Renderer::OGL33RasterState::GetSrcBlend() const {
	return mSrcBlend;
}

Renderer::BlendFactor Renderer::OGL33RasterState::GetDstBlend() const {
	return mDstBlend;
}

void Renderer::OGL33RasterState::SetBlend(BlendFactor src, BlendFactor dst) {
	mSrcBlend = src;
	mDstBlend = dst;
}

float Renderer::OGL33RasterState::GetPointSize() const {
	return mPointSize;
}

void Renderer::OGL33RasterState::SetPointSize(float size) {
	mPointSize = size;
}


float Renderer::OGL33RasterState::GetLineWidth() const {
	return mLineWidth;
}

void Renderer::OGL33RasterState::SetLineWidth(float width) {
	mLineWidth = width;
}