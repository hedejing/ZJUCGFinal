#include "utility.h"


Point::Point() {
	for (int i=0; i<4; i++)
		t[i] = 0;
}
Point::Point(double a, double b, double c) {
	t[0] = a;
	t[1] = b;
	t[2] = c;
	t[3] = 1;
}
Point::Point(double a, double b, double c, double w) {
	t[0] = a;
	t[1] = b;
	t[2] = c;
	t[3] = w;
}
double Point::getX() { return t[0]; }
double Point::getY() { return t[1]; }
double Point::getZ() { return t[2]; }
double Point::getW() { return t[3]; }
double &Point::operator[](int d) { return t[d]; }
const double &Point::operator[](int d) const { return t[d]; }

Vec::Vec() {
	for (int i=0; i<4; i++)
		t[i] = 0;
}
Vec::Vec(double a, double b, double c) {
	t[0] = a;
	t[1] = b;
	t[2] = c;
	t[3] = 1;
}
Vec::Vec(double a, double b, double c, double w) {
	t[0] = a;
	t[1] = b;
	t[2] = c;
	t[3] = w;
}
double Vec::getX() { return t[0]; }
double Vec::getY() { return t[1]; }
double Vec::getZ() { return t[2]; }
double Vec::getW() { return t[3]; }
double &Vec::operator[](int d) { return t[d]; }
const double &Vec::operator[](int d) const { return t[d]; }
double Vec::sqr() {  //没考虑w
	double ret = 0;
	for (int i=0; i<3; i++)
		ret += t[i] * t[i];
	return ret;
}
double Vec::abs() { return sqrt(sqr()); }
Vec Vec::normalize() { return *this/this->abs(); }
Vec Vec::operator-() { return Vec(-t[0], -t[1], -t[2]); }

//这些函数没有考虑w参数
Vec operator-(const Point &a, const Point &b) {	return Vec(a[0]-b[0], a[1]-b[1], a[2]-b[2]); }
Point operator+(const Point &a, const Vec &b) { return Point(a[0]+b[0], a[1]+b[1], a[2]+b[2]); }
Point &operator+=(Point &a, const Vec &b) { return a = a+b; }
Vec operator*(const Vec &a, const Vec &b) { return Vec(a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]); }
Vec operator*(Vec a, double t) { return Vec(a[0]*t, a[1]*t, a[2]*t); }
Vec operator*(double t, Vec a) { return a*t; }
Vec operator/(Vec a, GLdouble t) { return Vec(a[0]/t, a[1]/t, a[2]/t); }

ostream &operator<<(ostream &out, Point a) { return out<<a[0]<<" "<<a[1]<<" "<<a[2]; }
ostream &operator<<(ostream &out, Vec a) { return out<<a[0]<<" "<<a[1]<<" "<<a[2]; }



/*  COLOR  */
float COLOR::white[] = {1, 1, 1, 1};



string intToString(int a) {
	string s = "";
	if (a == 0) return s = "0";
	while (a) s += a%10+48, a /= 10;
	reverse(s.begin(), s.end());
	return s;
}
int stringToInt(string s) {  //只支持正数
	int ret = 0;
	for (int i=0; i<s.size(); i++) {
		if (s[i]<'0'||s[i]>'9') return -1;
		if (ret!=0 && 0x7fffffff/ret<(s[i]-48)) return -1;
		ret = ret * 10 + s[i] - 48;
	}
	return ret;
}