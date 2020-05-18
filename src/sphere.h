#ifndef __SPHERE_H__
#define __SPHERE_H__
#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility

struct sphere_t
{
	float	radius=1.0f;		// radius
	float	spino=0.0f;			// omega of spin
	float	revolr=0.0f;		// radius of revolution
	float	revolo=0.0f;		// omega of revolution
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update( float t );	
};

inline std::vector<sphere_t> create_spheres()
{
	std::vector<sphere_t> spheres;
	sphere_t s;
	
	//Sun
	s = {0.2f, 0.01f, 0.0f, 0.0f, vec4(1.0f,0.0f,0.0f,1.0f)};
	spheres.emplace_back(s);
	
	//Mercury
	s = {0.01f, 2.0f, 0.23f, 3.0f, vec4(0.4f,0.0f,0.4f,1.0f)};
	spheres.emplace_back(s);
	
	//Venus
	s = {0.03f, 1.8f, 0.31f, 2.0f, vec4(0.4f,0.0f,0.4f,1.0f)};
	spheres.emplace_back(s);
	
	//Earth
	s = {0.035f, 1.5f, 0.4f, 1.5f, vec4(0.4f,0.0f,0.4f,1.0f)};
	spheres.emplace_back(s);
	
	//Mars
	s = {0.033f, 1.9f, 0.48f, 2.5f, vec4(0.4f,0.0f,0.4f,1.0f)};
	spheres.emplace_back(s);

	//Jupiter
	s = {0.07f, 0.5f, 0.6f, 0.5f, vec4(0.4f,0.0f,0.4f,1.0f)};
	spheres.emplace_back(s);

	//Saturn
	s = {0.05f, 0.8f, 0.74f, 0.6f, vec4(0.4f,0.0f,0.4f,1.0f)};
	spheres.emplace_back(s);

	//Uranus
	s = {0.04f, 1.0f, 0.85f, 0.3f, vec4(0.4f,0.0f,0.4f,1.0f)};
	spheres.emplace_back(s);

	//Neptune
	s = {0.03f, 1.7f, 0.95f, 0.2f, vec4(0.4f,0.0f,0.4f,1.0f)};
	spheres.emplace_back(s);
	
	return spheres;
}

inline void sphere_t::update( float t )
{
	// Axis calibration
	float ang = PI / -2.0f;
	float ca = cos(ang), sa = sin(ang);

	// Spin
	float c	= -cos(spino * t), s = sin(spino * t);

	// Revolution
	float w = revolo*t;
	float x = revolr*cos(w);
	float y = revolr*sin(w);
	
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 roty_matrix =
	{
		ca, 0, sa, 0,
		0, 1, 0, 0,
		-sa, 0, ca, 0,
		0, 0, 0, 1
	};

	mat4 rotx_matrix =
	{
		1, 0, 0, 0,
		0, ca, -sa, 0,
		0, sa, ca, 0,
		0, 0, 0, 1
	};

//	mat4 rotz_matrix =
//	{
//		cos(ang), -sin(ang), 0, 0,
//		sin(ang), cos(ang), 0, 0,
//		0, 0, 1, 0,
//		0, 0, 0, 1
//	};

	mat4 rotation_matrix =
	{
		c, 0, s, 0,
		0, 1, 0, 0,
		-s, 0, c, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, x,
		0, 1, 0, 0,
		0, 0, 1, y,
		0, 0, 0, 1
	};
	
	model_matrix = translate_matrix * rotation_matrix * scale_matrix * roty_matrix * rotx_matrix;
}

#endif
