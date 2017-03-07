//====================
// CCamera.cpp
//====================
//--------------------
// インクルード
//--------------------
#include <windows.h>		// Windows プログラムにはこれを付ける
#include <GL/gl.h>			// OpenGL 関連のヘッダ
#include <GL/glu.h>			// OpenGL 関連のヘッダ
#define _USE_MATH_DEFINES	// math.h定義の定数を使用
#include <math.h>			// 三角関数、円周率で使用

#include "CCamera.h"

//--------------------
// CCameraコンストラクタ
//--------------------
CCamera::CCamera()
{
	Init();
}
//--------------------
// CCameraデストラクタ
//--------------------
CCamera::~CCamera()
{
}
//--------------------
// InitCamera()
// カメラ初期化
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
// カメラを動かす
//--------------------
void CCamera::Move()
{
	if(rad.x >= 2*M_PI)
		rad.x = 0.0f;
	if(rad.y >= 2*M_PI)
		rad.y = 0.0f;
	if(rad.z >= 2*M_PI)
		rad.z = 0.0f;

	//左回転
	if(GetAsyncKeyState(VK_LEFT)&0x8000)
	{
		rad.y += 0.05f;
	}
	//右回転
	if(GetAsyncKeyState(VK_RIGHT)&0x8000)
	{
		rad.y -= 0.05f;
	}
	//下移動
	if(GetAsyncKeyState(VK_DOWN)&0x8000 && !(GetAsyncKeyState(VK_SHIFT)&0x8000))
	{
		pos.y -= 0.05f;
	}
	//上移動
	if(GetAsyncKeyState(VK_UP)&0x8000 && !(GetAsyncKeyState(VK_SHIFT)&0x8000))
	{
		pos.y += 0.05f;
	}

	//拡大
	if(GetAsyncKeyState(VK_UP)&0x8000 && GetAsyncKeyState(VK_SHIFT)&0x8000)
	{
		range -= 0.05f;
	}
	//縮小
	if(GetAsyncKeyState(VK_DOWN)&0x8000 && GetAsyncKeyState(VK_SHIFT)&0x8000)
	{
		range += 0.05f;
	}

	pos.x = sinf(rad.y) * range;
	pos.z = cosf(rad.y) * range;
}
//--------------------
// Set()
// カメラのセット
//--------------------
void CCamera::Set()
{
	//ワールド/ビュー変換行列設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		pos.x,pos.y,pos.z,		//視点座標
		at.x,at.y,at.z,			//注視点座標
		up.x,up.y,up.z);		//アップベクトル
}