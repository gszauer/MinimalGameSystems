#define _CRT_SECURE_NO_WARNINGS

#include "vector.h"
#include "math.h"
#include "../AnimationInternal.h"
#include "../AnimationData.h"
#include "../AnimationState.h"
#include "../AnimationSkin.h"
#include "../AnimationBlend.h"
#include "../AnimationSerializer.h"
#include "../AnimationBuilder.h"

struct Sample {
	Vector<vec3> vertices;
	Vector<vec3> normals;
	Vector<uivec4> influences; 
	Vector<vec4> weights;
	Vector<vec3> skinned; 

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

Sample* gSample = 0;
bool gIsDownloading = false;
bool gIsRunning = false;

void StartToLoadFile(const char* address);
int IsFileLoaded(const char* address);
int FileSize(const char* address);
void FileCopyContents(const char* address, char* target);
void JavascriptLog(const char* message);
Animation::Builder::Frame MakeFrame(float time, float in, float value, float out);
void SetModelData(const char* input);
void SetAnimationData();
#if !_WIN32
extern "C" void JS_LogInt(int x);
#else
void JS_LogInt(int x);
#endif

#if _WIN32
#include <new>
#endif

#if _WIN32
const char* gMeshFile = "Assets/womanMesh.txt";
const char* gBindFile = "Assets/bindState.txt";
const char* gRestFile = "Assets/restState.txt";
const char* gRunningFile = "Assets/runningData.txt";
const char* gWalkingFile = "Assets/walkingData.txt";
#else
const char* gMeshFile = "https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/womanMesh.txt";
const char* gBindFile = "https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/bindState.txt";
const char* gRestFile = "https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/restState.txt";
const char* gRunningFile = "https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/runningData.txt";
const char* gWalkingFile = "https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/walkingData.txt";
#endif

extern "C" int IsRunning() {
	return (int)gIsRunning;
}

extern "C" int GetSkinnedVertexPointer() {
	return (int)(gSample->skinned[0].v);
}

extern "C" void Initialize() {
	gSample = (Sample*)Animation::Internal::Allocate(sizeof(Sample));
	new (gSample) Vector<Sample>();
	gSample->playTimeA = 0.0f;
	gSample->playTimeB = 0.0f;
	gSample->blendTime = 0.0f;

	gIsDownloading = true;
	gIsRunning = false;

	StartToLoadFile(gMeshFile);
	StartToLoadFile(gBindFile);
	StartToLoadFile(gRestFile);
	StartToLoadFile(gRunningFile);
	StartToLoadFile(gWalkingFile);
}

extern "C" void Update(float dt) {
	if (gIsDownloading && 
		IsFileLoaded(gMeshFile) &&
		IsFileLoaded(gBindFile) &&
		IsFileLoaded(gRestFile) &&
		IsFileLoaded(gRunningFile) &&
		IsFileLoaded(gWalkingFile)
		) {
		// Downloads finished
		JavascriptLog("C - Files finished downloading");

		int bufferSize = 0;
		int womanMeshSize = FileSize(gMeshFile);
		if (womanMeshSize > bufferSize) {
			bufferSize = womanMeshSize;
		}
		int bindStateSize = FileSize(gBindFile);
		if (bindStateSize > bufferSize) {
			bufferSize = bindStateSize;
		}
		int restStateSize = FileSize(gRestFile);
		if (restStateSize > bufferSize) {
			bufferSize = restStateSize;
		}
		int runningDataSize = FileSize(gRunningFile);
		if (runningDataSize > bufferSize) {
			bufferSize = runningDataSize;
		}
		int walkingDataSize = FileSize(gWalkingFile);
		if (walkingDataSize > bufferSize) {
			bufferSize = walkingDataSize;
		}

		//JS_LogInt(bufferSize + 1);
		char* readBuffer = (char*)Animation::Internal::Allocate(bufferSize + 1);
		
		FileCopyContents(gMeshFile, readBuffer);
		readBuffer[womanMeshSize] = '\0';
		SetModelData(readBuffer);

		FileCopyContents(gBindFile, readBuffer);
		readBuffer[bindStateSize] = '\0';
		Animation::Serializer::DeserializeState(gSample->bindPose, readBuffer);

		FileCopyContents(gRestFile, readBuffer);
		readBuffer[restStateSize] = '\0';
		Animation::Serializer::DeserializeState(gSample->restPose, readBuffer);

		FileCopyContents(gRunningFile, readBuffer);
		readBuffer[runningDataSize] = '\0';
		Animation::Serializer::DeserializeData(gSample->aniamtionDataB, readBuffer);

		FileCopyContents(gWalkingFile, readBuffer);
		readBuffer[walkingDataSize] = '\0';
		Animation::Serializer::DeserializeData(gSample->aniamtionDataA, readBuffer);

		SetAnimationData();

		Animation::Internal::Free(readBuffer);

		gIsDownloading = false;
	}
	if (gIsDownloading) {
		//JavascriptLog("downloading");
	}
	else {
		gSample->playTimeA = gSample->aniamtionDataA.Sample(gSample->animatedPoseA, gSample->playTimeA + dt, true);
		gSample->playTimeB = gSample->aniamtionDataB.Sample(gSample->animatedPoseB, gSample->playTimeB + dt, true);

		float result[4] = { 0.0f };
		gSample->blendTime = gSample->blendCurve.SampleTrack(result, 0, gSample->blendTime + dt, true);
		Animation::Blend(gSample->blendedPose, gSample->animatedPoseA, gSample->animatedPoseB, result[0]);

		Animation::ToMatrixPalette(gSample->animatedPosePalette[0].v, gSample->blendedPose);

		Animation::Skin::Apply(gSample->writePositions, gSample->readPositions, 1.0f, gSample->animatedPosePalette[0].v, gSample->invBindPosePalette[0].v, gSample->readInfluences, gSample->readWeights);
		//Animation::Skin::Apply(gSample->writeNormals, gSample->readNormals, 0.0f, gSample->animatedPosePalette[0].v, gSample->invBindPosePalette[0].v, gSample->readInfluences, gSample->readWeights);
		gIsRunning = true;
	}
}

extern "C" void Shutdown() {
	gSample->~Sample();
	Animation::Internal::Free(gSample);
	gSample = 0;
}

void SetModelData(const char* input) {
	//JavascriptLog(input);
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

	gSample->skinned.Resize(gSample->vertices.Size() + gSample->normals.Size());
}

void SetAnimationData() {
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

	gSample->writePositions.Set(gSample->skinned[0].v, gSample->vertices.Size() * 3, 6 * sizeof(float), 0);
	gSample->writeNormals.Set(gSample->skinned[0].v, gSample->vertices.Size() * 3, 6 * sizeof(float), 3 * sizeof(float));
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

int StrLen(const char* str) {
    int i = 0;
    for(; str[i]!='\0'; ++i);
    return i;
}

#if _WIN32
#include <io.h> 
#include <cstdio>
#include <iostream>
#define access _access_s

void StartToLoadFile(const char* address) {}

int IsFileLoaded(const char* address) {
	bool available = access(address, 0) == 0;
	return available;
}

int FileSize(const char* address) {
	FILE* p_file = NULL;
	p_file = fopen(address, "rb");
	fseek(p_file, 0, SEEK_END);
	int size = ftell(p_file);
	fclose(p_file);
	return size;
}

void FileCopyContents(const char* address, char* target) {
	char* buffer = 0;
	long length;
	FILE* f = fopen(address, "rb");

	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = new char[length];
		if (buffer) {
			fread(buffer, 1, length, f);
		}
		fclose(f);
	}

	if (buffer) {
		memcpy(target, buffer, length);
		delete[] buffer;
	}
}

void JavascriptLog(const char* message) {
	std::cout << message << "\n";
}

void JS_LogInt(int x) {
	std::cout << x << "\n";
}

#else
extern "C" void JS_StartToLoadFile(int  address, int stringLen);
extern "C" int JS_IsFileLoaded(int  address, int stringLen);
extern "C" int JS_FileSize(int  address, int stringLen);
extern "C" void JS_FileCopyContents(int  address, int addressLen, int target);
extern "C" void JS_JavascriptLog(int  message, int stringLen);

void StartToLoadFile(const char* address) {
	JS_StartToLoadFile((int)address, StrLen(address));
}

int IsFileLoaded(const char* address) {
	return JS_IsFileLoaded((int)address, StrLen(address));
}

int FileSize(const char* address) {
	return JS_FileSize((int)address, StrLen(address));
}

void FileCopyContents(const char* address, char* target) {
	JS_FileCopyContents((int)address, StrLen(address), (int)target);
}

void JavascriptLog(const char* message) {
	JS_JavascriptLog((int)message, StrLen(message));
}
#endif

#if _WIN32
int main(int argc, char** argv) {
	Initialize();
	Update(0.0f);
	Shutdown();
}
#endif