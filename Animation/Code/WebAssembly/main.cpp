#include "vector.h"
#include "math.h"
#include "../AnimationInternal.h"
#include "../AnimationData.h"
#include "../AnimationState.h"
#include "../AnimationSkin.h"
#include "../AnimationSerializer.h"
#include "../AnimationBuilder.h"

void KeepAplive() {
	Animation::Skin::Descriptor<float, 4> a;
	Animation::Skin::Descriptor<float, 3> b;
	Animation::Skin::Descriptor<float, 2> c;
	Animation::Skin::Descriptor<float, 1> d;
	Animation::Skin::Descriptor<unsigned int, 4> e;
	Animation::Skin::Descriptor<unsigned int, 3> f;
	Animation::Skin::Descriptor<unsigned int, 2> g;
	Animation::Skin::Descriptor<unsigned int, 1> h;
}

struct Sample {
	Vector<vec3> vertices; // Javascript sets this
	Vector<vec3> normals; // Jacasript sets this
	Vector<uivec4> influences; // Jacasript sets this
	Vector<vec4> weights; // Jacasript sets this

	// Everything else is internal
	Animation::Data aniamtionDataA;
	Animation::Data aniamtionDataB;
	Animation::Data blendCurve;
	Animation::State animatedPoseA;
	Animation::State animatedPoseB;
	Animation::State blendedPose;
	Animation::State restPose;
	Animation::State bindPose;
	float playTimeA;
	float playTimeB;
	float blendTime;
	Vector<mat4> invBindPosePalette;
	Vector<mat4> animatedPosePalette;

	Animation::Skin::Descriptor<float, 3> readPositions;
	Animation::Skin::Descriptor<float, 3> readNormals;
	Animation::Skin::Descriptor<float, 3> writePositions;
	Animation::Skin::Descriptor<float, 3> writeNormals;
	Animation::Skin::Descriptor<unsigned int, 4> readInfluences;
	Animation::Skin::Descriptor<float, 4> readWeights;
};

vec3* gSkinnedVerts = 0; // Javascript read / write
Sample* gSample = 0;

Animation::Builder::Frame MakeFrame(float time, float in, float value, float out);

extern "C" void Initialize(unsigned int numVerts) {
	gSkinnedVerts = (vec3*)Animation::Internal::Allocate(sizeof(vec3) * numVerts * 2);
	gSample = (Sample*)Animation::Internal::Allocate(sizeof(Sample));
	new (gSample) Vector<Sample>();
	gSample->playTimeA = 0.0f;
	gSample->playTimeB = 0.0f;
	gSample->blendTime = 0.0f;
}

extern "C" void SetModelData(const char* input) {
	unsigned int posSize = 0;
	unsigned int normSize = 0;
	unsigned int texSize = 0;
	unsigned int weightSize = 0;
	unsigned int infSize = 0;
	float ignore = 0.0f;
	unsigned int pivot = 0;
	input = Animation::Serializer::ReadUInt(input, pivot);

	input = Animation::Serializer::ReadUInt(input, posSize);
	gSample->vertices.Resize(posSize);
	for (unsigned int i = 0; i < posSize; ++i) {
		input = Animation::Serializer::ReadFloat(input, gSample->vertices[i].x);
		input = Animation::Serializer::ReadFloat(input, gSample->vertices[i].y);
		input = Animation::Serializer::ReadFloat(input, gSample->vertices[i].z);
	}

	input = Animation::Serializer::ReadUInt(input, normSize);
	gSample->normals.Resize(normSize);
	for (unsigned int i = 0; i < normSize; ++i) {
		input = Animation::Serializer::ReadFloat(input, gSample->normals[i].x);
		input = Animation::Serializer::ReadFloat(input, gSample->normals[i].y);
		input = Animation::Serializer::ReadFloat(input, gSample->normals[i].z);
	}

	input = Animation::Serializer::ReadUInt(input, texSize);
	for (unsigned int i = 0; i < texSize; ++i) {
		input = Animation::Serializer::ReadFloat(input, ignore);
		input = Animation::Serializer::ReadFloat(input, ignore);
	}

	input = Animation::Serializer::ReadUInt(input, weightSize);
	gSample->weights.Resize(weightSize);
	for (unsigned int i = 0; i < weightSize; ++i) {
		input = Animation::Serializer::ReadFloat(input, gSample->weights[i].x);
		input = Animation::Serializer::ReadFloat(input, gSample->weights[i].y);
		input = Animation::Serializer::ReadFloat(input, gSample->weights[i].z);
		input = Animation::Serializer::ReadFloat(input, gSample->weights[i].w);
	}

	input = Animation::Serializer::ReadUInt(input, infSize);
	gSample->influences.Resize(infSize);
	for (unsigned int i = 0; i < infSize; ++i) {
		input = Animation::Serializer::ReadUInt(input, gSample->influences[i].x);
		input = Animation::Serializer::ReadUInt(input, gSample->influences[i].y);
		input = Animation::Serializer::ReadUInt(input, gSample->influences[i].z);
		input = Animation::Serializer::ReadUInt(input, gSample->influences[i].w);
	}

	//gSample->skinned.Resize(gSample->vertices.Size() + gSample->normals.Size());
}

extern "C" void SetAnimationData(const char* bindState, const char* restState, const char* walkingData, const char* runningData) {
	Animation::Serializer::DeserializeState(gSample->bindPose, bindState);
	Animation::Serializer::DeserializeState(gSample->restPose, restState);
	Animation::Serializer::DeserializeData(gSample->aniamtionDataA, walkingData);
	Animation::Serializer::DeserializeData(gSample->aniamtionDataB, runningData);

	gSample->animatedPoseA = gSample->restPose;
	gSample->animatedPoseB = gSample->restPose;
	gSample->blendedPose = gSample->restPose;

	gSample->playTimeA = 0.0f;
	gSample->playTimeB = 0.0f;

	gSample->invBindPosePalette.Resize(gSample->bindPose.Size());
	gSample->animatedPosePalette.Resize(gSample->bindPose.Size());

	Animation::ToMatrixPalette(gSample->invBindPosePalette[0].v, gSample->bindPose);

	for (unsigned int i = 0; i < gSample->bindPose.Size(); ++i) {
		gSample->invBindPosePalette[i] = inverse(gSample->invBindPosePalette[i]);
	}

	Animation::Builder::Track blendTrack;
	blendTrack.SetJointID(0);
	blendTrack.SetTarget(Animation::Data::Component::Position);
	blendTrack.PushFrame(MakeFrame(0, 0, 0, 0));
	blendTrack.PushFrame(MakeFrame(0.4f * 2.0f, 0, 0, 0));
	blendTrack.PushFrame(MakeFrame(0.6f * 2.0f, 0, 1.0f, 0));
	blendTrack.PushFrame(MakeFrame(1.4f * 2.0f, 0, 1.0f, 0));
	blendTrack.PushFrame(MakeFrame(1.6f * 2.0f, 0, 0, 0));
	blendTrack.PushFrame(MakeFrame(2 * 2.0f, 0, 0, 0));
	Animation::Builder::Clip blendCurve;
	blendCurve.SetName("Blend Time");
	blendCurve.PushTrack(blendTrack);
	gSample->blendCurve = Animation::Builder::Convert(blendCurve);

	gSample->readPositions.Set(gSample->vertices[0].v, gSample->vertices.Size() * 3, 0, 0);
	gSample->readNormals.Set(gSample->normals[0].v, gSample->normals.Size() * 3, 0, 0);
	gSample->readInfluences.Set(gSample->influences[0].v, gSample->influences.Size() * 4, 0, 0);
	gSample->readWeights.Set(gSample->weights[0].v, gSample->weights.Size() * 4, 0, 0);

	gSample->writePositions.Set(gSkinnedVerts[0].v, gSample->vertices.Size() * 3, 6 * sizeof(float), 0);
	gSample->writeNormals.Set(gSkinnedVerts[0].v, gSample->vertices.Size() * 3, 6 * sizeof(float), 3 * sizeof(float));
}

extern "C" void Update(float dt) {
	
}

extern "C" void Shutdown() {
	gSample->~Sample();
	Animation::Internal::Free(gSample);
	Animation::Internal::Free(gSkinnedVerts);
	gSample = 0;
	gSkinnedVerts = 0;
}

void* MemCpy(void* dest, const void* src, unsigned int len) {
	char* d = (char*)dest;
	const char* s = (const char*)src;
	while (len--) {
		*d++ = *s++;
	}
	return dest;
}

Animation::Builder::Frame MakeFrame(float time, float in, float value, float out) {
	Animation::Builder::Frame result;

	result.time = time;
	result.in[0] = in;
	result.value[0] = value;
	result.out[0] = out;
	return result;
}

#include "../AnimationSkin.cpp"