
#include "texture.h"
#include <iostream>
using namespace std;

GLuint loadTexture(const char *fileName,bool sky) {  //返回值为0即为发生了错误
	FILE *fp = fopen(fileName, "rb");
	if (!fp) return 0;
	
	//..要判断文件类型

	int width, height;
    fseek(fp, 0x0012, SEEK_SET);
    fread(&width, 4, 1, fp);
    fread(&height, 4, 1, fp);
    fseek(fp, BMP_Header_Length, SEEK_SET);

	GLint total_bytes = height * ((width*3)-((width*3-1)%4+1) + 4);  //待测试。。  ??
	GLubyte *pixels = (GLubyte *)malloc(total_bytes);
	if (!pixels) { fclose(fp); return 0; }

	if (fread(pixels, total_bytes, 1, fp) <= 0) { free(pixels),fclose(fp); return 0; }
	
	{
		//图片缩放
		GLint maxLen;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxLen);
		if (maxLen < width || maxLen < height || (width&-width)!=width || (height&-height)!=height)
		{
			const int newWidth = 512, newHeight = 512;
			
			GLint newLineBytes, newTotalBytes;
			GLubyte *newPixels = 0;

			// 计算每行需要的字节数和总字节数
			newLineBytes = newWidth * 3;
			while (newLineBytes % 4 != 0) ++newLineBytes;
			newTotalBytes = newLineBytes * newHeight;

			// 分配内存
			newPixels = (GLubyte *)malloc(newTotalBytes);
			if (newPixels == 0)
			{
				free(pixels), fclose(fp);
				return 0;
			}

			// 进行像素缩放
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				newWidth, newHeight, GL_UNSIGNED_BYTE, newPixels);

			// 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
			free(pixels);
			pixels = newPixels;
			width = newWidth, height = newHeight;
		}
	}

	GLuint texture_ID;
	glGenTextures(1, &texture_ID);
	if (!texture_ID) { free(pixels),fclose(fp); return 0; }

	//绑定纹理
	GLint last_texture_ID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (!sky)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
	}
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);  //直接用纹理覆盖，没有光照效果
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);  //在做贴图时使用透明通道而不是glColor的alpha值参与blend运算。 贴图时:
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, last_texture_ID);

	fclose(fp);
	free(pixels);
	return texture_ID;
}
