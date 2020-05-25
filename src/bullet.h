#ifndef __BULLET_H__
#define __BULLET_H__
#include "cgmath.h"
#include "cgut.h"

extern float t;	// 전체 시간, 단 정지 기능을 위해 buffer가 빠진 값

struct Bullet
{
	float	radius=0.5f;	// radius
	float	height=0.5f;	// height
	vec3	pos;			// position of tank
	vec4	color;			// RGBA color in [0,1]
	uint	NTESS=30;

	float t0 = 0;				// 총 쏜 시점에서의 시간
	vec3 pos0 = 0;			//총 쏜 시점에서의 탱크 위치
	vec3 n0 = vec3(0);   //총 쏜 시점에서의 방향. camer의 n방향. 
	float speed = 10.0f;	// velocity of Bullet

	mat4	model_matrix;	// modeling transformation

	// public functions
	void	update(float t, const vec3& tpos);
	void launch(float t0, vec3 pos0, vec3 n0);
};

Bullet bullet = { 0.0f, 0.3f, vec3(7.0f,0,7.0f), vec4(1.0f,0.0f,0.0f,1.0f), 30};

//********** Bullet 움직임 파트 *************
inline void Bullet::launch(float _t0, vec3 _pos0, vec3 _n0)
{
	
	t0 = _t0;
	pos0 = _pos0;
	n0 = _n0;
	radius = 0.1f;
}

inline void Bullet::update(float t, const vec3& tpos)
{
	
	float dt = t - t0; //총 쏜 시점부터 흘러간 시간.
	pos.x = pos0.x + n0.x * speed * dt;
	pos.y = pos0.y + n0.y * speed * dt;
	pos.z = pos0.z + n0.z * speed * dt;
	

	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, height, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, pos.x,
		0, 1, 0, pos.y,
		0, 0, 1, pos.z,
		0, 0, 0, 1
	};


	
	
	model_matrix = translate_matrix * rotation_matrix * scale_matrix;
}

#endif // __BULLET_H__
