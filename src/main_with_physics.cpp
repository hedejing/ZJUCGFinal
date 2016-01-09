#include "World.h"
#include "BasicElement.h"
#include "texture.h"
#include "test.h"
#include "Physics.h"
#include "DrawScene.h"
using namespace std;

/*
	两种定义物体的方法：
	1. 在World::init()和glutMainLoop()之间定义。
	   (可以把定义写在World::int()之前，不过纹理定义等需要用到OpenGL函数的东西，必须要写在World::int()之后)
	2. 另开一个函数，然后用new来定义。当然，函数的引用位置也要在World::init()和glutMainLoop()之间。
	   (用new出来的指针可以对物体进行操作)


	注：在函数中如果不用new定义，就不能显示物体是因为函数结束时会调用那个对象的析构函数，然后那个对象就没了...
*/

void define() {
	double size = 1;
	GLuint tt=loadTexture("389a18c0.bmp");
	Cube *c = new Cube(Point(2, 2, 0),tt);
	c->rotateTo(45, 0, 0, 1);
	c->scaleValue = Vec(2, 2, 2);
	// hdj : CREATE a rigidbody for elements...
	// hdj : the first way is to design it by your own like this:
		//btCollisionShape* cubeShape = new btSphereShape(size);
		//btDefaultMotionState* cubeMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(2, 2, 0)));
		//btScalar mass = 1;
		//btVector3 fallInertia(0, 0, 0);
		//cubeShape->calculateLocalInertia(mass, fallInertia);
		//btRigidBody::btRigidBodyConstructionInfo cubeRigidBodyCI(mass, cubeMotionState, cubeShape, fallInertia);
		//btRigidBody* cubeRigidBody = new btRigidBody(cubeRigidBodyCI);

	//hdj: You can also use second way I provided...
	//hdj: This function is easy to use but also naive and a little bit dangerous...
	//hdj: be careful of the TYPE and always check the result
	btRigidBody* cubeRigidBody = Physics::CreateSimpleRigidBody(c, SimpleElementType::CUBE, 10); 
	if(cubeRigidBody!=NULL)
		Physics::AddRigidBodyAndElement(cubeRigidBody, c);

	Sphere *sphere = new Sphere(Point(2, 1, 2));  //球
	//btRigidBody* sphereRigidBody = Physics::CreateSimpleRigidBody(sphere, SimpleElementType::SPHERE);
	//if (sphereRigidBody != NULL)
	//	Physics::AddRigidBodyAndElement(sphereRigidBody, sphere);
	btRigidBody* sphereRigidBody = Physics::CreateRigidBodyForModelWithShape(sphere, Vec(1, 1, 1));
	Physics::AddRigidBodyAndElement(sphereRigidBody, sphere);

	Cone *cone = new Cone(Point(2, 2, -3));  //圆锥
	btRigidBody* coneRigidBody = Physics::CreateSimpleRigidBody(cone, SimpleElementType::CONE);
	if (coneRigidBody != NULL)
		Physics::AddRigidBodyAndElement(coneRigidBody, cone);

	Circle *circle = new Circle(Point(-2, 2, 2));  //圆
	btRigidBody* circleRigidBody = Physics::CreateSimpleRigidBody(circle, SimpleElementType::CIRCLE);
	if (circleRigidBody != NULL)
		Physics::AddRigidBodyAndElement(circleRigidBody, circle);

	Cylinder *cylinder = new Cylinder(Point(2, -2, 4));  //圆柱
	btRigidBody* cylinderRigidBody = Physics::CreateSimpleRigidBody(cylinder, SimpleElementType::CYLINDER);
	if (cylinderRigidBody != NULL)
		Physics::AddRigidBodyAndElement(cylinderRigidBody, cylinder);

	//调用了Physics:AddRigidBodyAndElement该物品就会接受物理世界的控制
	//所以如果调用了Physics:AddRigidBodyAndElement的话尽量不要像下面这样直接修改物体
	//cylinder->rotateTo(45, 0, 0, 1);

	//AviBoard *avi = new AviBoard(Point(0, 30, 0), 100, 100, "test2.rgb");
	
	objectmodel *tempobj1 = new objectmodel(Point(-50, 0, 4), "./obj/f1.obj");
	tempobj1->rotate(90, 0, 0, 1);
	tempobj1->addToPhysicsWorld(30, 10);
	//getTreesObj();
	objectmodel *tempobj2 = new objectmodel(Point(-30, -4, -10), "./obj/f2.obj");
	tempobj2->rotate(90, 0, 0, 1);
	tempobj2->rotate(-90, 1, 0, 0);
	tempobj2->addToPhysicsWorld(30, 10);
	
	objectmodel *tempobj3 = new objectmodel(Point(-30, 2, 10), "./obj/f3.obj");
	tempobj3->rotate(90, 0, 0, 1);
	tempobj3->rotate(180, 1, 0, 0);
	tempobj3->addToPhysicsWorld(30, 10);
	
	objectmodel *tempobj4 = new objectmodel(Point(30, 0, 10), "./obj/f4.obj");
	tempobj4->rotate(90, 0, 0, 1);
	tempobj4->rotate(90, 1, 0, 0);
	tempobj4->addToPhysicsWorld(30, 10);
	
	objectmodel *tempobj5 = new objectmodel(Point(-30, 0, 30), "./obj/f5.obj");
	tempobj5->rotate(90, 0, 0, 1);
	tempobj5->rotate(-90, 1, 0, 0);
	tempobj5->addToPhysicsWorld(30, 10);
	
	objectmodel *tempobj6 = new objectmodel(Point(-30, 0, -30), "./obj/f1.obj");
	tempobj6->rotate(90, 0, 0, 1);
	tempobj6->rotate(-90, 1, 0, 0);
	tempobj6->addToPhysicsWorld(30, 10);

	objectmodel *tempobj7 = new objectmodel(Point(25, 0, -20), "./obj/house1.obj");
	
	tempobj7->rotate(-90, 1, 0, 0);
	
	DrawScene_Of_CS("");
	//InitSkyBox(1999);

}

void draw_world()
{
	glMatrixMode(GL_MODELVIEW);
	//DrawSkyBox();
	glPushMatrix();
	glRotatef(90, -1, 0, 0);
	glTranslatef(-10, -10, 0);
	//LightManager::xoy_foor(200, 3, 100);
	glPopMatrix();
	//glPushMatrix();

	//glTranslatef(0, height, 0);
	//glutSolidCube(2);

	//glTranslatef(2, 0, 2);
	//glRotatef(-90, 1, 0, 0);
	//glutSolidCone(1, 2, 50, 50);
	//glRotatef(90, 1, 0, 0);

	//glTranslatef(0, 0, -4);
	//glutSolidSphere(1, 50, 50);

	//glTranslatef(-4, 1, 0);
	//glRotatef(45, 1, 0, 0);
	//glutSolidTorus(0.5f, 1, 50, 50);
	//glRotatef(-45, 1, 0, 0);
	//glPopMatrix();
}

Bullet *bullet;
btRigidBody* bulletRigidBody;

void update()
{
	// hdj : call update every time before redisplay
	Physics::update();
	glutPostRedisplay();
}

void shoot()
{
	//set the position of spwan
	Vec direct = (World::center - World::eye).normalize();
	Point spwan = World::eye + 10*direct;
	double radius = 0.05;
	bullet = new Bullet(spwan, radius);

	btCollisionShape* bulletShape = new btSphereShape(radius);
	btDefaultMotionState* bulletMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(spwan[0], spwan[1], spwan[2])));
	btScalar mass = 0.5;
	btVector3 fallInertia(0, 0, 0);
	bulletShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo bulletRigidBodyCI(mass, bulletMotionState, bulletShape, fallInertia);
	bulletRigidBody = new btRigidBody(bulletRigidBodyCI);
	bulletRigidBody->setLinearVelocity(100*btVector3(direct[0], direct[1], direct[2]));

	Physics::AddRigidBodyAndElement(bulletRigidBody, bullet);
}

void free()
{
	Physics::free();
}

int main(int argc, char *argv[]) {
	World::init(&argc, argv);
	World::_display = draw_world;
	World::_free = free;
	Physics::init();

	{  //设置相机的碰撞检测
		CameraModel *cameraModel = new CameraModel(Point(12, 12, 12));
		cameraModel->scale(Vec(2, 2, 2));
		btRigidBody *cameraModelRigidBody = Physics::CreateSimpleRigidBody(cameraModel, SimpleElementType::CAMERAMODEL, 5);
		if (cameraModelRigidBody != NULL) Physics::AddRigidBodyAndElement(cameraModelRigidBody, cameraModel);


		World::cameraModel = cameraModel;
		World::cameraModelRigidBody = cameraModelRigidBody;
	}
	World::setCamera(Point(12, 12, 12), Point(0, 12, 0));
	//World::setCamera(Point(-10, 10, 10), Point(0, 0, 0));


	//可以在这里使用glutIdleFunc();对idle进行重写
	//若要对鼠标和键盘函数进行重写，建议在World.cpp里修改
	glutIdleFunc(update);


	define();
	//Rect rect(Point(0, -3, 0));// , loadTexture("images//win7.bmp"));
	//rect.rotateTo(90, 0, 0, 1);
	//rect.scale(3, 3, 4);

	//
	//Rect rect2(Point(5, 1, 0));// , loadTexture("images//win7.bmp"));
	//rect2.scale(3, 3, 4);
	////rect2.rotateTo(90, 1, 0, 0);
	//rect2.rotate(90, 0, 1, 0);
	{  //LIGHTING
		//auto c = new Cube(Point(10, 10, 10));
		LightSource ls(Point(100, 100, 100, 1));
		LightManager::addLight(ls);
		//LightSource ls2(Point(-2, 6, 5, 1));
		//LightManager::addLight(ls2);
		//LightSource ls3(Point(-3, 6, -3, 1));
		//LightManager::addLight(ls3);
		//glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, Color(0.3, 0.3, 0.3, 0));
		glLightfv(GL_LIGHT0, GL_DIFFUSE, COLOR::white);
		glLightfv(GL_LIGHT0, GL_SPECULAR, COLOR::white);
		//glLightfv(GL_LIGHT0, GL_AMBIENT, Color(0, 0, 0, 0));//COLOR::white);
		//glLightfv(GL_LIGHT0, GL_DIFFUSE, Color(1, 0.2, 0.2, 0));//COLOR::white);
		//glLightfv(GL_LIGHT0, GL_SPECULAR, Color(1, 0.2, 0.2, 0));//COLOR::white);
		//glLightfv(GL_LIGHT0, GL_DIFFUSE, Color(1, 1, 1, 0));//COLOR::white);
		//glLightfv(GL_LIGHT0, GL_SPECULAR, Color(1,1, 1, 0));//COLOR::white);

		//glLightfv(GL_LIGHT1, GL_AMBIENT, Color(0, 0, 0, 0));//COLOR::white);
		//glLightfv(GL_LIGHT1, GL_DIFFUSE, Color(0.2, 0.2, 1, 0));//COLOR::white);
		//glLightfv(GL_LIGHT1, GL_SPECULAR, Color(0.2, 0.2, 1, 0));//COLOR::white);
		//glLightfv(GL_LIGHT1, GL_DIFFUSE, Color( 1, 1, 1, 0));//COLOR::white);
		//glLightfv(GL_LIGHT1, GL_SPECULAR, Color(1, 1, 1, 0));//COLOR::white);

		//glLightfv(GL_LIGHT2, GL_AMBIENT, Color(0, 0, 0, 0));//COLOR::white);
		//glLightfv(GL_LIGHT2, GL_DIFFUSE, Color(0.2, 1, 0.2, 0));//COLOR::white);
		//glLightfv(GL_LIGHT2, GL_SPECULAR, Color(0.2, 1, 0.2, 0));//COLOR::white);

		/*GLfloat dir[] = { 0, 0,-1};
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);*/
		glEnable(GL_LIGHT0);  //light0只是用做示例
		//glEnable(GL_LIGHT1);
		//glEnable(GL_LIGHT2);
	}

	glutMainLoop();

	return 0;
}
