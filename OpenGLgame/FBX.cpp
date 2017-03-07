// FBX.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "Texture.h"
#include "FBX.h"

// 文字列両端の二重引用符削除
char* trim(char* str)
{
	// 先頭の"を探す
	while (*str != '"' && *str) {
		str++;
	}
	if (*str == '\0') {
		return str;
	}
	// 先頭の"をスキップ
	str++;
	// 末尾の"を探す
	int i = strlen(str) - 1;
	while (str[i] != '"' && i > 0) {
		i--;
	}
	// 末尾の"を消す
	str[i] = '\0';

	return str;
}

// FBX ファイル読み込み
int LoadFBX(FBX* pFBX, char* pszPath)
{
	// FBX初期化
	if (pFBX == NULL || pszPath == NULL) {
		return -1;//引数異常
	}
	memset(pFBX, 0, sizeof(FBX));
	// FBXファイルを開く
	FILE* fp = fopen(pszPath, "rb");
	if (fp == NULL) {
		return -2;//ファイルが無い
	}
	// ファイルサイズ取得
	fseek(fp, 0L, SEEK_END);
	long lSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	// 読み込みメモリ確保
	char* psz = (char*)malloc(lSize + 1);
	if (psz == NULL) {
		fclose(fp);
		return -3;//メモリ不足
	}
	// 全体読み込み
	if (fread(psz, 1, lSize, fp) < (size_t)lSize) {
		free(psz);
		fclose(fp);
		return -4;//読み込みエラー
	}
	psz[lSize] = '\0';
	// FBXファイルを閉じる
	fclose(fp);
	// 解析開始
	VECTOR3 v;
	int phase = 0;
	int type;
	int nAlloc;
	int i;
	int nNrm;	// 法線ベクトル番号
	float fRadius;
	const char szSep[] = " ,:\t\r\n";
	char* token = strtok(psz, szSep);
	while (token) {
		switch (phase) {
		case 0:	// 頂点座標先頭を探す
			if (strcmp(token, "Vertices") == 0) {
				phase = 1;
				type = 0;
				nAlloc = 100;
				pFBX->pVtx = (FBXVECTOR*)malloc(sizeof(FBXVECTOR) * nAlloc);
				break;
			}
			break;
		case 1:	// 頂点座標開始
			if (*token != '-' && !isdigit(*token)) {
				break;				
			}
			phase = 2;
			// THRU
		case 2:	// 数字が続く間は頂点座標
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
						return -9;//メモリ不足
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
		case 3:	// インデックスの先頭を探す
INDEXSEARCH:
			if (strcmp(token, "PolygonVertexIndex") == 0) {
				phase = 4;
				nAlloc = 100;
				pFBX->pIdx = (short*)malloc(sizeof(short) * nAlloc);
				break;
			}
			break;
		case 4:	// インデックス開始
			if (*token != '-' && !isdigit(*token)) {
				break;				
			}
			phase = 5;
			// THRU
		case 5:	// 数字が続く間はインデックス
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
					return -10;//メモリ不足
				}
			}
			pFBX->pIdx[pFBX->nIdx] = (short)atoi(token);
			pFBX->nIdx++;
			break;
//		case 0:	// 頂点座標先頭を探す
		case 6:	// 法線ベクトル先頭を探す
//			if (strcmp(token, "Vertices") == 0) {
			if (strcmp(token, "Normals") == 0) {
//				phase = 1;
				phase = 7;
				pFBX->nNrmType++;//追加
				nNrm = 0;//追加
				type = 0;
				nAlloc = 100;
//				pFBX->pVtx = (FBXVECTOR*)malloc(sizeof(FBXVECTOR) * nAlloc);
				pFBX->pNrm = (FBXVECTOR*)malloc(sizeof(FBXVECTOR) * nAlloc);
				break;
			}
			/*追加*/
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
//		case 1:	// 頂点座標開始
		case 7:	// 法線ベクトル開始
			if (*token != '-' && !isdigit(*token)) {
				break;				
			}
//			phase = 2;
			phase = 8;
			// THRU
//		case 2:	// 数字が続く間は頂点座標
		case 8:	// 数字が続く間は法線ベクトル
			if (*token != '-' && !isdigit(*token)) {
				/*追加*/
				switch (pFBX->nNrmType) {
				case NRM_VTX:	// 頂点毎
					if (nNrm != pFBX->nVtx) {
						free(psz);
						return -13;	//法線ベクトル数異常
					}
					break;
				case NRM_IDX:	// インデックス毎
					if (nNrm != pFBX->nIdx) {
						free(psz);
						return -12;	//法線ベクトル数異常
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
						return -9;//メモリ不足
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

		case 9:	// UV座標先頭を探す
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
		case 10:	// UV座標開始
			if (*token != '-' && !isdigit(*token)) {
				break;				
			}
			phase = 11;
			// THRU
		case 11:	// 数字が続く間はUV座標
			if (*token != '-' && !isdigit(*token)) {
//				switch (pFBX->nUVType) {
//				case NRM_VTX:	// 頂点毎
//					if (pFBX->nUV != pFBX->nVtx) {
//						free(psz);
//						return -13;	//UV座標数異常
//					}
//					break;
//				case NRM_IDX:	// インデックス毎
//					if (pFBX->nUV != pFBX->nIdx) {
//						free(psz);
//						return -12;	//UV座標数異常
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
						return -9;//メモリ不足
					}
				}
			}
			pFBX->pUV[pFBX->nUV].p[type] = (GLfloat)atof(token);
			type = (type + 1) % 2;
			if (type == 0) {
				pFBX->nUV++;
			}
			break;
		case 15:	// UVインデックスの先頭を探す
UVIDXSEARCH:
			if (strcmp(token, "UVIndex") == 0) {
				phase = 16;
				nAlloc = 100;
				pFBX->nUVIdx = 0;
				pFBX->pUVIdx = (short*)malloc(sizeof(short) * nAlloc);
				break;
			}
			break;
		case 16:	// UVインデックス開始
			if (*token != '-' && !isdigit(*token)) {
				break;				
			}
			phase = 17;
			// THRU
		case 17:	// 数字が続く間はUVインデックス
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
					return -10;//メモリ不足
				}
			}
			pFBX->pUVIdx[pFBX->nUVIdx] = (short)atoi(token);
			pFBX->nUVIdx++;
			break;
		case 12:	// テクスチャ開始
			if (strcmp(token, "Texture") == 0) {
				phase = 13;
				break;
			}
			break;
		case 13:	// テクスチャ ファイル名開始
			if (strcmp(token, "FileName") == 0) {
				phase = 14;
				break;
			}
			break;
		case 14:	// テクスチャ読み込み
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

	// ワールドマトリックス セット
	glPushMatrix();
	glMultMatrixf(matWorld[0]);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);//白
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
// 環境光に対応する色
GLfloat ambient[4] = {0.3f, 0.3f, 0.3f, 1.0f};
glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
// 拡散光に対応する色
GLfloat diffuse[4] = {(float)((color >> 2) & 1),
					  (float)((color >> 1) & 1),
					  (float)(color & 1), 1.0f};
glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
// 自身が発光する色 (対応する光源色無し)
GLfloat emissive[4] = {0.0f, 0.0f, 0.0f, 1.0f};
glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
// 鏡面反射部分の色
GLfloat specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
// 鏡面反射の鋭さ
glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);
			//color = ((color + 1) % 7) + 1;	// 1～7の値
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
	// 法線ベクトルを表示
	extern bool g_bNormal;
	if (g_bNormal && pFBX->nNrmType != NRM_NONE) {
	VECTOR3 vN;
	glDisable(GL_LIGHTING);		//ライティング無効化
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);//黄色
	glBegin(GL_LINES);
	switch (pFBX->nNrmType) {
	case NRM_VTX:	// 頂点毎
		for (int i = 0; i < pFBX->nVtx; i++) {
			glVertex3fv(pFBX->pVtx[i].p);
			Vec3Scale(vN, pFBX->pNrm[i].p, pFBX->fRadius / 16.0f);
			glVertex3f(pFBX->pVtx[i].v.x + vN[0], pFBX->pVtx[i].v.y + vN[1], pFBX->pVtx[i].v.z + vN[2]);
		}
		break;
	case NRM_IDX:	// インデックス毎
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
	glEnable(GL_LIGHTING);		//ライティング有効化
	}

	// ビューマトリックスを復帰
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
	if (pFBX->pNrm) {		//追加
		free(pFBX->pNrm);	//追加
		pFBX->pNrm = NULL;	//追加
	}						//追加
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
