#ifndef __AI_H__
#define __AI_H__
#include "cgmath.h"
#include "cgut.h"
#include "colosseum.h"

extern float t;	// ��ü �ð�, �� ���� ����� ���� buffer�� ���� ��
extern struct Bullet bullet; //���߿� �Ѿ� ������ ����� ��������ߵ�. vecter<Bullet>�̶����..

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
	
	bool collision_true = 1; // �Ѿ��� �հ� �������µ��� ��� �浹 �Ͼ�°� ������. true �϶��� �浹��.   �浹�ϴ� ���� 0���� �ٲ�� ���� �ð� �� �ٽ� 1�� �ٲ�.
	vec3 collision_direction0 = vec3(0);  //�浹 ���������� �浹����.

	mat4	model_matrix;	// modeling transformation
	
	// public functions
	void	update( float t, const vec3& tpos );	
	void collision(vec3 bullet_pos);
};

ai_t ai = {0.3f, 0.3f, vec3(10.0f,colosseum.pos.y + colosseum.height * 0.5f + 0.5f * ai.height,-10.0f), vec4(1.0f,0.0f,0.0f,1.0f),30};

//********** ai ������ ��Ʈ *************
inline void ai_t::update( float t, const vec3& tpos )
{
	// A�� ai, B�� tank, O�� ����
	vec3 AB = vec3(tpos.x - pos.x, 0, tpos.z - pos.z);
	vec3 AO = vec3(-pos.x, 0, -pos.z);
	vec3 OB = vec3(tpos.x, 0, tpos.z);
	//float dt = t - t0;
	vec3 ref = vec3(0,1.0f,0);
	vec3 h = (AB.cross(ref)).normalize();
	float flag = h.dot(AO);

	// �ﰢ�� OAB���� B�� �а��� ��츦 �����ϰ�� h�� �̿��� ���
	// OB�� �����ϴ� ������ ��������� �������� A�� �̵��ϰ� �Ǵµ�
	// OB �ֺ��� A�� ��ġ�ϰ� �Ǹ� ������ �ϰ� �Ǳ� ������
	// B�� �а��� �ƴ� ��쿣 flag�� �۾����� ���� �������� A�� �̵�
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


	//********* �浹 �˻� **********//
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
	float n = 0.5f;		//�浹�� n�ʵ��� �̵��ϰ� (=ƨ���)
	float collision_speed = 0.35f;

	if (distance(vec4(bullet.pos, 1), vec4(pos, 1)) < (bullet.radius + radius))
	{
		if (collision_true==1)
		{
			play_sound();
			printf("collision! %d\n",collision_true);
			collision_t0 = t;
			collision_direction0 = ( pos - bullet.pos); //collision direction0 �� ��ġ��, �� ������ ���������� ����.
		}
	}

	if (t - collision_t0 < n) 
	{
		pos.x = pos.x + collision_direction0.x*collision_speed;
		pos.z = pos.z + collision_direction0.z*collision_speed;
		collision_true = 0;  //collision_true 0���� ���� ���浹 ���Ͼ��.
		printf("settrue 0 \n");
	}
	else
	{
		collision_true = 1;  //�ð� �� ������ �ٽ� �浹 �����ϰ�.
	}
}

#endif // __AI_H__
