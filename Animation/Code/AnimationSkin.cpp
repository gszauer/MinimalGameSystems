#include "AnimationSkin.h"
#include "AnimationInternal.h"

template<typename T, unsigned int N>
Animation::Skin::Descriptor<T, N>::Descriptor() {
	mData = 0;
	mCount = 0;
	mOffset = 0;
	mStride = sizeof(T) * N;
}

template<typename T, unsigned int N>
Animation::Skin::Descriptor<T, N>::Descriptor(T* data, unsigned int count, unsigned int stride, unsigned int offset) {
	Set(data, count, stride, offset);
}

template<typename T, unsigned int N>
void Animation::Skin::Descriptor<T, N>::Set(T* data, unsigned int count, unsigned int stride, unsigned int offset) {
	mData = data;
	mCount = count / N;
	mOffset = offset;
	if (stride == 0) {
		mStride = sizeof(T) * N;
	}
	else {
		mStride = stride;
	}
}

template<typename T, unsigned int N>
unsigned int Animation::Skin::Descriptor<T, N>::Size() const {
	return mCount;
}

template<typename T, unsigned int N>
const T* Animation::Skin::Descriptor<T, N>::operator[](unsigned int index) const {
	unsigned char* data = (unsigned char*)mData;
	T * cast = (T*)&data[index * mStride + mOffset];
	return cast;
}

template<typename T, unsigned int N>
T* Animation::Skin::Descriptor<T, N>::operator[](unsigned int index) {
	unsigned char* data = (unsigned char*)mData;
	T* cast = (T*)&data[index * mStride + mOffset];
	return cast;
}

template<typename T, unsigned int N>
const T* Animation::Skin::Descriptor<T, N>::GetData() const {
	return mData;
}

template<typename T, unsigned int N>
unsigned int Animation::Skin::Descriptor<T, N>::GetOffset() const {
	return mOffset;
}

template<typename T, unsigned int N>
unsigned int Animation::Skin::Descriptor<T, N>::GetStride() const {
	return mStride;
}

template<typename T, unsigned int N>
unsigned int Animation::Skin::Descriptor<T, N>::GetCount() const {
	return mCount;
}

template<typename T, unsigned int N>
void Animation::Skin::Descriptor<T, N>::SetData(T* data) {
	mData = data;
}

template<typename T, unsigned int N>
void Animation::Skin::Descriptor<T, N>::SetOffset(unsigned int offset) {
	mOffset;
}

template<typename T, unsigned int N>
void Animation::Skin::Descriptor<T, N>::SetCount(unsigned int length) {
	mCount = length;
}

template<typename T, unsigned int N>
void Animation::Skin::Descriptor<T, N>::SetStride(unsigned int stride) {
	mStride = stride;
}

void Animation::Skin::Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* animationMatrixPalette, const float* invBindPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights) {
	unsigned int numVerts = input.Size();

	for (unsigned int i = 0; i < numVerts; ++i) {
		const float* inputScalar = input[i];
		float vertex[4] = {
			inputScalar[0],
			inputScalar[1],
			inputScalar[2],
			w
		};
		const unsigned int* influence = influences[i];
		const float* weight = weights[i];

		float skin[16] = { 0 };
		float matrix[16] = { 0 };

		for (unsigned int j = 0; j < 4; ++j) {
			if (weight[j] >= 0.000001f) {
				// Multiply matrices
				const float* a = &animationMatrixPalette[influence[j] * 16];
				const float* b = &invBindPalette[influence[j] * 16];
				float* out = matrix;
				{
					float result[16] = { 0.0f };

					// Column 0
					result[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
					result[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
					result[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
					result[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];

					// Column 1
					result[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
					result[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
					result[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
					result[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];

					// Column 2
					result[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
					result[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
					result[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
					result[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];

					// Column 3
					result[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
					result[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
					result[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
					result[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];

					for (unsigned int k = 0; k < 16; ++k) {
						out[k] = result[k];
					}
				}

				for (unsigned int k = 0; k < 16; ++k) {
					skin[k] += matrix[k] * weight[j];
				}
			}
		}

		float result[4] = { // Multiply matrix * vertex
			vertex[0] * skin[0] + vertex[1] * skin[4] + vertex[2] * skin[8]  + vertex[3] * skin[12],
			vertex[0] * skin[1] + vertex[1] * skin[5] + vertex[2] * skin[9]  + vertex[3] * skin[13],
			vertex[0] * skin[2] + vertex[1] * skin[6] + vertex[2] * skin[10] + vertex[3] * skin[14],
			vertex[0] * skin[3] + vertex[1] * skin[7] + vertex[2] * skin[11] + vertex[3] * skin[15]
		};

		float* outputScalar = output[i];
		outputScalar[0] = result[0];
		outputScalar[1] = result[1];
		outputScalar[2] = result[2];
	}
}

void Animation::Skin::Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* skinMatrixPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights) {
	unsigned int numVerts = input.Size();

	for (unsigned int i = 0; i < numVerts; ++i) {
		const float* inputScalar = input[i];
		float vertex[4] = {
			inputScalar[0],
			inputScalar[1],
			inputScalar[2],
			w
		};
		const unsigned int* influence = influences[i];
		const float* weight = weights[i];

		float skin[16] = { 0.0f };

		for (unsigned int j = 0; j < 4; ++j) {
			if (weight[j] >= 0.000001f) {
				const float* matrix = &skinMatrixPalette[influence[j] * 16];
				for (unsigned int k = 0; k < 16; ++k) {
					skin[k] += matrix[k] * weight[j];
				}
			}
		}
		
		float result[4] = { // Multiply matrix * vertex
			vertex[0] * skin[0] + vertex[1] * skin[4] + vertex[2] * skin[8] + vertex[3] * skin[12],
			vertex[0] * skin[1] + vertex[1] * skin[5] + vertex[2] * skin[9] + vertex[3] * skin[13],
			vertex[0] * skin[2] + vertex[1] * skin[6] + vertex[2] * skin[10] + vertex[3] * skin[14],
			vertex[0] * skin[3] + vertex[1] * skin[7] + vertex[2] * skin[11] + vertex[3] * skin[15]
		};

		float* outputScalar = output[i];
		outputScalar[0] = result[0];
		outputScalar[1] = result[1];
		outputScalar[2] = result[2];
	}
}

template class Animation::Skin::Descriptor<float, 4>;
template class Animation::Skin::Descriptor<float, 3>;
template class Animation::Skin::Descriptor<float, 2>;
template class Animation::Skin::Descriptor<float, 1>;
template class Animation::Skin::Descriptor<unsigned int, 4>;
template class Animation::Skin::Descriptor<unsigned int, 3>;
template class Animation::Skin::Descriptor<unsigned int, 2>;
template class Animation::Skin::Descriptor<unsigned int, 1>;