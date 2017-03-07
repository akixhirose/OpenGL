//===================================================================
// �s��/�x�N�g�����Z���C�u����
//===================================================================
#include <math.h>
#include "matrix.h"

//-------------------------------------------------------------------
// �P�ʍs�񐶐�
//-------------------------------------------------------------------
void MatrixIdentity(MATRIX m)
{
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			m[i][j] = (float)(i == j);
		}
	}
}

//-------------------------------------------------------------------
// ���s�ړ��s�񐶐�
//-------------------------------------------------------------------
void MatrixTranslation(MATRIX m, float fX, float fY, float fZ)
{
	MatrixIdentity(m);
	m[3][0] = fX;
	m[3][1] = fY;
	m[3][2] = fZ;
}

//-------------------------------------------------------------------
// �g��k���s�񐶐�
//-------------------------------------------------------------------
void MatrixScaling(MATRIX m, float fSx, float fSy, float fSz)
{
	MatrixIdentity(m);
	m[0][0] = fSx;
	m[1][1] = fSy;
	m[2][2] = fSz;
}

//-------------------------------------------------------------------
// X����]�s�񐶐�
//-------------------------------------------------------------------
void MatrixRotationX(MATRIX m, float fPitch)
{
	MatrixIdentity(m);
	m[1][1] = m[2][2] = cosf(fPitch);
	m[1][2] = sinf(fPitch);
	m[2][1] = -m[1][2];
}

//-------------------------------------------------------------------
// Y����]�s�񐶐�
//-------------------------------------------------------------------
void MatrixRotationY(MATRIX m, float fYaw)
{
	MatrixIdentity(m);
	m[0][0] = m[2][2] = cosf(fYaw);
	m[2][0] = sinf(fYaw);
	m[0][2] = -m[2][0];
}

//-------------------------------------------------------------------
// Z����]�s�񐶐�
//-------------------------------------------------------------------
void MatrixRotationZ(MATRIX m, float fRoll)
{
	MatrixIdentity(m);
	m[0][0] = m[1][1] = cosf(fRoll);
	m[0][1] = sinf(fRoll);
	m[1][0] = -m[0][1];
}

//-------------------------------------------------------------------
// YXZ����]
//-------------------------------------------------------------------
void MatrixRotationYawPitchRoll(MATRIX m, float fYaw, float fPitch, float fRoll)
{
	MATRIX mX, mY, mZ;
	MatrixRotationX(mX, fPitch);
	MatrixRotationY(mY, fYaw);
	MatrixRotationZ(mZ, fRoll);
	MatrixMultiply(m, mZ, mX);
	MatrixMultiply(m, m, mY);
}

//-------------------------------------------------------------------
// �C�ӎ�����̉�]
//-------------------------------------------------------------------
void MatrixRotationAxis(MATRIX m, VECTOR3 v, float fAngle)
{
	float s = sinf(fAngle);
	float c = cosf(fAngle);
	float d = 1.0f - c;
	float& x = v[0];
	float& y = v[1];
	float& z = v[2];
	m[0][0] = c + d * x * x;
	m[0][1] = d * x * y + s * z;
	m[0][2] = d * x * z - s * y;
	m[0][3] = 0.0f;
	m[1][0] = d * x * y - s * z;
	m[1][1] = c + d * y * y;
	m[1][2] = d * y * z + s * x;
	m[1][3] = 0.0f;
	m[2][0] = d * x * z + s * y;
	m[2][1] = d * y * z - s * x;
	m[2][2] = c + d * z * z;
	m[2][3] = 0.0f;
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

// �s��~�s�� (���ʂ̍s��Ɍ��̍s��w��s��)
static void _MatrixMultiply(MATRIX m, MATRIX mL, MATRIX mR)
{
	int i, j, k;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			m[i][j] = 0.0f;
			for (k = 0; k < 4; k++) {
				m[i][j] += mL[k][j] * mR[i][k];
			}
		}
	}
}

//-------------------------------------------------------------------
// �s��~�s��
//-------------------------------------------------------------------
void MatrixMultiply(MATRIX m, MATRIX mL, MATRIX mR)
{
	if (m == mL || m == mR) {
		// �����̕ϐ��Ɠ������̂��v�Z�Ɏg�p
		MATRIX mW;
		int i, j;

		_MatrixMultiply(mW, mL, mR);
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				m[i][j] = mW[i][j];
			}
		}
	} else {
		// �v�Z�l�ƌ��ʂ͈قȂ�ϐ�
		_MatrixMultiply(m, mL, mR);
	}
}

// �s��~�x�N�g�� (���ʂ̃x�N�g���Ɍ��̃x�N�g���w��s��)
static void _Vec3TransformCoord(VECTOR3 v, MATRIX m, VECTOR3 vR)
{
	int i, j;

	for (i = 0; i < 3; i++) {
		v[i] = 0.0f;
		for (j = 0; j < 3; j++) {
			v[i] += m[j][i] * vR[j];
		}
		v[i] += m[j][i];
	}
}

//-------------------------------------------------------------------
// �s��~�x�N�g��
//-------------------------------------------------------------------
void Vec3TransformCoord(VECTOR3 v, MATRIX m, VECTOR3 vR)
{
	if (v == vR) {
		VECTOR3 vW;
		int i;

		_Vec3TransformCoord(vW, m, vR);
		for (i = 0; i < 3; i++) {
			v[i] = vW[i];
		}
	} else {
		_Vec3TransformCoord(v, m, vR);
	}
}

// �s��~�x�N�g��(�ړ�����) (���ʂ̃x�N�g���Ɍ��̃x�N�g���w��s��)
static void _Vec3TransformNormal(VECTOR3 v, MATRIX m, VECTOR3 vR)
{
	int i, j;

	for (i = 0; i < 3; i++) {
		v[i] = 0.0f;
		for (j = 0; j < 3; j++) {
			v[i] += m[j][i] * vR[j];
		}
	}
}

//-------------------------------------------------------------------
// �s��~�x�N�g��(�ړ�����)
//-------------------------------------------------------------------
void Vec3TransformNormal(VECTOR3 v, MATRIX m, VECTOR3 vR)
{
	if (v == vR) {
		VECTOR3 vW;
		int i;

		_Vec3TransformNormal(vW, m, vR);
		for (i = 0; i < 3; i++) {
			v[i] = vW[i];
		}
	} else {
		_Vec3TransformNormal(v, m, vR);
	}
}

// �x�N�g���̊O�� (���ʂ̃x�N�g���Ɍ��̃x�N�g���w��s��)
static void _Vec3Cross(VECTOR3 v, VECTOR3 vL, VECTOR3 vR)
{
	v[0] = vL[1] * vR[2] - vL[2] * vR[1];
	v[1] = vL[2] * vR[0] - vL[0] * vR[2];
	v[2] = vL[0] * vR[1] - vL[1] * vR[0];
}

//-------------------------------------------------------------------
// �x�N�g���̊O��
//-------------------------------------------------------------------
void Vec3Cross(VECTOR3 v, VECTOR3 vL, VECTOR3 vR)
{
	if (v == vL || v == vR) {
		VECTOR3 vW;
		int i;

		_Vec3Cross(vW, vL, vR);
		for (i = 0; i < 3; i++) {
			v[i] = vW[i];
		}
	} else {
		_Vec3Cross(v, vL, vR);
	}
}

//-------------------------------------------------------------------
// �x�N�g���̓���
//-------------------------------------------------------------------
float Vec3Dot(VECTOR3 vL, VECTOR3 vR)
{
	return vL[0] * vR[0] + vL[1] * vR[1] + vL[2] * vR[2];
}

//-------------------------------------------------------------------
// �x�N�g���̐��K��
//-------------------------------------------------------------------
void Vec3Normalize(VECTOR3 v, VECTOR3 vR)
{
	float f = 1.0f / Vec3Length(vR);
	v[0] = vR[0] * f;
	v[1] = vR[1] * f;
	v[2] = vR[2] * f;
}

//-------------------------------------------------------------------
// �x�N�g������2��̎擾
//-------------------------------------------------------------------
float Vec3LengthSq(VECTOR3 v)
{
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

//-------------------------------------------------------------------
// �x�N�g�����̎擾
//-------------------------------------------------------------------
float Vec3Length(VECTOR3 v)
{
	return sqrtf(Vec3LengthSq(v));
}

//-------------------------------------------------------------------
// �x�N�g���{�x�N�g��
//-------------------------------------------------------------------
void Vec3Add(VECTOR3 v, VECTOR3 vL, VECTOR3 vR)
{
	v[0] = vL[0] + vR[0];
	v[1] = vL[1] + vR[1];
	v[2] = vL[2] + vR[2];
}

//-------------------------------------------------------------------
// �x�N�g���|�x�N�g��
//-------------------------------------------------------------------
void Vec3Subtract(VECTOR3 v, VECTOR3 vL, VECTOR3 vR)
{
	v[0] = vL[0] - vR[0];
	v[1] = vL[1] - vR[1];
	v[2] = vL[2] - vR[2];
}

//-------------------------------------------------------------------
// �x�N�g���~�X�J���[
//-------------------------------------------------------------------
void Vec3Scale(VECTOR3 v, VECTOR3 vL, float fS)
{
	v[0] = vL[0] * fS;
	v[1] = vL[1] * fS;
	v[2] = vL[2] * fS;
}

//===================================================================
// eof
//===================================================================