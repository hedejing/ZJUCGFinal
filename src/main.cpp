#include "World.h"
#include "BasicElement.h"
#include "texture.h"
#include "test.h"
using namespace std;


/*
	两种定义物体的方法：
	1. 在World::init()和glutMainLoop()之间定义。
	   (可以把定义写在World::int()之前，不过纹理定义等需要用到OpenGL函数的东西，必须要写在World::int()之后)
	2. 另开一个函数，然后用new来定义。当然，函数的引用位置也要在World::init()和glutMainLoop()之间。
	   (用new出来的指针可以对物体进行操作)


	注：在函数中如果不用new定义，就不能显示物体是因为函数结束时会调用那个对象的析构函数，然后那个对象就没了...
*/


void define() {
	//Cube *c = new Cube(Point(2, 2, 0));
	//c->rotateTo(45, 1, 0, 0);  //正方体
	//new Sphere(Point(2, 1, 2));  //球
	//new Cone(Point(2, 2, -3));  //圆锥
	//new Circle(Point(-2, 2, 2));  //圆
	//Cylinder *cc = new Cylinder(Point(2, -2, 4));  //圆柱
	//cc->rotateTo(45, 0, 0, 1);
}
void draw_world()
{
	//glStaff::xyz_frame(2,2,2,false);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(90, -1, 0, 0);
	glTranslatef(-10, -10, 0);
	LightManager::xoy_foor(20, 3, 100);
	glPopMatrix();
	glPushMatrix();

	glTranslatef(0, 0, 0);
	glutSolidCube(2);

	glTranslatef(2, 0, 2);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(1, 2, 50, 50);
	glRotatef(90, 1, 0, 0);

	glTranslatef(0, 0, -4);
	glutSolidSphere(1, 50, 50);

	glTranslatef(-4, 1, 0);
	glRotatef(45, 1, 0, 0);
	glutSolidTorus(0.5f, 1, 50, 50);
	glRotatef(-45, 1, 0, 0);
	glPopMatrix();
}
int main(int argc, char *argv[]) {
	World::init(&argc, argv);
	World::setCamera(Point(-10, 10, 10), Point(0, 0, 0));
	//可以在这里使用glutIdleFunc();对idle进行重写
	//若要对鼠标和键盘函数进行重写，建议在World.cpp里修改
	

	
	define();
	//Rect rect(Point(0, -3, 0));// , loadTexture("images//win7.bmp"));
	//rect.rotateTo(90, 0, 0, 1);
	//rect.scale(3, 3, 4);

	//
	//Rect rect2(Point(5, 1, 0));// , loadTexture("images//win7.bmp"));
	//rect2.scale(3, 3, 4);
	////rect2.rotateTo(90, 1, 0, 0);
	//rect2.rotate(90, 0, 1, 0);

	World::_display = draw_world;
	{  //LIGHTING
		LightSource ls(Point(5, 6, -2, 1));
		LightManager::addLight(ls);
		LightSource ls2(Point(-2, 6, 5, 1));
		LightManager::addLight(ls2);
		LightSource ls3(Point(-3, 6, -3, 1));
		LightManager::addLight(ls3);
		//LightManager::lights.push_back(ls);
		//float lightPos0[] = {1, 0, 0, 1};
		//glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, Color(1, 0, 0, 0.5));//COLOR::white);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, Color(1, 0, 0, 0.5));//COLOR::white);
		glLightfv(GL_LIGHT0, GL_SPECULAR, Color(1, 0, 0, 0.5));//COLOR::white);

		glLightfv(GL_LIGHT1, GL_AMBIENT, Color(0, 0, 0, 0.5));//COLOR::white);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, Color(0, 0, 1, 0.5));//COLOR::white);
		glLightfv(GL_LIGHT1, GL_SPECULAR, Color(0, 0, 1, 0.5));//COLOR::white);

		glLightfv(GL_LIGHT2, GL_AMBIENT, Color(0, 0, 0, 0.5));//COLOR::white);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, Color(0, 1, 0, 0.5));//COLOR::white);
		glLightfv(GL_LIGHT2, GL_SPECULAR, Color(0, 1, 0, 0.5));//COLOR::white);

		/*GLfloat dir[] = { 0, 0,-1};
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);*/
		glEnable(GL_LIGHT0);  //light0只是用做示例
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	glutMainLoop();
	return 0;
}
