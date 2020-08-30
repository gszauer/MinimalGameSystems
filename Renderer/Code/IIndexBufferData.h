#ifndef _H_IINDEXBUFFERDATA_
#define _H_IINDEXBUFFERDATA_

#include "IIndexBufferView.h"

namespace Renderer {
	enum class IndexBufferUsageType {
		Static = 0,
		Dynamic = 1
	};

	class IIndexBufferData {
	protected:
		inline IIndexBufferData() { }
		IIndexBufferData(const IIndexBufferData&); // Disabled
		IIndexBufferData& operator=(const IIndexBufferData&); // Disabled
	public:
		virtual inline ~IIndexBufferData() { }

		virtual void Set(unsigned int bytes, const void* data, IndexBufferUsageType type = IndexBufferUsageType::Static) = 0;
		virtual void Update(unsigned int offset, unsigned int bytes, const void* data) = 0;

		virtual IndexBufferUsageType GetUsage() const = 0;
		virtual unsigned int Size() const = 0;

		virtual const IIndexBufferView* CreateView(unsigned int count = 0, IndexBufferDataType type = IndexBufferDataType::UShort, unsigned int offset = 0) const = 0;
	};
}

#endif