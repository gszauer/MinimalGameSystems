
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#include <iostream>
#include <vector>
#include "../AnimationState.h"
#include "../AnimationData.h"
#include "../AnimationBuilder.h"
#include "../AnimationSerializer.h"


struct vec3 {
	union {
		struct {
			float x, y, z;
		};
		float v[3];
	};

	inline vec3() : x(0), y(0), z(0) { }
};

struct vec4 {
	union {
		struct {
			float x, y, z, w;
		};
		float v[4];
	};

	inline vec4() : x(0), y(0), z(0), w(0) { }
};

struct ivec4 {
	union {
		struct {
			int x, y, z, w;
		};
		int v[4];
	};

	inline ivec4() : x(0), y(0), z(0), w(0) { }
};

struct vec2 {
	union {
		struct {
			float x, y;
		};
		float v[2];
	};

	inline vec2() : x(0), y(0) { }
};

struct Mesh {
	std::vector<vec3> position;
	std::vector<vec3> normal;
	std::vector<vec2> texCoord;
	std::vector<vec4> weights;
	std::vector<ivec4> influences;
	std::vector<unsigned int> indices;
	unsigned int pivot;
};

unsigned int SerializedMeshStringSize(Mesh& mesh) {
	std::vector<vec3>& position = mesh.position;
	std::vector<vec3>& normal = mesh.normal;
	std::vector<vec2>& texCoord = mesh.texCoord;
	std::vector<vec4>& weights = mesh.weights;
	std::vector<ivec4>& influences = mesh.influences;
	std::vector<unsigned int>& indices = mesh.indices;
	unsigned int pivot = mesh.pivot;

	unsigned int posSize = position.size();
	unsigned int normSize = normal.size();
	unsigned int texSize = texCoord.size();
	unsigned int weightSize = weights.size();
	unsigned int infSize = influences.size();
	unsigned int idxSize = indices.size();

	unsigned int result = 0;
	unsigned int space = 1;
	unsigned int newLine = 1;

	result += Animation::Serializer::StringLengthUInt(pivot) + space;
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(posSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < posSize; ++i) {
		result += Animation::Serializer::StringLengthFloat(position[i].x) + space;
		result += Animation::Serializer::StringLengthFloat(position[i].y) + space;
		result += Animation::Serializer::StringLengthFloat(position[i].z) + space;
	}
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(normSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < normSize; ++i) {
		result += Animation::Serializer::StringLengthFloat(normal[i].x) + space;
		result += Animation::Serializer::StringLengthFloat(normal[i].y) + space;
		result += Animation::Serializer::StringLengthFloat(normal[i].z) + space;
	}
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(texSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < texSize; ++i) {
		result += Animation::Serializer::StringLengthFloat(texCoord[i].x) + space;
		result += Animation::Serializer::StringLengthFloat(texCoord[i].y) + space;
	}
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(weightSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < weightSize; ++i) {
		result += Animation::Serializer::StringLengthFloat(weights[i].x) + space;
		result += Animation::Serializer::StringLengthFloat(weights[i].y) + space;
		result += Animation::Serializer::StringLengthFloat(weights[i].z) + space;
		result += Animation::Serializer::StringLengthFloat(weights[i].w) + space;
	}
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(infSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < infSize; ++i) {
		result += Animation::Serializer::StringLengthUInt(influences[i].x) + space;
		result += Animation::Serializer::StringLengthUInt(influences[i].y) + space;
		result += Animation::Serializer::StringLengthUInt(influences[i].z) + space;
		result += Animation::Serializer::StringLengthUInt(influences[i].w) + space;
	}
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(idxSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < idxSize; ++i) {
		result += Animation::Serializer::StringLengthUInt(indices[i]) + space;
	}
	result += newLine;

	result += 1; // null term


	const char* str = "mPivot: mIndices: mIndices.size(): mInfluences: mInfluences.size(): mWeights: mWeights.size(): mTexCoords: mTexCoords.size(): mNormals: mNormals.size(): mPositions: mPositions.size(): ";
	const char* s;
	for (s = str; *s; ++s);
	unsigned int strLen = (unsigned int)(s - str);
	result += strLen;

	return result;
}

void SerializeMesh(Mesh& mesh, char* output) {
	std::vector<vec3>& position = mesh.position;
	std::vector<vec3>& normal = mesh.normal;
	std::vector<vec2>& texCoord = mesh.texCoord;
	std::vector<vec4>& weights = mesh.weights;
	std::vector<ivec4>& influences = mesh.influences;
	std::vector<unsigned int>& indices = mesh.indices;
	unsigned int pivot = mesh.pivot;

	unsigned int posSize = position.size();
	unsigned int normSize = normal.size();
	unsigned int texSize = texCoord.size();
	unsigned int weightSize = weights.size();
	unsigned int infSize = influences.size();
	unsigned int idxSize = indices.size();

	const char* message = "mPivot: ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, pivot);
	output = Animation::Serializer::WriteNewLine(output);

	const char* message = "mPositions.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, posSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mPositions: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < posSize; ++i) {
		output = Animation::Serializer::WriteFloat(output, position[i].x);
		output = Animation::Serializer::WriteFloat(output, position[i].y);
		output = Animation::Serializer::WriteFloat(output, position[i].z);
	}
	output = Animation::Serializer::WriteNewLine(output);

	message = "mNormals.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, normSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mNormals: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < normSize; ++i) {
		output = Animation::Serializer::WriteFloat(output, normal[i].x);
		output = Animation::Serializer::WriteFloat(output, normal[i].y);
		output = Animation::Serializer::WriteFloat(output, normal[i].z);
	}
	output = Animation::Serializer::WriteNewLine(output);

	message = "mTexCoords.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, texSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mTexCoords: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < texSize; ++i) {
		output = Animation::Serializer::WriteFloat(output, texCoord[i].x);
		output = Animation::Serializer::WriteFloat(output, texCoord[i].y);
	}
	output = Animation::Serializer::WriteNewLine(output);

	message = "mWeights.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, weightSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mWeights: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < weightSize; ++i) {
		output = Animation::Serializer::WriteFloat(output, weights[i].x);
		output = Animation::Serializer::WriteFloat(output, weights[i].y);
		output = Animation::Serializer::WriteFloat(output, weights[i].z);
		output = Animation::Serializer::WriteFloat(output, weights[i].w);
	}
	output = Animation::Serializer::WriteNewLine(output);

	message = "mInfluences.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, infSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mInfluences: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < infSize; ++i) {
		output = Animation::Serializer::WriteUInt(output, influences[i].x);
		output = Animation::Serializer::WriteUInt(output, influences[i].y);
		output = Animation::Serializer::WriteUInt(output, influences[i].z);
		output = Animation::Serializer::WriteUInt(output, influences[i].w);
	}
	output = Animation::Serializer::WriteNewLine(output);

	message = "mIndices.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, idxSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mIndices: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < idxSize; ++i) {
		output = Animation::Serializer::WriteUInt(output, indices[i]);
	}
	output = Animation::Serializer::WriteNewLine(output);

	*output = '\0';
}


int main(int argc, char** argv) {
	std::cout << "Hellow world\n";
	return 0;
}