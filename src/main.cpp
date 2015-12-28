#include "World.h"
#include "BasicElement.h"
#include "texture.h"
using namespace std;

int main(int argc, char *argv[]) {
	World::init(&argc, argv);
	Cube cube(Point(10, 0, 0));
	Rect rect(Point(10, 2, 0), loadTexture("images//win7.bmp"));
	rect.scale(3, 3, 4);
	//可以在这里使用glutIdleFunc();对idle进行重写
	//若要对鼠标和键盘函数进行重写，建议在World.cpp里修改
	glutMainLoop();
	return 0;
}