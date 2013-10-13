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
scene( NULL ), sceneCode( SceneCodeEmpty4Develop ){
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








//================================================== �V�[���t�@�N�g�� ==================================================

/**
 * �V�[������
 */
static Scene* createScene( SceneCode code, Global* g ){
	if ( code == SceneCodeEmpty4Develop ) return new EmptyScene( g );
	
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
