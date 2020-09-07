#ifndef _H_OGL33RASTERSTATE_
#define _H_OGL33RASTERSTATE_

#include "../IRasterState.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33GraphicsDevice;

	class OGL33RasterState : public IRasterState {
		friend class OGL33GraphicsDevice;
	protected:
		OGL33GraphicsDevice* mOwner;
		FillMode mFillMode;
		FillFace mFillFace;
		CullMode mCullMode;
		Renderer::WindingOrder mWindingOrder;
		ScissorState mScissorState;
		GLint mScissorRect[4];
		BlendFactor mSrcBlend;
		BlendFactor mDstBlend;
		GLfloat mLineWidth;
		GLfloat mPointSize;
	protected:
		OGL33RasterState(); // Disabled
		OGL33RasterState(const IGraphicsDevice&);
	public:
		OGL33RasterState(const OGL33RasterState&);
		IRasterState& operator=(const OGL33RasterState& other);
		~OGL33RasterState();

		FillMode GetFillMode() const;
		void SetFillMode(FillMode mode);

		FillFace GetFillFace() const;
		void SetFillFace(FillFace face);

		CullMode GetCullMode() const;
		void SetCullMode(CullMode mode);

		Renderer::WindingOrder GetWindingOrder() const;
		void WindingOrder(Renderer::WindingOrder order);

		ScissorState GetScissorState() const;
		void SetScissorState(ScissorState state);

		void GetScissorRect(unsigned int& x, unsigned int& y, unsigned int& w, unsigned int& h) const;
		void SetScissorRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h);

		BlendFactor GetSrcBlend() const;
		BlendFactor GetDstBlend() const;
		void SetBlend(BlendFactor src, BlendFactor dst);

		float GetPointSize() const;
		void SetPointSize(float size);

		float GetLineWidth() const;
		void SetLineWidth(float width);

		const IGraphicsDevice* GetOwner() const;
	};
}

#endif