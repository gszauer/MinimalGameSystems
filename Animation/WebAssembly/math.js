/*jshint esversion: 6 */
const VEC3_EPSILON = 0.000001;

function v2_zero() {
	return [0, 0];
}

function v2_one() {
	return [1, 1];
}

function v3_zero() {
	return [0, 0, 0];
}

function v3_one() {
	return [1, 1, 1];
}

function v3_right() {
	return [1, 0, 0];
}

function v3_up() {
	return [0, 1, 0];
}

function v3_forward() {
	return [0, 0, 1];
}

function v4_zero() {
	return [0, 0, 0, 0];
}

function v4_one() {
	return [1, 1, 1, 1];
}

function v3_add(l, r) {
	return [l[0] + r[0], l[1] + r[1], l[2] + r[2]];
}

function v3_sub(l, r) {
	return [l[0] - r[0], l[1] - r[1], l[2] - r[2]];
}

function v3_scale(v, f) {
	let result = [v[0] * f, v[1] * f, v[2] * f];
	return result;
}

function v3_mul(l, r) {
	return [l[0] * r[0], l[1] * r[1], l[2] * r[2]];
}

function v3_dot(l, r) {
	return l[0] * r[0] + l[1] * r[1] + l[2] * r[2];
}

function v3_lenSq(v) {
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

function v3_len(v) {
	let lenSq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (lenSq < VEC3_EPSILON) {
		return 0.0;
	}
	return Math.sqrt(lenSq);
}

function v3_normalize(v) {
	let lenSq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (lenSq < VEC3_EPSILON) {
		return;
	}
	let invLen = 1.0 / Math.sqrt(lenSq);

	v[0] *= invLen;
	v[1] *= invLen;
	v[2] *= invLen;
}

function v3_normalized(v) {
	let lenSq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (lenSq < VEC3_EPSILON) {
		return [v[0], v[1], v[2]];
	}
	let invLen = 1.0 / Math.sqrt(lenSq);

	return [
		v[0] * invLen,
		v[1] * invLen,
		v[2] * invLen
	];
}

function v3_angle(l, r) {
	let sqMagL = l[0] * l[1] + l[2] * l[3] + l[4] * l[5];
	let sqMagR = r[0] * r[1] + r[2] * r[3] + r[4] * r[5];

	if (sqMagL < VEC3_EPSILON || sqMagR < VEC3_EPSILON) {
		return 0.0;
	}

	let dot = l[0] * r[0] + l[1] * r[1] + l[2] * r[2];
	let len = Math.sqrt(sqMagL) * Math.sqrt(sqMagR);
	return Math.acos(dot / len);
}

function v3_project(a, b) {
	let magBSq = v3_len(b);
	if (magBSq < VEC3_EPSILON) {
		return [0, 0, 0];
	}
	let scale = v3_dot(a, b) / magBSq;
	return [
		b[0] * scale,
		b[1] * scale,
		b[2] * scale
	];
}

function v3_reject(a, b) {
	let projection = v3_project(a, b);
	return v3_sub(a, projection);
}

function v3_reflect(a, b) {
	let magBSq = v3_len(b);
	if (magBSq < VEC3_EPSILON) {
		return [0, 0, 0];
	}
	let scale = v3_dot(a, b) / magBSq;
	let proj2 = v3_scale(b, scale * 2.0);
	return v3_sub(a, proj2);
}

function v3_negate(v) {
	return [
		-v[0],
		-v[1],
		-v[2]
	];
}

function v3_cross(l, r) {
	return [
		l[1] * r[2] - l[2] * r[1],
		l[2] * r[0] - l[0] * r[2],
		l[0] * r[1] - l[1] * r[0]
	];
}

function v3_lerp(s, e, t) {
	return [
		s[0] + (e[0] - s[0]) * t,
		s[1] + (e[1] - s[1]) * t,
		s[2] + (e[2] - s[2]) * t
	];
}

function v3_nlerp(s, e, t) {
	let lin = [
		s[0] + (e[0] - s[0]) * t,
		s[1] + (e[1] - s[1]) * t,
		s[2] + (e[2] - s[2]) * t
	];
	v3_normalize(lin);
	return lin;
}

function v3_slerp(s, e, t) {
	if (t < 0.01) {
		return v3_nlerp(s, e, t);
	}

	let from = v3_normalized(s);
	let to = v3_normalized(e);

	let theta = v3_angle(from, to);
	let sin_theta = Math.sin(theta);

	let a = Math.sin((1.0 - t) * theta) / sin_theta;
	let b = Math.sin(t * theta) / sin_theta;

	return v3_add(v3_scale(from, a), v3_scale(to, b));
}

function v3_eq(l, r) {
	let diff = v3_sub(l, r);
	return v3_lenSq(diff) < VEC3_EPSILON;
}

/*jshint esversion: 6 */
const MAT4_EPSILON = 0.000001;

function m4_identity() {
	return [
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	];
}

function m4_zero() {
	return [
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0
	];
}

function m4_copy(m) {
	return [
		m[0], m[1], m[2], m[3],
		m[4], m[5], m[6], m[7],
		m[8], m[9], m[10], m[11],
		m[12], m[13], m[14], m[15]
	];
}

function m4_zero() {
	return [
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0
	];
}

function m4_eq(a, b) {
	for (let i = 0; i < 16; ++i) {
		if (Math.abs(a[i] - b[i]) > MAT4_EPSILON) {
			return false;
		}
	}
	return true;
}

function m4_add(a, b) {
	return [
		a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3],
		a[4] + b[4], a[5] + b[5], a[6] + b[6], a[7] + b[7],
		a[8] + b[8], a[9] + b[9], a[10] + b[10], a[11] + b[11],
		a[12] + b[12], a[13] + b[13], a[14] + b[14], a[15] + b[15]
	];
}

function m4_scale(m, f) {
	return [
		m[0] * f, m[1] * f, m[2] * f, m[3] * f,
		m[4] * f, m[5] * f, m[6] * f, m[7] * f,
		m[8] * f, m[9] * f, m[10] * f, m[11] * f,
		m[12] * f, m[13] * f, m[14] * f, m[15] * f
	];
}

function M4_MUL_DOT(a, b, aRow, bCol) {
    return a[0 * 4 + aRow] * b[bCol * 4 + 0] + a[1 * 4 + aRow] * b[bCol * 4 + 1] + a[2 * 4 + aRow] * b[bCol * 4 + 2] + a[3 * 4 + aRow] * b[bCol * 4 + 3];
}

function m4_mul(a, b) {
	return [
		M4_MUL_DOT(a, b, 0, 0), M4_MUL_DOT(a, b, 1, 0), M4_MUL_DOT(a, b, 2, 0), M4_MUL_DOT(a, b, 3, 0), // Column 0
		M4_MUL_DOT(a, b, 0, 1), M4_MUL_DOT(a, b, 1, 1), M4_MUL_DOT(a, b, 2, 1), M4_MUL_DOT(a, b, 3, 1), // Column 1
		M4_MUL_DOT(a, b, 0, 2), M4_MUL_DOT(a, b, 1, 2), M4_MUL_DOT(a, b, 2, 2), M4_MUL_DOT(a, b, 3, 2), // Column 2
		M4_MUL_DOT(a, b, 0, 3), M4_MUL_DOT(a, b, 1, 3), M4_MUL_DOT(a, b, 2, 3), M4_MUL_DOT(a, b, 3, 3)  // Column 3
	];
}

function M4_VEC4_DOT(m, mRow, x, y, z, w) {
	return x * m[0 * 4 + mRow] + y * m[1 * 4 + mRow] + z * m[2 * 4 + mRow] + w * m[3 * 4 + mRow];
}

function m4_transform(m, v) {
	return [
		M4_VEC4_DOT(m, 0, v[0], v[1], v[2], v[3]),
		M4_VEC4_DOT(m, 1, v[0], v[1], v[2], v[3]),
		M4_VEC4_DOT(m, 2, v[0], v[1], v[2], v[3]),
		M4_VEC4_DOT(m, 3, v[0], v[1], v[2], v[3])
	];
}

function m4_transformVector(m, v) {
	return [
		M4_VEC4_DOT(m, 0, v[0], v[1], v[2], 0.0),
		M4_VEC4_DOT(m, 1, v[0], v[1], v[2], 0.0),
		M4_VEC4_DOT(m, 2, v[0], v[1], v[2], 0.0)
	];
}

function m4_transformPoint(m, v) {
	return [
		M4_VEC4_DOT(m, 0, v[0], v[1], v[2], 1.0),
		M4_VEC4_DOT(m, 1, v[0], v[1], v[2], 1.0),
		M4_VEC4_DOT(m, 2, v[0], v[1], v[2], 1.0)
	];
}

function M4_SWAP(m, x, y) {
	let t = m[x]; 
	m[x] = m[y]; 
	m[y] = t; 
}

function m4_transpose(m) {
	M4_SWAP(m, 4 , 1);
	M4_SWAP(m, 8 , 2);
	M4_SWAP(m, 12, 3);
	M4_SWAP(m, 9 , 6);
	M4_SWAP(m, 13, 7);
	M4_SWAP(m, 14, 11);
}

function m4_transposed(m) {
	return [
		m[0], m[4], m[8], m[12],
		m[1], m[5], m[9], m[13],
		m[2], m[6], m[10], m[14],
		m[3], m[7], m[11], m[15]
	];
}

function M4_3X3MINOR(m, c0, c1, c2, r0, r1, r2) {
    return m[c0 * 4 + r0] * (m[c1 * 4 + r1] * m[c2 * 4 + r2] - m[c1 * 4 + r2] * m[c2 * 4 + r1]) - 
     m[c1 * 4 + r0] * (m[c0 * 4 + r1] * m[c2 * 4 + r2] - m[c0 * 4 + r2] * m[c2 * 4 + r1]) + 
     m[c2 * 4 + r0] * (m[c0 * 4 + r1] * m[c1 * 4 + r2] - m[c0 * 4 + r2] * m[c1 * 4 + r1]);
}

function m4_determinant(m) {
	return  m[0] * M4_3X3MINOR(m, 1, 2, 3, 1, 2, 3) - m[4] * M4_3X3MINOR(m, 0, 2, 3, 1, 2, 3) + m[8] * M4_3X3MINOR(m, 0, 1, 3, 1, 2, 3) - m[12] * M4_3X3MINOR(m, 0, 1, 2, 1, 2, 3);
}

function m4_adjugate(m) {
	// Cofactor(M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
	let cofactor = m4_identity();

	cofactor[0] =   M4_3X3MINOR(m, 1, 2, 3, 1, 2, 3);
	cofactor[1] =  -M4_3X3MINOR(m, 1, 2, 3, 0, 2, 3);
	cofactor[2] =   M4_3X3MINOR(m, 1, 2, 3, 0, 1, 3);
	cofactor[3] =  -M4_3X3MINOR(m, 1, 2, 3, 0, 1, 2);

	cofactor[4] =  -M4_3X3MINOR(m, 0, 2, 3, 1, 2, 3);
	cofactor[5] =   M4_3X3MINOR(m, 0, 2, 3, 0, 2, 3);
	cofactor[6] =  -M4_3X3MINOR(m, 0, 2, 3, 0, 1, 3);
	cofactor[7] =   M4_3X3MINOR(m, 0, 2, 3, 0, 1, 2);

	cofactor[8] =   M4_3X3MINOR(m, 0, 1, 3, 1, 2, 3);
	cofactor[9] =  -M4_3X3MINOR(m, 0, 1, 3, 0, 2, 3);
	cofactor[10] =  M4_3X3MINOR(m, 0, 1, 3, 0, 1, 3);
	cofactor[11] = -M4_3X3MINOR(m, 0, 1, 3, 0, 1, 2);

	cofactor[12] = -M4_3X3MINOR(m, 0, 1, 2, 1, 2, 3);
	cofactor[13] =  M4_3X3MINOR(m, 0, 1, 2, 0, 2, 3);
	cofactor[14] = -M4_3X3MINOR(m, 0, 1, 2, 0, 1, 3);
	cofactor[15] =  M4_3X3MINOR(m, 0, 1, 2, 0, 1, 2);

	m4_transpose(cofactor);
	return cofactor;
}

function m4_inverse(m) {
	let det = m4_determinant(m);

	if (det == 0.0) {
		console.error("WARNING: Trying to invert a matrix with a zero determinant\n");
		return m4_identity();
	}
	let adj = m4_adjugate(m);

	return m4_scale(adj, 1.0 / det);
}

function m4_invert(m) {
	let det = m4_determinant(m);

	if (det == 0.0) {
		console.error("WARNING: Trying to invert a matrix with a zero determinant\n");
		m[1]  = m[2]  = m[3]  = 0.0;
		m[4]  = m[6]  = m[7]  = 0.0;
		m[8]  = m[9]  = m[11] = 0.0;
		m[12] = m[13] = m[14] = 0.0;
		m[0]  = m[5]  = m[10] = m[15] = 1.0;
		return;
	}

	let adj = m4_adjugate(m);
	let inv = m4_scale(adj, 1.0 / det);

	m[0]  = inv[0]; m[1]   = inv[1]; m[2]   = inv[2]; m[3]   = inv[3];
	m[4]  = inv[4]; m[5]   = inv[5]; m[6]   = inv[6]; m[7]   = inv[7];
	m[8]  = inv[8]; m[9]   = inv[9]; m[10]  = inv[10]; m[11] = inv[11];
	m[12] = inv[12]; m[13] = inv[13]; m[14] = inv[14]; m[15] = inv[15];
}

function m4_frustum(l, r, b, t, n, f) {
	if (l == r || t == b || n == f) {
		console.error("WARNING: Trying to create invalid frustum\n");
		return m4_identity();
	}
	return [
		(2.0 * n) / (r - l), 0.0                , 0.0                   ,  0.0,
		0.0                , (2.0 * n) / (t - b), 0.0                   ,  0.0,
		(r + l) / (r - l)  , (t + b) / (t - b)  , (-(f + n)) / (f - n)  , -1.0,
		0.0                , 0.0                , (-2 * f * n) / (f - n),  0.0
	];
}

function m4_perspective(fov, aspect, znear, zfar) {
	let ymax = znear * Math.tan(fov * 3.14159265359 / 360.0);
	let xmax = ymax * aspect;

	return m4_frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

function m4_ortho(l, r, b, t, n, f) {
	if (l == r || t == b || n == f) {
		return m4_identity();
	}
	return [
		2.0 / (r - l)       , 0.0                 , 0.0                 , 0.0,
		0.0                 , 2.0 / (t - b)       , 0.0                 , 0.0,
		0.0                 , 0.0                 , -2.0 / (f - n)      , 0.0,
		-((r + l) / (r - l)), -((t + b) / (t - b)), -((f + n) / (f - n)), 1.0
	];
}

function m4_lookAt(position, target, up) {
	let f = v3_scale(v3_normalized(v3_sub(target, position)), -1.0);
	let r = v3_cross(up, f);
	if (v3_eq(r, v3_zero())) {
		return m4_identity();
	}
	v3_normalize(r);
	let u = v3_normalized(v3_cross(f, r));

	let t = [
		-v3_dot(r, position),
		-v3_dot(u, position),
		-v3_dot(f, position)
	];

	return [ 
		r[0], u[0], f[0], 0.0,
		r[1], u[1], f[1], 0.0,
		r[2], u[2], f[2], 0.0,
		t[0], t[1], t[2], 1.0
	];
}

/*jshint esversion: 6 */
const QUAT_EPSILON = 0.000001;
const QUAT_DEG2RAD = 0.0174533;
const QUAT_RAD2DEG = 57.2958;

function q_identity() {
	return [0, 0, 0, 1];
}

function q_angleAxis(angle, axis) {
	let norm = v3_normalized(axis);
	let s = Math.sin(angle * 0.5);

	return [
		norm[0] * s,
		norm[1] * s,
		norm[2] * s,
		Math.cos(angle * 0.5)
	];
}

function q_fromTo(from, to) {
	let f = v3_normalized(from);
	let t = v3_normalized(to);

	if (f == t) {
		return q_identity();
	}
	else if (f == v3_scale(t * -1.0)) {
		let ortho = [1, 0, 0];
		if (Math.fabs(f[1]) < Math.fabs(f[0])) {
			ortho = [0, 1, 0];
		}
		if (Math.fabs(f[2]) < Math.fabs(f[1]) && Math.fabs(f[2]) < Math.fabs(f[0])) {
			ortho = [0, 0, 1];
		}

		let axis = v3_normalized(v3_cross(f, ortho));
		return [axis[0], axis[1], axis[2], 0];
	}

	let half = v3_normalized(f + t);
	let axis = v3_cross(f, half);

	return [
		axis[0],
		axis[1],
		axis[2],
		v3_dot(f, half)
	];
}

function q_getAxis(quat) {
	return v3_normalized([quat[0], quat[1], quat[2]]);
}

function q_getAngle(quat) {
	return 2.0 * Math.acos(quat[3]);
}

function q_add(a, b) {
	return [
		a[0] + b[0],
		a[1] + b[1],
		a[2] + b[2],
		a[3] + b[3]
	];
}

function q_sub(a, b) {
	return [
		a[0] - b[0],
		a[1] - b[1],
		a[2] - b[2],
		a[3] - b[3]
	];
}

function q_scale(a, b) {
	return [
		a[0] * b,
		a[1] * b,
		a[2] * b,
		a[3] * b
	];
}

function q_negate(q) {
	return [
		-q[0],
		-q[1],
		-q[2],
		-q[3]
	];
}

function q_eq(left, right) {
	return (Math.fabs(left[0] - right[0]) <= QUAT_EPSILON && 
		    Math.fabs(left[1] - right[1]) <= QUAT_EPSILON && 
		    Math.fabs(left[2] - right[2]) <= QUAT_EPSILON && 
		    Math.fabs(left[3] - right[3]) <= QUAT_EPSILON);
}

function q_dot(a, b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

function q_lenSq(q) {
	return q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
}

function q_len(q) {
	let lenSq = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
	if (lenSq < QUAT_EPSILON) {
		return 0.0;
	}
	return Math.sqrt(lenSq);
}

function q_normalize(q) {
	let lenSq = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
	if (lenSq < QUAT_EPSILON) {
		return;
	}
	let i_len = 1.0 / Math.sqrt(lenSq);

	q[0] *= i_len;
	q[1] *= i_len;
	q[2] *= i_len;
	q[3] *= i_len;
}

function q_normalized(q) {
	let lenSq = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
	if (lenSq < QUAT_EPSILON) {
		return q_identity();
	}
	let i_len = 1.0 / Math.sqrt(lenSq);

	return [
		q[0] * i_len,
		q[1] * i_len,
		q[2] * i_len,
		q[3] * i_len
	];
}

function q_conjugate(q) {
	return [
		-q[0],
		-q[1],
		-q[2],
		q[3]
	];
}

function q_inverse(q) {
	let lenSq = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
	if (lenSq < QUAT_EPSILON) {
		return q_identity();
	}
	let recip = 1.0 / lenSq;

	return [
		-q[0] * recip,
		-q[1] * recip,
		-q[2] * recip,
		 q[3] * recip
	];
}

function q_invert(q) {
	let lenSq = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
	if (lenSq < QUAT_EPSILON) {
		return q_identity();
	}
	let recip = 1.0 / lenSq;

	
	q[0] = -q[0] * recip,
	q[1] = -q[1] * recip,
	q[2] = -q[2] * recip,
	q[3] =  q[3] * recip
}

function q_mul(Q1, Q2) {
	return [
	  	 Q2[0] * Q1[3] + Q2[1] * Q1[2] - Q2[2] * Q1[1] + Q2[3] * Q1[0],
		-Q2[0] * Q1[2] + Q2[1] * Q1[3] + Q2[2] * Q1[0] + Q2[3] * Q1[1],
		 Q2[0] * Q1[1] - Q2[1] * Q1[0] + Q2[2] * Q1[3] + Q2[3] * Q1[2],
		-Q2[0] * Q1[0] - Q2[1] * Q1[1] - Q2[2] * Q1[2] + Q2[3] * Q1[3]
	];
}

function q_transformVector(q, v) {
	let q_vector = [q[0], q[1], q[2]];
	let q_scalar = q[3];

	return v3_add(v3_add(
		v3_scale(v3_scale(q_vector, 2.0), v3_dot(q_vector, v)),
		v3_scale(v, q_scalar * q_scalar - v3_dot(q_vector, q_vector))),
		v3_scale(v3_scale(v3_cross(q_vector, v), 2.0), q_scalar));
}

function q_mix(from, to, t) {
	return q_add(q_scale(from, (1.0 - t)), q_scale(to, t));
}

function q_nlerp(from, to, t) {
	return q_normalized(q_add(from, q_scale(q_sub(to, from),  t)));
}

function q_pow(q, f) {
	let angle = 2.0 * Math.acos(q[3]);
	let axis = v3_normalized([q[0], q[1], q[2]]);

	let halfCos = Math.cos(f * angle * 0.5);
	let halfSin = Math.sin(f * angle * 0.5);

	return [
		axis[0] * halfSin,
		axis[1] * halfSin,
		axis[2] * halfSin,
		halfCos
	];
}

function q_slerp(start, end, t) {
	if (Math.fabs(v3_dot(start, end)) > 1.0 - QUAT_EPSILON) {
		return q_nlerp(start, end, t);
	}

	return q_normalized(q_mul(q_pow(q_mul(q_inverse(start), end), t), start));
}

function q_lookRotation(direcion, up) {
	let f = v3_normalized(direcion);
	let u = v3_normalized(up);
	let r = v3_cross(u, f);
	u = v3_cross(f, r);

	// From world forward to object forward
	let f2d = q_fromTo([0, 0, 1], f);

	// what direction is the new object up?
	let objectUp = q_transformVector(f2d, [0, 1, 0]);
	// From object up to desired up
	let u2u = q_fromTo(objectUp, u);

	// Rotate to forward direction first, then twist to correct up
	let result = q_mul(f2d, u2u);
	// Dont forget to normalize the result
	return q_normalized(result);
}

function q_toMat4(q) {
	let r = q_transformVector(q, [1, 0, 0]);
	let u = q_transformVector(q, [0, 1, 0]);
	let f = q_transformVector(q, [0, 0, 1]);

	return [
		r[0], r[1], r[2], 0,
		u[0], u[1], u[2], 0,
		f[0], f[1], f[2], 0,
		0, 0, 0, 1
	];
}

function m4_toQuat(m) {
	let up = v3_normalized([m[4], m[5], m[6]]);
	let forward = v3_normalized([m[8], m[9], m[10]]);
	let right = v3_cross(up, forward);
	up = v3_cross(forward, right);

	return q_lookRotation(forward, up);
}

/*jshint esversion: 6 */
const XFORM_EPSILON = 0.000001;

function t_identity() {
	let result = {};
	result.position = [0, 0, 0];
	result.rotation = [0, 0, 0, 1];
	result.scale = [1, 1, 1];
	return result;
}

function t_copy(t) {
	let result = {};
	result.position = [t.position[0], t.position[1], t.position[2]];
	result.rotation = [t.rotation[0], t.rotation[1], t.rotation[2], t.rotation[3]];
	result.scale = [t.scale[0], t.scale[1], t.scale[2]];
	return result;
}

function t_new(pos, rot, scl) {
	let result = {};
	result.position = [pos[0], pos[1], pos[2]];
	result.rotation = [rot[0], rot[1], rot[2], rot[3]];
	result.scale = [scl[0], scl[1], scl[2]];
	return result;
}

function t_combine(a, b) {
	let result = t_identity();

	result.scale = v3_mul(a.scale, b.scale);
	result.rotation = q_mul(b.rotation, a.rotation);

	result.position = q_transformVector(a.rotation, v3_mul(a.scale, b.position));
	result.position = v3_add(a.position, result.position);

	return result;
}

function t_inverse(t) {
	let inv = t_identity();

	inv.rotation = q_inverse(t.rotation);

	inv.scale[0] = Math.abs(t.scale[0]) < XFORM_EPSILON ? 0.0 : 1.0 / t.scale[0];
	inv.scale[1] = Math.abs(t.scale[1]) < XFORM_EPSILON ? 0.0 : 1.0 / t.scale[1];
	inv.scale[2] = Math.abs(t.scale[2]) < XFORM_EPSILON ? 0.0 : 1.0 / t.scale[2];

	let invTranslation = v3_scale(t.position, -1.0);
	inv.position = q_transformVector(inv.rotation, v3_mul(inv.scale, invTranslation));

	return inv;
}

function t_mix(a, b, t) {
	let bRot = [b.rotation[0], b.rotation[1], b.rotation[2], b.rotation[3]];
	if (q_dot(a.rotation, bRot) < 0.0) {
		q_invert(bRot);
	}
	return t_new(
		v3_lerp(a.position, b.position, t),
		q_nlerp(a.rotation, bRot, t),
		v3_lerp(a.scale, b.scale, t)
	);
}

function t_toMat4(t) {
	let x = q_transformVector(t.rotation, [1, 0, 0]);
	let y = q_transformVector(t.rotation, [0, 1, 0]);
	let z = q_transformVector(t.rotation, [0, 0, 1]);

	x = v3_scale(x, t.scale[0]);
	y = v3_scale(y, t.scale[1]);
	z = v3_scale(z, t.scale[2]);

	let p = t.position;

	return [
		x[0], x[1], x[2], 0,
		y[0], y[1], y[2], 0,
		z[0], z[1], z[2], 0,
		p[0], p[1], p[2], 1
	];
}

function m4_toTransform(m) {
	let out = t_identity();

	out.position = [m.v[12], m.v[13], m.v[14]];
	out.rotation = m4_toQuat(m);

	let rotScaleMat = [
		m.v[0], m.v[1], m.v[2], 0,
		m.v[4], m.v[5], m.v[6], 0,
		m.v[8], m.v[9], m.v[10], 0,
		0, 0, 0, 1
	];
	let invRotMat = q_toMat4(q_inverse(out.rotation));
	let scaleSkewMat = m4_mul(rotScaleMat, invRotMat);

	out.scale = [
		scaleSkewMat[0],
		scaleSkewMat[5],
		scaleSkewMat[10]
	];

	return out;
}

function t_transformPoint(a, b) {
	let out = [0, 0, 0];

	out = q_transformVector(a.rotation * v3_scale(a.scale, b));
	out = v3_add(a.position, out);

	return out;
}

function t_transformVector(a, b) {
	return q_transformVector(a.rotation, v3_scale(a.scale, b));
}

function t_eq(a, b) {
	return v3_eq(a.position == b.position) &&
		q_eq(a.rotation == b.rotation) &&
		v3_eq(a.scale == b.scale);
}