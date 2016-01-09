#include <cmath>
#include <Windows.h>
#include <io.h>
#include "World.h"
#include "utility.h"
#include "texture.h"
#include "DrawScene.h"

//#define NO_SHADOW
unsigned int World::nextId = 0;
map<unsigned int, BasicElement *> World::objects = map<unsigned int, BasicElement *>();
deque<unsigned int> World::trash = deque<unsigned int>();

int World::windowHandle;
int World::windowHeight, World::windowWidth;
int World::windowPos[2];
int World::cursorPos[2];
int World::focusState;

CameraModel *World::cameraModel = NULL;
int World::changing = 0;
btRigidBody *World::cameraModelRigidBody = NULL;
Point World::eye = Point(0, 0, 0);
Point World::center = Point(1, 0, 0);
const Vec World::up = Vec(0, 1, 0);
double World::elevation[2] = { 0.0, 0.0 };
double World::moveSpeed = 0.4;
double World::rotateSpeed = 0.1;
double World::zoomFactor = 1, World::zoomSpeed = 0.005;

unsigned int World::chosenID = -1;
int World::gameMode = 1;
int World::gameModeTotalNum = 2;

void(*World::_display)() = NULL;
void(*World::_free)() = NULL;

double World::worldBound[3][2] = {
	{ -100, 100 },  //x
	{ -100, 100 },  //y
	{ -100, 100 },  //z
};

int World::mouseState[3] = { GLUT_UP, GLUT_UP, GLUT_UP };



bool World::jumping = 0;
double World::jumpHeight = 0, World::jumpA = 0, World::jumpV = 0, World::jumpDt = 0.025;



extern vector<wall_info> Walls;
bool World::couldMoveTo(Point eye) {
	for (auto &wall : Walls) {
		auto t = eye - wall.position;
		bool flag = 1;
		for (int i=0; i<3; i++) flag &= (fabs(t[i]) < fabs(wall.size[i]+1.5));
		//ios::sync_with_stdio(false);
		//cout<<eye<<" "<<wall.position<<" | "<<t<<" "<<wall.size<<" | "<<flag<<endl;
		if (flag) return 0;
	}
	return 1;
}



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

		//glutFullScreen();
		//glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		ShowCursor(0);
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
#ifndef NO_SHADOW
		LightManager::shadow_init();
#endif
		glEnable(GL_BLEND);
	}
	{
		InitSkyBox();
	}
	//...
}
void World::setCursorToCenter() {
	SetCursorPos(cursorPos[0] = windowWidth / 2 + windowPos[0], cursorPos[1] = windowHeight / 2 + windowPos[1]);  //TODO  这里的cursorPos并不准确
}

void World::reCenter() {
	const double M_PI = 3.14159265358979323846;
	double theta = elevation[0] * M_PI / 180, fi = elevation[1] * M_PI / 180;
	Vec t = center - eye;
	center = eye + Vec(cos(fi)*cos(theta), sin(fi), cos(fi)*sin(theta))*t.abs();
}
double World::getCameraHeight() {
	return 2.5 + jumpHeight;
}
void World::syncWithCameraModel() {
	if (gameMode == GAME_MODE) {
		eye[1] = getCameraHeight();
		if (cameraModelRigidBody != NULL && cameraModel != NULL) {
			btTransform trans = cameraModelRigidBody->getWorldTransform();
			trans.setOrigin(btVector3(eye[0], eye[1], eye[2]));
			cameraModelRigidBody->setWorldTransform(trans);
		}
	}
}
void World::setCamera(Point _eye, Point _center) {
	++changing;
	eye = _eye;  center = _center;
	Vec t = center - eye;
	elevation[0] = atan2(t[2], t[0]) * 180 / M_PI;
	elevation[1] = atan2(t[1], hypot(t[0], t[2])) * 180 / M_PI;

	syncWithCameraModel();
	--changing;
}
void World::move(double dx, double dy, double dz) { move(Vec(dx, dy, dz)); }
void World::move(const Vec &ds) {
	++changing;
	Point _eye = eye;
	if (gameMode == GAME_MODE) {
		_eye[0] += ds[0];
		_eye[2] += ds[2];
		if (!couldMoveTo(_eye)) {
		}
		else {
			eye[0] += ds[0];
			eye[2] += ds[2];
			center[0] += ds[0];
			center[2] += ds[2];
		}
	}
	else {
		_eye += ds;
		if (!couldMoveTo(_eye)) {
		}
		else {
			eye += ds; center += ds;
		}
	}
	
	syncWithCameraModel();
	--changing;
}
void World::move(int d, double step) {
	if (d < 0 || d > 2) return;
	++changing;
	Point _eye = eye;
	if (gameMode == GAME_MODE) {
		if (d == 0) {
			Vec t = ((center - eye)*up).normalize();
			t = step * moveSpeed * t;
			
			_eye[0] += t[0];
			_eye[2] += t[2];
			cout<<"eye="<<eye<<"    "<<"_eye="<<_eye<<endl;
			if (!couldMoveTo(_eye)) {
			}
			else {
			puts("haha");
				eye[0] += t[0];
				eye[2] += t[2];
				center[0] += t[0];
				center[2] += t[2];
			}
		}
		else if (d == 1) {
			Vec t = (center - eye).normalize();
			t = step * moveSpeed *t;
			
			_eye[0] += t[0];
			_eye[2] += t[2];
			if (!couldMoveTo(_eye)) {
			}
			else {
				eye[0] += t[0];
				eye[2] += t[2];
				center[0] += t[0];
				center[2] += t[2];
			}
		}
		else {
			Vec t = step * moveSpeed * up;
			
			_eye[0] += t[0];
			_eye[2] += t[2];
			if (!couldMoveTo(_eye)) {
			}
			else {
				eye[0] += t[0];
				eye[2] += t[2];
				center[0] += t[0];
				center[2] += t[2];
			}
		}
	}
	else {
		if (d == 0) {
			Vec t = ((center - eye)*up).normalize();
			t = step * moveSpeed * t;

			_eye += t;
			if (!couldMoveTo(_eye)) {
			}
			else {
				eye += t; center += t;
			}
		}
		else if (d == 1) {
			Vec t = (center - eye).normalize();
			t = step * moveSpeed *t;

			_eye += t;
			if (!couldMoveTo(_eye)) {
			}
			else {
				eye += t;  center += t;
			}
		}
		else {
			_eye += step * moveSpeed * up;
			if (!couldMoveTo(_eye)) {
			}
			else {
				eye += step * moveSpeed * up;
				center += step * moveSpeed * up;
			}
		}
	}

	syncWithCameraModel();
	--changing;
}
void World::_move(double dx, double dy) {
	++changing;

	if (gameMode == GOD_MODE) {
		Vec up = Vec(0,1,0);

		//couldMoveTo判断
		Vec t = -(up*(center-eye)).normalize()*dx*0.1;
		eye += moveSpeed*t; center += moveSpeed*t;
		t = -up*dy*0.1;
		eye += moveSpeed*t; center += moveSpeed*t;
	}
	else {
	}
	--changing;
}
void World::rotate(double angle, double x, double y, double z) {}
void World::rotate(int d, double step) {
	if (d < 0 || d > 1) return;
	if (d == 0) elevation[0] += rotateSpeed * step;
	else elevation[1] += rotateSpeed * step;
	if (elevation[1] > 45) elevation[1] = 45;
	if (elevation[1] < -45) elevation[1] = -45;

	reCenter();
}
void World::zoom(double d) {
	zoomFactor += zoomSpeed * d;
	if (zoomFactor > 1.5) zoomFactor = 1.5;
	if (zoomFactor < 0.6) zoomFactor = 0.6;
}

void World::drawAll() {
	for (auto it = objects.begin(); it != objects.end();) {
		if (it->second->classType == 1 && !isInside(it->second->centroid)) {
			BasicElement *be = it->second;
			it++;
			delete be;
		}
		else {
			it->second->draw();
			it++;
		}
	}
	//for (auto o : objects) {
	//	if (!isInside(o.second->centroid)) {
	//		objects.erase(o.first);
	//		delete o.second;
	//	}
	//	else  o.second->draw();
	//}
	if (_display != NULL) _display();

	/*  鼠标的绘制  */
	glDisable(GL_LIGHTING);
	double length = 0.02, perc = 0.1;
	Vec right = (center - eye) * up; right = right.normalize()*length;
	Vec _up = right * (center - eye); _up = _up.normalize()*length;
	Vec east = right, west = -right;
	Vec north = _up, south = -_up;
	Point pos = eye + (center - eye).normalize()*1.1;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	glBegin(GL_LINES);
	//glColor3f(0.2, 0.2, 0.4);
	glColor3f(0, 0, 0);

	glVertex3f(east[0], east[1], east[2]);
	glVertex3f(east[0] * perc, east[1] * perc, east[2] * perc);
	glVertex3f(west[0], west[1], west[2]);
	glVertex3f(west[0] * perc, west[1] * perc, west[2] * perc);

	glVertex3f(north[0], north[1], north[2]);
	glVertex3f(north[0] * perc, north[1] * perc, north[2] * perc);
	glVertex3f(south[0], south[1], south[2]);
	glVertex3f(south[0] * perc, south[1] * perc, south[2] * perc);
	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

bool World::isInside(Point p) {
	if (p[0] < eye[0] + worldBound[0][0] || p[0] > eye[0] + worldBound[0][1]) return 0;
	if (p[1] < eye[1] + worldBound[1][0] || p[1] > eye[1] + worldBound[1][1]) return 0;
	if (p[2] < eye[2] + worldBound[2][0] || p[2] > eye[2] + worldBound[2][1]) return 0;
	return 1;
}

/*  GLU FUNC  */
void World::perspective() {
	gluPerspective(90*zoomFactor, (double)windowWidth / windowHeight, 1, 3000);  //听说fovy要设在45度以下才能获得比较好的效果
}
void World::lookAt() {
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
}

/*  GLUT FUNC  */
void World::idle() {
	glutPostRedisplay();
}


void World::display() {
	windowPos[0] = glutGet(GLUT_WINDOW_X);
	windowPos[1] = glutGet(GLUT_WINDOW_Y);
	windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	if (focusState == GLUT_ENTERED) setCursorToCenter();

	if (cameraModel != NULL && changing <= 0) {
		//puts("changing eye");
		Vec tmp = center - eye;
		//eye = cameraModel->centroid;
		//center = eye + tmp;
		syncWithCameraModel();
	}

	subtractBlood();
	solveJump();

#ifndef NO_SHADOW
	LightManager::displayWithShadow(drawAll);
#else
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	perspective();  //gluPerspective(45, (double)windowWidth / windowHeight, 0.1, 500);

	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		up[0], up[1], up[2]);
	drawAll();
#endif
	glutSwapBuffers();

	{
		int err = glGetError();
		//if (err != GL_NO_ERROR) printf("%d: %s\n", err, (char *)glewGetErrorString(err));
		//1281:  #define GL_INVALID_VALUE 0x0501
	}
}


extern void shoot();

//  下面函数修改后，请在README.md中注明
void World::keyboard(unsigned char key, int x, int y) {
	const double moveStep = 1;
	const double rotateStep = 8;

	switch (key) {
	case 27: {
				 if (_free != NULL)
					 _free();
				 exit(0);
	}
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
	case ' ':
		puts("haha");
		jump();//shoot();
		break;
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
	case GLUT_KEY_F6:  //不能用F12
		(++gameMode) %= gameModeTotalNum;
		break;
	default:
		break;
	}
}

//  TODO
void World::motion(int x, int y) {
	POINT pos;
	Point pcenter;
	btTransform trans;
	if (focusState == GLUT_ENTERED) {
		switch (mouseState[0] * 4 + mouseState[1] * 2 + mouseState[2]) {
			/*  orbit  */
		case 3:  //左键
			if (gameMode == GAME_MODE) {
			}
			else {
				GetCursorPos(&pos);
				x = pos.x;  y = pos.y;
				elevation[0] += rotateSpeed * (x - cursorPos[0]);
				elevation[1] += -rotateSpeed * (y - cursorPos[1]);
				if (elevation[1] > 45) elevation[1] = 45;
				if (elevation[1] < -45) elevation[1] = -45;

				pcenter = center;
				reCenter();
				eye = eye + (pcenter - center);
				center = pcenter;
			}
			break;

			/*  zoom  */
		case 5:  //中键
			GetCursorPos(&pos);
			zoom(pos.y - cursorPos[1]);
			break;

			/*  zoom to fit  */
		case 1:  //左键+中键
			zoomFactor = 1;
			break;

			/*  pan  */
		case 6:
			if (gameMode == GAME_MODE) {
			}
			else {
				GetCursorPos(&pos);
				x = pos.x;  y = pos.y;
				_move(x - cursorPos[0], y - cursorPos[1]);
			}
			break;
		default:
			break;
		}

		glutPostRedisplay();
	}
}
void World::passiveMotion(int x, int y) {
	POINT pos;
	if (focusState == GLUT_ENTERED) {
		GetCursorPos(&pos);
		x = pos.x;  y = pos.y;
		elevation[0] += rotateSpeed * (x - cursorPos[0]);
		elevation[1] += -rotateSpeed * (y - cursorPos[1]);
		if (elevation[1] > 45) elevation[1] = 45;
		if (elevation[1] < -45) elevation[1] = -45;
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
		if (gameMode == GAME_MODE) shoot();
		else mousedw(x, y, button);

	}
	mouseState[button] = state;

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
	perspective();  //gluPerspective(45*zoomFactor, 1, 1, 500);

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
	fwrite(&fileHeader.bfSize, sizeof(DWord)* 3, 1, pWritingFile);

	infoHeader.biSize = 40;
	infoHeader.biWidth = windowWidth;
	infoHeader.biHeight = windowHeight;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = 0;
	infoHeader.biSizeImage = windowHeight * rowBytes;
	infoHeader.biXPelsPerMeter = infoHeader.biYPelsPerMeter = 0;
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

void World::subtractBlood() {
	/*btCollisionConfiguration* bt_collision_configuration;
	btCollisionDispatcher* bt_dispatcher;
	btBroadphaseInterface* bt_broadphase;
	btCollisionWorld* bt_collision_world;

	double scene_size = 500;
	unsigned int max_objects = 16000;

	bt_collision_configuration = new btDefaultCollisionConfiguration();
	bt_dispatcher = new btCollisionDispatcher(bt_collision_configuration);

	btScalar sscene_size = (btScalar) scene_size;
	btVector3 worldAabbMin(-sscene_size, -sscene_size, -sscene_size);
	btVector3 worldAabbMax(sscene_size, sscene_size, sscene_size);
	//This is one type of broadphase, bullet has others that might be faster depending on the application
	bt_broadphase = new bt32BitAxisSweep3(worldAabbMin, worldAabbMax, max_objects, 0, true);  // true for disabling raycast accelerator

	bt_collision_world = new btCollisionWorld(bt_dispatcher, bt_broadphase, bt_collision_configuration);
	//Create two collision objects
	btCollisionObject* sphere_A = new btCollisionObject();
	btCollisionObject* sphere_B = new btCollisionObject();
	//Move each to a specific location
	sphere_A->getWorldTransform().setOrigin(btVector3((btScalar) 2, (btScalar) 1.5, (btScalar) 0));
	sphere_B->getWorldTransform().setOrigin(btVector3((btScalar) 2, (btScalar) 0, (btScalar) 0));
	//Create a sphere with a radius of 1
	btSphereShape * sphere_shape = new btSphereShape(1);
	//Set the shape of each collision object
	sphere_A->setCollisionShape(sphere_shape);
	sphere_B->setCollisionShape(sphere_shape);
	//Add the collision objects to our collision world
	bt_collision_world->addCollisionObject(sphere_A);
	bt_collision_world->addCollisionObject(sphere_B);

	//Perform collision detection
	bt_collision_world->performDiscreteCollisionDetection();

	int numManifolds = bt_collision_world->getDispatcher()->getNumManifolds();
	//For each contact manifold
	for (int i = 0; i < numManifolds; i++) {
		btPersistentManifold* contactManifold = bt_collision_world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
		const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());
		contactManifold->refreshContactPoints(obA->getWorldTransform(), obB->getWorldTransform());
		int numContacts = contactManifold->getNumContacts();
		//For each contact point in that manifold
		for (int j = 0; j < numContacts; j++) {
			//Get the contact information
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			btVector3 ptA = pt.getPositionWorldOnA();
			btVector3 ptB = pt.getPositionWorldOnB();
			double ptdist = pt.getDistance();
		}
	}

	delete bt_collision_configuration;
	delete bt_dispatcher;
	delete bt_broadphase;
	//delete bt_collision_world;
	delete sphere_A;
	delete sphere_B;
	delete sphere_shape;*/
}



/*  相机跳跃  */
void World::jump() {
	if (!jumping) {
		jumping = 1;
		jumpA = -10;
		jumpV = 7.5;
	}
}
void World::solveJump() {
	if (jumping) {
		jumpHeight = jumpHeight + jumpV*jumpDt;
		jumpV = jumpV + jumpA*jumpDt;
		if (jumpHeight < 0) jumping = 0;
	}
}
