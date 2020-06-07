#ifndef _H_ANIMATION_SKIN_
#define _H_ANIMATION_SKIN_

namespace Animation {

	namespace Skin {
		template<typename T, unsigned int N>
		class Descriptor {
		protected:
			T* mData;
			unsigned int mDataSize; // sieof data type
			unsigned int mOffset;   // offset in bytes
			unsigned int mLength;   // length in bytes
			unsigned int mStride;   // stride in bytes
			unsigned int mSize;	   // data size in bytes (ie vec3 = sizeof(float) * 3)
		public:
			SkinDescriptor();
			SkinDescriptor(T* data, unsigned int dataSize, unsigned int offset = 0);
			SkinDescriptor(const SkinDescriptor&);
			SkinDescriptor& operator=(const SkinDescriptor&);
			~SkinDescriptor();

			unsigned int Size() const; // Or Count. How many elements would be in here if it was an std::vector 
			const T& operator[](unsigned int index) const; // Access each element, intended to be read only
			T& operator[](unsigned int index); // Access each element, read or write

			const T* GetData() const;
			unsigned int GetDataSize() const;
			unsigned int GetOffset() const;
			unsigned int GetLength() const;
			unsigned int GetStride() const;
			unsigned int GetSize() const;

			void SetData(T* data);
			void SetDataSize(unsigned int size);
			void SetOffset(unsigned int offset);
			void SetLength(unsigned int length);
			void SetStride(unsigned int stride);
			void SetSize(unsigned int size);
		};

		void Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* animationMatrixPalette, const float* invBindPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights);
		void Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* skinMatrixPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights);
	}
}

#endif