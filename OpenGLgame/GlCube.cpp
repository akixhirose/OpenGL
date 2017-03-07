//====================
// GlCube.cpp
//====================
//--------------------
// インクルード
//--------------------
#include <windows.h>		// Windows プログラムにはこれを付ける
#include <GL/gl.h>			// OpenGL 関連のヘッダ
#include <GL/glu.h>			// OpenGL 関連のヘッダ
#define _USE_MATH_DEFINES	// math.h定義の定数を使用
#include <math.h>			// 三角関数、円周率で使用
#include "Texture.h"
#include "GlCube.h"
//--------------------
// CCubeコンストラクタ
//--------------------
CCube::CCube()
{
//	CreateCube(0.5f);
}
//--------------------
// CCubeデストラクタ
//--------------------
CCube::~CCube()
{
}
//--------------------
// InitCube関数
// 初期化処理を行う
//--------------------
void CCube::CreateCube(float size,VECTOR pos)
{
	//配列を用意する
	vertices[0][0] = -size + pos.x;
	vertices[0][1] = -size + pos.y;
	vertices[0][2] = size + pos.z;

	vertices[1][0] = size + pos.x;
	vertices[1][1] = -size + pos.y;
	vertices[1][2] = size + pos.z;

	vertices[2][0] = size + pos.x;
	vertices[2][1] = size + pos.y;
	vertices[2][2] = size + pos.z;

	vertices[3][0] = -size + pos.x;
	vertices[3][1] = size + pos.y;
	vertices[3][2] = size + pos.z;

	vertices[4][0] = size + pos.x;
	vertices[4][1] = -size + pos.y;
	vertices[4][2] = -size + pos.z;

	vertices[5][0] = -size + pos.x;
	vertices[5][1] = -size + pos.y;
	vertices[5][2] = -size + pos.z;

	vertices[6][0] = -size + pos.x;
	vertices[6][1] = size + pos.y;
	vertices[6][2] = -size + pos.z;

	vertices[7][0] = size + pos.x;
	vertices[7][1] = size + pos.y;
	vertices[7][2] = -size + pos.z;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
//--------------------
// DrawCube関数
// 立方体描画する
//--------------------
void CCube::DrawCube(GLuint g_uTexIDAlphabet,GLuint i)
{
	/* 前 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[1]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[3]);
	glEnd();

	
	switch(i)
	{
	case 0:
	glBindTexture(GL_TEXTURE_2D,g_uTexIDAlphabet);
	glEnable(GL_TEXTURE_2D);
	/* 後 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f,0.0f);
		glVertex3fv(vertices[4]);
		glTexCoord2f(1.0f,0.0f);
		glVertex3fv(vertices[5]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3fv(vertices[6]);
		glTexCoord2f(0.0f,1.0f);
		glVertex3fv(vertices[7]);
	glEnd();
	
	/* 右 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[4]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[2]);
	glEnd();

	/* 左 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[0]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[6]);
	glEnd();

	/* 上 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[6]);
	glEnd();

	/* 下 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[4]);
	glEnd();
	break;

	case 1:
	/* 後 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[4]);
		glVertex3fv(vertices[5]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[7]);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D,g_uTexIDAlphabet);
	glEnable(GL_TEXTURE_2D);
	/* 右 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f,0.0f);
		glVertex3fv(vertices[1]);
		glTexCoord2f(1.0f,0.0f);
		glVertex3fv(vertices[4]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3fv(vertices[7]);
		glTexCoord2f(0.0f,1.0f);
		glVertex3fv(vertices[2]);
	glEnd();

	/* 左 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[0]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[6]);
	glEnd();

	/* 上 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[6]);
	glEnd();

	/* 下 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[4]);
	glEnd();
		break;

	case 2:
	/* 後 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[4]);
		glVertex3fv(vertices[5]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[7]);
	glEnd();
	
	/* 右 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[4]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[2]);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,g_uTexIDAlphabet);
	glEnable(GL_TEXTURE_2D);
	/* 左 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f,0.0f);
		glVertex3fv(vertices[5]);
		glTexCoord2f(1.0f,0.0f);
		glVertex3fv(vertices[0]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3fv(vertices[3]);
		glTexCoord2f(0.0f,1.0f);
		glVertex3fv(vertices[6]);
	glEnd();

	/* 上 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[6]);
	glEnd();

	/* 下 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[4]);
	glEnd();
		break;

	case 3:
	/* 後 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[4]);
		glVertex3fv(vertices[5]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[7]);
	glEnd();
	
	/* 右 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[4]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[2]);
	glEnd();

	/* 左 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[0]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[6]);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,g_uTexIDAlphabet);
	glEnable(GL_TEXTURE_2D);
	/* 上 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f,0.0f);
		glVertex3fv(vertices[3]);
		glTexCoord2f(0.0f,1.0f);
		glVertex3fv(vertices[2]);
		glTexCoord2f(1.0f,1.0f);
		glVertex3fv(vertices[7]);
		glTexCoord2f(1.0f,0.0f);
		glVertex3fv(vertices[6]);
	glEnd();

	/* 下 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[4]);
	glEnd();
		break;

	case 4:
	/* 後 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[4]);
		glVertex3fv(vertices[5]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[7]);
	glEnd();
	
	/* 右 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[4]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[2]);
	glEnd();

	/* 左 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[0]);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[6]);
	glEnd();

	/* 上 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[6]);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D,g_uTexIDAlphabet);
	glEnable(GL_TEXTURE_2D);
	/* 下 */
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f,0.0f);
		glVertex3fv(vertices[1]);
		glTexCoord2f(1.0f,0.0f);
		glVertex3fv(vertices[0]);
		glTexCoord2f(1.0f,1.0f);
		glVertex3fv(vertices[5]);
		glTexCoord2f(0.0f,1.0f);
		glVertex3fv(vertices[4]);
	glEnd();
		break;
	}
	glFlush();

}