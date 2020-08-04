#define _CRT_SECURE_NO_WARNINGS

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
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
	inline vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
};

struct vec4 {
	union {
		struct {
			float x, y, z, w;
		};
		float v[4];
	};

	inline vec4() : x(0), y(0), z(0), w(0) { }
	inline vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }
};

struct ivec4 {
	union {
		struct {
			int x, y, z, w;
		};
		int v[4];
	};

	inline ivec4() : x(0), y(0), z(0), w(0) { }
	inline ivec4(int _x, int _y, int _z, int _w) : x(_x), y(_y), z(_z), w(_w) { }
};

struct vec2 {
	union {
		struct {
			float x, y;
		};
		float v[2];
	};

	inline vec2() : x(0), y(0) { }
	inline vec2(float _x, float _y) : x(_x), y(_y) { }
};

struct Mesh {
	std::string name;
	std::vector<vec3> position;
	std::vector<vec3> normal;
	std::vector<vec2> texCoord;
	std::vector<vec4> weights;
	std::vector<ivec4> influences;
	std::vector<unsigned int> indices;
	unsigned int pivot;
};

Animation::State LoadRestPose(cgltf_data* data);
Animation::State LoadBindPose(cgltf_data* data);
std::vector<Animation::Data> LoadClips(cgltf_data* data);
std::vector<Mesh> LoadMeshes(cgltf_data* data);
unsigned int SerializedMeshStringSize(Mesh& mesh);
void SerializeMesh(Mesh& mesh, char* output);

int main(int argc, char** argv) {
	const char* path = "Assets/Zombie.gltf"; // TODO: Parse from args
	// TODO: Check if file exists before continuing

	cgltf_options options;
	memset(&options, 0, sizeof(cgltf_options));
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, path, &data);
	if (result != cgltf_result_success) {
		std::cout << "Could not load input file: " << path << "\n";
		return 0;
	}
	result = cgltf_load_buffers(&options, data, path);
	if (result != cgltf_result_success) {
		cgltf_free(data);
		std::cout << "Could not load buffers for: " << path << "\n";
		return 0;
	}
	result = cgltf_validate(data);
	if (result != cgltf_result_success) {
		cgltf_free(data);
		std::cout << "Invalid gltf file: " << path << "\n";
		return 0;
	}

	Animation::State restPose = LoadRestPose(data);
	Animation::State bindPose = LoadBindPose(data);
	std::vector<Animation::Data> clips = LoadClips(data);
	std::vector<Mesh> meshes = LoadMeshes(data);

	cgltf_free(data);

	std::ofstream out;

	char* outputBuffer = new char[Animation::Serializer::SerializedStateSize(bindPose)];
	Animation::Serializer::SerializeState(outputBuffer, bindPose);
	out.open("bindState.txt");
	out << outputBuffer;
	out.close();
	delete[] outputBuffer;

	outputBuffer = new char[Animation::Serializer::SerializedStateSize(restPose)];
	Animation::Serializer::SerializeState(outputBuffer, restPose);
	out.open("restState.txt");
	out << outputBuffer;
	out.close();
	delete[] outputBuffer;

	char clip_name[32];
	char file_name[512];
	for (size_t i = 0; i < clips.size(); ++i) {
		outputBuffer = new char[Animation::Serializer::SerializedDataSize(clips[i])];
		Animation::Serializer::SerializeData(outputBuffer, clips[i]);

		sprintf(clip_name, "CLIP_%d\0", i);
		const char* label = clips[i].GetLabel();
		if (label == 0 || strlen(label) == 0) {
			label = clip_name;
		}

		sprintf(file_name, "%sData.txt", label);
		out.open(file_name);
		out << outputBuffer;
		out.close();
		delete[] outputBuffer;
	}

	for (size_t i = 0; i < meshes.size(); ++i) {
		outputBuffer = new char[SerializedMeshStringSize(meshes[i])];
		SerializeMesh(meshes[i], outputBuffer);

		sprintf(clip_name, "MESH_%d\0", i);
		const char* label = meshes[i].name.c_str();
		if (label == 0 || strlen(label) == 0) {
			label = clip_name;
		}

		sprintf(file_name, "%s.Meshtxt", label);
		out.open(file_name);
		out << outputBuffer;
		out.close();
		delete[] outputBuffer;
	}

	return 0;
}


struct quat {
	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		struct {
			vec3 vector;
			float scalar;
		};
		float v[4];
	};

	inline quat() :
		x(0), y(0), z(0), w(1) { }
	inline quat(float _x, float _y, float _z, float _w) :
		x(_x), y(_y), z(_z), w(_w) {}
};

struct mat4 {
	union {
		float v[16];
		struct {
			vec4 right;
			vec4 up;
			vec4 forward;
			vec4 position;
		};
		struct {
			//            row 1     row 2     row 3     row 4
			/* column 1 */float xx; float xy; float xz; float xw;
			/* column 2 */float yx; float yy; float yz; float yw;
			/* column 3 */float zx; float zy; float zz; float zw;
			/* column 4 */float tx; float ty; float tz; float tw;
		};
		struct {
			float c0r0; float c0r1; float c0r2; float c0r3;
			float c1r0; float c1r1; float c1r2; float c1r3;
			float c2r0; float c2r1; float c2r2; float c2r3;
			float c3r0; float c3r1; float c3r2; float c3r3;
		};
		struct {
			float r0c0; float r1c0; float r2c0; float r3c0;
			float r0c1; float r1c1; float r2c1; float r3c1;
			float r0c2; float r1c2; float r2c2; float r3c2;
			float r0c3; float r1c3; float r2c3; float r3c3;
		};
	};
	// Include constructors here

	inline mat4() :
		xx(1), xy(0), xz(0), xw(0),
		yx(0), yy(1), yz(0), yw(0),
		zx(0), zy(0), zz(1), zw(0),
		tx(0), ty(0), tz(0), tw(1) {}

	inline mat4(float* fv) :
		xx(fv[0]), xy(fv[1]), xz(fv[2]), xw(fv[3]),
		yx(fv[4]), yy(fv[5]), yz(fv[6]), yw(fv[7]),
		zx(fv[8]), zy(fv[9]), zz(fv[10]), zw(fv[11]),
		tx(fv[12]), ty(fv[13]), tz(fv[14]), tw(fv[15]) { }

	inline mat4(
		float _00, float _01, float _02, float _03,
		float _10, float _11, float _12, float _13,
		float _20, float _21, float _22, float _23,
		float _30, float _31, float _32, float _33) :
		xx(_00), xy(_01), xz(_02), xw(_03),
		yx(_10), yy(_11), yz(_12), yw(_13),
		zx(_20), zy(_21), zz(_22), zw(_23),
		tx(_30), ty(_31), tz(_32), tw(_33) { }
}; // end mat4 struct

struct Transform {
	vec3 position;
	quat rotation;
	vec3 scale;
	Transform() :
		position(vec3(0, 0, 0)),
		rotation(quat(0, 0, 0, 1)),
		scale(vec3(1, 1, 1)) {}
	Transform(const vec3& p, const quat& r, const vec3& s) :
		position(p), rotation(r), scale(s) {}
}; // End of transform struct

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

	message = "mPositions.size(): ";
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

Transform mat4ToTransform(const mat4& m);

Transform GetLocalTransform(cgltf_node& node) {
	Transform result;

	if (node.has_matrix) {
		std::cout << "mat4 nodes not supported\n"; // TODO: mat4ToTransform
		result.position = vec3(0, 0, 0);
		result.rotation = quat(0, 0, 0, 1);
		result.scale = vec3(1, 1, 1);
	}

	if (node.has_translation) {
		result.position = vec3(node.translation[0], node.translation[1], node.translation[2]);
	}

	if (node.has_rotation) {
		result.rotation = quat(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
	}

	if (node.has_scale) {
		result.scale = vec3(node.scale[0], node.scale[1], node.scale[2]);
	}

	return result;
}

int GetNodeIndex(cgltf_node* target, cgltf_node* allNodes, unsigned int numNodes) {
	if (target == 0) {
		return -1;
	}
	for (unsigned int i = 0; i < numNodes; ++i) {
		if (target == &allNodes[i]) {
			return (int)i;
		}
	}
	return -1;
}

void GetScalarValues(std::vector<float>& outScalars, unsigned int inComponentCount, const cgltf_accessor& inAccessor) {
	outScalars.resize(inAccessor.count * inComponentCount);
	for (cgltf_size i = 0; i < inAccessor.count; ++i) {
		cgltf_accessor_read_float(&inAccessor, i, &outScalars[i * inComponentCount], inComponentCount);
	}
}

Animation::State LoadRestPose(cgltf_data* data) {
	unsigned int boneCount = (unsigned int)data->nodes_count;
	Animation::State result;
	result.Resize(boneCount);

	for (unsigned int i = 0; i < boneCount; ++i) {
		cgltf_node* node = &(data->nodes[i]);

		Transform transform = GetLocalTransform(data->nodes[i]);
		result.SetRelativePosition(i, transform.position.v);
		result.SetRelativeRotation(i, transform.rotation.v);
		result.SetRelativeScale(i, transform.scale.v);

		int parent = GetNodeIndex(node->parent, data->nodes, boneCount);
		result.SetParent(i, parent);
	}

	return result;
}

mat4 inverse(const mat4& m);
Transform mat4ToTransform(const mat4& m);
Transform combine(const Transform& a, const Transform& b);
Transform inverse(const Transform& t);

Animation::State LoadBindPose(cgltf_data* data) {
	Animation::State restPose = LoadRestPose(data);
	unsigned int numBones = restPose.Size();
	std::vector<Transform> worldBindPose(numBones);
	for (unsigned int i = 0; i < numBones; ++i) {
		restPose.GetAbsoluteTransform(i, worldBindPose[i].position.v, worldBindPose[i].rotation.v, worldBindPose[i].scale.v);
	}
	unsigned int numSkins = (unsigned int)data->skins_count;
	for (unsigned int i = 0; i < numSkins; ++i) {
		cgltf_skin* skin = &(data->skins[i]);
		std::vector<float> invBindAccessor;
		GetScalarValues(invBindAccessor, 16, *skin->inverse_bind_matrices);

		unsigned int numJoints = (unsigned int)skin->joints_count;
		for (unsigned int j = 0; j < numJoints; ++j) {
			// Read the ivnerse bind matrix of the joint
			float* matrix = &(invBindAccessor[j * 16]);
			mat4 invBindMatrix = mat4(matrix);
			// invert, convert to transform
			mat4 bindMatrix = inverse(invBindMatrix);
			Transform bindTransform = mat4ToTransform(bindMatrix);
			// Set that transform in the worldBindPose.
			cgltf_node* jointNode = skin->joints[j];
			int jointIndex = GetNodeIndex(jointNode, data->nodes, numBones);
			worldBindPose[jointIndex] = bindTransform;
		} // end for each joint
	} // end for each skin
	// Convert the world bind pose to a regular bind pose
	Animation::State bindPose = restPose;
	for (unsigned int i = 0; i < numBones; ++i) {
		Transform current = worldBindPose[i];
		int p = bindPose.GetParent(i);
		if (p >= 0) { // Bring into parent space
			Transform parent = worldBindPose[p];
			current = combine(inverse(parent), current);
		}
		bindPose.SetRelativePosition(i, current.position.v);
		bindPose.SetRelativeRotation(i, current.rotation.v);
		bindPose.SetRelativeScale(i, current.scale.v);
	}

	return bindPose;
} // End LoadBindPose function

#define M4_3X3MINOR(c0, c1, c2, r0, r1, r2) \
    (m.v[c0 * 4 + r0] * (m.v[c1 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c1 * 4 + r2] * m.v[c2 * 4 + r1]) - \
     m.v[c1 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c2 * 4 + r1]) + \
     m.v[c2 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c1 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c1 * 4 + r1]))

float determinant(const mat4& m) {
	return  m.v[0] * M4_3X3MINOR(1, 2, 3, 1, 2, 3)
		- m.v[4] * M4_3X3MINOR(0, 2, 3, 1, 2, 3)
		+ m.v[8] * M4_3X3MINOR(0, 1, 3, 1, 2, 3)
		- m.v[12] * M4_3X3MINOR(0, 1, 2, 1, 2, 3);
}

mat4 transposed(const mat4& m) {
	return mat4(
		m.xx, m.yx, m.zx, m.tx,
		m.xy, m.yy, m.zy, m.ty,
		m.xz, m.yz, m.zz, m.tz,
		m.xw, m.yw, m.zw, m.tw
		);
}

mat4 adjugate(const mat4& m) {
	// Cofactor(M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
	mat4 cofactor;

	cofactor.v[0] = M4_3X3MINOR(1, 2, 3, 1, 2, 3);
	cofactor.v[1] = -M4_3X3MINOR(1, 2, 3, 0, 2, 3);
	cofactor.v[2] = M4_3X3MINOR(1, 2, 3, 0, 1, 3);
	cofactor.v[3] = -M4_3X3MINOR(1, 2, 3, 0, 1, 2);

	cofactor.v[4] = -M4_3X3MINOR(0, 2, 3, 1, 2, 3);
	cofactor.v[5] = M4_3X3MINOR(0, 2, 3, 0, 2, 3);
	cofactor.v[6] = -M4_3X3MINOR(0, 2, 3, 0, 1, 3);
	cofactor.v[7] = M4_3X3MINOR(0, 2, 3, 0, 1, 2);

	cofactor.v[8] = M4_3X3MINOR(0, 1, 3, 1, 2, 3);
	cofactor.v[9] = -M4_3X3MINOR(0, 1, 3, 0, 2, 3);
	cofactor.v[10] = M4_3X3MINOR(0, 1, 3, 0, 1, 3);
	cofactor.v[11] = -M4_3X3MINOR(0, 1, 3, 0, 1, 2);

	cofactor.v[12] = -M4_3X3MINOR(0, 1, 2, 1, 2, 3);
	cofactor.v[13] = M4_3X3MINOR(0, 1, 2, 0, 2, 3);
	cofactor.v[14] = -M4_3X3MINOR(0, 1, 2, 0, 1, 3);
	cofactor.v[15] = M4_3X3MINOR(0, 1, 2, 0, 1, 2);

	return transposed(cofactor);
}

vec3 normalized(const vec3& v) {
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < 0.000001f) {
		return v;
	}
	float invLen = 1.0f / sqrtf(lenSq);

	return vec3(
		v.x * invLen,
		v.y * invLen,
		v.z * invLen
		);
}

vec3 cross(const vec3& l, const vec3& r) {
	return vec3(
		l.y * r.z - l.z * r.y,
		l.z * r.x - l.x * r.z,
		l.x * r.y - l.y * r.x
		);
}

float dot(const vec3& l, const vec3& r) {
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

bool operator==(const vec3& l, const vec3& r) {
	vec3 diff(l.x - r.x, l.y - r.y, l.z - r.z);
	return dot(diff, diff) < 0.000001f;
}

vec3 operator*(const vec3& v, float f) {
	return vec3(v.x * f, v.y * f, v.z * f);
}

vec3 operator+(const vec3& l, const vec3& r) {
	return vec3(l.x + r.x, l.y + r.y, l.z + r.z);
}

quat fromTo(const vec3& from, const vec3& to) {
	vec3 f = normalized(from);
	vec3 t = normalized(to);

	if (f == t) {
		return quat();
	}
	else if (f == t * -1.0f) {
		vec3 ortho = vec3(1, 0, 0);
		if (fabsf(f.y) < fabsf(f.x)) {
			ortho = vec3(0, 1, 0);
		}
		if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x)) {
			ortho = vec3(0, 0, 1);
		}

		vec3 axis = normalized(cross(f, ortho));
		return quat(axis.x, axis.y, axis.z, 0);
	}

	vec3 half = normalized(f + t);
	vec3 axis = cross(f, half);

	return quat(
		axis.x,
		axis.y,
		axis.z,
		dot(f, half)
		);
}

vec3 operator*(const quat& q, const vec3& v) {
	return    q.vector * 2.0f * dot(q.vector, v) +
		v * (q.scalar * q.scalar - dot(q.vector, q.vector)) +
		cross(q.vector, v) * 2.0f * q.scalar;
}

quat operator*(const quat& Q1, const quat& Q2) {
	return quat(
		Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x,
		-Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y,
		Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z,
		-Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w
		);
}

quat normalized(const quat& q) {
	float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < 0.000001f) {
		return quat();
	}
	float i_len = 1.0f / sqrtf(lenSq);

	return quat(
		q.x * i_len,
		q.y * i_len,
		q.z * i_len,
		q.w * i_len
		);
}

quat lookRotation(const vec3& direcion, const vec3& up) {
	// Find orthonormal basis vectors
	vec3 f = normalized(direcion);
	vec3 u = normalized(up);
	vec3 r = cross(u, f);
	u = cross(f, r);

	// From world forward to object forward
	quat f2d = fromTo(vec3(0, 0, 1), f);

	// what direction is the new object up?
	vec3 objectUp = f2d * vec3(0, 1, 0);
	// From object up to desired up
	quat u2u = fromTo(objectUp, u);

	// Rotate to forward direction first, then twist to correct up
	quat result = f2d * u2u;
	// Don’t forget to normalize the result
	return normalized(result);
}

quat mat4ToQuat(const mat4& m) {
	vec3 up = normalized(vec3(m.up.x, m.up.y, m.up.z));
	vec3 forward = normalized(vec3(m.forward.x, m.forward.y, m.forward.z));
	vec3 right = cross(up, forward);
	up = cross(forward, right);

	return lookRotation(forward, up);
}

mat4 quatToMat4(const quat& q) {
	vec3 r = q * vec3(1, 0, 0);
	vec3 u = q * vec3(0, 1, 0);
	vec3 f = q * vec3(0, 0, 1);

	return mat4(
		r.x, r.y, r.z, 0,
		u.x, u.y, u.z, 0,
		f.x, f.y, f.z, 0,
		0, 0, 0, 1
		);
}

quat inverse(const quat& q) {
	float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < 0.000001f) {
		return quat();
	}
	float recip = 1.0f / lenSq;

	// conjugate / norm
	return quat(
		-q.x * recip,
		-q.y * recip,
		-q.z * recip,
		q.w * recip
		);
}

#define M4D(aRow, bCol) \
    a.v[0 * 4 + aRow] * b.v[bCol * 4 + 0] + \
    a.v[1 * 4 + aRow] * b.v[bCol * 4 + 1] + \
    a.v[2 * 4 + aRow] * b.v[bCol * 4 + 2] + \
    a.v[3 * 4 + aRow] * b.v[bCol * 4 + 3]

mat4 operator*(const mat4& a, const mat4& b) {
	return mat4(
		M4D(0, 0), M4D(1, 0), M4D(2, 0), M4D(3, 0), // Column 0
		M4D(0, 1), M4D(1, 1), M4D(2, 1), M4D(3, 1), // Column 1
		M4D(0, 2), M4D(1, 2), M4D(2, 2), M4D(3, 2), // Column 2
		M4D(0, 3), M4D(1, 3), M4D(2, 3), M4D(3, 3)  // Column 3
		);
}

Transform mat4ToTransform(const mat4& m) {
	Transform out;

	out.position = vec3(m.v[12], m.v[13], m.v[14]);
	out.rotation = mat4ToQuat(m);

	mat4 rotScaleMat(
		m.v[0], m.v[1], m.v[2], 0,
		m.v[4], m.v[5], m.v[6], 0,
		m.v[8], m.v[9], m.v[10], 0,
		0, 0, 0, 1
		);
	mat4 invRotMat = quatToMat4(inverse(out.rotation));
	mat4 scaleSkewMat = rotScaleMat * invRotMat;

	out.scale = vec3(
		scaleSkewMat.v[0],
		scaleSkewMat.v[5],
		scaleSkewMat.v[10]
		);

	return out;
}

mat4 operator*(const mat4& m, float f) {
	return mat4(
		m.xx * f, m.xy * f, m.xz * f, m.xw * f,
		m.yx * f, m.yy * f, m.yz * f, m.yw * f,
		m.zx * f, m.zy * f, m.zz * f, m.zw * f,
		m.tx * f, m.ty * f, m.tz * f, m.tw * f
		);
}

mat4 inverse(const mat4& m) {
	float det = determinant(m);

	if (det == 0.0f) { // Epsilon check would need to be REALLY small
		std::cout << "WARNING: Trying to invert a matrix with a zero determinant\n";
		return mat4();
	}
	mat4 adj = adjugate(m);

	return adj * (1.0f / det);
}

vec3 operator*(const vec3& l, const vec3& r) {
	return vec3(l.x * r.x, l.y * r.y, l.z * r.z);
}

Transform combine(const Transform& a, const Transform& b) {
	Transform out;

	out.scale = a.scale * b.scale;
	out.rotation = b.rotation * a.rotation;

	out.position = a.rotation * (a.scale * b.position);
	out.position = a.position + out.position;

	return out;
}

Transform inverse(const Transform& t) {
	Transform inv;

	inv.rotation = inverse(t.rotation);

	inv.scale.x = fabs(t.scale.x) < 0.000001f ? 0.0f : 1.0f / t.scale.x;
	inv.scale.y = fabs(t.scale.y) < 0.000001f ? 0.0f : 1.0f / t.scale.y;
	inv.scale.z = fabs(t.scale.z) < 0.000001f ? 0.0f : 1.0f / t.scale.z;

	vec3 invTranslation = t.position * -1.0f;
	inv.position = inv.rotation * (inv.scale * invTranslation);

	return inv;
}

std::vector<std::string> GetClipNames(cgltf_data* data) {
	unsigned int numClips = (unsigned int)data->animations_count;
	unsigned int numNodes = (unsigned int)data->nodes_count;

	std::vector<std::string> result;
	result.resize(numClips);

	for (unsigned int i = 0; i < numClips; ++i) {
		if (data->animations[i].name == 0) {
			result[i] = "NULL\n";
		}
		else if (strlen(data->animations[i].name) == 0) {
			result[i] = "EMPTY\n";
		}
		else {
			result[i] = data->animations[i].name;
		}
	}

	return result;
}

void MeshFromAttribute(Mesh& outMesh, cgltf_attribute& attribute, cgltf_skin* skin, cgltf_node* nodes, unsigned int nodeCount) {
	cgltf_attribute_type attribType = attribute.type;
	cgltf_accessor& accessor = *attribute.data;

	unsigned int componentCount = 0;
	if (accessor.type == cgltf_type_vec2) {
		componentCount = 2;
	}
	else if (accessor.type == cgltf_type_vec3) {
		componentCount = 3;
	}
	else if (accessor.type == cgltf_type_vec4) {
		componentCount = 4;
	}
	std::vector<float> values;
	GetScalarValues(values, componentCount, accessor);
	unsigned int acessorCount = (unsigned int)accessor.count;

	std::vector<vec3>& positions = outMesh.position;
	std::vector<vec3>& normals = outMesh.normal;
	std::vector<vec2>& texCoords = outMesh.texCoord;
	std::vector<ivec4>& influences = outMesh.influences;
	std::vector<vec4>& weights = outMesh.weights;

	for (unsigned int i = 0; i < acessorCount; ++i) {
		int index = i * componentCount;
		switch (attribType) {
		case cgltf_attribute_type_position:
			positions.push_back(vec3(values[index + 0], values[index + 1], values[index + 2]));
			break;
		case cgltf_attribute_type_texcoord:
			texCoords.push_back(vec2(values[index + 0], values[index + 1]));
			break;
		case cgltf_attribute_type_weights:
			weights.push_back(vec4(values[index + 0], values[index + 1], values[index + 2], values[index + 3]));
			break;
		case cgltf_attribute_type_normal:
		{
			vec3 normal = vec3(values[index + 0], values[index + 1], values[index + 2]);
			if (dot(normal, normal) < 0.000001f) {
				normal = vec3(0, 1, 0);
			}
			normals.push_back(normalized(normal));
		}
		break;
		case cgltf_attribute_type_joints:
		{
			// These indices are skin relative. This function has no information about the
			// skin that is being parsed. Add +0.5f to round, since we can't read ints
			ivec4 joints(
				(int)(values[index + 0] + 0.5f),
				(int)(values[index + 1] + 0.5f),
				(int)(values[index + 2] + 0.5f),
				(int)(values[index + 3] + 0.5f)
			);

			joints.x = std::max(0, GetNodeIndex(skin->joints[joints.x], nodes, nodeCount));
			joints.y = std::max(0, GetNodeIndex(skin->joints[joints.y], nodes, nodeCount));
			joints.z = std::max(0, GetNodeIndex(skin->joints[joints.z], nodes, nodeCount));
			joints.w = std::max(0, GetNodeIndex(skin->joints[joints.w], nodes, nodeCount));

			influences.push_back(joints);
		}
		break;
		} // End switch statement
	}
}// End of MeshFromAttribute function

std::vector<Mesh> LoadMeshes(cgltf_data* data) {
	std::vector<Mesh> result;
	cgltf_node* nodes = data->nodes;
	unsigned int nodeCount = (unsigned int)data->nodes_count;

	for (unsigned int i = 0; i < nodeCount; ++i) {
		cgltf_node* node = &nodes[i];
		if (node->mesh == 0 || node->skin == 0) {
			continue;
		}
		unsigned int numPrims = (unsigned int)node->mesh->primitives_count;
		for (unsigned int j = 0; j < numPrims; ++j) {
			result.push_back(Mesh());
			Mesh& mesh = result[result.size() - 1];

			cgltf_primitive* primitive = &node->mesh->primitives[j];

			unsigned int numAttributes = (unsigned int)primitive->attributes_count;
			for (unsigned int k = 0; k < numAttributes; ++k) {
				cgltf_attribute* attribute = &primitive->attributes[k];
				MeshFromAttribute(mesh, *attribute, node->skin, nodes, nodeCount);
			}
			if (primitive->indices != 0) {
				unsigned int indexCount = (unsigned int)primitive->indices->count;
				std::vector<unsigned int>& indices = mesh.indices;
				indices.resize(indexCount);

				for (unsigned int k = 0; k < indexCount; ++k) {
					indices[k] = (unsigned int)cgltf_accessor_read_index(primitive->indices, k);
				}
			}
		}
	}

	return result;
}

template<typename T, int N>
void TrackFromChannel(Animation::Builder::Track& inOutTrack, const cgltf_animation_channel& inChannel) {
	cgltf_animation_sampler& sampler = *inChannel.sampler;

	std::vector<float> timelineFloats;
	GetScalarValues(timelineFloats, 1, *sampler.input);

	std::vector<float> valueFloats;
	GetScalarValues(valueFloats, N, *sampler.output);

	unsigned int numFrames = (unsigned int)sampler.input->count;
	unsigned int numberOfValuesPerFrame = valueFloats.size() / timelineFloats.size();
	inOutTrack.Resize(numFrames);

	for (unsigned int i = 0; i < numFrames; ++i) {
		int baseIndex = i * numberOfValuesPerFrame;
		Animation::Builder::Frame& frame = inOutTrack[i];
		int offset = 0;

		frame.time = timelineFloats[i];

		for (int component = 0; component < N; ++component) {
			frame.in[component] = (inChannel.sampler->interpolation == cgltf_interpolation_type_cubic_spline) ? valueFloats[baseIndex + offset++] : 0.0f;
		}

		for (int component = 0; component < N; ++component) {
			frame.value[component] = valueFloats[baseIndex + offset++];
		}

		for (int component = 0; component < N; ++component) {
			frame.out[component] = (inChannel.sampler->interpolation == cgltf_interpolation_type_cubic_spline) ? valueFloats[baseIndex + offset++] : 0.0f;
		}
	}

	if (inChannel.sampler->interpolation == cgltf_interpolation_type_step) {
		//inOutTrack.ForceStep();
	}
	else if (inChannel.sampler->interpolation == cgltf_interpolation_type_linear) {
		inOutTrack.ForceLinear();
	}
}

std::vector<Animation::Data> LoadClips(cgltf_data* data) {
	unsigned int numClips = (unsigned int)data->animations_count;
	unsigned int numNodes = (unsigned int)data->nodes_count;

	std::vector<Animation::Builder::Clip> result;
	std::vector<Animation::Data> final;
	result.resize(numClips);
	final.resize(numClips);

	for (unsigned int i = 0; i < numClips; ++i) {
		result[i].SetName(data->animations[i].name);

		unsigned int numChannels = (unsigned int)data->animations[i].channels_count;
		for (unsigned int j = 0; j < numChannels; ++j) {
			cgltf_animation_channel& channel = data->animations[i].channels[j];
			cgltf_node* target = channel.target_node;
			int nodeId = GetNodeIndex(target, data->nodes, numNodes);
			Animation::Builder::Track track;
			track.SetJointID(nodeId);
			if (channel.target_path == cgltf_animation_path_type_translation) {
				track.SetTarget(Animation::Data::Component::Position);
				TrackFromChannel<vec3, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_scale) {
				track.SetTarget(Animation::Data::Component::Scale);
				TrackFromChannel<vec3, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_rotation) {
				track.SetTarget(Animation::Data::Component::Rotation);
				TrackFromChannel<quat, 4>(track, channel);
			}
			if (channel.target_path == cgltf_animation_path_type_translation ||
				channel.target_path == cgltf_animation_path_type_scale ||
				channel.target_path == cgltf_animation_path_type_rotation) {
				result[i].PushTrack(track);
			}
		}
	}

	for (unsigned int i = 0; i < numClips; ++i) {
		final[i] = Animation::Builder::Convert(result[i]);
	}
	return final;
}