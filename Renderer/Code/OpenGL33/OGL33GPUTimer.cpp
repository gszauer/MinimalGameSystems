#include "OGL33GPUTimer.h"
#include "OGL33Loader.h"

const Renderer::IGraphicsDevice* Renderer::OGL33GPUTimer::GetOwner() const {
	return mOwner;
}

Renderer::OGL33GPUTimer::OGL33GPUTimer(const IGraphicsDevice& owner) {
	mOwner = &owner;
	glGenQueries(1, &mCounter);
}

Renderer::OGL33GPUTimer::~OGL33GPUTimer() {
	mOwner = 0;
	glDeleteQueries(1, &mCounter);
}

void Renderer::OGL33GPUTimer::Insert() {
	glQueryCounter(mCounter, GL_TIMESTAMP);
}

bool Renderer::OGL33GPUTimer::IsDone() {
	GLint isDone = 0;
	glGetQueryObjectiv(mCounter, GL_QUERY_RESULT_AVAILABLE, &isDone);
	return isDone != 0;
}

float Renderer::OGL33GPUTimer::operator-(const IGPUTimer& other) {
	const OGL33GPUTimer* o = (const OGL33GPUTimer*)&other;
	GLint64 startTime, stopTime;
	glGetQueryObjectui64v(mCounter, GL_QUERY_RESULT, &startTime);
	glGetQueryObjectui64v(o->mCounter, GL_QUERY_RESULT, &stopTime);

	return float(stopTime - startTime) / 1000000.0f;
}

