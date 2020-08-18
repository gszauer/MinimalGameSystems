/*jshint esversion: 6 */

const INDEXBUFFER_TYPE = {
	INT16: 0,
	INT32: 1,
};

const EXTENSION_TYPE = {
	UINTINDEX: 0,
	FLOATTEX: 1
};

function IsExtensionSupported(gl, ext) {
	let force16 = false;

	let parts = window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi, function(m,key,value) {
		if (key == "index") {
			if (value == "short") {
				force16 = true;
			}
		}
	});

	if (ext == EXTENSION_TYPE.UINTINDEX) {
		if (!force16) {
			let ext = gl.getExtension('OES_element_index_uint');
			return ext !== null;
		}
		else {
			return false;
		}
	}
	if (ext == EXTENSION_TYPE.FLOATTEX) {
		var ext = gl.getExtension('OES_texture_float');
		return ext !== null;
	}

	return false;
}

// Shader
function _CompileShader(gl, source, type) {
	let shader = gl.createShader(type);
	gl.shaderSource(shader, source);
	gl.compileShader(shader);
	if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
		console.error('An error occurred compiling the shaders: ' + gl.getShaderInfoLog(shader));
		gl.deleteShader(shader);
		return null;
	}

	return shader;
}

function _LinkShader(gl, vertexShader, fragmentShader) { 
	let shaderProgram = gl.createProgram();
	gl.attachShader(shaderProgram, vertexShader);
	gl.attachShader(shaderProgram, fragmentShader);
	gl.linkProgram(shaderProgram);

	if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
		gl.deleteProgram(shaderProgram);

		console.error('Unable to initialize the shader program: ' + gl.getProgramInfoLog(shaderProgram));
		return null;
	}

	return shaderProgram;
}

function LoadShaderFromFile(gl, vertFile, fragFile) { 
	let shaderObject = { };
	shaderObject.mIsLoaded = false;
	shaderObject.v_handle = null;
	shaderObject.f_handle = null;
	shaderObject.handle = null;

	let vertXhttp = new XMLHttpRequest();
	vertXhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			shaderObject.v_handle = _CompileShader(gl, this.responseText, gl.VERTEX_SHADER);
			if (shaderObject.f_handle != null && shaderObject.handle == null) {
				shaderObject.handle = _LinkShader(gl, shaderObject.v_handle, shaderObject.f_handle);
				gl.deleteShader(shaderObject.v_handle); shaderObject.v_handle = null;
				gl.deleteShader(shaderObject.f_handle); shaderObject.f_handle = null;
				shaderObject.mIsLoaded = true;
			}
		}
	};
	vertXhttp.open("GET", vertFile, true);
	vertXhttp.send();

	let fragXhttp = new XMLHttpRequest();
	fragXhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			shaderObject.f_handle = _CompileShader(gl, this.responseText, gl.FRAGMENT_SHADER);
			if (shaderObject.v_handle != null && shaderObject.handle == null) {
				shaderObject.handle = _LinkShader(gl, shaderObject.v_handle, shaderObject.f_handle);
				gl.deleteShader(shaderObject.v_handle); shaderObject.v_handle = null;
				gl.deleteShader(shaderObject.f_handle); shaderObject.f_handle = null;
				shaderObject.mIsLoaded = true;
			}
		}
	};
	fragXhttp.open("GET", fragFile, true);
	fragXhttp.send();

	return shaderObject;
}

function LoadShaderFromString(gl, vertSource, fragSource) { 
	let shaderObject = { };
	shaderObject.mIsLoaded = true;
	shaderObject.v_handle = _CompileShader(gl, vertSource, gl.VERTEX_SHADER);
	shaderObject.f_handle = _CompileShader(gl, fragSource, gl.FRAGMENT_SHADER);
	shaderObject.handle = _LinkShader(gl, shaderObject.v_handle, shaderObject.f_handle);
	
	gl.deleteShader(shaderObject.v_handle); shaderObject.v_handle = null;
	gl.deleteShader(shaderObject.f_handle); shaderObject.f_handle = null;
	
	if (shaderObject.handle == null) {
		shaderObject.mIsLoaded = false;
	}

	return shaderObject;
}

function ShaderIsDoneLoading(gl, shaderHandle) { 
	if (shaderHandle == null) {
		return true;
	}
	return shaderHandle.mIsLoaded;
}

function ShaderBind(gl, shaderHandle) { 
	gl.useProgram(shaderHandle.handle);
}

function ShaderGetAttribute(gl, shaderHandle, attribName) { 
	return gl.getAttribLocation(shaderHandle.handle, attribName);
}

function ShaderGetUniform(gl, shaderHandle, uniformName) { 
	return gl.getUniformLocation(shaderHandle.handle, uniformName);
}

function ShaderUnbind(gl) { 
	gl.useProgram(null);
}

function DestroyShader(gl, shaderHandle) { 
	gl.deleteProgram(shaderHandle.handle);
	shaderHandle.handle = null;
}

const ATTRIB_TYPE = {
	UNKNOWN: 0,
	INT: 1,
	FLOAT: 2,
	VEC2: 3,
	VEC3: 4,
	VEC4: 5,
	IVEC4: 6
};

// Attributes
function MakeAttribute(gl) { 
	let result = {};
	result.handle = gl.createBuffer();
	result.type = ATTRIB_TYPE.UNKNOWN;
	return result;
}

function _SetAttribPointer(gl, attribHandle, attribSlot) {
	if (attribHandle.type == ATTRIB_TYPE.UNKNOWN) {
		console.error("Binding an empty attribute");
	}
	else if (attribHandle.type == ATTRIB_TYPE.FLOAT) {
		gl.vertexAttribPointer(attribSlot, 1, gl.FLOAT, false, 0, 0);
	}
	else if (attribHandle.type == ATTRIB_TYPE.VEC2) {
		gl.vertexAttribPointer(attribSlot, 2, gl.FLOAT, false, 0, 0);
	}
	else if (attribHandle.type == ATTRIB_TYPE.VEC3) {
		gl.vertexAttribPointer(attribSlot, 3, gl.FLOAT, false, 0, 0);
	}
	else if (attribHandle.type == ATTRIB_TYPE.VEC4) {
		gl.vertexAttribPointer(attribSlot, 4, gl.FLOAT, false, 0, 0);
	}
	// WebGL1 does not support integer attributes. Instead, these must be floating point
	// To be honest, this kind of sucks. Even if we where to pass gl.SHORT to the 
	// vertexAttribPointer function, the shader data would still be floating point!
	else if (attribHandle.type == ATTRIB_TYPE.INT) {
		gl.vertexAttribPointer(attribSlot, 1, gl.FLOAT, false, 0, 0);
	}
	else if (attribHandle.type == ATTRIB_TYPE.IVEC4) {
		gl.vertexAttribPointer(attribSlot, 4, gl.FLOAT, false, 0, 0);
	}
}

function AttributeBind(gl, attribHandle, attribSlot) { 
	gl.bindBuffer(gl.ARRAY_BUFFER, attribHandle.handle);
	gl.enableVertexAttribArray(attribSlot);
	_SetAttribPointer(gl, attribHandle, attribSlot);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
}

// Expecting an ArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer
// Or a SharedArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
function AttributeInt(gl, attribHandle, arr) { 
	attribHandle.type = ATTRIB_TYPE.INT;

	gl.bindBuffer(gl.ARRAY_BUFFER, attribHandle.handle);
	gl.bufferData(gl.ARRAY_BUFFER, arr, gl.STREAM_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
}

// Expecting an ArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer
// Or a SharedArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
function AttributeFloat(gl, attribHandle, arr) { 
	attribHandle.type = ATTRIB_TYPE.FLOAT;

	if (arr.constructor !== Float32Array) {
		console.error("Bad data type");
	}

	gl.bindBuffer(gl.ARRAY_BUFFER, attribHandle.handle);
	gl.bufferData(gl.ARRAY_BUFFER, arr, gl.STREAM_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
}

// Expecting an ArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer
// Or a SharedArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
function AttributeVec2(gl, attribHandle, arr) { 
	attribHandle.type = ATTRIB_TYPE.VEC2;

	if (arr.constructor !== Float32Array) {
		console.error("Bad data type");
	}

	gl.bindBuffer(gl.ARRAY_BUFFER, attribHandle.handle);
	gl.bufferData(gl.ARRAY_BUFFER, arr, gl.STREAM_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
}

// Expecting an ArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer
// Or a SharedArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
function AttributeVec3(gl, attribHandle, arr) { 
	attribHandle.type = ATTRIB_TYPE.VEC3;

	if (arr.constructor !== Float32Array) {
		console.error("Bad data type");
	}

	gl.bindBuffer(gl.ARRAY_BUFFER, attribHandle.handle);
	gl.bufferData(gl.ARRAY_BUFFER, arr, gl.STREAM_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
}

// Expecting an ArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer
// Or a SharedArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
function AttributeVec4(gl, attribHandle, arr) { 
	attribHandle.type = ATTRIB_TYPE.VEC4;

	if (arr.constructor !== Float32Array) {
		console.error("Bad data type");
	}

	gl.bindBuffer(gl.ARRAY_BUFFER, attribHandle.handle);
	gl.bufferData(gl.ARRAY_BUFFER, arr, gl.STREAM_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
}

// Expecting an ArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer
// Or a SharedArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
function AttributeIVec4(gl, attribHandle, arr) { 
	attribHandle.type = ATTRIB_TYPE.IVEC4;

	if (arr.constructor !== Float32Array) {
		console.error("Bad data type");
	}

	gl.bindBuffer(gl.ARRAY_BUFFER, attribHandle.handle);
	gl.bufferData(gl.ARRAY_BUFFER, arr, gl.STREAM_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
}

function AttributeUnbind(gl, attribHandle, attribSlot) { 
	gl.bindBuffer(gl.ARRAY_BUFFER, attribHandle.handle);
	gl.disableVertexAttribArray(attribSlot);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
}

function DestroyAttribute(gl, attribHandle) { 
	gl.deleteBuffer(attribHandle.handle);
	attribHandle.handle = null;
}

// Index buffers
function MakeIndexBuffer(gl) {
	let result = {};
	result.handle = gl.createBuffer();
	result.numIndices = 0;
	let ext = IsExtensionSupported(gl, EXTENSION_TYPE.UINTINDEX);
	result.dataType = ext ? gl.UNSIGNED_INT : gl.UNSIGNED_SHORT;
	result.componentType = ext ? INDEXBUFFER_TYPE.INT32 : INDEXBUFFER_TYPE.INT16;
	return result;
}

// Expecting an ArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer
// Or a SharedArrayBuffer: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
function IndexBufferData(gl, bufferHandle, arr) {
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, bufferHandle.handle);
	
	if (arr.constructor != Uint16Array && arr.constructor != Uint32Array) {
		console.error("Bad Data Type");
	}
	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, arr, gl.STREAM_DRAW);
	
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
	bufferHandle.numIndices = arr.length;
}

function DestroyIndexBuffer(gl, bufferHandle) {
	gl.deleteBuffer(bufferHandle.handle);
	bufferHandle.handle = null;
}

// Uniforms
function UniformInt(gl, uniformSlot, arg) { 
	gl.uniform1f(uniformSlot, arg);
}

function UniformFloat(gl, uniformSlot, arg) { 
	gl.uniform1f(uniformSlot, arg);
}

// Expects an Int32Array:  https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Int32Array
function UniformIntArray(gl, uniformSlot, arg) { 
	gl.uniform1fv(uniformSlot, arg);
}

// Expects a float array of 3 elements [0, 1, 2]
function UniformVec3(gl, uniformSlot, arg) { 
	gl.uniform3f(uniformSlot, arg[0], arg[1], arg[2]);
}

// Expects a Float32Array: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Float32Array
function UniformVec3Array(gl, uniformSlot, arg) { 
	gl.uniform3fv(uniformSlot, arg);
}

// Expects a Float32Array: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Float32Array
// Or an array of floating point values, 16 elements long
function UniformMat4(gl, uniformSlot, arg) { 
	gl.uniformMatrix4fv(uniformSlot, false, arg); 
}

// Textures
function LoadTextureFromFile(gl, texFile) {
	let result = {};
	let texture = gl.createTexture();
	result.handle = texture;
	result.mIsLoaded = false;
	gl.bindTexture(gl.TEXTURE_2D, texture);

	// Because images have to be download over the internet they might take a moment until they are ready.
	// Until then put a single pixel in the texture so we can use it immediately. 
	// When the image has finished downloading we'll update the texture with the contents of the image.
	const level = 0;
	const internalFormat = gl.RGBA;
	const width = 1;
	const height = 1;
	const border = 0;
	const srcFormat = gl.RGBA;
	const srcType = gl.UNSIGNED_BYTE;
	const pixel = new Uint8Array([0, 0, 255, 255]);
	gl.texImage2D(gl.TEXTURE_2D, level, internalFormat, width, height, border, srcFormat, srcType, pixel);

	let image = new Image();
	image.onload = function() {
		result.mIsLoaded = true;
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.texImage2D(gl.TEXTURE_2D, level, internalFormat, srcFormat, srcType, image);

		// WebGL1 has different requirements for power of 2 images
		// vs non power of 2 images so check if the image is a
		// power of 2 in both dimensions.
		//if (_IsPowerOf2(image.width) && _IsPowerOf2(image.height)) { // Yes, it's a power of 2. Generate mips.
			// ASSUME EVERYTHING IS A POWER OF 2
		gl.generateMipmap(gl.TEXTURE_2D);
		//} else { // No, it's not a power of 2. Turn off mips and set wrapping to clamp to edge
		//	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
		//	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
		//	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
		//}
	};
	image.src = texFile;

	return result;
}

function TextureIsDoneLoading(gl, texHandle) { 
	return texHandle.mIsLoaded;
}

function TextureBind(gl, texHandle, uniformIndex, textureIndex) { 
	gl.activeTexture(gl.TEXTURE0 + textureIndex);
	gl.bindTexture(gl.TEXTURE_2D, texHandle.handle);
	gl.uniform1i(uniformIndex, textureIndex);
}

function TextureUnbind(gl, textureIndex) { 
	gl.activeTexture(gl.TEXTURE0 + textureIndex);
	gl.bindTexture(gl.TEXTURE_2D, null);
	gl.activeTexture(gl.TEXTURE0);
}

function DestroyTexture(gl, texHandle) { 
	gl.deleteTexture(texHandle.handle);
}

// Drawing functions
const DRAW_MODE = {
	TRIANGLES: 0,
	LINES: 1,
	LINE_STRIP: 2,
	POINTS: 3
};

function _DrawModeToGL(gl, drawMode) {
	if (drawMode == DRAW_MODE.TRIANGLES) {
		return gl.TRIANGLES;
	}
	else if (drawMode == DRAW_MODE.LINES) {
		return gl.LINES;
	}
	else if (drawMode == DRAW_MODE.LINE_STRIP) {
		return gl.LINE_STRIP;
	}
	else if (drawMode == DRAW_MODE.POINTS) {
		return gl.POINTS;
	}
	console.error("Converting unknown draw mode");
	return gl.TRIANGLES;
}

function Draw(gl, drawMode, indexBufferOrVertexCount) {
	if (typeof indexBufferOrVertexCount==='number') {
		gl.drawArrays(_DrawModeToGL(gl, drawMode), 0, indexBufferOrVertexCount);
	}
	else {
		let numIndices = indexBufferOrVertexCount.numIndices;
		let dataType = indexBufferOrVertexCount.dataType;

		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBufferOrVertexCount.handle);
		gl.drawElements(_DrawModeToGL(gl, drawMode), numIndices, dataType, 0);
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
	}
}

function GetProgramInfo(gl, program) {
    var result = {
        attributes: [],
        uniforms: [],
        attributeCount: 0,
        uniformCount: 0
    },
        activeUniforms = gl.getProgramParameter(program, gl.ACTIVE_UNIFORMS),
        activeAttributes = gl.getProgramParameter(program, gl.ACTIVE_ATTRIBUTES);

    // Taken from the WebGl spec:
    // http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.14
    var enums = {
        0x8B50: 'FLOAT_VEC2',
        0x8B51: 'FLOAT_VEC3',
        0x8B52: 'FLOAT_VEC4',
        0x8B53: 'INT_VEC2',
        0x8B54: 'INT_VEC3',
        0x8B55: 'INT_VEC4',
        0x8B56: 'BOOL',
        0x8B57: 'BOOL_VEC2',
        0x8B58: 'BOOL_VEC3',
        0x8B59: 'BOOL_VEC4',
        0x8B5A: 'FLOAT_MAT2',
        0x8B5B: 'FLOAT_MAT3',
        0x8B5C: 'FLOAT_MAT4',
        0x8B5E: 'SAMPLER_2D',
        0x8B60: 'SAMPLER_CUBE',
        0x1400: 'BYTE',
        0x1401: 'UNSIGNED_BYTE',
        0x1402: 'SHORT',
        0x1403: 'UNSIGNED_SHORT',
        0x1404: 'INT',
        0x1405: 'UNSIGNED_INT',
        0x1406: 'FLOAT'
    };

    // Loop through active uniforms
    for (var i=0; i < activeUniforms; i++) {
        var uniform = gl.getActiveUniform(program, i);
        uniform.typeName = enums[uniform.type];
        result.uniforms.push(uniform);
        result.uniformCount += uniform.size;
    }

    // Loop through active attributes
    for (var i=0; i < activeAttributes; i++) {
        var attribute = gl.getActiveAttrib(program, i);
        attribute.typeName = enums[attribute.type];
        result.attributes.push(attribute);
        result.attributeCount += attribute.size;
    }

    return result;
}

/* Mesh */

/*jshint esversion: 6 */

function Mesh(gl, filePath) {
	this.mPosition = null;
	this.mNormal = null;
	this.mTexCoord = null;
	this.mWeights = null;
	this.mInfluences = null;
	this.mIndices = null;

	this.mCPUPositionRef = null;
	this.mCPUNormRef = null;
	this.mIsLoaded = false;

	this.mPosAttrib = MakeAttribute(gl);
	this.mNormAttrib = MakeAttribute(gl);
	this.mUvAttrib = MakeAttribute(gl);
	this.mWeightAttrib = MakeAttribute(gl);
	this.mInfluenceAttrib = MakeAttribute(gl);
	this.mIndexBuffer = MakeIndexBuffer(gl);

	this.mPosePalette = [];
	this.gl = gl;

	let mesh = this;
	let vertXhttp = new XMLHttpRequest();
	vertXhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			let content = JSON.parse(this.responseText);
			mesh.mPosition = new Float32Array(content.positions);
			mesh.mNormal = new Float32Array(content.normals);
			mesh.mTexCoord = new Float32Array(content.uvs);
			mesh.mWeights = new Float32Array(content.weights);
			mesh.mInfluences = new Float32Array(content.influences);
			mesh.mIndices =  (mesh.mIndexBuffer.componentType == INDEXBUFFER_TYPE.INT16)?new Uint16Array(content.indices) : new Uint32Array(content.indices);
			mesh.mCPUPositionRef = new Float32Array(content.positions);
			mesh.mCPUNormRef = new Float32Array(content.normals);
			mesh.mIsLoaded = true;
		}
	};
	vertXhttp.open("GET", filePath, true);
	vertXhttp.send();
}

Mesh.prototype.CPUSkin = function(skeleton, pose) {
	let numVerts = this.mPosition.length;
	if (numVerts == 0 || this.mInfluences.length == 0 || this.mWeights.length == 0) { 
		console.error("Mesh has no positions to skin");
		return;
	}
	numVerts = numVerts / 3;
	
	pose.GetMatrixPalette(this.mPosePalette);
	let invPosePalette = skeleton.GetInvBindPose();
	let numBones = this.mPosePalette.length;
	for (let i = 0; i < numBones; ++i) {
		this.mPosePalette[i] = m4_mul(this.mPosePalette[i], invPosePalette[i]);
	}

	for (let i = 0; i < numVerts; i++) {
		let j = [this.mInfluences[i * 4 + 0], this.mInfluences[i * 4 + 1], this.mInfluences[i * 4 + 2], this.mInfluences[i * 4 + 3]];
		let w = [this.mWeights[i * 4 + 0], this.mWeights[i * 4 + 1], this.mWeights[i * 4 + 2], this.mWeights[i * 4 + 3]];

		if (this.mCPUPositionRef.length > 0) {
			let p = [this.mCPUPositionRef[i * 3 + 0], this.mCPUPositionRef[i * 3 + 1], this.mCPUPositionRef[i * 3 + 2]];
			p = v3_add(
				v3_add(
					v3_scale(m4_transformPoint(this.mPosePalette[j[0]], p), w[0]),
					v3_scale(m4_transformPoint(this.mPosePalette[j[1]], p), w[1])
				),
				v3_add(
					v3_scale(m4_transformPoint(this.mPosePalette[j[2]], p), w[2]),
					v3_scale(m4_transformPoint(this.mPosePalette[j[3]], p), w[3])
				)
			);

			/*if (isNaN(p[0]) || isNaN(p[1]) || isNaN(p[2])) {
				console.logerror("skinned nan");
			}
			if (p[0] == p[1] && p[1] == p[2] && p[2] == 0.0) {
				console.log("skinned 0");
			}*/

			this.mPosition[i * 3 + 0] = p[0];
			this.mPosition[i * 3 + 1] = p[1];
			this.mPosition[i * 3 + 2] = p[2];
		}
		else {
			console.error("can't skin");
		}
		if (this.mNormal.length > 0 && this.mCPUNormRef.length > 0) {
			let n = [this.mCPUNormRef[i * 3 + 0], this.mCPUNormRef[i * 3 + 1], this.mCPUNormRef[i * 3 + 2]];
			n = v3_add(
				v3_add(
					(w[0] < 0.00001)? [0, 0, 0] : v3_scale(m4_transformVector(this.mPosePalette[j[0]], n), w[0]),
					(w[1] < 0.00001)? [0, 0, 0] : v3_scale(m4_transformVector(this.mPosePalette[j[1]], n), w[1])
				),
				v3_add(
					(w[2] < 0.00001)? [0, 0, 0] : v3_scale(m4_transformVector(this.mPosePalette[j[2]], n), w[2]),
					(w[3] < 0.00001)? [0, 0, 0] : v3_scale(m4_transformVector(this.mPosePalette[j[3]], n), w[3])
				)
			);
			this.mNormal[i * 3 + 0] = n[0];
			this.mNormal[i * 3 + 1] = n[1];
			this.mNormal[i * 3 + 2] = n[2];
		}
		else {
			console.error("can't skin");
		}
	}
}

Mesh.prototype.GetPosition = function() {
	return this.mPosition;
};

Mesh.prototype.GetNormal = function() {
	return this.mNormal;
};

Mesh.prototype.GetTexCoord = function() {
	return this.mTexCoord;
};

Mesh.prototype.GetWeights = function() {
	return this.mWeights;
};

Mesh.prototype.GetInfluences = function() {
	return this.mInfluences;
};

Mesh.prototype.GetIndices = function() {
	return this.mIndices;
};

Mesh.prototype.UpdateOpenGLDisplayBuffersOnly = function() {
	if (this.mPosition.length > 0) {
		AttributeVec3(this.gl, this.mPosAttrib, this.mPosition);
	}
	if (this.mNormal.length > 0) {
		AttributeVec3(this.gl, this.mNormAttrib, this.mNormal);
	}
}

Mesh.prototype.UpdateOpenGLBuffers = function() {
	if (this.mPosition.length > 0) {
		AttributeVec3(this.gl, this.mPosAttrib, this.mPosition);
	}
	if (this.mNormal.length > 0) {
		AttributeVec3(this.gl, this.mNormAttrib, this.mNormal);
	}
	if (this.mTexCoord.length > 0) {
		AttributeVec2(this.gl, this.mUvAttrib, this.mTexCoord);
	}
	if (this.mWeights.length > 0) {
		AttributeVec4(this.gl, this.mWeightAttrib, this.mWeights);
	}
	if (this.mInfluences.length > 0) {
		AttributeIVec4(this.gl, this.mInfluenceAttrib, this.mInfluences);
	}
	if (this.mIndices.length > 0) {
		IndexBufferData(this.gl, this.mIndexBuffer, this.mIndices);
	}
};

Mesh.prototype.Bind = function(pos, norm, tex, weight, influence) {
	if (pos >= 0) {
		AttributeBind(this.gl, this.mPosAttrib, pos);
	}
	if (norm >= 0) {
		AttributeBind(this.gl, this.mNormAttrib, norm);
	}
	if (tex >= 0) {
		AttributeBind(this.gl, this.mUvAttrib, tex);
	}
	if (weight >= 0) {
		AttributeBind(this.gl, this.mWeightAttrib, weight);
	}
	if (influence >= 0) {
		AttributeBind(this.gl, this.mInfluenceAttrib, influence);
	}
};

Mesh.prototype.Draw = function() {
	if (this.mIndices.length > 0) {
		Draw(this.gl, DRAW_MODE.TRIANGLES, this.mIndexBuffer);
	}
	else {
		Draw(this.gl, DRAW_MODE.TRIANGLES, this.mPosition.length);
	}
};

Mesh.prototype.DrawLines = function() {
	if (this.mIndices.length > 0) {
		Draw(this.gl, DRAW_MODE.LINES, this.mIndexBuffer);
	}
	else {
		Draw(this.gl, DRAW_MODE.LINES, this.mPosition.length);
	}
};

Mesh.prototype.UnBind = function(pos, norm, tex, weight, influence) {
	if (pos >= 0) {
		AttributeUnbind(this.gl, this.mPosAttrib, pos);
	}
	if (norm >= 0) {
		AttributeUnbind(this.gl, this.mNormAttrib, norm);
	}
	if (tex >= 0) {
		AttributeUnbind(this.gl, this.mUvAttrib, tex);
	}
	if (weight >= 0) {
		AttributeUnbind(this.gl, this.mWeightAttrib, weight);
	}
	if (influence >= 0) {
		AttributeUnbind(this.gl, this.mInfluenceAttrib, influence);
	}
};

Mesh.prototype.IsLoaded = function() {
	return this.mIsLoaded;
}