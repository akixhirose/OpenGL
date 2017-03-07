//====================================================================
//    [ OGLbase ] OpenGL
//====================================================================
//-------- �C���N���[�h�w�b�_
#define STRICT
#include <windows.h>		// Windows �v���O�����ɂ͂����t����
#include <tchar.h>			// �ėp�e�L�X�g �}�b�s���O
#include <GL/gl.h>			// OpenGL �֘A�̃w�b�_
#include <GL/glu.h>			// OpenGL �֘A�̃w�b�_
#include <stdio.h>			// �����񏈗��Ŏg�p
#include <mmsystem.h>		// ���ԊǗ��Ŏg�p
#define _USE_MATH_DEFINES	// math.h��`�̒萔���g�p
#include <math.h>			// �O�p�֐��A�~�����Ŏg�p

#include "FBX.h"
#include "Texture.h"

#include "CCamera.h"		//�J�����N���X
#include "GlCube.h"			//�����̃N���X

//-------- ���C�u�����̃����N�i�����L�q���Ă����Εʓr�����N�ݒ肪�s�v�ƂȂ�j
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

//-------- �萔��`
#define CLASS_NAME		_T("OGLbase")						// �E�B���h�E�N���X��
#define CAPTION_NAME	_T("[ OGLGame ]")	// �L���v�V������

#define FRAME_RATE			(1000/60)						// �t���[�����[�g
#define SCREEN_WIDTH		640								// �X�N���[���̕�
#define SCREEN_HEIGHT		480								// �X�N���[���̍���
#define FOVY				45.0f							// ����p
#define NEAR_CLIP			1.0f							// �j�A�N���b�v���s������
#define FAR_CLIP			10000.0f						// �t�@�[�N���b�v���s������

#define RAD					(float)(M_PI/180.0)				// ���W�A���ϊ��p�萔

#define FONT_WIDTH			10								// �t�H���g��
#define FONT_HEIGHT			16								// �t�H���g����

//----- ���_�t�H�[�}�b�g��`
struct FVERTEX {
	// GL_T2F_V3F
	GLfloat		tu, tv;			// �e�N�X�`�����W
	GLfloat		x, y, z;		// ���_���W
};

//----- �O���[�o���ϐ�
TCHAR		g_szDebug[4096];		// �o�͕�����o�b�t�@
float		g_fFPS;					// �t���[�����J�E���g�p

// OpenGL �֘A
HDC			g_hDC;					// �f�o�C�X�R���e�L�X�g
HGLRC		g_hRC;					// �J�����g�R���e�L�X�g
bool		g_bWindow;				// �E�B���h�E���[�h
GLuint		g_uFontTexID;			// �t�H���g�e�N�X�`�� ID
GLfloat		g_MatView[4][4];		// �r���[ �}�g���b�N�X
GLfloat		g_MatProj[4][4];		// �v���W�F�N�V���� �}�g���b�N�X

//���f��
FBX g_fbxModel;
MATRIX g_mtxModel;

FBX g_fbxModelLand;
MATRIX g_mtxModelLand;


typedef enum {
	INIT = 0,
	TITLE,
	GAME,
	GAMECLEAR,
	GAMEOVER,
	MAX_STATE
}eState;
eState g_status = INIT;

GLuint nAlphabet = 0;
GLuint g_nI = 0;

CCube g_cube;
CCamera g_camera;
VECTOR pos;
GLuint	g_uTexIDTitle;	//�^�C�g���p�e�N�X�`��
GLuint	g_uTexIDGameOver;	//�Q�[���I�[�o�[�p�e�N�X�`��
GLuint	g_uTexIDGameClear;	//�Q�[���N���A�p�e�N�X�`��
GLuint	g_uTexIDAlphabet;	//�A���t�@�x�b�g�p�e�N�X�`��

//----- �v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
long InitializeGraphics(HWND hWnd, bool bWindow);
HRESULT CleanupGraphics(HWND hWnd, bool bWindow);
HRESULT RenderGraphics();
void Init2D();
void Init3D();
GLuint LoadTexture(HWND hWnd, LPCSTR pszFName);
void ReleaseTexture(GLuint* puTexID);
void DrawChar(int x, int y, int c);
void DrawStr(int x, int y, LPCTSTR psz);
void GameMain();
void DrawMain();

void LoadAlphabetTex();

//-------------------------------------------------------------------------------------------------
// ���C��
//-------------------------------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int iCmdShow)
{
	HWND	hWnd;
	MSG		msg;
	DWORD	dwExecLastTime, dwFPSLastTime, dwCurrentTime, dwFrameCount;

	// �E�B���h�E �N���X��o�^
	WNDCLASS wndClass = {
		CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst, LoadIcon(hInst, IDI_APPLICATION),
		LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(WHITE_BRUSH), NULL, CLASS_NAME
	};
	if (RegisterClass(&wndClass) == 0) return false;

	// �E�B���h�E �T�C�Y���擾
	RECT rc = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	DWORD dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	DWORD dwExStyle = 0;
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

	// �E�B���h�E���쐬
	hWnd = CreateWindowEx(dwExStyle, CLASS_NAME, CAPTION_NAME, dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, hInst, NULL);
	if (hWnd == NULL) return false;

	g_bWindow = false;
	if (IDYES == MessageBox(hWnd, _T("�E�B���h�E ���[�h�Ŏ��s���܂����H"), _T("��ʃ��[�h"), MB_YESNO))
		g_bWindow = true;
	if (FAILED(InitializeGraphics(hWnd, g_bWindow))) return 0;	// OpenGL �̏�����

	// �ϐ�������
	timeBeginPeriod(1);									// �^�C�}�̕���\���ŏ��ɃZ�b�g
	dwFPSLastTime = dwExecLastTime = timeGetTime();		// ���݂̃V�X�e�� �^�C�}���擾
	dwExecLastTime -= FRAME_RATE;
	dwFrameCount = 0;
	g_fFPS = 0.0f;

	// ���C�� �E�C���h�E ���[�v
	msg.message = WM_NULL;
	while (WM_QUIT != msg.message) {						// WM_QUIT ������܂Ń��[�v
		if (PeekMessage(&msg,NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			dwCurrentTime = timeGetTime();					// ���݂̃^�C�}�l���擾
			if (dwCurrentTime - dwFPSLastTime >= 500) {		// 0.5 �b���ƂɌv��
				// �t���[�������v�Z
				g_fFPS = dwFrameCount * 1000.0f / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;	// �^�C�}�l���X�V
				dwFrameCount = 0;				// �t���[�� �J�E���^�����Z�b�g
			}
			// ���̕ӂŎ��ԊǗ�
			while (dwCurrentTime - dwExecLastTime >= FRAME_RATE) {	// ��莞�Ԃ��o�߂�����E�E�E
				dwExecLastTime += FRAME_RATE;						// �^�C�}�l���X�V
				GameMain();											// �Q�[�� ���C������
			}
			RenderGraphics();								// �����_�����O���������s
			dwFrameCount++;									// �t���[�� �J�E���g���{�P
			Sleep(1);										// �������� Windows �ɐ����߂�
		}
	}
	timeEndPeriod(1);						// �V�X�e�� �^�C�}�̕���\�����ɖ߂�
	return (int)msg.wParam;
}

//-------------------------------------------------------------------------------------------------
// ���b�Z�[�W �n���h��
//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CLOSE:
		CleanupGraphics(hWnd, g_bWindow);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			return 0;
		}
		break;
	case WM_IME_NOTIFY:
		return 0;
		break;
	case WM_SETCURSOR:
//		if(){
			SetCursor(NULL);
			return 0;
			break;
//		}
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
// OpenGL �̏�����
//-------------------------------------------------------------------------------------------------
HRESULT InitializeGraphics(HWND hWnd, bool bWindow)
{
	// �s�N�Z�� �t�H�[�}�b�g
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,						// Version
		PFD_DRAW_TO_WINDOW |	// Supports GDI
		PFD_SUPPORT_OPENGL |	// Supports OpenGL
		PFD_DOUBLEBUFFER,		// Use double buffering (more efficient drawing)
		PFD_TYPE_RGBA,			// No pallettes
		24, 					// Number of color planes
		 						// in each color buffer
		8,	0,					// for Red-component
		8,	0,					// for Green-component
		8,	0,					// for Blue-component
		8,	0,					// for Alpha-component
		0,						// Number of planes
								// of Accumulation buffer
		16,						// for Red-component
		16,						// for Green-component
		16,						// for Blue-component
		16,						// for Alpha-component
		24, 					// Depth of Z-buffer
		0,						// Depth of Stencil-buffer
		0,						// Depth of Auxiliary-buffer
		0,						// Now is ignored
		0,						// Number of planes
		0,						// Now is ignored
		0,						// Color of transparent mask
		0						// Now is ignored
	};

	// �E�B���h�E�^�t�� �X�N���[���؂�ւ�
	if (!bWindow) {
		DEVMODE	devmode;
		devmode.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		devmode.dmBitsPerPel	= 32;
		devmode.dmPelsWidth		= SCREEN_WIDTH;
		devmode.dmPelsHeight	= SCREEN_HEIGHT;
		devmode.dmSize			= sizeof devmode;
		if (ChangeDisplaySettings(&devmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			MessageBox(hWnd, _T("�t���X�N���[���؂�ւ��Ɏ��s���܂���"),
				_T("error"), MB_OK | MB_ICONERROR);
			return E_FAIL;
		}
		SetWindowLong(hWnd, GWL_STYLE, WS_VISIBLE);
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0,
			SCREEN_WIDTH, SCREEN_HEIGHT, SWP_SHOWWINDOW);
	}

	// �E�B���h�E �R���e�L�X�g�̎擾
	g_hDC = GetDC(hWnd);

	// ���v����ł��߂��s�N�Z�� �t�H�[�}�b�g�̑I��
	int nPixelFormat = ChoosePixelFormat(g_hDC, &pfd);
	if (!nPixelFormat) {
		MessageBox(hWnd, _T("�s�N�Z���t�H�[�}�b�g�̑I���Ɏ��s���܂���"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// �s�N�Z�� �t�H�[�}�b�g�̐ݒ�
	if (!SetPixelFormat(g_hDC, nPixelFormat, &pfd)) {
		MessageBox(hWnd, _T("�s�N�Z���t�H�[�}�b�g�̐ݒ�Ɏ��s���܂���"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// OpenGL �����_�����O �R���e�L�X�g���쐬
	g_hRC = wglCreateContext(g_hDC);
	if (!g_hRC) {
		MessageBox(hWnd, _T("�����_�����O�R���e�L�X�g�̐����Ɏ��s���܂���"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// OpenGL �����_�����O �R���e�L�X�g�����C�N
	if (!wglMakeCurrent(g_hDC, g_hRC)) {
		MessageBox(hWnd, _T("�����_�����O�R���e�L�X�g�̃��C�N�Ɏ��s���܂���"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// �t�H���g �Z�b�g�A�b�v
	g_uFontTexID = LoadTexture("tga/font.tga");

	//tga�t�@�C���ǂݍ���
	g_uTexIDTitle = LoadTexture("tga/title2.tga");
	g_uTexIDGameOver = LoadTexture("tga/over.tga");
	g_uTexIDGameClear = LoadTexture("tga/ok.tga");

	//FBX�t�@�C���ǂݍ���
	ZeroMemory(&g_fbxModel, sizeof(g_fbxModel));
	LoadFBX(&g_fbxModel,"arrow.fbx");
	MatrixIdentity(g_mtxModel);
	//�n��
	ZeroMemory(&g_fbxModelLand, sizeof(g_fbxModelLand));
	LoadFBX(&g_fbxModelLand,"arrow.fbx");
	MatrixIdentity(g_mtxModelLand);
	return S_OK;
}
//-------------------------------------------------------------------------------------------------
// OpenGL �̃N���[���A�b�v
//-------------------------------------------------------------------------------------------------
HRESULT CleanupGraphics(HWND hWnd, bool bWindow)
{
	ReleaseTexture(g_uFontTexID);		// �t�H���g�p�e�N�X�`�����
	g_uFontTexID = 0;

	wglMakeCurrent(NULL, NULL);
	if (g_hRC) {						// �J�����g�R���e�L�X�g�����
		wglDeleteContext(g_hRC);
		g_hRC = NULL;
	}
	if (g_hDC) {						// �f�o�C�X�R���e�L�X�g�����
		ReleaseDC(hWnd, g_hDC);
		g_hDC = NULL;
	}
	if (!bWindow)
		ChangeDisplaySettings(NULL, 0);
	return S_OK;
}

//-------------------------------------------------------------------------------------------------
// OpenGL �̃����_�����O
//-------------------------------------------------------------------------------------------------
HRESULT RenderGraphics()
{
	// �o�b�N �o�b�t�@�� Z �o�b�t�@���N���A
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Init3D();
	DrawMain();

	Init2D();
	// �f�o�b�O �X�g�����O�`��
	glColor3f(1.0f, 1.0f, 1.0f);
	DrawStr(0, 0, g_szDebug);

	// �o�b�N �o�b�t�@���v���C�}�� �o�b�t�@�ɃR�s�[
	SwapBuffers(g_hDC);

	return S_OK;
}

//---------------------------------------------------------------------------------------
// 2D �p�����_�����O �X�e�[�g������
//---------------------------------------------------------------------------------------
void Init2D()
{
	// ���C�g������
	glDisable(GL_LIGHTING);

	// �e�N�X�`����L���ɂ���
	glEnable(GL_TEXTURE_2D);

	// �|���S���̕Жʂ����`��𖳌���
	glDisable(GL_CULL_FACE);

	// �[�x�o�b�t�@�𖳌���
	glDisable(GL_DEPTH_TEST);

	// �A���t�@ �u�����f�B���O�L����
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);

	// �v���W�F�N �V�����}�g���b�N�X�Ƃ��Đ��ˉe��ݒ�
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, -1.0, 1.0);

	// �r���[ �}�g���b�N�X�ݒ�
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//---------------------------------------------------------------------------------------
// 3D �p�����_�����O �X�e�[�g������
//---------------------------------------------------------------------------------------
void Init3D()
{
	// �e�N�X�`���𖳌��ɂ���
	glDisable(GL_TEXTURE_2D);

	// �|���S���̕Жʂ����`���L����
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// �[�x�o�b�t�@��L����
	glEnable(GL_DEPTH_TEST);

	// �A���t�@ �u�����f�B���O������
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	// �v���W�F�N�V���� �}�g���b�N�X�ݒ�
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, (double)SCREEN_WIDTH / SCREEN_HEIGHT, NEAR_CLIP, FAR_CLIP);
	glGetFloatv(GL_PROJECTION_MATRIX, g_MatProj[0]);

	// �r���[ �}�g���b�N�X�ݒ�
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f,												// ���_�ʒu(X)
			  0.0f,												// ���_�ʒu(Y)
			  SCREEN_HEIGHT * 0.5f / tanf(RAD * FOVY * 0.5f),	// ���_�ʒu(Z)
			  0.0f, 0.0f, 0.0f,		// �����_�ʒu
			  0.0f, 1.0f, 0.0f);	// �A�b�v�x�N�g��
	glGetFloatv(GL_MODELVIEW_MATRIX, g_MatView[0]);
}

//---------------------------------------------------------------------------------------
// 1 �����`��
//---------------------------------------------------------------------------------------
void DrawChar(int x, int y, int c)
{
	FVERTEX	vtxQuad[4];

	vtxQuad[0].tu = (float)(c & 15) / FONT_HEIGHT;
	vtxQuad[0].tv = (float)(15 - (c >> 4)) / FONT_HEIGHT;
	vtxQuad[0].x  = (float)x;
	vtxQuad[0].y  = (float)y + FONT_HEIGHT;
	vtxQuad[0].z  = 0.0f;
	vtxQuad[1].tu = (float)((c & 15) + 1.0f) / FONT_HEIGHT;
	vtxQuad[1].tv = (float)(15 - (c >> 4)) / FONT_HEIGHT;
	vtxQuad[1].x  = (float)x + FONT_HEIGHT;
	vtxQuad[1].y  = (float)y + FONT_HEIGHT;
	vtxQuad[1].z  = 0.0f;
	vtxQuad[2].tu = (float)((c & 15) + 1.0f) / FONT_HEIGHT;
	vtxQuad[2].tv = (float)(15 - (c >> 4) + 1.0f) / FONT_HEIGHT;
	vtxQuad[2].x  = (float)x + FONT_HEIGHT;
	vtxQuad[2].y  = (float)y;
	vtxQuad[2].z  = 0.0f;
	vtxQuad[3].tu = (float)(c & 15) / FONT_HEIGHT;
	vtxQuad[3].tv = (float)(15 - (c >> 4) + 1.0f) / FONT_HEIGHT;
	vtxQuad[3].x  = (float)x;
	vtxQuad[3].y  = (float)y;
	vtxQuad[3].z  = 0.0f;

	glEnable(GL_TEXTURE_2D);	// �e�N�X�`���L����
	glBindTexture(GL_TEXTURE_2D, g_uFontTexID);
	glInterleavedArrays(GL_T2F_V3F, 0, vtxQuad);
	glDrawArrays(GL_QUADS, 0, 4);
}

//---------------------------------------------------------------------------------------
// ������`��
//---------------------------------------------------------------------------------------
void DrawStr(int x, int y, LPCTSTR psz)
{
	for (; *psz; ++psz) {
		switch (*psz) {
		case _T('\n'):
			x = 0;
			y += FONT_HEIGHT;
			break;
		default:
			DrawChar(x, y, *psz);
			x += FONT_WIDTH;
			break;
		}
	}
}

//-----------------------------------------------------------------------------
//�@�Q�[���{��
//-----------------------------------------------------------------------------
void GameMain()
{
	const struct{
		int nKey;
		DWORD dwMask;
	} keytbl[] = {
		{VK_LEFT,	0x00000001},
		{VK_RIGHT,	0x00000002},
		{VK_UP,		0x00000004},
		{VK_DOWN,	0x00000008},
		{VK_SPACE,	0x00000010},
		{VK_RETURN,	0x00000020},

	};

	TCHAR	str[256];
	static DWORD dwPrev = 0;
	DWORD dwKey;

	g_szDebug[0] = _T('\0');	// �f�o�b�O �X�g�����O �o�b�t�@������

	_stprintf(str, _T("FPS = %.1f\n"), g_fFPS);
	lstrcat(g_szDebug, str);

	// �����Ƀ��C��������ǉ�.

	//�L�[�̃g���K�[����
	dwKey = 0;
	for(int i = 0 ; i < _countof(keytbl) ; i++)
	{
		if(GetAsyncKeyState(keytbl[i].nKey) & 0x8000){
			dwKey |= keytbl[i].dwMask;
		}
	}
	
	switch(g_status)
	{
	case INIT:
		//�Q�[�����ł̏���������
		g_status = TITLE;
		//�J�����̏�����
		g_camera.Init();

		//THRU
	case TITLE:
		//Enter�ŊJ�n
		if((dwPrev & 0x0020) && (dwKey & 0x0020) == 0)
			g_status = GAME;
		break;

	case GAME:
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 0.0f;
//		g_cube.CreateCube(0.5f,pos);
		//�J�����ړ�
		g_camera.Move();
		
		//�X�y�[�X��Enter�Ŏ���
		if((dwPrev & 0x0020) && (dwKey & 0x0020) == 0)
			g_status = GAMEOVER;

		break;

	case GAMECLEAR:
		//�J�����ʒu�Ƃ肠����������
		g_camera.Init();
		//Enter�ŏ�������
		if((dwPrev & 0x0020) && (dwKey & 0x0020) == 0)
			g_status = INIT;
		break;
	case GAMEOVER:
		//�J�����ʒu�Ƃ肠����������
		g_camera.Init();

		//Enter�ŏ�������
		if((dwPrev & 0x0020) && (dwKey & 0x0020) == 0)
			g_status = INIT;
		break;
	}

	dwPrev = dwKey;
}

//-----------------------------------------------------------------------------
//�@�`��{��
//-----------------------------------------------------------------------------
void DrawMain()
{
	// �����ɕ`�揈����ǉ�.
	switch(g_status)
	{
	//����������
	case INIT:
		//�ˉe�ϊ��s��ݒ�
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30,4.0f/3.0f,1.0f,10000.0f);

		//THRU
	
	//�^�C�g�����
	case TITLE:
		Init2D();
		DrawStr(20,20,_T("MODE_TITLE"));
		DrawStr(20,35,_T("PUSH_ENTER"));
		Init3D();
		//�J�����ݒ�
		g_camera.Set();

		glBindTexture(GL_TEXTURE_2D,g_uTexIDTitle);
		glEnable(GL_TEXTURE_2D);
		//�l�p�`�`��
		glBegin(GL_POLYGON);
		glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glEnd();
	
		break;

	//�Q�[�������
	case GAME:
		Init2D();
		DrawStr(20,20,_T("MODE_GAME"));
		DrawStr(20,35,_T("PUSH_ENTER_TO_GAMEOVER"));
		DrawStr(20,65,_T("UP:CAMERA_UP"));
		DrawStr(20,80,_T("DOWN:CAMERA_DOWN"));
		DrawStr(20,95,_T("LEFT:ROTATION_LEFT"));
		DrawStr(20,110,_T("RIGHT:ROTATION_RIGHT"));
		DrawStr(20,125,_T("SHIFT+UP:ZOOM_IN"));
		DrawStr(20,140,_T("SHIFT+DOWN:ZOOM_OUT"));
		Init3D();
		//�J�����ݒ�
		g_camera.Set();

		//���f���`��
		MatrixScaling(g_mtxModel, 0.0025f, 0.0025f, 0.0025f);
		DrawFBX(&g_fbxModel,g_mtxModel);

		//���f���`��
		MatrixScaling(g_mtxModelLand, 25, 25, 25);
		DrawFBX(&g_fbxModelLand,g_mtxModelLand);

		break;

	case GAMECLEAR:
		Init2D();
		DrawStr(20,20,_T("MODE_GAMECLEAR"));
		DrawStr(20,35,_T("PUSH_ENTER"));
		Init3D();
		//�J�����ݒ�
		g_camera.Set();

		glBindTexture(GL_TEXTURE_2D,g_uTexIDGameClear);
		glEnable(GL_TEXTURE_2D);
		//�l�p�`�`��
		glBegin(GL_POLYGON);
		glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glEnd();

		break;

	//�Q�[���I�[�o�[���
	case GAMEOVER:
		Init2D();
		DrawStr(20,20,_T("MODE_GAMEOVER"));
		DrawStr(20,35,_T("PUSH_ENTER"));
		Init3D();
		//�J�����ݒ�
		g_camera.Set();

		glBindTexture(GL_TEXTURE_2D,g_uTexIDGameOver);
		glEnable(GL_TEXTURE_2D);
		//�l�p�`�`��
		glBegin(GL_POLYGON);
		glColor4f(1.0f, 0.5f, 0.5f, 1.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glEnd();

		break;
	}
}

//--------------------------------------------------------------------
// eof
//--------------------------------------------------------------------
