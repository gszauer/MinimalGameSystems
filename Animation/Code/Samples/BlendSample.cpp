#include "BlendSample.h"
#include "../AnimationBlend.h"
#include "../AnimationBuilder.h"
#include <iostream>

namespace BlendSampleInternal {
	static Animation::Builder::Frame MakeFrame(float time, float in, float value, float out) {
		Animation::Builder::Frame result;

		result.time = time;
		result.in[0] = in;
		result.value[0] = value;
		result.out[0] = out;
		return result;
	}
}

using BlendSampleInternal::MakeFrame;

void BlendSample::LoadModel() {
	char* fileContent = ReadFileContents("Assets/womanMesh.txt");
	const char* input = fileContent;

	unsigned int posSize = 0;
	unsigned int normSize = 0;
	unsigned int texSize = 0;
	unsigned int weightSize = 0;
	unsigned int infSize = 0;
	unsigned int idxSize = 0;

	input = Animation::ReadUInt(input, posSize);
	mVertices.resize(posSize);
	for (unsigned int i = 0; i < posSize; ++i) {
		input = Animation::ReadFloat(input, mVertices[i].x);
		input = Animation::ReadFloat(input, mVertices[i].y);
		input = Animation::ReadFloat(input, mVertices[i].z);
	}

	input = Animation::ReadUInt(input, normSize);
	mNormals.resize(normSize);
	for (unsigned int i = 0; i < normSize; ++i) {
		input = Animation::ReadFloat(input, mNormals[i].x);
		input = Animation::ReadFloat(input, mNormals[i].y);
		input = Animation::ReadFloat(input, mNormals[i].z);
	}

	input = Animation::ReadUInt(input, texSize);
	mTexCoords.resize(texSize);
	for (unsigned int i = 0; i < texSize; ++i) {
		input = Animation::ReadFloat(input, mTexCoords[i].x);
		input = Animation::ReadFloat(input, mTexCoords[i].y);
	}

	input = Animation::ReadUInt(input, weightSize);
	mWeights.resize(weightSize);
	for (unsigned int i = 0; i < weightSize; ++i) {
		input = Animation::ReadFloat(input, mWeights[i].x);
		input = Animation::ReadFloat(input, mWeights[i].y);
		input = Animation::ReadFloat(input, mWeights[i].z);
		input = Animation::ReadFloat(input, mWeights[i].w);
	}

	input = Animation::ReadUInt(input, infSize);
	mInfluences.resize(infSize);
	for (unsigned int i = 0; i < infSize; ++i) {
		input = Animation::ReadUInt(input, mInfluences[i].x);
		input = Animation::ReadUInt(input, mInfluences[i].y);
		input = Animation::ReadUInt(input, mInfluences[i].z);
		input = Animation::ReadUInt(input, mInfluences[i].w);
	}

	input = Animation::ReadUInt(input, idxSize);
	mIndices.resize(idxSize);
	for (unsigned int i = 0; i < idxSize; ++i) {
		input = Animation::ReadUInt(input, mIndices[i]);
	}

	free(fileContent);

	mSkinned.resize(mVertices.size() + mNormals.size());
}

void BlendSample::LoadAnimation() {
	char* input = ReadFileContents("Assets/bindState.txt");
	mBindPose.DeSerializeFromString(input);
	free(input);

	input = ReadFileContents("Assets/restState.txt");
	mRestPose.DeSerializeFromString(input);
	mAnimatedPoseA = mRestPose;
	mAnimatedPoseB = mRestPose;
	mBlendedPose = mRestPose;
	free(input);

	input = ReadFileContents("Assets/walkingData.txt");
	mAniamtionDataA.DeSerializeFromString(input);
	free(input);

	input = ReadFileContents("Assets/runningData.txt");
	mAniamtionDataB.DeSerializeFromString(input);
	free(input);

	mPlayTimeA = 0.0f;
	mPlayTimeB = 0.0f;

	m_InvBindPosePalette.resize(mBindPose.Size());
	m_AnimatedPosePalette.resize(mBindPose.Size());

	Animation::ToMatrixPalette(m_InvBindPosePalette[0].v, mBindPose);

	for (unsigned int i = 0; i < mBindPose.Size(); ++i) {
		Animation::Internal::InvertMatrix(m_InvBindPosePalette[i].v, m_InvBindPosePalette[i].v);
	}

	Animation::Builder::Track blendTrack;
	blendTrack.SetJointID(0);
	blendTrack.SetTarget(Animation::Data::Component::Position);
	blendTrack.PushFrame(MakeFrame(0, 0, 0, 0));
	blendTrack.PushFrame(MakeFrame(0.4f * 2.0f, 0, 0, 0));
	blendTrack.PushFrame(MakeFrame(0.6f * 2.0f, 0, 1.0f, 0));
	blendTrack.PushFrame(MakeFrame(1.4f * 2.0f, 0, 1.0f, 0));
	blendTrack.PushFrame(MakeFrame(1.6f * 2.0f, 0, 0, 0));
	blendTrack.PushFrame(MakeFrame(2 * 2.0f, 0, 0, 0));
	Animation::Builder::Clip blendCurve;
	blendCurve.SetName("Blend Time");
	blendCurve.PushTrack(blendTrack);
	mBlendCurve = Animation::Builder::Convert(blendCurve);
}

void BlendSample::InitDescriptors() {
	mReadPositions.Set(mVertices[0].v, (unsigned int)mVertices.size() * 3, 0, 0);
	mReadNormals.Set(mNormals[0].v, (unsigned int)mNormals.size() * 3, 0, 0);
	mReadInfluences.Set(mInfluences[0].v, (unsigned int)mInfluences.size() * 4, 0, 0);
	mReadWeights.Set(mWeights[0].v, (unsigned int)mWeights.size() * 4, 0, 0);

	mWritePositions.Set(mSkinned[0].v, (unsigned int)mVertices.size() * 3, 6 * sizeof(float), 0);
	mWriteNormals.Set(mSkinned[0].v, (unsigned int)mVertices.size() * 3, 6 * sizeof(float), 3 * sizeof(float));

	for (unsigned int i = 0; i < mReadPositions.Size(); ++i) {
		*(mWritePositions[i] + 0) = *(mReadPositions[i] + 0);
		*(mWritePositions[i] + 1) = *(mReadPositions[i] + 1);
		*(mWritePositions[i] + 2) = *(mReadPositions[i] + 2);

		*(mWriteNormals[i] + 0) = *(mReadNormals[i] + 0);
		*(mWriteNormals[i] + 1) = *(mReadNormals[i] + 1);
		*(mWriteNormals[i] + 2) = *(mReadNormals[i] + 2);
	}
}

void BlendSample::InitOpenGL() {
	mOpenGLInitialized = false;

	if (sizeof(unsigned int) != sizeof(float)) {
		return;
	}
	if (mVertices.size() == 0) {
		return;
	}
	unsigned int numVerts = (unsigned int)mVertices.size();
	if (numVerts != (unsigned int)mNormals.size()) {
		return;
	}
	if (numVerts != (unsigned int)mInfluences.size()) {
		return;
	}
	if (numVerts != (unsigned int)mWeights.size()) {
		return;
	}
	if (numVerts != (unsigned int)mTexCoords.size()) {
		return;
	}

	glGenVertexArrays(1, &mCharacterVAO);
	glGenBuffers(1, &mCharacterStaticVBO);
	glGenBuffers(1, &mCharacterDynamicVBO);
	glGenBuffers(1, &mCharacterIBO);

	// Make interleaved static VBO
	GLsizei bytes = 0;
	bytes += (GLsizei)(sizeof(float) * mTexCoords.size() * 2);
	bytes += (GLsizei)(sizeof(int) * mInfluences.size() * 4);
	bytes += (GLsizei)(sizeof(float) * mWeights.size() * 4);

	std::vector<float> staticBufferData;
	staticBufferData.resize(mTexCoords.size() * 2 + mInfluences.size() * 4 + mWeights.size() * 4);

	unsigned int current = 0;
	for (unsigned int i = 0; i < numVerts; ++i) {
		staticBufferData[current++] = mTexCoords[i].x;
		staticBufferData[current++] = mTexCoords[i].y;
		staticBufferData[current++] = (float)mInfluences[i].x;
		staticBufferData[current++] = (float)mInfluences[i].y;
		staticBufferData[current++] = (float)mInfluences[i].z;
		staticBufferData[current++] = (float)mInfluences[i].w;
		staticBufferData[current++] = mWeights[i].x;
		staticBufferData[current++] = mWeights[i].y;
		staticBufferData[current++] = mWeights[i].z;
		staticBufferData[current++] = mWeights[i].w;
	}

	glBindVertexArray(mCharacterVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterStaticVBO);
	glBufferData(GL_ARRAY_BUFFER, bytes, &staticBufferData[0], GL_STATIC_DRAW);

	bytes = 0;
	bytes += (GLsizei)(sizeof(float) * mVertices.size() * 3);
	bytes += (GLsizei)(sizeof(float) * mNormals.size() * 3);
	mSkinned.resize(mVertices.size() + mNormals.size());

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterDynamicVBO);
	glBufferData(GL_ARRAY_BUFFER, bytes, &mSkinned[0].v, GL_STREAM_DRAW);

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

void BlendSample::Initialize() {
	LoadModel();
	LoadAnimation();
	InitOpenGL();
	InitDescriptors();
}

void BlendSample::Update(float dt) {
	if (!mOpenGLInitialized) {
		return;
	}

	mPlayTimeA = mAniamtionDataA.Sample(mAnimatedPoseA, mPlayTimeA + dt, true);
	mPlayTimeB = mAniamtionDataB.Sample(mAnimatedPoseB, mPlayTimeB + dt, true);

	float result[4] = { 0.0f };
	mBlendTime = mBlendCurve.SampleTrack(result, 0, mBlendTime + dt, true);
	Animation::Blend(mBlendedPose, mAnimatedPoseA, mAnimatedPoseB, result[0]);

	Animation::ToMatrixPalette(m_AnimatedPosePalette[0].v, mBlendedPose);

	Animation::Skin::Apply(mWritePositions, mReadPositions, 1.0f, m_AnimatedPosePalette[0].v, m_InvBindPosePalette[0].v, mReadInfluences, mReadWeights);
	Animation::Skin::Apply(mWriteNormals, mReadNormals, 0.0f, m_AnimatedPosePalette[0].v, m_InvBindPosePalette[0].v, mReadInfluences, mReadWeights);
}

void BlendSample::Render(float aspect) {
	if (!mOpenGLInitialized) {
		return;
	}

	mat4 model, view, projection, mvp, viewProjection;
	model.position = vec4(-3, 0, 0, 1);
	view = lookAt(vec3(0.0f, 7.0f, 5.0f), vec3(0.0f, 3.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = perspective(60.0f, aspect, 0.01f, 1000.0f);
	mvp = projection * view * model;
	viewProjection = projection * view;

	glEnable(GL_DEPTH_TEST);

	glBindVertexArray(mCharacterVAO);

	glUseProgram(mCharacterShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mCharacterTexture); // mCharacterTexture to GL_TEXTURE0
	glUniform1i(mCharacterUniformTexture, 0); // Bind GL_TEXTURE0 to mCharacterUniformTexture

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterStaticVBO);
	glEnableVertexAttribArray(mCharacterAttribTexCoord);
	glVertexAttribPointer(mCharacterAttribTexCoord, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, mCharacterDynamicVBO);
	//glBufferData(GL_ARRAY_BUFFER, mSkinned.size() * 3 * sizeof(float), mSkinned[0].v, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mSkinned.size() * 3 * sizeof(float), mSkinned[0].v);

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
		unsigned int numVerts = (unsigned int)mVertices.size();
		glDrawArrays(GL_TRIANGLES, 0, numVerts);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glBindVertexArray(0);
}

void BlendSample::Shutdown() {
	mVertices.clear();
	mNormals.clear();
	mSkinned.clear();
	mTexCoords.clear();
	mInfluences.clear();
	mWeights.clear();
	mIndices.clear();
	m_InvBindPosePalette.clear();
	m_AnimatedPosePalette.clear();
	mAniamtionDataA = Animation::Data();
	mAniamtionDataB = Animation::Data();
	mBlendCurve = Animation::Data();
	mAnimatedPoseA = Animation::State();
	mAnimatedPoseB = Animation::State();
	mBlendedPose = Animation::State();
	mRestPose = Animation::State();
	mBindPose = Animation::State();

	if (!mOpenGLInitialized) {
		return;
	}

	glDeleteTextures(1, &mCharacterTexture);
	glDeleteBuffers(1, &mCharacterIBO);
	glDeleteBuffers(1, &mCharacterStaticVBO);
	glDeleteBuffers(1, &mCharacterDynamicVBO);
	glDeleteVertexArrays(1, &mCharacterVAO);
	glDeleteProgram(mCharacterShader);
	mOpenGLInitialized = false;
}