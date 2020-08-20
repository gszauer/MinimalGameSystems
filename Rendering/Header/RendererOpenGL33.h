#ifndef _H_RENDEREROPENGL33_
#define _H_RENDEREROPENGL33_

#include "RendererOpenGL33/OGL33FrameBuffer.h"
#include "RendererOpenGL33/OGL33GPUTimer.h"
#include "RendererOpenGL33/OGL33GraphicsDevice.h"
#include "RendererOpenGL33/OGL33IndexBuffer.h"
#include "RendererOpenGL33/OGL33RasterState.h"
#include "RendererOpenGL33/OGL33Sampler.h"
#include "RendererOpenGL33/OGL33Shader.h"
#include "RendererOpenGL33/OGL33Texture.h"
#include "RendererOpenGL33/OGL33VertexBuffer.h"

#include "glad.h"
#undef APIENTRY
#include <windows.h>

namespace Renderer {
	// TODO: Put all windows specific code in here! While the OGL classes can invoke glad, they should never invlude windows.h
	// This is the only file where Windows.h is allowed
	OGL33GraphicsDevice* CreateOpenGL33GraphicsDevice(HWND hwnd);
	void DestroyOpenGL33GraphicsDevice(OGL33GraphicsDevice* device);
}

#endif