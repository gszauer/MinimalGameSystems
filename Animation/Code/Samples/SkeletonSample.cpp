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

		m_Vertices.push_back(vec3(iPos));
		m_Vertices.push_back(vec3(pPos));
	}

	m_Skinned = m_Vertices;
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

void SkeletonSample::DrawSkeleton(const Animation::State& state, const mat4& mvp, const vec3& color) {
	float iPos[3] = { 0.0f };
	float pPos[3] = { 0.0f };
	unsigned int _index = 0;

	for (int i = 0, size = (int)state.Size(); i < size; ++i) {
		int p = state.GetParent(i);
		if (p < 0) { continue; }

		state.GetAbsolutePosition(i, iPos);
		state.GetAbsolutePosition(p, pPos);

		m_Skinned[_index++] = vec3(iPos);
		m_Skinned[_index++] = vec3(pPos);
	}
	
	glBindVertexArray(mSkeletonVAO);
	glUseProgram(mSkeletonShader);

	glBindBuffer(GL_ARRAY_BUFFER, mSkeletonVBO);

	glEnableVertexAttribArray(mVertexAttrib);
	glBufferData(GL_ARRAY_BUFFER, m_Skinned.size() * 3 * sizeof(float), &m_Skinned[0].v[0], GL_STREAM_DRAW);
	glVertexAttribPointer(mVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glUniformMatrix4fv(mMVPUniform, 1, GL_FALSE, mvp.v);
	glUniform3fv(mColorUniform, 1, color.v);

	unsigned int numVerts = (unsigned int)m_Skinned.size();
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
	mPlayTime = mAnimationData.Sample(mAnimatedPose, mPlayTime + dt, true);
}

void SkeletonSample::Render(float aspect) {
	mat4 model, view, projection, mvp;
	
	model.position = vec4(-3.0f, 0.0f, 0.0f, 1.0f);
	view = lookAt(vec3(0, 7, 5), vec3(0, 3, 0), vec3(0, 1,0 ));
	projection = perspective(60.0f, aspect, 0.01f, 1000.0f);

	mvp = projection * view * model;

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