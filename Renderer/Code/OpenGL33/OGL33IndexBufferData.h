#ifndef _H_OGLINDEXBUFFERDATA_
#define _H_OGLINDEXBUFFERDATA_

#include "../IIndexBufferData.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33IndexBufferData : public IIndexBufferData {
		friend class OGL33GraphicsDevice;
	protected:
		IndexBufferUsageType mUsageType;
		unsigned int mSize;
		GLuint mBuffer;
	protected:
		OGL33IndexBufferData();
		OGL33IndexBufferData(const OGL33IndexBufferData&); // Disabled
		OGL33IndexBufferData& operator=(const OGL33IndexBufferData&); // Disabled
	public:
		~OGL33IndexBufferData();

		void Set(unsigned int bytes, const void* data, IndexBufferUsageType type = IndexBufferUsageType::Static);
		void Update(unsigned int offset, unsigned int bytes, const void* data);

		IndexBufferUsageType GetUsage() const;
		unsigned int Size() const;
		const IIndexBufferView* CreateView(unsigned int count = 0, IndexBufferDataType type = IndexBufferDataType::UShort, unsigned int offset = 0) const;
		void DestroyView(const IIndexBufferView* view);
	};
}

#endif