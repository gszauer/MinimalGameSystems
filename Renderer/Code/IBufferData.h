#ifndef _H_IBUFFERDATA_
#define _H_IBUFFERDATA_

#include "IBufferView.h"

namespace Renderer {
	enum class BufferDataUsage {
		Static = 0,
		Dynamic = 1
	};

	enum class BufferDataIntent {
		Index = 0,
		Element = 1
	};

	class IContext;

	class IBufferData {
	protected:
		IBufferData(); // Disabled
		IBufferData(const IBufferData&); // Disabled
		IBufferData& operator=(const IBufferData&); // Disabled
		inline IBufferData(const IContext& owner) { }
	public:
		virtual inline ~IBufferData() { }

		virtual void Set(BufferDataIntent intent, unsigned int bytes, const void* data, BufferDataUsage usate = BufferDataUsage::Static) = 0;
		virtual void Update(BufferDataIntent intent, unsigned int offset, unsigned int bytes, const void* data) = 0;

		virtual BufferDataUsage GetUsage() const = 0;
		virtual unsigned int Size() const = 0;

		virtual const IBufferView* CreateView(unsigned int numComponents = 1, BufferDataType type = BufferDataType::Float, unsigned int stride = 0, unsigned int offset = 0) const = 0;
		virtual void DestroyView(const IBufferView* view) const = 0;

		virtual const IContext* GetOwner() const = 0;
	};
}

#endif