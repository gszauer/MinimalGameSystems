#ifndef _H_CURVES_SAMPLE_
#define _H_CURVES_SAMPLE_

#include "ISample.h"

class CurvesSample : public ISample {
protected:
	Animation::Data mAnimationData;
protected:
	GLuint mCurveVAO = 0;
	GLuint mCurveVBO = 0;
	GLuint mCurveShader = 0;
protected:
	GLint mColorAttrib = 0;
	GLint mVertexAttrib = 0;
	GLint mMVPUniform = 0;
	unsigned int mNumVerts = 0;
public:
	void Initialize();
	void Update(float dt);
	void Render(float aspect);
	void Shutdown();
};

#endif