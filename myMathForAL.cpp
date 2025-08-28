#include "myMathForAL.h"

float Length(const Vector3& v) { 
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); 
}

float Dot(const Vector3& V1, const Vector3& V2) { return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z; }

Vector3 Cross(const Vector3& v1, const Vector3& v2) { return {(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x)}; }

Vector3 Normalize(const Vector3& v) {
	float length = Length(v);
	if (length != 0) {
		return {v.x / length, v.y / length, v.z / length};
	}
	return Vector3();
}

Vector3 TranformM4toV3(const Matrix4x4& m) { 
	return Vector3(m.m[3][0],m.m[3][1],m.m[3][2]);
}

Vector3 Lerp(Vector3 a, Vector3 b, float t) {
	Vector3 result = {};
	result.x = a.x + (b.x - a.x) * t;
	result.y = a.y + (b.y - a.y) * t;
	result.z = a.z + (b.z - a.z) * t;

	return result;
}

float Lerp(float a, float b, float t) { 
	return (a + (b - a) * t); 
}

float easyIn(float a, float b, float t, float r) {
	float eased = powf(t, r);
	return Lerp(a,b,eased);
}

float easyOut(float a, float b, float t, float r) {
	float eased = 1.0f - powf(1.0f - t, r);
	return Lerp(a,b,eased);
}

Vector4 MultV4ByM4(const Vector4& v, const Matrix4x4& m) {
	Vector4 result;
	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
	result.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
	return result;
}


Matrix4x4 MultM(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 resuit = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				resuit.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return resuit;
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	float determinant = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

	                    - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

	                    - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

	                    + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

	                    + m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

	                    - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

	                    - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

	                    + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	return Matrix4x4{
	    (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] -
	     m.m[1][1] * m.m[2][3] * m.m[3][2]) /
	        determinant, /// 11

	    (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] +
	     m.m[0][1] * m.m[2][3] * m.m[3][2]) /
	        determinant, /// 12

	    (+m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] -
	     m.m[0][1] * m.m[1][3] * m.m[3][2]) /
	        determinant, /// 13

	    (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] +
	     m.m[0][1] * m.m[1][3] * m.m[2][2]) /
	        determinant, /// 14

	    (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] +
	     m.m[1][0] * m.m[2][3] * m.m[3][2]) /
	        determinant, /// 21

	    (+m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] -
	     m.m[0][0] * m.m[2][3] * m.m[3][2]) /
	        determinant, /// 22

	    (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] +
	     m.m[0][0] * m.m[1][3] * m.m[3][2]) /
	        determinant, /// 23

	    (+m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] -
	     m.m[0][0] * m.m[1][3] * m.m[2][2]) /
	        determinant, /// 24

	    (+m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] -
	     m.m[1][0] * m.m[2][3] * m.m[3][1]) /
	        determinant, /// 31

	    (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] +
	     m.m[0][0] * m.m[2][3] * m.m[3][1]) /
	        determinant, /// 32

	    (+m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] -
	     m.m[0][0] * m.m[1][3] * m.m[3][1]) /
	        determinant, /// 33

	    (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] +
	     m.m[0][0] * m.m[1][3] * m.m[2][1]) /
	        determinant, /// 34

	    (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] +
	     m.m[1][0] * m.m[2][2] * m.m[3][1]) /
	        determinant, /// 41

	    (+m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] -
	     m.m[0][0] * m.m[2][2] * m.m[3][1]) /
	        determinant, /// 42

	    (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] +
	     m.m[0][0] * m.m[1][2] * m.m[3][1]) /
	        determinant, /// 43

	    (+m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] -
	     m.m[0][0] * m.m[1][2] * m.m[2][1]) /
	        determinant /// 44
	};
}


Matrix4x4 MakeScaleMatrixM(const Vector3 scole) { return Matrix4x4{scole.x, 0.0f, 0.0f, 0.0f, 0.0f, scole.y, 0.0f, 0.0f, 0.0f, 0.0f, scole.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}; }

Matrix4x4 MakeRotateXMatrixM(const float theta) {
	Matrix4x4 rX = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cosf(theta), sinf(theta), 0.0f, 0.0f, -sinf(theta), cosf(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	return rX;
}

Matrix4x4 MakeRotateYMatrixM(const float theta) {
	Matrix4x4 rY = {cosf(theta), 0.0f, -sinf(theta), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, sinf(theta), 0.0f, cosf(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	return rY;
}

Matrix4x4 MakeRotateZMatrixM(const float theta) {
	Matrix4x4 rZ = {cosf(theta), sinf(theta), 0.0f, 0.0f, -sinf(theta), cosf(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	return rZ;
}

Matrix4x4 MakeRotateMatrixM(const Matrix4x4 thetaX, const Matrix4x4 thetaY, const Matrix4x4 thetaZ) { return MultM(thetaX, MultM(thetaY, thetaZ)); }

Matrix4x4 MakeTranslateMatrixM(const Vector3 translate) { return Matrix4x4{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f}; }

Matrix4x4 MakeAffineMatrixM(Vector3 scale, Vector3 rotate, Vector3 translate) {
	Matrix4x4 r1 = MakeScaleMatrixM(scale);

	Matrix4x4 r2 = MakeRotateMatrixM(MakeRotateXMatrixM(rotate.x), MakeRotateYMatrixM(rotate.y), MakeRotateZMatrixM(rotate.z));

	Matrix4x4 r3 = MakeTranslateMatrixM(translate);

	// return Matrix4x4{ r3 * (r2 * r1) };
	// return Matrix4x4{ r1 * r2 * r3 };
	return Matrix4x4{MultM(r1 ,MultM(r2 , r3))};
}

//////////////////////////////////////////////////////////////////////////////////////

Matrix4x4 MakeWorldMatrix(Matrix4x4 origin) { return origin; }

Matrix4x4 MakeViewMatrix(Vector3 scole, Vector3 rotate, Vector3 translate) { return MakeAffineMatrixM(scole, rotate, translate); }

Matrix4x4 MakeViewTargetMatrix(Vector3 camera, Vector3 target, Vector3 up) { 
	Vector3 eyeTarget;
	eyeTarget.x = target.x - camera.x;
	eyeTarget.y = target.y - camera.y;
	eyeTarget.z = target.z - camera.z;
	Vector3 zAxis = Normalize(eyeTarget);        // 相機的前方向（從 target 看向 eye）
	Vector3 xAxis = Normalize(Cross(up, zAxis)); // 相機的右方向
	Vector3 yAxis = Cross(zAxis, xAxis);         // 相機的上方向（重新正交化）

	return {xAxis.x, yAxis.x, zAxis.x, 0,
		xAxis.y, yAxis.y, zAxis.y, 0,
		xAxis.z, yAxis.z, zAxis.z, 0,
		-Dot(xAxis, camera), -Dot(yAxis, camera), -Dot(zAxis, camera), 1};

}


Matrix4x4 MakeProjectionMatrix(float leftO, float rightO, float topO, float bottomO, float nearO, float farO) {
	return {2.0f / (rightO - leftO),
	        0.0f,
	        0.0f,
	        0.0f,
	        0.0f,
	        2.0f / (topO - bottomO),
	        0.0f,
	        0.0f,
	        0.0f,
	        0.0f,
	        1.0f / (farO - nearO),
	        0.0f,
	        (leftO + rightO) / (leftO - rightO),
	        (topO + bottomO) / (bottomO - topO),
	        nearO / (nearO - farO),
	        1.0f};
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	return {(1.0f / aspectRatio) * (1.0f / tanf(fovY / 2.0f)), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / (tanf(fovY / 2.0f)), 0.0f, 0.0f, 0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f, 0.0f, 0.0f,
	        -(nearClip * farClip) / (farClip - nearClip),      0.0f};
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minD, float maxD) {
	return {width / 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, -(height / 2.0f), 0.0f, 0.0f, 0.0f, 0.0f, maxD - minD, 0.0f, left + (width / 2.0f), top + (height / 2.0f), minD, 1.0f};
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	return {
	    2.0f / (right - left),
	    0.0f,
	    0.0f,
	    0.0f,
	    0.0f,
	    2.0f / (top - bottom),
	    0.0f,
	    0.0f,
	    0.0f,
	    0.0f,
	    1.0f / (farClip - nearClip),
	    0.0f,
	    (left + right) / (left - right),
	    (top + bottom) / (bottom - top),
	    nearClip / (nearClip - farClip),
	    1.0f};
}
