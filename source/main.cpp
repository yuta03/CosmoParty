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

//-------------------- 3D --------------------

/**
 * 3Dベクトル
 */
struct Vector3f : public VECTOR{
	Vector3f();
	Vector3f( const VECTOR& rhs );
	Vector3f( float ax, float ay, float az );
	~Vector3f();
	
	Vector3f& set( float ax, float ay, float az );
	
	Vector3f& operator =( const VECTOR& rhs );
	Vector3f& operator +=( const VECTOR& rhs );
	
	Vector3f& setLength( float len );
	
	float getLength() const;
	float getLength2() const;
};

const Vector3f operator +( const VECTOR& lhs, const VECTOR& rhs );
const Vector3f operator -( const VECTOR& lhs, const VECTOR& rhs );
float getDistance( const VECTOR& lhs, const VECTOR& rhs );

/**
 * 生成
 */
Vector3f::Vector3f(){
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}
/**
 * 生成(コピー)
 */
Vector3f::Vector3f( const VECTOR& rhs ){
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
}
/**
 * 生成
 */
Vector3f::Vector3f( float _x, float _y, float _z ){
	x = _x;
	y = _y;
	z = _z;
}
/**
 * 破棄
 */
Vector3f::~Vector3f(){
}
/**
 * 値変更
 */
Vector3f& Vector3f::set( float _x, float _y, float _z ){
	x = _x;
	y = _y;
	z = _z;
	return *this;
}
/**
 * コピー
 */
Vector3f& Vector3f::operator =( const VECTOR& rhs ){
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}
/**
 * 加算
 */
Vector3f& Vector3f::operator +=( const VECTOR& rhs ){
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}
/**
 * 長さ変更
 */
Vector3f& Vector3f::setLength( float len ){
	float oriLen = getLength();
	if ( oriLen < FLT_EPSILON ) return *this;
	float v = len / oriLen;
	
	x = x * v;
	y = y * v;
	z = z * v;
	
	return *this;
}
/**
 * 長さ取得
 */
float Vector3f::getLength() const{
	return ( float )sqrt( getLength2() );
}
/**
 * 長さの２乗取得
 */
float Vector3f::getLength2() const{
	return x * x + y * y + z * z;
}

/**
 * 加算
 */
const Vector3f operator +( const VECTOR& lhs, const VECTOR& rhs ){
	return Vector3f( lhs.x + rhs.x,  lhs.y + rhs.y, lhs.z + rhs.z );
}
/**
 * 減算
 */
const Vector3f operator -( const VECTOR& lhs, const VECTOR& rhs ){
	return Vector3f( lhs.x - rhs.x,  lhs.y - rhs.y, lhs.z - rhs.z );
}
/**
 * 
 */
float getDistance( const VECTOR& lhs, const VECTOR& rhs ){
	float x = lhs.x - rhs.x;
	float y = lhs.y - rhs.y;
	float z = lhs.z - rhs.z;
	return ( float )sqrt( x * x + y * y + z * z );
}

static const float F_PI = ( float )M_PI; /** float 型のπ */

//-------------------- キー --------------------

/**
 * キーパッド
 */
class Keypad{
	int pressSt_; /** 押されているキー */
	int pushSt_; /** 押されたキー */
	int releaseSt_; /** 離されたキー */
	float stickX_; /** スティック x 成分 */
	float stickY_; /** スティック y 成分 */
	
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
	
	float getStickX() const;
	float getStickY() const;
};

/**
 * キーパッド生成
 */
Keypad::Keypad():
pressSt_( 0 ), pushSt_( 0 ), releaseSt_( 0 ),
stickX_( 0.0f ), stickY_( 0.0f ){
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
	// キーパッド
	int prev = pressSt_;
	pressSt_ = GetJoypadInputState( DX_INPUT_KEY_PAD1 );
	releaseSt_ = prev & ( pressSt_ ^ prev );
	pushSt_  = pressSt_ & ( pressSt_ ^ prev );
	
	// スティック
	int joypadX = 0;
	int joypadY = 0;
	GetJoypadAnalogInput( &joypadX, &joypadY, DX_INPUT_PAD1 );
	stickX_ = ( float )joypadX / 1000.0f;
	stickY_ = ( float )joypadY / 1000.0f;
	
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
/**
 * スティック x 成分取得
 */
float Keypad::getStickX() const{
	return stickX_;
}
/**
 * スティック y 成分取得
 */
float Keypad::getStickY() const{
	return stickY_;
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
	SceneCodePlayerControll4Develop, /** 【開発用】プレイヤー操作シーン */
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
scene( NULL ), sceneCode( SceneCodePlayerControll4Develop ){
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



//================================================== 【開発用】プレイヤー操作シーン ==================================================


/**
 * カメラ情報
 */
struct Camera{
	Vector3f pos; /** 位置 */
	Vector3f target; /** 対象 */
};

/**
 * プレイヤー情報
 */
struct Player{
	Vector3f pos; /** 位置 */
	Vector3f mv; /** 移動速度 */
	Vector3f rot; /** 回転 */
};

/**
 * ターゲット情報
 */
struct Target{
	Vector3f pos; /** 位置 */
	Vector3f mv; /** 移送速度 */
};


/**
 * プレイヤーの制御に必要な情報
 */
struct PlayerUnit{
	Keypad& keypad; /** キーパッド */
	Player& player; /** プレイヤ */
	Target& target; /** ターゲット */
	Camera& camera; /** カメラ */
	
	PlayerUnit( Keypad& k, Player& p, Target& t, Camera& c ):
	keypad( k ), player( p ), target( t ), camera( c ){}
};

/**
 * プレイヤーコントローラ
 */
class PlayerController{
public:
	PlayerController( PlayerUnit& unit );
	~PlayerController();

public:
	void controll();
	void update();
	
private:
	PlayerUnit& unit_;
	bool holdJoypad_;
};

/**
 * 生成
 */
PlayerController::PlayerController( PlayerUnit& unit ):
unit_( unit ){
}
/**
 * 破棄
 */
PlayerController::~PlayerController(){
}
/**
 * 操作
 */
void PlayerController::controll(){
	bool leftAvert = false;
	bool rightAvert = false;
	float stickX = 0.0f;
	float stickY = 0.0f;
	
	// ジョイパッド状態取得
	holdJoypad_ = false;
	stickX = unit_.keypad.getStickX();
	stickY = unit_.keypad.getStickY();
	if ( !( fabs( stickX ) < FLT_EPSILON && fabs( stickY ) < FLT_EPSILON ) ){
		holdJoypad_ = true;
	}
	
	// 操作・速度更新
	{
		// ターゲット操作
		{
			if ( holdJoypad_ ){
				// ジョイパッド操作中
				unit_.target.mv.x = stickX * 100.0f;
				unit_.target.mv.y = -stickY * 100.0f;
			}else{
				// ジョイパッド非操作中
				unit_.target.mv.set( 0.0f, 0.0f, 0.0f );
			}

			if ( unit_.keypad.isPressed( PAD_INPUT_7 ) ) leftAvert = true;
			if ( unit_.keypad.isPressed( PAD_INPUT_8 ) ) rightAvert = true;
		}
		
		// プレイヤ
		{
			// 向き
			{
				Vector3f diff;
				
				diff = unit_.target.pos - unit_.player.pos;
				
				if ( diff.getLength2() > FLT_EPSILON ){
					unit_.player.rot.x = ( float )( atan2( ( float )-diff.y, ( float )diff.z ) );
					unit_.player.rot.y = ( float )( atan2( ( float )diff.z, ( float )-diff.x ) - F_PI * 0.5f );
					
					if ( !( leftAvert || rightAvert ) ){
						unit_.player.rot.z = -unit_.player.rot.y;
					}
				}else{
					unit_.player.rot.x = 0.0f;
					unit_.player.rot.y = 0.0f;
					unit_.player.rot.z = 0.0f;
				}
			}
			
			// 移動速度
			{
				MATRIX my = MGetRotY( unit_.player.rot.y );
				MATRIX mx = MGetRotX( unit_.player.rot.x );
				Vector3f v = VTransform( Vector3f( 0.0f, 0.0f, 30.0f ), mx );
				v = VTransform( v, my );
				
				unit_.player.mv.x = v.x;
				unit_.player.mv.y = v.y;
				unit_.player.mv.z = 0.0f;
			}
			
			// 向き
			if ( leftAvert || rightAvert ){
				if ( leftAvert ){
					unit_.player.rot.z = unit_.player.rot.z * 0.4f + F_PI / 2.0f * 0.6f;
				}
				else if ( rightAvert ){
					unit_.player.rot.z = unit_.player.rot.z * 0.4f + ( -F_PI / 2.0f * 0.6f );
				}
				unit_.player.rot.y *= 2.0f; // 演出としてより傾ける
				unit_.player.mv.x *= 2.0f;
			}
			
			if ( unit_.player.rot.y < -F_PI / 2.0f ) unit_.player.rot.y = -F_PI / 2.0f;
			if ( unit_.player.rot.y > F_PI / 2.0f ) unit_.player.rot.y = F_PI / 2.0f;
		}
	}
}
/**
 * 更新
 */
void PlayerController::update(){
	// 位置更新
	{
		// プレイヤ
		{
			unit_.player.pos += unit_.player.mv;

			// 位置制限
			if ( unit_.player.pos.x < -750.0f ) unit_.player.pos.x = -750.0f;
			if ( unit_.player.pos.x > 750.0f ) unit_.player.pos.x = 750.0f;
			if ( unit_.player.pos.y < 0.0f ) unit_.player.pos.y = 0.0f;
			if ( unit_.player.pos.y > 350.0f ) unit_.player.pos.y = 350.0f;
		}
		
		// ターゲット
		{
			if ( holdJoypad_ ){
				// ジョイパッド操作中
				unit_.target.pos += unit_.target.mv;
			}else{
				// ジョイパッド非操作中
				
				// 位置を戻す
				unit_.target.pos.x = unit_.player.pos.x * 0.1f + unit_.target.pos.x * 0.9f;
				unit_.target.pos.y = unit_.player.pos.y * 0.1f + unit_.target.pos.y * 0.9f;
			}
			
			unit_.target.pos.z = unit_.player.pos.z + 1000.0f;
			
			// 位置制限
			if ( unit_.target.pos.x < -750.0f ) unit_.target.pos.x = -750.0f;
			if ( unit_.target.pos.x > 750.0f ) unit_.target.pos.x = 750.0f;
			if ( unit_.target.pos.y < -500.0f ) unit_.target.pos.y = -500.0f;
			if ( unit_.target.pos.y > 1000.0f ) unit_.target.pos.y = 1000.0f;
		}
	}
	
	// カメラ位置更新
	{
		unit_.camera.target.set( 0.0f, 0.0f, unit_.player.pos.z );
		unit_.camera.pos = unit_.camera.target + Vector3f( 0.0f, 0.0f, -1500.0f );
	}
}



/**
 * プレイヤー操作シーン</BR>
 * 1.0f = 1cm とする。
 */
class PlayerControllScene : public virtual Scene{
public:
	PlayerControllScene( Global* g );
	~PlayerControllScene();
	
	void onUpdate();
	void onDraw();

private:
	Global* g_; /** グローバルオブジェクト */
	int playerModelId_; /** プレイヤーモデル ID */
	int targetModelId_; /** ターゲットモデル ID */
	
	Player player_; /** プレイヤー */
	Target target_; /** ターゲット */
	Camera camera_; /** カメラ */
};

/**
 * 生成
 */
PlayerControllScene::PlayerControllScene( Global* g ):
Scene(),
g_( g ),
playerModelId_( -1 ), targetModelId_( -1 ),
player_(), target_(), camera_(){
	// モデル読み込み
	playerModelId_ = MV1LoadModel( "resource/airplane_player.x" );
	targetModelId_ = MV1LoadModel( "resource/target.x" );
}
/**
 * 破棄
 */
PlayerControllScene::~PlayerControllScene(){
	// モデル破棄
	MV1DeleteModel( playerModelId_ ); playerModelId_ = -1;
	MV1DeleteModel( targetModelId_ ); targetModelId_ = -1;
}

/**
 * 更新
 */
void PlayerControllScene::onUpdate(){
	PlayerUnit unit( g_->keypad1, player_, target_, camera_ );
	PlayerController ctrler( unit );
	
	ctrler.controll();
	ctrler.update();
}

/**
 * 描画
 */
void PlayerControllScene::onDraw(){
	ClearDrawScreen(); // クリア
	
	// カメラ
	{
		SetupCamera_Perspective( F_PI * 30.f / 180.f ); // 画角
		SetCameraNearFar( 1.0f, 3000.0f ); // 前面・背面の距離
		
		SetCameraPositionAndTarget_UpVecY( camera_.pos, camera_.target ); // カメラ姿勢
	}
	
	// プレイヤー描画
	{
		MATRIX matrix;
		
		MV1SetPosition( playerModelId_, player_.pos );
		
		matrix = MMult( MGetRotX( player_.rot.x ), MGetRotY( player_.rot.y ) );
		matrix = MMult( MGetRotZ( player_.rot.z ), matrix );
		
		MV1SetRotationMatrix( playerModelId_, matrix );
		
		MV1DrawModel( playerModelId_ );
	}

	// ターゲット描画
	{
		MV1SetWriteZBuffer( targetModelId_, FALSE ); // Z バッファ更新しない
		
		// 1 つ目
		MV1SetPosition( targetModelId_, target_.pos );
		
		MV1DrawModel( targetModelId_ );
		
		// 2 つ目
		Vector3f point = player_.pos + target_.pos;
		point.x *= 0.5f;
		point.y *= 0.5f;
		point.z *= 0.5f;
		MV1SetPosition( targetModelId_, point );
		
		MV1DrawModel( targetModelId_ );
		
		MV1SetWriteZBuffer( targetModelId_, TRUE ); // Z バッファ更新する(元に戻す)
	}
	
	// HUD
	{
		char msg[ 256 ] = { 0 };
		
		//sprintf( msg, "cnt = %d", cnt_ );
		//DrawString( 0, 0, msg, GetColor( 255, 255, 255 ) ) ;
		
		sprintf( msg, "pos(%3.3f, %3.3f, %3.3f)", player_.pos.x, player_.pos.y, player_.pos.z );
		DrawString( 0, 30, msg, GetColor( 255, 255, 255 ) ) ;
		
		sprintf( msg, "rot(%3.3f, %3.3f, %3.3f)", player_.rot.x, player_.rot.y, player_.rot.z );
		DrawString( 0, 60, msg, GetColor( 255, 255, 255 ) ) ;
		
		//sprintf( msg, "relative(%3.3f, %3.3f, %3.3f)", relative.x, relative.y, relative.z );
		//DrawString( 0, 60, msg, GetColor( 255, 255, 255 ) ) ;
		//
		//sprintf( msg, "target(%3.3f, %3.3f, %3.3f)", target.pos.x, target.pos.y, target.pos.z );
		//DrawString( 0, 90, msg, GetColor( 255, 255, 255 ) ) ;
	}
	
	ScreenFlip(); // 描画完了
}







//================================================== シーンファクトリ ==================================================

/**
 * シーン生成
 */
static Scene* createScene( SceneCode code, Global* g ){
	if ( code == SceneCodeEmpty4Develop ) return new EmptyScene( g );
	if ( code == SceneCodePlayerControll4Develop ) return new PlayerControllScene( g );
	
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
