#ifndef __BasicElement_H__
#define __BasicElement_H__
#include "Model.h"
#include "utility.h"

/*  基本三维物体  */
class BasicElement {
private:

public:
	unsigned int id;
	Point centroid;  //体心
	Quat rotateQuat;  //GLMat rotateMat;
	Vec scaleValue;
	int classType;  //1为子弹，2为可以减血的物体，一般为0
	BasicElement();
	virtual ~BasicElement();
	unsigned int getId();
	virtual bool shouldBeRemoved();


	/*  draw函数不仅绘制了物体，还会对物体进行相应的平移和旋转操作  */
	virtual void draw();

	/*  drawNaive函数类似glutSolidTeapot函数，将物体以原点为中心进行绘制  */
	virtual void drawNaive();


	/*  此函数的使用类似glRotate函数的使用，只不过它可以多次叠加使用。
		(就是rotate(90, 1, 1, 1)和rotate(100, 1, 0, 1)这两个旋转操作可以叠加)
		另外，此函数的angle参数为角度制                                        */
	void rotate(double angle, double x, double y, double z);  //这个函数貌似会出现万向节死锁问题，还是尽量用下面那个函数吧

	/*  此函数每次以初始状态为基准，使物体绕着某个向量旋转angle角度  */
	void rotateTo(double angle, double x, double y, double z);

	/*  相对位移函数  */
	void move(double dx, double dy, double dz); 

	/*  把物体的中心移动到某个位置  */
	void moveTo(Point p);

	/*  缩放函数  */
	void scale(double tx, double ty, double tz);
	void scale(Vec t);
	void scale(double t);
};

class Cube : public BasicElement {
private:
	GLuint tex=0;
public:
	Cube(Point p);
	Cube(Point p, GLuint t);
	void drawNaive();
};

class Sphere : public BasicElement {
public:
	double radius;
	int slices, stacks;
	Sphere(Point p, double radius=0.5, int slices=50, int stacks=50);
	void drawNaive();
};

class Cone : public BasicElement {
public:
	double radius, height;
	int slices, stacks;
	Cone(Point p, double radius=0.5, double height=1.0, int slices=50, int stacks=50);
	void drawNaive();
};

class Circle : public BasicElement {
public:
	double radius;
	int slices;
	Circle(Point p, double radius=0.5, int slices=100);
	void circle();
	virtual void drawNaive();
};
class Cylinder : public Circle {
public:
	double height;
	int stacks;
	Cylinder(Point p, double radius=0.5, double height=0.5, int slices=100, int stacks=100);
	void drawNaive();
};

//TODO  多面棱柱和多面棱台

class Rect : public BasicElement {
public:
	Model M;
	unsigned int tex;
	Rect(Point p, unsigned int texture=0);
	Rect(unsigned int texture=0);
	void drawNaive();
};

class AviBoard : public BasicElement {
private:
	int nowtime;//计时
	int timebase;//计时，nowtime-timebase是否大于一帧的时间
	int pixel_w;//长
	int pixel_h;//宽
	int framenum;//总帧数
	int count;
	double height;
	double weight;
	unsigned char *buffer;//avi
	unsigned char *tmpbuff;
public:
	AviBoard(Point p,double height,double weight,string videoname);
	~AviBoard(void);
	void drawNaive();
};


/*Life Bar*/
class LifeBar {
private:
	double barlen = 4, barw = 0.5;
	double max_life;
public:
	LifeBar();
	LifeBar(double _max_life);
	LifeBar(double _max_life, double len, double w);
	~LifeBar();
	void DrawBar(Point pos, double life);
};
class objectmodel : public BasicElement {
protected:
	Model* mymo;
public:
	//objectmodel(objectmodel&other)
	//{
	//	mymo = new Model(*other.mymo);
	//}
	objectmodel(Point p, string name);
	virtual ~objectmodel(void);
	//virtual void draw();
	virtual void drawNaive();
	void objectmodel::addToPhysicsWorld(double coliisionscale = 30, double weight = 100000);
};

class Monster : public objectmodel {
protected:
	int blood;
	int fullblood;
	LifeBar lifeBar;
public:
	Monster(Point p, string name);
	void subblood();
	bool shouldBeRemoved();
	void draw();
	void drawNaive();
};

class CameraModel : public BasicElement {
public:
	CameraModel(Point p);
	CameraModel();
	void drawNaive();
};


class Bullet : public BasicElement {
public:
	double radius;
	int slices, stacks;
	Bullet(Point p, double radius=0.5, int slices=50, int stacks=50);
	void drawNaive();
	bool shouldBeRemoved();
};

#endif
