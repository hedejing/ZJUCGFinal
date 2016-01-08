#ifndef __LIGHTSOURCE_H__
#define __LIGHTSOURCE_H__

#include "utility.h"

struct ShadowStuff{
	//GLuint tex_shadow; // ÎÆÀíÃû×Ö
	vector<GLuint> tex_shadow;
	GLuint frame_buffer_s;
	const int shadow_w = 1024, shadow_h = 1024;
	ShadowStuff();
};

struct LightSource
{
	LightSource(const Point &p);
	bool enable;
	Color ambient;
	Color diffuse;
	Color specular;

	Point position;
	
	double spotExponent;
	double spotCutoff;

	struct Attenuation {
		Attenuation();

		double constant;
		double linear;
		double quadratic;
	} attenuation;
};

struct LightManager {
	static ShadowStuff shadowStuff;
	static vector<LightSource> lights;

	static GLuint tex_walls;


	static void shadow_init();
	static void addLight(const LightSource & light);
	static void EnableLight(unsigned id);
	static void DisableLight(unsigned id);
	static void xoy_foor(float len, float tex_repeat, int subdivision);
	static void (*draw_world)();

	static void displayWithShadow(void (*_draw_world)());
};

#endif
