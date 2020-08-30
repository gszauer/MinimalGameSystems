#ifndef _H_OGL33_INDEXBUFFERVIEW_
#define _H_OGL33_INDEXBUFFERVIEW_

#include "../IIndexBufferView.h"

namespace Renderer {
	class OGL33IndexBufferData;

	class OGL33IndexBufferView : public IIndexBufferView {
		friend class OGL33IndexBufferData;
	protected:
		const OGL33IndexBufferData* mOwner;
		unsigned int mCount;
		IndexBufferDataType mType;
		unsigned int mOffset;
	protected:
		OGL33IndexBufferView(); // Disabled
		OGL33IndexBufferView(const OGL33IndexBufferView& other); // Disabled
		OGL33IndexBufferView& operator=(const OGL33IndexBufferView& other); // Disabled
		OGL33IndexBufferView(const IIndexBufferData& data, unsigned int count = 0, IndexBufferDataType type = IndexBufferDataType::UShort, unsigned int offset = 0);
	public:
		~OGL33IndexBufferView();

		const IIndexBufferData* GetOwner() const;
		unsigned int GetCount() const;
		IndexBufferDataType GetType() const;
		unsigned int GetOffset() const;
	};
}

#endif 