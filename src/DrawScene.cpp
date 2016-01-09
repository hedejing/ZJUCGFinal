#include "DrawScene.h"
#include <vector>
#include "Physics.h"
vector<Rect> skybox;
void DrawSkyBox()
{
	for (auto t : skybox)
	{
		printf("done");
		t.draw();
	}
}
void InitSkyBox(double length)
{
	
	GLuint tex;
	tex = loadTexture("teide/negx.bmp");
	Rect *tmp1=new Rect(Point(-length/2, 0, 0), tex);
	tmp1->scale(length);
	tmp1->rotate(90, 1, 0, 0);
	tmp1->rotate(90, 1, 0, 0);
	//tmp->rotate(180, 0, 1, 0);
	

	tex = loadTexture("teide/posx.bmp");
	Rect *tmp2=new Rect(Point(length/2, 0, 0), tex);
	tmp2->scale(length);
	tmp2->rotate(90, 1, 0, 0);
	tmp2->rotate(90, 1, 0, 0);
	tmp2->rotate(180, 0, 1, 0);
	
	

	tex = loadTexture("teide/negz.bmp");
	Rect *tmp3=new Rect(Point(0, 0, -length/2), tex);
	tmp3->scale(length);
	tmp3->rotateTo(90, 0, 1, 0);

	tex = loadTexture("teide/posz.bmp");
	Rect *tmp4=new Rect(Point(0, 0, length/2), tex);
	tmp4->scale(length);
	tmp4->rotateTo(90, 0, 1, 0);
	tmp4->rotate(180, 0, 1, 0);

	tex = loadTexture("teide/negy.bmp");
	Rect *tmp5=new Rect(Point(0, -length/2, 0), tex);
	tmp5->scale(length);
	tmp5->rotate(270, 1, 0, 0);
	tmp5->rotate(90, 0, 0, 1);
	

	tex = loadTexture("teide/posy.bmp");
	Rect *tmp6=new Rect(Point(0, length/2, 0), tex);
	tmp6->scale(length);
	//tmp->rotate(90, 0, 1, 0);
	//tmp->rotate(90, 0, 1, 0);
	tmp6->rotate(90, 1, 0, 0);
	tmp6->rotate(-90, 0, 0, 1);
	/*skybox.push_back(tmp1);
	skybox.push_back(tmp2);
	skybox.push_back(tmp3);
	skybox.push_back(tmp4);
	skybox.push_back(tmp5);
	skybox.push_back(tmp6);
	*/return;
}
using namespace std;
struct wall_info{
	Vec size;
	GLuint texutre;
	Point position;
};


#define board_w 20
#define board_h 20
#define rect_width 10
vector<wall_info> Walls;
GLuint board_info[board_w][board_h];
bool PushSceneOfName(string name)
{
	//push all the objects into the vector

	wall_info a;
	GLuint tex_board = loadTexture("board.bmp");
	GLuint tex_nitu = loadTexture("nitu.bmp");
	for (int i = 0; i < board_h; i++)
	{
		for (int j = 0; j < board_w; j++)
		{
			if ((i - board_h / 2)*(i - board_h / 2) + (j - board_w / 2)*(j - board_w / 2) < (0.9*board_w / 2)*(0.9*board_h / 2))
				board_info[i][j] = tex_board;
			else
				board_info[i][j] = tex_nitu;
		}
	}

	for (int i = -3; i < 5; i++)
	{
		a.size = Vec(5, 3, 1);
		a.texutre = tex_board;
		a.position = Point(i * 2 * 5, 0.5, 0);
		Walls.push_back(a);
	}

	for (int i = 0; i < 2; i++)
	{
		a.size = Vec(1, 3, 5);
		a.texutre = tex_board;
		a.position = Point(46, -1.5, 4 + i * 2 * 5);
		Walls.push_back(a);
	}

	for (int i = 0; i < 4; i++)
	{
		a.size = Vec(1, 3, 5);
		a.texutre = tex_board;
		a.position = Point(-36, -1.5, 4 + i * 2 * 5);
		Walls.push_back(a);
	}

	for (int i = 0; i < 5; i++)
	{
		a.size = Vec(5, 3, 1);
		a.texutre = tex_board;
		a.position = Point(i * 2 * 5, 0.5, 18);
		Walls.push_back(a);
	}


	return 0;
}
void DrawScene_Of_CS(string name)
{
	//	DrawSkyBox(1999);
	//Draw walls
	PushSceneOfName(name);
	for (auto wall : Walls)
	{
		Cube *tmp = new Cube(wall.position, wall.texutre);
		tmp->scaleValue = wall.size;
		btRigidBody *temp = Physics::CreateSimpleRigidBody(tmp, SimpleElementType::CUBE, 1000000);
		btCollisionShape* shape = new  btBoxShape(btVector3(wall.size[0] - 0.005, wall.size[1] - 0.005, wall.size[2] - 0.005));
		btDefaultMotionState* MotionState =
			new btDefaultMotionState(btTransform(
			btQuaternion(0, 0, 0, 1),
			btVector3(wall.position[0], wall.position[1], wall.position[2])));
		btVector3 Inertia(0, 0, 0);
		shape->calculateLocalInertia(10000000, Inertia);
		btRigidBody::btRigidBodyConstructionInfo RigidBodyCI(10000000, MotionState, shape, Inertia);
		btRigidBody* RigidBody = new btRigidBody(RigidBodyCI);
		Physics::AddRigidBodyAndElement(RigidBody, tmp);
	}
	for (int i = 0; i < board_w; i++)
	{
		for (int j = 0; j < board_h; j++)
		{
			Rect *tmp = new Rect(Point(rect_width*(i - board_w / 2), 0, rect_width*(j - board_h / 2)), board_info[i][j]);
			tmp->scale(rect_width);
			tmp->rotateTo(90, 0, 0, 1);
		}
	}
}