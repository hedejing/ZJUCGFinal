#include "BasicElement.h"
#include "World.h"
#include "utility.h"

/*  BasicElement  */
BasicElement::BasicElement() {
	id = World::getNextId(this);
	this->rotateMat = ::rotateMat(0, 0, 1, 0);
	scaleValue = Vec(1, 1, 1);
}
BasicElement::~BasicElement() {
	World::erase(id);
}
unsigned int BasicElement::getId() {
	return id;
}

void BasicElement::draw() {
	glPushMatrix();
		glLoadName(id);
		glTranslated(centroid[0], centroid[1], centroid[2]);
		glMultMatrixd(rotateMat);
		glScaled(scaleValue[0], scaleValue[1], scaleValue[2]);
		drawNaive();  //父类函数调用子类的函数??可以！
		glLoadName(0);
	glPopMatrix();
}
void BasicElement::drawNaive() {}


void BasicElement::rotate(double angle, double x, double y, double z) {  //TODO  貌似会出现万向节死锁问题，还是尽量用下面那个函数吧
	rotateMat = rotateMat * ::rotateMat(angle, x, y, z);
}
void BasicElement::rotateTo(double angle, double x, double y, double z) {
	rotateMat = ::rotateMat(angle, x, y, z);
}
void BasicElement::move(double dx, double dy, double dz) {
	centroid += Vec(dx, dy, dz);
}
void BasicElement::moveTo(Point p) {
	centroid = p;
}
void BasicElement::scale(double tx, double ty, double tz) {
	scale(Vec(tx, ty, tz));
}
void BasicElement::scale(Vec t) {
	if (t[0] <= 0 || t[1] <= 0 || t[2] <= 0) return;
	for (int i=0; i<3; i++) scaleValue[i] *= t[i];
}
void BasicElement::scale(double t) {
	scale(Vec(t, t, t));
}



/*  Cube  */
Cube::Cube(Point p) {
	centroid = p;
}
void Cube::drawNaive() {
	glutSolidCube(1);
}

/*  Sphere  */
Sphere::Sphere(Point p, double radius, int slices, int stacks) {
	centroid = p;
	this->radius = radius;
	this->slices = slices;
	this->stacks = stacks;
}
void Sphere::drawNaive() {
	glutSolidSphere(radius, slices, stacks);
}

/*  Cone  */
Cone::Cone(Point p, double radius, double height, int slices, int stacks) {
	centroid = p;
	this->radius = radius;
	this->height = height;
	this->slices = slices;
	this->stacks = stacks;
}
void Cone::drawNaive() {
	glutSolidCone(radius, height, slices, stacks);
}

/*  Circle  */
Circle::Circle(Point p, double radius, int slices) {
	centroid = p;
	this->radius = radius;
	this->slices = slices;
}
void Circle::circle() {  //TODO  circle函数的法向计算有问题
	glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0, 1, 0), glVertex3f(0.0f, 0.0f, 0.0f);
		double step = 2 * M_PI / slices;
		for (double i=2*M_PI+0.1; i>=0; i-=step)
			glNormal3f(0, 1, 0), glVertex3f(sin(i), 0.0f, cos(i));
	glEnd();
}
void Circle::drawNaive() {
	glPushMatrix();
		glScaled(radius*2, radius*2, radius*2);
		circle();
	glPopMatrix();
}

/*  Cylinder  */
Cylinder::Cylinder(Point p, double radius, double height, int slices, int stacks):Circle(p, radius, slices) {
	this->height = height;
	this->stacks = stacks;
}
void Cylinder::drawNaive() {
	glPushMatrix();
		glScaled(1, height, 1);
		glBegin(GL_QUAD_STRIP);
			double step = 2 * M_PI / stacks;
			for (double i=2*M_PI+0.1; i>=0; i-=step) {
				glNormal3f(sin(i),  0, cos(i)), glVertex3f(sin(i),  0.5f, cos(i));
				glNormal3f(sin(i),  0, cos(i)), glVertex3f(sin(i), -0.5f, cos(i));
			}
		glEnd();
		glTranslated(0, 0.5, 0); glPushMatrix(); glRotatef(180, 1, 0, 0); circle(); glPopMatrix();
		glTranslated(0, -1, 0); circle();
	glPopMatrix();
}

/*  Rect: 带纹理的矩形  */
Rect::Rect(Point p, unsigned int texture) {
	centroid = p;
	tex = texture;
}
Rect::Rect(unsigned int texture) {
	centroid = Point();
	tex = texture;
}
void Rect::drawNaive() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0,0.5,0.5);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0,0.5,-0.5);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0,-0.5,-0.5);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0,-0.5,0.5);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}
