#include "math.h"
#include "../AnimationInternal.h"

mat4 frustum(float l, float r, float b, float t, float n, float f) {
	if (l == r || t == b || n == f) {
		return mat4(); // Error
	}
	return mat4(
	(2.0f * n) / (r - l), 0, 0, 0,
		0, (2.0f * n) / (t - b), 0, 0,
		(r + l) / (r - l), (t + b) / (t - b), (-(f + n)) / (f - n), -1,
		0, 0, (-2 * f * n) / (f - n), 0
		);
}

#define pi 3.141592f

mat4 operator+(const mat4& a, const mat4& b) {
	return mat4(
		a.xx + b.xx, a.xy + b.xy, a.xz + b.xz, a.xw + b.xw,
		a.yx + b.yx, a.yy + b.yy, a.yz + b.yz, a.yw + b.yw,
		a.zx + b.zx, a.zy + b.zy, a.zz + b.zz, a.zw + b.zw,
		a.tx + b.tx, a.ty + b.ty, a.tz + b.tz, a.tw + b.tw
		);
}


float absf(float x) {
	return x < 0.0f? -x : x;
}

float sinf(float x) {
    const float B = 4.0f/pi;
    const float C = -4.0f/(pi*pi);

    float y = B * x + C * x * absf(x);

	//  const float Q = 0.775;
	const float P = 0.225f;
	y = P * (y * absf(y) - y) + y;   // Q * y + P * y * absf(y)

	return y;
}

float cosf(float x) {
	return sinf(x + pi/2.0f);
}

float tanf(float x) {
	float cos = cosf(x);

	if (Animation::Internal::FloatCompare(cos, 0.0f)) {
		return 0.0f;
	}

	float sin = sinf(x);

	return sin / cos;
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
	float invLen = Animation::Internal::InvSqrt(lenSq);

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
	float invLen = Animation::Internal::InvSqrt(lenSq);

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

#define M4V4D(mRow, x, y, z, w) \
    x * m.v[0 * 4 + mRow] + \
    y * m.v[1 * 4 + mRow] + \
    z * m.v[2 * 4 + mRow] + \
    w * m.v[3 * 4 + mRow]

vec4 operator*(const mat4& m, const vec4& v) {
	return vec4(
		M4V4D(0, v.x, v.y, v.z, v.w),
		M4V4D(1, v.x, v.y, v.z, v.w),
		M4V4D(2, v.x, v.y, v.z, v.w),
		M4V4D(3, v.x, v.y, v.z, v.w)
		);
}

mat4 inverse(const mat4& m) {
	float det = determinant(m);

	if (det == 0.0f) { // Epsilon check would need to be REALLY small
		return mat4();
	}
	mat4 adj = adjugate(m);

	return adj * (1.0f / det);
}

#define M4_3X3MINOR(c0, c1, c2, r0, r1, r2) \
    (m.v[c0 * 4 + r0] * (m.v[c1 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c1 * 4 + r2] * m.v[c2 * 4 + r1]) - \
     m.v[c1 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c2 * 4 + r1]) + \
     m.v[c2 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c1 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c1 * 4 + r1]))

float determinant(const mat4& m) {
	return  m.v[0] * M4_3X3MINOR(1, 2, 3, 1, 2, 3)
		- m.v[4] * M4_3X3MINOR(0, 2, 3, 1, 2, 3)
		+ m.v[8] * M4_3X3MINOR(0, 1, 3, 1, 2, 3)
		- m.v[12] * M4_3X3MINOR(0, 1, 2, 1, 2, 3);
}

mat4 adjugate(const mat4& m) {
	// Cofactor(M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
	mat4 cofactor;

	cofactor.v[0] = M4_3X3MINOR(1, 2, 3, 1, 2, 3);
	cofactor.v[1] = -M4_3X3MINOR(1, 2, 3, 0, 2, 3);
	cofactor.v[2] = M4_3X3MINOR(1, 2, 3, 0, 1, 3);
	cofactor.v[3] = -M4_3X3MINOR(1, 2, 3, 0, 1, 2);

	cofactor.v[4] = -M4_3X3MINOR(0, 2, 3, 1, 2, 3);
	cofactor.v[5] = M4_3X3MINOR(0, 2, 3, 0, 2, 3);
	cofactor.v[6] = -M4_3X3MINOR(0, 2, 3, 0, 1, 3);
	cofactor.v[7] = M4_3X3MINOR(0, 2, 3, 0, 1, 2);

	cofactor.v[8] = M4_3X3MINOR(0, 1, 3, 1, 2, 3);
	cofactor.v[9] = -M4_3X3MINOR(0, 1, 3, 0, 2, 3);
	cofactor.v[10] = M4_3X3MINOR(0, 1, 3, 0, 1, 3);
	cofactor.v[11] = -M4_3X3MINOR(0, 1, 3, 0, 1, 2);

	cofactor.v[12] = -M4_3X3MINOR(0, 1, 2, 1, 2, 3);
	cofactor.v[13] = M4_3X3MINOR(0, 1, 2, 0, 2, 3);
	cofactor.v[14] = -M4_3X3MINOR(0, 1, 2, 0, 1, 3);
	cofactor.v[15] = M4_3X3MINOR(0, 1, 2, 0, 1, 2);

	return transposed(cofactor);
}

mat4 transposed(const mat4& m) {
	return mat4(
		m.xx, m.yx, m.zx, m.tx,
		m.xy, m.yy, m.zy, m.ty,
		m.xz, m.yz, m.zz, m.tz,
		m.xw, m.yw, m.zw, m.tw
		);
}

mat4 operator*(const mat4& m, float f) {
	return mat4(
		m.xx * f, m.xy * f, m.xz * f, m.xw * f,
		m.yx * f, m.yy * f, m.yz * f, m.yw * f,
		m.zx * f, m.zy * f, m.zz * f, m.zw * f,
		m.tx * f, m.ty * f, m.tz * f, m.tw * f
		);
}
