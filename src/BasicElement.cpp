#include "BasicElement.h"
#include "World.h"
#include "utility.h"

/*  BasicElement  */
BasicElement::BasicElement() {
	id = World::getNextId(this);
	scaleValue = Vec(1, 1, 1);
}
BasicElement::~BasicElement() {
	World::erase(id);
}
unsigned int BasicElement::getId() {
	return id;
}

void BasicElement::rotate(double angle, double x, double y, double z) {
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
void BasicElement::draw() {
	glPushMatrix();
		glLoadName(id);
		glTranslated(centroid[0], centroid[1], centroid[2]);
		glScaled(scaleValue[0], scaleValue[1], scaleValue[2]);
		drawNaive();  //父类函数调用子类的函数??可以！
		glLoadName(0);
	glPopMatrix();
}
void BasicElement::drawNaive() {}




/*  Cube  */
Cube::Cube(Point p) {
	centroid = p;
}
void Cube::drawNaive() {
	glutSolidCube(1);
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
