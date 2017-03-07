//====================================================================
//    [ OGLbase ] OpenGL
//====================================================================
//-------- インクルードヘッダ
#define STRICT
#include <windows.h>		// Windows プログラムにはこれを付ける
#include <tchar.h>			// 汎用テキスト マッピング
#include <GL/gl.h>			// OpenGL 関連のヘッダ
#include <GL/glu.h>			// OpenGL 関連のヘッダ
#include <stdio.h>			// 文字列処理で使用
#include <mmsystem.h>		// 時間管理で使用
#define _USE_MATH_DEFINES	// math.h定義の定数を使用
#include <math.h>			// 三角関数、円周率で使用

#include "FBX.h"
#include "Texture.h"

#include "CCamera.h"		//カメラクラス
#include "GlCube.h"			//立方体クラス

//-------- ライブラリのリンク（こう記述しておけば別途リンク設定が不要となる）
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

//-------- 定数定義
#define CLASS_NAME		_T("OGLbase")						// ウィンドウクラス名
#define CAPTION_NAME	_T("[ OGLGame ]")	// キャプション名

#define FRAME_RATE			(1000/60)						// フレームレート
#define SCREEN_WIDTH		640								// スクリーンの幅
#define SCREEN_HEIGHT		480								// スクリーンの高さ
#define FOVY				45.0f							// 視野角
#define NEAR_CLIP			1.0f							// ニアクリップを行う距離
#define FAR_CLIP			10000.0f						// ファークリップを行う距離

#define RAD					(float)(M_PI/180.0)				// ラジアン変換用定数

#define FONT_WIDTH			10								// フォント幅
#define FONT_HEIGHT			16								// フォント高さ

//----- 頂点フォーマット定義
struct FVERTEX {
	// GL_T2F_V3F
	GLfloat		tu, tv;			// テクスチャ座標
	GLfloat		x, y, z;		// 頂点座標
};

//----- グローバル変数
TCHAR		g_szDebug[4096];		// 出力文字列バッファ
float		g_fFPS;					// フレーム数カウント用

// OpenGL 関連
HDC			g_hDC;					// デバイスコンテキスト
HGLRC		g_hRC;					// カレントコンテキスト
bool		g_bWindow;				// ウィンドウモード
GLuint		g_uFontTexID;			// フォントテクスチャ ID
GLfloat		g_MatView[4][4];		// ビュー マトリックス
GLfloat		g_MatProj[4][4];		// プロジェクション マトリックス

//モデル
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
GLuint	g_uTexIDTitle;	//タイトル用テクスチャ
GLuint	g_uTexIDGameOver;	//ゲームオーバー用テクスチャ
GLuint	g_uTexIDGameClear;	//ゲームクリア用テクスチャ
GLuint	g_uTexIDAlphabet;	//アルファベット用テクスチャ

//----- プロトタイプ宣言
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
// メイン
//-------------------------------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int iCmdShow)
{
	HWND	hWnd;
	MSG		msg;
	DWORD	dwExecLastTime, dwFPSLastTime, dwCurrentTime, dwFrameCount;

	// ウィンドウ クラスを登録
	WNDCLASS wndClass = {
		CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst, LoadIcon(hInst, IDI_APPLICATION),
		LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(WHITE_BRUSH), NULL, CLASS_NAME
	};
	if (RegisterClass(&wndClass) == 0) return false;

	// ウィンドウ サイズを取得
	RECT rc = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	DWORD dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	DWORD dwExStyle = 0;
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

	// ウィンドウを作成
	hWnd = CreateWindowEx(dwExStyle, CLASS_NAME, CAPTION_NAME, dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, hInst, NULL);
	if (hWnd == NULL) return false;

	g_bWindow = false;
	if (IDYES == MessageBox(hWnd, _T("ウィンドウ モードで実行しますか？"), _T("画面モード"), MB_YESNO))
		g_bWindow = true;
	if (FAILED(InitializeGraphics(hWnd, g_bWindow))) return 0;	// OpenGL の初期化

	// 変数初期化
	timeBeginPeriod(1);									// タイマの分解能を最小にセット
	dwFPSLastTime = dwExecLastTime = timeGetTime();		// 現在のシステム タイマを取得
	dwExecLastTime -= FRAME_RATE;
	dwFrameCount = 0;
	g_fFPS = 0.0f;

	// メイン ウインドウ ループ
	msg.message = WM_NULL;
	while (WM_QUIT != msg.message) {						// WM_QUIT がくるまでループ
		if (PeekMessage(&msg,NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			dwCurrentTime = timeGetTime();					// 現在のタイマ値を取得
			if (dwCurrentTime - dwFPSLastTime >= 500) {		// 0.5 秒ごとに計測
				// フレーム数を計算
				g_fFPS = dwFrameCount * 1000.0f / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;	// タイマ値を更新
				dwFrameCount = 0;				// フレーム カウンタをリセット
			}
			// この辺で時間管理
			while (dwCurrentTime - dwExecLastTime >= FRAME_RATE) {	// 一定時間が経過したら・・・
				dwExecLastTime += FRAME_RATE;						// タイマ値を更新
				GameMain();											// ゲーム メイン処理
			}
			RenderGraphics();								// レンダリング処理を実行
			dwFrameCount++;									// フレーム カウントを＋１
			Sleep(1);										// いったん Windows に制御を戻す
		}
	}
	timeEndPeriod(1);						// システム タイマの分解能を元に戻す
	return (int)msg.wParam;
}

//-------------------------------------------------------------------------------------------------
// メッセージ ハンドラ
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
// OpenGL の初期化
//-------------------------------------------------------------------------------------------------
HRESULT InitializeGraphics(HWND hWnd, bool bWindow)
{
	// ピクセル フォーマット
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

	// ウィンドウ／フル スクリーン切り替え
	if (!bWindow) {
		DEVMODE	devmode;
		devmode.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		devmode.dmBitsPerPel	= 32;
		devmode.dmPelsWidth		= SCREEN_WIDTH;
		devmode.dmPelsHeight	= SCREEN_HEIGHT;
		devmode.dmSize			= sizeof devmode;
		if (ChangeDisplaySettings(&devmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			MessageBox(hWnd, _T("フルスクリーン切り替えに失敗しました"),
				_T("error"), MB_OK | MB_ICONERROR);
			return E_FAIL;
		}
		SetWindowLong(hWnd, GWL_STYLE, WS_VISIBLE);
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0,
			SCREEN_WIDTH, SCREEN_HEIGHT, SWP_SHOWWINDOW);
	}

	// ウィンドウ コンテキストの取得
	g_hDC = GetDC(hWnd);

	// 合致する最も近いピクセル フォーマットの選択
	int nPixelFormat = ChoosePixelFormat(g_hDC, &pfd);
	if (!nPixelFormat) {
		MessageBox(hWnd, _T("ピクセルフォーマットの選択に失敗しました"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// ピクセル フォーマットの設定
	if (!SetPixelFormat(g_hDC, nPixelFormat, &pfd)) {
		MessageBox(hWnd, _T("ピクセルフォーマットの設定に失敗しました"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// OpenGL レンダリング コンテキストを作成
	g_hRC = wglCreateContext(g_hDC);
	if (!g_hRC) {
		MessageBox(hWnd, _T("レンダリングコンテキストの生成に失敗しました"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// OpenGL レンダリング コンテキストをメイク
	if (!wglMakeCurrent(g_hDC, g_hRC)) {
		MessageBox(hWnd, _T("レンダリングコンテキストのメイクに失敗しました"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// フォント セットアップ
	g_uFontTexID = LoadTexture("tga/font.tga");

	//tgaファイル読み込み
	g_uTexIDTitle = LoadTexture("tga/title2.tga");
	g_uTexIDGameOver = LoadTexture("tga/over.tga");
	g_uTexIDGameClear = LoadTexture("tga/ok.tga");

	//FBXファイル読み込み
	ZeroMemory(&g_fbxModel, sizeof(g_fbxModel));
	LoadFBX(&g_fbxModel,"arrow.fbx");
	MatrixIdentity(g_mtxModel);
	//地面
	ZeroMemory(&g_fbxModelLand, sizeof(g_fbxModelLand));
	LoadFBX(&g_fbxModelLand,"arrow.fbx");
	MatrixIdentity(g_mtxModelLand);
	return S_OK;
}
//-------------------------------------------------------------------------------------------------
// OpenGL のクリーンアップ
//-------------------------------------------------------------------------------------------------
HRESULT CleanupGraphics(HWND hWnd, bool bWindow)
{
	ReleaseTexture(g_uFontTexID);		// フォント用テクスチャ解放
	g_uFontTexID = 0;

	wglMakeCurrent(NULL, NULL);
	if (g_hRC) {						// カレントコンテキストを解放
		wglDeleteContext(g_hRC);
		g_hRC = NULL;
	}
	if (g_hDC) {						// デバイスコンテキストを解放
		ReleaseDC(hWnd, g_hDC);
		g_hDC = NULL;
	}
	if (!bWindow)
		ChangeDisplaySettings(NULL, 0);
	return S_OK;
}

//-------------------------------------------------------------------------------------------------
// OpenGL のレンダリング
//-------------------------------------------------------------------------------------------------
HRESULT RenderGraphics()
{
	// バック バッファと Z バッファをクリア
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Init3D();
	DrawMain();

	Init2D();
	// デバッグ ストリング描画
	glColor3f(1.0f, 1.0f, 1.0f);
	DrawStr(0, 0, g_szDebug);

	// バック バッファをプライマリ バッファにコピー
	SwapBuffers(g_hDC);

	return S_OK;
}

//---------------------------------------------------------------------------------------
// 2D 用レンダリング ステート初期化
//---------------------------------------------------------------------------------------
void Init2D()
{
	// ライト無効化
	glDisable(GL_LIGHTING);

	// テクスチャを有効にする
	glEnable(GL_TEXTURE_2D);

	// ポリゴンの片面だけ描画を無効化
	glDisable(GL_CULL_FACE);

	// 深度バッファを無効化
	glDisable(GL_DEPTH_TEST);

	// アルファ ブレンディング有効化
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);

	// プロジェク ションマトリックスとして正射影を設定
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, -1.0, 1.0);

	// ビュー マトリックス設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//---------------------------------------------------------------------------------------
// 3D 用レンダリング ステート初期化
//---------------------------------------------------------------------------------------
void Init3D()
{
	// テクスチャを無効にする
	glDisable(GL_TEXTURE_2D);

	// ポリゴンの片面だけ描画を有効化
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// 深度バッファを有効化
	glEnable(GL_DEPTH_TEST);

	// アルファ ブレンディング無効化
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	// プロジェクション マトリックス設定
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, (double)SCREEN_WIDTH / SCREEN_HEIGHT, NEAR_CLIP, FAR_CLIP);
	glGetFloatv(GL_PROJECTION_MATRIX, g_MatProj[0]);

	// ビュー マトリックス設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f,												// 視点位置(X)
			  0.0f,												// 視点位置(Y)
			  SCREEN_HEIGHT * 0.5f / tanf(RAD * FOVY * 0.5f),	// 視点位置(Z)
			  0.0f, 0.0f, 0.0f,		// 注視点位置
			  0.0f, 1.0f, 0.0f);	// アップベクトル
	glGetFloatv(GL_MODELVIEW_MATRIX, g_MatView[0]);
}

//---------------------------------------------------------------------------------------
// 1 文字描画
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

	glEnable(GL_TEXTURE_2D);	// テクスチャ有効化
	glBindTexture(GL_TEXTURE_2D, g_uFontTexID);
	glInterleavedArrays(GL_T2F_V3F, 0, vtxQuad);
	glDrawArrays(GL_QUADS, 0, 4);
}

//---------------------------------------------------------------------------------------
// 文字列描画
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
//　ゲーム本体
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

	g_szDebug[0] = _T('\0');	// デバッグ ストリング バッファ初期化

	_stprintf(str, _T("FPS = %.1f\n"), g_fFPS);
	lstrcat(g_szDebug, str);

	// ここにメイン処理を追加.

	//キーのトリガー処理
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
		//ゲーム内での初期化処理
		g_status = TITLE;
		//カメラの初期化
		g_camera.Init();

		//THRU
	case TITLE:
		//Enterで開始
		if((dwPrev & 0x0020) && (dwKey & 0x0020) == 0)
			g_status = GAME;
		break;

	case GAME:
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 0.0f;
//		g_cube.CreateCube(0.5f,pos);
		//カメラ移動
		g_camera.Move();
		
		//スペースかEnterで次へ
		if((dwPrev & 0x0020) && (dwKey & 0x0020) == 0)
			g_status = GAMEOVER;

		break;

	case GAMECLEAR:
		//カメラ位置とりあえず初期化
		g_camera.Init();
		//Enterで初期化へ
		if((dwPrev & 0x0020) && (dwKey & 0x0020) == 0)
			g_status = INIT;
		break;
	case GAMEOVER:
		//カメラ位置とりあえず初期化
		g_camera.Init();

		//Enterで初期化へ
		if((dwPrev & 0x0020) && (dwKey & 0x0020) == 0)
			g_status = INIT;
		break;
	}

	dwPrev = dwKey;
}

//-----------------------------------------------------------------------------
//　描画本体
//-----------------------------------------------------------------------------
void DrawMain()
{
	// ここに描画処理を追加.
	switch(g_status)
	{
	//初期化処理
	case INIT:
		//射影変換行列設定
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30,4.0f/3.0f,1.0f,10000.0f);

		//THRU
	
	//タイトル画面
	case TITLE:
		Init2D();
		DrawStr(20,20,_T("MODE_TITLE"));
		DrawStr(20,35,_T("PUSH_ENTER"));
		Init3D();
		//カメラ設定
		g_camera.Set();

		glBindTexture(GL_TEXTURE_2D,g_uTexIDTitle);
		glEnable(GL_TEXTURE_2D);
		//四角形描画
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

	//ゲーム中画面
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
		//カメラ設定
		g_camera.Set();

		//モデル描画
		MatrixScaling(g_mtxModel, 0.0025f, 0.0025f, 0.0025f);
		DrawFBX(&g_fbxModel,g_mtxModel);

		//モデル描画
		MatrixScaling(g_mtxModelLand, 25, 25, 25);
		DrawFBX(&g_fbxModelLand,g_mtxModelLand);

		break;

	case GAMECLEAR:
		Init2D();
		DrawStr(20,20,_T("MODE_GAMECLEAR"));
		DrawStr(20,35,_T("PUSH_ENTER"));
		Init3D();
		//カメラ設定
		g_camera.Set();

		glBindTexture(GL_TEXTURE_2D,g_uTexIDGameClear);
		glEnable(GL_TEXTURE_2D);
		//四角形描画
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

	//ゲームオーバー画面
	case GAMEOVER:
		Init2D();
		DrawStr(20,20,_T("MODE_GAMEOVER"));
		DrawStr(20,35,_T("PUSH_ENTER"));
		Init3D();
		//カメラ設定
		g_camera.Set();

		glBindTexture(GL_TEXTURE_2D,g_uTexIDGameOver);
		glEnable(GL_TEXTURE_2D);
		//四角形描画
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
