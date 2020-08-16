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
	Vector<vec3> skinned; // Interleaved pos/rot. Set in c++, read by js

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

void StartToLoadFile(const char* address);
int IsFileLoaded(const char* address);
int FileSize(const char* address);
void FileCopyContents(const char* address, char* target);
void JavascriptLog(const char* message);
Animation::Builder::Frame MakeFrame(float time, float in, float value, float out);
void SetModelData(const char* input);
void SetAnimationData();
extern "C" void JS_LogInt(int x);

extern "C" void Initialize() {
	gSample = (Sample*)Animation::Internal::Allocate(sizeof(Sample));
	new (gSample) Vector<Sample>();
	gSample->playTimeA = 0.0f;
	gSample->playTimeB = 0.0f;
	gSample->blendTime = 0.0f;

	gIsDownloading = true;

	StartToLoadFile("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/womanMesh.txt");
	StartToLoadFile("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/bindState.txt");
	StartToLoadFile("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/restState.txt");
	StartToLoadFile("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/runningData.txt");
	StartToLoadFile("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/walkingData.txt");
}

extern "C" void Update(float dt) {
	if (gIsDownloading && 
		IsFileLoaded("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/womanMesh.txt") &&
		IsFileLoaded("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/bindState.txt") &&
		IsFileLoaded("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/restState.txt") &&
		IsFileLoaded("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/runningData.txt") &&
		IsFileLoaded("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/walkingData.txt")
		) {
		// Downloads finished
		JavascriptLog("All files downloaded");

		int bufferSize = 0;
		int womanMeshSize = FileSize("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/womanMesh.txt");
		if (womanMeshSize > bufferSize) {
			bufferSize = womanMeshSize;
		}
		int bindStateSize = FileSize("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/bindState.txt");
		if (bindStateSize > bufferSize) {
			bufferSize = bindStateSize;
		}
		int restStateSize = FileSize("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/restState.txt");
		if (restStateSize > bufferSize) {
			bufferSize = restStateSize;
		}
		int runningDataSize = FileSize("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/runningData.txt");
		if (runningDataSize > bufferSize) {
			bufferSize = runningDataSize;
		}
		int walkingDataSize = FileSize("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/walkingData.txt");
		if (walkingDataSize > bufferSize) {
			bufferSize = walkingDataSize;
		}

		JS_LogInt(bufferSize + 1);
		char* readBuffer = (char*)Animation::Internal::Allocate(bufferSize + 1);
		
		FileCopyContents("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/womanMesh.txt", readBuffer);
		readBuffer[womanMeshSize] = '\0';
		SetModelData(readBuffer);

		FileCopyContents("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/bindState.txt", readBuffer);
		readBuffer[bindStateSize] = '\0';
		//Animation::Serializer::DeserializeState(gSample->bindPose, readBuffer);

		FileCopyContents("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/restState.txt", readBuffer);
		readBuffer[restStateSize] = '\0';
		//Animation::Serializer::DeserializeState(gSample->restPose, readBuffer);

		FileCopyContents("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/runningData.txt", readBuffer);
		readBuffer[runningDataSize] = '\0';
		//Animation::Serializer::DeserializeData(gSample->aniamtionDataB, readBuffer);

		FileCopyContents("https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/walkingData.txt", readBuffer);
		readBuffer[walkingDataSize] = '\0';
		//Animation::Serializer::DeserializeData(gSample->aniamtionDataA, readBuffer);

		//SetAnimationData();

		Animation::Internal::Free(readBuffer);

		gIsDownloading = false;
	}
	if (gIsDownloading) {
		//JavascriptLog("downloading");
	}
	else {
		// Update
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
	JS_LogInt(pivot);

	input = Animation::Serializer::ReadUInt(input, posSize);
	gSample->vertices.Resize(posSize);
	//JS_LogInt(posSize);
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

#include "../AnimationSkin.cpp"

extern "C" void JS_StartToLoadFile(int  address, int stringLen);
extern "C" int JS_IsFileLoaded(int  address, int stringLen);
extern "C" int JS_FileSize(int  address, int stringLen);
extern "C" void JS_FileCopyContents(int  address, int addressLen, int target);
extern "C" void JS_JavascriptLog(int  message, int stringLen);

int StrLen(const char* str) {
    int i = 0;
    for(; str[i]!='\0'; ++i);
    return i;
}

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