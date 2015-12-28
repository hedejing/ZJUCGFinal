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
	~BasicElement();
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
};

class Cube : public BasicElement {
public:
	Cube(Point p);
	void drawNaive();
};

class Rect : public BasicElement {
public:
	unsigned int tex;
	Rect(Point p, unsigned int texture=0);
	Rect(unsigned int texture=0);
	void drawNaive();
};


#endif
