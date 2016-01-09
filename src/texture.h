#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include <cstdlib>
#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdio>

const int BMP_Header_Length = 54;
#pragma pack(2)
struct BMPFileHeader {
	unsigned short	bfType;			// 文件类型
	unsigned long   bfSize;			// 文件大小
	unsigned short	bfReserved1;	// 保留位
	unsigned short	bfReserved2;	// 保留位
	unsigned long	bfOffBits;		// 数据偏移位置
};

#pragma pack()
// BMP信息头
struct BMPInfoHeader {
	unsigned long	biSize;			// 此结构大小
	long			biWidth;		// 图像宽度
	long			biHeight;		// 图像高度
	unsigned short	biPlanes;		// 调色板数量
	unsigned short	biBitCount;		// 每个象素对应的位数，24：24位图，32：带alpha通道的24位图
	unsigned long	biCompression;	// 压缩
	unsigned long	biSizeImage;	// 图像大小
	long			biXPelsPerMeter;// 横向分辨率
	long			biYPelsPerMeter;// 纵向分辨率
	unsigned long	biClrUsed;		// 颜色使用数
	unsigned long	biClrImportant;	// 重要颜色数
};

GLuint loadTexture(const char *fileName);  //返回值为0即为发生了错误
#endif