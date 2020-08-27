#ifndef _H_IRASTERSTATE_
#define _H_IRASTERSTATE_

// TODO: Redo all enums to be power of twos!

namespace Renderer {
	enum class FillMode {
		Point = 0,
		Line = 1,
		Fill = 2
	};

	enum class FillFace {
		Front = 1,
		Back = 2,
		Both = 3
	};

	enum class CullMode {
		Disabled = 0,
		Front = 1,
		Back = 2,
		Both = 3
	};

	enum class WindingOrder {
		Disabled = 0,
		Clockwise = 1,
		CounterClockwise = 2
	};

	enum class ScissorState {
		Enabled = 0,
		Disabled = 1
	};

	enum class BlendFactor {
		Disabled = 0,
		Zero = 1,
		One = 2,
		SrcColor = 3,
		OneMinusSrcColor = 4,
		DstColor = 5,
		OneMinusDestColor = 6,
		SrcAlpha = 7,
		OneMinusSrcAlpha = 8,
		DstAlpha = 9,
		OneMinusdstAlpha = 10,
		ConstantColor = 11,
		OneMinusConstantColor = 12,
		ConstantAlpha = 13,
		OneMinusConstantAlpha = 14,
		SrcAlphaSaturate = 15,
		Src1Color = 16,
		OneMinusSrc1Color = 17,
		Src1Alpha = 18,
		OneMinusSrc1Alpha = 19
	};

	class IRasterState {
	private:
		IRasterState(const IRasterState&);
	protected:
		inline IRasterState() { }
		inline virtual ~IRasterState() { }
	public:
		virtual IRasterState& operator=(const IRasterState& other) { return *this; };
		virtual FillMode GetFillMode() const = 0;
		virtual void SetFillMode(FillMode mode) = 0;

		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual const float* GetClearColor() = 0;

		virtual FillFace GetFillFace() const = 0;
		virtual void SetFillFace(FillFace face) = 0;

		virtual CullMode GetCullMode() const = 0;
		virtual void SetCullMode(CullMode mode) = 0;

		virtual WindingOrder GetWindingOrder() const = 0;
		virtual void WindingOrder(WindingOrder order) = 0;

		virtual ScissorState GetScissorState() const = 0;
		virtual void SetScissorState(ScissorState state) = 0;
		virtual void SetScissorRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;

		virtual BlendFactor GetSrcBlendFactor() const = 0;
		virtual BlendFactor GetDstBlendFactor() const = 0;
		virtual void SetBlendFactor(BlendFactor src, BlendFactor dst) = 0;

		virtual float GetPointSize() const = 0;
		virtual void SetPointSize(float size) = 0;

		virtual float GetLineWidth() const = 0;
		virtual void SetLineWidth(float width) = 0;

		virtual const unsigned int* GetScissorRect() = 0;
	};
}

#endif