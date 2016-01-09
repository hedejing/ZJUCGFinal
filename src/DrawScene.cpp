#include "DrawScene.h"
#include <vector>
#include "Physics.h"
vector<Rect> skybox;

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
	
	GLuint tex;
	tex = loadTexture("teide/negx.bmp");
	drawTexRect(Point(-length / 2, 0, 0), length, tex, 0);
	

	tex = loadTexture("teide/posx.bmp");
	drawTexRect(Point(length / 2, 0, 0), length, tex, 1);

	tex = loadTexture("teide/negz.bmp");
	drawTexRect(Point(0, 0, -length / 2), length, tex, 2);


	tex = loadTexture("teide/posz.bmp");
	drawTexRect(Point(0, 0, length / 2), length, tex, 3);


	tex = loadTexture("teide/negy.bmp");
	drawTexRect(Point(0, -length / 2, 0), length, tex, 4);

	

	tex = loadTexture("teide/posy.bmp");
	drawTexRect(Point(0, length / 2, 0), length, tex, 5);
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