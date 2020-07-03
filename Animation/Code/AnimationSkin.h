#ifndef _H_ANIMATION_SKIN_
#define _H_ANIMATION_SKIN_

namespace Animation {

	namespace Skin {
		template<typename T, unsigned int N>
		class Descriptor {
		protected:
			T* mData;
			unsigned int mCount;    // number of elements in mData array
			unsigned int mOffset;   // offset to first element (in bytes)
			unsigned int mStride;   // stride in bytes
		public:
			Descriptor();
			Descriptor(T* data, unsigned int count, unsigned int stride = 0, unsigned int offset = 0);

			unsigned int Size() const; // Or Count. How many elements would be in here if it was an std::vector 
			const T& operator[](unsigned int index) const; // Access each element, intended to be read only
			T& operator[](unsigned int index); // Access each element, read or write

			const T* GetData() const;
			unsigned int GetOffset() const;
			unsigned int GetStride() const;
			unsigned int GetCount() const;

			void Set(T* data, unsigned int count, unsigned int stride = 0, unsigned int offset = 0);
			void SetData(T* data);
			void SetOffset(unsigned int offset);
			void SetStride(unsigned int stride);
			void SetCount(unsigned int size);
		};

		void Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* animationMatrixPalette, const float* invBindPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights);
		void Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* skinMatrixPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights);
	}
};

#endif