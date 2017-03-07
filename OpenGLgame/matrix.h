//===================================================================
// 行列/ベクトル演算ライブラリ
//===================================================================
#pragma once

// 行列/ベクトル型定義
typedef float MATRIX[4][4];
typedef float VECTOR3[3];

// プロトタイプ
void MatrixIdentity(MATRIX m);
void MatrixTranslation(MATRIX m, float fX, float fY, float fZ);
void MatrixScaling(MATRIX m, float fSx, float fSy, float fSz);
void MatrixRotationX(MATRIX m, float fPitch);
void MatrixRotationY(MATRIX m, float fYaw);
void MatrixRotationZ(MATRIX m, float fRoll);
void MatrixRotationYawPitchRoll(MATRIX m, float fYaw, float fPitch, float fRoll);
void MatrixRotationAxis(MATRIX m, VECTOR3 v, float fAngle);
void MatrixMultiply(MATRIX m, MATRIX mL, MATRIX mR);
void Vec3TransformCoord(VECTOR3 v, MATRIX m, VECTOR3 vR);
void Vec3TransformNormal(VECTOR3 v, MATRIX m, VECTOR3 vR);
void Vec3Cross(VECTOR3 v, VECTOR3 vL, VECTOR3 vR);
float Vec3Dot(VECTOR3 vL, VECTOR3 vR);
void Vec3Normalize(VECTOR3 v, VECTOR3 vR);
float Vec3LengthSq(VECTOR3 v);
float Vec3Length(VECTOR3 v);
void Vec3Add(VECTOR3 v, VECTOR3 vL, VECTOR3 vR);
void Vec3Subtract(VECTOR3 v, VECTOR3 vL, VECTOR3 vR);
void Vec3Scale(VECTOR3 v, VECTOR3 vL, float fS);

//===================================================================
// eof
//===================================================================