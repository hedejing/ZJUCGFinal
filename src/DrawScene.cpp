#include "DrawScene.h"
#include <vector>
#include "Physics.h"
vector<Rect> skybox;
void DrawSkyBox()
{
	for (auto t : skybox)
	{
		t.draw();
	}
}
void InitSkyBox(double length)
{
	
	GLuint tex;
	tex = loadTexture("teide/negx.bmp");
	Rect tmp1(Point(-length/2, 0, 0), tex);
	tmp1.scale(length);
	tmp1.rotate(90, 1, 0, 0);
	tmp1.rotate(90, 1, 0, 0);
	//tmp->rotate(180, 0, 1, 0);
	

	tex = loadTexture("teide/posx.bmp");
	Rect tmp2(Point(length/2, 0, 0), tex);
	tmp2.scale(length);
	tmp2.rotate(90, 1, 0, 0);
	tmp2.rotate(90, 1, 0, 0);
	tmp2.rotate(180, 0, 1, 0);
	
	

	tex = loadTexture("teide/negz.bmp");
	Rect tmp3(Point(0, 0, -length/2), tex);
	tmp3.scale(length);
	tmp3.rotateTo(90, 0, 1, 0);

	tex = loadTexture("teide/posz.bmp");
	Rect tmp4(Point(0, 0, length/2), tex);
	tmp4.scale(length);
	tmp4.rotateTo(90, 0, 1, 0);
	tmp4.rotate(180, 0, 1, 0);

	tex = loadTexture("teide/negy.bmp");
	Rect tmp5(Point(0, -length/2, 0), tex);
	tmp5.scale(length);
	tmp5.rotate(270, 1, 0, 0);
	tmp5.rotate(90, 0, 0, 1);
	

	tex = loadTexture("teide/posy.bmp");
	Rect tmp6(Point(0, length/2, 0), tex);
	tmp6.scale(length);
	//tmp->rotate(90, 0, 1, 0);
	//tmp->rotate(90, 0, 1, 0);
	tmp6.rotate(90, 1, 0, 0);
	tmp6.rotate(-90, 0, 0, 1);
	skybox.push_back(tmp1);
	skybox.push_back(tmp2);
	skybox.push_back(tmp3);
	skybox.push_back(tmp4);
	skybox.push_back(tmp5);
	skybox.push_back(tmp6);
	return;
}
using namespace std;
struct wall_info{
	Vec size;
	GLuint texutre;
	Point position;
};
vector<wall_info> Walls;
bool PushSceneOfName(string name)
{
	//push all the objects into the vector
	return 0;
}
void DrawScene_Of_CS(string name)
{
//	DrawSkyBox(1999);
	//Draw walls
	for (auto wall : Walls)
	{
		Cube *tmp = new Cube(wall.position, wall.texutre);
		tmp->scaleValue=wall.size;
		btRigidBody *temp = Physics::CreateSimpleRigidBody(tmp, SimpleElementType::CUBE,1000000);
	}
}