#include "DrawScene.h"
#include <vector>
#include "Physics.h"
vector<Rect> skybox;
GLuint tex[6]; 
void InitSkyBox()
{
	tex[0] = loadTexture("teide/negx.bmp");
	tex[1] = loadTexture("teide/posx.bmp");
	tex[2] = loadTexture("teide/negz.bmp");
	tex[3] = loadTexture("teide/posz.bmp");
	tex[4] = loadTexture("teide/negy.bmp");
	tex[5] = loadTexture("teide/posy.bmp");
}

void drawTexRect(Point centroid, double scaleValue, GLuint tex, int face)
{
	glPushMatrix();
	glTranslated(centroid[0], centroid[1], centroid[2]);
	switch (face) {
	case 0: {
		glRotatef(180, 1,0,0);
		glRotatef(180, 0, 0, 1);
		break;
	}
	case 1: {
		glRotatef(180, 1, 0, 0);
		glRotatef(180, 0, 1, 0);
		glRotatef(180, 0, 0, 1);
		break;
	}
	case 2: {
		glRotatef(90, 0, 1, 0);
		break;
	}
	case 3: {
		glRotatef(90, 0, 1, 0);
		glRotatef(180, 0, 1, 0);
		break;
	}
	case 4: {
		glRotatef(-90, 0, 1, 0);
		glRotatef(90, 0, 0, 1);

		break;
	}
	case 5: {
		glRotatef(-90, 0, 1, 0);
		glRotatef(90, 0, 0, 1);
		break;
	}
	}
	glScaled(scaleValue, scaleValue, scaleValue);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0, 0.5, 0.5);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0, 0.5, -0.5);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0, -0.5, -0.5);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0, -0.5, 0.5);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawSkyBox(double length)
{
	//tex = loadTexture("teide/negx.bmp");
	drawTexRect(Point(-length / 2, 0, 0), length, tex[0], 0);
	//tex = loadTexture("teide/posx.bmp");
	drawTexRect(Point(length / 2, 0, 0), length, tex[1], 1);
	//tex = loadTexture("teide/negz.bmp");
	drawTexRect(Point(0, 0, -length / 2), length, tex[2], 2);
	//tex = loadTexture("teide/posz.bmp");
	drawTexRect(Point(0, 0, length / 2), length, tex[3], 3);
	//tex = loadTexture("teide/negy.bmp");
	drawTexRect(Point(0, -length / 2, 0), length, tex[4], 4);
	//tex = loadTexture("teide/posy.bmp");
	drawTexRect(Point(0, length / 2, 0), length, tex[5], 5);
	return;
}
using namespace std;
#define board_w 20
#define board_h 20
#define rect_width 10
vector<wall_info> Walls;
GLuint board_info[board_w][board_h];
bool iawall[board_w*rect_width][board_h*rect_width] = { false };
bool IsWall(float x, float z){
	x -= board_w*rect_width / 2;
	z -= board_h*rect_width / 2;
	return iawall[(int)x][(int)z];
}


bool PushSceneOfName(string name)
{
	//push all the objects into the vector

	wall_info a;
	GLuint tex_board = loadTexture("board.bmp");
	GLuint tex_nitu = loadTexture("nitu.bmp");
	//unsigned char  filewall[board_h][board_w];
	//FILE *fp = fopen(name.c_str(), "rb");
	//fread(filewall, board_w*board_h *rect_width * rect_width, 1, fp);//��ȡ����
	//fclose(fp);

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

	for (int i = -3; i < 5; i++)//ÖÐ¼äµÄ³¤Ç½
	{
		a.size = Vec(5, 3, 1);
		a.texutre = tex_board;
		a.position = Point(i * 2 * 5, 0.5, 0);
		for (int j = a.position[0] - 5; j < a.position[0] + 5; j++)
		{
			iawall[j + board_w*rect_width / 2][0 + board_w*rect_width / 2] = true;
			iawall[j + board_w*rect_width / 2][-1 + board_w*rect_width / 2] = true;
		}
		Walls.push_back(a);

	}
	for (int i = -3; i < 8; i++)
	{
		a.size = Vec(5, 3, 1);
		a.texutre = tex_board;
		a.position = Point(i * 2 * 5, 0.5, 38);
		for (int j = a.position[0] - 5; j < a.position[0] + 5; j++)
		{
			iawall[j + board_w*rect_width / 2][38 + board_w*rect_width / 2] = true;
			iawall[j + board_w*rect_width / 2][37 + board_w*rect_width / 2] = true;
		}
		Walls.push_back(a);
	}
	for (int i = 0; i < 2; i++)
	{
		a.size = Vec(1, 3, 5);
		a.texutre = tex_board;
		a.position = Point(46, -1.5, 4 + i * 2 * 5);
		for (int j = a.position[2] - 5; j < a.position[2] + 5; j++)
		{
			iawall[45 + board_w*rect_width / 2][j + board_w*rect_width / 2] = true;
			iawall[46 + board_w*rect_width / 2][j + board_w*rect_width / 2] = true;
		}
		Walls.push_back(a);
	}

	for (int i = -4; i < 4; i++)
	{
		a.size = Vec(1, 3, 5);
		a.texutre = tex_board;
		a.position = Point(-36, -1.5, 4 + i * 2 * 5);
		for (int j = a.position[2] - 5; j < a.position[2] + 5; j++)
		{
			iawall[-36 + board_w*rect_width / 2][j + board_w*rect_width / 2] = true;
			iawall[-37 + board_w*rect_width / 2][j + board_w*rect_width / 2] = true;
		}
		Walls.push_back(a);
	}

	for (int i = -3; i < 1; i++){
		a.size = Vec(5, 3, 1);
		a.texutre = tex_board;
		a.position = Point(i * 2 * 5, 0.5, -20);
		for (int j = a.position[0] - 5; j < a.position[0] + 5; j++)
		{
			iawall[j + board_w*rect_width / 2][-20 + board_w*rect_width / 2] = true;
			iawall[j + board_w*rect_width / 2][-21 + board_w*rect_width / 2] = true;
		}
		Walls.push_back(a);
	}

	for (int i = -3; i < 5; i++){
		a.size = Vec(5, 3, 1);
		a.texutre = tex_board;
		a.position = Point(i * 2 * 5, 0.5, -40);
		for (int j = a.position[0] - 5; j < a.position[0] + 5; j++)
		{
			iawall[j + board_w*rect_width / 2][-40 + board_w*rect_width / 2] = true;
			iawall[j + board_w*rect_width / 2][-41 + board_w*rect_width / 2] = true;
		}
		Walls.push_back(a);
	}
	for (int i = 0; i < 5; i++)
	{
		a.size = Vec(5, 3, 1);
		a.texutre = tex_board;
		a.position = Point(i * 2 * 5, 0.5, 18);
		for (int j = a.position[0] - 5; j < a.position[0] + 5; j++)
		{
			iawall[j + board_w*rect_width / 2][18 + board_w*rect_width / 2] = true;
			iawall[j + board_w*rect_width / 2][17 + board_w*rect_width / 2] = true;
		}
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