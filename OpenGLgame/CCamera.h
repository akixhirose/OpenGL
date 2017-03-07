//=======================
// CCamera.h
//=======================

#ifndef __CCAMERA_H__
#define __CCAMERA_H__

class CamVec3
{
public:
	float x,y,z;
};

class CCamera
{
private:
	CamVec3 pos;	//�ʒu
	CamVec3 at;		//�����_
	CamVec3 up;		//�A�b�v�x�N�g��

	CamVec3 rad;	//��]�v�Z�p
	float range;	//�����v�Z�p

public:
	CCamera();
	~CCamera();

	void Init(void);
	void Move(void);
	void Set(void);
};


#endif