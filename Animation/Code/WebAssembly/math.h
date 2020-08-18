#ifndef _H_MATH_
#define _H_MATH_

template<typename T>
struct TVec2 {
	union {
		struct {
			T x;
			T y;
		};
		T v[2];
	};
	inline TVec2() : x(T(0)), y(T(0)) { }
	inline TVec2(T _x, T _y) :
		x(_x), y(_y) { }
	inline TVec2(T* fv) :
		x(fv[0]), y(fv[1]) { }
};

typedef TVec2<float> vec2;
typedef TVec2<int> ivec2;
typedef TVec2<unsigned int> uivec2;

struct vec3 {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float v[3];
	};
	inline vec3() : x(0.0f), y(0.0f), z(0.0f) { }
	inline vec3(float _x, float _y, float _z) :
		x(_x), y(_y), z(_z) { }
	inline vec3(float* fv) :
		x(fv[0]), y(fv[1]), z(fv[2]) { }
};

template<typename T>
struct TVec4 {
	union {
		struct {
			T x;
			T y;
			T z;
			T w;
		};
		T v[4];
	};
	inline TVec4() : x((T)0), y((T)0), z((T)0), w((T)0) { }
	inline TVec4(T _x, T _y, T _z, T _w) :
		x(_x), y(_y), z(_z), w(_w) { }
	inline TVec4(T* fv) :
		x(fv[0]), y(fv[1]), z(fv[2]), w(fv[3]) { }
	inline TVec4(const vec3& v, float _w) : x((T)v.x), y((T)v.y), z((T)v.z), w(_w) { }
};

typedef TVec4<float> vec4;
typedef TVec4<int> ivec4;
typedef TVec4<unsigned int> uivec4;

struct mat4 {
	union {
		float v[16];
		struct {
			vec4 right;
			vec4 up;
			vec4 forward;
			vec4 position;
		};
		struct {
			//            row 1     row 2     row 3     row 4
			/* column 1 */float xx; float xy; float xz; float xw;
			/* column 2 */float yx; float yy; float yz; float yw;
			/* column 3 */float zx; float zy; float zz; float zw;
			/* column 4 */float tx; float ty; float tz; float tw;
		};
		struct {
			float c0r0; float c0r1; float c0r2; float c0r3;
			float c1r0; float c1r1; float c1r2; float c1r3;
			float c2r0; float c2r1; float c2r2; float c2r3;
			float c3r0; float c3r1; float c3r2; float c3r3;
		};
		struct {
			float r0c0; float r1c0; float r2c0; float r3c0;
			float r0c1; float r1c1; float r2c1; float r3c1;
			float r0c2; float r1c2; float r2c2; float r3c2;
			float r0c3; float r1c3; float r2c3; float r3c3;
		};
	};
	inline mat4() :
		xx(1), xy(0), xz(0), xw(0),
		yx(0), yy(1), yz(0), yw(0),
		zx(0), zy(0), zz(1), zw(0),
		tx(0), ty(0), tz(0), tw(1) {}

	inline mat4(float* fv) :
		xx(fv[0]), xy(fv[1]), xz(fv[2]), xw(fv[3]),
		yx(fv[4]), yy(fv[5]), yz(fv[6]), yw(fv[7]),
		zx(fv[8]), zy(fv[9]), zz(fv[10]), zw(fv[11]),
		tx(fv[12]), ty(fv[13]), tz(fv[14]), tw(fv[15]) { }

	inline mat4(
		float _00, float _01, float _02, float _03,
		float _10, float _11, float _12, float _13,
		float _20, float _21, float _22, float _23,
		float _30, float _31, float _32, float _33) :
		xx(_00), xy(_01), xz(_02), xw(_03),
		yx(_10), yy(_11), yz(_12), yw(_13),
		zx(_20), zy(_21), zz(_22), zw(_23),
		tx(_30), ty(_31), tz(_32), tw(_33) { }
}; // end mat4 struct

void normalize(vec3& v);
vec3 normalized(const vec3& v);
vec3 cross(const vec3& l, const vec3& r);
float dot(const vec3& l, const vec3& r);
vec3 operator*(const vec3& l, const vec3& r);
vec3 operator-(const vec3& l, const vec3& r);
vec3 operator*(const vec3& v, float f);
bool operator==(const vec3& l, const vec3& r);
bool operator!=(const vec3& l, const vec3& r);

mat4 frustum(float l, float r, float b, float t, float n, float f);
mat4 perspective(float fov, float aspect, float znear, float zfar);
mat4 ortho(float l, float r, float b, float t, float n, float f);
mat4 lookAt(const vec3& position, const vec3& target, const vec3& up);
mat4 operator*(const mat4& a, const mat4& b);
mat4 operator*(const mat4& m, float f);
mat4 inverse(const mat4& m);
mat4 adjugate(const mat4& m);
float determinant(const mat4& m);
mat4 transposed(const mat4& m);
mat4 operator+(const mat4& a, const mat4& b);
vec4 operator*(const mat4& m, const vec4& v);

#endif