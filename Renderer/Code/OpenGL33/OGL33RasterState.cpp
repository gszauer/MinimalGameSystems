#include <RendererOpenGL33/OGL33RasterState.h>

Renderer::OGL33RasterState::OGL33RasterState() {
	mFillMode = Renderer::FillMode::Fill;
	mFillFace = Renderer::FillFace::Front;
	mCullMode = Renderer::CullMode::Back;
	mWindingOrder = Renderer::WindingOrder::CounterClockwise;
	mScissorState = Renderer::ScissorState::Disabled;
	mScissorRect[0] = mScissorRect[1] = 0;
	mScissorRect[2] = mScissorRect[3]= 0;
	mScissorCast[0] = mScissorCast[1] = 0;
	mScissorCast[2] = mScissorCast[3] = 0;
	mClearColor[0] = mClearColor[1] = 0.0f;
	mClearColor[2] = mClearColor[3] = 0.0f;
	mClearCast[0] = mClearCast[1] = 0.0f;
	mClearCast[2] = mClearCast[3] = 0.0f;
	mSrcBlend = BlendFactor::Disabled;
	mDstBlend = BlendFactor::Disabled;
	mLineWidth = 1.0f;
	mPointSize = 1.0f;
}

Renderer::IRasterState& Renderer::OGL33RasterState::operator=(const IRasterState& _other) {
	const Renderer::OGL33RasterState& other = (const Renderer::OGL33RasterState&)_other;
	mFillMode = other.mFillMode;
	mFillFace = other.mFillFace;
	mCullMode = other.mCullMode;
	mWindingOrder = other.mWindingOrder;
	mScissorState = other.mScissorState;
	mScissorRect[0] = other.mScissorRect[0];
	mScissorRect[2] = other.mScissorRect[2];
	mScissorCast[0] = other.mScissorCast[0];
	mScissorCast[2] = other.mScissorCast[2];
	mSrcBlend = other.mSrcBlend;
	mDstBlend = other.mDstBlend;
	mLineWidth = other.mLineWidth;
	mPointSize = other.mPointSize;
	mClearColor[0] = other.mClearColor[0];
	mClearColor[1] = other.mClearColor[1];
	mClearColor[2] = other.mClearColor[2];
	mClearColor[3] = other.mClearColor[3];
	
	return *this;
}


Renderer::OGL33RasterState::~OGL33RasterState() { }

void Renderer::OGL33RasterState::Apply(const OGL33RasterState& oldState, const OGL33RasterState& newState, bool force) {
	bool fillModeValid = newState.mFillMode == oldState.mFillMode;
	bool fillFaceValid = newState.mFillFace == oldState.mFillFace;
	if (!fillModeValid || !fillFaceValid || force) {
		GLenum mode = GL_FILL;
		if (newState.mFillMode == Renderer::FillMode::Line) {
			mode = GL_LINE;
		}
		else if (newState.mFillMode == Renderer::FillMode::Point) {
			mode = GL_POINT;
		}

		GLenum face = GL_FRONT;
		if (newState.mFillFace == Renderer::FillFace::Back) {
			face = GL_BACK;
		}
		else if (newState.mFillFace == Renderer::FillFace::Both) {
			face = GL_FRONT_AND_BACK;
		}
		glPolygonMode(face, mode);
	}

	bool cullingIsEnabled = newState.mCullMode != Renderer::CullMode::Disabled;
	bool cullingShouldEnabled = oldState.mCullMode != Renderer::CullMode::Disabled;
	if (cullingIsEnabled != cullingShouldEnabled || force) {
		if (cullingIsEnabled) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
	}

	if (newState.mCullMode != oldState.mCullMode || force) {
		GLenum mode = GL_BACK;
		if (newState.mCullMode == Renderer::CullMode::Front) {
			mode = GL_FRONT;
		}
		else if (newState.mCullMode == Renderer::CullMode::Both) {
			mode = GL_FRONT_AND_BACK;
		}
		glCullFace(mode);
	}

	bool windindIsEnabled = newState.mWindingOrder != Renderer::WindingOrder::Disabled;
	bool windindShouldEnabled = oldState.mWindingOrder != Renderer::WindingOrder::Disabled;
	if (windindIsEnabled != windindShouldEnabled || force) {
		if (windindIsEnabled) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
	}

	if (newState.mWindingOrder != oldState.mWindingOrder || force) {
		GLenum mode = GL_CCW;
		if (newState.mWindingOrder == Renderer::WindingOrder::Clockwise) {
			mode = GL_CW;
		}
		glFrontFace(mode);
	}

	if (newState.mScissorState != oldState.mScissorState || force) {
		if (newState.mScissorState == Renderer::ScissorState::Enabled) {
			glEnable(GL_SCISSOR_TEST);
		}
		else {
			glDisable(GL_SCISSOR_TEST);
		}
	}

	bool sameX = newState.mScissorRect[0] == oldState.mScissorRect[0];
	bool sameY = newState.mScissorRect[1] == oldState.mScissorRect[1];
	bool sameW = newState.mScissorRect[2] == oldState.mScissorRect[2];
	bool sameH = newState.mScissorRect[3] == oldState.mScissorRect[3];
	if (!sameX || !sameY || !sameW || !sameH || force) {
		glScissor(newState.mScissorRect[0], newState.mScissorRect[1], newState.mScissorRect[2], newState.mScissorRect[3]);
	}

	bool blendIsEnabled = newState.mSrcBlend != Renderer::BlendFactor::Disabled && newState.mDstBlend != Renderer::BlendFactor::Disabled;
	bool blendShouldEnabled = oldState.mSrcBlend != Renderer::BlendFactor::Disabled && oldState.mDstBlend != Renderer::BlendFactor::Disabled;
	if (blendIsEnabled != blendShouldEnabled || force) {
		if (blendIsEnabled) {
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}
	}

	bool sameSrc = newState.mSrcBlend == oldState.mSrcBlend;
	bool sameDst = newState.mDstBlend == oldState.mDstBlend;
	if (!sameSrc || !sameDst || force) {
		glBlendFunc(ConvertBlendFactor(newState.mSrcBlend), ConvertBlendFactor(newState.mDstBlend));
	}

	if (newState.mLineWidth != oldState.mLineWidth || force) {
		glLineWidth(newState.mLineWidth);
	}

	if (newState.mPointSize != oldState.mPointSize || force) {
		glPointSize(newState.mPointSize);
	}

	bool sameR = newState.mClearColor[0] == oldState.mClearColor[0];
	bool sameG = newState.mClearColor[1] == oldState.mClearColor[1];
	bool sameB = newState.mClearColor[2] == oldState.mClearColor[2];
	bool sameA = newState.mClearColor[3] == oldState.mClearColor[3];
	if (!sameR || !sameG || !sameB || !sameA || force) {
		glClearColor(newState.mClearColor[0], newState.mClearColor[1], newState.mClearColor[2], newState.mClearColor[3]);
	}
}

GLenum Renderer::OGL33RasterState::ConvertBlendFactor(BlendFactor factor){
	if (factor == Renderer::BlendFactor::Zero) {
		return GL_ZERO;
	}
	else if (factor == Renderer::BlendFactor::One) {
		return GL_ONE;
	}
	else if (factor == Renderer::BlendFactor::SrcColor) {
		return GL_SRC_COLOR;
	}
	else if (factor == Renderer::BlendFactor::OneMinusSrcColor) {
		return GL_ONE_MINUS_SRC_COLOR;
	}
	else if (factor == Renderer::BlendFactor::DstColor) {
		return GL_DST_COLOR;
	}
	else if (factor == Renderer::BlendFactor::OneMinusDestColor) {
		return GL_ONE_MINUS_DST_COLOR;
	}
	else if (factor == Renderer::BlendFactor::SrcAlpha) {
		return GL_SRC_ALPHA;
	}
	else if (factor == Renderer::BlendFactor::OneMinusSrcAlpha) {
		return GL_ONE_MINUS_SRC_ALPHA;
	}
	else if (factor == Renderer::BlendFactor::DstAlpha) {
		return GL_DST_ALPHA;
	}
	else if (factor == Renderer::BlendFactor::OneMinusdstAlpha) {
		return GL_ONE_MINUS_DST_ALPHA;
	}
	else if (factor == Renderer::BlendFactor::ConstantColor) {
		return GL_CONSTANT_COLOR;
	}
	else if (factor == Renderer::BlendFactor::OneMinusConstantColor) {
		return GL_ONE_MINUS_CONSTANT_COLOR;
	}
	else if (factor == Renderer::BlendFactor::ConstantAlpha) {
		return GL_CONSTANT_ALPHA;
	}
	else if (factor == Renderer::BlendFactor::OneMinusConstantAlpha) {
		return GL_ONE_MINUS_CONSTANT_ALPHA;
	}
	else if (factor == Renderer::BlendFactor::SrcAlphaSaturate) {
		return GL_SRC_ALPHA_SATURATE;
	}
	else if (factor == Renderer::BlendFactor::Src1Color) {
		return GL_SRC1_COLOR;
	}
	else if (factor == Renderer::BlendFactor::OneMinusSrc1Color) {
		return GL_ONE_MINUS_SRC1_COLOR;
	}
	else if (factor == Renderer::BlendFactor::Src1Alpha) {
		return GL_SRC1_ALPHA;
	}
	else if (factor == Renderer::BlendFactor::OneMinusSrc1Alpha) {
		return GL_ONE_MINUS_SRC1_ALPHA;
	}
	return (GLenum)0;
}

Renderer::FillMode Renderer::OGL33RasterState::GetFillMode() const { 
	return mFillMode;
}

Renderer::FillFace Renderer::OGL33RasterState::GetFillFace() const {
	return mFillFace;
}


Renderer::CullMode Renderer::OGL33RasterState::GetCullMode() const { 
	return mCullMode;
}

Renderer::WindingOrder Renderer::OGL33RasterState::GetWindingOrder() const {
	return mWindingOrder;
}

Renderer::ScissorState Renderer::OGL33RasterState::GetScissorState() const {
	return mScissorState;
}

Renderer::BlendFactor Renderer::OGL33RasterState::GetSrcBlendFactor() const {
	return mSrcBlend;
}

Renderer::BlendFactor Renderer::OGL33RasterState::GetDstBlendFactor() const {
	return mDstBlend;
}

float Renderer::OGL33RasterState::GetPointSize() const {
	return mPointSize;
}

float Renderer::OGL33RasterState::GetLineWidth() const {
	return mLineWidth;
}

const unsigned int* Renderer::OGL33RasterState::GetScissorRect() {
	mScissorCast[0] = (unsigned int)mScissorRect[0];
	mScissorCast[1] = (unsigned int)mScissorRect[1];
	mScissorCast[2] = (unsigned int)mScissorRect[2];
	mScissorCast[3] = (unsigned int)mScissorRect[3];
	return mScissorCast;
}

void Renderer::OGL33RasterState::SetFillMode(FillMode mode) {
	mFillMode = mode;
}

void Renderer::OGL33RasterState::SetCullMode(CullMode mode) {
	mCullMode = mode;
}

void Renderer::OGL33RasterState::SetFillFace(FillFace face) {
	mFillFace = face;
}

void Renderer::OGL33RasterState::WindingOrder(Renderer::WindingOrder order) {
	mWindingOrder = order;
}

void Renderer::OGL33RasterState::SetScissorState(ScissorState state) {
	mScissorState = state;
}

void Renderer::OGL33RasterState::SetScissorRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
	mScissorRect[0] = x;
	mScissorRect[1] = y;
	mScissorRect[2] = w;
	mScissorRect[3] = h;
}

void Renderer::OGL33RasterState::SetBlendFactor(BlendFactor src, BlendFactor dst) {
	mSrcBlend = src;
	mDstBlend = dst;
}

void Renderer::OGL33RasterState::SetLineWidth(float width) {
	mLineWidth = width;
}

void Renderer::OGL33RasterState::SetPointSize(float size) {
	mPointSize = size;
}

void Renderer::OGL33RasterState::SetClearColor(float r, float g, float b, float a) {
	mClearColor[0] = r;
	mClearColor[1] = g;
	mClearColor[2] = b;
	mClearColor[3] = a;
}
const float* Renderer::OGL33RasterState::GetClearColor() {
	mClearCast[0] = (float)mClearColor[0];
	mClearCast[1] = (float)mClearColor[1];
	mClearCast[2] = (float)mClearColor[2];
	mClearCast[3] = (float)mClearColor[3];
	return mClearCast;
}