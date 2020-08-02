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

	// TODO: Convert to better frames
	frame1.time = 0.25f;
	frame1.in[0] = Animation::Builder::StepTangent;
	frame1.value[0] = 0.25f;
	frame1.out[0] = Animation::Builder::StepTangent;
	frame1.in[1] = 0.0f;
	frame1.value[1] = 0.5f;
	frame1.out[1] = 0.0f;
	frame1.in[2] = -4.0f;
	frame1.value[2] = 0.0f;
	frame1.out[2] = 4.0f;

	frame2.time = 0.5f;
	frame2.in[0] = Animation::Builder::StepTangent;
	frame2.value[0] = 0.5f;
	frame2.out[0] = Animation::Builder::StepTangent;
	frame2.in[1] = 0.0f;
	frame2.value[1] = 0.75f;
	frame2.out[1] = 0.0f;
	frame2.in[2] = 4.0f;
	frame2.value[2] = 1.0f;
	frame2.out[2] = -4.0f;

	frame3.time = 0.75f;
	frame3.in[0] = Animation::Builder::StepTangent;
	frame3.value[0] = 0.25f;
	frame3.out[0] = Animation::Builder::StepTangent;
	frame3.in[1] = 0.0f;
	frame3.value[1] = 0.5f;
	frame3.out[1] = 0.0f;
	frame3.in[2] = -4.0f;
	frame3.value[2] = 0.0f;
	frame3.out[2] = 4.0f;

	track = Animation::Builder::Track();
	track.SetTarget(Animation::Data::Component::Position);
	track.SetJointID(1);
	track.PushFrames(&frames[0], 3);

	clip.PushTrack(track);

	frame1 = Animation::Builder::Frame();
	frame2 = Animation::Builder::Frame();
	frame3 = Animation::Builder::Frame();
	Animation::Builder::Frame frame4, frame5, frame6, frame7, frame0;
	track = Animation::Builder::Track();
	track.SetJointID(2);

	frame0.time = 0.0f;
	frame0.in[0] = 0.0f;
	frame0.value[0] = 1.0f;
	frame0.out[0] = -8.286444f;

	frame1.time = 0.05976868f;
	frame1.in[0] = -8.286444f;
	frame1.value[0] = 0.5047302f;
	frame1.out[0] = 5.0676f;

	frame2.time = 0.1444702f;
	frame2.in[0] = 5.0676f;
	frame2.value[0] = 0.9339638f;
	frame2.out[0] = -22.03639f;

	frame3.time = 0.3136442f;
	frame3.in[0] = 8.614264f;
	frame3.value[0] = 0.4386729f;
	frame3.out[0] = 11.181f;

	frame4.time = 0.4447021f;
	frame4.in[0] = -0.8708439f;
	frame4.value[0] = 0.728775f;
	frame4.out[0] = 0.0f;

	frame5.time = 0.6016647f;
	frame5.in[0] = Animation::Data::StepLimit;
	frame5.value[0] = 0.2806604f;
	frame5.out[0] = 0.0f;

	frame6.time = 0.7348642f;
	frame6.in[0] = Animation::Data::StepLimit;
	frame6.value[0] = 0.7429142f;
	frame6.out[0] = 0.0f;

	frame7.time = 1.0f;
	frame7.in[0] = 0.0f;
	frame7.value[0] = 0.0f;
	frame7.out[0] = 0.0f;

	track.SetTarget(Animation::Data::Component::Position);
	track.PushFrame(frame0);
	track.PushFrame(frame1);
	track.PushFrame(frame2);
	track.PushFrame(frame3);
	track.PushFrame(frame4);
	track.PushFrame(frame5);
	track.PushFrame(frame6);
	track.PushFrame(frame7);

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
	vec3 magenta = vec3(0, 1, 1);

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
	verts.push_back(vec3(0.0f, 0.0f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(1.0f, 0.0f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(0.0f, 0.5f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(1.0f, 0.5f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(0.0f, 1.0f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(1.0f, 1.0f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(0.0f, 0.0f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(0.0f, 1.0f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(0.5f, 0.0f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(0.5f, 1.0f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(1.0f, 0.0f, 0.0f));
	verts.push_back(magenta);
	verts.push_back(vec3(1.0f, 1.0f, 0.0f));
	verts.push_back(magenta);

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

void CurvesSample::Update(float dt) {

}

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