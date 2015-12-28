#ifndef _Model_H_
#define _Model_H_
#include "GL/GLUT.H"
#include <vector>
#include <iostream>
#include<string>
#define Model_ERROR1 "List ID Initialization failure"
#define Model_ERROR2 "OBJ file open failure"
class Model
{
protected:
      struct v{
            double x;
            double y;
            double z;
      };
      struct f{
            unsigned int v1,v2,v3;
      };
	  struct vt{
		double x,y;
	  };
	  GLuint DrawListID;
	  std::vector<v> vertices;
	  std::vector<v> vert_nor;
      std::vector<f> faces,face_tex;
	  std::vector<vt> texture_ord;
	  std::vector<f> normals;
public:
	//在绘制obj之前记得读取一个obj文件，在opengl初始化之后再读取
	  virtual void Readobj(const char * filename);
	  //绘制函数
      virtual void draw();
	  Model();
	  //拷贝构造
	  Model(const Model &);
      ~Model(void);
};
#endif