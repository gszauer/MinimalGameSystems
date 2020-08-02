#ifndef _H_SKINNEDSAMPLE_
#define _H_SKINNEDSAMPLE_

#include "ISample.h"

class SkinnedSample : public ISample {
protected: // Raw model data. mSkinned is a write buffer, it contains interleaved vertex and normal data
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec3> mSkinned;
	std::vector<vec2> mTexCoords;
	std::vector<uivec4> mInfluences;
	std::vector<vec4> mWeights;
	std::vector<unsigned int> mIndices;
protected: // Animation data. the two vectors are flat matrix arrays, elements = num matrices * 16
	Animation::Data mAniamtionData;
	Animation::State mAnimatedPose;
	Animation::State mRestPose;
	Animation::State mBindPose;
	float mPlayTime = 0.0f;
	std::vector<mat4> m_InvBindPosePalette;
	std::vector<mat4> m_AnimatedPosePalette;
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
	GLuint mCharacterVAO = 0;
	GLuint mCharacterStaticVBO = 0;
	GLuint mCharacterDynamicVBO = 0;
	GLuint mCharacterIBO = 0;
	GLuint mCharacterShader = 0;
	GLuint mCharacterTexture = 0;
	bool mOpenGLInitialized = false;
protected:
	GLint mCharacterUniformVP = 0;
	GLint mCharacterUniformModel = 0;
	GLint mCharacterUniformTexture = 0;
	GLint mCharacterAttribPosition = 0;
	GLint mCharacterAttribTexCoord = 0;
	GLint mCharacterAttribNormal = 0;
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

#endif