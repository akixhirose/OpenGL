//====================
// CCamera.cpp
//====================
//--------------------
// �C���N���[�h
//--------------------
#include <windows.h>		// Windows �v���O�����ɂ͂����t����
#include <GL/gl.h>			// OpenGL �֘A�̃w�b�_
#include <GL/glu.h>			// OpenGL �֘A�̃w�b�_
#define _USE_MATH_DEFINES	// math.h��`�̒萔���g�p
#include <math.h>			// �O�p�֐��A�~�����Ŏg�p

#include "CCamera.h"

//--------------------
// CCamera�R���X�g���N�^
//--------------------
CCamera::CCamera()
{
	Init();
}
//--------------------
// CCamera�f�X�g���N�^
//--------------------
CCamera::~CCamera()
{
}
//--------------------
// InitCamera()
// �J����������
//--------------------
void CCamera::Init()
{
	pos.x = 0.0f;
	pos.y = 0.0f;
	pos.z = 5.0f;

	at.x = at.y = at.z = 0.0f;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	rad.x = rad.y = rad.z = 0.0f;

	range = 5.0f;
}
//--------------------
// Move()
// �J�����𓮂���
//--------------------
void CCamera::Move()
{
	if(rad.x >= 2*M_PI)
		rad.x = 0.0f;
	if(rad.y >= 2*M_PI)
		rad.y = 0.0f;
	if(rad.z >= 2*M_PI)
		rad.z = 0.0f;

	//����]
	if(GetAsyncKeyState(VK_LEFT)&0x8000)
	{
		rad.y += 0.05f;
	}
	//�E��]
	if(GetAsyncKeyState(VK_RIGHT)&0x8000)
	{
		rad.y -= 0.05f;
	}
	//���ړ�
	if(GetAsyncKeyState(VK_DOWN)&0x8000 && !(GetAsyncKeyState(VK_SHIFT)&0x8000))
	{
		pos.y -= 0.05f;
	}
	//��ړ�
	if(GetAsyncKeyState(VK_UP)&0x8000 && !(GetAsyncKeyState(VK_SHIFT)&0x8000))
	{
		pos.y += 0.05f;
	}

	//�g��
	if(GetAsyncKeyState(VK_UP)&0x8000 && GetAsyncKeyState(VK_SHIFT)&0x8000)
	{
		range -= 0.05f;
	}
	//�k��
	if(GetAsyncKeyState(VK_DOWN)&0x8000 && GetAsyncKeyState(VK_SHIFT)&0x8000)
	{
		range += 0.05f;
	}

	pos.x = sinf(rad.y) * range;
	pos.z = cosf(rad.y) * range;
}
//--------------------
// Set()
// �J�����̃Z�b�g
//--------------------
void CCamera::Set()
{
	//���[���h/�r���[�ϊ��s��ݒ�
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		pos.x,pos.y,pos.z,		//���_���W
		at.x,at.y,at.z,			//�����_���W
		up.x,up.y,up.z);		//�A�b�v�x�N�g��
}