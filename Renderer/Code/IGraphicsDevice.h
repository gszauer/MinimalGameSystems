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

	enum class NDCComponent {
		Near = 0,
		Far = 1,
		Left = 2,
		Right = 3,
		Top = 4,
		Bottom = 5
	};

	class IRasterState;
	class IFrameBuffer;
	class ITexture;
	class IIndexBufferData;
	class IVertexBufferData;
	class IBufferView;
	class IShader;
	class IGPUTimer;
	class ITextureSampler;
	class IShaderUniform;
	class IShaderAttribute;

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

		virtual IIndexBufferData* CreateIndexBuffer() const = 0;
		virtual void DestroyIndexBuffer(const IIndexBufferData* buffer) const = 0;

		virtual IVertexBufferData* CreateVertexBuffer() const = 0;
		virtual void DestroyVertexBuffer(const IVertexBufferData* buffer) const = 0;

		virtual ITexture* CreateTexture() const = 0;
		virtual void DestroyTexture(const ITexture* texture) const = 0;

		virtual IShader* CreateShader(const char* vertex, const char* fragment) const = 0;
		virtual void DestroyShader(const IShader* shader) const = 0;

		virtual IRasterState* CreateRasterState() const = 0;
		virtual void DestroyRasterState(const IRasterState* state) const = 0;

		// Bind / unbind resources
		virtual void SetFrameBuffer(const IFrameBuffer* buffer) = 0; // Calling with 0 will bind to default FBO
		virtual void SetShader(const IShader* shader) = 0; // Calling with 0 will unbind shader
		virtual void SetAttribute(const IShaderAttribute* attrib, const IBufferView* buffer); // Call with 0 to unbind
		void SetTexture(const IShaderUniform* uniform, const ITextureSampler* sampler); // No need to unbind
		void SetUniform(const IShaderUniform* uniform, void* data, unsigned int count = 1);

		/*
		void Initialize() {
			IGraphicsDevice graphicsDevice = CreateWin32OpenGL33GraphicsDevice(.....
			
			ITexture* colorAttachment = graphicsDevice->CreateTexture();
			colorAttachment->Set(monitorWidth, monitorHeight, TextureType::RGB);
			
			ITexture* depthStencilAttachment =  graphicsDevice->CreateTexture();
			depthStencilAttachment->Set(monitorWidth, monitorHeight, TextureType::DepthStencil);

			IFrameBuffer* frameBuffer = graphicsDevice->CreateFrameBuffer(colorAttachment, depthStencilAttachment);
			IRasterState* rasterState = &graphicsDevice->GetDefaultRasterState();

			// TODO: This right here is where I fucked up. There should be no need to have seperate IndexBufferData and VertexBufferData classes
			// Instead, let's create a single calss, called BufferData. This can contain both vertex and index information.
			// This will introduce a Decent amount of overhead to the per platform device code, but keep the API clean. 
			// Below is the new API proposal, it does away with IVertexBufferData, IIndexBufferData, IVertexBufferView and IIndexBufferView
			// Replacing them with IBufferData, IComponentView, IIndexView

			IBufferData* mesh = graphicsDevice->CreateBuffer();
			LoadMeshFromFile("suzane.txt", mesh);
			IIndexView* indicesBuffer = mesh->CreateIndexView(36); // Defaule offset is 0, default type is uint16
			unsigned int offset = sizeof(uint16) * 36; // Vertex and normal data is interleaved after index offset
			IAttributeView* positionsBuffer = mesh->CreateAttributeView(3, VertexBufferDataType::Float, sizeof(float) * 8, offset);
			IAttributeView* normalsBuffer = mesh->CreateAttributeView(3, VertexBufferDataType::Float, sizeof(float) * 8, offset + sizeof(float) * 3);
			IAttributeView* texCoordBuffer = mesh->CreateAttributeView(2, VertexBufferDataType::Float, sizeof(float) * 8, offset + sizeof(float) * 6);

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

			IGPUTimer* startTimer = graphicsDevice->CreateTimer();
			IGPUTimer* endTimer = graphicsDevice->CreateTimer();

		}

		void Tick() {
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
			
			graphicsDevice->Draw(DeawMode::Triangles, indicesBuffer);

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

		// Modofy various states
		virtual void Clear(Clear clear) = 0;
		virtual void ApplyRasterState(const IRasterState* state) = 0;

		// Drawing geometry
		virtual void Draw(DrawMode mode, const IIndexBufferView* buffer) const = 0;
		virtual void Draw(DrawMode mode, unsigned int first, unsigned int numVerts) const = 0;
		virtual void Draw(DrawMode mode, const IIndexBufferView* buffer, unsigned int first, unsigned int numVerts) const = 0;

		virtual void DrawInstanced(DrawMode mode, const IIndexBufferView* buffer, unsigned int instanceCount) const = 0;
		virtual void DrawInstanced(DrawMode mode, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const = 0;
		virtual void DrawInstanced(DrawMode mode, const IIndexBufferView* buffer, unsigned int first, unsigned int numVerts, unsigned int instanceCount) const = 0;

		// Viewport
		virtual const float* GetViewportDimensions() const = 0;
		virtual float GetViewportX() const = 0;
		virtual float GetViewportY() const = 0;
		virtual float GetViewportWidth() const = 0;
		virtual float GetViewportHeight() const = 0;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;

		// NDC Info
		virtual const float* GetNDCDimensions() const = 0;
		virtual float GetNDCComponent(NDCComponent component) const = 0;

		// Vsynch
		virtual void SetVSynch(bool val) = 0;
		virtual bool GetVSynch() const = 0;

		// Debug Info
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual float GetCPURenderTime() const = 0; // Milliseconds
		virtual float GetGPURenderTime() const = 0; // Milliseconds
		virtual unsigned int GetNumDrawCalls() const = 0;
		virtual unsigned int GetNumInstancedDrawCalls() const = 0;
		virtual unsigned int GetNumUnInstancedDrawCalls() const = 0;

		virtual unsigned int GetFrameBufferCount() const = 0;
		virtual unsigned int GetBufferDataCount() const = 0;
		virtual unsigned int GetIndexViewCount() const = 0;
		virtual unsigned int GetVertexViewCount() const = 0;
		virtual unsigned int GetTextureCount() const = 0;
		virtual unsigned int GetSamplerCuont() const = 0;
		virtual unsigned int GetShaderCount() const = 0;
		virtual unsigned int GetRasterStateCount() const = 0;

		virtual unsigned int GetRasterSwitchCount() const = 0;
		virtual unsigned int GetFrameBufferSwitchCount() const = 0;
		virtual unsigned int GetShaderSwitchCount() const = 0;
	};
}

#endif