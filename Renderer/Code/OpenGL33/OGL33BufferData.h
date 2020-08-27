#ifndef _H_OGL33VERTEXBUFFER_
#define _H_OGL33VERTEXBUFFER_

#include "../IBufferData.h"

namespace Renderer {
	class OGL33BufferData : public IBufferData {
		friend class OGL33GraphicsDevice;
	protected:
		BufferUsageType mUsageType;
		unsigned int mSize;
		// TODO: Buffer ID and implementation details
	protected:
		OGL33BufferData();
	public:
		~OGL33BufferData();

		void Set(unsigned int bytes, void* data, BufferUsageType type = BufferUsageType::Static);
		void Update(unsigned int offset, unsigned int bytes, void* data);

		BufferUsageType GetUsage() const;
		unsigned int Size() const;
	};
}

#endif