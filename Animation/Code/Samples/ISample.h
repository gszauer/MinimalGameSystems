#ifndef _H_ISAMPLE_
#define _H_ISAMPLE_

#include "../AnimationState.h"
#include "../AnimationData.h"
#include "../AnimationHelpers.h"
#include "../AnimationSkin.h"
#include "../AnimationBlend.h"
#include "../glad.h"
#include <vector>

class ISample {
public:
	virtual void Initialize() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render(float aspect) = 0;
	virtual void Shutdown() = 0;
};

void Perspective(float* out, float fov, float aspect, float znear, float zfar);
void LookAt(float* out, const float* position, const float* target, const float* up);
char* ReadFileContents(const char* file);

// TODO: Remove GLAD. Move common OpenGL definitions in here!

#endif