#include "../Header/Application.h"

Application::Application() {
}

Application::~Application() {

}

void Application::Initialize(Renderer::IGraphicsDevice* device) {
	mDevice = device;

	mInterleaved = mDevice->MakeVertexBuffer();

	mBasic.position = mDevice->MakeVertexBuffer();
	mBasic.color = mDevice->MakeVertexBuffer();

	mIndexed.position = mDevice->MakeVertexBuffer();
	mIndexed.color = mDevice->MakeVertexBuffer();
	mIndexed.indices = mDevice->MakeIndexBuffer();

	mInterleavedIndexed.data = mDevice->MakeVertexBuffer();
	mInterleavedIndexed.indices = mDevice->MakeIndexBuffer();

	float position[9] = {
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	mBasic.position->SetFloat(3, position, 9);

	mShader = mDevice->MakeShader();
	// TODO: Shader source
}

void Application::Update(float inDeltaTime) {
	
}

void Application::Render(float inAspectRatio) {
	mDevice->ApplyRasterState(mDevice->GetDefaultRasterState());
	mDevice->Clear(Renderer::Clear::ColorDepth);

	mDevice->BindShader(mShader);
	mDevice->Draw(Renderer::DrawMode::Triangles, 0, 3);
	mDevice->UnbindShader();
}

void Application::Shutdown() {
	mDevice->DestroyVertexBuffer(mInterleaved);
	mDevice->DestroyVertexBuffer(mBasic.position);
	mDevice->DestroyVertexBuffer(mBasic.color);
	mDevice->DestroyVertexBuffer(mIndexed.position);
	mDevice->DestroyVertexBuffer(mIndexed.color);
	mDevice->DestroyVertexBuffer(mInterleavedIndexed.data);
	mDevice->DestroyIndexBuffer(mIndexed.indices);
	mDevice->DestroyIndexBuffer(mInterleavedIndexed.indices);
	mDevice->DestroyShader(mShader);
}