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
	m_Vertices.resize(posSize);
	for (unsigned int i = 0; i < posSize; ++i) {
		input = Animation::ReadFloat(input, m_Vertices[i].x);
		input = Animation::ReadFloat(input, m_Vertices[i].y);
		input = Animation::ReadFloat(input, m_Vertices[i].z);
	}

	input = Animation::ReadUInt(input, normSize);
	m_Normals.resize(normSize);
	for (unsigned int i = 0; i < normSize; ++i) {
		input = Animation::ReadFloat(input, m_Normals[i].x);
		input = Animation::ReadFloat(input, m_Normals[i].y);
		input = Animation::ReadFloat(input, m_Normals[i].z);
	}

	input = Animation::ReadUInt(input, texSize);
	m_TexCoords.resize(texSize);
	for (unsigned int i = 0; i < texSize; ++i) {
		input = Animation::ReadFloat(input, m_TexCoords[i].x);
		input = Animation::ReadFloat(input, m_TexCoords[i].y);
	}

	input = Animation::ReadUInt(input, weightSize);
	m_Weights.resize(weightSize);
	for (unsigned int i = 0; i < weightSize; ++i) {
		input = Animation::ReadFloat(input, m_Weights[i].x);
		input = Animation::ReadFloat(input, m_Weights[i].y);
		input = Animation::ReadFloat(input, m_Weights[i].z);
		input = Animation::ReadFloat(input, m_Weights[i].w);
	}

	input = Animation::ReadUInt(input, infSize);
	m_Influences.resize(infSize);
	for (unsigned int i = 0; i < infSize; ++i) {
		input = Animation::ReadUInt(input, m_Influences[i].x);
		input = Animation::ReadUInt(input, m_Influences[i].y);
		input = Animation::ReadUInt(input, m_Influences[i].z);
		input = Animation::ReadUInt(input, m_Influences[i].w);
	}

	input = Animation::ReadUInt(input, idxSize);
	mIndices.resize(idxSize);
	for (unsigned int i = 0; i < idxSize; ++i) {
		input = Animation::ReadUInt(input, mIndices[i]);
	}

	free(fileContent);

	m_Skinned.resize(m_Vertices.size() + m_Normals.size());
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
	m_InvBindPosePalette.resize(mBindPose.Size());
	m_AnimatedPosePalette.resize(mBindPose.Size());
	
	mBindPose.ToMatrixPalette(m_InvBindPosePalette[0].v, mBindPose.Size() * 16);

#if 0
	for (unsigned int i = 0; i < mBindPose.Size(); ++i) {
		Animation::Internal::InvertMatrix(m_InvBindPosePalette[i].v, m_InvBindPosePalette[i].v);
	}
#else
	input = ReadFileContents("Assets/inverseBindPose.txt");
	unsigned int numMatrixElements = 0;
	const char* reader = input;
	reader = Animation::ReadUInt(reader, numMatrixElements);
	unsigned int numMatrices = numMatrixElements / 16;

	m_InvBindPosePalette.resize(numMatrices);
	float* write = m_InvBindPosePalette[0].v;
	for (int i = 0; i < numMatrixElements; ++i) {
		reader = Animation::ReadFloat(reader, *write);
		write += 1;
	}
	free(input);
#endif
}

void SkinnedSample::InitDescriptors() {
	mReadPositions.Set(m_Vertices[0].v, (unsigned int)m_Vertices.size() * 3, 0, 0);
	mReadNormals.Set(m_Normals[0].v, (unsigned int)m_Normals.size() * 3, 0, 0);
	mReadInfluences.Set(m_Influences[0].v, (unsigned int)m_Influences.size() * 4, 0, 0);
	mReadWeights.Set(m_Weights[0].v, (unsigned int)m_Weights.size() * 4, 0, 0);

	mWritePositions.Set(m_Skinned[0].v, (unsigned int)m_Vertices.size() * 3, 6 * sizeof(float), 0);
	mWriteNormals.Set(m_Skinned[0].v, (unsigned int)m_Vertices.size() * 3, 6 * sizeof(float), 3 * sizeof(float));

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
	if (m_Vertices.size() == 0) {
		return;
	}
	unsigned int numVerts = (unsigned int)m_Vertices.size();
	if (numVerts != (unsigned int)m_Normals.size()) {
		return;
	}
	if (numVerts != (unsigned int)m_Influences.size()) {
		return;
	}
	if (numVerts != (unsigned int)m_Weights.size()) {
		return;
	}
	if (numVerts != (unsigned int)m_TexCoords.size()) {
		return;
	}

	glGenVertexArrays(1, &mCharacterVAO);
	glGenBuffers(1, &mCharacterStaticVBO);
	glGenBuffers(1, &mCharacterDynamicVBO);
	glGenBuffers(1, &mCharacterIBO);

	// Make interleaved static VBO
	GLsizei bytes = 0;
	bytes += (GLsizei)(sizeof(float) * m_TexCoords.size() * 2);
	bytes += (GLsizei)(sizeof(int) * m_Influences.size() * 4);
	bytes += (GLsizei)(sizeof(float) * m_Weights.size() * 4);

	std::vector<float> staticBufferData;
	staticBufferData.resize(m_TexCoords.size() * 2 + m_Influences.size() * 4 + m_Weights.size() * 4);

	unsigned int current = 0;
	for (unsigned int i = 0; i < numVerts; ++i) {
		staticBufferData[current++] = m_TexCoords[i].x;
		staticBufferData[current++] = m_TexCoords[i].y;
		staticBufferData[current++] = (float)m_Influences[i].x;
		staticBufferData[current++] = (float)m_Influences[i].y;
		staticBufferData[current++] = (float)m_Influences[i].z;
		staticBufferData[current++] = (float)m_Influences[i].w;
		staticBufferData[current++] = m_Weights[i].x;
		staticBufferData[current++] = m_Weights[i].y;
		staticBufferData[current++] = m_Weights[i].z;
		staticBufferData[current++] = m_Weights[i].w;
	}

	glBindVertexArray(mCharacterVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterStaticVBO);
	glBufferData(GL_ARRAY_BUFFER, bytes, &staticBufferData[0], GL_STATIC_DRAW);

	bytes = 0;
	bytes += (GLsizei)(sizeof(float) * m_Vertices.size() * 3);
	bytes += (GLsizei)(sizeof(float) * m_Normals.size() * 3);
	m_Skinned.resize(m_Vertices.size() + m_Normals.size());

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterDynamicVBO);
	glBufferData(GL_ARRAY_BUFFER, bytes, &m_Skinned[0].v, GL_STREAM_DRAW);

	if (mIndices.size() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCharacterIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mIndices.size(), &mIndices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	char* v_source = ReadFileContents("Assets/char_vert.txt");
	char* f_source = ReadFileContents("Assets/char_frag.txt");
	mCharacterShader = CompileShaders(v_source, f_source);
	free(v_source);
	free(f_source);
	
	glUseProgram(mCharacterShader);

	mCharacterUniformVP = glGetUniformLocation(mCharacterShader, "viewProjection");
	mCharacterUniformModel = glGetUniformLocation(mCharacterShader, "model");
	mCharacterUniformTexture = glGetUniformLocation(mCharacterShader, "tex0");
	mCharacterAttribPosition = glGetAttribLocation(mCharacterShader, "position");
	mCharacterAttribTexCoord = glGetAttribLocation(mCharacterShader, "texCoord");
	mCharacterAttribNormal = glGetAttribLocation(mCharacterShader, "normal");

	glUseProgram(0);

	const char row_color[32][3] = { {0xe7, 0xbd, 0x91}, {0xe7, 0xbd, 0x91}, {0xe7, 0xbd, 0x91}, {0xe7, 0xbd, 0x91}, {0xe7, 0xbd, 0x91}, {0xe7, 0xbd, 0x91}, {0x29, 0x29, 0x29}, {0x29, 0x29, 0x29}, {0x29, 0x29, 0x29}, {0x29, 0x29, 0x29}, {0x29, 0x29, 0x29}, {0xd1, 0xc2, 0x70}, {0xd1, 0xc2, 0x70}, {0xd1, 0xc2, 0x70}, {0xd1, 0xc2, 0x70}, {0xd1, 0xc2, 0x70}, {0xd1, 0xc2, 0x70}, {0xe7, 0x65, 0x5c}, {0xe7, 0x65, 0x5c}, {0xe7, 0x65, 0x5c}, {0xe7, 0x65, 0x5c}, {0xe7, 0x65, 0x5c}, {0xe7, 0x65, 0x5c}, {0x61, 0x87, 0xd3}, {0x61, 0x87, 0xd3}, {0x61, 0x87, 0xd3}, {0x61, 0x87, 0xd3}, {0x61, 0x87, 0xd3}, {0xf0, 0x47, 0x2f}, {0xf0, 0x47, 0x2f}, {0xf0, 0x47, 0x2f}, {0xf0, 0x47, 0x2f} };
	unsigned char texture[32 * 32 * 3] = { 0 };

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

#if	0
	mPlayTime = mAniamtionData.Sample(mAnimatedPose, mPlayTime + dt, true);
	mAnimatedPose.ToMatrixPalette(m_AnimatedPosePalette[0].v, mAnimatedPose.Size());

	for (unsigned int i = 0; i < m_Vertices.size(); ++i) {
		vec4 vertex(m_Vertices[i].x, m_Vertices[i].y, m_Vertices[i].z, 1.0f);
		vec4 normal(m_Normals[i].x, m_Normals[i].y, m_Normals[i].z, 0.0f);

		mat4 skin;
		skin.xx = skin.yy = skin.zz = skin.tw = 0.0f;

		for (int j = 0; j < 4; ++j) {
			unsigned int influence = m_Influences[i].v[j];
			float weight = m_Weights[i].v[j];

			if (weight > 0.0f) {
				float matrix[16] = { 0.0f };
				Animation::Internal::MultiplyMatrices(matrix, m_AnimatedPosePalette[influence].v, m_InvBindPosePalette[influence].v);

				for (int k = 0; k < 16; ++k) {
					skin.v[k] += matrix[k] * weight;
				}
			}
		}

		float result[4] = { 0.0f };
		Animation::Internal::MultiplyMat4Vec4(result, skin, vertex);
		mSkinned[i * 2 + 0] = vec3(result[0], result[1], result[2]);
		Animation::Internal::MultiplyMat4Vec4(result, skin, normal);
		mSkinned[i * 2 + 1] = vec3(result[0], result[1], result[2]);
	}
#else
	mPlayTime = mAniamtionData.Sample(mAnimatedPose, mPlayTime + dt, true);

	unsigned int numJoints = mAnimatedPose.Size();
	mAnimatedPose.ToMatrixPalette(m_AnimatedPosePalette[0].v, numJoints);

#if 0
	for (unsigned int i = 0; i < numJoints; ++i) {
		Animation::Internal::MultiplyMatrices(m_AnimatedPosePalette[i].v, m_AnimatedPosePalette[i].v, m_InvBindPosePalette[i].v);
	}
	Animation::Skin::Apply(mWritePositions, mReadPositions, 1.0f, m_AnimatedPosePalette[0].v, mReadInfluences, mReadWeights);
	Animation::Skin::Apply(mWriteNormals, mReadNormals, 0.0f, m_AnimatedPosePalette[0].v, mReadInfluences, mReadWeights);
#else
	Animation::Skin::Apply(mWritePositions, mReadPositions, 1.0f, m_AnimatedPosePalette[0].v, m_InvBindPosePalette[0].v, mReadInfluences, mReadWeights);
	Animation::Skin::Apply(mWriteNormals, mReadNormals, 0.0f, m_AnimatedPosePalette[0].v, m_InvBindPosePalette[0].v, mReadInfluences, mReadWeights);
#endif
#endif
}

void SkinnedSample::DrawAnimatedModel(const mat4& viewProjection, const mat4& model) {
	glBindVertexArray(mCharacterVAO);

	glUseProgram(mCharacterShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mCharacterTexture); // mCharacterTexture to GL_TEXTURE0
	glUniform1i(mCharacterUniformTexture, 0); // Bind GL_TEXTURE0 to mCharacterUniformTexture

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterStaticVBO);
	glEnableVertexAttribArray(mCharacterAttribTexCoord);
	glVertexAttribPointer(mCharacterAttribTexCoord, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterDynamicVBO);
	glBufferData(GL_ARRAY_BUFFER, m_Skinned.size() * 6 * sizeof(float), m_Skinned[0].v, GL_STREAM_DRAW);

	glEnableVertexAttribArray(mCharacterAttribPosition);
	glVertexAttribPointer(mCharacterAttribPosition, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);

	glEnableVertexAttribArray(mCharacterAttribNormal);
	glVertexAttribPointer(mCharacterAttribNormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

	glUniformMatrix4fv(mCharacterUniformVP, 1, GL_FALSE, viewProjection.v);
	glUniformMatrix4fv(mCharacterUniformModel, 1, GL_FALSE, model.v);

	if (mIndices.size() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCharacterIBO);
		glDrawElements(GL_TRIANGLES, (GLsizei)mIndices.size(), GL_UNSIGNED_INT, (GLvoid*)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		unsigned int numVerts = (unsigned int)m_Vertices.size();
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

	mat4 model, view, projection, mvp;
	view = lookAt(vec3(0.0f, 7.0f, 5.0f), vec3(0.0f, 3.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = perspective(60.0f, aspect, 0.01f, 1000.0f);
	mvp = projection * view * model;

	glEnable(GL_DEPTH_TEST);
	DrawAnimatedModel(mvp, model);
}

void SkinnedSample::Shutdown() {
	m_Vertices.clear();
	m_Normals.clear();
	m_Skinned.clear();
	m_TexCoords.clear();
	m_Influences.clear();
	m_Weights.clear();
	mIndices.clear();
	m_InvBindPosePalette.clear();
	m_AnimatedPosePalette.clear();
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