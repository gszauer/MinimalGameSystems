#ifndef _H_ANIMATION_SKIN_
#define _H_ANIMATION_SKIN_

namespace Animation {
	namespace Skin {
		template<typename T, unsigned int N>
		class Descriptor {
		protected:
			T* mData;
			unsigned int mCount;    // Number of elements in mData if elements are N-Touples. So if there are 6 elements and N is 2 then mCount would be 3
			unsigned int mOffset;   // offset to first element (in bytes)
			unsigned int mStride;   // stride of elements (in bytes)
		public:
			Descriptor();
			Descriptor(T* data, unsigned int count, unsigned int strideInBytes = 0, unsigned int offsetInBytes = 0); // See, Set function

			unsigned int Size() const; // Or Count. How many elements would be in here if it was an std::vector with each element being an N-Touple
			const T* operator[](unsigned int index) const; // Access each element, intended to be read only
			T* operator[](unsigned int index); // Access each element, read or write

			const T* GetData() const;
			unsigned int GetOffset() const;
			unsigned int GetStride() const;
			unsigned int GetCount() const;

			// If strideInBytes is 0, it will be figured out as sizeof(T) * N
			void Set(T* data, unsigned int count, unsigned int strideInBytes = 0, unsigned int offset = 0);
			void SetData(T* data);
			void SetOffset(unsigned int offsetInBytes); // Keep in mind, in bytes
			void SetStride(unsigned int strideInBytes); // Keep in mind, in bytes
			void SetCount(unsigned int size); // Be careful, this is a RAW SET. IE, if you do 7, but N is 3 then the size is actually invalid.
		};

		void Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* animationMatrixPalette, const float* invBindPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights);
		void Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* skinMatrixPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights);
	}
};

#endif