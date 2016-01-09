#include "DrawScene.h"
#include <vector>
#include "Physics.h"
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
	//Draw walls
	for (auto wall : Walls)
	{
		Cube *tmp = new Cube(wall.position, wall.texutre);
		tmp->scaleValue=wall.size;
		btRigidBody *temp = Physics::CreateSimpleRigidBody(tmp, SimpleElementType::CUBE,1000000);
	}
}