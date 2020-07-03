#include "AnimationState.h"
#include "AnimationData.h"
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

namespace Animation {
	extern void MultiplyMatrices(float* out, const float* a, const float* b);
}

///////////////////////////////////////////////////////////////////////////////////////
// Usage examples
// CPUSkinnedCharacterSample - displays a CPU skinned character mesh
// BlendedCharacterSample - displays a cpu skinned mesh that cycles between animations
// CurvesSample - displays the raw curves of a test animation track
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
	bool mIsValid = false;
public:
	void Initialize();
	void Update(float dt);
	void Render(float aspect);
	void Shutdown();

	unsigned int Serialize(char* output, unsigned int outputSize) const;
	void DeSerialize(const char* input, unsigned int inputSize);
	unsigned int SerializedSize() const;
};

///////////////////////////////////////////////////////////////////////////////////////
// CPUSkinnedCharacterSample Implementation
///////////////////////////////////////////////////////////////////////////////////////

void CPUSkinnedCharacterSample::Initialize() {
	mIsValid = false;
	std::ifstream t("Assets/CPUSample.txt");
	std::stringstream buffer;
	buffer << t.rdbuf();

	const std::string& str = buffer.str();
	DeSerialize(str.c_str(), str.size());
	mPlayTime = 0.0f;

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
		// TODO
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	mIsValid = true;
}

void CPUSkinnedCharacterSample::Update(float dt) {
	if (!mIsValid) {
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
	if (!mIsValid) {
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
		// TODO: Draw arrays
	}
	else {
		// TODO: Draw elements
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

	if (!mIsValid) {
		return;
	}

	glDeleteBuffers(1, &mIBO);
	glDeleteBuffers(1, &mStaticVBO);
	glDeleteBuffers(1, &mDynamicVBO);
	glDeleteVertexArrays(1, &mVAO);
}

unsigned int CPUSkinnedCharacterSample::Serialize(char* output, unsigned int outputSize) const {
	// TOOD
	return 0;
}

void CPUSkinnedCharacterSample::DeSerialize(const char* input, unsigned int inputSize) {
	// TODO
}

unsigned int CPUSkinnedCharacterSample::SerializedSize() const {
	// TODO
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
// Everything below this line is just WinMain + OpenGL loader. Nothing too interesting.
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	return 0;
}