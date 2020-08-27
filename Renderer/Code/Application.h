#ifndef _H_APPLICATION_
#define _H_APPLICATION_

#include "Renderer.h"

// TODO: Sample that does a clear
// TODO: Sample that has four triangles
//    A basic one with positions and colors
//    An indexed one to show indexing works
//    Both of the previous but interleaved

class Application {
	struct BasicTriangle {
		Renderer::IVertexBuffer* position;
		Renderer::IVertexBuffer* color;
	};
	struct IndexedTriangle {
		Renderer::IVertexBuffer* position;
		Renderer::IVertexBuffer* color;
		Renderer::IIndexBuffer* indices;
	};
	struct InterleavedIndexedTriangle {
		Renderer::IVertexBuffer* data;
		Renderer::IIndexBuffer* indices;
	};
private:
	BasicTriangle mBasic;
	IndexedTriangle mIndexed;
	Renderer::IVertexBuffer* mInterleaved;
	InterleavedIndexedTriangle mInterleavedIndexed;
	Renderer::IGraphicsDevice* mDevice;
	Renderer::IShader* mShader;
private:
	Application(const Application&);
	Application& operator=(const Application&);
public:
	Application();
	~Application();

	void Initialize(Renderer::IGraphicsDevice* device);
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif
