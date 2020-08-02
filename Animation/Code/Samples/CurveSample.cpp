#include "CurvesSample.h"
#include <vector>
#include <iostream>
#include "../AnimationBuilder.h"

namespace CurveSampleInternal {
	static Animation::Builder::Frame MakeFrame(float time, const vec3& in, const vec3& value, const vec3& out) {
		Animation::Builder::Frame result;

		result.time = time;

		result.in[0] = in.x;
		result.in[1] = in.y;
		result.in[2] = in.z;

		result.value[0] = value.x;;
		result.value[1] = value.y;
		result.value[2] = value.z;

		result.out[0] = out.x;
		result.out[1] = out.y;
		result.out[2] = out.z;

		return result;
	}

	static Animation::Builder::Frame MakeFrame(float time, float in, float value, float out) {
		Animation::Builder::Frame result;

		result.time = time;
		result.in[0] = in;
		result.value[0] = value;
		result.out[0] = out;
		return result;
	}
}

using CurveSampleInternal::MakeFrame;
using Animation::Builder::StepTangent;

void CurvesSample::Initialize() {
	// Build test animation data
	Animation::Builder::Frame frames[8]; 
	frames[0] = MakeFrame(0.0f, vec3(1.0f, -1.0f, StepTangent), vec3(0, 1, 0.5f), vec3(1, -5.157074f, StepTangent));
	frames[1] = MakeFrame(1, vec3(1.0f, -1.088976f, StepTangent), vec3(1, 0, 0.75f), vec3(1, -1.088976f, StepTangent));

	Animation::Builder::Track track;
	track.SetTarget(Animation::Data::Component::Position);
	track.SetJointID(0);
	track.PushFrames(&frames[0], 2);

	Animation::Builder::Clip clip;
	clip.PushTrack(track);

	frames[0] = MakeFrame(0.25f, vec3(StepTangent, 0.0f, -4.0f), vec3(0.25f, 0.5f, 0.0f), vec3(StepTangent, 0.0f, 4.0f));
	frames[1] = MakeFrame(0.5f, vec3(StepTangent, 0.0f, 4.0f), vec3(0.5f, 0.75f, 1.0f), vec3(StepTangent, 0.0f, -4.0f));
	frames[2] = MakeFrame(0.75f, vec3(StepTangent, 0.0f, -4.0f), vec3(0.25f, 0.5f, 0.0f), vec3(StepTangent, 0.0f, 4.0f));

	track = Animation::Builder::Track();
	track.SetTarget(Animation::Data::Component::Position);
	track.SetJointID(1);
	track.PushFrames(&frames[0], 3);

	clip.PushTrack(track);

	frames[0] = MakeFrame(0.0f, 0.0f, 1.0f, -8.286444f);
	frames[1] = MakeFrame(0.05976868f, -8.286444f, 0.5047302f, 5.0676f);
	frames[2] = MakeFrame(0.1444702f, 5.0676f, 0.9339638f, -22.03639f);
	frames[3] = MakeFrame(0.3136442f, 8.614264f, 0.4386729f, 11.181f);
	frames[4] = MakeFrame(0.4447021f, -0.8708439f, 0.728775f, 0.0f);
	frames[5] = MakeFrame(0.6016647f, StepTangent, 0.2806604f, 0.0f);
	frames[6] = MakeFrame(0.7348642f, StepTangent, 0.7429142f, 0.0f);
	frames[7] = MakeFrame(1.0f, 0.0f, 0.0f, 0.0f);

	track = Animation::Builder::Track();
	track.SetTarget(Animation::Data::Component::Position);
	track.SetJointID(2);
	track.PushFrames(&frames[0], 8);

	clip.PushTrack(track);

	mAnimationData = Animation::Builder::Convert(clip);
	Animation::State thisState, nextState;
	thisState.Resize(3);
	thisState.SetParent(0, -1);
	thisState.SetParent(1, -1);
	thisState.SetParent(2, -1);
	nextState = thisState;

	std::vector<vec3> verts;

	float thisPosition[3] = { 0.0f, 0.0f, 0.0f };
	float nextPosition[3] = { 0.0f, 0.0f, 0.0f };

	vec3 red = vec3(1, 0, 0);
	vec3 green = vec3(0, 1, 0);
	vec3 blue = vec3(0, 0, 1);
	vec3 yellow = vec3(1, 1, 0);
	vec3 cyan = vec3(0, 1, 1);

	for (unsigned int i = 0; i < 200 - 1; ++i) {
		float this_t = (float)i / 199.0f;
		float next_t = (float)(i + 1) / 199.0f;

		mAnimationData.Sample(thisState, this_t, false);
		mAnimationData.Sample(nextState, next_t, false);

		thisState.GetAbsolutePosition(0, thisPosition);
		nextState.GetAbsolutePosition(0, nextPosition);

		verts.push_back(vec3(this_t * 0.5f, thisPosition[0] * 0.5f, 0.0f));
		verts.push_back(red);
		verts.push_back(vec3(next_t * 0.5f, nextPosition[0] * 0.5f, 0.0f));
		verts.push_back(red);

		verts.push_back(vec3(this_t * 0.5f, thisPosition[1] * 0.5f, 0.0f));
		verts.push_back(green);
		verts.push_back(vec3(next_t * 0.5f, nextPosition[1] * 0.5f, 0.0f));
		verts.push_back(green);

		verts.push_back(vec3(this_t * 0.5f, thisPosition[2] * 0.5f, 0.0f));
		verts.push_back(blue);
		verts.push_back(vec3(next_t * 0.5f, nextPosition[2] * 0.5f, 0.0f));
		verts.push_back(blue);

		thisState.GetAbsolutePosition(2, thisPosition);
		nextState.GetAbsolutePosition(2, nextPosition);

		verts.push_back(vec3(this_t * 0.5f + 0.5f, thisPosition[0] * 0.5f, 0.0f));
		verts.push_back(yellow);
		verts.push_back(vec3(next_t * 0.5f + 0.5f, nextPosition[0] * 0.5f, 0.0f));
		verts.push_back(yellow);

		mAnimationData.Sample(thisState, this_t, true);
		mAnimationData.Sample(nextState, next_t, true);

		thisState.GetAbsolutePosition(1, thisPosition);
		nextState.GetAbsolutePosition(1, nextPosition);

		verts.push_back(vec3(this_t * 0.5f, thisPosition[0] * 0.5f + 0.5f, 0.0f));
		verts.push_back(red);
		verts.push_back(vec3(next_t * 0.5f, nextPosition[0] * 0.5f + 0.5f, 0.0f));
		verts.push_back(red);
		verts.push_back(vec3(this_t * 0.5f, thisPosition[1] * 0.5f + 0.5f, 0.0f));
		verts.push_back(green);
		verts.push_back(vec3(next_t * 0.5f, nextPosition[1] * 0.5f + 0.5f, 0.0f));
		verts.push_back(green);
		verts.push_back(vec3(this_t * 0.5f, thisPosition[2] * 0.5f + 0.5f, 0.0f));
		verts.push_back(blue);
		verts.push_back(vec3(next_t * 0.5f, nextPosition[2] * 0.5f + 0.5f, 0.0f));
		verts.push_back(blue);
	}

	// Draw borders
	verts.push_back(vec3(0.0f, 0.5f, 0.0f));
	verts.push_back(cyan);
	verts.push_back(vec3(1.0f, 0.5f, 0.0f));
	verts.push_back(cyan);
	verts.push_back(vec3(0.5f, 0.0f, 0.0f));
	verts.push_back(cyan);
	verts.push_back(vec3(0.5f, 1.0f, 0.0f));
	verts.push_back(cyan);

	mNumVerts = (unsigned int)verts.size() / 2;

	// Setup OpenGL
	glGenVertexArrays(1, &mCurveVAO);
	glGenBuffers(1, &mCurveVBO);
	const char* v_source = ReadFileContents("Assets/curve_vert.txt");
	const char* f_source = ReadFileContents("Assets/curve_frag.txt");
	mCurveShader = CompileShaders(v_source, f_source);
	free((void*)v_source);
	free((void*)f_source);

	// Populate OpenGL data
	glBindVertexArray(mCurveVAO);
	glUseProgram(mCurveShader);

	mMVPUniform = glGetUniformLocation(mCurveShader, "mvp");
	mVertexAttrib = glGetAttribLocation(mCurveShader, "position");
	mColorAttrib = glGetAttribLocation(mCurveShader, "color");

	glBindBuffer(GL_ARRAY_BUFFER, mCurveVBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * 6 * sizeof(float), &verts[0].v[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(mVertexAttrib);
	glVertexAttribPointer(mVertexAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(mColorAttrib);
	glVertexAttribPointer(mColorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 3));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glBindVertexArray(0);
}

void CurvesSample::Update(float dt) { }

void CurvesSample::Render(float aspect) {
	mat4 mvp, view, projection;
	projection = ortho(0.0f, 1.0f * aspect, 0.0f, 1.0f, 0.001f, 50.0f);
	view.position = vec4(0, 0, -3, 1);
	mvp = projection * view;

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(mCurveVAO);
	glUseProgram(mCurveShader);
	glUniformMatrix4fv(mMVPUniform, 1, GL_FALSE, mvp.v);
	glDrawArrays(GL_LINES, 0, mNumVerts);
	glUseProgram(0);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void CurvesSample::Shutdown() {
	glDeleteProgram(mCurveShader);
	glDeleteBuffers(1, &mCurveVBO);
	glDeleteVertexArrays(1, &mCurveVAO);
}