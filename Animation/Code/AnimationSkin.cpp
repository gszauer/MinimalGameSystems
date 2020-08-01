#include "AnimationSkin.h"
#include "AnimationHelpers.h"

template Animation::Skin::Descriptor<float, 4>;
template Animation::Skin::Descriptor<float, 3>;
template Animation::Skin::Descriptor<float, 2>;
template Animation::Skin::Descriptor<float, 1>;
template Animation::Skin::Descriptor<unsigned int, 4>;
template Animation::Skin::Descriptor<unsigned int, 3>;
template Animation::Skin::Descriptor<unsigned int, 2>;
template Animation::Skin::Descriptor<unsigned int, 1>;

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
				Animation::Internal::MultiplyMatrices(matrix, &animationMatrixPalette[influence[j] * 16], &invBindPalette[influence[j] * 16]);
				for (unsigned int k = 0; k < 16; ++k) {
					skin[k] += matrix[k] * weight[j];
				}
			}
		}

		float result[4] = { 0 };
		Animation::Internal::MultiplyMat4Vec4(result, skin, vertex);

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

		float result[4] = { 0 };
		Animation::Internal::MultiplyMat4Vec4(result, skin, vertex);

		float* outputScalar = output[i];
		outputScalar[0] = result[0];
		outputScalar[1] = result[1];
		outputScalar[2] = result[2];
	}
}