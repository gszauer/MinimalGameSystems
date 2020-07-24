//#pragma warning(disable : 28251)
//#pragma warning(disable : 28159)
//#pragma warning(disable : 26451)
#define _CRT_SECURE_NO_WARNINGS
#include "ISample.h"
#include <iostream>
#include <cstdio>

void Frustum(float* out, float l, float r, float b, float t, float n, float f) {
	if (l == r || t == b || n == f) {
		std::cout << "WARNING: Trying to create invalid frustum\n";
		return; // Error
	}

	out[0] = (2.0f * n) / (r - l);
	out[1] = 0.0f;
	out[2] = 0.0f;
	out[3] = 0.0f;

	out[4] = 0.0f;
	out[5] = (2.0f * n) / (t - b);
	out[6] = 0.0f;
	out[7] = 0.0f;

	out[8] = (r + l) / (r - l);
	out[9] = (t + b) / (t - b);
	out[10] = (-(f + n)) / (f - n);
	out[11] = -1.0f;

	out[12] = 0.0f;
	out[13] = 0.0f;
	out[14] = (-2 * f * n) / (f - n);
	out[15] = 0.0f;
}

void Perspective(float* out, float fov, float aspect, float znear, float zfar) {
	float ymax = znear * tanf(fov * 3.14159265359f / 360.0f);
	float xmax = ymax * aspect;

	return Frustum(out, -xmax, xmax, -ymax, ymax, znear, zfar);
}

void LookAt(float* out, const float* position, const float* target, const float* up) {
	float delta[3] = {
		target[0] - position[0],
		target[1] - position[1],
		target[2] - position[2]
	};

	float invDeltaLen = 0.0f;
	float deltaDot = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
	if (deltaDot > 0.00001f) { // Len sq
		invDeltaLen = 1.0f / sqrtf(deltaDot);
	}

	float f[3] = { // Forward is negative Z
		delta[0] * invDeltaLen * -1.0f,
		delta[1] * invDeltaLen * -1.0f,
		delta[2] * invDeltaLen * -1.0f
	};

	float r[3] = { // cross(up, f);
		up[1] * f[2] - up[2] * f[1],
		up[2] * f[0] - up[0] * f[2],
		up[0] * f[1] - up[1] * f[0]
	};

	float rDot = r[0] * r[0] + r[1] * r[1] + r[2] * r[2];
	if (rDot < 0.00001f) {
		std::cout << "Could not find right\n";
		return;
	}
	else { // normalize(r);
		float invRLen = 1.0f / sqrtf(rDot);
		r[0] *= invRLen;
		r[1] *= invRLen;
		r[2] *= invRLen;
	}

	float u[3] = { // normalized(cross(f, r));
		f[1] * r[2] - f[2] * r[1],
		f[2] * r[0] - f[0] * r[2],
		f[0] * r[1] - f[1] * r[0]
	};

	float uDot = u[0] * u[0] + u[1] * u[1] + u[2] * u[2];
	if (uDot < 0.00001f) {
		std::cout << "Could not find up\n";
		return;
	}
	else { // do the normalize here
		float invULen = 1.0f / sqrtf(uDot);
		u[0] *= invULen;
		u[1] *= invULen;
		u[2] *= invULen;
	}

	float t[3] = {
		-1.0f * (r[0] * position[0] + r[1] * position[1] + r[2] * position[2]),
		-1.0f * (u[0] * position[0] + u[1] * position[1] + u[2] * position[2]),
		-1.0f * (f[0] * position[0] + f[1] * position[1] + f[2] * position[2]),
	};

	out[0] = r[0];
	out[1] = u[0];
	out[2] = f[0];
	out[3] = 0.0f;

	out[4] = r[1];
	out[5] = u[1];
	out[6] = f[1];
	out[7] = 0.0f;

	out[8] = r[2];
	out[9] = u[2];
	out[10] = f[2];
	out[11] = 0.0f;

	out[12] = t[0];
	out[13] = t[1];
	out[14] = t[2];
	out[15] = 1.0f;
}

char* ReadFileContents(const char* filename) {
	char* buffer = 0;

	FILE* f = fopen(filename, "r");

	if (f) {
		fseek(f, 0, SEEK_END);
		long length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = (char*)malloc(sizeof(char) * (length + 1));
		if (buffer) {
			memset(buffer, 0, sizeof(char) * (length + 1));
			fread(buffer, 1, length, f);
			buffer[length] = '\0';
		}
		else {
			std::cout << "Couldn't allocate buffer\n";
		}
		fclose(f);
	}
	else {
		std::cout << "Could not open file: " << filename << "\n";
	}

	return buffer;
}