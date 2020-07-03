#include "AnimationSkin.h"

namespace Animation {
	static void MultiplyMatrices(float* out, const float* a, const float* b) {
		// Column 0
		out[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
		out[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
		out[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
		out[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];

		// Column 1
		out[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
		out[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
		out[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
		out[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];

		// Column 2
		out[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
		out[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
		out[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
		out[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];

		// Column 3
		out[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
		out[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
		out[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
		out[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
	}

	static void ScaleMatrix(float* out, const float* in, float s) {
		out[0] =  in[0 ] * s;
		out[1] =  in[1 ] * s;
		out[2] =  in[2 ] * s;
		out[3] =  in[3 ] * s;
		out[4] =  in[4 ] * s;
		out[5] =  in[5 ] * s;
		out[6] =  in[6 ] * s;
		out[7] =  in[7 ] * s;
		out[8] =  in[8 ] * s;
		out[9] =  in[9 ] * s;
		out[10] = in[10] * s;
		out[11] = in[11] * s;
		out[12] = in[12] * s;
		out[13] = in[13] * s;
		out[14] = in[14] * s;
		out[15] = in[15] * s;
	}

	static void AddMatrices(float* out, const float* a, const float* b) {
		out[0] = a[0] + b[0];
		out[1] = a[1] + b[1];
		out[2] = a[2] + b[2];
		out[3] = a[3] + b[3];
		out[4] = a[4] + b[4];
		out[5] = a[5] + b[5];
		out[6] = a[6] + b[6];
		out[7] = a[7] + b[7];
		out[8] = a[8] + b[8];
		out[9] = a[9] + b[9];
		out[10] = a[10] + b[10];
		out[11] = a[11] + b[11];
		out[12] = a[12] + b[12];
		out[13] = a[13] + b[13];
		out[14] = a[14] + b[14];
		out[15] = a[15] + b[15];
	}

	static void MultiplyMat4Vec4(float* out, const float* mat, const float* vec) {
		out[0] = vec[0] * mat[0] + vec[1] * mat[4] + vec[2] * mat[8] + vec[3] * mat[12];
		out[1] = vec[0] * mat[1] + vec[1] * mat[5] + vec[2] * mat[9] + vec[3] * mat[13];
		out[2] = vec[0] * mat[2] + vec[1] * mat[6] + vec[2] * mat[10] + vec[3] * mat[14];
		out[3] = vec[0] * mat[3] + vec[1] * mat[7] + vec[2] * mat[11] + vec[3] * mat[15];
	}
}

template<typename T, unsigned int N>
Animation::Skin::Descriptor<T, N>::Descriptor() {
	mData = 0;
	mCount = 0;
	mOffset = 0;
	mStride = 0;
}

template<typename T, unsigned int N>
Animation::Skin::Descriptor<T, N>::Descriptor(T* data, unsigned int count, unsigned int stride, unsigned int offset) {
	mData = data;
	mCount = 0;
	mOffset = 0;
	mStride = 0;
}

template<typename T, unsigned int N>
void Animation::Skin::Descriptor<T, N>::Set(T* data, unsigned int count, unsigned int stride, unsigned int offset) {
	mData = data;
	mCount = count;
	mOffset = offset;
	mStride = stride;
}

template<typename T, unsigned int N>
unsigned int Animation::Skin::Descriptor<T, N>::Size() const {
	return mCount;
}

template<typename T, unsigned int N>
const T& Animation::Skin::Descriptor<T, N>::operator[](unsigned int index) const {
	unsigned char* data = (unsigned char*)mData;
	T* cast = (T*)&data[index * (sizeof(T) * N + mStride)];
	return *cast;
}

template<typename T, unsigned int N>
T& Animation::Skin::Descriptor<T, N>::operator[](unsigned int index) {
	unsigned char* data = (unsigned char*)mData;
	T* cast = (T*)&data[index * (sizeof(T) * N + mStride)];
	return *cast;
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
		const float* inputScalar = &input[i];
		float vertex[4] = {
			inputScalar[0],
			inputScalar[1],
			inputScalar[2],
			w
		};
		const unsigned int* influence = &influences[i];
		const float* weight = &weights[i];

		float m0[16], m1[16], m2[16], m3[16], skin[16] = { 0 };

		if (weight[0] >= 0.000001f) {
			MultiplyMatrices(m0, &animationMatrixPalette[influence[0] * 16], &invBindPalette[influence[0] * 16]);
			ScaleMatrix(m0, m0, weight[0]);
		}

		if (weight[1] >= 0.000001f) {
			MultiplyMatrices(m1, &animationMatrixPalette[influence[1] * 16], &invBindPalette[influence[1] * 16]);
			ScaleMatrix(m1, m1, weight[1]);
		}

		if (weight[2] >= 0.000001f) {
			MultiplyMatrices(m2, &animationMatrixPalette[influence[2] * 16], &invBindPalette[influence[2] * 16]);
			ScaleMatrix(m2, m2, weight[2]);
		}

		if (weight[3] >= 0.000001f) {
			MultiplyMatrices(m3, &animationMatrixPalette[influence[3] * 16], &invBindPalette[influence[3] * 16]);
			ScaleMatrix(m3, m3, weight[3]);
		}

		AddMatrices(skin, m0, m1);
		AddMatrices(skin, skin, m2);
		AddMatrices(skin, skin, m3);

		float result[4] = { 0 };
		MultiplyMat4Vec4(result, skin, vertex);

		float* outputScalar = &output[i];
		outputScalar[0] = result[0];
		outputScalar[1] = result[1];
		outputScalar[2] = result[2];
	}
}

void Animation::Skin::Apply(Descriptor<float, 3>& output, const Descriptor<float, 3>& input, float w, const float* skinMatrixPalette, const Descriptor<unsigned int, 4>& influences, const Descriptor<float, 4>& weights) {
	unsigned int numVerts = input.Size();

	for (unsigned int i = 0; i < numVerts; ++i) {
		const float* inputScalar = &input[i];
		float vertex[4] = {
			inputScalar[0],
			inputScalar[1],
			inputScalar[2],
			w
		};
		const unsigned int* influence = &influences[i];
		const float* weight = &weights[i];

		float m0[16], m1[16], m2[16], m3[16], skin[16] = { 0 };

		if (weight[0] >= 0.000001f) {
			ScaleMatrix(m0, &skinMatrixPalette[influence[0] * 16], weight[0]);
		}

		if (weight[1] >= 0.000001f) {
			ScaleMatrix(m1, &skinMatrixPalette[influence[1] * 16], weight[1]);
		}

		if (weight[2] >= 0.000001f) {
			ScaleMatrix(m2, &skinMatrixPalette[influence[2] * 16], weight[2]);
		}

		if (weight[3] >= 0.000001f) {
			ScaleMatrix(m3, &skinMatrixPalette[influence[3] * 16], weight[3]);
		}

		AddMatrices(skin, m0, m1);
		AddMatrices(skin, skin, m2);
		AddMatrices(skin, skin, m3);

		float result[4] = { 0 };
		MultiplyMat4Vec4(result, skin, vertex);

		float* outputScalar = &output[i];
		outputScalar[0] = result[0];
		outputScalar[1] = result[1];
		outputScalar[2] = result[2];
	}
}