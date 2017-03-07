// FBX.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "Texture.h"
#include "FBX.h"

// �����񗼒[�̓�d���p���폜
char* trim(char* str)
{
	// �擪��"��T��
	while (*str != '"' && *str) {
		str++;
	}
	if (*str == '\0') {
		return str;
	}
	// �擪��"���X�L�b�v
	str++;
	// ������"��T��
	int i = strlen(str) - 1;
	while (str[i] != '"' && i > 0) {
		i--;
	}
	// ������"������
	str[i] = '\0';

	return str;
}

// FBX �t�@�C���ǂݍ���
int LoadFBX(FBX* pFBX, char* pszPath)
{
	// FBX������
	if (pFBX == NULL || pszPath == NULL) {
		return -1;//�����ُ�
	}
	memset(pFBX, 0, sizeof(FBX));
	// FBX�t�@�C�����J��
	FILE* fp = fopen(pszPath, "rb");
	if (fp == NULL) {
		return -2;//�t�@�C��������
	}
	// �t�@�C���T�C�Y�擾
	fseek(fp, 0L, SEEK_END);
	long lSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	// �ǂݍ��݃������m��
	char* psz = (char*)malloc(lSize + 1);
	if (psz == NULL) {
		fclose(fp);
		return -3;//�������s��
	}
	// �S�̓ǂݍ���
	if (fread(psz, 1, lSize, fp) < (size_t)lSize) {
		free(psz);
		fclose(fp);
		return -4;//�ǂݍ��݃G���[
	}
	psz[lSize] = '\0';
	// FBX�t�@�C�������
	fclose(fp);
	// ��͊J�n
	VECTOR3 v;
	int phase = 0;
	int type;
	int nAlloc;
	int i;
	int nNrm;	// �@���x�N�g���ԍ�
	float fRadius;
	const char szSep[] = " ,:\t\r\n";
	char* token = strtok(psz, szSep);
	while (token) {
		switch (phase) {
		case 0:	// ���_���W�擪��T��
			if (strcmp(token, "Vertices") == 0) {
				phase = 1;
				type = 0;
				nAlloc = 100;
				pFBX->pVtx = (FBXVECTOR*)malloc(sizeof(FBXVECTOR) * nAlloc);
				break;
			}
			break;
		case 1:	// ���_���W�J�n
			if (*token != '-' && !isdigit(*token)) {
				break;				
			}
			phase = 2;
			// THRU
		case 2:	// �����������Ԃ͒��_���W
			if (*token != '-' && !isdigit(*token)) {
				for (i = 0; i < 3; i++) {
					Vec3Add(v, pFBX->vMin.p, pFBX->vMax.p);
					Vec3Scale(pFBX->vCenter.p, v, 0.5f);
				}
				Vec3Subtract(v, pFBX->pVtx[0].p, pFBX->vCenter.p);
				pFBX->fRadius = Vec3Length(v);
				for (i = 1; i < pFBX->nVtx; i++) {
					Vec3Subtract(v, pFBX->pVtx[i].p, pFBX->vCenter.p);
					fRadius = Vec3Length(v);
					if (pFBX->fRadius < fRadius) {
						pFBX->fRadius = fRadius;
					}
				}
				phase = 3;
				goto INDEXSEARCH;				
			}
			if (type == 0) {
				if (pFBX->nVtx >= nAlloc) {
					nAlloc += 100;
					FBXVECTOR* pNew = (FBXVECTOR*)realloc(pFBX->pVtx, sizeof(FBXVECTOR) * nAlloc);
					if (pNew)
						pFBX->pVtx = pNew;
					else {
						free(psz);
						return -9;//�������s��
					}
				}
			}
			pFBX->pVtx[pFBX->nVtx].p[type] = (GLfloat)atof(token);
			if (pFBX->nVtx > 0) {
				if (pFBX->vMin.p[type] > pFBX->pVtx[pFBX->nVtx].p[type]) {
					pFBX->vMin.p[type] = pFBX->pVtx[pFBX->nVtx].p[type];
				}
				if (pFBX->vMax.p[type] < pFBX->pVtx[pFBX->nVtx].p[type]) {
					pFBX->vMax.p[type] = pFBX->pVtx[pFBX->nVtx].p[type];
				}
			} else {
				pFBX->vMin.p[type] = pFBX->pVtx[pFBX->nVtx].p[type];
				pFBX->vMax.p[type] = pFBX->vMin.p[type];
			}
			type = (type + 1) % 3;
			if (type == 0) {
				pFBX->nVtx++;
			}
			break;
		case 3:	// �C���f�b�N�X�̐擪��T��
INDEXSEARCH:
			if (strcmp(token, "PolygonVertexIndex") == 0) {
				phase = 4;
				nAlloc = 100;
				pFBX->pIdx = (short*)malloc(sizeof(short) * nAlloc);
				break;
			}
			break;
		case 4:	// �C���f�b�N�X�J�n
			if (*token != '-' && !isdigit(*token)) {
				break;				
			}
			phase = 5;
			// THRU
		case 5:	// �����������Ԃ̓C���f�b�N�X
			if (*token != '-' && !isdigit(*token)) {
				phase = 6;
				break;
			}
			if (pFBX->nIdx >= nAlloc) {
				nAlloc += 100;
				short* pNew = (short*)realloc(pFBX->pIdx, sizeof(short) * nAlloc);
				if (pNew)
					pFBX->pIdx = pNew;
				else {
					free(psz);
					return -10;//�������s��
				}
			}
			pFBX->pIdx[pFBX->nIdx] = (short)atoi(token);
			pFBX->nIdx++;
			break;
//		case 0:	// ���_���W�擪��T��
		case 6:	// �@���x�N�g���擪��T��
//			if (strcmp(token, "Vertices") == 0) {
			if (strcmp(token, "Normals") == 0) {
//				phase = 1;
				phase = 7;
				pFBX->nNrmType++;//�ǉ�
				nNrm = 0;//�ǉ�
				type = 0;
				nAlloc = 100;
//				pFBX->pVtx = (FBXVECTOR*)malloc(sizeof(FBXVECTOR) * nAlloc);
				pFBX->pNrm = (FBXVECTOR*)malloc(sizeof(FBXVECTOR) * nAlloc);
				break;
			}
			/*�ǉ�*/
			if (strcmp(token, "MappingInformationType") == 0) {
				phase = 61;
				break;
			}
			break;
		case 61:// MappingInformationType
			if (strcmp(token, "\"ByVertice\"") == 0) {
				pFBX->nNrmType = 0;
				phase = 6;
				break;
			}
			if (strcmp(token, "\"ByPolygonVertex\"") == 0) {
				pFBX->nNrmType = 1;
				phase = 6;
				break;
			}
			break;
//		case 1:	// ���_���W�J�n
		case 7:	// �@���x�N�g���J�n
			if (*token != '-' && !isdigit(*token)) {
				break;				
			}
//			phase = 2;
			phase = 8;
			// THRU
//		case 2:	// �����������Ԃ͒��_���W
		case 8:	// �����������Ԃ͖@���x�N�g��
			if (*token != '-' && !isdigit(*token)) {
				/*�ǉ�*/
				switch (pFBX->nNrmType) {
				case NRM_VTX:	// ���_��
					if (nNrm != pFBX->nVtx) {
						free(psz);
						return -13;	//�@���x�N�g�����ُ�
					}
					break;
				case NRM_IDX:	// �C���f�b�N�X��
					if (nNrm != pFBX->nIdx) {
						free(psz);
						return -12;	//�@���x�N�g�����ُ�
					}
					break;
				}
//				for (i = 0; i < 3; i++) {
//					Vec3Add(v, pFBX->vMin.p, pFBX->vMax.p);
//					Vec3Scale(pFBX->vCenter.p, v, 0.5f);
//				}
//				Vec3Subtract(v, pFBX->pVtx[0].p, pFBX->vCenter.p);
//				pFBX->fRadius = Vec3Length(v);
//				for (i = 1; i < pFBX->nVtx; i++) {
//					Vec3Subtract(v, pFBX->pVtx[i].p, pFBX->vCenter.p);
//					fRadius = Vec3Length(v);
//					if (pFBX->fRadius < fRadius) {
//						pFBX->fRadius = fRadius;
//					}
//				}
//				phase = 3;
				phase = 9;
//				goto INDEXSEARCH;				
				break;
			}
			if (type == 0) {
//				if (pFBX->nVtx >= nAlloc) {
				if (nNrm >= nAlloc) {
					nAlloc += 100;
//					FBXVECTOR* pNew = (FBXVECTOR*)realloc(pFBX->pVtx, sizeof(FBXVECTOR) * nAlloc);
					FBXVECTOR* pNew = (FBXVECTOR*)realloc(pFBX->pNrm, sizeof(FBXVECTOR) * nAlloc);
					if (pNew)
//						pFBX->pVtx = pNew;
						pFBX->pNrm = pNew;
					else {
						free(psz);
						return -9;//�������s��
					}
				}
			}
//			pFBX->pVtx[pFBX->nVtx].p[type] = (GLfloat)atof(token);
			pFBX->pNrm[nNrm].p[type] = (GLfloat)atof(token);
//			if (pFBX->nVtx > 0) {
//				if (pFBX->vMin.p[type] > pFBX->pVtx[pFBX->nVtx].p[type]) {
//					pFBX->vMin.p[type] = pFBX->pVtx[pFBX->nVtx].p[type];
//				}
//				if (pFBX->vMax.p[type] < pFBX->pVtx[pFBX->nVtx].p[type]) {
//					pFBX->vMax.p[type] = pFBX->pVtx[pFBX->nVtx].p[type];
//				}
//			} else {
//				pFBX->vMin.p[type] = pFBX->pVtx[pFBX->nVtx].p[type];
//				pFBX->vMax.p[type] = pFBX->vMin.p[type];
//			}
			type = (type + 1) % 3;
			if (type == 0) {
//				pFBX->nVtx++;
				nNrm++;
			}
			break;

		case 9:	// UV���W�擪��T��
			if (strcmp(token, "UV") == 0) {
				pFBX->nUVType++;
				pFBX->nUV = 0;
				type = 0;
				nAlloc = 100;
				phase = 10;
				pFBX->pUV = (FBXVECTOR2*)malloc(sizeof(FBXVECTOR2) * nAlloc);
				break;
			}
			if (strcmp(token, "MappingInformationType") == 0) {
				phase = 91;
				break;
			}
			break;
		case 91:// MappingInformationType
			if (strcmp(token, "\"ByVertice\"") == 0) {
				pFBX->nUVType = 0;
				phase = 8;
				break;
			}
			if (strcmp(token, "\"ByPolygonVertex\"") == 0) {
				pFBX->nUVType = 1;
				phase = 8;
				break;
			}
			break;
		case 10:	// UV���W�J�n
			if (*token != '-' && !isdigit(*token)) {
				break;				
			}
			phase = 11;
			// THRU
		case 11:	// �����������Ԃ�UV���W
			if (*token != '-' && !isdigit(*token)) {
//				switch (pFBX->nUVType) {
//				case NRM_VTX:	// ���_��
//					if (pFBX->nUV != pFBX->nVtx) {
//						free(psz);
//						return -13;	//UV���W���ُ�
//					}
//					break;
//				case NRM_IDX:	// �C���f�b�N�X��
//					if (pFBX->nUV != pFBX->nIdx) {
//						free(psz);
//						return -12;	//UV���W���ُ�
//					}
//					break;
//				}
				phase = 15;
//				break;
				goto UVIDXSEARCH;
			}
			if (type == 0) {
				if (pFBX->nUV >= nAlloc) {
					nAlloc += 100;
					FBXVECTOR2* pNew = (FBXVECTOR2*)realloc(pFBX->pUV, sizeof(FBXVECTOR2) * nAlloc);
					if (pNew)
						pFBX->pUV = pNew;
					else {
						free(psz);
						return -9;//�������s��
					}
				}
			}
			pFBX->pUV[pFBX->nUV].p[type] = (GLfloat)atof(token);
			type = (type + 1) % 2;
			if (type == 0) {
				pFBX->nUV++;
			}
			break;
		case 15:	// UV�C���f�b�N�X�̐擪��T��
UVIDXSEARCH:
			if (strcmp(token, "UVIndex") == 0) {
				phase = 16;
				nAlloc = 100;
				pFBX->nUVIdx = 0;
				pFBX->pUVIdx = (short*)malloc(sizeof(short) * nAlloc);
				break;
			}
			break;
		case 16:	// UV�C���f�b�N�X�J�n
			if (*token != '-' && !isdigit(*token)) {
				break;				
			}
			phase = 17;
			// THRU
		case 17:	// �����������Ԃ�UV�C���f�b�N�X
			if (*token != '-' && !isdigit(*token)) {
				phase = 12;
				break;
			}
			if (pFBX->nUVIdx >= nAlloc) {
				nAlloc += 100;
				short* pNew = (short*)realloc(pFBX->pUVIdx, sizeof(short) * nAlloc);
				if (pNew)
					pFBX->pUVIdx = pNew;
				else {
					free(psz);
					return -10;//�������s��
				}
			}
			pFBX->pUVIdx[pFBX->nUVIdx] = (short)atoi(token);
			pFBX->nUVIdx++;
			break;
		case 12:	// �e�N�X�`���J�n
			if (strcmp(token, "Texture") == 0) {
				phase = 13;
				break;
			}
			break;
		case 13:	// �e�N�X�`�� �t�@�C�����J�n
			if (strcmp(token, "FileName") == 0) {
				phase = 14;
				break;
			}
			break;
		case 14:	// �e�N�X�`���ǂݍ���
			pFBX->uTexture = LoadTexture(trim(token));
			phase = 18;
			break;

		default:
			break;
		}
		token = strtok(NULL, szSep);
	}
	free(psz);
	return 0;
}

void DrawFBX(FBX* pFBX, MATRIX matWorld)
{
	DWORD dwCtrl = GetAsyncKeyState(VK_CONTROL) & 0x8000;
	if (dwCtrl) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// ���[���h�}�g���b�N�X �Z�b�g
	glPushMatrix();
	glMultMatrixf(matWorld[0]);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);//��
	if (pFBX->uTexture != 0) {
		glBindTexture(GL_TEXTURE_2D, pFBX->uTexture);
		glEnable(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}

	int nStep = 0;
	int color = 7;
	short* pIdx = pFBX->pIdx;
	for (int i = 0; i < pFBX->nIdx; i++, pIdx++) {
		if (nStep == 0) {
//			glColor4f((float)((color >> 2) & 1),
//				(float)((color >> 1) & 1),
//				(float)(color & 1), 1.0f);
// �����ɑΉ�����F
GLfloat ambient[4] = {0.3f, 0.3f, 0.3f, 1.0f};
glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
// �g�U���ɑΉ�����F
GLfloat diffuse[4] = {(float)((color >> 2) & 1),
					  (float)((color >> 1) & 1),
					  (float)(color & 1), 1.0f};
glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
// ���g����������F (�Ή���������F����)
GLfloat emissive[4] = {0.0f, 0.0f, 0.0f, 1.0f};
glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
// ���ʔ��˕����̐F
GLfloat specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
// ���ʔ��˂̉s��
glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);
			//color = ((color + 1) % 7) + 1;	// 1�`7�̒l
			glBegin(GL_POLYGON);
		}
		short s = *pIdx;
		if (s < 0) {
			s = ~s;
		}
		switch (pFBX->nNrmType) {
		case NRM_VTX:
			glNormal3fv(pFBX->pNrm[s].p);
			break;
		case NRM_IDX:
			glNormal3fv(pFBX->pNrm[i].p);
			break;
		}
		if (pFBX->uTexture && pFBX->pUV) {
			if (pFBX->pUVIdx) {
				glTexCoord2fv(pFBX->pUV[pFBX->pUVIdx[i]].p);
			} else {
				glTexCoord2fv(pFBX->pUV[s].p);
			}
		}
		glVertex3fv(pFBX->pVtx[s].p);
		nStep++;
		if (*pIdx < 0) {
			glEnd();
			nStep = 0;
		}
	}
/*
	// �@���x�N�g����\��
	extern bool g_bNormal;
	if (g_bNormal && pFBX->nNrmType != NRM_NONE) {
	VECTOR3 vN;
	glDisable(GL_LIGHTING);		//���C�e�B���O������
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);//���F
	glBegin(GL_LINES);
	switch (pFBX->nNrmType) {
	case NRM_VTX:	// ���_��
		for (int i = 0; i < pFBX->nVtx; i++) {
			glVertex3fv(pFBX->pVtx[i].p);
			Vec3Scale(vN, pFBX->pNrm[i].p, pFBX->fRadius / 16.0f);
			glVertex3f(pFBX->pVtx[i].v.x + vN[0], pFBX->pVtx[i].v.y + vN[1], pFBX->pVtx[i].v.z + vN[2]);
		}
		break;
	case NRM_IDX:	// �C���f�b�N�X��
		for (int i = 0; i < pFBX->nIdx; i++) {
			short s = pFBX->pIdx[i];
			if (s < 0) {
				s = ~s;
			}
			glVertex3fv(pFBX->pVtx[s].p);
			Vec3Scale(vN, pFBX->pNrm[i].p, pFBX->fRadius / 16.0f);
			glVertex3f(pFBX->pVtx[s].v.x + vN[0], pFBX->pVtx[s].v.y + vN[1], pFBX->pVtx[s].v.z + vN[2]);
		}
		break;
	default:
		break;
	}
	glEnd();
	glEnable(GL_LIGHTING);		//���C�e�B���O�L����
	}

	// �r���[�}�g���b�N�X�𕜋A
	glPopMatrix();

	if (dwCtrl) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}*/
}

void ReleaseFBX(FBX* pFBX)
{
	if (!pFBX) return;
	ReleaseTexture(pFBX->uTexture);
	pFBX->uTexture = 0;
	if (pFBX->pUVIdx) {
		free(pFBX->pUVIdx);
		pFBX->pUVIdx = NULL;
	}
	if (pFBX->pUV) {
		free(pFBX->pUV);
		pFBX->pUV = NULL;
	}
	if (pFBX->pNrm) {		//�ǉ�
		free(pFBX->pNrm);	//�ǉ�
		pFBX->pNrm = NULL;	//�ǉ�
	}						//�ǉ�
	if (pFBX->pVtx) {
		free(pFBX->pVtx);
		pFBX->pVtx = NULL;
	}
	pFBX->nVtx = 0;
	if (pFBX->pIdx) {
		free(pFBX->pIdx);
		pFBX->pIdx = NULL;
	}
	pFBX->nIdx = 0;
}
