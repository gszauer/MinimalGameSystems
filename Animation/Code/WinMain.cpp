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
public:
	void Initialize();
	void Update(float dt);
	void Render(float aspect);
	void Shutdown();

	unsigned int Serialize(char* output, unsigned int outputSize) const;
	bool DeSerialize(const char* input, unsigned int inputSize);
	unsigned int SerializedSize() const;
};

///////////////////////////////////////////////////////////////////////////////////////
// CPUSkinnedCharacterSample Implementation
///////////////////////////////////////////////////////////////////////////////////////

void CPUSkinnedCharacterSample::Initialize() {
	mOpenGLInitialized = false;
	std::ifstream t("Assets/CPUSample.txt");
	std::stringstream buffer;
	buffer << t.rdbuf();

	const std::string& str = buffer.str();
	if (!DeSerialize(str.c_str(), str.size())) {
		return;
	}
	mPlayTime = 0.0f;

	// TODO: Setup descriptors

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

unsigned int CPUSkinnedCharacterSample::Serialize(char* output, unsigned int outputSize) const {
	// TOOD
	return 0;
}

#define READ_UINT_BLOCK(target, count, comps, stream) \
	target.resize(count * comps); \
	for (unsigned int i = 0; i < count; ++i) { \
		for (unsigned int j = 0; j < comps; ++j) { \
			target[i * comps + j] = ReadUInt(stream); \
			stream = AdvanceUInt(stream); \
		} \
	}

#define READ_FLOAT_BLOCK(target, count, comps, stream) \
	target.resize(count * comps); \
	for (unsigned int i = 0; i < count; ++i) { \
		for (unsigned int j = 0; j < comps; ++j) { \
			target[i * comps + j] = ReadFloat(stream); \
			stream = AdvanceFloat(stream); \
		} \
	}

#define READ_UINT(name, stream) \
	unsigned int name = ReadUInt(stream); \
	stream = AdvanceUInt(stream);

bool CPUSkinnedCharacterSample::DeSerialize(const char* input, unsigned int inputSize) {
	const char* start = input;
	// TODO: Confirm that input size is big enough to hold the header data

	//READ_UINT(numVerts, input);
	//READ_UINT(numNorms, input);
	//READ_UINT(numTexCoords, input);
	//READ_UINT(numInfluences, input);
	//READ_UINT(numWeights, input);
	//READ_UINT(numIndices, input);
	//READ_UINT(animatedDataSize, input);
	//READ_UINT(restPoseSize, input);
	//READ_UINT(bindPoseSize, input);

	// TODO: Confirm that input size is big enough to hold actual data

	//READ_FLOAT_BLOCK(mVertices, numVerts, 3, input);
	//READ_FLOAT_BLOCK(mNormals, numNorms, 3, input);
	//READ_FLOAT_BLOCK(mTexCoords, numTexCoords, 2, input);
	//READ_UINT_BLOCK(mInfluences, numInfluences, 4, input);
	//READ_FLOAT_BLOCK(mWeights, numWeights, 4, input);
	//READ_UINT_BLOCK(mIndices, numIndices, 1, input);

	// TODO: Deserialize animatedDataSize
	// TODO: Deserialize restPoseSize
	// TODO: Deserialize bindPoseSize

	if (input - start != inputSize) {
		return false;
	}

	return true;
}

unsigned int CPUSkinnedCharacterSample::SerializedSize() const {
	unsigned int total = 0;

	total += sizeof(unsigned int); // Num Verts (position)
	total += sizeof(unsigned int); // Num Verts (normal)
	total += sizeof(unsigned int); // Num Verts (texcoords)
	total += sizeof(unsigned int); // Num Verts (influences)
	total += sizeof(unsigned int); // Num Verts (weights)
	total += sizeof(unsigned int); // Num Verts (indices)
	total += sizeof(unsigned int); // mAniamtionData size
	total += sizeof(unsigned int); // mRestPose size
	total += sizeof(unsigned int); // mBindPose size

	total += sizeof(float) * mVertices.size();
	total += sizeof(float) * mNormals.size();
	total += sizeof(float) * mTexCoords.size();
	total += sizeof(float) * mInfluences.size();
	total += sizeof(float) * mWeights.size();
	total += sizeof(float) * mIndices.size();

	total += mAniamtionData.SerializedSize(); // Animation data
	//total += mRestPose.SerializedSize();
	//total += mBindPose.SerializedSize();
	// TODO
	
	return total;
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