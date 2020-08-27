#ifndef _H_OGL33_INDEXBUFFERVIEW_
#define _H_OGL33_INDEXBUFFERVIEW_

#include "../IIndexBufferView.h"

namespace Renderer {
	class OGL33BufferData;

	class OGL33IndexBufferView {
		friend class OGL33BufferData;
	protected:
		const OGL33BufferData* mOwner;
		unsigned int mCount;
		IndexBufferDataType mType;
		unsigned int mOffset;
	protected:
		OGL33IndexBufferView();
	public:
		OGL33IndexBufferView(const IBufferData& data, unsigned int count = 0, IndexBufferDataType type = IndexBufferDataType::UShort, unsigned int offset = 0);
		OGL33IndexBufferView(const OGL33IndexBufferView& other);
		OGL33IndexBufferView& operator=(const OGL33IndexBufferView& other);
		~OGL33IndexBufferView();

		const IBufferData* GetBuffer();
		unsigned int GetCount();
		IndexBufferDataType GetType();
		unsigned int GetOffset();
	};
}

#endif 