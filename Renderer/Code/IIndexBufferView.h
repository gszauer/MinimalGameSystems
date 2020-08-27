#ifndef _H_IINDEXBUFFERVIEW_
#define _H_IINDEXBUFFERVIEW_

namespace Renderer {
	enum class IndexBufferDataType {
		UByte = 1,
		UShort = 2,
		UInt = 4,
	};

	class IBufferData;

	class IIndexBufferView {
	protected:
		inline IIndexBufferView() { }
	public:
		inline IIndexBufferView(const IBufferData& data, unsigned int count = 0, IndexBufferDataType type = IndexBufferDataType::UShort, unsigned int offset = 0)) { }
		inline IIndexBufferView(const IIndexBufferView&) { }
		inline virtual IIndexBufferView& operator=(const IIndexBufferView&) { return *this; }
		inline virtual ~IIndexBufferView() { }
		
		virtual const IBufferData* GetBuffer() const = 0;
		virtual unsigned int GetCount() const = 0;
		virtual IndexBufferDataType GetType() const = 0;
		virtual unsigned int GetOffset() const = 0;
	};
}

#endif