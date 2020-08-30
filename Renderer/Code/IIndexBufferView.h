#ifndef _H_IINDEXBUFFERVIEW_
#define _H_IINDEXBUFFERVIEW_

namespace Renderer {
	enum class IndexBufferDataType {
		UByte = 1,
		UShort = 2,
		UInt = 4,
	};

	class IIndexBufferData;

	class IIndexBufferView {
	protected:
		IIndexBufferView(); // Disabled
		IIndexBufferView(const IIndexBufferView&); // Disabled
		IIndexBufferView& operator=(const IIndexBufferView&); // Disabled
		inline IIndexBufferView(const IIndexBufferData& data, unsigned int count = 0, IndexBufferDataType type = IndexBufferDataType::UShort, unsigned int offset = 0) { }
	public:
		inline virtual ~IIndexBufferView() { }
		
		virtual const IIndexBufferData* GetOwner() const = 0;
		virtual unsigned int GetCount() const = 0;
		virtual IndexBufferDataType GetType() const = 0;
		virtual unsigned int GetOffset() const = 0;
	};
}

#endif