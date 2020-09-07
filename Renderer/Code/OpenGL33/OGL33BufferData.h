#ifndef _H_OGL33BUFFERDATA_
#define _H_OGL33BUFFERDATA_

#include "../IBufferData.h"
#include "OGL33Loader.h"

namespace Renderer {
	class OGL33Context;

	class OGL33BufferData : public IBufferData {
		friend class OGL33Context;
	protected:
		BufferDataUsage mUsageType;
		unsigned int mSize;
		GLuint mBuffer;
		const OGL33Context* mOwner;
	protected:
		OGL33BufferData(); // Disabled
		OGL33BufferData(const OGL33BufferData&); // Disabled
		OGL33BufferData& operator=(const OGL33BufferData&); // Disabled
		OGL33BufferData(const IContext&);
	public:
		~OGL33BufferData();

		void Set(BufferDataIntent intent, unsigned int bytes, const void* data, BufferDataUsage type = BufferDataUsage::Static);
		void Update(BufferDataIntent intent, unsigned int offset, unsigned int bytes, const void* data);

		BufferDataUsage GetUsage() const;
		unsigned int Size() const;

		const IBufferView* CreateView(unsigned int numComponents = 1, BufferDataType type = BufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0) const;
		void DestroyView(const IBufferView* view) const;
		
		const IContext* GetOwner() const;
		GLuint GetHandle() const;
	};
}

#endif