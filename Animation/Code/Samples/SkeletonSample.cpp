#include "SkeletonSample.h"
#include <iostream>

void SkeletonSample::CreateModel() {
	float iPos[3] = { 0.0f };
	float pPos[3] = { 0.0f };

	for (int i = 0, size = (int)mRestPose.Size(); i < size; ++i) {
		int p = mRestPose.GetParent(i);
		if (p < 0) {
			continue;
		}

		mRestPose.GetAbsolutePosition(i, iPos);
		mRestPose.GetAbsolutePosition(p, pPos);

		mVertices.push_back(iPos[0]);
		mVertices.push_back(iPos[1]);
		mVertices.push_back(iPos[2]);

		mVertices.push_back(pPos[0]);
		mVertices.push_back(pPos[1]);
		mVertices.push_back(pPos[2]);
	}

	mSkinned = mVertices;
}

void SkeletonSample::LoadAnimation() {
	char* input = ReadFileContents("Assets/bindState.txt");
	mBindPose.DeSerializeFromString(input);
	free(input);

	input = ReadFileContents("Assets/restState.txt");
	mRestPose.DeSerializeFromString(input);
	mAnimatedPose = mRestPose;
	free(input);

	input = ReadFileContents("Assets/walkingData.txt");
	mAnimationData.DeSerializeFromString(input);
	free(input);

	mPlayTime = 0.0f;
}

void SkeletonSample::InitOpenGL() {
	glGenVertexArrays(1, &mSkeletonVAO);
	glGenBuffers(1, &mSkeletonVBO);
	mSkeletonShader = glCreateProgram();

	glUseProgram(mSkeletonShader);

	// Compile vertex shader
	const char* v_source = ReadFileContents("Assets/skel_vert.txt");
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
	};

	// Compile fragment shader
	const char* f_source = ReadFileContents("Assets/skel_frag.txt");
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
	};

	// Link shaders
	glAttachShader(mSkeletonShader, v_shader);
	glAttachShader(mSkeletonShader, f_shader);
	glLinkProgram(mSkeletonShader);
	success = 0;
	glGetProgramiv(mSkeletonShader, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(mSkeletonShader, 512, NULL, infoLog);
		std::cout << "ERROR: Shader linking failed.\n";
		std::cout << "\t" << infoLog << "\n";
	}
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	mColorUniform = glGetUniformLocation(mSkeletonShader, "color");
	mMVPUniform = glGetUniformLocation(mSkeletonShader, "mvp");
	mVertexAttrib = glGetAttribLocation(mSkeletonShader, "position");

	glUseProgram(0);
}

void SkeletonSample::DrawSkeleton(const Animation::State& state, float* mvp, float* color) {
	float iPos[3] = { 0.0f };
	float pPos[3] = { 0.0f };
	unsigned int index = 0;

	for (int i = 0, size = (int)mRestPose.Size(); i < size; ++i) {
		int p = mRestPose.GetParent(i);
		if (p < 0) { continue; }

		mRestPose.GetAbsolutePosition(i, iPos);
		mRestPose.GetAbsolutePosition(p, pPos);

		mSkinned[index++] = iPos[0];
		mSkinned[index++] = iPos[1];
		mSkinned[index++] = iPos[2];

		mSkinned[index++] = pPos[0];
		mSkinned[index++] = pPos[1];
		mSkinned[index++] = pPos[2];
	}
	
	glBindVertexArray(mSkeletonVAO);
	glUseProgram(mSkeletonShader);

	glBindBuffer(GL_ARRAY_BUFFER, mSkeletonVBO);

	glEnableVertexAttribArray(mVertexAttrib);
	glBufferData(GL_ARRAY_BUFFER, mSkinned.size() * sizeof(float), &mSkinned[0], GL_STREAM_DRAW);
	glVertexAttribPointer(mVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glUniformMatrix4fv(mMVPUniform, 1, GL_FALSE, mvp);
	glUniform3fv(mColorUniform, 1, color);

	unsigned int numVerts = (unsigned int)(mSkinned.size() / 3);
	glDrawArrays(GL_LINES, 0, numVerts);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glBindVertexArray(0);
}

void SkeletonSample::Initialize() {
	LoadAnimation();
	CreateModel();
	InitOpenGL();
}

void SkeletonSample::Update(float dt) {
	// TODO: Copy state to mSkinned
}

void SkeletonSample::Render(float aspect) {
	float model[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	float view[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	float projection[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	float mvp[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	float position[3] = { 0.0f, 7.0f, -5.0f };
	float target[3] = { 0.0f, 3.0f, 0.0f };
	float up[3] = { 0.0f, 1.0f, 0.0f };

	LookAt(view, position, target, up);
	Perspective(projection, 60.0f, aspect, 0.01f, 1000.0f);
	//Animation::MultiplyMatrices(mvp, view, model);
	//Animation::MultiplyMatrices(mvp, projection, mvp);
	Animation::MultiplyMatrices(mvp, projection, view);

	float red[3] = { 1.0f, 0.0f, 0.0f };
	float green[3] = { 0.0f, 1.0f, 0.0f };
	float blue[3] = { 0.0f, 0.0f, 1.0f };

	glDisable(GL_DEPTH_TEST);
	DrawSkeleton(mRestPose, mvp, red);
	DrawSkeleton(mBindPose, mvp, green);
	DrawSkeleton(mAnimatedPose, mvp, blue);
	glEnable(GL_DEPTH_TEST);
}

void SkeletonSample::Shutdown() {
	glDeleteProgram(mSkeletonShader);
	glDeleteBuffers(1, &mSkeletonVBO);
	glDeleteVertexArrays(1, &mSkeletonVAO);
}