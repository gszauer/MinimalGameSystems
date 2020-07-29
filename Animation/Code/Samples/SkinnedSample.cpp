#include "SkinnedSample.h"
#include <iostream>

void SkinnedSample::LoadModel() {
	char* fileContent = ReadFileContents("Assets/womanMesh.txt");
	const char* input = fileContent;

	unsigned int posSize = 0;
	unsigned int normSize = 0;
	unsigned int texSize = 0;
	unsigned int weightSize = 0;
	unsigned int infSize = 0;
	unsigned int idxSize = 0;

	input = Animation::ReadUInt(input, posSize);
	mVertices.resize(posSize * 3);
	for (unsigned int i = 0; i < posSize; ++i) {
		input = Animation::ReadFloat(input, mVertices[i * 3 + 0]);
		input = Animation::ReadFloat(input, mVertices[i * 3 + 1]);
		input = Animation::ReadFloat(input, mVertices[i * 3 + 2]);
	}

	input = Animation::ReadUInt(input, normSize);
	mNormals.resize(normSize * 3);
	for (unsigned int i = 0; i < normSize; ++i) {
		input = Animation::ReadFloat(input, mNormals[i * 3 + 0]);
		input = Animation::ReadFloat(input, mNormals[i * 3 + 1]);
		input = Animation::ReadFloat(input, mNormals[i * 3 + 2]);
	}

	input = Animation::ReadUInt(input, texSize);
	mTexCoords.resize(texSize * 2);
	for (unsigned int i = 0; i < texSize; ++i) {
		input = Animation::ReadFloat(input, mTexCoords[i * 2 + 0]);
		input = Animation::ReadFloat(input, mTexCoords[i * 2 + 1]);
	}

	input = Animation::ReadUInt(input, weightSize);
	mWeights.resize(weightSize * 4);
	for (unsigned int i = 0; i < weightSize; ++i) {
		input = Animation::ReadFloat(input, mWeights[i * 4 + 0]);
		input = Animation::ReadFloat(input, mWeights[i * 4 + 1]);
		input = Animation::ReadFloat(input, mWeights[i * 4 + 2]);
		input = Animation::ReadFloat(input, mWeights[i * 4 + 3]);
	}

	input = Animation::ReadUInt(input, infSize);
	mInfluences.resize(infSize * 4);
	for (unsigned int i = 0; i < infSize; ++i) {
		input = Animation::ReadUInt(input, mInfluences[i * 4 + 0]);
		input = Animation::ReadUInt(input, mInfluences[i * 4 + 1]);
		input = Animation::ReadUInt(input, mInfluences[i * 4 + 2]);
		input = Animation::ReadUInt(input, mInfluences[i * 4 + 3]);
	}

	input = Animation::ReadUInt(input, idxSize);
	mIndices.resize(idxSize);
	for (unsigned int i = 0; i < idxSize; ++i) {
		input = Animation::ReadUInt(input, mIndices[i]);
	}

	free(fileContent);

	mSkinned.resize(mVertices.size() + mNormals.size());
}

void SkinnedSample::LoadAnimation() {
	char* input = ReadFileContents("Assets/bindState.txt");
	mBindPose.DeSerializeFromString(input);
	free(input);

	input = ReadFileContents("Assets/restState.txt");
	mRestPose.DeSerializeFromString(input);
	mAnimatedPose = mRestPose;
	free(input);

	input = ReadFileContents("Assets/walkingData.txt");
	mAniamtionData.DeSerializeFromString(input);
	free(input);

	mPlayTime = 0.0f;
	mInvBindPosePalette.resize(mBindPose.Size() * 16);
	mAnimatedPosePalette.resize(mBindPose.Size() * 16);

	mBindPose.ToMatrixPalette(&mInvBindPosePalette[0], mBindPose.Size() * 16);
	for (int joint = 0; joint < mBindPose.Size(); ++joint) {
		Animation::InvertMatrix(&mInvBindPosePalette[joint * 16], &mInvBindPosePalette[joint * 16]);
	}
}

void SkinnedSample::InitDescriptors() {
	mReadPositions.Set(&mVertices[0], (unsigned int)mVertices.size(), 0, 0);
	mReadNormals.Set(&mNormals[0], (unsigned int)mNormals.size(), 0, 0);
	mReadInfluences.Set(&mInfluences[0], (unsigned int)mInfluences.size(), 0, 0);
	mReadWeights.Set(&mWeights[0], (unsigned int)mWeights.size(), 0, 0);

	mWritePositions.Set(&mSkinned[0], (unsigned int)mVertices.size(), 6 * sizeof(float), 0);
	mWriteNormals.Set(&mSkinned[0], (unsigned int)mVertices.size(), 6 * sizeof(float), 3 * sizeof(float));

	for (unsigned int i = 0; i < mReadPositions.Size(); ++i) {
		*(mWritePositions[i] + 0) = *(mReadPositions[i] + 0);
		*(mWritePositions[i] + 1) = *(mReadPositions[i] + 1);
		*(mWritePositions[i] + 2) = *(mReadPositions[i] + 2);

		*(mWriteNormals[i] + 0) = *(mReadNormals[i] + 0);
		*(mWriteNormals[i] + 1) = *(mReadNormals[i] + 1);
		*(mWriteNormals[i] + 2) = *(mReadNormals[i] + 2);
	}
}

void SkinnedSample::InitOpenGL() {
	mOpenGLInitialized = false;

	if (sizeof(unsigned int) != sizeof(float)) {
		return;
	}
	if (mVertices.size() == 0) {
		return;
	}
	unsigned int numVerts = (unsigned int)(mVertices.size() / 3);
	if (numVerts != (unsigned int)(mNormals.size() / 3)) {
		return;
	}
	if (numVerts != (unsigned int)(mInfluences.size() / 4)) {
		return;
	}
	if (numVerts != (unsigned int)(mWeights.size() / 4)) {
		return;
	}
	if (numVerts != (unsigned int)(mTexCoords.size() / 2)) {
		return;
	}

	glGenVertexArrays(1, &mCharacterVAO);
	glGenBuffers(1, &mCharacterStaticVBO);
	glGenBuffers(1, &mCharacterDynamicVBO);
	glGenBuffers(1, &mCharacterIBO);

	// Make interleaved static VBO
	GLsizei bytes = 0;
	bytes += (GLsizei)(sizeof(float) * mTexCoords.size());
	bytes += (GLsizei)(sizeof(unsigned int) * mInfluences.size());
	bytes += (GLsizei)(sizeof(float) * mWeights.size());

	std::vector<float> staticBufferData;
	staticBufferData.resize(mTexCoords.size() + mInfluences.size() + mWeights.size());

	unsigned int current = 0;
	for (unsigned int i = 0; i < numVerts; ++i) {
		staticBufferData[current++] = mTexCoords[i * 2 + 0];
		staticBufferData[current++] = mTexCoords[i * 2 + 1];
		staticBufferData[current++] = (float)mInfluences[i * 3 + 0];
		staticBufferData[current++] = (float)mInfluences[i * 3 + 1];
		staticBufferData[current++] = (float)mInfluences[i * 3 + 2];
		staticBufferData[current++] = (float)mInfluences[i * 3 + 3];
		staticBufferData[current++] = mWeights[i * 3 + 0];
		staticBufferData[current++] = mWeights[i * 3 + 1];
		staticBufferData[current++] = mWeights[i * 3 + 2];
		staticBufferData[current++] = mWeights[i * 3 + 3];
	}

	glBindVertexArray(mCharacterVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterStaticVBO);
	glBufferData(GL_ARRAY_BUFFER, bytes, &staticBufferData[0], GL_STATIC_DRAW);

	bytes = 0;
	bytes += (GLsizei)(sizeof(float) * mVertices.size());
	bytes += (GLsizei)(sizeof(float) * mNormals.size());
	mSkinned.resize(mVertices.size() + mNormals.size());

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterDynamicVBO);
	glBufferData(GL_ARRAY_BUFFER, bytes, &mSkinned[0], GL_STREAM_DRAW);

	if (mIndices.size() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCharacterIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mIndices.size(), &mIndices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mCharacterShader = glCreateProgram();
	glUseProgram(mCharacterShader);

	// Compile vertex shader
	const char* v_source = ReadFileContents("Assets/char_vert.txt");
	GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v_shader, 1, &v_source, NULL);
	glCompileShader(v_shader);
	int success = 0;
	glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(v_shader, 512, NULL, infoLog);
		std::cout << "ERROR: Vertex compilation failed.\n";
		std::cout << "\t" << infoLog << "\n";
		glDeleteShader(v_shader);
		return;
	};
	//free((void*)v_source);

	// Compile fragment shader
	const char* f_source = ReadFileContents("Assets/char_frag.txt");
	unsigned int f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f_shader, 1, &f_source, NULL);
	glCompileShader(f_shader);
	success = 0;
	glGetShaderiv(f_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(f_shader, 512, NULL, infoLog);
		std::cout << "ERROR: Fragment compilation failed.\n";
		std::cout << "\t" << infoLog << "\n";
		glDeleteShader(f_shader);
		return;
	};
	//free((void*)f_source);


	// Link shaders
	glAttachShader(mCharacterShader, v_shader);
	glAttachShader(mCharacterShader, f_shader);
	glLinkProgram(mCharacterShader);
	success = 0;
	glGetProgramiv(mCharacterShader, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(mCharacterShader, 512, NULL, infoLog);
		std::cout << "ERROR: Shader linking failed.\n";
		std::cout << "\t" << infoLog << "\n";
		glDeleteShader(v_shader);
		glDeleteShader(f_shader);
		return;
	}
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	mCharacterUniformVP = glGetUniformLocation(mCharacterShader, "viewProjection");
	mCharacterUniformModel = glGetUniformLocation(mCharacterShader, "model");
	mCharacterUniformTexture = glGetUniformLocation(mCharacterShader, "tex0");
	mCharacterAttribPosition = glGetAttribLocation(mCharacterShader, "position");
	mCharacterAttribTexCoord = glGetAttribLocation(mCharacterShader, "texCoord");
	mCharacterAttribNormal = glGetAttribLocation(mCharacterShader, "normal");

	glUseProgram(0);

	unsigned char texture[32 * 32 * 3] = { 0 };

	char row_color[32][3] = {
		{0xe7, 0xbd, 0x91},
		{0xe7, 0xbd, 0x91},
		{0xe7, 0xbd, 0x91},
		{0xe7, 0xbd, 0x91},
		{0xe7, 0xbd, 0x91},
		{0xe7, 0xbd, 0x91},
		{0x29, 0x29, 0x29},
		{0x29, 0x29, 0x29},
		{0x29, 0x29, 0x29},
		{0x29, 0x29, 0x29},
		{0x29, 0x29, 0x29},
		{0xd1, 0xc2, 0x70},
		{0xd1, 0xc2, 0x70},
		{0xd1, 0xc2, 0x70},
		{0xd1, 0xc2, 0x70},
		{0xd1, 0xc2, 0x70},
		{0xd1, 0xc2, 0x70},
		{0xe7, 0x65, 0x5c},
		{0xe7, 0x65, 0x5c},
		{0xe7, 0x65, 0x5c},
		{0xe7, 0x65, 0x5c},
		{0xe7, 0x65, 0x5c},
		{0xe7, 0x65, 0x5c},
		{0x61, 0x87, 0xd3},
		{0x61, 0x87, 0xd3},
		{0x61, 0x87, 0xd3},
		{0x61, 0x87, 0xd3},
		{0x61, 0x87, 0xd3},
		{0xf0, 0x47, 0x2f},
		{0xf0, 0x47, 0x2f},
		{0xf0, 0x47, 0x2f},
		{0xf0, 0x47, 0x2f}
	};

	for (unsigned int row = 0; row < 32; ++row) {
		for (unsigned int col = 0; col < 32; ++col) {
			unsigned int index = ((row * 32)) + col;

			texture[index * 3 + 0] = row_color[row][0];
			texture[index * 3 + 1] = row_color[row][1];
			texture[index * 3 + 2] = row_color[row][2];
		}
	}

	glGenTextures(1, &mCharacterTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mCharacterTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	mOpenGLInitialized = true;
}

void SkinnedSample::Initialize() {
	LoadModel();
	LoadAnimation();
	InitOpenGL();
	InitDescriptors();
}

void SkinnedSample::Update(float dt) {
	if (!mOpenGLInitialized) {
		return;
	}

#if 1
	mPlayTime = mAniamtionData.Sample(mAnimatedPose, mPlayTime + dt, true);

	unsigned int numJoints = mAnimatedPose.Size();
	mAnimatedPose.ToMatrixPalette(&mAnimatedPosePalette[0], numJoints * 16);
	//for (unsigned int i = 0; i < numJoints; ++i) {
		//Animation::MultiplyMatrices(&mAnimatedPosePalette[i * 16], &mAnimatedPosePalette[i * 16], &mInvBindPosePalette[i * 16]);
	//}

	//Animation::Skin::Apply(mWritePositions, mReadPositions, 1.0f, &mAnimatedPosePalette[0], mReadInfluences, mReadWeights);
	//Animation::Skin::Apply(mWriteNormals, mReadNormals, 0.0f, &mAnimatedPosePalette[0], mReadInfluences, mReadWeights);

	for (unsigned int i = 0; i < mReadPositions.Size(); ++i) {
		float vertex[4] = {
			* (mReadPositions[i] + 0),
			* (mReadPositions[i] + 1),
			* (mReadPositions[i] + 2),
			1.0f
		};

		float skin[16] = { 0.0f };
		for (int j = 0; j < 4; ++j) {
			unsigned int influence = *(mReadInfluences[i] + j);
			float weight = (*(mReadWeights[i] + j));

			float matrix[16] = { 0.0f };
			Animation::MultiplyMatrices(matrix, &mAnimatedPosePalette[influence], &mInvBindPosePalette[influence]);

			for (int k = 0; k < 16; ++k) {
				skin[k] += matrix[k] * weight;
			}
		}

		float result[4] = { 0.0f };
		Animation::MultiplyMat4Vec4(result, skin, vertex);

		*(mWritePositions[i] + 0) = result[0];
		*(mWritePositions[i] + 1) = result[1];
		*(mWritePositions[i] + 2) = result[2];

		*(mWriteNormals[i] + 0) = *(mReadNormals[i] + 0);
		*(mWriteNormals[i] + 1) = *(mReadNormals[i] + 1);
		*(mWriteNormals[i] + 2) = *(mReadNormals[i] + 2);
	}
#else
	for (unsigned int i = 0; i < mReadPositions.Size(); ++i) {
		*(mWritePositions[i] + 0) = *(mReadPositions[i] + 0);
		*(mWritePositions[i] + 1) = *(mReadPositions[i] + 1);
		*(mWritePositions[i] + 2) = *(mReadPositions[i] + 2);

		*(mWriteNormals[i] + 0) = *(mReadNormals[i] + 0);
		*(mWriteNormals[i] + 1) = *(mReadNormals[i] + 1);
		*(mWriteNormals[i] + 2) = *(mReadNormals[i] + 2);
	}
#endif
}

void SkinnedSample::DrawAnimatedModel(float* viewProjection, float* model) {
	glBindVertexArray(mCharacterVAO);

	glUseProgram(mCharacterShader);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mCharacterTexture); // mCharacterTexture to GL_TEXTURE0
	glUniform1i(mCharacterUniformTexture, 0); // Bind GL_TEXTURE0 to mCharacterUniformTexture

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterStaticVBO);
	glEnableVertexAttribArray(mCharacterAttribTexCoord);
	glVertexAttribPointer(mCharacterAttribTexCoord, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterDynamicVBO);
	glBufferData(GL_ARRAY_BUFFER, mSkinned.size() * sizeof(float), &mSkinned[0], GL_STREAM_DRAW);

	glEnableVertexAttribArray(mCharacterAttribPosition);
	glVertexAttribPointer(mCharacterAttribPosition, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);

	glEnableVertexAttribArray(mCharacterAttribNormal);
	glVertexAttribPointer(mCharacterAttribNormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

	glUniformMatrix4fv(mCharacterUniformVP, 1, GL_FALSE, viewProjection);
	glUniformMatrix4fv(mCharacterUniformModel, 1, GL_FALSE, model);

	if (mIndices.size() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCharacterIBO);
		glDrawElements(GL_TRIANGLES, (GLsizei)mIndices.size(), GL_UNSIGNED_INT, (GLvoid*)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		unsigned int numVerts = (unsigned int)(mVertices.size() / 3);
		glDrawArrays(GL_TRIANGLES, 0, numVerts);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glBindVertexArray(0);
}

void SkinnedSample::Render(float aspect) {
	if (!mOpenGLInitialized) {
		return;
	}

	float model[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	float view[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	float projection[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	float mvp[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	float position[3] = { 0.0f, 7.0f, 5.0f };
	float target[3] = { 0.0f, 3.0f, 0.0f };
	float up[3] = { 0.0f, 1.0f, 0.0f };

	LookAt(view, position, target, up);
	Perspective(projection, 60.0f, aspect, 0.01f, 1000.0f);
	float tmp[16] = { 0.0f };
	Animation::MultiplyMatrices(mvp, view, model);
	Animation::MultiplyMatrices(mvp, projection, mvp);

	glEnable(GL_DEPTH_TEST);
	DrawAnimatedModel(mvp, model);
}

void SkinnedSample::Shutdown() {
	mVertices.clear();
	mNormals.clear();
	mSkinned.clear();
	mTexCoords.clear();
	mInfluences.clear();
	mWeights.clear();
	mIndices.clear();
	mInvBindPosePalette.clear();
	mAnimatedPosePalette.clear();
	mPlayTime = 0.0f;
	mAniamtionData = Animation::Data();
	mAnimatedPose = Animation::State();
	mRestPose = Animation::State();
	mBindPose = Animation::State();

	if (!mOpenGLInitialized) {
		return;
	}

	glDeleteTextures (1, &mCharacterTexture);
	glDeleteBuffers(1, &mCharacterIBO);
	glDeleteBuffers(1, &mCharacterStaticVBO);
	glDeleteBuffers(1, &mCharacterDynamicVBO);
	glDeleteVertexArrays(1, &mCharacterVAO);
	glDeleteProgram(mCharacterShader);
	mOpenGLInitialized = false;
}