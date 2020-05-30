#ifndef __AI_H__
#define __AI_H__
#include "cgmath.h"
#include "cgut.h"
#include "colosseum.h"
#include "camera.h"

extern float t;	// ��ü �ð�, �� ���� ����� ���� buffer�� ���� ��

void play_sound();

struct ai_t
{
	float	radius=0.5f;	// radius
	float	height=0.5f;	// height
	vec3	pos;			// position of tank
	vec4	color;			// RGBA color in [0,1]
	uint	NTESS=30;
	float	mass = radius*radius*height;
	float	speed = 0.005f / sqrt(mass);	// velocity of ai
	bool	death = 0;
	
	float t0=0;				// time buffer for halt
	
	float collision_t0=0;
	
	bool collision_true = 1; // �Ѿ��� �հ� �������µ��� ��� �浹 �Ͼ�°� ������. true �϶��� �浹��.   �浹�ϴ� ���� 0���� �ٲ�� ���� �ð� �� �ٽ� 1�� �ٲ�.
	vec3 collision_direction0 = vec3(0);  //�浹 ���������� �浹����.

	mat4	model_matrix;	// modeling transformation
	mat4 model_matrix_head;
	mat4 model_matrix_arm;
	
	// public functions
	void	update( float t, const vec3& tpos );	
	bool collision(vec3 tpos, float tradius, float tmass);
	bool collision_bullet(vec3 tpos, float tradius, float tmass);
	void	update_head(float t);
	void	update_arm();
};

// �ߺ��� ���� ai�� ����� �Լ�, ���̵��� ���� anum�� �����ϸ� ��
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
	a = { 0.3f + 0.15f * abs(cosrand()), 0.3f + 0.3f * abs(cosrand()), vec3(colosseum.radius / 1.4f * cosrand(), colosseum.pos.y + colosseum.height * 0.5f + 0.5f * a.height, colosseum.radius / 1.4f * cosrand()), vec4(0.5f,0.5f,0.5f,1.0f), 30};
	ais.push_back(a);

	int canum = 1; //current ai number

	int tempFlag = 0;

	//make ai
	while (canum < anum)
	{
	a = { 0.3f + 0.15f * abs(cosrand()), 0.3f + 0.3f * abs(cosrand()), vec3(colosseum.radius / 1.4f * cosrand(), colosseum.pos.y + colosseum.height * 0.5f + 0.5f * a.height, colosseum.radius / 1.4f * cosrand()), vec4(0.5f,0.5f,0.5f,1.0f), 30};

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
int num_death_ai = 0;
auto ais = std::move(create_ais(anum));

//********** ai ������ ��Ʈ *************
inline void ai_t::update( float t, const vec3& tpos )
{
	// Death �˻�
	if ( death == 0 ) {
		if (colosseum.radius + radius <=
				sqrt(
					(colosseum.pos.x-pos.x)*(colosseum.pos.x-pos.x) +
					(colosseum.pos.z-pos.z)*(colosseum.pos.z-pos.z)
					)
		   ) {
			death = 1; num_death_ai++; t0 = t; printf("Death\n");
		}
	}

	if ( death == 0 ) {
		// A�� ai, B�� tank, O�� ����
		vec3 AB = vec3(tpos.x - pos.x, 0, tpos.z - pos.z);
		vec3 AO = vec3(-pos.x, 0, -pos.z);
		vec3 OB = vec3(tpos.x, 0, tpos.z);
		vec3 ref = vec3(0,1.0f,0);
		vec3 h = (AB.cross(ref)).normalize();
		float flag = h.dot(AO);

		// �ﰢ�� OAB���� B�� �а��� ��츦 �����ϰ�� h�� �̿��� ���
		// OB�� �����ϴ� ������ ��������� �������� A�� �̵��ϰ� �Ǵµ�
		// OB �ֺ��� A�� ��ġ�ϰ� �Ǹ� ������ �ϰ� �Ǳ� ������
		// B�� �а��� �ƴ� ��쿣 flag�� �۾����� ���� �������� A�� �̵�
		// �ӵ��� �۾����� ���� ����Ű�� ������ ����
		if ( AB.dot(OB) <= 0 )
		{
			if ( flag >= 0 ) pos += h * speed;
			else pos += -h * speed;
		}
		else
		{
			if (flag > 2.0f * speed) pos += h * speed;
			else if (flag > -2.0f * speed) pos += AO.normalize() * speed;
			else pos += -h * speed;
		}
		
		pos.y = colosseum.pos.y + colosseum.height * 0.5f + 0.5f * height;
	}
	else if ( t - t0 < 1.0f ) {
		pos.y -= 0.05f;
	}

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

inline bool ai_t::collision(vec3 tpos, float tradius, float tmass)
{
	bool hit = false;

	if (death == 1) return false;	// ���� �� ��귮 ���̷��� �� �Ŷ� ���� ��
	float collision_time = 0.3f;		// ƨ�涧 �󸶳� �������� �����̴���  //�浹 �� ���浹 ���� interval
	float collision_speed = 0.2f * tmass / sqrt(mass);	// �浹 �� �ӵ��� ��� ���� �ӵ��� ���, �ڱ� ������ �ݺ���ϰ�

	if (distance(vec4(tpos, 1), vec4(pos, 1)) < (tradius + radius))
	{

		if (collision_true == 1)
		{
			hit = true;
			play_sound();
			//printf("collision! %d\n",collision_true);
			collision_t0 = t;
			collision_direction0 = (pos - tpos); //collision direction0 �� ��ġ��, �� ������ ���������� ����.
		}
	}

	if (t - collision_t0 < collision_time)
	{
		pos.x = pos.x + collision_direction0.x * collision_speed;
		pos.z = pos.z + collision_direction0.z * collision_speed;
		collision_true = 0;  //collision_true 0���� ���� ���浹 ���Ͼ��.
		//printf("settrue 0 \n");
	}
	else
	{
		collision_true = 1;  //�ð� �� ������ �ٽ� �浹 �����ϰ�.
	}

	return hit;
}

inline bool ai_t::collision_bullet(vec3 tpos, float tradius, float tmass)
{
	bool hit = false;
	if (death == 1) return false;	// ���� �� ��귮 ���̷��� �� �Ŷ� ���� ��
	float collision_time = 0.3f;		// ƨ�涧 �󸶳� �������� �����̴���  //�浹 �� ���浹 ���� interval
	float collision_speed = 0.2f * tmass / sqrt(mass);	// �浹 �� �ӵ��� ��� ���� �ӵ��� ���, �ڱ� ������ �ݺ���ϰ�

	if (distance(vec4(tpos, 1), vec4(pos, 1)) < (tradius + radius))
	{
		hit = true;
		if (collision_true == 1)
		{
			play_sound();
			//printf("collision! %d\n",collision_true);
			collision_t0 = t;
			collision_direction0 = (pos - tpos); //collision direction0 �� ��ġ��, �� ������ ���������� ����.
		}
	}

	if (t - collision_t0 < collision_time)
	{
		pos.x = pos.x + collision_direction0.x * collision_speed;
		pos.z = pos.z + collision_direction0.z * collision_speed;
		collision_true = 0;  //collision_true 0���� ���� ���浹 ���Ͼ��.
		//printf("settrue 0 \n");
	}
	else
	{
		collision_true = 1;  //�ð� �� ������ �ٽ� �浹 �����ϰ�.
	}

	return hit;
}

inline void ai_t::update_head(float t)
{
	mat4 translate_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, height*1.3f,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		cos(3*t), 0, sin(3*t), 0,
		0, 1, 0, 0,
		-sin(3*t), 0, cos(3*t), 0,
		0, 0, 0, 1
	};

	model_matrix_head = translate_matrix * model_matrix * rotation_matrix;
}

inline void ai_t::update_arm()
{
	mat4 translate_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, height *0.3f,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, cos(PI / 2), -sin(PI/2), 0,
		0, sin(PI/2), cos(PI / 2), 0,
		0, 0, 0, 1
	};

	mat4	scale_matrix =
	{
		0.8f,0,0,0,
		0,2.0f,0,0,
		0,0,0.8f,0,
		0,0,0,1

	};

	model_matrix_arm = translate_matrix * model_matrix * rotation_matrix*scale_matrix;
}

#endif // __AI_H__
