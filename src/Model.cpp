#include "Model.h"
#include <fstream>
using namespace std;
void Model::draw()
{
	glCallList(DrawListID);
}
Model::Model()
{
	DrawListID=glGenLists(1);
	if (DrawListID==0) throw Model_ERROR1;

}
Model::Model(const Model &another)
{
	DrawListID=another.DrawListID;
}
void Model::Readobj(const char * filename)
{
ifstream fin;
	fin.open(filename,ifstream::in);
	if (!fin) {
	//	throw Model_ERROR2;
		return;}
	string s;
	while (fin>>s)
	{
		switch(*s.c_str())
		{
		case '#':
			 getline(fin,s);
			break;
		case 'v':
			 if (*(s.c_str()+1)=='n')//法线向量
			 {
				v vertex;
				fin>>vertex.x>>vertex.y>>vertex.z;
				vert_nor.push_back(vertex);
			 }
			 else if (*(s.c_str()+1)=='t')//纹理坐标
			 {
				vt vertex;
				fin>>vertex.x>>vertex.y;
				texture_ord.push_back(vertex);
			 }
			 else//顶点
			 {
				v vertex;
				fin>>vertex.x>>vertex.y>>vertex.z;
				vertices.push_back(vertex);
			 }
			break;
		case 'f'://面信息
			{
				f face,tex,normal;

				char useless;
				fin>>face.v1>>useless>>tex.v1>>useless>>normal.v1//过滤掉斜杠
					>>face.v2>>useless>>tex.v2>>useless>>normal.v2
					>>face.v3>>useless>>tex.v3>>useless>>normal.v3;
				faces.push_back(face);
				face_tex.push_back(tex);
				normals.push_back(normal);
					break;
			}
		}
	}
	glNewList(DrawListID,GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (int i=0;i<faces.size();i++)
	{
		v v1=vertices[faces[i].v1-1];
		v v2=vertices[faces[i].v2-1];
		v v3=vertices[faces[i].v3-1];
		vt tex1=texture_ord[face_tex[i].v1-1];
		vt tex2=texture_ord[face_tex[i].v2-1];
		vt tex3=texture_ord[face_tex[i].v3-1];
		v nor1=vert_nor[normals[i].v1-1];
		v nor2=vert_nor[normals[i].v2-1];
		v nor3=vert_nor[normals[i].v3-1];
		glNormal3f(nor1.x,nor1.y,nor1.z);//第一个顶点
		glTexCoord2f(tex1.x,tex1.y);
		glVertex3f(v1.x,v1.y,v1.z);
		glNormal3f(nor2.x,nor2.y,nor2.z);//第二个顶点
		glTexCoord2f(tex2.x,tex2.y);
		glVertex3f(v2.x,v2.y,v2.z);
		glNormal3f(nor3.x,nor3.y,nor3.z);//第三个顶点
		glTexCoord2f(tex3.x,tex3.y);
		glVertex3f(v3.x,v3.y,v3.z);
	}
	glEnd();
	glEndList();
	vertices.clear();
	faces.clear();
	face_tex.clear();
	texture_ord.clear();
	normals.clear();
}
Model::~Model()
{
	glDeleteLists(DrawListID,1);
}