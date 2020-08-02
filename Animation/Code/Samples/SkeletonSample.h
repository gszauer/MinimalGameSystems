#ifndef _H_SKELETON_SAMPLE_
#define _H_SKELETON_SAMPLE_

#include "ISample.h"

class SkeletonSample : public ISample {
protected:
	std::vector<vec3> m_Vertices;
	std::vector<vec3> m_Skinned;
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
	void DrawSkeleton(const Animation::State& state, const mat4& mvp, const vec3& color);
public:
	void Initialize();
	void Update(float dt);
	void Render(float aspect);
	void Shutdown();
};

#endif