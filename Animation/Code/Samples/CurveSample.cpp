#include "CurvesSample.h"
#include "../glad.h"
#include <vector>
#include <iostream>
#include "../AnimationBuilder.h"

void PusV(std::vector<float>& target, float x, float y, float z, float r, float g, float b);
void PusV(std::vector<float>& target, float* position, float* color);
void Ortho(float* result, float l, float r, float b, float t, float n, float f);

void CurvesSample::Initialize() {
	// Build test animation data
	Animation::Builder::Frame frame1;
	Animation::Builder::Frame frame2;
	frame2.time = 1.0f;
	frame2.value[0] = 1.0f; // x
	frame2.value[1] = 1.0f; // y

	frame1.out[0] = 1.0f;
	frame2.in[0] = 1.0f;

	Animation::Builder::Track track;
	track.SetTarget(Animation::Data::Component::Position);
	track.PushFrame(frame1);
	track.PushFrame(frame2);
	//track.ForceLinear();

	Animation::Builder::Clip clip;
	clip.PushTrack(track);

	mAnimationData = Animation::Builder::Convert(clip);
	Animation::State thisState, nextState;
	thisState.Resize(1);
	thisState.SetParent(0, -1);
	nextState.Resize(1);
	nextState.SetParent(0, -1);

	// Record vertices to display
	std::vector<float> verts;
	PusV(verts, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	PusV(verts, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	PusV(verts, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	PusV(verts, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//PusV(verts, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	//PusV(verts, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	float thisPosition[3] = { 0.0f, 0.0f, 0.0f };
	float nextPosition[3] = { 0.0f, 0.0f, 0.0f };
	float color[3] = { 0.0f, 0.0f, 1.0f };

	for (unsigned int i = 0; i < 200 - 1; ++i) {
		float this_t = (float)i / 199.0f;
		mAnimationData.Sample(thisState, this_t, false);
		float next_t = (float)(i + 1) / 199.0f;
		mAnimationData.Sample(nextState, next_t, false);

		for (unsigned int j = 0; j < thisState.Size(); ++j) {
			thisState.GetAbsolutePosition(j, thisPosition);
			nextState.GetAbsolutePosition(j, nextPosition);

			PusV(verts, thisPosition, color);
			PusV(verts, nextPosition, color);
		}
	}

	mNumVerts = (unsigned int)verts.size() / 6;

	// Setup OpenGL
	glGenVertexArrays(1, &mCurveVAO);
	glGenBuffers(1, &mCurveVBO);
	mCurveShader = glCreateProgram();

	// Compile vertex shader
	const char* v_source = ReadFileContents("Assets/curve_vert.txt");
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
	const char* f_source = ReadFileContents("Assets/curve_frag.txt");
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
	glAttachShader(mCurveShader, v_shader);
	glAttachShader(mCurveShader, f_shader);
	glLinkProgram(mCurveShader);
	success = 0;
	glGetProgramiv(mCurveShader, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(mCurveShader, 512, NULL, infoLog);
		std::cout << "ERROR: Shader linking failed.\n";
		std::cout << "\t" << infoLog << "\n";
	}
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	glUseProgram(0);

	// Populate OpenGL data
	glBindVertexArray(mCurveVAO);
	glUseProgram(mCurveShader);

	mMVPUniform = glGetUniformLocation(mCurveShader, "mvp");
	mVertexAttrib = glGetAttribLocation(mCurveShader, "position");
	mColorAttrib = glGetAttribLocation(mCurveShader, "color");

	glBindBuffer(GL_ARRAY_BUFFER, mCurveVBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(mVertexAttrib); // TODO: These are probably wrong Renderdoc says they are trash
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
/*
	V: aspect of the viewport:
	  V = viewport_width / viewport_height
	P: aspect of the ortho projection:
	  P = (right - left) / (top - bottom)
	O: aspect of some axis-aligned rectangle which is drawn
	  O = (x_max - x_min) / (y_max - y_min)
	Screen Pos: O / P * V
*/

	float projection[16] = { 0.0f };
	float halfAspect = aspect;
	Ortho(projection, -0.01f, 1.0f * aspect, -0.01f, 1.0f, 0.001f, 50.0f);

	float view[16] = { 0.0f };
	view[0] = view[5] = view[10] = view[15] = 1.0f;
	view[14] = -3.0f;

	float mvp[16] = { 0.0f };
	Animation::MultiplyMatrices(mvp, projection, view);

	glBindVertexArray(mCurveVAO);
	glUseProgram(mCurveShader);
	glUniformMatrix4fv(mMVPUniform, 1, GL_FALSE, mvp);
	glDrawArrays(GL_LINES, 0, mNumVerts);
	glUseProgram(0);
	glBindVertexArray(0);
}

void CurvesSample::Shutdown() {
	glDeleteProgram(mCurveShader);
	glDeleteBuffers(1, &mCurveVBO);
	glDeleteVertexArrays(1, &mCurveVAO);
}

void PusV(std::vector<float>& target, float* position, float* color) {
	target.push_back(position[0]);
	target.push_back(position[1]);
	target.push_back(position[2]);

	target.push_back(color[0]);
	target.push_back(color[1]);
	target.push_back(color[2]);
}

void PusV(std::vector<float>& target, float x, float y, float z, float r, float g, float b) {
	target.push_back(x);
	target.push_back(y);
	target.push_back(z);

	target.push_back(r);
	target.push_back(g);
	target.push_back(b);
}


void Ortho(float* result, float l, float r, float b, float t, float n, float f) {
	if (l == r || t == b || n == f) {
		return;
	}
	
	result[0] = 2.0f / (r - l);
	result[1] = 0.0f;
	result[2] = 0.0f;
	result[3] = 0.0f;

	result[4] = 0.0f;
	result[5] = 2.0f / (t - b);
	result[6] = 0.0f;
	result[7] = 0.0f;

	result[8] = 0.0f;
	result[9] = 0.0f;
	result[10] = -2.0f / (f - n);
	result[11] = 0.0f;

	result[12] = -((r + l) / (r - l));
	result[13] = -((t + b) / (t - b));
	result[14] = -((f + n) / (f - n));
	result[15] = 1.0f;
}