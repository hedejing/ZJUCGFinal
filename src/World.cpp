#include <cmath>
#include <Windows.h>
#include "World.h"
#include "utility.h"


unsigned int World::nextId = 0;
map<unsigned int, BasicElement *> World::objects = map<unsigned int, BasicElement *>();
deque<unsigned int> World::trash = deque<unsigned int>();

int World::windowHandle;
int World::windowHeight, World::windowWidth;
int World::windowPos[2];
int World::cursorPos[2];
int World::focusState;

Point World::eye = Point(0,0,0);
Point World::center = Point(1,0,0);
const Vec World::up = Vec(0,1,0);
double World::elevation[2] = {0.0, 0.0};
double World::moveSpeed = 0.4;
double World::rotateSpeed = 0.1;

unsigned int World::chosenID = -1;





unsigned int World::getNextId() {
	if (trash.empty()) {
		return ++nextId;
	}
	else {
		int id = trash.front();
		trash.pop_front();
		return id;
	}
}
unsigned int World::getNextId(BasicElement *be) {
	int id = getNextId();
	objects[id] = be;
	return id;
}
int World::erase(unsigned int id) {
	if (objects.empty()) return -1;
	objects.erase(id);
	trash.push_back(id);
	return 0;
}
void World::init(int *argc, char *argv[], int windowHeight, int windowWidth, int xPos, int yPos) {
	{  //GLUT INIT
		glutInit(argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
		glutInitWindowPosition(World::windowPos[0]=xPos, World::windowPos[1]=yPos);
		glutInitWindowSize(World::windowWidth=windowWidth, World::windowHeight=windowHeight);
		windowHandle = glutCreateWindow("Simple GLUT App");
		glutDisplayFunc(GLfunc::display);
		//glutReshapeFunc(reshape);
		glutKeyboardFunc(GLfunc::keyboard);
		glutMotionFunc(GLfunc::motion);
		glutPassiveMotionFunc(GLfunc::passiveMotion);
		glutMouseFunc(GLfunc::mouseClick);
		glutEntryFunc(GLfunc::entry);
		glutIdleFunc(GLfunc::idle);
		
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	}
	{  //OPENGL INIT
		glClearColor(0, 0, 0, 1);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_LIGHTING);
		GLfloat lightPos0[] = {5,5,5,1};
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, COLOR::white);
		glEnable(GL_LIGHT0);  //light0只是用做示例
	}
	{  //WORLD INIT
		SetCursorPos(windowWidth/2 + windowPos[0], windowHeight/2 + windowPos[1]);  //TODO  这里的cursorPos并不准确
		POINT pos;
		GetCursorPos(&pos);
		cursorPos[0] = pos.x;  cursorPos[1] = pos.y;
	}
	//...
}

void World::reCenter() {
	const double M_PI = 3.14159265358979323846;
	double theta = elevation[0]*M_PI/180, fi = elevation[1]*M_PI/180;
	center = eye + Vec(cos(fi)*cos(theta), sin(fi), cos(fi)*sin(theta));
}
void World::move(double dx, double dy, double dz) { move(Vec(dx, dy, dz)); }
void World::move(const Vec &ds) { eye+=ds; center+=ds; }
void World::move(int d, double step) {
	Point &eye = World::eye, &center = World::center;
	const Vec &up = World::up;
	double &moveSpeed = World::moveSpeed;

	if (d < 0  || d > 2) return;
	if (d==0) {
		Vec t = ((center-eye)*up).normalize();
		t = step * moveSpeed * t;
		eye += t; center += t;
	}
	else if (d == 1) {
		Vec t = (center-eye).normalize();
		t = step * moveSpeed *t;
		eye += t;  center += t;
	}
	else {
		eye += step * moveSpeed * up;
		center += step * moveSpeed * up;
	}
}
void World::rotate(double angle, double x, double y, double z) {}
void World::rotate(int d, double step) {
	if (d < 0  || d > 1) return;
	if (d == 0) elevation[0] += rotateSpeed * step;
	else elevation[1] += rotateSpeed * step;
	if (elevation[1] > 75) elevation[1] = 75;
	if (elevation[1] < -75) elevation[1] = -75;

	reCenter();
}
void World::drawAll() {
	//视角的设置
	for (auto o : objects) o.second->draw();
}




/*  GLUT FUNC  */
void GLfunc::idle() {
	glutPostRedisplay();
}

void GLfunc::display() {
	if (World::focusState == GLUT_ENTERED)
		SetCursorPos(World::windowWidth/2 + World::windowPos[0], World::windowHeight/2 + World::windowPos[1]);  //TODO  这里的cursorPos并不准确

glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)World::windowWidth/World::windowHeight, 0.1, 500);


glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	Point eye = World::eye, center = World::center;
	Vec up = World::up;
	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		up[0], up[1], up[2]);

	World::drawAll();

	glutSwapBuffers();
}

void GLfunc::keyboard(unsigned char key, int x, int y) {
	double &moveSpeed = World::moveSpeed;
	const double moveStep = 1;
	const double rotateStep = 8;

	switch (key) {
	case 27: exit(0);
	case 'a': case 'A':  //出现图形变扁的原因可能是我的这种移动一个像素转某个角度的逻辑有问题
		World::move(0, -moveStep);
		break;
	case 'd': case 'D':
		World::move(0,  moveStep);
		break;
	case 'w': case 'W':
		World::move(1,  moveStep);
		break;
	case 's': case 'S':
		World::move(1, -moveStep);
		break;
	case 'z': case 'Z':
		World::move(2, -moveStep);
		break;
	case 'c': case 'C':
		World::move(2, moveStep);
		break;
	case '-':  //速度的调整
		moveSpeed -= 0.005;
		break;
	case '+':
		moveSpeed += 0.005;
		break;

	case 'j': case 'J':  //出现图形变扁的原因可能是我的这种移动一个像素转某个角度的逻辑有问题
		World::rotate(0, -rotateStep);
		break;
	case 'l': case 'L':
		World::rotate(0,  rotateStep);
		break;
	case 'i': case 'I':
		World::rotate(1,  rotateStep);
		break;
	case 'k': case 'K':
		World::rotate(1, -rotateStep);
		break;
	//case 'f': case 'F':
	//	autoForward = !autoForward;
	//	break;

	//case ',': case '<':
	//	REP (i, 0, 2) lightColor[1][i] -= 0.1;
	//	break;
	//case '.': case '>':
	//	REP (i, 0, 2) lightColor[1][i] += 0.1;
	//	break;
	default:
		break;
	}
	glutPostRedisplay();
}


void gl_select(int x, int y);
void mousedw(int x, int y, int but);

//  TODO
void GLfunc::motion(int x, int y) {
}
void GLfunc::passiveMotion(int x, int y) {
	if (World::focusState == GLUT_ENTERED) {
		POINT pos; GetCursorPos(&pos);
		x = pos.x;  y = pos.y;
		World::elevation[0] += World::rotateSpeed * (x - World::cursorPos[0]);
		World::elevation[1] += -World::rotateSpeed * (y - World::cursorPos[1]);
		if (World::elevation[1] > 75) World::elevation[1] = 75;
		if (World::elevation[1] < -75) World::elevation[1] = -75;
	
		World::reCenter();
		glutPostRedisplay();
	}
}
void GLfunc::entry(int state) {
	World::focusState = state;
	if (state == GLUT_ENTERED)
		SetCursorPos(World::windowWidth/2 + World::windowPos[0], World::windowHeight/2 + World::windowPos[1]);
}
void GLfunc::mouseClick(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		mousedw(x, y, button);
	}
}











































void mousedw(int x, int y, int but) {
	//printf("Mouse button %d pressed at %d %d\n", but, x, y);
	gl_select(x, World::windowHeight - y); //Important: gl (0,0) ist bottom left but window coords (0,0) are top left so we have to change this!
}

void gl_select(int x, int y) {
	GLuint buff[4096] = { 0 };
	GLint hits, view[4];

	glSelectBuffer(4096, buff);

	//retrieve info about the viewport
	glGetIntegerv(GL_VIEWPORT, view);
	//switch select mode
	glRenderMode(GL_SELECT);


	glInitNames();
	glPushName(0);

	//modify the vieving volume, restricting selection area around the cursor
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//restrict the draw to an area around the cursor
	gluPickMatrix(x, y, 0.1, 0.1, view);
	gluPerspective(45, 1, 1, 500);

	//pretend to draw the objects onto the screen
	glMatrixMode(GL_MODELVIEW);
	glutSwapBuffers();
	World::drawAll();

	//restore the view
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//get number of objects drawed in that area and return to render mode
	hits = glRenderMode(GL_RENDER);

	if (hits >= 1) {
		int choose = buff[3];
		GLuint depth = buff[1];

		for (int i = 0; i < hits; i++) {
			if (buff[i * 4 + 1] < (GLuint)depth) {	//obtain selected object with minimum depth
				choose = buff[i * 4 + 3];
				depth = buff[i * 4 + 1];
			}
		}
		//changecolor[choose] = !changecolor[choose];
		World::chosenID = choose;
		//dbg(choosenID);
	}
	else World::chosenID = -1;
	cout<<hits<<" "<<World::chosenID<<endl;

	glMatrixMode(GL_MODELVIEW);
}