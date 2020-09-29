#ifndef _H_IBUFFERVIEW_
#define _H_IBUFFERVIEW_

namespace Renderer {
	enum class BufferDataType {
		Float = 0,
		Int = 1,
		UInt = 2,
		UShort = 4,
		UByte = 5
	};

	class IBufferData;

	class IBufferView {	
	protected:
		IBufferView(); // Disabled
		IBufferView(const IBufferView&); // Disabled
		IBufferView& operator=(const IBufferView&); // Disabled
		inline IBufferView(const IBufferData& data, unsigned int count, unsigned int numComponents = 1, BufferDataType type = BufferDataType::Float, unsigned int strideInBytes = 0, unsigned int offsetInBytes = 0) { }
	public:
		inline virtual ~IBufferView() { }
		
		virtual const IBufferData* GetOwner() const = 0;
		virtual unsigned int GetNumComponents() const = 0;
		
		virtual unsigned int GetStride() const = 0;
		virtual unsigned int GetOffset() const = 0;

		virtual BufferDataType GetType() const = 0;
		virtual unsigned int GetCount() const = 0;
	};
}

#endif 