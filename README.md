# Computer Graphics
##### 2015 ZJU Computer Graphics Final Project
<br/>
# 目前进度：
1. 可以方便地绘制物体了。(具体的绘制步骤见"API使用方法"和“main.cpp”)
2. ~~用键盘可以操作相机前后左右上下移动了。(分别对应键盘的wsadcz)~~ <br/>
   现在可以用鼠标操控了。
   - 按着左键拖动鼠标可以实现orbit功能。
   - 用中键往前后拖动可以放大、缩小
   - 左键、中键一起按：zoom to fit
   - 按着右键不放可以移动相机位置
3. 用键盘可以旋转相机的角度了，并且可以朝着相机的朝向前进。(键盘上的jlik分别对应往左、往右、往上、往下旋转)
4. 鼠标左键点选物体已实现，不过具体操作还没加上去，现在只能看cmd输出。
5. 按F1可以截图了。截图文件会放在screenshots文件夹中。
6. 已经可以绘制阴影了。

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
#### 用于创建和管理三维世界的类：[World](src/World.h)

#### 用于表示基本三维物体的结构：[BasicElement](src/BasicElement.h)

#### 用于放各种功能的函数、全局变量的文件：[utility](src/utility.h)

#### 用于表示用obj等格式读入的三维物体的结构：[Model](src/Model.h)




## 还需要完善的地方：
1. 现有代码还未经过完整测试，若遇到bug还请在群里说一下。
2. 光源可能需要另外定义一个类，而不是使用BasicElement。
3. 还可以认领的任务：
   - 多面棱柱、多面棱台
   - 鼠标的固定
   - 基本材质、纹理的编辑
   - 光源的编辑



## 另：和丁诗伦讨论后有改动的地方：
1.	id的类型从long long改为了unsigned int，因为glLoadName函数本身只能接受Unsigned int类型的参数。
2.	drawBody函数改成了drawNaive
