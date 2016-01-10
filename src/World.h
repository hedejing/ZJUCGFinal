#ifndef __World_H__
#define __World_H__
#define GLUT_DISABLE_ATEXIT_HACK
#include <map>
#include <deque>
#include "LightSource.h"
#include "BasicElement.h"
#include "Physics.h"
using namespace std;


class World {
private:
	static unsigned int nextId;

	/*  此map结构用于保存世界中的所有物体  */
	static map<unsigned int, BasicElement *> objects;

	/*  此双端队列用于维护已删除的三维物体id  */
	static deque<unsigned int> trash;

	static unsigned int getNextId();
	static unsigned int getNextId(BasicElement *be);
	static int erase(unsigned int id);

public:
	static int windowHandle;
	static int windowHeight, windowWidth;
	static int windowPos[2];
	static int cursorPos[2];
	static int focusState;

	static CameraModel *cameraModel;  static int changing;
	static btRigidBody *cameraModelRigidBody;
	static Point eye, center;
	static const Vec up;  //up向量不能被修改
	static double elevation[2];
	static double moveSpeed;
	static double rotateSpeed;
	static double zoomFactor, zoomSpeed;

	static unsigned int chosenID;

	/*  游戏模式：0，上帝模式；1，游戏模式  */
	static int gameMode;
	static int gameModeTotalNum;
	static const int GOD_MODE = 0;
	static const int GAME_MODE = 1;


	/*  初始化函数  */
	static void init(int *argc, char *argv[], int windowHeight=800, int windowWidth=800, int xPos=100, int yPos=100);
	static void setCursorToCenter();

	static void reCenter();
	static void setCamera(Point eye, Point center);


	/*  移动相机  */
	static void move(double dx, double dy, double dz);
	static void move(const Vec &ds);
	static void move(int d, double step);
	static void _move(double dx, double dy);

	/*  旋转相机角度(角度制)  */
	static void rotate(double angle, double x, double y, double z);

	/*  旋转相机(d=01分别表示沿水平、沿竖直方向转，d等于其他值不做事；step为正则往右转或往上转)
		相机仰角在[-45°,45°]之间                                                            */
	static void rotate(int d, double step);

	/*  将bullet那边的相机位置更改  */
	static void syncWithCameraModel();

	/*  zoom  */
	static void zoom(double d);


	/*  一次性绘制所有物体  */
	static void drawAll();

	/*  外部可传入绘制函数  */
	static void (*_display)();
	static void(*_free)();

	/*  判断子弹是否该删除  */
	static double worldBound[3][2];
	static bool isInside(Point p);

	friend class BasicElement;


	/*  GLU FUNC  */
	static void perspective();
	static void lookAt();


	/*  GLUT FUNC  */
	static int mouseState[3];
	static int keyboardPressing[256];

	static void idle();  //加上static后就可以传入glutIdleFunc()里了
	static void display();
	static void keyboard(unsigned char key, int x, int y);
	static void keyboardUp(unsigned char key, int x, int y);
	static void special(int key, int x, int y);
	static void motion(int x, int y);
	static void passiveMotion(int x, int y);
	static void entry(int state);
	static void mouseClick(int button, int state, int x, int y);

		/*  点选辅助函数  */
		static void gl_select(int x, int y);
		static void mousedw(int x, int y, int but);

		/*  键盘辅助函数  */
		static void keyboardResponse();

	//TODO  碰撞检测、光照处理等函数需要添加


	/*  截屏  */
	static string scrennshotsDir;
	static void grabScreen();


	/*  减血函数(没实现)  */
	static void subtractBlood();

	static bool couldMoveTo(Point p);

	static bool jumping;
	static double jumpHeight, jumpA, jumpV, jumpDt;
	static double getCameraHeight();
	static void jump();
	static void solveJump();
};



#endif
