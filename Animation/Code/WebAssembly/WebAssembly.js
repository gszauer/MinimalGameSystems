var gDownloadQueue = { };

var gProgram = null;
var gImports = {};
var gMemory = null;
var gFMemory = null;
var gExports = null;

const gUtf8Decoder = new TextDecoder("utf-8");
const gUtf8Encoder = new TextEncoder("utf-8");

var gGl = null;
var gApplication = null;
var gCMeshView = null;

var js_num_samples = 60;
var js_update_times = [];
var js_update_index = 0;

var c_num_samples = 60;
var c_update_times = [];
var c_update_index = 0;

var dom_js_label = null;
var dom_js_node = null;

var dom_c_label = null;
var dom_c_node = null;

function StartToLoadFile(address) {
	if (gDownloadQueue[address] === undefined) {
		gDownloadQueue[address] = window.XMLHttpRequest ? new XMLHttpRequest() : new ActiveXObject("Microsoft.XMLHTTP");
		gDownloadQueue[address].IsFileDownloaded = false;
		gDownloadQueue[address].onreadystatechange = function() { 
			if (this.readyState == 4) {
				gDownloadQueue[address].IsFileDownloaded = true;
			} 
		};
		gDownloadQueue[address].open("GET", address);
		gDownloadQueue[address].send();
	}
}

function IsFileLoaded(address) {
	if (gDownloadQueue[address] !== undefined) {
		return gDownloadQueue[address].IsFileDownloaded? 1 : 0;
	}
	return 0;
}

function FileSize(address) {
	if (gDownloadQueue[address] !== undefined) {
		if (gDownloadQueue[address].responseText !== undefined) {
			return gDownloadQueue[address].responseText.length;
		}
	}
	return 0;
}

function FileContents(address) {
	if (gDownloadQueue[address] !== undefined) {
		if (gDownloadQueue[address].responseText !== undefined) {
			return gDownloadQueue[address].responseText;
		}
	}
	return "";
}

function JS_StartToLoadFile(address, strLen) {
	let file = gUtf8Decoder.decode(gMemory.subarray(address, address+strLen));
	StartToLoadFile(file);
}

function JS_IsFileLoaded(address, strLen) {
	let file = gUtf8Decoder.decode(gMemory.subarray(address, address+strLen));
	return IsFileLoaded(file);
}

function JS_FileSize(address, strLen) {
	let file = gUtf8Decoder.decode(gMemory.subarray(address, address+strLen));
	return FileSize(file);
}

function JS_FileCopyContents(address, addressLen, target) {
	let file = gUtf8Decoder.decode(gMemory.subarray(address, address+addressLen));
	let content = FileContents(file);

	let copyObject = gMemory.subarray(target, target+content.length)
	copyObject.set(gUtf8Encoder.encode(content));
}

function JS_JavascriptLog(message, strLen) {
	let str = gMemory.subarray(message, message+strLen);
	console.log(gUtf8Decoder.decode(str));
}

function JS_LogInt(num) {
	console.log(num);
}

async function Initialize() {
	// Load WASM
	let request = await fetch('animation.wasm');
	let binary = await request.arrayBuffer();
	gImports.memory = new WebAssembly.Memory( 
		{'initial':32 * 4}  // 8 MB 
	);
	gImports['JS_StartToLoadFile'] = JS_StartToLoadFile;
	gImports['JS_IsFileLoaded'] = JS_IsFileLoaded;
	gImports['JS_FileSize'] = JS_FileSize;
	gImports['JS_FileCopyContents'] = JS_FileCopyContents;
	gImports['JS_JavascriptLog'] = JS_JavascriptLog;
	gImports['JS_LogInt'] = JS_LogInt;
	gMemory = new Uint8Array(gImports.memory.buffer);
	gProgram = await WebAssembly.instantiate( binary, { "env":gImports } );
	gExports = gProgram.instance.exports;
	
	dom_js_label = document.querySelector("#jsLabel");
	dom_js_node = document.createTextNode("");
	dom_js_node.nodeValue = "Waiting for samples";
	dom_js_label.innerHtml = "";
	dom_js_label.appendChild(dom_js_node);

	dom_c_label = document.querySelector("#cLabel");
	dom_c_node = document.createTextNode("");
	dom_c_node.nodeValue = "Waiting for samples";
	dom_c_label.innerHtml = "";
	dom_c_label.appendChild(dom_c_node);

	// Do the same thing, but in C!
	gExports.Initialize();

	let canvas = document.querySelector("#glCanvas");
	gGl = canvas.getContext("webgl");
	gGl.enable(gGl.DEPTH_TEST);
	gGl.enable(gGl.CULL_FACE);
	gGl.enable(gGl.CULL_FACE);
	gGl.enable(gGl.SCISSOR_TEST);
	if (gGl === null) {
		alert("Unable to initialize WebGL. Your browser or machine may not support it.");
		return;
	}
	gApplication = new FullPageAnimated(gGl, canvas);

	for (let i = 0; i < js_num_samples; ++i) {
		js_update_times.push(0.0);
	}

	for (let i = 0; i < c_num_samples; ++i) {
		c_update_times.push(0.0);
	}

	// Update Window
	window.setInterval(Loop, 16);
}

function Loop() {
	gApplication.Loop();
	gApplication.InvokeLoad();
	const t0 = performance.now();
	gApplication.InvokeUpdate(gApplication.mDeltaTime);
	const t1 = performance.now();
	gApplication.InvokeRender(gApplication.mAspectRatio);

	js_update_times[js_update_index++] = t1 - t0;
	if (js_update_index == js_num_samples) {
		let avg = 0;
		for (let i = 0; i < js_num_samples; ++i) {
			avg += js_update_times[i];
		}
		avg /= js_num_samples;
		dom_js_node.nodeValue = "Js update: " + avg.toFixed(3) + "ms";
		js_update_index = 0;
	}

	const t2 = performance.now();
	gExports.Update(1.0 / 60.0); // TODO: actual delta time
	const t3 = performance.now();

	c_update_times[c_update_index++] = t3 - t2;
	if (c_update_index == c_num_samples) {
		let avg = 0;
		for (let i = 0; i < c_num_samples; ++i) {
			avg += c_update_times[i];
		}
		avg /= c_num_samples;
		dom_c_node.nodeValue = "C update: " + avg.toFixed(3) + "ms";
		c_update_index = 0;
	}

	if (gExports.IsRunning() && gApplication.mIsRunning) {
		gGl.clearColor(0.5, 0.7, 0.6, 1.0); // gl.clearColor(0.7, 0.5, 0.6, 1.0);
		gGl.scissor(gApplication.mCanvas.width / 2, 0, gApplication.mCanvas.width / 2, gApplication.mCanvas.height);
		gGl.viewport(gApplication.mCanvas.width / 2, 0, gApplication.mCanvas.width / 2, gApplication.mCanvas.height);
		gGl.clear(gGl.COLOR_BUFFER_BIT | gGl.DEPTH_BUFFER_BIT);

		let projection = m4_perspective(60.0, gApplication.mAspectRatio, 0.01, 100.0);
		let view = m4_lookAt([0, 5, 7], [0, 3, 0], [0, 1, 0]);
		let model = m4_identity();
		let mvp = m4_mul(m4_mul(projection, view), model);

		ShaderBind(gGl, gApplication.mShader);

		UniformMat4(gGl, gApplication.mUniformModel, model);
		UniformMat4(gGl, gApplication.mUniformMVP, mvp);
		TextureBind(gGl, gApplication.mDisplayTexture, gApplication.mUniformTex, 0);

		let numVerts = gApplication.mWomanMesh.mPosition.length;
		numVerts = numVerts / 3;

		if (gFMemory === null) {
			gFMemory = Float32Array.from(gMemory.subarray(gExports.GetSkinnedVertexPointer()));
		}

		for (let i = 0; i < numVerts; ++i) {
			gApplication.mWomanMesh.mPosition[i * 3 + 0] = gFMemory[i * 6 + 0];
			gApplication.mWomanMesh.mPosition[i * 3 + 1] = gFMemory[i * 6 + 1];
			gApplication.mWomanMesh.mPosition[i * 3 + 2] = gFMemory[i * 6 + 2];

			//gApplication.mWomanMesh.mNormal[i * 3 + 0] = gFMemory[i * 6 + 3];
			//gApplication.mWomanMesh.mNormal[i * 3 + 1] = gFMemory[i * 6 + 4];
			//gApplication.mWomanMesh.mNormal[i * 3 + 2] = gFMemory[i * 6 + 5];
		}
		gApplication.mWomanMesh.UpdateOpenGLDisplayBuffersOnly();

		gApplication.mWomanMesh.Bind(gApplication.mAttribPos, gApplication.mAttribNorm, gApplication.mAttribUV, -1, -1);
		gApplication.mWomanMesh.Draw();
		gApplication.mWomanMesh.UnBind(gApplication.mAttribPos, gApplication.mAttribNorm, gApplication.mAttribUV, -1, -1);

		TextureUnbind(gGl, 0);

		ShaderUnbind(gGl);
	}
}



window.onload = Initialize();

window.onresize = function() {
	document.body.height = window.innerHeight;
}