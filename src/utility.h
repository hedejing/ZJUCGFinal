#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <iostream>
using namespace std;


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


Vec operator-(const Point &a, const Point &b);  //此函数没有考虑w参数
Vec operator-(const Vec &a, const Vec &b);  //此函数没有考虑w参数
Vec operator+(const Vec &a, const Vec &b);
Vec operator*(const Vec &a, const Vec &b);
Vec operator*(Vec a, double t);
Vec operator*(double t, Vec a);
Vec operator/(Vec a, double t);

namespace COLOR {
	float white[];
};


#endif
