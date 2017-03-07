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
	CamVec3 pos;	//位置
	CamVec3 at;		//注視点
	CamVec3 up;		//アップベクトル

	CamVec3 rad;	//回転計算用
	float range;	//距離計算用

public:
	CCamera();
	~CCamera();

	void Init(void);
	void Move(void);
	void Set(void);
};


#endif