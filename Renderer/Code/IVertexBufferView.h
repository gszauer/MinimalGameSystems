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

	class IVertexBufferData;

	class IVertexBufferView {	
	protected:
		IVertexBufferView(); // Disabled
		IVertexBufferView(const IVertexBufferView&); // Disabled
		IVertexBufferView& operator=(const IVertexBufferView&); // Disabled
		inline IVertexBufferView(const IVertexBufferData& data, unsigned int numComponents = 1, VertexBufferDataType type = VertexBufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0) { }
	public:
		inline virtual ~IVertexBufferView() { }
		
		virtual const IVertexBufferData* GetOwner() const = 0;
		virtual unsigned int GetNumComponents() const = 0;
		virtual VertexBufferDataType GetType() const = 0;
		virtual unsigned int GetStride() const = 0;
		virtual unsigned int GetOffset() const = 0;
	};
}

#endif 