#ifndef _H_IVERTEXBUFFERVIEW_
#define _H_IVERTEXBUFFERVIEW_

namespace Renderer {
	enum class VertexBufferDataType {
		Float = 0,
		Int = 1,
		UInt = 2,
		Double = 3,
		UShort = 4
	};

	class IBufferData;

	class IVertexBufferView {	
	protected:
		inline IVertexBufferView() { }
	public:
		inline IVertexBufferView(const IBufferData& data, unsigned int numComponents = 1, VertexBufferDataType type = VertexBufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0) { }

		inline IVertexBufferView(const IVertexBufferView&) { }
		inline virtual IVertexBufferView& operator=(const IVertexBufferView&) { return *this; }
		inline virtual ~IVertexBufferView() { }
		
		virtual const IBufferData* GetBuffer() const = 0;
		virtual unsigned int GetNumComponents() const = 0;
		virtual VertexBufferDataType GetType() const = 0;
		virtual unsigned int GetStride() const = 0;
		virtual unsigned int GetOffset() const = 0;
	};
}

#endif 