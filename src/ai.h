#ifndef __AI_H__
#define __AI_H__
#include "cgmath.h"
#include "cgut.h"
#include "colosseum.h"
#include "camera.h"

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
	float	mass = radius*radius*height;
	float	speed = 0.01f / mass;	// velocity of ai
	
	float t0=0;				// time buffer for halt
	
	float collision_t0=0;
	
	bool collision_true = 1; // �Ѿ��� �հ� �������µ��� ��� �浹 �Ͼ�°� ������. true �϶��� �浹��.   �浹�ϴ� ���� 0���� �ٲ�� ���� �ð� �� �ٽ� 1�� �ٲ�.
	vec3 collision_direction0 = vec3(0);  //�浹 ���������� �浹����.

	mat4	model_matrix;	// modeling transformation
	
	// public functions
	void	update( float t, const vec3& tpos );	
	void collision(vec3 tpos, float tradius);
};

// �ߺ��� ���� ai�� ����� �Լ�, ���̵��� ���� anum�� �����ϸ� ��
#define cosrand() cos((float)rand()) 
inline std::vector<ai_t> create_ais(int anum)
{
	std::srand((unsigned int) time(0)); //for rand
	
	//var for comparing to avoid collision or overlaps
	float compx = 0;
	float compz = 0;
	float compr = 0;
	float tempx = 0;
	float tempz = 0;
	float tempr = 0;
	float sumr = 0;
	float Dx = 0;
	float Dz = 0;

	std::vector<ai_t> ais;
	ai_t a;

	//first ai
	a = { 0.3f + abs(cosrand()), 0.3f + abs(cosrand()), vec3(colosseum.radius / 1.4f * cosrand(), colosseum.pos.y + colosseum.height * 0.5f + 0.5f * a.height, colosseum.radius / 1.4f * cosrand()), vec4(0.5f,0.5f,0.5f,1.0f), 30};
	ais.push_back(a);

	int canum = 1; //current ai number

	int tempFlag = 0;

	//make ai
	while (canum < anum)
	{
		a = { 0.3f + abs(cosrand()), 0.3f + abs(cosrand()), vec3(colosseum.radius / 1.4f * cosrand(), colosseum.pos.y + colosseum.height * 0.5f + 0.5f * a.height, colosseum.radius / 1.4f * cosrand()), vec4(0.5f,0.5f,0.5f,1.0f), 30};

		tempx = a.pos.x;
		tempz = a.pos.z;
		tempr = a.radius;

		for (int i = 0; i < canum; i++)
		{
			compx = ais[i].pos.x;
			compz = ais[i].pos.z;
			compr = ais[i].radius;
			Dx = compx - tempx;
			Dz = compz - tempz;
			sumr = compr + tempr;
			if (Dx*Dx + Dz*Dz <= sumr*sumr)
			{
				tempFlag = 1; break;
			}
		}
		
		if (tempFlag == 0)
		{
			ais.push_back(a);
			++canum;
		}
		
		tempFlag = 0;
	}

	return ais;
}

// ais vector ����
int anum = 3;	//ai ����
auto ais = std::move(create_ais(anum));

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
		pos += AO.normalize() * speed;
	}
	/*
	printf("%f %f %f %f\n", pos.x, pos.y, pos.z, length(vec2(pos.x,pos.z)));
	printf("%f %f %f\n", tpos.x, tpos.y, tpos.z);
	printf("%f %f %f\n", h.x, h.y, h.z);
	printf("%f\n", AB.dot(AO));
	*/
	pos.y = colosseum.pos.y + colosseum.height * 0.5f + 0.5f * height;

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

inline void ai_t::collision(vec3 tpos, float tradius)
{
	float n = 0.01f;		//�浹�� n�ʵ��� �̵��ϰ� (=ƨ���)
	float collision_speed = 0.5f / mass;	// ������ �ݺ���ϰ� ����

	if (distance(vec4(tpos, 1), vec4(pos, 1)) < (tradius + radius))
	{
		if (collision_true==1)
		{
			play_sound();
			printf("collision! %d\n",collision_true);
			collision_t0 = t;
			collision_direction0 = ( pos - tpos); //collision direction0 �� ��ġ��, �� ������ ���������� ����.
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
