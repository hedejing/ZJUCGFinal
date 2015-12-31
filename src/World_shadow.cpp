#include <cmath>
#include <Windows.h>
#include <io.h>
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

Point World::eye = Point(0, 0, 0);
Point World::center = Point(1, 0, 0);
const Vec World::up = Vec(0, 1, 0);
double World::elevation[2] = { 0.0, 0.0 };
double World::moveSpeed = 0.4;
double World::rotateSpeed = 0.1;

unsigned int World::chosenID = -1;

World::ShadowStuff World::shadowStuff;
vector<LightSource> World::lights;

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
		glutInitWindowPosition(World::windowPos[0] = xPos, World::windowPos[1] = yPos);
		glutInitWindowSize(World::windowWidth = windowWidth, World::windowHeight = windowHeight);
		windowHandle = glutCreateWindow("Simple GLUT App");
		glutDisplayFunc(display);
		//glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);
		glutSpecialFunc(special);
		glutMotionFunc(motion);
		glutPassiveMotionFunc(passiveMotion);
		glutMouseFunc(mouseClick);
		glutEntryFunc(entry);
		glutIdleFunc(idle);

		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	}
	{  //GLEW INIT
		glewExperimental = TRUE;
		if (GLenum err = glewInit())
			fprintf(stderr, "Error is%s", glewGetErrorString(err));
	}
	{  //OPENGL INIT
		glClearColor(0, 0, 0, 1);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}
	{  //WORLD INIT
		setCursorToCenter();
		POINT pos; GetCursorPos(&pos);
		cursorPos[0] = pos.x;  cursorPos[1] = pos.y;

	}
	{	//SHADOW INIT
		shadow_init();
		glEnable(GL_BLEND);

	}
	//...
}
void World::setCursorToCenter() {
	SetCursorPos(windowWidth / 2 + windowPos[0], windowHeight / 2 + windowPos[1]);  //TODO  这里的cursorPos并不准确
}

void World::reCenter() {
	const double M_PI = 3.14159265358979323846;
	double theta = elevation[0] * M_PI / 180, fi = elevation[1] * M_PI / 180;
	center = eye + Vec(cos(fi)*cos(theta), sin(fi), cos(fi)*sin(theta));
}
void World::move(double dx, double dy, double dz) { move(Vec(dx, dy, dz)); }
void World::move(const Vec &ds) { eye += ds; center += ds; }
void World::move(int d, double step) {
	if (d < 0 || d > 2) return;
	if (d == 0) {
		Vec t = ((center - eye)*up).normalize();
		t = step * moveSpeed * t;
		eye += t; center += t;
	}
	else if (d == 1) {
		Vec t = (center - eye).normalize();
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
	if (d < 0 || d > 1) return;
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
void World::idle() {
	glutPostRedisplay();
}

void World::display() {
	if (focusState == GLUT_ENTERED)
		setCursorToCenter();

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(45, (double)windowWidth / windowHeight, 0.1, 500);

	//glMatrixMode(GL_MODELVIEW);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();
	//gluLookAt(eye[0], eye[1], eye[2],
	//	center[0], center[1], center[2],
	//	up[0], up[1], up[2]);


	displayWithShadow();
	//drawAll();
	glutSwapBuffers();
}


/*
glActiveTexture(GL_TEXTURE0) 普通贴图
glActiveTexture(GL_TEXTURE1) 阴影纹理
系统支持的最大纹理宽度和长度，查询 glGetIntegerv(GL_MAX_TEXTURE_SIZE, GLint*)
在我的电脑上 最大纹理尺寸为 16382
有的可能是8192
*/


void xoy_foor(float len, float tex_repeat, int subdivision)
{
	GLfloat color[] = { .8f, 0.8f, 0.8f, 1 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(len, len, 1);
	glNormal3f(0, 0, 1);
	float u = 1.0f / subdivision;
	float tu = tex_repeat / subdivision;
	for (int i = 0; i < subdivision; ++i)
		for (int j = 0; j < subdivision; ++j) {
			float ox = u*i, oy = u*j;
			float tox = tu*i, toy = tu*j;
			glBegin(GL_POLYGON);
			glTexCoord2f(tox, toy);		glVertex3f(ox, oy, 0);
			glTexCoord2f(tox + tu, toy);		glVertex3f(ox + u, oy, 0);
			glTexCoord2f(tox + tu, toy + tu);	glVertex3f(ox + u, oy + u, 0);
			glTexCoord2f(tox, toy + tu);	glVertex3f(ox, oy + u, 0);
			glEnd();
		}
	glPopMatrix();
}

void World::draw_world()
{
	//glStaff::xyz_frame(2,2,2,false);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(90, -1, 0, 0);
	glTranslatef(-10, -10, 0);
	xoy_foor(20, 3, 100);
	glPopMatrix();
	glPushMatrix();

	glTranslatef(0, 0, 0);
	glutSolidCube(2);

	glTranslatef(2, 0, 2);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(1, 2, 50, 50);
	glRotatef(90, 1, 0, 0);

	glTranslatef(0, 0, -4);
	glutSolidSphere(1, 50, 50);

	glTranslatef(-4, 1, 0);
	glRotatef(45, 1, 0, 0);
	glutSolidTorus(0.5f, 1, 50, 50);
	glRotatef(-45, 1, 0, 0);
	glPopMatrix();
	drawAll();
	glPopMatrix();
}

void World::draw_model()
{
	/*glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(0, 0, 0);
	glutSolidCube(2);

	glTranslatef(2, 0, 2);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(1, 2, 50, 50);
	glRotatef(90, 1, 0, 0);

	glTranslatef(0, 0, -4);
	glutSolidSphere(1, 50, 50);

	glTranslatef(-4, 1, 0);
	glRotatef(45, 1, 0, 0);
	glutSolidTorus(0.5f, 1, 50, 50);
	glRotatef(-45, 1, 0, 0);

	glPopMatrix();
	drawAll();*/
}
GLuint tex_walls;

#include "texture.h"
void World::shadow_init()
{
	glActiveTexture(GL_TEXTURE0); // 普通贴图
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	tex_walls = loadTexture("checker.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1); // 阴影纹理
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// 自动生成纹理
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);
#ifdef SINGEL_LIGHT
	glGenTextures(1, &shadowStuff.tex_shadow);
	glBindTexture(GL_TEXTURE_2D, shadowStuff.tex_shadow);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat c[4] = { 1,1,1, 1 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowStuff.shadow_w, shadowStuff.shadow_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
#endif
	// frame buffer object
	glGenFramebuffers(1, &shadowStuff.frame_buffer_s);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowStuff.frame_buffer_s);
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowStuff.tex_shadow, 0);
}

void World::AddLight(const LightSource & light)
{
	lights.push_back(light);
	glActiveTexture(GL_TEXTURE1); // 阴影纹理
	GLuint newShadowTexID;
	glGenTextures(1, &newShadowTexID);
	glBindTexture(GL_TEXTURE_2D, newShadowTexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat c[4] = { 1,1,1, 1 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowStuff.shadow_w, shadowStuff.shadow_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glDisable(GL_TEXTURE1);
	shadowStuff.tex_shadow.push_back(newShadowTexID);
}

void World::EnableLight(unsigned id)
{
	if (id >= lights.size())
	{
		fprintf(stderr, "Light id doesn't exist!!!\n");
		system("pause"); // pending...
	}
	else
		lights[id].enable = true;
}

void World::DisableLight(unsigned id)
{
	if (id >= lights.size())
	{
		fprintf(stderr, "Light id doesn't exist!!!\n");
		system("pause"); // pending...
	}
	else
		lights[id].enable = false;
}

void World::displayWithShadow()
{
	if (focusState == GLUT_ENTERED)
		setCursorToCenter();

	//---------------------------------------第1次绘制，生成深度纹理--------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowStuff.frame_buffer_s);
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowStuff.tex_shadow, 0);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, shadowStuff.shadow_w, shadowStuff.shadow_h);
	GLenum re = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	glClear(GL_DEPTH_BUFFER_BIT);

	// 不需要光照和纹理
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if (li) glDisable(GL_LIGHTING);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);

	// 将摄像机放置在光源位置，投影矩阵和视图矩阵
	GLMat shadowMatP = perspectiveMat(90, (double)windowWidth / windowHeight, 1.0, 1.0e10f);
	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);
#ifdef SINGEL_LIGHT
	GLMat shadowMatV = lookAtMat(Vec(lights[0].position[0], lights[0].position[1], lights[0].position[2]), Vec(0, 0, 0), up);
	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// hedjin : value of near must be 1.0 !!! 
	gluPerspective(90, (double)windowWidth / windowHeight, 1.0f, 1.0e10f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Point localeye = lights[0].position;
	gluLookAt(localeye[0], localeye[1], localeye[2], 0, 0, 0, up[0], up[1], up[2]);
	draw_world();
	draw_model();
	glDisable(GL_CULL_FACE); glCullFace(GL_BACK);
#else
	vector<GLMat> shadowMatV;
	shadowMatV.resize(lights.size());
	for (int i = 0; i < lights.size(); i++)
	{
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowStuff.tex_shadow[i], 0);
		glClear(GL_DEPTH_BUFFER);
		shadowMatV[i] = lookAtMat(Vec(lights[i].position[0], lights[i].position[1], lights[i].position[2]), Vec(0, 0, 0), up);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90, (double)windowWidth / windowHeight, 1.0f, 1.0e10f);// hedjin : value of near must be 1.0 !!! 
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Point localeye = lights[0].position;
		gluLookAt(localeye[0], localeye[1], localeye[2], 0, 0, 0, up[0], up[1], up[2]);
		draw_world();
		draw_model();
	}
#endif
	glDisable(GL_CULL_FACE); glCullFace(GL_BACK);

	//glActiveTexture(GL_TEXTURE1);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluLookAt(eye[0], eye[1], eye[2],
	//	center[0], center[1], center[2],
	//	up[0], up[1], up[2]);
	//double bias[4][4] = {
	//	{ 0.5, 0.0, 0.0, 0.0 },
	//	{ 0.0, 0.5, 0.0, 0.0 },
	//	{ 0.0, 0.0, 0.5, 0.0 },
	//	{ 0.5, 0.5, 0.49, 1.0 }
	//};
	//GLMat mat_view = lookAtMat(Vec(eye[0], eye[1], eye[2]), Vec(center[0], center[1], center[2]), up);
	//GLMat biasMat(bias);
	//GLMat mat = biasMat * shadowMatP * shadowMatV;//* mat_view.affineInverse();
	//mat = mat.transpose();
	//glTexGendv(GL_S, GL_EYE_PLANE, mat[0]);
	//glTexGendv(GL_T, GL_EYE_PLANE, mat[1]);
	//glTexGendv(GL_R, GL_EYE_PLANE, mat[2]);
	//glTexGendv(GL_Q, GL_EYE_PLANE, mat[3]);
	//glEnable(GL_TEXTURE_2D);
	//if (save_stencil) {
	//	save_stencil = false;
	//	glActiveTexture(GL_TEXTURE1);
	//	GLfloat* data = new GLfloat[shadowStuff.shadow_w, shadowStuff.shadow_h];
	//	for (int i = 0; i<lights.size(); ++i) {
	//		glBindTexture(GL_TEXTURE_2D, shadowStuff.tex_shadow);
	//		glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);//获取纹理数据
	//		char ss[50]; sprintf(ss, "d%d.png", i);
	//		//il_saveImgDep(ss, data, shadowStuff.shadow_w, shadowStuff.shadow_w);
	//		//cv::Mat m(shadowStuff.shadow_w, shadowStuff.shadow_h, CV_32F, data);
	//		//cv::imwrite(ss, m);
	//	}
	//	delete[] data;
	//}

	// 恢复视口、颜色模板、光照
	glViewport(0, 0, windowWidth, windowHeight);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if (li) glEnable(GL_LIGHTING);

	//-------------------------------------------第2次绘制，绘制场景------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 1 环境光
	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i].enable)
			continue;
		glDisable(GL_LIGHT0 + i);
	}
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_walls);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		up[0], up[1], up[2]);
	draw_world();
	draw_model();

	//2 点光源
	GLfloat la[4]; glGetFloatv(GL_LIGHT_MODEL_AMBIENT, la);
	float gac[4] = { 0,0,0,1 }; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac); // black
	glEnable(GL_LIGHT0);
	glActiveTexture(GL_TEXTURE1); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_walls);
	glEnable(GL_BLEND);
	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);
#ifdef SINGEL_LIGHT
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		up[0], up[1], up[2]);
	Point p = lights[0].position;
	GLfloat lightpos[] = { p[0], p[1], p[2], p[3] };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos); // 位置式光源
	draw_world();
	draw_model();
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la); // 恢复环境光
	glDepthFunc(GL_LESS); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
#else
	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i].enable)
			continue;
		//active shadow texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowStuff.tex_shadow[i]);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye[0], eye[1], eye[2],
			center[0], center[1], center[2],
			up[0], up[1], up[2]);
		double bias[4][4] = {
			{ 0.5, 0.0, 0.0, 0.0 },
			{ 0.0, 0.5, 0.0, 0.0 },
			{ 0.0, 0.0, 0.5, 0.0 },
			{ 0.5, 0.5, 0.49, 1.0 }
		};
		GLMat mat_view = lookAtMat(Vec(eye[0], eye[1], eye[2]), Vec(center[0], center[1], center[2]), up);
		GLMat biasMat(bias);
		GLMat mat = biasMat * shadowMatP * shadowMatV[i];//* mat_view.affineInverse();
		mat = mat.transpose();
		glTexGendv(GL_S, GL_EYE_PLANE, mat[0]);
		glTexGendv(GL_T, GL_EYE_PLANE, mat[1]);
		glTexGendv(GL_R, GL_EYE_PLANE, mat[2]);
		glTexGendv(GL_Q, GL_EYE_PLANE, mat[3]);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye[0], eye[1], eye[2],
			center[0], center[1], center[2],
			up[0], up[1], up[2]);

		//enable light
		Point p = lights[i].position;
		GLfloat lightpos[] = { p[0], p[1], p[2], p[3] };
		glLightfv(GL_LIGHT0 + i, GL_POSITION, lightpos);
		glEnable(GL_LIGHT0 + i);

		glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye[0], eye[1], eye[2],
			center[0], center[1], center[2],
			up[0], up[1], up[2]);
		draw_world();
		glDisable(GL_TEXTURE_2D);
		draw_model();
		glDisable(GL_LIGHT0 + i);
	}
#endif
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la); // 恢复环境光
	glDepthFunc(GL_LESS); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
//	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
//	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
}




void World::keyboard(unsigned char key, int x, int y) {
	const double moveStep = 1;
	const double rotateStep = 8;

	switch (key) {
	case 27: exit(0);
	case 'a': case 'A':  //出现图形变扁的原因可能是我的这种移动一个像素转某个角度的逻辑有问题
		move(0, -moveStep);
		break;
	case 'd': case 'D':
		move(0, moveStep);
		break;
	case 'w': case 'W':
		move(1, moveStep);
		break;
	case 's': case 'S':
		move(1, -moveStep);
		break;
	case 'z': case 'Z':
		move(2, -moveStep);
		break;
	case 'c': case 'C':
		move(2, moveStep);
		break;
	case '-':  //速度的调整
		moveSpeed -= 0.005;
		break;
	case '+':
		moveSpeed += 0.005;
		break;

	case 'j': case 'J':  //出现图形变扁的原因可能是我的这种移动一个像素转某个角度的逻辑有问题
		rotate(0, -rotateStep);
		break;
	case 'l': case 'L':
		rotate(0, rotateStep);
		break;
	case 'i': case 'I':
		rotate(1, rotateStep);
		break;
	case 'k': case 'K':
		rotate(1, -rotateStep);
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
void World::special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F1:
		grabScreen();
		break;
	default:
		break;
	}
}

//  TODO
void World::motion(int x, int y) {
}
void World::passiveMotion(int x, int y) {
	if (focusState == GLUT_ENTERED) {
		POINT pos; GetCursorPos(&pos);
		x = pos.x;  y = pos.y;
		elevation[0] += rotateSpeed * (x - cursorPos[0]);
		elevation[1] += -rotateSpeed * (y - cursorPos[1]);
		if (elevation[1] > 75) elevation[1] = 75;
		if (elevation[1] < -75) elevation[1] = -75;

		reCenter();
		glutPostRedisplay();
	}
}
void World::entry(int state) {
	focusState = state;
	if (state == GLUT_ENTERED)
		setCursorToCenter();
}
void World::mouseClick(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		mousedw(x, y, button);
	}
}


void World::mousedw(int x, int y, int but) {
	//printf("Mouse button %d pressed at %d %d\n", but, x, y);
	gl_select(x, windowHeight - y); //Important: gl (0,0) ist bottom left but window coords (0,0) are top left so we have to change this!
}

void World::gl_select(int x, int y) {
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
	drawAll();

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
		chosenID = choose;
	}
	else chosenID = -1;
	cout << "hits: " << hits << "   ID: " << chosenID << endl;

	glMatrixMode(GL_MODELVIEW);
}




/*  截图  */

string World::scrennshotsDir = "screenshots";


typedef unsigned char Byte;
typedef unsigned short int Word;
typedef unsigned long int DWord;
struct BitMapFileHeader {  //位图文件头
	Word bfType;  //不能放在里面一起读，因为short对齐成了32位??  //那要是对齐变成了64位的要怎么办??...(现在应该还不会出现这种情况...)
	DWord bfSize;  //说明该位图文件的大小，用字节为单位
	Word bfReserved1;
	Word bfReserved2;
	DWord bfOffBits;  //说明从 "the beginning of the file" 到 "the bitmap data" 的偏移量。  //对于一般的bmp来说是54
};
struct BitMapInfoHeader {
	DWord biSize;  //说明BITMAPINFOHEADER结构所需要的字节数
	DWord biWidth;
	DWord biHeight;  //biHeight为正，则图像是从左上往右下的顺序输入
	Word biPlanes;  //位平面数
	Word biBitCount;  //每像素位数
	DWord biCompression;  //压缩类型，0为不压缩
	DWord biSizeImage;  //压缩图像大小（字节数）
	DWord biXPelsPerMeter;  //水平分辨率
	DWord biYPelsPerMeter;
	DWord biClrUsed;  //使用的色彩数
	DWord biClrImportant;  //重要色彩数
};
#define BMP_Header_Length 54
//void getFiles(string path, vector<string>& files) {  
//    //文件句柄  
//    long   hFile   =   0;  
//    //文件信息  
//    struct _finddata_t fileinfo;
//    string p;  
//    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  
//    {  
//        do  
//        {  
//            //如果是目录,迭代之  
//            //如果不是,加入列表  
//            if((fileinfo.attrib &  _A_SUBDIR))  
//            {  
//                if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  
//                    getFiles( p.assign(path).append("\\").append(fileinfo.name), files );  
//            }  
//            else  
//            {  
//                files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
//            }  
//        }while(_findnext(hFile, &fileinfo)  == 0);  
//        _findclose(hFile);  
//    }  
//}
//int getMax(string s) {
//	int i,j;
//	for (i=s.size()-1; i>=0; i--)
//		if (s[i] == '.') break;
//		else if (s[i] == '\\') return -1;
//	for (j=i-1; j>=0; j--)
//		if (s[j] == '\\') break;
//	if (i<0 || j<0 || i==j+1) return -1;
//	return stringToInt(s.substr(j+1, i-j-1));
//}
void World::grabScreen(void) {  //TODO  截图时可以输出一个提示信息
	//static int times = -1;
	//if (times == -1) {
	//	vector<string> files;
	//	getFiles(scrennshotsDir, files);
	//	for (int i=0; i<files.size(); i++) times = max(times, getMax(files[i]));
	//}
	//times++;
	SYSTEMTIME sys;
	GetLocalTime(&sys);

	FILE*    pWritingFile;
	GLubyte* pPixelData;
	GLubyte  BMP_Header[BMP_Header_Length];
	GLint    i, j;
	GLint    PixelDataLength;

	// 计算像素数据的实际长度
	i = windowWidth * 3;   // 得到每一行的像素数据长度
	while (i % 4 != 0) ++i;

	PixelDataLength = i * windowHeight;

	// 分配内存和打开文件
	pPixelData = (GLubyte*)malloc(PixelDataLength);
	if (pPixelData == 0) exit(0);

	//fopen_s(&pWritingFile, (scrennshotsDir + "\\\\" + intToString(times) + ".bmp").c_str(), "wb");
	string s = intToString(sys.wYear) + "-" + intToString(sys.wMonth) + "-" + intToString(sys.wDay) + "-" + intToString(sys.wHour) + "-" + intToString(sys.wMinute) + "-" + intToString(sys.wSecond) + "-" + intToString(sys.wMilliseconds);
	fopen_s(&pWritingFile, (scrennshotsDir + "\\\\" + s + ".bmp").c_str(), "wb");
	if (pWritingFile == 0) exit(0);

	// 读取像素
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  //使得每行的像素以4的倍数对齐
	glReadPixels(0, 0, windowWidth, windowHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);


	BitMapFileHeader fileHeader;
	fileHeader.bfType = 0x4D42;
	fwrite(&fileHeader.bfType, sizeof(Word), 1, pWritingFile);

	BitMapInfoHeader infoHeader;
	int rowBytes = (24 >> 3) * windowWidth, offset = rowBytes % 4;
	if (offset != 0) rowBytes += offset = 4 - offset;
	fileHeader.bfSize = windowHeight * windowWidth * 3 + 54;
	fileHeader.bfReserved1 = fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = 54;
	fwrite(&fileHeader.bfSize, sizeof(DWord) * 3, 1, pWritingFile);

	infoHeader.biSize = 40;
	infoHeader.biWidth = windowWidth;
	infoHeader.biHeight = windowHeight;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = 0;
	infoHeader.biSizeImage = windowHeight * rowBytes;  //原文件比这个大2，居然也可以..(应该是原图忘了字节数为4的倍数这一点...)
	infoHeader.biXPelsPerMeter = infoHeader.biYPelsPerMeter = 0;  //??
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;
	fwrite(&infoHeader, sizeof(BitMapInfoHeader), 1, pWritingFile);

	// 把dummy.bmp的文件头复制为新文件的文件头
	//fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);  //OpenGL连BMP_Header都有...
	//fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET);
	i = windowWidth;
	j = windowHeight;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);

	// 写入像素数据
	fseek(pWritingFile, 0, SEEK_END);
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

	// 释放内存和关闭文件
	fclose(pWritingFile);
	free(pPixelData);
}