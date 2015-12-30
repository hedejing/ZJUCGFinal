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
	new Cube(Point(10, 0, 0));  //正方体
	new Sphere(Point(10, -2, -4));  //球
	new Cone(Point(10, -2, -2));  //圆锥
	new Circle(Point(10, -2, 2));  //圆
	Cylinder *cc = new Cylinder(Point(10, -2, 0));  //圆柱
	cc->rotateTo(45, 0, 0, 1);
}
int main(int argc, char *argv[]) {
	World::init(&argc, argv);
	//可以在这里使用glutIdleFunc();对idle进行重写
	//若要对鼠标和键盘函数进行重写，建议在World.cpp里修改
	

	
	define();
	Rect rect(Point(10, 2, 0), loadTexture("images//win7.bmp"));
	rect.scale(3, 3, 4);



	{  //LIGHTING
		GLfloat lightPos0[] = {5,5,5,1};
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, Color(1, 1, 1));//COLOR::white);
		glEnable(GL_LIGHT0);  //light0只是用做示例
	}




	glutMainLoop();
	return 0;
}
