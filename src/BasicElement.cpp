#include "BasicElement.h"
#include "World.h"
#include "utility.h"

/*  BasicElement  */
BasicElement::BasicElement() {
	id = World::getNextId(this);
	//this->rotateMat = ::rotateMat(0, 0, 1, 0);
	scaleValue = Vec(1, 1, 1);
	//hdj : add an assignment for rotateQuat 1/8
	rotateQuat = Quat(0, 0, 0, 1);

	classType = 0;
}
BasicElement::~BasicElement() {
	Physics::rigidbody2elements.erase(this);
	World::erase(id);
}
unsigned int BasicElement::getId() {
	return id;
}
bool BasicElement::shouldBeRemoved() {
	return 0;
}

void BasicElement::draw() {
	float ambient[4] = { 0.5, 0.5, 0.5, 1 };
	float diffuse[4] = { 0.5, 0.5, 0.5, 1 };
	float specular[4] = { 0.2, 0.2, 0.2, 1 };
	float shinni = 40;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shinni);
	glPushMatrix();
		glLoadName(id);
		glTranslated(centroid[0], centroid[1], centroid[2]);
		glMultMatrixd((GLMat)rotateQuat);  //glMultMatrixd(rotateMat);
		glScaled(scaleValue[0], scaleValue[1], scaleValue[2]);
		drawNaive();  //父类函数调用子类的函数??可以！
		glLoadName(0);
	glPopMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shinni);
}
void BasicElement::drawNaive() {}


void BasicElement::rotate(double angle, double x, double y, double z) {  //TODO  貌似会出现万向节死锁问题，还是尽量用下面那个函数吧
	//rotateMat = ::rotateMat(angle, x, y, z) * rotateMat;
	rotateQuat = ::rotateQuat(angle, x, y, z) * rotateQuat;
}
void BasicElement::rotateTo(double angle, double x, double y, double z) {
	//rotateMat = ::rotateMat(angle, x, y, z);
	rotateQuat = ::rotateQuat(angle, x, y, z);
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
static void
drawBox(GLfloat size, GLenum type)
{
	static GLfloat n[6][3] =
	{
		{ -1.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, -1.0, 0.0 },
		{ 0.0, 0.0, 1.0 },
		{ 0.0, 0.0, -1.0 }
	};
	static GLint faces[6][4] =
	{
		{ 0, 1, 2, 3 },
		{ 3, 2, 6, 7 },
		{ 7, 6, 5, 4 },
		{ 4, 5, 1, 0 },
		{ 5, 6, 2, 1 },
		{ 7, 4, 0, 3 }
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

	for (i = 5; i >= 0; i--) {
		glBegin(type);
		glNormal3fv(&n[i][0]);
		glTexCoord2f(0, 0);
		glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2f(1, 0);
		glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2f(1, 1);
		glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2f(0, 1);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}

void APIENTRY
MySolidCube(GLdouble size)
{
	drawBox(size, GL_QUADS);
}
Cube::Cube(Point p) {
	centroid = p;
}
void Cube::drawNaive() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	MySolidCube(2);
	glDisable(GL_TEXTURE_2D);
}
Cube::Cube(Point p, GLuint t)
{
	tex = t;
	centroid = p;
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

AviBoard::AviBoard(Point p, double height, double weight, string videoname)
{
	centroid = p;
	FILE *fp = fopen(videoname.c_str(), "rb");
	if (fp == NULL){		//打开失败
		printf("Cannot open this file.\n");
		return ;	//需要统一？
	}
	fread(&pixel_w, sizeof(int), 1, fp);//获取长款
	fread(&pixel_h, sizeof(int), 1, fp);
	fread(&framenum, sizeof(int), 1, fp);//获取总帧数
	buffer = new unsigned char[pixel_w*pixel_h *framenum * 3];
	fread(buffer,  pixel_w*pixel_h *framenum * 3, 1,fp);//获取数据
	tmpbuff = buffer;
	fclose(fp);
	nowtime = 0;
	timebase = 0;
	count = 0;
	this->height = height;
	this->weight = weight;
}

AviBoard::~AviBoard()
{
	delete[] buffer;
}

void AviBoard::drawNaive() {
	nowtime = glutGet(GLUT_ELAPSED_TIME);//获取时间
	if (nowtime - timebase > 40) {
		if (count == framenum-1)
		{
			tmpbuff = buffer;
			count = 0;
		}
		else
		{
			tmpbuff = tmpbuff + pixel_w*pixel_h * 3;
			count++;
		}
		timebase = nowtime;
	}
	glPushMatrix();
		glRasterPos3f(-1.0f, 1.0f, 0);
		glPixelZoom((float)weight / (float)pixel_w, -(float)height / (float)pixel_h);
		glDrawPixels(pixel_w, pixel_h, GL_RGB, GL_UNSIGNED_BYTE, tmpbuff);
	glPopMatrix();
}



LifeBar::LifeBar() {}
LifeBar::LifeBar(double _max_life) :max_life(_max_life) {}
LifeBar::LifeBar(double _max_life, double len, double w) : barlen(len), barw(w), max_life(_max_life) {}
LifeBar::~LifeBar() {}
void LifeBar::DrawBar(Point pos, double life)
{
	glEnable(GL_COLOR_MATERIAL);
	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(-barlen/2, 0, -barw/2);
	glVertex3f(-barlen / 2, 0, barw / 2);
	glVertex3f(barlen / 2, 0, barw / 2);
	glVertex3f(barlen / 2, 0, -barw / 2);
	glVertex3f(-barlen / 2, 0, -barw / 2);
	glEnd();
	if (life < max_life / 3) glColor3f(1, 0, 0);
	else glColor3f(0, 1, 0);
	glBegin(GL_QUADS);
	glVertex3f(-barlen / 2, 0, -barw / 2);
	glVertex3f(-barlen / 2, 0, barw/2);
	glVertex3f(-barlen / 2 + barlen * (life / max_life), 0, barw / 2);
	glVertex3f(-barlen / 2 + barlen*(life / max_life), 0, -barw / 2);		
	glEnd();
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
	float am[4] = { 0.3, 0.3, 0.3, 1 };
	float nor[4] = { 0.5, 0.5, 0.5, 1 };
	float zero[4] = { 0.3, 0.3, 0.3, 1 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, am);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, nor);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0);
}


objectmodel::objectmodel(Point p, string name)
{
	centroid = p;
	mymo = new  Model();
	mymo->Readobj(name.c_str());
	scale(0.1);
}

void objectmodel::drawNaive()
{
	mymo->draw();
}
void objectmodel::addToPhysicsWorld(double coliisionscale , double weight)
{
	btRigidBody* temp = Physics::CreateRigidBodyForModelWithShape(this, mymo->getCollisionRect() / coliisionscale, weight);
	Physics::AddRigidBodyAndElement(temp, this);
}
objectmodel::~objectmodel(void) {
}

Monster::Monster(Point p, string name):objectmodel(p, name) {
	classType = 2;
	blood = 100;
	fullblood = 100;
	lifeBar = LifeBar(fullblood);
}
void Monster::subblood(){
	blood = blood - 25;
}
bool Monster::shouldBeRemoved() {
	if (blood <= 0)
		return true;
	else
		return false;
}
void Monster::draw() {
	glPushMatrix();
		glPushMatrix();
			glTranslated(centroid[0], centroid[1], centroid[2]);
			glTranslated(0, mymo->getCollisionRect()[1]*scaleValue[1]+1, 0);
			glRotatef(90-World::elevation[0], 0, 1, 0);
			glRotatef(90, 1, 0, 0);
			lifeBar.DrawBar(Point(0, 0, 0), blood);
		glPopMatrix();
		glLoadName(id);
		glTranslated(centroid[0], centroid[1], centroid[2]);
		glPushMatrix();
			glTranslated(0, mymo->getCollisionRect()[1]*scaleValue[1]+1, 0);
			glRotatef(90-World::elevation[0], 0, 1, 0);
			glRotatef(90, 1, 0, 0);
			lifeBar.DrawBar(Point(0, 0, 0), blood);
		glPopMatrix();
		glMultMatrixd((GLMat)rotateQuat);  //glMultMatrixd(rotateMat);
		glScaled(scaleValue[0], scaleValue[1], scaleValue[2]);
		drawNaive();  //父类函数调用子类的函数??可以！
		glLoadName(0);
	glPopMatrix();
}
void Monster::drawNaive() {
	mymo->draw();
}


CameraModel::CameraModel(Point p) {
	centroid = p;
}
CameraModel::CameraModel() {
}
void CameraModel::drawNaive() {
}


Bullet::Bullet(Point p, double radius, int slices, int stacks, int tex) {
	classType = 1;
	centroid = p;
	this->radius = radius;
	this->slices = slices;
	this->stacks = stacks;
	this->tex = tex;
}
void Bullet::drawNaive() {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Color(0.2, 0.2, 0.2));
	glutSolidSphere(radius, slices, stacks);
}
bool Bullet::shouldBeRemoved() {
	return !World::isInside(centroid);
}

/****************explosion*************************************/		
void Explosion::drawNaive()
{
	GLfloat colors[12][3] = {
		{ 1.0f, 0.5f, 0.5f }, { 1.0f, 0.75f, 0.5f }, { 1.0f, 1.0f, 0.5f }, { 0.75f, 1.0f, 0.5f },
		{ 0.5f, 1.0f, 0.5f }, { 0.5f, 1.0f, 0.75f }, { 0.5f, 1.0f, 1.0f }, { 0.5f, 0.75f, 1.0f },
		{ 0.5f, 0.5f, 1.0f }, { 0.75f, 0.5f, 1.0f }, { 1.0f, 0.5f, 1.0f }, { 1.0f, 0.5f, 0.75f }
	};
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texture[0]);
	float red[4]={1,0,0,1};
	glPushMatrix();
	for (loop = 0; loop<MAX_PARTICLES; loop++)					// Loop Through All The Particles
	{
		if (particle[loop].active)							// If The Particle Is Active
		{
			float x = particle[loop].x;						// Grab Our Particle X Position
			float y = particle[loop].y;						// Grab Our Particle Y Position
			float z = particle[loop].z + zoom;					// Particle Z Pos + Zoom
			
			// Draw The Particle Using Our RGB Values, Fade The Particle Based On It's Life
			glColor4f(1, 1, 1, 1);
			//cout << particle[loop].life << endl;
			glBegin(GL_TRIANGLE_STRIP);						// Build Quad From A Triangle Strip
			glVertex3f(x + 0.05f, y + 0.05f, z); // Top Right
			glVertex3f(x - 0.05f, y + 0.05f, z); // Top Left
			glVertex3f(x + 0.05f, y - 0.05f, z); // Bottom Right
			glVertex3f(x - 0.05f, y - 0.05f, z); // Bottom Left
			glEnd();										// Done Building Triangle Strip

			particle[loop].x += particle[loop].xi / (slowdown * 1000);// Move On The X Axis By X Speed
			particle[loop].y += particle[loop].yi / (slowdown * 1000);// Move On The Y Axis By Y Speed
			particle[loop].z += particle[loop].zi / (slowdown * 1000);// Move On The Z Axis By Z Speed

			particle[loop].xi += particle[loop].xg;			// Take Pull On X Axis Into Account
			particle[loop].yi += particle[loop].yg;			// Take Pull On Y Axis Into Account
			particle[loop].zi += particle[loop].zg;			// Take Pull On Z Axis Into Account
			particle[loop].life -= particle[loop].fade;		// Reduce Particles Life By 'Fade'

			if (particle[loop].life<0.0f)					// If Particle Is Burned Out
			{
				particle[loop].life = 1.0f;					// Give It New Life
				particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;	// Random Fade Value
				particle[loop].x = 0.0f;						// Center On X Axis
				particle[loop].y = 0.0f;						// Center On Y Axis
				particle[loop].z = 0.0f;						// Center On Z Axis
				particle[loop].xi = xspeed + float((rand() % 60) - 32.0f);	// X Axis Speed And Direction
				particle[loop].yi = yspeed + float((rand() % 60) - 30.0f);	// Y Axis Speed And Direction
				particle[loop].zi = float((rand() % 60) - 30.0f);	// Z Axis Speed And Direction
				particle[loop].r = colors[col][0];			// Select Red From Color Table
				particle[loop].g = colors[col][1];			// Select Green From Color Table
				particle[loop].b = colors[col][2];			// Select Blue From Color Table
				//cout <<' '<< particle[loop].x<<' ' << particle[loop].y<<' ' << particle[loop].z << endl;
			}
		}
	}
	glPopMatrix();
	glEnable(GL_LIGHTING);
	//glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
}
