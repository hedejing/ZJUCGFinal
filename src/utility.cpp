#include "utility.h"


/*  Point and Vec  */
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
double Vec::sqr() {
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
Vec operator+(const Vec &a, const Vec &b) { return Vec(a[0]+b[0], a[1]+b[1], a[2]+b[2]); }
Point &operator+=(Point &a, const Vec &b) { return a = a+b; }
Vec operator*(const Vec &a, const Vec &b) { return Vec(a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]); }
double operator%(const Vec &a, const Vec &b) { return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]; }
Vec operator*(Vec a, double t) { return Vec(a[0]*t, a[1]*t, a[2]*t); }
Vec operator*(double t, Vec a) { return a*t; }
Vec operator/(Vec a, double t) { return Vec(a[0]/t, a[1]/t, a[2]/t); }

ostream &operator<<(ostream &out, Point a) { return out<<a[0]<<" "<<a[1]<<" "<<a[2]; }
ostream &operator<<(ostream &out, Vec a) { return out<<a[0]<<" "<<a[1]<<" "<<a[2]; }




/*  GLMat  */
GLMat::GLMat() { GLMat(1); }
GLMat::GLMat(double d) { memset(a, 0, sizeof(a)); for (int i=0; i<4; i++) a[i][i]=d; }
GLMat::GLMat(double t[4][4]) {
	for (int i=0; i<4; i++) for (int j=0; j<4; j++)
		a[i][j] = t[i][j];
}
double *GLMat::operator[](int d) { return a[d]; }
Vec GLMat::operator()(int d) { return Vec(a[0][d],a[1][d],a[2][d]); }
GLMat::operator double *() { return &a[0][0]; }
GLMat GLMat::transpose() {
	GLMat ret, t=*this;
	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++) ret[i][j] = t[j][i];
	return ret;
}
GLMat GLMat::operator*(const GLMat &t) const {  //可能需要改一下(这个改了后面的也要改)
	GLMat a=*this, b=t;
	GLMat c = GLMat(0.0);
	a=a.transpose(); b=b.transpose();
	for (int i=0; i<4; i++) for (int j=0; j<4; j++)
		for (int k=0; k<4; k++) c[i][j] += a[i][k]*b[k][j];
	const double EPS = 1e-14;
	for (int i=0; i<4; i++) for (int j=0; j<4; j++) {
		if (fabs(int(c[i][j])-c[i][j])<EPS) c[i][j]=int(c[i][j]);
		if (fabs(int(c[i][j]+1)-c[i][j])<EPS) c[i][j]=int(c[i][j]);
	}
	return c.transpose();
}
GLMat &GLMat::operator*=(GLMat t) { return *this=*this*t; }

ostream &operator<<(ostream &out, GLMat a) {
	for (int i=0; i<4; i++) for (int j=0; j<4; j++)
		out<<a[i][j]<<char(j==3?10:32);
	return out;
}

GLMat rotateMat(double angle, double x, double y, double z) {
	angle = angle*M_PI/180;
	Vec t = Vec(x, y, z).normalize();  //旋转向量原来要单位化...
	x = t[0]; y = t[1]; z = t[2];

	double c = cos(angle), s = sin(angle);
	double mat[4][4] = {
		{x*x*(1-c)+c, x*y*(1-c)+z*s, x*z*(1-c)-y*s, 0},
		{x*y*(1-c)-z*s, y*y*(1-c)+c, y*z*(1-c)+x*s, 0},
		{x*z*(1-c)+y*s, y*z*(1-c)-x*s, z*z*(1-c)+c, 0},
		{0, 0, 0, 1}
	};
	return GLMat(mat);
}




/*  Quat(四元数)  */
Quat::Quat():w(0),x(0),y(0),z(0) {}
Quat::Quat(GLdouble w,GLdouble x,GLdouble y,GLdouble z):w(w),x(x),y(y),z(z) {}
Quat::Quat(GLdouble w, Vec v):w(w),x(v[0]),y(v[1]),z(v[2]) {}
Vec Quat::vec() { return Vec(x,y,z); }
GLdouble Quat::sqr() { return w*w+x*x+y*y+z*z; }
GLdouble Quat::norm() { return sqrt(sqr()); }
Quat Quat::normalize() { return *this/(*this).norm(); }
Quat::operator GLMat() {
	double mat[4][4] = {
		{1-2*(y*y+z*z), 2*(x*y-z*w), 2*(x*z+y*w), 0},
		{2*(x*y+z*w), 1-2*(x*x+z*z), 2*(y*z-x*w), 0},
		{2*(x*z-y*w), 2*(y*z+x*w), 1-2*(x*x+y*y), 0},
		{0, 0, 0, 1}
	};
	return GLMat(mat).transpose();
}

Quat operator+(Quat a, Quat b) {
	return Quat(a.w+b.w, a.vec()+b.vec());
}
Quat operator*(Quat a, Quat b) {
	return Quat(
		a.w*b.w - a.vec()%b.vec(),
		a.w*b.vec() + b.w*a.vec() + a.vec()*b.vec()
	);
}
Quat operator/(Quat a,GLdouble d) {
	return Quat(a.w/d, a.vec()/d);
}
Quat Quat::operator~() {  //逆
	return Quat(w, -Vec(x,y,z))/sqr();
}
Quat rotateQuat(double angle, double x, double y, double z) {
	angle = angle * M_PI / 180;
	Vec n = Vec(x, y, z).normalize();
	return Quat(cos(angle/2), n*sin(angle/2)).normalize();
}





/*  COLOR  */
float COLOR::white[] = {1, 1, 1, 1};
float COLOR::blue[] = {0, 0, 1, 1};




/*  EXTRA FUNCTIONS  */
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