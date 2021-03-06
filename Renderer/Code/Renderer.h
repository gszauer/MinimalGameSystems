#ifndef _H_RENDERER_
#define _H_RENDERER_

#include "IBufferData.h"
#include "IBufferView.h"
#include "IContext.h"
#include "IFrameBuffer.h"
#include "IRasterState.h"
#include "IShader.h"
#include "IShaderAttribute.h"
#include "IShaderUniform.h"
#include "ITexture.h"
#include "ITextureSampler.h"

namespace Renderer {
	class OGL33Context;

	// TODO: Get to a compileable state, then figute out this interface!

	OGL33Context* Win32CreateOpenGL33Context(); 
	void Win32DestroyOpenGL33Context(const OGL33Context*);

	void Win32InitOpenGL33();
	void Win32ShutdownOpenGL33();
}

#endif