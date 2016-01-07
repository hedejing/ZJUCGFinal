#ifndef _Model_H_
#define _Model_H_
#include "GLUT.H"
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include "texture.h"
#define IF_KA 1
#define IF_KD 2
#define IF_KS 4
#define IF_TF 8
#define IF_KE 16
#define IF_IL 32
#define IF_KD_TE 64
#define IF_KA_TE 128
#define IF_KS_TE 256
#define IF_NS_TE 512
#define IF_NS 1024
#define Model_ERROR1 "List ID Initialization failure"
#define Model_ERROR2 "OBJ file open failure"
class Mtl_info{
protected:
	struct A_mtl{
		int content;
		float ka[3], kd[3], ks[3], tf[3],ke[3];
		int illum_type;
		float d, ns, sh, ni;
		GLuint Kd_Texture_ID,Ka_Texture_ID,Ks_Texture_ID,Ns_Texture_ID;
	};
public:
	std::map<std::string, A_mtl> mtls;
	Mtl_info(const char *filename);
	~Mtl_info();
	void change_MTL(std::string mtl_name);
};
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
	  struct group{
		  int begin_index, end_index;
		  std::string name;
	  };
	  GLuint DrawListID;
	  std::vector<v> vertices;
	  std::vector<v> vert_nor;
      std::vector<f> faces,face_tex;
	  std::vector<vt> texture_ord;
	  std::vector<f> normals;
	  std::vector<group> g, mtl_group;
	  Mtl_info *obj_mtl=NULL;
public:
	//在绘制obj之前记得读取一个obj文件，在opengl初始化之后再读取
	  virtual void Readobj(const char * filename);
	  //绘制函数
	  static GLuint LoadTGA(const char *filename);//用于读取TGA类型的纹理
	  virtual void draw();
	  Model();
	  //拷贝构造
	  Model(const Model &);
      ~Model(void);
};
#endif