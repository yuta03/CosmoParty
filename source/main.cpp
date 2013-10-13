//================================================== �C���N���[�h ==================================================
#include "../DxLib.h"

#define _USE_MATH_DEFINES
# include <math.h>
#undef _USE_MATH_DEFINES
#include <float.h>

#include <string>
#include <vector>

#pragma comment ( lib, "winmm.lib" )

//================================================== ���O��� ==================================================
using namespace std;

//================================================== ���[�e�B���e�B ==================================================

//-------------------- 3D --------------------

/**
 * 3D�x�N�g��
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
 * ����
 */
Vector3f::Vector3f(){
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}
/**
 * ����(�R�s�[)
 */
Vector3f::Vector3f( const VECTOR& rhs ){
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
}
/**
 * ����
 */
Vector3f::Vector3f( float _x, float _y, float _z ){
	x = _x;
	y = _y;
	z = _z;
}
/**
 * �j��
 */
Vector3f::~Vector3f(){
}
/**
 * �l�ύX
 */
Vector3f& Vector3f::set( float _x, float _y, float _z ){
	x = _x;
	y = _y;
	z = _z;
	return *this;
}
/**
 * �R�s�[
 */
Vector3f& Vector3f::operator =( const VECTOR& rhs ){
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}
/**
 * ���Z
 */
Vector3f& Vector3f::operator +=( const VECTOR& rhs ){
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}
/**
 * �����ύX
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
 * �����擾
 */
float Vector3f::getLength() const{
	return ( float )sqrt( getLength2() );
}
/**
 * �����̂Q��擾
 */
float Vector3f::getLength2() const{
	return x * x + y * y + z * z;
}

/**
 * ���Z
 */
const Vector3f operator +( const VECTOR& lhs, const VECTOR& rhs ){
	return Vector3f( lhs.x + rhs.x,  lhs.y + rhs.y, lhs.z + rhs.z );
}
/**
 * ���Z
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

static const float F_PI = ( float )M_PI; /** float �^�̃� */

//-------------------- �L�[ --------------------

/**
 * �L�[�p�b�h
 */
class Keypad{
	int pressSt_; /** ������Ă���L�[ */
	int pushSt_; /** �����ꂽ�L�[ */
	int releaseSt_; /** �����ꂽ�L�[ */
	
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
 * �L�[�p�b�h����
 */
Keypad::Keypad():
pressSt_( 0 ), pushSt_( 0 ), releaseSt_( 0 ){
}
/**
 * �L�[�p�b�h�j��
 */
Keypad::~Keypad(){
}

/**
 * �L�[�p�b�h��ԍX�V
 */
Keypad& Keypad::update(){
	int prev = pressSt_;
	pressSt_ = GetJoypadInputState( DX_INPUT_KEY_PAD1 );
	releaseSt_ = prev & ( pressSt_ ^ prev );
	pushSt_  = pressSt_ & ( pressSt_ ^ prev );
	
	return *this;
}

/**
 * ������Ă��邩�ǂ���
 */
bool Keypad::isPressed( int keyCode ) const{
	return ( pressSt_ & keyCode ) != 0;
}
/**
 * �����ꂽ���ǂ���
 */
bool Keypad::isPushed( int keyCode ) const{
	return ( pushSt_ & keyCode ) != 0;
}
/**
 * �����ꂽ���ǂ���
 */
bool Keypad::isReleased( int keyCode ) const{
	return ( releaseSt_ & keyCode ) != 0;
}

//-------------------- �V�[�� --------------------

struct Global;

/**
 * �e�V�[���N���X�̃C���^�[�t�F�C�X
 */
class Scene{
public:
	Scene(){}
	virtual ~Scene(){}
	
	virtual void onUpdate() = 0;
	virtual void onDraw() = 0;
};


//================================================== �O���[�o���I�u�W�F�N�g ==================================================

/**
 * �V�[���R�[�h
 */
enum SceneCode{
	SceneCodeEmpty4Develop = 0, /** �y�J���p�z��V�[�� */
	SceneCodePlayerControll4Develop, /** �y�J���p�z�v���C���[����V�[�� */
};

/**
 * �S�̂Ŏg���I�u�W�F�N�g�S
 */
struct Global{
	Keypad keypad1; /** �L�[�p�b�h */
	Scene* scene; /** �V�[�� */
	SceneCode sceneCode; /** �V�[���R�[�h */
	
	Global();
	~Global();
};

/**
 * ����
 */
Global::Global():
keypad1(),
scene( NULL ), sceneCode( SceneCodePlayerControll4Develop ){
}

/**
 * �j��
 */
Global::~Global(){
	delete scene; scene = NULL;
}

//================================================== �y�J���p�z��V�[�� ==================================================

/**
 * ��V�[��
 */
class EmptyScene : public virtual Scene{
public:
	EmptyScene( Global* g );
	~EmptyScene();
	
	void onUpdate();
	void onDraw();

private:
	Global* g_; /** �O���[�o���I�u�W�F�N�g */
};

/**
 * ����
 */
EmptyScene::EmptyScene( Global* g ):
Scene(),
g_( g ){
}
/**
 * �j��
 */
EmptyScene::~EmptyScene(){
}

/**
 * �X�V
 */
void EmptyScene::onUpdate(){
}

/**
 * �`��
 */
void EmptyScene::onDraw(){
	ClearDrawScreen(); // �N���A
	ScreenFlip(); // �`�抮��
}



//================================================== �y�J���p�z�v���C���[����V�[�� ==================================================


/**
 * �J�������
 */
struct Camera{
	Vector3f position; /** �ʒu */
	Vector3f target; /** �Ώ� */
};

/**
 * �v���C���[���
 */
struct Player{
	Vector3f position; /** �ʒu */
};

/**
 * �v���C���[����V�[��
 */
class PlayerControllScene : public virtual Scene{
public:
	PlayerControllScene( Global* g );
	~PlayerControllScene();
	
	void onUpdate();
	void onDraw();

private:
	Global* g_; /** �O���[�o���I�u�W�F�N�g */
	int modelId_; /** ���f�� ID */
	
	Player player_; /** �v���C���[ */
	Camera camera_; /** �J���� */
};

/**
 * ����
 */
PlayerControllScene::PlayerControllScene( Global* g ):
Scene(),
g_( g ),
modelId_( -1 ),
player_(), camera_(){
	// ���f���ǂݍ���
	modelId_ = MV1LoadModel( "resource/airplane_player.x" );
}
/**
 * �j��
 */
PlayerControllScene::~PlayerControllScene(){
	// ���f���j��
	MV1DeleteModel( modelId_ ); modelId_ = -1;
}

/**
 * �X�V
 */
void PlayerControllScene::onUpdate(){
	// �v���C��
	player_.position.set( 0.0f, 0.0f, 0.0f );
	
	// �J�����ݒ�
	camera_.target = player_.position;
	camera_.position = camera_.target + Vector3f( 0.0f, 1.0f, -6.0f );
}

/**
 * �`��
 */
void PlayerControllScene::onDraw(){
	ClearDrawScreen(); // �N���A
	
	// �J����
	{
		SetupCamera_Perspective( F_PI * 30.f / 180.f ); // ��p
		SetCameraNearFar( 1.0f, 1000.0f ); // �O�ʁE�w�ʂ̋���
		
		SetCameraPositionAndTarget_UpVecY( camera_.position, camera_.target ); // �J�����p��
	}
	
	// �v���C���[�`��
	{
		MV1SetPosition( modelId_, player_.position );
		MV1SetScale( modelId_, Vector3f( 0.0075f, 0.0075f, 0.0075f ) );
		
		MV1DrawModel( modelId_ ); // �`��
	}
	
	ScreenFlip(); // �`�抮��
}







//================================================== �V�[���t�@�N�g�� ==================================================

/**
 * �V�[������
 */
static Scene* createScene( SceneCode code, Global* g ){
	if ( code == SceneCodeEmpty4Develop ) return new EmptyScene( g );
	if ( code == SceneCodePlayerControll4Develop ) return new PlayerControllScene( g );
	
	return NULL;
}

//================================================== �G���g���|�C���g ==================================================

/**
 * ���C�����[�v
 */
static void runMainLoop(){
	Global global;
	
	// ������
	global.scene = createScene( global.sceneCode, &global );
	
	// ���݂̃V�[���R�[�h
	SceneCode sceneCode = global.sceneCode;
	
	// �Q�[�����[�v
	while( ProcessMessage() == 0 && CheckHitKey( KEY_INPUT_ESCAPE ) == 0 ){
		// �I������
		if ( CheckHitKey( KEY_INPUT_ESCAPE ) != 0 ){
			break;
		}
		
		unsigned int bgnTm = timeGetTime(); // �t���[���̊J�n����
		
		// �L�[����
		global.keypad1.update();
		
		if ( sceneCode != global.sceneCode ){
			sceneCode = global.sceneCode;
			delete global.scene;
			global.scene = NULL;
			global.scene = createScene( global.sceneCode, &global );
		}
		
		// �V�[���X�V
		global.scene->onUpdate();
		global.scene->onDraw();
		
		// �X���[�v
		unsigned int endTm = timeGetTime(); // �t���[���̏I������
		int cost = endTm - bgnTm;
		if ( cost < 1 ) cost = 1;
		if ( cost > 16 ) cost = 16;
		
		Sleep( 16 - cost ); // �X���[�v���鎞��
	}
}

/**
 * �G���g���|�C���g
 */
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ){
	ChangeWindowMode( TRUE ); // �E�B���h�E���[�h
	//SetGraphMode( 1280, 720, 16 ); // �𑜓x(HD)
	SetGraphMode( 640, 360, 16 ); // �𑜓x(HVGAW)
	if( DxLib_Init() == -1 ){
		// �c�w���C�u��������������
		 return -1; // �G���[���N�����璼���ɏI��
	}
	
	SetDrawScreen( DX_SCREEN_BACK ); // �`���ݒ�
	timeBeginPeriod(1); // ���Ԑ��x
	
	runMainLoop(); // ���C�����[�v
	
	timeEndPeriod(1); // ���Ԑ��x
	DxLib_End(); // �c�w���C�u�����g�p�̏I������
	
	return 0;
}
