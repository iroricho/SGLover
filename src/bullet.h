#ifndef __BULLET_H__
#define __BULLET_H__
#include "cgmath.h"
#include "cgut.h"
#include "camera.h"
#include "colosseum.h"

extern float t;	// 전체 시간, 단 정지 기능을 위해 buffer가 빠진 값

struct Bullet
{
	float	radius=0.0f;	// radius
	vec3	pos = vec3(0, -10.0f, 0);			// position of tank
	vec4	color;			// RGBA color in [0,1]
	uint	NTESS=30;
	float	mass=10.0f * radius*radius*radius;

	float t0 = 0;				// 총 쏜 시점에서의 시간
	vec3 pos0 = 0;			//총 쏜 시점에서의 탱크 위치
	vec3 n0 = vec3(0);   //총 쏜 시점에서의 방향. camera의 n방향. 
	float speed = 10.0f;	// velocity of Bullet

	mat4	model_matrix;	// modeling transformation

	bool	disappear_flag = 1;

	// public functions
	void	update(float t, const vec3& tpos);
	void launch(float t0, vec3 pos0, vec3 n0);
	void disappear();
};

std::vector<Bullet> bullet_list;
int bullet_num = 120;

//********** Bullet 움직임 파트 *************
inline void Bullet::launch(float _t0, vec3 _pos0, vec3 _n0)
{
	disappear_flag = 0;
	t0 = _t0;
	pos0 = _pos0;
	n0 = _n0;
	radius = 0.15f;
	mass = 140.0f * radius*radius*radius;
	speed = 10.5f / mass;
}

inline void Bullet::disappear()
{
	disappear_flag = 1;
	radius = 0.0f;
}

inline void Bullet::update(float t, const vec3& tpos)
{
	if (disappear_flag == 0) {
		// 불렛이 경기장 밖으로 나갔는지 검사
		if (colosseum.radius + radius <=
				sqrt(
					(colosseum.pos.x-pos.x)*(colosseum.pos.x-pos.x) +
					(colosseum.pos.z-pos.z)*(colosseum.pos.z-pos.z)
					)
		   ) { disappear(); }

		float dt = t - t0; //총 쏜 시점부터 흘러간 시간.
		pos.x = pos0.x + n0.x * speed * dt;
		pos.y = pos0.y + n0.y * speed * dt;
		pos.z = pos0.z + n0.z * speed * dt;
	}

	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		cos(2*t), 0, sin(2*t), 0,
		0, 1, 0, 0,
		-sin(2*t), 0, cos(2*t), 0,
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
