#ifndef _H_TEXTURE
#define _H_TEXTURE

#include <windows.h>		// Windows 関連のヘッダ
#include <GL/gl.h>			// OpenGL 関連のヘッダ
#include <GL/glu.h>			// OpenGL 関連のヘッダ

GLuint LoadTexture(const char *pszFName);
void ReleaseTexture(GLuint uTexID);

//-------- ライブラリのリンク
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#endif	// _H_TEXTURE