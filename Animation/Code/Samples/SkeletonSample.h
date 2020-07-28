#ifndef _H_SKELETON_SAMPLE_
#define _H_SKELETON_SAMPLE_

#include "ISample.h"

class SkeletonSample : public ISample {
protected:
	std::vector<float> mVertices;
	std::vector<float> mSkinned;
protected:
	Animation::Data mAnimationData;
	Animation::State mBindPose;
	Animation::State mRestPose;
	Animation::State mAnimatedPose;
	float mPlayTime = 0.0f;
protected:
	GLuint mSkeletonVAO = 0;
	GLuint mSkeletonVBO = 0;
	GLuint mSkeletonShader = 0;
protected:
	GLint mColorUniform = 0;
	GLint mVertexAttrib = 0;
	GLint mMVPUniform = 0;
private:
	void CreateModel();
	void LoadAnimation();
	void InitOpenGL();
	void DrawSkeleton(const Animation::State& state, float* mvp, float* color);
public:
	void Initialize();
	void Update(float dt);
	void Render(float aspect);
	void Shutdown();
};

#endif