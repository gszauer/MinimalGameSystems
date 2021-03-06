#include "OGL33BufferData.h"
#include "OGL33BufferView.h"
#include "OGL33Context.h"
#include "OGL33FrameBuffer.h"
#include "OGL33Internal.h"
#include "OGL33Loader.h"
#include "OGL33RasterState.h"
#include "OGL33Shader.h"
#include "OGL33ShaderAttribute.h"
#include "OGL33ShaderUniform.h"
#include "OGL33Texture.h"
#include "OGL33TextureSampler.h"

namespace Renderer {
	namespace OGL33Internal {
		inline GLenum BufferDataTypeToGlEnum(BufferDataType type) {
			if (type == BufferDataType::Int) {
				return GL_INT;
			}
			else if (type == BufferDataType::UInt) {
				return GL_UNSIGNED_INT;
			}
			else if (type == BufferDataType::UShort) {
				return GL_UNSIGNED_SHORT;
			}
			else if (type == BufferDataType::UByte) {
				return GL_UNSIGNED_BYTE;
			}
			return GL_FLOAT;
		}

		inline GLenum FillModeToEnum(FillMode fillMode) {
			if (fillMode == FillMode::Point) {
				return GL_POINT;
			}
			else if (fillMode == FillMode::Line) {
				return GL_LINE;
			}
			return GL_FILL;
		}

		inline FillMode EnumToFillMode(GLenum glenum) {
			if (glenum == GL_POINT) {
				return FillMode::Point;
			}
			else if (glenum == GL_LINE) {
				return FillMode::Line;
			}
			return FillMode::Fill;
		}

		inline GLenum FillFaceToEnum(FillFace fillFace) {
			if (fillFace == FillFace::Front) {
				return GL_FRONT;
			}
			else if (fillFace == FillFace::Back) {
				return GL_BACK;
			}
			return GL_FRONT_AND_BACK;
		}

		inline FillFace EnumToFillFace(GLenum glenum) {
			if (glenum == GL_FRONT) {
				return FillFace::Front;
			}
			else if (glenum == GL_BACK) {
				return FillFace::Back;
			}
			return FillFace::Both;
		}

		inline GLenum CullModeToEnum(CullMode cullMode) {
			if (cullMode == CullMode::Front) {
				return GL_FRONT;
			}
			else if (cullMode == CullMode::Back) {
				return GL_BACK;
			}
			else if (cullMode == CullMode::Both) {
				return GL_FRONT_AND_BACK;
			}
			return GL_INVALID_ENUM;
		}

		inline CullMode EnumToCullMode(GLenum glenum) {
			if (glenum == GL_FRONT) {
				return CullMode::Front;
			}
			else if (glenum == GL_BACK) {
				return CullMode::Back;
			}
			else if (glenum == GL_FRONT_AND_BACK) {
				return CullMode::Both;
			}
			return CullMode::Disabled;
		}

		inline GLenum WindingOrderToEnum(WindingOrder windingOrder) {
			if (windingOrder == WindingOrder::Clockwise) {
				return GL_CW;
			}
			return GL_CCW;
		}

		inline WindingOrder EnumToWindingOrder(GLenum glenum) {
			if (glenum == GL_CW) {
				return WindingOrder::Clockwise;
			}
			return WindingOrder::CounterClockwise;
		}

		inline GLenum BlendFactorToEnum(BlendFactor blend) {
			if (blend == BlendFactor::Zero) {
				return GL_ZERO;
			}
			else if (blend == BlendFactor::One) {
				return GL_ONE;
			}
			else if (blend == BlendFactor::SrcColor) {
				return GL_SRC_COLOR;
			}
			else if (blend == BlendFactor::OneMinusSrcColor) {
				return GL_ONE_MINUS_SRC_COLOR;
			}
			else if (blend == BlendFactor::DstColor) {
				return GL_DST_COLOR;
			}
			else if (blend == BlendFactor::OneMinusDestColor) {
				return GL_ONE_MINUS_DST_COLOR;
			}
			else if (blend == BlendFactor::SrcAlpha) {
				return GL_SRC_ALPHA;
			}
			else if (blend == BlendFactor::OneMinusSrcAlpha) {
				return GL_ONE_MINUS_SRC_ALPHA;
			}
			else if (blend == BlendFactor::DstAlpha) {
				return GL_DST_ALPHA;
			}
			else if (blend == BlendFactor::OneMinusdstAlpha) {
				return GL_ONE_MINUS_DST_ALPHA;
			}
			else if (blend == BlendFactor::ConstantColor) {
				return GL_CONSTANT_COLOR;
			}
			else if (blend == BlendFactor::OneMinusConstantColor) {
				return GL_ONE_MINUS_CONSTANT_COLOR;
			}
			else if (blend == BlendFactor::ConstantAlpha) {
				return GL_CONSTANT_ALPHA;
			}
			else if (blend == BlendFactor::OneMinusConstantAlpha) {
				return GL_ONE_MINUS_CONSTANT_ALPHA;
			}
			else if (blend == BlendFactor::SrcAlphaSaturate) {
				return GL_SRC_ALPHA_SATURATE;
			}
			else if (blend == BlendFactor::Src1Color) {
				return GL_SRC1_COLOR;
			}
			else if (blend == BlendFactor::OneMinusSrc1Color) {
				return GL_ONE_MINUS_SRC1_COLOR;
			}
			else if (blend == BlendFactor::Src1Alpha) {
				return GL_SRC1_ALPHA;
			}
			else if (blend == BlendFactor::OneMinusSrc1Alpha) {
				return GL_ONE_MINUS_SRC1_ALPHA;
			}

			return GL_INVALID_ENUM;
		}

		inline BlendFactor EnumToBlendFactor(GLenum rgb, GLenum blend) {
			if (blend == GL_ZERO) {
				return BlendFactor::Zero;
			}
			else if (blend == GL_ONE) {
				return BlendFactor::One;
			}
			else if (blend == GL_SRC_COLOR) {
				return BlendFactor::SrcColor;
			}
			else if (blend == GL_ONE_MINUS_SRC_COLOR) {
				return BlendFactor::OneMinusSrcColor;
			}
			else if (blend == GL_DST_COLOR) {
				return BlendFactor::DstColor;
			}
			else if (blend == GL_ONE_MINUS_DST_COLOR) {
				return BlendFactor::OneMinusDestColor;
			}
			else if (blend == GL_SRC_ALPHA) {
				return BlendFactor::SrcAlpha;
			}
			else if (blend == GL_ONE_MINUS_SRC_ALPHA) {
				return BlendFactor::OneMinusSrcAlpha;
			}
			else if (blend == GL_DST_ALPHA) {
				return BlendFactor::DstAlpha;
			}
			else if (blend == GL_ONE_MINUS_DST_ALPHA) {
				return BlendFactor::OneMinusdstAlpha;
			}
			else if (blend == GL_CONSTANT_COLOR) {
				return BlendFactor::ConstantColor;
			}
			else if (blend == GL_ONE_MINUS_CONSTANT_COLOR) {
				return BlendFactor::OneMinusConstantColor;
			}
			else if (blend == GL_CONSTANT_ALPHA) {
				return BlendFactor::ConstantAlpha;
			}
			else if (blend == GL_ONE_MINUS_CONSTANT_ALPHA) {
				return BlendFactor::OneMinusConstantAlpha;
			}
			else if (blend == GL_SRC_ALPHA_SATURATE) {
				return BlendFactor::SrcAlphaSaturate;
			}
			else if (blend == GL_SRC1_COLOR) {
				return BlendFactor::Src1Color;
			}
			else if (blend == GL_ONE_MINUS_SRC1_COLOR) {
				return  BlendFactor::OneMinusSrc1Color;
			}
			else if (blend == GL_SRC1_ALPHA) {
				return BlendFactor::Src1Alpha;
			}
			else if (blend == GL_ONE_MINUS_SRC1_ALPHA) {
				return BlendFactor::OneMinusSrc1Alpha;
			}

			return BlendFactor::Disabled;
		}

		inline GLenum DrawModeToEnum(DrawMode mode) {
			if (mode == DrawMode::Points) {
				return GL_POINTS;
			}
			else if (mode == DrawMode::LineStrip) {
				return GL_LINE_STRIP;
			}
			else if (mode == DrawMode::LineLoop) {
				return GL_LINE_LOOP;
			}
			else if (mode == DrawMode::Lines) {
				return GL_LINES;
			}
			else if (mode == DrawMode::TriangleStrip) {
				return GL_TRIANGLE_STRIP;
			}
			else if (mode == DrawMode::TriangleFan) {
				return GL_TRIANGLE_FAN;
			}
			return GL_TRIANGLES;
		}
	}
}

Renderer::OGL33Context::OGL33Context() : mCurrentState(*this), mDefaultState(*this) {
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	mRendererName = "OpenGL 3.3 Core";
	mBoundFrameBuffer = 0;
	mBoundShader = 0;
	mNumBoundTextures = 0;
	mNumBoundAttribs = 0;
	mCullingEnabled = false; 
	mBlendingEnabled = false; 
	mViewPort[0] = mViewPort[1] = mViewPort[2] = mViewPort[3] = 0;

	mVSynchOn = false; 
	mVSynchSupported = false; 

	PullNativeState();

	mDefaultState = mCurrentState;
}

Renderer::OGL33Context::~OGL33Context() {
	mBoundAttribs.clear();
	mNumBoundAttribs = 0;
	mNumBoundTextures = 0;

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &mVAO);
}

const char* Renderer::OGL33Context::GetName() const {
	return mRendererName;
}

Renderer::IFrameBuffer* Renderer::OGL33Context::CreateFrameBuffer(ITexture* colorAttachment, ITexture* depthAttachment, DepthUsage usage) const {
	if (colorAttachment == 0 && depthAttachment == 0) {
		return 0;
	}

	OGL33Texture* glColor = (OGL33Texture*)colorAttachment;
	OGL33Texture* glDepth = (OGL33Texture*)depthAttachment;

	Renderer::OGL33FrameBuffer* result = (Renderer::OGL33FrameBuffer*)Renderer::OGL33Internal::Alloc(sizeof(Renderer::OGL33FrameBuffer));
	new (result) Renderer::OGL33FrameBuffer(*this);

	result->SetColorTarget(glColor);

	if (usage == DepthUsage::Depth) {
		result->SetDepthTarget(glDepth);
	}
	else { //usage == DepthUsage::DepthStencil
		result->SetDepthStencilTarget(glDepth);
	}

	return result;
}

void Renderer::OGL33Context::DestroyFrameBuffer(const IFrameBuffer* buffer) const {
	const Renderer::OGL33FrameBuffer* glBuffer = (const Renderer::OGL33FrameBuffer*)buffer;
	glBuffer->~OGL33FrameBuffer();
	Renderer::OGL33Internal::Free(buffer);
}

Renderer::IBufferData* Renderer::OGL33Context::CreateBuffer() const {
	OGL33BufferData* result = (OGL33BufferData*)Renderer::OGL33Internal::Alloc(sizeof(Renderer::OGL33BufferData));
	new (result)OGL33BufferData(*this);
	return result;
}

void Renderer::OGL33Context::DestroyBuffer(const IBufferData* buffer) const {
	const OGL33BufferData* glBuffer = (const OGL33BufferData*)buffer;
	glBuffer->~OGL33BufferData();
	Renderer::OGL33Internal::Free(buffer);
}

Renderer::ITexture* Renderer::OGL33Context::CreateTexture() const {
	OGL33Texture* result = (OGL33Texture*)Renderer::OGL33Internal::Alloc(sizeof(OGL33Texture));
	new(result)OGL33Texture(*this);
	return result;
}

void Renderer::OGL33Context::DestroyTexture(const Renderer::ITexture* texture) const {
	const OGL33Texture* glTex = (const OGL33Texture*)texture;
	glTex->~OGL33Texture();
	Renderer::OGL33Internal::Free(texture);
}

Renderer::IShader* Renderer::OGL33Context::CreateShader(const char* vertex, const char* fragment) const {
	OGL33Shader* result = (OGL33Shader*)Renderer::OGL33Internal::Alloc(sizeof(OGL33Shader));
	new(result)OGL33Shader(*this, vertex, fragment);
	return result;
}

void Renderer::OGL33Context::DestroyShader(const IShader* shader) const {
	const OGL33Shader* glShader = (const OGL33Shader*)shader;
	glShader->~OGL33Shader();
	Renderer::OGL33Internal::Free(shader);
}

Renderer::IRasterState* Renderer::OGL33Context::CreateRasterState() const {
	OGL33RasterState* result = (Renderer::OGL33RasterState*)Renderer::OGL33Internal::Alloc(sizeof(Renderer::OGL33RasterState));
	new(result)Renderer::OGL33RasterState(*this);
	return result;
}

void Renderer::OGL33Context::DestroyRasterState(const IRasterState* state) const {
	const OGL33RasterState* glState = (const OGL33RasterState*)state;
	glState->~OGL33RasterState();
	Renderer::OGL33Internal::Free(state);
}

void Renderer::OGL33Context::BindFrameBuffer(const IFrameBuffer* buffer) {
	const OGL33FrameBuffer* glBuffer = (const OGL33FrameBuffer*)buffer;

	if (buffer == 0) {
		if (mBoundFrameBuffer != 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			mBoundFrameBuffer = 0;
		}
	}
	else {
		if (mBoundFrameBuffer != glBuffer->mFrameBuffer) {
			glBindFramebuffer(GL_FRAMEBUFFER, glBuffer->mFrameBuffer);
			mBoundFrameBuffer = glBuffer->mFrameBuffer;
		}
	}
}

void Renderer::OGL33Context::BindShader(const IShader* shader) {
	const OGL33Shader* glShader = (const OGL33Shader*)shader;

	if (shader == 0) {
		if (mBoundShader != 0) {
			glUseProgram(0);
			mBoundShader = 0;
		}
	}
	else {
		if (mBoundShader != glShader->GetHandle()) {
			glUseProgram(glShader->GetHandle());
			mBoundShader = glShader->GetHandle();
		}
	}
}

void Renderer::OGL33Context::BindAttribute(const IShaderAttribute* attrib, const IBufferView* buffer) {
	const OGL33BufferData* glBufferData = (const OGL33BufferData*)buffer->GetOwner();
	const OGL33ShaderAttribute* glAttrib = (const OGL33ShaderAttribute*)attrib;

	GLuint index = glAttrib->GetIndex();
	GLuint bufferHandle = glBufferData->GetHandle();

	glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
	glEnableVertexAttribArray(index);

	BufferDataType bufferType = buffer->GetType();
	if (bufferType == BufferDataType::Float) {
		glVertexAttribPointer(index, buffer->GetNumComponents(), Renderer::OGL33Internal::BufferDataTypeToGlEnum(bufferType), GL_FALSE, buffer->GetStride(), (const void*)buffer->GetOffset());
	}
	else {
		glVertexAttribIPointer(index, buffer->GetNumComponents(), Renderer::OGL33Internal::BufferDataTypeToGlEnum(bufferType), buffer->GetStride(), (const void*)buffer->GetOffset());
	}

	if (mNumBoundAttribs < (unsigned int)mBoundAttribs.size()) {
		mBoundAttribs[mNumBoundAttribs].first = bufferHandle;
		mBoundAttribs[mNumBoundAttribs].second = index;
	}
	else {
		mBoundAttribs.push_back(std::pair<unsigned int, unsigned int>(bufferHandle, index));
	}
	mNumBoundAttribs += 1;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::OGL33Context::BindTexture(const IShaderUniform* uniform, const ITextureSampler* sampler) {
	const OGL33Texture* glTexture = (const OGL33Texture*)sampler->GetOwner();
	const OGL33ShaderUniform* glUniform = (const OGL33ShaderUniform*)uniform;

	glActiveTexture(GL_TEXTURE0 + mNumBoundTextures);
	glBindTexture(GL_TEXTURE_2D, glTexture->GetHandle());
	glUniform1i(glUniform->GetIndex(), mNumBoundTextures); // Bind uniform X to sampler Y
	glActiveTexture(GL_TEXTURE0);

	mNumBoundTextures += 1;
}

void Renderer::OGL33Context::SetUniform(const IShaderUniform* uniform, const void* data, unsigned int count) {
	const OGL33ShaderUniform* glUniform = (const OGL33ShaderUniform*)uniform;

	ShaderUniformType uniformType = uniform->GetType();
	bool isArray = glUniform->IsArray();
	unsigned int location = glUniform->GetIndex();

	const GLfloat* dataAsGlFloat = (const GLfloat*)data;
	const GLint* dataAsGlInt = (const GLint*)data;
	const GLuint* dataAsGlUInt = (const GLuint*)data;

	if (uniformType == ShaderUniformType::Float) {
		glUniform1f(location, *dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Vec2F) {
		glUniform2fv(location, count, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Vec3F) {
		glUniform3fv(location, count, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Vec4F) {
		glUniform4fv(location, count, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Int) {
		glUniform1i(location, *dataAsGlInt);
	}
	else if (uniformType == ShaderUniformType::Vec2I) {
		glUniform2iv(location, count, dataAsGlInt);
	}
	else if (uniformType == ShaderUniformType::Vec3I) {
		glUniform3iv(location, count, dataAsGlInt);
	}
	else if (uniformType == ShaderUniformType::Vec4I) {
		glUniform4iv(location, count, dataAsGlInt);
	}
	else if (uniformType == ShaderUniformType::UnsignedInt) {
		glUniform2ui(location, count, *dataAsGlUInt);
	}
	else if (uniformType == ShaderUniformType::Vec2U) {
		glUniform2uiv(location, count, dataAsGlUInt);
	}
	else if (uniformType == ShaderUniformType::Vec3U) {
		glUniform3uiv(location, count, dataAsGlUInt);
	}
	else if (uniformType == ShaderUniformType::Vec4U) {
		glUniform4uiv(location, count, dataAsGlUInt);
	}
	else if (uniformType == ShaderUniformType::Mat2F) {
		glUniformMatrix2fv(location, count, GL_FALSE, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Mat3F) {
		glUniformMatrix3fv(location, count, GL_FALSE, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Mat4F) {
		glUniformMatrix4fv(location, count, GL_FALSE, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Mat2x3F) {
		glUniformMatrix2x3fv(location, count, GL_FALSE, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Mat2x4F) {
		glUniformMatrix2x4fv(location, count, GL_FALSE, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Mat3x2F) {
		glUniformMatrix3x2fv(location, count, GL_FALSE, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Mat3x4F) {
		glUniformMatrix3x4fv(location, count, GL_FALSE, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Mat4X2F) {
		glUniformMatrix4x2fv(location, count, GL_FALSE, dataAsGlFloat);
	}
	else if (uniformType == ShaderUniformType::Mat4X3F) {
		glUniformMatrix4x3fv(location, count, GL_FALSE, dataAsGlFloat);
	}
	// Samplers are handled elsewhere
}

void Renderer::OGL33Context::Unbind(UnbindTarget target) {
	unsigned int uiTarget = (unsigned int)target;
	if (target & (unsigned int)UnbindTarget::FrameBuffer == (unsigned int)UnbindTarget::FrameBuffer) {
		if (mBoundFrameBuffer != 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			mBoundFrameBuffer = 0;
		}
	}
	if (target & (unsigned int)UnbindTarget::Shader == (unsigned int)UnbindTarget::Shader) {
		if (mBoundShader != 0) {
			glUseProgram(0);
			mBoundShader = 0;
		}
	}
	if (target & (unsigned int)UnbindTarget::Attribute == (unsigned int)UnbindTarget::Attribute) {
		for (unsigned int i = 0; i < mNumBoundAttribs; ++i) {
			GLuint bufferHandle = mBoundAttribs[i].first;
			GLuint index = mBoundAttribs[i].second;
			
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glDisableVertexAttribArray(index);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		mNumBoundAttribs = 0;
	}
	if (target & (unsigned int)UnbindTarget::Texture == (unsigned int)UnbindTarget::Texture) {
		for (unsigned int i = 0; i < mNumBoundTextures; ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glActiveTexture(GL_TEXTURE0);
		mNumBoundTextures = 0;
	}
}

void Renderer::OGL33Context::Clear(Renderer::ClearTarget clear) {
	GLenum clearFlags = 0;

	unsigned int uiClear = (unsigned int)clear;
	if (uiClear & (unsigned int)Renderer::ClearTarget::Color == (unsigned int)Renderer::ClearTarget::Color) {
		clearFlags |= GL_COLOR_BUFFER_BIT;
	}
	if (uiClear & (unsigned int)Renderer::ClearTarget::Depth == (unsigned int)Renderer::ClearTarget::Depth) {
		clearFlags |= GL_DEPTH_BUFFER_BIT;
	}
	if (uiClear & (unsigned int)Renderer::ClearTarget::Stencil == (unsigned int)Renderer::ClearTarget::Stencil) {
		clearFlags |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(clearFlags);
}

void Renderer::OGL33Context::ApplyRasterState(const IRasterState* state) {
	if (state == 0) {
		ApplyRasterState(&mDefaultState);
		return;
	}

	const OGL33RasterState* glState = (const OGL33RasterState *)&state;

	Renderer::FillMode fillMode = glState->mFillMode;
	Renderer::FillFace fillFace = glState->mFillFace;
	Renderer::CullMode cullMode = glState->mCullMode;
	Renderer::WindingOrder windingOrder = glState->mWindingOrder;
	Renderer::ScissorState scissorState = glState->mScissorState;
	GLint scissorRect[4] = {
		glState->mScissorRect[0],
		glState->mScissorRect[1],
		glState->mScissorRect[2],
		glState->mScissorRect[3]
	};
	Renderer::BlendFactor srcBlend = glState->mSrcBlend;
	Renderer::BlendFactor dstBlend = glState->mDstBlend;
	GLfloat lineWidth = glState->mLineWidth;
	GLfloat pointSize = glState->mPointSize;

	if (mCurrentState.mFillMode != fillMode || mCurrentState.mFillFace != fillFace) {
		glPolygonMode(OGL33Internal::FillFaceToEnum(fillFace), OGL33Internal::FillModeToEnum(fillMode));
		mCurrentState.mFillMode = fillMode;
		mCurrentState.mFillFace = fillFace;
	}
	
	

	if (mCurrentState.mWindingOrder != windingOrder) {
		glFrontFace(Renderer::OGL33Internal::WindingOrderToEnum(windingOrder));
		mCurrentState.mWindingOrder = windingOrder;
	}
	if (mCurrentState.mScissorState != scissorState) {
		if (scissorState == Renderer::ScissorState::Disabled) {
			glDisable(GL_SCISSOR_TEST);
		}
		else if (scissorState == Renderer::ScissorState::Enabled) {
			glEnable(GL_SCISSOR_TEST);
		}
		mCurrentState.mScissorState = scissorState;
	}
	if (mCurrentState.mScissorRect[0] != scissorRect[0] ||
		mCurrentState.mScissorRect[1] != scissorRect[1] ||
		mCurrentState.mScissorRect[2] != scissorRect[2] ||
		mCurrentState.mScissorRect[3] != scissorRect[3]) {
		
		glScissor(scissorRect[0], scissorRect[1], scissorRect[2], scissorRect[3]);

		mCurrentState.mScissorRect[0] = scissorRect[0];
		mCurrentState.mScissorRect[1] = scissorRect[1];
		mCurrentState.mScissorRect[2] = scissorRect[2];
		mCurrentState.mScissorRect[3] = scissorRect[3];
	}
	if (mCurrentState.mLineWidth != lineWidth) {
		glLineWidth(lineWidth);
		mCurrentState.mLineWidth = lineWidth;
	}
	if (mCurrentState.mPointSize != pointSize) {
		glPointSize(pointSize);
		mCurrentState.mPointSize = pointSize;
	}

	bool shouldEnableCulling = cullMode != Renderer::CullMode::Disabled;
	if (mCurrentState.mCullMode != cullMode || (mCullingEnabled != shouldEnableCulling)) {
		if (cullMode == Renderer::CullMode::Disabled) {
			if (mCullingEnabled) {
				glDisable(GL_CULL_FACE);
				mCullingEnabled = false;
			}
		}
		else {
			if (!mCullingEnabled) {
				glEnable(GL_CULL_FACE);
				mCullingEnabled = true;
			}
			glCullFace(OGL33Internal::CullModeToEnum(cullMode));
		}
		mCurrentState.mCullMode = cullMode;
	}
	
	GLenum src = Renderer::OGL33Internal::BlendFactorToEnum(srcBlend);
	GLenum dst = Renderer::OGL33Internal::BlendFactorToEnum(dstBlend);
	bool shouldEnableBlend = srcBlend != BlendFactor::Disabled && dstBlend != BlendFactor::Disabled;
	if ((mCurrentState.mSrcBlend != srcBlend || mCurrentState.mDstBlend != dstBlend) || mBlendingEnabled != shouldEnableBlend) {
		bool disabled = srcBlend == BlendFactor::Disabled || dstBlend == BlendFactor::Disabled;

		if (!disabled) {
			if (!mBlendingEnabled) {
				glEnable(GL_BLEND);
				mBlendingEnabled = true;
			}
			//glBlendFunc(src, dst);
			glBlendFuncSeparate(src, src, dst, dst);
		}
		else {
			if (mBlendingEnabled) {
				glDisable(GL_BLEND);
				mBlendingEnabled = false;
			}
		}

		mCurrentState.mSrcBlend = srcBlend;
		mCurrentState.mDstBlend = dstBlend;
	}
}

void Renderer::OGL33Context::GetViewport(unsigned int& x, unsigned int& y, unsigned int& w, unsigned int& h) const {
	x = mViewPort[0];
	y = mViewPort[1];
	w = mViewPort[2];
	h = mViewPort[3];
}

void Renderer::OGL33Context::SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
	if (x != mViewPort[0] || y != mViewPort[1] || w != mViewPort[2] || h != mViewPort[3]) {

		glViewport((GLint)x, (GLint)y, (GLsizei)w, (GLsizei)h);

		mViewPort[0] = x;
		mViewPort[1] = y;
		mViewPort[2] = w;
		mViewPort[3] = h;
	}
}

void Renderer::OGL33Context::Draw(DrawMode mode, const IBufferView* buffer, unsigned int instanceCount) const {
	const OGL33BufferView* glBufferView = (const OGL33BufferView*)buffer;
	const OGL33BufferData* glBufferData = (const OGL33BufferData*)glBufferView->GetOwner();

	GLenum glMode = Renderer::OGL33Internal::DrawModeToEnum(mode);
	GLenum glType = Renderer::OGL33Internal::BufferDataTypeToGlEnum(glBufferView->GetType());
	GLuint handle = glBufferData->GetHandle();
	GLsizei count = (GLsizei)glBufferView->GetCount();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
	glDrawElementsInstanced(glMode, count, glType, (const void*)glBufferView->GetOffset(), instanceCount);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::OGL33Context::Draw(DrawMode mode, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const {
	GLenum glMode = Renderer::OGL33Internal::DrawModeToEnum(mode);
	glDrawArraysInstanced(glMode, first, numVerts, instanceCount);
}

GLuint Renderer::OGL33Context::GetBoundFBO() const {
	return mBoundFrameBuffer;
}

GLuint Renderer::OGL33Context::GetBoundShader() const {
	return mBoundShader;
}

void Renderer::OGL33Context::GetNDC(float& left, float& right, float& top, float& bottom, float& near, float& far) const {
	left = -1.0f;
	right = 1.0f;
	bottom = -1.0f;
	top = 1.0f;
	near = -1.0f;
	far = 1.0f;
}

void Renderer::OGL33Context::SetVSynch(bool val) {
	if (mVSynchSupported) {
		if (val) {
			if (!mVSynchOn) {
				if (wglSwapIntervalEXT(1)) {
					mVSynchOn = true;
				}
			}
		}
		else {
			if (mVSynchOn) {
				wglSwapIntervalEXT(0);
				mVSynchOn = false;
			}
		}
	}
}

bool Renderer::OGL33Context::GetVSynch() const {
	return mVSynchSupported && mVSynchOn;
}

void Renderer::OGL33Context::PullNativeState() {
	mVSynchSupported = strstr(wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;
	if (mVSynchSupported) {
		mVSynchOn = wglGetSwapIntervalEXT() > 0;
		if (!mVSynchOn) {
			SetVSynch(true);
		}
	}

	mCullingEnabled = glIsEnabled(GL_CULL_FACE);
	mBlendingEnabled = glIsEnabled(GL_BLEND);

	GLint viewport[4] = { 0 };
	glGetIntegerv(GL_VIEWPORT, viewport);
	mViewPort[0] = (unsigned int)viewport[0];
	mViewPort[1] = (unsigned int)viewport[1];
	mViewPort[2] = (unsigned int)viewport[2];
	mViewPort[3] = (unsigned int)viewport[3];

	GLint polyMode[2] = { 0 };
	glGetIntegerv(GL_POLYGON_MODE, polyMode);

	GLint cullFace = 0;
	glGetIntegerv(GL_CULL_FACE_MODE, &cullFace);

	GLint frontFace = 0;
	glGetIntegerv(GL_FRONT_FACE, &frontFace);

	GLint scissor[4] = { 0 };
	glGetIntegerv(GL_SCISSOR_BOX, scissor);
	mCurrentState.mScissorRect[0] = (unsigned int)scissor[0];
	mCurrentState.mScissorRect[1] = (unsigned int)scissor[1];
	mCurrentState.mScissorRect[2] = (unsigned int)scissor[2];
	mCurrentState.mScissorRect[3] = (unsigned int)scissor[3];

	mCurrentState.mFillMode = Renderer::OGL33Internal::EnumToFillMode(polyMode[0]);
	mCurrentState.mFillFace = Renderer::OGL33Internal::EnumToFillFace(polyMode[1]);
	mCurrentState.mCullMode = Renderer::OGL33Internal::EnumToCullMode(cullFace);
	mCurrentState.mWindingOrder = Renderer::OGL33Internal::EnumToWindingOrder(frontFace);
	mCurrentState.mScissorState = glIsEnabled(GL_SCISSOR_TEST) ? Renderer::ScissorState::Enabled : Renderer::ScissorState::Disabled;

	GLint srcBlend[2] = { 0 };
	glGetIntegerv(GL_BLEND_SRC_RGB, &srcBlend[0]);
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcBlend[1]);

	GLint dstBlend[2] = { 0 };
	glGetIntegerv(GL_BLEND_DST_RGB, &dstBlend[0]);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &dstBlend[1]);

	mCurrentState.mSrcBlend = Renderer::OGL33Internal::EnumToBlendFactor(srcBlend[0], srcBlend[1]);
	mCurrentState.mDstBlend = Renderer::OGL33Internal::EnumToBlendFactor(dstBlend[0], dstBlend[1]);
	
	glGetFloatv(GL_LINE_WIDTH, &mCurrentState.mLineWidth);
	glGetFloatv(GL_POINT_SIZE, &mCurrentState.mPointSize);
}