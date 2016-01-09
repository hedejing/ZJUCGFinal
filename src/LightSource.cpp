#include "LightSource.h"
#include "texture.h"
#include "World.h"
#include "utility.h"

ShadowStuff LightManager::shadowStuff;
vector<LightSource> LightManager::lights;

GLuint LightManager::tex_walls;

void (*LightManager::draw_world)() = NULL;

LightSource::LightSource(const Point &p):position(p) {
	enable = true;
}

LightSource::Attenuation::Attenuation() {}

ShadowStuff::ShadowStuff() {}

bool save_stencil = false;

/*
glActiveTexture(GL_TEXTURE0) 普通贴图
glActiveTexture(GL_TEXTURE1) 阴影纹理
系统支持的最大纹理宽度和长度，查询 glGetIntegerv(GL_MAX_TEXTURE_SIZE, GLint*)
在我的电脑上 最大纹理尺寸为 16382
有的可能是8192
*/


void LightManager::xoy_foor(float len, float tex_repeat, int subdivision)
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

void LightManager::shadow_init()
{
	glActiveTexture(GL_TEXTURE0); // 普通贴图
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	tex_walls = loadTexture("images\\checker.bmp");
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

void LightManager::addLight(const LightSource & light)
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

void LightManager::EnableLight(unsigned id)
{
	if (id >= lights.size())
	{
		fprintf(stderr, "Light id doesn't exist!!!\n");
		system("pause"); // pending...
	}
	else
		lights[id].enable = true;
}

void LightManager::DisableLight(unsigned id)
{
	if (id >= lights.size())
	{
		fprintf(stderr, "Light id doesn't exist!!!\n");
		system("pause"); // pending...
	}
	else
		lights[id].enable = false;
}

void saveImaeg()
{

}

void LightManager::displayWithShadow(void (*_draw_world)())
{
	draw_world = _draw_world;
	//---------------------------------------第1次绘制，生成深度纹理--------
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	GLMat shadowMatP = perspectiveMat(90, (double)World::windowWidth / World::windowHeight, 1.0, 1.0e10f);
	//glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);
	//glMatrixMode(GL_PROJECTION); glPushMatrix();
	//glMatrixMode(GL_MODELVIEW); glPushMatrix();
#ifdef SINGEL_LIGHT
	GLMat shadowMatV = lookAtMat(Vec(lights[0].position[0], lights[0].position[1], lights[0].position[2]), Vec(0, 0, 0), up);
	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// hedjin : value of near must be 1.0 !!! 
	World::perspective();  //gluPerspective(90, (double)windowWidth / windowHeight, 1.0f, 1.0e10f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Point localeye = lights[0].position;
	gluLookAt(localeye[0], localeye[1], localeye[2], 0, 0, 0, up[0], up[1], up[2]);
	draw_world();
	glDisable(GL_CULL_FACE); glCullFace(GL_BACK);
#else
	vector<GLMat> shadowMatV;
	shadowMatV.resize(lights.size());
	Point shadowCenter(0, 0, 0);
	//shadowCenter = World::center;
	shadowCenter = Point(0, 1, 0);
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < lights.size(); i++)
	{
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowStuff.tex_shadow[i], 0);
		//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow[i], 0);
//		glBindTexture(GL_TEXTURE_2D, shadowStuff.tex_shadow[i]);
		//glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);

		glClear(GL_DEPTH_BUFFER_BIT);
		shadowMatV[i] = lookAtMat(lights[i].position, shadowCenter, World::up);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		World::perspective();
		//gluPerspective(90, (double)World::windowWidth / World::windowHeight, 1.0f, 1.0e10f);// hedjin : value of near must be 1.0 !!! 
		//gluPerspective(90, 1, 1.0f, 1.0e10f);// hedjin : value of near must be 1.0 !!! 
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Point localeye = lights[i].position;
		gluLookAt(localeye[0], localeye[1], localeye[2], shadowCenter[0], shadowCenter[1], shadowCenter[2], World::up[0], World::up[1], World::up[2]);
		draw_world();
	}
	//glDisable(GL_TEXTURE_2D);
#endif
	//glMatrixMode(GL_PROJECTION); glPopMatrix();
	//glMatrixMode(GL_MODELVIEW); glPopMatrix();
	//glDisable(GL_CULL_FACE); glCullFace(GL_BACK);

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
	
	if (save_stencil) {
		save_stencil = false;
		puts("Save");
		glActiveTexture(GL_TEXTURE1); glEnable(GL_TEXTURE_2D);
		GLfloat* data = new GLfloat[shadowStuff.shadow_w*shadowStuff.shadow_h];
		for (int i = 0; i < lights.size(); ++i) {
			glBindTexture(GL_TEXTURE_2D, shadowStuff.tex_shadow[i]);

			//glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);//获取纹理数据
			char ss[50]; sprintf(ss, "d%d.ppm", i);
			int w = shadowStuff.shadow_w;
			int h = shadowStuff.shadow_h;
			FILE *f = fopen(ss, "w");
			fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
			for (int i = 0; i < w*h; i++)
			{
				int value = max(0.0f, min(1.0f, data[i])) * 255;
				fprintf(f, "%d %d %d ", value, value, value);
			}
			fclose(f);
		}
		//delete[] data;
	}

	// 恢复视口、颜色模板、光照
	glViewport(0, 0, World::windowWidth, World::windowHeight);
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
	World::lookAt();
	draw_world();

	//2 点光源
	GLfloat la[4]; glGetFloatv(GL_LIGHT_MODEL_AMBIENT, la);
	float gac[4] = { 0,0,0,1 }; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac); // black
	glEnable(GL_LIGHT0);
	glActiveTexture(GL_TEXTURE1); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, LightManager::tex_walls);
	glEnable(GL_BLEND);
	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);
#ifdef SINGEL_LIGHT
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	World::lookAt();
	Point p = lights[0].position;
	GLfloat lightpos[] = { p[0], p[1], p[2], p[3] };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos); // 位置式光源
	draw_world();
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
		//glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
		//glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, shadowStuff.tex_shadow[i]);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		World::lookAt();
		double bias[4][4] = {
			{ 0.5, 0.0, 0.0, 0.0 },
			{ 0.0, 0.5, 0.0, 0.0 },
			{ 0.0, 0.0, 0.5, 0.0 },
			{ 0.5, 0.5, 0.49, 1.0 }
		};
		GLMat mat_view = lookAtMat(World::eye, World::center, World::up);
		GLMat biasMat(bias);
		GLMat mat = biasMat * shadowMatP * shadowMatV[i];// *mat_view.affineInverse();
		mat = mat.transpose();
		glTexGendv(GL_S, GL_EYE_PLANE, mat[0]);
		glTexGendv(GL_T, GL_EYE_PLANE, mat[1]);
		glTexGendv(GL_R, GL_EYE_PLANE, mat[2]);
		glTexGendv(GL_Q, GL_EYE_PLANE, mat[3]);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		World::lookAt();

		//enable light
		Point p = lights[i].position;
		GLfloat lightpos[] = { p[0], p[1], p[2], p[3] };
		glLightfv(GL_LIGHT0 + i, GL_POSITION, lightpos);
		glEnable(GL_LIGHT0 + i);

		glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		World::lookAt();
		draw_world();
		//glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHT0 + i);
	}
#endif
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la); // 恢复环境光
	glDepthFunc(GL_LESS); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_BLEND);
//	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
//	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
}
