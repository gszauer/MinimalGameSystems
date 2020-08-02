//#pragma warning(disable : 28251)
//#pragma warning(disable : 28159)
//#pragma warning(disable : 26451)
#define _CRT_SECURE_NO_WARNINGS
#include "ISample.h"
#include <iostream>
#include <cstdio>

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

mat4 frustum(float l, float r, float b, float t, float n, float f) {
	if (l == r || t == b || n == f) {
		std::cout << "WARNING: Trying to create invalid frustum\n";
		return mat4(); // Error
	}
	return mat4(
	(2.0f * n) / (r - l), 0, 0, 0,
		0, (2.0f * n) / (t - b), 0, 0,
		(r + l) / (r - l), (t + b) / (t - b), (-(f + n)) / (f - n), -1,
		0, 0, (-2 * f * n) / (f - n), 0
		);
}

mat4 perspective(float fov, float aspect, float znear, float zfar) {
	float ymax = znear * tanf(fov * 3.14159265359f / 360.0f);
	float xmax = ymax * aspect;

	return frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

mat4 ortho(float l, float r, float b, float t, float n, float f) {
	if (l == r || t == b || n == f) {
		return mat4(); // Error
	}
	return mat4(
		2.0f / (r - l), 0, 0, 0,
		0, 2.0f / (t - b), 0, 0,
		0, 0, -2.0f / (f - n), 0,
		-((r + l) / (r - l)), -((t + b) / (t - b)), -((f + n) / (f - n)), 1
		);
}

bool operator==(const vec3& l, const vec3& r) {
	vec3 diff(l - r);
	return dot(diff, diff) < 0.000001f;
}

bool operator!=(const vec3& l, const vec3& r) {
	return !(l == r);
}

mat4 lookAt(const vec3& position, const vec3& target, const vec3& up) {
	// Remember, forward is negative z
	vec3 f = normalized(target - position) * -1.0f;
	vec3 r = cross(up, f); // Right handed
	if (r == vec3(0, 0, 0)) {
		return mat4(); // Error
	}
	normalize(r);
	vec3 u = normalized(cross(f, r)); // Right handed

	vec3 t = vec3(
		-dot(r, position),
		-dot(u, position),
		-dot(f, position)
		);

	return mat4(
		// Transpose upper 3x3 matrix to invert it
		r.x, u.x, f.x, 0,
		r.y, u.y, f.y, 0,
		r.z, u.z, f.z, 0,
		t.x, t.y, t.z, 1
		);
}

void normalize(vec3& v) {
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < 0.000001f) {
		return;
	}
	float invLen = 1.0f / sqrtf(lenSq);

	v.x *= invLen;
	v.y *= invLen;
	v.z *= invLen;
}

vec3 operator-(const vec3& l, const vec3& r) {
	return vec3(l.x - r.x, l.y - r.y, l.z - r.z);
}

vec3 operator*(const vec3& v, float f) {
	return vec3(v.x * f, v.y * f, v.z * f);
}

vec3 operator*(const vec3& l, const vec3& r) {
	return vec3(l.x * r.x, l.y * r.y, l.z * r.z);
}

vec3 normalized(const vec3& v) {
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < 0.000001f) {
		return v;
	}
	float invLen = 1.0f / sqrtf(lenSq);

	return vec3(
		v.x * invLen,
		v.y * invLen,
		v.z * invLen
		);
}

vec3 cross(const vec3& l, const vec3& r) {
	return vec3(
		l.y * r.z - l.z * r.y,
		l.z * r.x - l.x * r.z,
		l.x * r.y - l.y * r.x
		);
}

float dot(const vec3& l, const vec3& r) {
	return l.x * r.x + l.y * r.y + l.z * r.z;
}


#define M4D(aRow, bCol) \
    a.v[0 * 4 + aRow] * b.v[bCol * 4 + 0] + \
    a.v[1 * 4 + aRow] * b.v[bCol * 4 + 1] + \
    a.v[2 * 4 + aRow] * b.v[bCol * 4 + 2] + \
    a.v[3 * 4 + aRow] * b.v[bCol * 4 + 3]

mat4 operator*(const mat4& a, const mat4& b) {
	return mat4(
		M4D(0, 0), M4D(1, 0), M4D(2, 0), M4D(3, 0), // Column 0
		M4D(0, 1), M4D(1, 1), M4D(2, 1), M4D(3, 1), // Column 1
		M4D(0, 2), M4D(1, 2), M4D(2, 2), M4D(3, 2), // Column 2
		M4D(0, 3), M4D(1, 3), M4D(2, 3), M4D(3, 3)  // Column 3
		);
}

GLuint CompileShaders(const char* v_source, const char* f_source) {
	GLuint shader = glCreateProgram();

	// Compile vertex shader
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
	glAttachShader(shader, v_shader);
	glAttachShader(shader, f_shader);
	glLinkProgram(shader);
	success = 0;
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR: Shader linking failed.\n";
		std::cout << "\t" << infoLog << "\n";
	}
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	glUseProgram(0);

	return shader;
}