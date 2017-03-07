// FBX.h
#ifndef __H_FBX
#define __H_FBX

#include <windows.h>
#include <GL/gl.h>

#include "matrix.h"

// �@���x�N�g���`��
enum {
	NRM_NONE = 0,	// �@���x�N�g������
	NRM_VTX,		// ���_���̖@���x�N�g��
	NRM_IDX,		// �C���f�b�N�X���̖@���x�N�g��
};

// �x�N�g���^
union FBXVECTOR {
	struct {
		GLfloat	x, y, z;
	}		v;
	VECTOR3	p;
};

typedef float VECTOR2[2];

union FBXVECTOR2 {
	struct {
		GLfloat x, y;
	}		v;
	VECTOR2	p;
};

// FBX �\����
struct FBX {
	int			nVtx;		// ���_��
	FBXVECTOR*	pVtx;		// ���_���W
	int			nIdx;		// �C���f�b�N�X��
	short*		pIdx;		// �C���f�b�N�X
	FBXVECTOR*	pNrm;		// �@���x�N�g��
	int			nNrmType;	// �@���x�N�g���`��

	int			nUV;		// UV���W��
	FBXVECTOR2*	pUV;		// UV���W
	int			nUVType;	// UV���W�`��
	int			nUVIdx;		// UV�C���f�b�N�X��
	short*		pUVIdx;		// UV�C���f�b�N�X
	GLuint		uTexture;	// �e�N�X�`��

	FBXVECTOR	vMin;		// ���E�{�b�N�X�ŏ��l
	FBXVECTOR	vMax;		// ���E�{�b�N�X�ő�l
	FBXVECTOR	vCenter;	// ���E�{�b�N�X/�����S
	float		fRadius;	// ���E�����a
};

int LoadFBX(FBX* pFBX, char* pszPath);
void DrawFBX(FBX* pFBX, MATRIX matWorld);
void ReleaseFBX(FBX* pFBX);

#endif