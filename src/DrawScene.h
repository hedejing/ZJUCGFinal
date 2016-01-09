#ifndef _DRAW_SCENE_H_
#define _DRAW_SCENE_H_
#include "utility.h"
#include "BasicElement.h"
struct wall_info{
	Vec size;
	GLuint texutre;
	Point position;
};
void getTreesObj();
void DrawScene_Of_CS(string name);
//void DrawSkyBox();
void InitSkyBox();
void drawSkyBox(double length);
#endif