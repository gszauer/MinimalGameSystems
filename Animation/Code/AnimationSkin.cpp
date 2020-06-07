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
		// TODO
	}
}

Animation::SkinDescriptor::SkinDescriptor() {
	data = 0;
	dataSize = 0;
	offset = 0;
	length = 0;
	stride = 0;
	size = 0;
}

unsigned int Animation::SkinDescriptor::GetNumElements() const {
	return (stride / length) / dataSize;
}

unsigned int Animation::SkinDescriptor::GetElementSize() const {
	return size / dataSize;
}

unsigned int* Animation::SkinDescriptor::GetUIntAt(unsigned int index) const {
	return (unsigned int*)(data + index * stride);
}

float* Animation::SkinDescriptor::GetScalarAt(unsigned int index) const {
	return (float*)(data + index * stride);
}

Animation::SkinDescriptor Animation::MakeFloatDescriptor(float* array, unsigned int size) {
	SkinDescriptor descriptor;

	descriptor.scalar = array;
	descriptor.dataSize = sizeof(float);
	descriptor.offset = 0;
	descriptor.length = sizeof(float) * size;
	descriptor.stride = sizeof(float);

	return descriptor;
}


Animation::SkinDescriptor Animation::MakeFloat2Descriptor(float* array, unsigned int size) {
	SkinDescriptor descriptor;

	descriptor.scalar = array;
	descriptor.dataSize = sizeof(float);
	descriptor.offset = 0;
	descriptor.length = sizeof(float) * size;
	descriptor.stride = sizeof(float) * 2;

	return descriptor;
}


Animation::SkinDescriptor Animation::MakeFloat3Descriptor(float* array, unsigned int size) {
	SkinDescriptor descriptor;

	descriptor.scalar = array;
	descriptor.dataSize = sizeof(float);
	descriptor.offset = 0;
	descriptor.length = sizeof(float) * size;
	descriptor.stride = sizeof(float) * 3;

	return descriptor;
}

Animation::SkinDescriptor Animation::MakeFloat4Descriptor(float* array, unsigned int size) {
	SkinDescriptor descriptor;

	descriptor.scalar = array;
	descriptor.dataSize = sizeof(float);
	descriptor.offset = 0;
	descriptor.length = sizeof(float) * size;
	descriptor.stride = sizeof(float) * 4;

	return descriptor;
}


Animation::SkinDescriptor Animation::MakeUIntDescriptor(unsigned int* array, unsigned int size) {
	SkinDescriptor descriptor;

	descriptor.uint = array;
	descriptor.dataSize = sizeof(unsigned int);
	descriptor.offset = 0;
	descriptor.length = sizeof(unsigned int) * size;
	descriptor.stride = sizeof(unsigned int);

	return descriptor;
}

Animation::SkinDescriptor Animation::MakeUInt2Descriptor(unsigned int* array, unsigned int size) {
	SkinDescriptor descriptor;

	descriptor.uint = array;
	descriptor.dataSize = sizeof(unsigned int);
	descriptor.offset = 0;
	descriptor.length = sizeof(unsigned int) * size;
	descriptor.stride = sizeof(unsigned int) * 2;

	return descriptor;
}

Animation::SkinDescriptor Animation::MakeUInt3Descriptor(unsigned int* array, unsigned int size) {
	SkinDescriptor descriptor;

	descriptor.uint = array;
	descriptor.dataSize = sizeof(unsigned int);
	descriptor.offset = 0;
	descriptor.length = sizeof(unsigned int) * size;
	descriptor.stride = sizeof(unsigned int) * 3;

	return descriptor;
}

Animation::SkinDescriptor Animation::MakeUInt4Descriptor(unsigned int* array, unsigned int size) {
	SkinDescriptor descriptor;

	descriptor.uint = array;
	descriptor.dataSize = sizeof(unsigned int);
	descriptor.offset = 0;
	descriptor.length = sizeof(unsigned int) * size;
	descriptor.stride = sizeof(unsigned int) * 4;

	return descriptor;
}

void Animation::Skin(SkinDescriptor& output, const float* skinPalette, const float* invBindPalette, const SkinDescriptor & input, float w, const SkinDescriptor & influences, const SkinDescriptor & weights) {
	unsigned int numVerts = input.GetNumElements();

	for (unsigned int i = 0; i < numVerts; ++i) {
		float* inputScalar = input.GetScalarAt(i);
		float vertex[4] = {
			inputScalar[0],
			inputScalar[1],
			inputScalar[2],
			w
		};
		unsigned int* influence = influences.GetUIntAt(i);
		float* weight = weights.GetScalarAt(i);

		float m0[16], m1[16], m2[16], m3[16], skin[16] = { 0 };

		if (weight[0] >= 0.000001f) {
			MultiplyMatrices(m0, &skinPalette[influence[0] * 16], &invBindPalette[influence[0] * 16]);
			ScaleMatrix(m0, m0, weight[0]);
		}

		if (weight[1] >= 0.000001f) {
			MultiplyMatrices(m1, &skinPalette[influence[1] * 16], &invBindPalette[influence[1] * 16]);
			ScaleMatrix(m1, m1, weight[1]);
		}

		if (weight[2] >= 0.000001f) {
			MultiplyMatrices(m2, &skinPalette[influence[2] * 16], &invBindPalette[influence[2] * 16]);
			ScaleMatrix(m2, m2, weight[2]);
		}

		if (weight[3] >= 0.000001f) {
			MultiplyMatrices(m3, &skinPalette[influence[3] * 16], &invBindPalette[influence[3] * 16]);
			ScaleMatrix(m3, m3, weight[3]);
		}

		AddMatrices(skin, m0, m1);
		AddMatrices(skin, skin, m2);
		AddMatrices(skin, skin, m3);

		float result[4] = { 0 };
		MultiplyMat4Vec4(result, skin, vertex);

		float* outputScalar = output.GetScalarAt(i);
		outputScalar[0] = result[0];
		outputScalar[1] = result[1];
		outputScalar[2] = result[2];
	}
}