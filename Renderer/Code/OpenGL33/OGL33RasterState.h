#ifndef _H_OGL33RASTERSTATE_
#define _H_OGL33RASTERSTATE_

#include "../Renderer/IRasterState.h"
#include "glad.h"

// TODO: Fully quantify everything to the Renderer namespace

namespace Renderer {
	class OGL33RasterState : public IRasterState {
		friend class OGL33GraphicsDevice;
	protected:
		FillMode mFillMode;
		FillFace mFillFace;
		CullMode mCullMode;
		Renderer::WindingOrder mWindingOrder;
		ScissorState mScissorState;
		GLint mScissorRect[4];
		unsigned int mScissorCast[4];
		GLfloat mClearColor[4];
		float mClearCast[4];
		BlendFactor mSrcBlend;
		BlendFactor mDstBlend;
		float mLineWidth;
		float mPointSize;
	protected:
		OGL33RasterState();
		~OGL33RasterState();
		static void Apply(const OGL33RasterState& oldState, const OGL33RasterState& newState, bool force);
		static GLenum ConvertBlendFactor(BlendFactor factor);
	public:
		IRasterState& operator=(const IRasterState& other);

		void SetFillMode(FillMode mode);
		void SetCullMode(CullMode mode);
		void SetFillFace(FillFace face);
		void WindingOrder(Renderer::WindingOrder order);
		void SetScissorState(ScissorState state);
		void SetScissorRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
		void SetBlendFactor(BlendFactor src, BlendFactor dst);
		void SetLineWidth(float width);
		void SetPointSize(float size);

		FillMode GetFillMode() const;
		FillFace GetFillFace() const;
		CullMode GetCullMode() const;
		Renderer::WindingOrder GetWindingOrder() const;
		ScissorState GetScissorState() const;
		BlendFactor GetSrcBlendFactor() const;
		BlendFactor GetDstBlendFactor() const;
		float GetPointSize() const;
		float GetLineWidth() const;

		void SetClearColor(float r, float g, float b, float a);
		const float* GetClearColor();
		const unsigned int* GetScissorRect();
	};
}

#endif