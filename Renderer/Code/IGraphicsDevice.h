#ifndef _H_IGRAPHICSDEVICE_
#define _H_IGRAPHICSDEVICE_

namespace Renderer {
	enum class Clear {
		Color = (1 << 1),
		Depth = (1 << 2),
		Stencil = (1 << 3),
		ColorDepth = (1 << 1) | (1 << 2),
		DepthStencil = (1 << 2) | (1 << 3),
		All = (1 << 1) | (1 << 2) | (1 << 3)
	};

	enum class DrawMode {
		Points = 0,
		LineStrip = 1,
		LineLoop = 2,
		Lines = 3, 
		TriangleStrip = 4,
		TriangleFan = 5,
		Triangles = 6
	};

	class IBufferData;
	class IBufferView;
	class IFrameBuffer;
	class IRasterState;
	class IShader;
	class IShaderAttribute;
	class IShaderUniform;
	class ITexture;
	class ITextureSampler;

	class IGraphicsDevice { // TODO: Rename to context, not device
	protected:
		IGraphicsDevice(const IGraphicsDevice&); // Disabled
		virtual IGraphicsDevice& operator=(const IGraphicsDevice&); // Disabled
		inline IGraphicsDevice() { } // Private
	public:
		virtual inline ~IGraphicsDevice() { }

		// Get functions
		virtual const char* GetName() const = 0;
		virtual const IRasterState& GetDefaultRasterState() const = 0;

		// Create / Destroy Resources
		virtual IFrameBuffer* CreateFrameBuffer(ITexture* colorAttachment = 0, ITexture* depthAttachment = 0) const = 0;
		virtual void DestroyFrameBuffer(const IFrameBuffer* buffer) const = 0;

		virtual IBufferData* CreateBuffer() const = 0;
		virtual void DestroyBuffer(const IBufferData* buffer) const = 0;

		virtual ITexture* CreateTexture() const = 0;
		virtual void DestroyTexture(const ITexture* texture) const = 0;

		virtual IShader* CreateShader(const char* vertex, const char* fragment) const = 0;
		virtual void DestroyShader(const IShader* shader) const = 0;

		virtual IRasterState* CreateRasterState() const = 0;
		virtual void DestroyRasterState(const IRasterState* state) const = 0;

		// Bind / unbind resources
		virtual void SetFrameBuffer(const IFrameBuffer* buffer) = 0; // Calling with 0 will bind to default FBO
		virtual void SetShader(const IShader* shader) = 0; // Calling with 0 will unbind shader
		virtual void SetAttribute(const IShaderAttribute* attrib, const IBufferView* buffer) = 0; // Call with 0 to unbind
		virtual void SetTexture(const IShaderUniform* uniform, const ITextureSampler* sampler) = 0; // No need to unbind
		virtual void SetUniform(const IShaderUniform* uniform, void* data, unsigned int count = 1) = 0;

		// Modofy various states
		virtual void Clear(Clear clear) = 0;
		virtual void ApplyRasterState(const IRasterState* state) = 0;

		// Viewport
		virtual void GetViewport(unsigned int& x, unsigned int& y, unsigned int& w, unsigned int& h) const = 0;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
		
		// Drawing geometry
		virtual void Draw(DrawMode mode, const IBufferView* buffer, unsigned int instanceCount = 1) const = 0;
		virtual void Draw(DrawMode mode, const IBufferView* buffer, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const = 0;
		virtual void Draw(DrawMode mode, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const = 0;

		// NDC Info
		virtual void GetNDC(float& left, float& right, float& top, float& bottom, float& near, float& far) const = 0;

		// Vsynch
		virtual void SetVSynch(bool val) = 0;
		virtual bool GetVSynch() const = 0;
	};
}

// Usage
/*void Initialize() {
	IGraphicsDevice graphicsDevice = CreateWin32OpenGL33GraphicsDevice(.....

	ITexture* colorAttachment = graphicsDevice->CreateTexture();
	colorAttachment->Set(monitorWidth, monitorHeight, TextureType::RGB);

	ITexture* depthStencilAttachment =  graphicsDevice->CreateTexture();
	depthStencilAttachment->Set(monitorWidth, monitorHeight, TextureType::DepthStencil);

	IFrameBuffer* frameBuffer = graphicsDevice->CreateFrameBuffer(colorAttachment, depthStencilAttachment);
	IRasterState* rasterState = &graphicsDevice->GetDefaultRasterState();

	IBufferData* mesh = graphicsDevice->CreateBuffer();
	LoadMeshFromFile("suzane.txt", mesh);
	IBufferView* indexBuffer = mesh->CreateView(1, BufferDataType::UShort); // Defaule offset is 0
	unsigned int offset = sizeof(uint16) * 36; // Vertex and normal data is interleaved after index offset
	IBufferView* positionsBuffer = mesh->CreateView(3, VertexBufferDataType::Float, sizeof(float) * 8, offset);
	IBufferView* normalsBuffer = mesh->CreateView(3, VertexBufferDataType::Float, sizeof(float) * 8, offset + sizeof(float) * 3);
	IBufferView* texCoordBuffer = mesh->CreateView(2, VertexBufferDataType::Float, sizeof(float) * 8, offset + sizeof(float) * 6);

	ITexture* diffuseTexture = graphicsDevice->CreateTexture();
	LoadTextureFromFile("diffuse.png", diffuseTexture);
	ITextureSampler* diffuseSampler = diffuseTexture->CreateSampler();

	IShader* shader = graphicsDevice->CreateShader("vertex.vert", "fragment.frag");

	IShaderAttribute* positionAttrib = shader->GetAttribute("positions");
	IShaderAttribute* normalAttrib = shader->GetAttribute("normals");
	IShaderAttribute* texCoordAttrib = shader->GetAttribute("uv");
	IShaderUniform* diffuseUniform = shader->GetUniform("tex0");
	IShaderUniform* mvpUniform = shader->GetUniform("mvp");
	IShaderUniform* lightUniform = shader->GetUniform("light");
	IShaderUniform* skinUniform = shader->GetUniform("skin");
}*/

/*void Tick() {
	graphicsDevice->Clear(Clear::All);

	mat4 mvpMatrix; // Frame data
	vec3 lightDir; // Frame data
	std::vector<mat4> skin; // FrameData

	graphicsDevice->SetFrameBuffer(frameBuffer);
	graphicsDevice->SetViewport(0, 0, windowWith, windowHeight);

	graphicsDevice->SetShader(shader);
	graphicsDevice->ApplyRasterState(rasterState);

	graphicsDevice->SetAttribute(positionAttrib, positionsBuffer);
	graphicsDevice->SetAttribute(normalAttrib, normalsBuffer);
	graphicsDevice->SetAttribute(texCoordAttrib, texCoordBuffer);

	graphicsDevice->SetUniform(mvpUniform, mvpMatrix.v);
	graphicsDevice->SetUniform(lightUniform, lightDir.v);
	graphicsDevice->SetUniform(skinUniform, skin[0].v, skin.size());

	graphicsDevice->SetTexture(diffuseUniform, diffuseSampler);

	graphicsDevice->Draw(DeawMode::Triangles, indexBuffer);

	#if 1
		graphicsDevice->Unbind(UnbindTarget::Attribs | UnbindTarget::Textures | UnbindTarget::Shader | UnbindTarget::FrameBuffer);
	#else
		graphicsDevice->SetAttribute(positionAttrib, 0);
		graphicsDevice->SetAttribute(normalAttrib, 0);
		graphicsDevice->SetAttribute(texCoordAttrib, 0);
		graphicsDevice->SetTexture(diffuseUniform, 0);
		graphicsDevice->SetShader(0);
		graphicsDevice->SetFrameBuffer(0);
	#endif
}
*/

#endif