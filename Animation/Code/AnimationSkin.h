#ifndef _H_ANIMATION_SKIN_
#define _H_ANIMATION_SKIN_

namespace Animation {
	class State;

	struct SkinDescriptor {
		union {
			unsigned char* data;
			unsigned int* uint;
			float* scalar;
		};
		unsigned int dataSize; // sieof data type
		unsigned int offset;   // offset in bytes
		unsigned int length;   // length in bytes
		unsigned int stride;   // stride in bytes
		unsigned int size;	   // data size in bytes (ie vec3 = sizeof(float) * 3)

		inline SkinDescriptor();

		unsigned int GetNumElements() const; // ie: How many vertices are there
		unsigned int GetElementSize() const; // ie: how big is a single element, like 3 floats, 4

		unsigned int* GetUIntAt(unsigned int index) const;
		float* GetScalarAt(unsigned int index) const;
	};

	SkinDescriptor MakeVec3Descriptor(float* array, unsigned int size);
	SkinDescriptor MakeVec4Descriptor(float* array, unsigned int size);
	SkinDescriptor MakeUInt4Descriptor(unsigned int* array, unsigned int size);

	void Skin(SkinDescriptor& output, const float* skinMatrixPalette, const float* invBindPalette, const SkinDescriptor& input, float w, const SkinDescriptor& influences, const SkinDescriptor& weights);
}

#endif