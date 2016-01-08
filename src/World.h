#ifndef __World_H__
#define __World_H__

#include <map>
#include <deque>
#include "LightSource.h"
#include "BasicElement.h"
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

	static Point eye, center;
	static const Vec up;  //up向量不能被修改
	static double elevation[2];
	static double moveSpeed;
	static double rotateSpeed;
	static double zoomFactor, zoomSpeed;

	static unsigned int chosenID;

	/*  游戏模式：0，上帝模式；1，游戏模式  */
	static int gameMode;
	static int gameModeNum;


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
		相机仰角在[-75°,75°]之间                                                            */
	static void rotate(int d, double step);

	/*  zoom  */
	static void zoom(double d);


	/*  一次性绘制所有物体  */
	static void drawAll();

	/*  外部可传入绘制函数  */
	static void (*_display)();
	static void(*_free)();

	friend class BasicElement;


	/*  GLU FUNC  */
	static void perspective();
	static void lookAt();


	/*  GLUT FUNC  */
	static int mouseState[3];

	static void idle();  //加上static后就可以传入glutIdleFunc()里了
	static void display();
	static void keyboard(unsigned char key, int x, int y);
	static void special(int key, int x, int y);
	static void motion(int x, int y);
	static void passiveMotion(int x, int y);
	static void entry(int state);
	static void mouseClick(int button, int state, int x, int y);

		/*  点选辅助函数  */
		static void gl_select(int x, int y);
		static void mousedw(int x, int y, int but);

	//TODO  碰撞检测、光照处理等函数需要添加


	/*  截屏  */
	static string scrennshotsDir;
	static void grabScreen();


};



#endif
