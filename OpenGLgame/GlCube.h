//=======================
// GlCube.h
//=======================

#ifndef __GLCUBE_H__
#define __GLCUBE_H__

class VECTOR
{
public:
	float x,y,z;
};

class CCube
{
private:
	GLfloat vertices[8][3];
public:
	CCube();
	~CCube();
	void CreateCube(float size,VECTOR pos);
	void DrawCube(GLuint g_uTexIDAlphabet,GLuint i);
};

#endif