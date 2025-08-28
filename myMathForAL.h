#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;
#include "myMathForAL.h"

float Length(const Vector3& v);

float Dot(const Vector3& V1, const Vector3& V2);

Vector3 Cross(const Vector3& v1, const Vector3& v2);

Vector3 Normalize(const Vector3& v);

Vector3 TranformM4toV3(const Matrix4x4& m1);

Vector3 Lerp(Vector3 a, Vector3 b, float t);

float Lerp(float a, float b, float t);

float easyIn(float a, float b, float t, float r);

float easyOut(float a, float b, float t, float r);

Vector4 MultV4ByM4(const Vector4& v, const Matrix4x4& m);

Matrix4x4 MultM(const Matrix4x4& m1, const Matrix4x4& m2);

Matrix4x4 Inverse(const Matrix4x4& m);

/// <summary>
/// 拡縮のVector3からMatrix4x4への変換
/// </summary>
/// <param name="scole">Vector3</param>
/// <returns>結果Matrix4x4</returns>
Matrix4x4 MakeScaleMatrixM(const Vector3 scole);

Matrix4x4 MakeRotateXMatrixM(const float theta);

Matrix4x4 MakeRotateYMatrixM(const float theta);

Matrix4x4 MakeRotateZMatrixM(const float theta);

/// <summary>
/// 回転のVector3からMatrix4x4への変換
/// </summary>
/// <param name="rotate">Vector3</param>
/// <returns>結果Matrix4x4</returns>
Matrix4x4 MakeRotateMatrixM(const Matrix4x4 thetaX, const Matrix4x4 thetaY, const Matrix4x4 thetaZ);

/// <summary>
/// 移動のVector3からMatrix4x4への変換
/// </summary>
/// <param name="translate">Vector3</param>
/// <returns>結果Matrix4x4</returns>
Matrix4x4 MakeTranslateMatrixM(const Vector3 translate);

/// <summary>
/// Matrix4x4のアフィン変換(Vector3から)
/// </summary>
/// <param name="scole">Vector3</param>
/// <param name="rotate">Vector3</param>
/// <param name="translate">Vector3</param>
/// <returns>結果Matrix4x4</returns>
Matrix4x4 MakeAffineMatrixM(Vector3 scale, Vector3 rotate, Vector3 translate);

/////////////////////////////////////////////////////////////////////////

Matrix4x4 MakeWorldMatrix(Matrix4x4 origin);

Matrix4x4 MakeViewMatrix(Vector3 scole, Vector3 rotate, Vector3 translate);

Matrix4x4 MakeViewTargetMatrix(Vector3 camera, Vector3 target, Vector3 up);

Matrix4x4 MakeProjectionMatrix(float leftO, float rightO, float topO, float bottomO, float nearO, float farO);

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minD, float maxD);

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);




