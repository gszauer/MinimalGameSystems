/*jshint esversion: 6 */

function Sample(gl, canvas) {
	this.mGl = gl;
	this.mCanvas = canvas;
	this.mIsRunning = false;
	this.mIsLoading =false;
	this.mDebugName = "Sample";
	this.mLastUpdateTime = performance.now();
	this.mSkipClear = false;
	this.mTimeMod = 1.0;
	this.mFar = null;
	this.mAspectRatio = 0;

	this.mCanGPUSkinUsingTextures = IsExtensionSupported(gl, EXTENSION_TYPE.FLOATTEX);

	let sample = this;
	let parts = window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi, function(m,key,value) {
		if (key == "index") {
			if (value == "short") { // For documentation only
			}
		}
		else if (key=="time") {
			sample.mTimeMod  = parseFloat(value);
		}
	});

	this.mNumFramesSinceLastResize = 0;
	this.mLastRequestedDisplayWidth = 0;
	this.mLastDesiredHeight = 0;
}

Sample.prototype.Initialize = function (gl) { }

Sample.prototype.InvokeInitialize = function () {
	if (!this.mIsRunning && !this.mIsLoading) {
		this.Initialize(this.mGl);
		this.mIsRunning = false;
		this.mIsLoading = true;
	}
}

Sample.prototype.Load = function (gl) { return true; }

Sample.prototype.InvokeLoad = function() {
	if (!this.mIsRunning && this.mIsLoading) {
		if (this.Load(this.mGl)) {
			this.mIsRunning = true;
			this.mIsLoading = false;
		}
	}
}

Sample.prototype.Update = function(gl, deltaTime) { }


Sample.prototype.InvokeUpdate = function(deltaTime) {
	if (this.mIsRunning) {
		this.Update(this.mGl, deltaTime * this.mTimeMod);
	}
}

Sample.prototype.Render = function(gl, aspectRatio) { }

Sample.prototype.InvokeRender = function(aspectRatio) {
	if (!this.mSkipClear) {
		gl.scissor(0, 0, this.mCanvas.width / 2, this.mCanvas.height);
		gl.viewport(0, 0, this.mCanvas.width / 2, this.mCanvas.height);
		this.mGl.clearColor(0.5, 0.6, 0.7, 1.0);
		this.mGl.clear(this.mGl.COLOR_BUFFER_BIT | this.mGl.DEPTH_BUFFER_BIT);
	}
	if (this.mIsRunning) {
		this.Render(this.mGl, aspectRatio);
	}
}

Sample.prototype.Loop = function() {
	let bounds = this.mCanvas.getBoundingClientRect();

	let vVis = bounds.bottom >= 0  && bounds.top <= (window.innerHeight || document.documentElement.clientHeight);
	let hVis = bounds.right >= 0 && bounds.left <= (window.innerWidth || document.documentElement.clientWidth);
	let vis = hVis && vVis;
	this.InvokeInitialize();
	
	if (!vis) {
		return;
	}

	let gl = this.mGl;

	/*let resize = false;
	let desiredCSSWidth = gl.canvas.clientWidth;
	let desiredCSSHeight = gl.canvas.clientHeight;

	if (desiredCSSWidth != this.mLastDesiredWidth || desiredCSSHeight != this.mLastDesiredHeight) {
		resize = true;
	}
	if (this.mLastCanvasStyleWidth != Math.floor(desiredCSSWidth  * devicePixelRatio)) {
		resize = true;
	}
	if (this.mLastCanvasStyleHeight != Math.floor(desiredCSSHeight * devicePixelRatio)) {
		resize = true
	}

	if (resize) {
		let devicePixelRatio = window.devicePixelRatio || 1;

		gl.canvas.width  = Math.floor(desiredCSSWidth  * devicePixelRatio);
		gl.canvas.height = Math.floor(desiredCSSHeight * devicePixelRatio);

		gl.canvas.style.width  = desiredCSSWidth  + "px";
		gl.canvas.style.height = desiredCSSHeight + "px";

		this.mLastCanvasStyleWidth = gl.canvas.width;
		this.mLastCanvasStyleHeight = gl.canvas.height;

		gl.scissor(0, 0, gl.canvas.width, gl.canvas.height);
		gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

		console.log("Resized: " + this.mDebugName);
	}
	this.mLastDesiredWidth = desiredCSSWidth;
	this.mLastDesiredHeight = desiredCSSHeight;*/

	var realToCSSPixels = window.devicePixelRatio;
	var displayWidth  = Math.floor(this.mCanvas.clientWidth  * realToCSSPixels);
	var displayHeight = Math.floor(this.mCanvas.clientHeight * realToCSSPixels);

	if (this.mLastRequestedDisplayWidth !== displayWidth || this.mLastDesiredHeight !== displayHeight) {
		this.mNumFramesSinceLastResize += 1;
		if (this.mNumFramesSinceLastResize > 30) {
			this.mNumFramesSinceLastResize = 30;
		}
	}
	else {
		this.mNumFramesSinceLastResize -= 1;
		if (this.mNumFramesSinceLastResize < 0) {
			this.mNumFramesSinceLastResize = 0;
		}
	}

	this.mLastRequestedDisplayWidth = displayWidth;
	this.mLastDesiredHeight = displayHeight;

	if (this.mNumFramesSinceLastResize == 0 && (this.mCanvas.width  !== displayWidth || this.mCanvas.height !== displayHeight)) {
		gl.canvas.width  = displayWidth;
		gl.canvas.height = displayHeight;
		
		gl.scissor(0, 0, this.mCanvas.width / 2, this.mCanvas.height);
		gl.viewport(0, 0, this.mCanvas.width / 2, this.mCanvas.height);
	}

	let thisTime = performance.now();
	let lastTime = this.mLastUpdateTime;
	this.mLastUpdateTime = thisTime;

	let deltaTime = (thisTime - lastTime) * 0.001;
	//let aspectRatio = desiredCSSWidth / desiredCSSHeight;
	let aspectRatio = (displayWidth / 2) / displayHeight;

	if (deltaTime >= 0.0333) {
		deltaTime = 0.0333;
	}

	this.InvokeLoad();
	this.InvokeUpdate(deltaTime);
	this.InvokeRender(aspectRatio);
}

/* Full screen sample */

/*jshint esversion: 6 */
// Sample 02 shows that we can display a static mesh. 
//CPU skinning might be added later....
function FullPageAnimated(gl, canvas) {
	Sample.call(this, gl, canvas);

	this.mWomanMesh = null;
	this.mWomanTexture = null;

	this.mWomanRestPose = null;
	this.mWomanBindPose = null;
	this.mWomanSkeleton = null;

	this.mWalkingClip = null;
	this.mRunningClip = null;
	this.mBlendedPose = null;
	this.mWalkingAnimatedPose = null;
	this.mRunningAnimatedPose = null;
	this.mWalkingPlaybackTime = 0.0;
	this.mRunningPlaybackTime = 0.0;

	this.mPosePalette = null;
	this.mInvBindPalette = null;
	this.mCombinedPalette = null;

	this.mShader = null;
	
	this.mAttribPos = null;
	this.mAttribNorm = null;
	this.mAttribUV = null;
	this.mUniformModel = null;
	this.mUniformMVP = null;
	this.mUniformTex = null;
	
	this.mDebugName = "FullPageAnimated";
	this.mSkipClear = true;
}

FullPageAnimated.prototype = Object.create(Sample.prototype);
FullPageAnimated.prototype.constructor = FullPageAnimated;

FullPageAnimated.prototype.Initialize = function (gl) {
	this.mSkipClear = true;
	this.mShader = LoadShaderFromFile(gl, "webgl_vert.txt", "webgl_frag.txt");

	this.mDisplayTexture = LoadTextureFromFile(gl, "Woman.png");
	this.mWomanMesh = new Mesh(gl, "webgl_mesh.txt");
	this.mWomanSkeleton = new Skeleton();
	this.mWomanSkeleton.LoadFromFile("webgl_skeleton.txt");
	this.mWalkingClip = new Clip();
	this.mWalkingClip.LoadFromFile("webgl_walking.txt");
	this.mRunningClip = new Clip();
	this.mRunningClip.LoadFromFile("webgl_running.txt");
};

FullPageAnimated.prototype.Load = function(gl) {
	if (ShaderIsDoneLoading(gl, this.mShader) && TextureIsDoneLoading(gl, this.mDisplayTexture) && this.mWomanMesh.IsLoaded() && this.mWomanSkeleton.IsLoaded() && this.mWalkingClip.IsLoaded()) {
		this.mWalkingClip.RecalculateDuration();
		this.mRunningClip.RecalculateDuration();
		this.mWomanMesh.UpdateOpenGLBuffers();

		this.mWomanRestPose = new Pose();
		this.mWomanRestPose.Copy(this.mWomanSkeleton.mRestPose);
		this.mWomanBindPose = new Pose();
		this.mWomanBindPose.Copy(this.mWomanSkeleton.mBindPose);
		this.mWalkingAnimatedPose = new Pose();
		this.mWalkingAnimatedPose.Copy(this.mWomanSkeleton.mRestPose);
		this.mRunningAnimatedPose = new Pose();
		this.mRunningAnimatedPose.Copy(this.mWomanSkeleton.mRestPose);
		this.mBlendedPose = new Pose();
		this.mBlendedPose.Copy(this.mWomanSkeleton.mRestPose);
		
		this.mWalkingPlaybackTime = this.mWalkingClip.GetStartTime();
		this.mRunningPlaybackTime = this.mRunningClip.GetStartTime();
		this.mPosePalette = [];
		this.mCombinedPalette = [];
		this.mInvBindPalette = [];
		this.mWalkingAnimatedPose.GetMatrixPalette(this.mPosePalette);
		this.mInvBindPalette = this.mWomanSkeleton.GetInvBindPose();
		for (let i = 0; i < this.mPosePalette.length; ++i) {
			this.mCombinedPalette.push(m4_identity());
		}

		this.mAttribPos = this.mShader == null? null : ShaderGetAttribute(gl, this.mShader, "position");
		this.mAttribNorm = this.mShader == null? null : ShaderGetAttribute(gl, this.mShader, "normal");
		this.mAttribUV = this.mShader == null? null : ShaderGetAttribute(gl, this.mShader, "texCoord");

		this.mUniformMVP = this.mShader == null? null : ShaderGetUniform(gl, this.mShader, "mvp");
		this.mUniformModel = this.mShader == null? null : ShaderGetUniform(gl, this.mShader, "model");
		this.mUniformTex = this.mShader == null? null : ShaderGetUniform(gl, this.mShader, "tex0");

		if (this.mPosePalette.length != this.mInvBindPalette.length || this.mPosePalette.length != this.mCombinedPalette.length) {
			console.error("bad pose lengths");
		}
		
		this.mVertexPositions = MakeAttribute(gl);
		this.mVertexNormals = MakeAttribute(gl);
		this.mVertexTexCoords = MakeAttribute(gl);
		this.mIndexBuffer = MakeIndexBuffer(gl);

		return true;
	}
	return false;
};

FullPageAnimated.prototype.Update = function(gl, deltaTime) {
	this.mWalkingPlaybackTime = this.mWalkingClip.Sample(this.mWalkingAnimatedPose, this.mWalkingPlaybackTime + deltaTime);
	this.mRunningPlaybackTime = this.mRunningClip.Sample(this.mRunningAnimatedPose, this.mRunningPlaybackTime + deltaTime);

	this.mBlendedPose.Blend(this.mWalkingAnimatedPose, this.mRunningAnimatedPose, 0.75);

	this.mWomanMesh.CPUSkin(this.mWomanSkeleton, this.mBlendedPose);
};

FullPageAnimated.prototype.Render = function(gl, aspectRatio) {
	this.mAspectRatio = aspectRatio;

	gl.clearColor(0.5, 0.6, 0.7, 1.0);
	gl.scissor(0, 0, this.mCanvas.width / 2, this.mCanvas.height);
	gl.viewport(0, 0, this.mCanvas.width / 2, this.mCanvas.height);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	let projection = m4_perspective(60.0, aspectRatio, 0.01, 100.0);
	let view = m4_lookAt([0, 5, 7], [0, 3, 0], [0, 1, 0]);
	let model = m4_identity();
	let mvp = m4_mul(m4_mul(projection, view), model);

	ShaderBind(gl, this.mShader);

	UniformMat4(gl, this.mUniformModel, model);
	UniformMat4(gl, this.mUniformMVP, mvp);
	TextureBind(gl, this.mDisplayTexture, this.mUniformTex, 0);

	this.mWomanMesh.Bind(this.mAttribPos, this.mAttribNorm, this.mAttribUV, -1, -1);
	this.mWomanMesh.Draw();
	this.mWomanMesh.UnBind(this.mAttribPos, this.mAttribNorm, this.mAttribUV, -1, -1);

	TextureUnbind(gl, 0);

	ShaderUnbind(gl);
};