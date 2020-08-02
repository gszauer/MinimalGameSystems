#ifndef _H_BLENDSAMPLE_
#define _H_BLEND_SAMPLE_


#include "ISample.h"

// TODO: Migrate samples over to common data structures first

class BlendSample : public ISample {
protected: // Raw model data. mSkinned is a write buffer, it contains interleaved vertex and normal data
	std::vector<float> mVertices;
	std::vector<float> mNormals;
	std::vector<float> mSkinned;
	std::vector<float> mTexCoords;
	std::vector<unsigned int> mInfluences;
	std::vector<float> mWeights;
	std::vector<unsigned int> mIndices;
protected: // Animation data. the two vectors are flat matrix arrays, elements = num matrices * 16
	Animation::Data mAniamtionDataA;
	Animation::State mAnimatedPoseA;
	Animation::Data mAniamtionDataB;
	Animation::State mAnimatedPoseB;
	Animation::State mAnimatedPose;
	Animation::State mRestPose;
	Animation::State mBindPose;
	float mPlayTime = 0.0f;
	Animation::Data mBlendCurve;
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
	void DrawAnimatedModel(float* viewProjection, float* model);
public:
	void Initialize();
	void Update(float dt);
	void Render(float aspect);
	void Shutdown();
};

#endif // !_H_BLENDSAMPLE_