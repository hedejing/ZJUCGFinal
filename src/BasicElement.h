#ifndef __BasicElement_H__
#define __BasicElement_H__

#include "utility.h"

/*  基本三维物体  */
class BasicElement {
private:
	unsigned int id;

public:
	Point centroid;  //体心
	Vec scaleValue;
	BasicElement();
	virtual ~BasicElement();
	unsigned int getId();


	/*  draw函数不仅绘制了物体，还会对物体进行相应的平移和旋转操作  */
	void draw();

	/*  drawNaive函数类似glutSolidTeapot函数，将物体以原点为中心进行绘制  */
	virtual void drawNaive();


	/*  此函数的使用类似glRotate函数的使用，只不过它可以多次叠加使用。
		(就是rotate(90, 1, 1, 1)和rotate(100, 1, 0, 1)这两个旋转操作可以叠加)
		另外，此函数的angle参数为角度制                                        */
	void rotate(double angle, double x, double y, double z);

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
public:
	Cube(Point p);
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
	unsigned int tex;
	Rect(Point p, unsigned int texture=0);
	Rect(unsigned int texture=0);
	void drawNaive();
};


#endif
