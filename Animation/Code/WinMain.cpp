#define _CRT_SECURE_NO_WARNINGS

#include "AnimationState.h"
#include "AnimationData.h"
#include "AnimationHelpers.h"
#include "AnimationSkin.h"
#include "AnimationBlend.h"

#include <vector>
#include <fstream>
#include <sstream>

#include "glad.h"

#pragma comment(lib, "opengl32.lib") 
#pragma comment(lib, "Shcore.lib")
#if _DEBUG
	#include <crtdbg.h>
	#pragma comment(linker, "/subsystem:console")
	// TODO
	/*int main(int argc, const char** argv) {
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
		WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWDEFAULT);
		return 0;
	}*/
#else 
	#ifdef _WIN64
		#pragma comment(linker, "/subsystem:windows,5.02")
	#else
		#pragma comment(linker, "/subsystem:windows,5.01")
	#endif
#endif

using std::vector;

///////////////////////////////////////////////////////////////////////////////////////
// Usage examples
// CPUSkinnedCharacterSample - displays a CPU skinned character mesh
// BlendedCharacterSample - displays a cpu skinned mesh that cycles between animations
// DisplayCurvesSample - displays the raw curves of a test animation track
///////////////////////////////////////////////////////////////////////////////////////

class CPUSkinnedCharacterSample {
protected: // Raw model data. mSkinned is a write buffer, it contains interleaved vertex and normal data
	std::vector<float> mVertices;
	std::vector<float> mNormals;
	std::vector<float> mSkinned;
	std::vector<float> mTexCoords;
	std::vector<unsigned int> mInfluences;
	std::vector<float> mWeights;
	std::vector<unsigned int> mIndices;
protected: // Animation data. the two vectors are flat matrix arrays, elements = num matrices * 16
	Animation::Data mAniamtionData;
	Animation::State mAnimatedPose;
	Animation::State mRestPose;
	Animation::State mBindPose;
	float mPlayTime = 0.0f;
	std::vector<float> mInvBindPosePalette;
	std::vector<float> mAnimatedPosePalette;
protected: // The animation system works trough descriptors to allow for things like interleaved arrays
	// Read positions points to mVertices, writePositions points to mSkinnedVertices. The actual animation
	// API doesn't care how the underlying data is stored
	Animation::Skin::Descriptor<float, 3> mReadPositions;
	Animation::Skin::Descriptor<float, 3> mReadNormals;
	Animation::Skin::Descriptor<float, 3> mWritePositions;
	Animation::Skin::Descriptor<float, 3> mWriteNormals;
	Animation::Skin::Descriptor<unsigned int, 4> mReadInfluences;
	Animation::Skin::Descriptor<float, 4> mReadWeights;
protected: // OpenGL 3.3 stuff to draw the mesh to screen
	GLuint mVAO = 0;
	GLuint mStaticVBO = 0;
	GLuint mDynamicVBO = 0;
	GLuint mIBO = 0;
	GLuint mShader = 0;
	bool mOpenGLInitialized = false;
private:
	void LoadModel();
	void LoadAnimation();
	void InitDescriptors();
	void InitOpenGL();
public:
	void Initialize();
	void Update(float dt);
	void Render(float aspect);
	void Shutdown();
};

///////////////////////////////////////////////////////////////////////////////////////
// CPUSkinnedCharacterSample Implementation
///////////////////////////////////////////////////////////////////////////////////////

void CPUSkinnedCharacterSample::LoadModel() {
	std::ifstream file("Assets/womanMesh.txt");
	std::stringstream buffer;
	buffer << file.rdbuf();
	const std::string& str = buffer.str();
	const char* input = str.c_str();

	unsigned int posSize = 0;
	unsigned int normSize = 0;
	unsigned int texSize = 0;
	unsigned int weightSize = 0;
	unsigned int infSize = 0;
	unsigned int idxSize = 0;

	input = Animation::ReadUInt(input, posSize);
	mVertices.resize(posSize * 3);
	for (unsigned int i = 0; i < posSize; ++i) {
		input = Animation::ReadFloat(input, mVertices[i * 3 + 0]);
		input = Animation::ReadFloat(input, mVertices[i * 3 + 1]);
		input = Animation::ReadFloat(input, mVertices[i * 3 + 2]);
	}

	input = Animation::ReadUInt(input, normSize);
	mNormals.resize(normSize * 3);
	for (unsigned int i = 0; i < normSize; ++i) {
		input = Animation::ReadFloat(input, mNormals[i * 3 + 0]);
		input = Animation::ReadFloat(input, mNormals[i * 3 + 1]);
		input = Animation::ReadFloat(input, mNormals[i * 3 + 2]);
	}

	input = Animation::ReadUInt(input, texSize);
	mTexCoords.resize(texSize * 2);
	for (unsigned int i = 0; i < texSize; ++i) {
		input = Animation::ReadFloat(input, mTexCoords[i * 2 + 0]);
		input = Animation::ReadFloat(input, mTexCoords[i * 2 + 1]);
	}

	input = Animation::ReadUInt(input, weightSize);
	mWeights.resize(weightSize * 4);
	for (unsigned int i = 0; i < weightSize; ++i) {
		input = Animation::ReadFloat(input, mWeights[i * 4 + 0]);
		input = Animation::ReadFloat(input, mWeights[i * 4 + 1]);
		input = Animation::ReadFloat(input, mWeights[i * 4 + 2]);
		input = Animation::ReadFloat(input, mWeights[i * 4 + 3]);
	}

	input = Animation::ReadUInt(input, infSize);
	mInfluences.resize(infSize * 4);
	for (unsigned int i = 0; i < infSize; ++i) {
		input = Animation::ReadUInt(input, mInfluences[i * 4 + 0]);
		input = Animation::ReadUInt(input, mInfluences[i * 4 + 1]);
		input = Animation::ReadUInt(input, mInfluences[i * 4 + 2]);
		input = Animation::ReadUInt(input, mInfluences[i * 4 + 3]);
	}

	input = Animation::ReadUInt(input, idxSize);
	mIndices.resize(idxSize);
	for (unsigned int i = 0; i < idxSize; ++i) {
		input = Animation::ReadUInt(input, mIndices[i]);
	}

	buffer.clear();
	file.close();

	mSkinned.resize(mVertices.size() + mNormals.size());
}

void CPUSkinnedCharacterSample::LoadAnimation() {
	std::ifstream file("Assets/bindState.txt");
	std::stringstream buffer;
	buffer << file.rdbuf();
	const char* input = buffer.str().c_str();
	mBindPose.DeSerializeFromString(input);
	buffer.clear();
	file.close();

	file = std::ifstream("Assets/restState.txt");
	buffer << file.rdbuf();
	input = buffer.str().c_str();
	mRestPose.DeSerializeFromString(input);
	mAnimatedPose = mRestPose;
	buffer.clear();
	file.close();

	file = std::ifstream("Assets/walkingData.txt");
	buffer << file.rdbuf();
	input = buffer.str().c_str();
	mAniamtionData.DeSerializeFromString(input);
	buffer.clear();
	file.close();

	mPlayTime = 0.0f;
	mInvBindPosePalette.resize(mBindPose.Size() * 16);
	mAnimatedPosePalette.resize(mBindPose.Size() * 16);
}

void CPUSkinnedCharacterSample::InitDescriptors() {
	// TODO
}

void CPUSkinnedCharacterSample::InitOpenGL() {
	mOpenGLInitialized = false;

	if (sizeof(unsigned int) != sizeof(float)) {
		return;
	}
	if (mVertices.size() == 0) {
		return;
	}
	unsigned int numVerts = mVertices.size() / 3;
	if (numVerts != mNormals.size() / 3) {
		return;
	}
	if (numVerts != mInfluences.size() / 4) {
		return;
	}
	if (numVerts != mWeights.size() / 4) {
		return;
	}
	if (numVerts != mTexCoords.size() / 2) {
		return;
	}

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mStaticVBO);
	glGenBuffers(1, &mDynamicVBO);
	glGenBuffers(1, &mIBO);

	// Make interleaved static VBO
	GLsizei bytes = 0;
	bytes += sizeof(float) * mTexCoords.size();
	bytes += sizeof(unsigned int) * mInfluences.size();
	bytes += sizeof(float) * mWeights.size();

	std::vector<float> staticBufferData;
	staticBufferData.resize(mTexCoords.size() + mInfluences.size() + mWeights.size());

	unsigned int current = 0;
	for (unsigned int i = 0; i < numVerts; ++i) {
		staticBufferData[current++] = mTexCoords[i * 3 + 0];
		staticBufferData[current++] = mTexCoords[i * 3 + 1];
		staticBufferData[current++] = (float)mInfluences[i * 3 + 0];
		staticBufferData[current++] = (float)mInfluences[i * 3 + 1];
		staticBufferData[current++] = (float)mInfluences[i * 3 + 2];
		staticBufferData[current++] = (float)mInfluences[i * 3 + 3];
		staticBufferData[current++] = mWeights[i * 3 + 0];
		staticBufferData[current++] = mWeights[i * 3 + 1];
		staticBufferData[current++] = mWeights[i * 3 + 2];
		staticBufferData[current++] = mWeights[i * 3 + 3];
	}

	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mStaticVBO);
	glBufferData(GL_ARRAY_BUFFER, bytes, &staticBufferData[0], GL_STATIC_DRAW);

	bytes = 0;
	bytes += sizeof(float) * mVertices.size();
	bytes += sizeof(float) * mNormals.size();
	mSkinned.resize(mVertices.size() + mNormals.size());
	memset(&mSkinned[0], 0, mSkinned.size());

	glBindBuffer(GL_ARRAY_BUFFER, mDynamicVBO);
	glBufferData(GL_ARRAY_BUFFER, bytes, &mSkinned[0], GL_STREAM_DRAW);

	if (mIndices.size() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mIndices.size(), &mIndices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// TODO: Compile and set up shaders

	mOpenGLInitialized = true;
}

void CPUSkinnedCharacterSample::Initialize() {
	LoadModel();
	LoadAnimation();
	InitDescriptors();
	InitOpenGL();
}

void CPUSkinnedCharacterSample::Update(float dt) {
	if (!mOpenGLInitialized) {
		return;
	}
	mPlayTime = mAniamtionData.Sample(mAnimatedPose, mPlayTime + dt, true);

	unsigned int numJoints = mAnimatedPose.Size();
	mAnimatedPose.ToMatrixPalette(&mAnimatedPosePalette[0], numJoints * 16);
	for (unsigned int i = 0; i < numJoints; ++i) {
		Animation::MultiplyMatrices(&mAnimatedPosePalette[i * 16], &mAnimatedPosePalette[i * 16], &mInvBindPosePalette[i * 16]);
	}

	Animation::Skin::Apply(mWritePositions, mReadPositions, 1.0f, &mAnimatedPosePalette[0], mReadInfluences, mReadWeights);
	Animation::Skin::Apply(mWriteNormals, mReadNormals, 0.0f, &mAnimatedPosePalette[0], mReadInfluences, mReadWeights);
}

void CPUSkinnedCharacterSample::Render(float aspect) {
	if (!mOpenGLInitialized) {
		return;
	}

	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mStaticVBO);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (GLvoid*)0);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (GLvoid*)(2 * sizeof(float)));
	glVertexAttribIPointer(5, 4, GL_UNSIGNED_INT, 10 * sizeof(float), (GLvoid*)(6 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, mDynamicVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

	if (mIndices.size() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
		glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, (GLvoid*)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		unsigned int numVerts = mVertices.size() / 3;
		glDrawArrays(GL_TRIANGLES, 0, numVerts);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CPUSkinnedCharacterSample::Shutdown() {
	mVertices.clear();
	mNormals.clear();
	mSkinned.clear();
	mTexCoords.clear();
	mInfluences.clear();
	mWeights.clear();
	mIndices.clear();
	mInvBindPosePalette.clear();
	mAnimatedPosePalette.clear();
	mPlayTime = 0.0f;
	mAniamtionData = Animation::Data();
	mAnimatedPose = Animation::State();
	mRestPose = Animation::State();
	mBindPose = Animation::State();

	if (!mOpenGLInitialized) {
		return;
	}

	glDeleteBuffers(1, &mIBO);
	glDeleteBuffers(1, &mStaticVBO);
	glDeleteBuffers(1, &mDynamicVBO);
	glDeleteVertexArrays(1, &mVAO);
	mOpenGLInitialized = false;
}
///////////////////////////////////////////////////////////////////////////////////////
// Everything below this line is just WinMain + OpenGL loader. Nothing too interesting.
///////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

int main(int argc, char** argv) {

	int y;
	std::cin >> y;
	return 0;
}