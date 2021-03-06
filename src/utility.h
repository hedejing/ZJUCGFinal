#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GL/glut.h>
#include <Windows.h>
using namespace std;

#pragma comment (lib, "glew32.lib")
#define  GLUT_WHEEL_UP 3           //定义滚轮操作  
#define  GLUT_WHEEL_DOWN 4  

/*  Point and Vec  */
struct Point {
	double t[4];
	Point();
	Point(double a, double b, double c);
	Point(double a, double b, double c, double w);
	double getX();
	double getY();
	double getZ();
	double getW();
	double &operator[](int d);
	const double &operator[](int d) const;
	
	friend ostream &operator<<(ostream &out, Point a);
};
struct Vec {
	double t[4];
	Vec();
	Vec(double a, double b, double c);
	Vec(double a, double b, double c, double w);
	// hedejin : add a implict conversion
	//Vec(const Point& p);
	double getX();
	double getY();
	double getZ();
	double getW();
	double &operator[](int d);
	const double &operator[](int d) const;
	Vec operator-();
	
	double sqr();  //没考虑w
	double abs();  //向量模长
	Vec normalize();  //单位化
	
	friend ostream &operator<<(ostream &out, Vec a);
};

Point &operator+=(Point &a, const Vec &b);
Point operator+(const Point &a, const Vec &b);
Point operator-(const Point &a, const Vec &b);

Vec operator-(const Point &a, const Point &b);  //此函数没有考虑w参数
Vec operator-(const Vec &a, const Vec &b);  //此函数没有考虑w参数
Vec operator+(const Vec &a, const Vec &b);
Vec operator-(const Vec &a, const Vec &b);
Vec operator*(const Vec &a, const Vec &b);
double operator%(const Vec &a, const Vec &b);
double operator%(const Vec &a, const Point &b);
Vec operator*(Vec a, double t);
Vec operator*(double t, Vec a);
Vec operator/(Vec a, double t);




/*  GLMat  */
struct GLMat {
	double a[4][4];
	static const double EPS;
	GLMat();
	GLMat(double d);
	GLMat(double t[4][4]);
	double *operator[](int d);
	Vec operator()(int d);
	operator double *();
	GLMat transpose();
	GLMat affineInverse();

	GLMat operator*(const GLMat &t) const;
	GLMat &operator*=(GLMat t);

	friend ostream &operator<<(ostream &out, GLMat a);
};
GLMat rotateMat(double angle, double x, double y, double z);
GLMat translateMat(double x, double y, double z);
GLMat lookAtMat(Point eye, Point center, Vec up);
GLMat lookAtMat(double eyex, double eyey, double eyez, double centerx, double centery, double centerz,  double upx, double upy, double upz);
GLMat perspectiveMat(double fovy, double aspect, double zNear, double zFar);



/*  Quat  */

struct Quat {
	double w, x, y, z;
	Quat();
	Quat(double w,double x,double y,double z);
	Quat(double w, Vec v);
	Vec vec();
	double sqr();
	double norm();
	Quat normalize();
	operator GLMat();
	friend Quat operator+(Quat a, Quat b);
	friend Quat operator*(Quat a, Quat b);
	friend Quat operator/(Quat a,double d) ;
	Quat operator~();
};
Quat rotateQuat(double angle, double x, double y, double z);




/*  COLOR  */
namespace COLOR {
	float white[];
	float blue[];
};

struct Color {
	float t[4];
	Color();
	Color(double r, double g, double b);
	Color(double r, double g, double b, double w);
	operator float *();
};


const double M_PI = 3.1415926535897932384;
string intToString(int a);
int stringToInt(string s);  //只支持正数
SYSTEMTIME getCurrentTime();
long long calTimeDis(SYSTEMTIME pret, SYSTEMTIME t);





#endif
