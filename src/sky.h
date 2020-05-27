#ifndef __SKY_H__
#define __SKY_H__
#include "cgmath.h"
#include "cgut.h"

extern float t;	// ��ü �ð�, �� ���� ����� ���� buffer�� ���� ��

struct Sky
{
	float	radius=0.5f;	// radius
	vec3	pos;			// position of tank
	vec4	color;			// RGBA color in [0,1]
	uint	NTESS=29;

	float t0 = 0;				// �� �� ���������� �ð�
	vec3 pos0 = 0;			//�� �� ���������� ��ũ ��ġ
	vec3 n0 = vec3(0);   //�� �� ���������� ����. camer�� n����. 
	float speed = 10.0f;	// velocity of Sky

	mat4	model_matrix;	// modeling transformation

	// public functions
	void	update(float t, const vec3& tpos);
	void launch(float t0, vec3 pos0, vec3 n0);
};

Sky sky = { 300,  vec3(0.0f,0,0.0f), vec4(1.0f,0.0f,0.0f,1.0f), 29};

//********** Sky ������ ��Ʈ *************


inline void Sky::update(float t, const vec3& tpos)
{
	

	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		cos(0.1f*t), 0,-sin(0.1f * t), 0,
		0, 1, 0, 0,
		sin(0.1f * t), 0, cos(0.1f * t), 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};


	
	
	model_matrix = translate_matrix * rotation_matrix * scale_matrix;
}

#endif //__SKY_H__
