// FBX.h
#ifndef __H_FBX
#define __H_FBX

#include <windows.h>
#include <GL/gl.h>

#include "matrix.h"

// 法線ベクトル形式
enum {
	NRM_NONE = 0,	// 法線ベクトル無し
	NRM_VTX,		// 頂点毎の法線ベクトル
	NRM_IDX,		// インデックス毎の法線ベクトル
};

// ベクトル型
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

// FBX 構造体
struct FBX {
	int			nVtx;		// 頂点数
	FBXVECTOR*	pVtx;		// 頂点座標
	int			nIdx;		// インデックス数
	short*		pIdx;		// インデックス
	FBXVECTOR*	pNrm;		// 法線ベクトル
	int			nNrmType;	// 法線ベクトル形式

	int			nUV;		// UV座標数
	FBXVECTOR2*	pUV;		// UV座標
	int			nUVType;	// UV座標形式
	int			nUVIdx;		// UVインデックス数
	short*		pUVIdx;		// UVインデックス
	GLuint		uTexture;	// テクスチャ

	FBXVECTOR	vMin;		// 境界ボックス最小値
	FBXVECTOR	vMax;		// 境界ボックス最大値
	FBXVECTOR	vCenter;	// 境界ボックス/球中心
	float		fRadius;	// 境界球半径
};

int LoadFBX(FBX* pFBX, char* pszPath);
void DrawFBX(FBX* pFBX, MATRIX matWorld);
void ReleaseFBX(FBX* pFBX);

#endif