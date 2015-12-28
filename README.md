# Computer Graphics
##### 2015 ZJU Computer Graphics Final Project
<br/>
# 目前进度：
1. 可以方便地绘制物体了。(具体的绘制步骤见"API使用方法"和“main.cpp”)
2. 用键盘可以操作相机前后左右上下移动了。(分别对应键盘的wsadcz) <br/>
   现在可以用鼠标操控了。
3. 用键盘可以旋转相机的角度了，并且可以朝着相机的朝向前进。(键盘上的jlik分别对应往左、往右、往上、往下旋转)
4. 鼠标左键点选物体已实现，不过具体操作还没加上去，现在只能看cmd输出。

<br/>

## 基本世界定义：
定义y轴正方向为竖直向上的方向。<br/>
初始时看向x正方向，此时右侧为z轴正方向。
<br/>

## API使用方法：
#### 在世界中添加物体
1. 在BasicElement.h和BasicElement.cpp定义继承自BasicElement的类，并重载drawNaive函数。
2. 在main函数中：
	1. 调用initWorld函数对世界进行初始化
	2. 定义自己定义类(或已有BasicElement的子类)的对象，并设置坐标、朝向(旋转角)等
	3. 然后再调用glutMainLoop就可以开始显示定义的物体了
(注：光照可能需要自己定义。另外，如果要用到材质或纹理坐标，可能需要自行定义一个新的BasicElement类)


## 基本类定义
#### 用于创建和管理三维世界的类：World
```
class World {
private:
	static unsigned int nextId;

	/*  此map结构用于保存世界中的所有物体  */
	static map<unsigned int, BasicElement *> objects;

	/*  此双端队列用于维护已删除的三维物体id  */
	static deque<unsigned int> trash;

	static unsigned int getNextId();
	static unsigned int getNextId(BasicElement *be);
	static int erase(unsigned int id);

public:
	static int windowHandle;
	static int windowHeight, windowWidth;

	static Point eye, center;
	static const Vec up;  //up向量不能被修改
	static double moveSpeed;

	static unsigned int chosenID;


	/*  初始化函数  */
	static void init(int *argc, char *argv[], int windowHeight=800, int windowWidth=800);

	/*  移动相机  */
	static void move(double dx, double dy, double dz);

	/*  旋转相机角度(角度制)  */
	static void rotate(double angle, double x, double y, double z);

	/*  一次性绘制所有物体  */
	static void drawAll();

	friend class BasicElement;
};
```

#### 用于表示基本三维物体的结构：BasicElement
```
class BasicElement {
private:
	unsigned int id;

public:
	Point centroid;  //体心
	BasicElement();
	~BasicElement();
	unsigned int getId();


	/*  draw函数不仅绘制了物体，还会对物体进行相应的平移和旋转操作  */
	void draw();

	/*  drawNaive函数类似glutSolidTeapot函数，将物体以原点为中心进行绘制  */
	virtual void drawNaive();


	/*  此函数的使用类似glRotate函数的使用，只不过它可以多次叠加使用。
		(就是rotate(90, 1, 1, 1)和rotate(100, 1, 0, 1)这两个旋转操作可以叠加)
		另外，此函数的angle参数为角度制                                        */
	void rotate(double angle, double x, double y, double z);

	/*  相对位移函数  */
	void move(double dx, double dy, double dz);

	/*  把物体的中心移动到某个位置  */
	void moveTo(Point p);
};
```

#### 用于表示用obj等格式读入的三维物体的结构：Model
```
class Model
{
protected:
      struct v{
            double x;
            double y;
            double z;
      };
      struct f{
            unsigned int v1,v2,v3;
      };
	  struct vt{
		double x,y;
	  };
	  GLuint DrawListID;
	  std::vector<v> vertices;
	  std::vector<v> vert_nor;
      std::vector<f> faces,face_tex;
	  std::vector<vt> texture_ord;
	  std::vector<f> normals;
public:
	//在绘制obj之前记得读取一个obj文件，在opengl初始化之后再读取
	  virtual void Readobj(const char * filename);
	  //绘制函数
      virtual void draw();
	  Model();
	  //拷贝构造
	  Model(const Model &);
      ~Model(void);
};
```





## 还需要完善的地方：
1. 现有代码还未经过完整测试，若遇到bug还请在群里说一下。
2. 光源可能需要另外定义一个类，而不是使用BasicElement。



## 另：和丁诗伦讨论后有改动的地方：
1.	id的类型从long long改为了unsigned int，因为glLoadName函数本身只能接受Unsigned int类型的参数。
2.	drawBody函数改成了drawNaive
