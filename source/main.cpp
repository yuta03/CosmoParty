//================================================== インクルード ==================================================
#include "../DxLib.h"

#define _USE_MATH_DEFINES
# include <math.h>
#undef _USE_MATH_DEFINES
#include <float.h>

#include <string>
#include <vector>

#pragma comment ( lib, "winmm.lib" )

//================================================== 名前空間 ==================================================
using namespace std;

//================================================== ユーティリティ ==================================================

//-------------------- キー --------------------

/**
 * キーパッド
 */
class Keypad{
	int pressSt_; /** 押されているキー */
	int pushSt_; /** 押されたキー */
	int releaseSt_; /** 離されたキー */
	
public:
	Keypad();
	~Keypad();
private:
	Keypad( const Keypad& );
	Keypad& operator=( const Keypad& );
	
	
public:
	Keypad& update();
	
public:
	bool isPressed( int keyCode ) const;
	bool isPushed( int keyCode ) const;
	bool isReleased( int keyCode ) const;
};

/**
 * キーパッド生成
 */
Keypad::Keypad():
pressSt_( 0 ), pushSt_( 0 ), releaseSt_( 0 ){
}
/**
 * キーパッド破棄
 */
Keypad::~Keypad(){
}

/**
 * キーパッド状態更新
 */
Keypad& Keypad::update(){
	int prev = pressSt_;
	pressSt_ = GetJoypadInputState( DX_INPUT_KEY_PAD1 );
	releaseSt_ = prev & ( pressSt_ ^ prev );
	pushSt_  = pressSt_ & ( pressSt_ ^ prev );
	
	return *this;
}

/**
 * 押されているかどうか
 */
bool Keypad::isPressed( int keyCode ) const{
	return ( pressSt_ & keyCode ) != 0;
}
/**
 * 押されたかどうか
 */
bool Keypad::isPushed( int keyCode ) const{
	return ( pushSt_ & keyCode ) != 0;
}
/**
 * 離されたかどうか
 */
bool Keypad::isReleased( int keyCode ) const{
	return ( releaseSt_ & keyCode ) != 0;
}

//-------------------- シーン --------------------

struct Global;

/**
 * 各シーンクラスのインターフェイス
 */
class Scene{
public:
	Scene(){}
	virtual ~Scene(){}
	
	virtual void onUpdate() = 0;
	virtual void onDraw() = 0;
};


//================================================== グローバルオブジェクト ==================================================

/**
 * シーンコード
 */
enum SceneCode{
	SceneCodeEmpty4Develop = 0, /** 【開発用】空シーン */
};

/**
 * 全体で使うオブジェクト郡
 */
struct Global{
	Keypad keypad1; /** キーパッド */
	Scene* scene; /** シーン */
	SceneCode sceneCode; /** シーンコード */
	
	Global();
	~Global();
};

/**
 * 生成
 */
Global::Global():
keypad1(),
scene( NULL ), sceneCode( SceneCodeEmpty4Develop ){
}

/**
 * 破棄
 */
Global::~Global(){
	delete scene; scene = NULL;
}

//================================================== 【開発用】空シーン ==================================================

/**
 * 空シーン
 */
class EmptyScene : public virtual Scene{
public:
	EmptyScene( Global* g );
	~EmptyScene();
	
	void onUpdate();
	void onDraw();

private:
	Global* g_; /** グローバルオブジェクト */
};

/**
 * 生成
 */
EmptyScene::EmptyScene( Global* g ):
Scene(),
g_( g ){
}
/**
 * 破棄
 */
EmptyScene::~EmptyScene(){
}

/**
 * 更新
 */
void EmptyScene::onUpdate(){
}

/**
 * 描画
 */
void EmptyScene::onDraw(){
	ClearDrawScreen(); // クリア
	ScreenFlip(); // 描画完了
}








//================================================== シーンファクトリ ==================================================

/**
 * シーン生成
 */
static Scene* createScene( SceneCode code, Global* g ){
	if ( code == SceneCodeEmpty4Develop ) return new EmptyScene( g );
	
	return NULL;
}

//================================================== エントリポイント ==================================================

/**
 * メインループ
 */
static void runMainLoop(){
	Global global;
	
	// 初期化
	global.scene = createScene( global.sceneCode, &global );
	
	// 現在のシーンコード
	SceneCode sceneCode = global.sceneCode;
	
	// ゲームループ
	while( ProcessMessage() == 0 && CheckHitKey( KEY_INPUT_ESCAPE ) == 0 ){
		// 終了判定
		if ( CheckHitKey( KEY_INPUT_ESCAPE ) != 0 ){
			break;
		}
		
		unsigned int bgnTm = timeGetTime(); // フレームの開始時刻
		
		// キー操作
		global.keypad1.update();
		
		if ( sceneCode != global.sceneCode ){
			sceneCode = global.sceneCode;
			delete global.scene;
			global.scene = NULL;
			global.scene = createScene( global.sceneCode, &global );
		}
		
		// シーン更新
		global.scene->onUpdate();
		global.scene->onDraw();
		
		// スリープ
		unsigned int endTm = timeGetTime(); // フレームの終了時刻
		int cost = endTm - bgnTm;
		if ( cost < 1 ) cost = 1;
		if ( cost > 16 ) cost = 16;
		
		Sleep( 16 - cost ); // スリープする時間
	}
}

/**
 * エントリポイント
 */
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ){
	ChangeWindowMode( TRUE ); // ウィンドウモード
	//SetGraphMode( 1280, 720, 16 ); // 解像度(HD)
	SetGraphMode( 640, 360, 16 ); // 解像度(HVGAW)
	if( DxLib_Init() == -1 ){
		// ＤＸライブラリ初期化処理
		 return -1; // エラーが起きたら直ちに終了
	}
	
	SetDrawScreen( DX_SCREEN_BACK ); // 描画先設定
	timeBeginPeriod(1); // 時間精度
	
	runMainLoop(); // メインループ
	
	timeEndPeriod(1); // 時間精度
	DxLib_End(); // ＤＸライブラリ使用の終了処理
	
	return 0;
}
