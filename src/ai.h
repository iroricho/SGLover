#ifndef __AI_H__
#define __AI_H__
#include "cgmath.h"
#include "cgut.h"
#include "colosseum.h"

extern float t;	// 전체 시간, 단 정지 기능을 위해 buffer가 빠진 값
extern struct Bullet bullet; //나중에 총알 여러개 생기면 수정해줘야됨. vecter<Bullet>이라던가..

void play_sound();

struct ai_t
{
	float	radius=0.5f;	// radius
	float	height=0.5f;	// height
	vec3	pos;			// position of tank
	vec4	color;			// RGBA color in [0,1]
	uint	NTESS=30;

	float t0=0;				// time buffer for halt
	float speed = 0.01f;	// velocity of ai
	
	float collision_t0=0;
	
	bool collision_true = 1; // 총알이 뚫고 지나가는동안 계속 충돌 일어나는것 방지용. true 일때만 충돌함.   충돌하는 순간 0으로 바뀌고 일정 시간 후 다시 1로 바뀜.
	vec3 collision_direction0 = vec3(0);  //충돌 시점에서의 충돌방향.

	mat4	model_matrix;	// modeling transformation
	
	// public functions
	void	update( float t, const vec3& tpos );	
	void collision(vec3 bullet_pos);
};

ai_t ai = {0.3f, 0.3f, vec3(10.0f,colosseum.pos.y + colosseum.height * 0.5f + 0.5f * ai.height,-10.0f), vec4(1.0f,0.0f,0.0f,1.0f),30};

//********** ai 움직임 파트 *************
inline void ai_t::update( float t, const vec3& tpos )
{
	// A가 ai, B가 tank, O가 원점
	vec3 AB = vec3(tpos.x - pos.x, 0, tpos.z - pos.z);
	vec3 AO = vec3(-pos.x, 0, -pos.z);
	vec3 OB = vec3(tpos.x, 0, tpos.z);
	//float dt = t - t0;
	vec3 ref = vec3(0,1.0f,0);
	vec3 h = (AB.cross(ref)).normalize();
	float flag = h.dot(AO);

	// 삼각형 OAB에서 B가 둔각인 경우를 제외하고는 h를 이용할 경우
	// OB를 포함하는 직선에 가까워지는 방향으로 A가 이동하게 되는데
	// OB 주변에 A가 위치하게 되면 진동을 하게 되기 때문에
	// B가 둔각이 아닐 경우엔 flag가 작아지면 원점 방향으로 A가 이동
	if ( AB.dot(OB) <= 0 )
	{
		if ( flag >= 0 ) pos += h * speed;
		else pos += -h * speed;
	}
	else
	{
		if ( flag >= speed ) pos += h * speed;
		else if ( flag >= 0 ) pos += AO.normalize() * speed;
		else if ( flag >= -speed ) pos += AO.normalize() * speed;
		else pos += -h * speed;
		
	}
	/*
	printf("%f %f %f %f\n", pos.x, pos.y, pos.z, length(vec2(pos.x,pos.z)));
	printf("%f %f %f\n", tpos.x, tpos.y, tpos.z);
	printf("%f %f %f\n", h.x, h.y, h.z);
	printf("%f\n", AB.dot(AO));
	*/
	pos.y = colosseum.pos.y + colosseum.height * 0.5f + 0.5f * height;


	//********* 충돌 검사 **********//
	collision(bullet.pos);

	



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

inline void ai_t::collision(vec3 bullet_pos)
{
	float n = 0.5f;		//충돌시 n초동안 이동하게 (=튕기게)
	float collision_speed = 0.35f;

	if (distance(vec4(bullet.pos, 1), vec4(pos, 1)) < (bullet.radius + radius))
	{
		if (collision_true==1)
		{
			play_sound();
			printf("collision! %d\n",collision_true);
			collision_t0 = t;
			collision_direction0 = ( pos - bullet.pos); //collision direction0 을 고치면, 더 복잡한 물리구현도 가능.
		}
	}

	if (t - collision_t0 < n) 
	{
		pos.x = pos.x + collision_direction0.x*collision_speed;
		pos.z = pos.z + collision_direction0.z*collision_speed;
		collision_true = 0;  //collision_true 0으로 만들어서 재충돌 안일어나게.
		printf("settrue 0 \n");
	}
	else
	{
		collision_true = 1;  //시간 다 지나면 다시 충돌 가능하게.
	}
}

#endif // __AI_H__
