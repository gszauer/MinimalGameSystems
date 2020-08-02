#include "SkeletonSample.h"
#include <iostream>

void SkeletonSample::DrawSkeleton(const Animation::State& state, const mat4& mvp, const vec3& color) {
	vec3 iPos, pPos;
	unsigned int index = 0;

	for (int i = 0, size = (int)state.Size(); i < size; ++i) {
		int p = state.GetParent(i);
		if (p < 0) { continue; }

		state.GetAbsolutePosition(i, iPos.v);
		state.GetAbsolutePosition(p, pPos.v);

		m_Skinned[index++] = iPos;
		m_Skinned[index++] = pPos;
	}
	
	glBindVertexArray(mSkeletonVAO);
	glUseProgram(mSkeletonShader);

	glBindBuffer(GL_ARRAY_BUFFER, mSkeletonVBO);

	glEnableVertexAttribArray(mVertexAttrib);
	glBufferData(GL_ARRAY_BUFFER, m_Skinned.size() * 3 * sizeof(float), m_Skinned[0].v, GL_STREAM_DRAW);
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
	mPlayTime = 0.0f;
	
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

	vec3 iPos, pPos;
	for (int i = 0, size = (int)mRestPose.Size(); i < size; ++i) {
		int p = mRestPose.GetParent(i);
		if (p < 0) {
			continue;
		}

		mRestPose.GetAbsolutePosition(i, iPos.v);
		mRestPose.GetAbsolutePosition(p, pPos.v);

		m_Vertices.push_back(iPos);
		m_Vertices.push_back(pPos);
	}

	m_Skinned = m_Vertices;

	glGenVertexArrays(1, &mSkeletonVAO);
	glGenBuffers(1, &mSkeletonVBO);

	char* v_shader = ReadFileContents("Assets/skel_vert.txt");
	char* f_shader = ReadFileContents("Assets/skel_frag.txt");
	mSkeletonShader = CompileShaders(v_shader, f_shader);
	free(v_shader);
	free(f_shader);

	glUseProgram(mSkeletonShader);
	mColorUniform = glGetUniformLocation(mSkeletonShader, "color");
	mMVPUniform = glGetUniformLocation(mSkeletonShader, "mvp");
	mVertexAttrib = glGetAttribLocation(mSkeletonShader, "position");

	glUseProgram(0);
}

void SkeletonSample::Update(float dt) {
	mPlayTime = mAnimationData.Sample(mAnimatedPose, mPlayTime + dt, true);
}

void SkeletonSample::Render(float aspect) {
	mat4 model, view, projection, mvp;
	
	model.position = vec4(3.0f, 0.0f, 0.0f, 1.0f);
	view = lookAt(vec3(0, 7, 5), vec3(0, 3, 0), vec3(0, 1,0 ));
	projection = perspective(60.0f, aspect, 0.01f, 1000.0f);

	mvp = projection * view * model;

	vec3 red(1.0f, 0.0f, 0.0f);
	vec3 green(0.0f, 1.0f, 0.0f);
	vec3 blue(0.0f, 0.0f, 1.0f);

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