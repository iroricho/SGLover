#ifndef __AI_H__
#define __AI_H__
#include "cgmath.h"
#include "cgut.h"

extern float t;	// ��ü �ð�, �� ���� ����� ���� buffer�� ���� ��
extern struct Bullet bullet; //���߿� �Ѿ� ������ ����� ��������ߵ�. vecter<Bullet>�̶����..

void play_sound();

//********** Temp var for adjust tank height *********
float cy = 0.0f;	// �ݷμ��� ���� ��ġ�Դϴ� (y��) �������ּ���

struct AI
{
	float radius = 0.3f;	// radius of AI
	vec3	pos;			// position of AI
	vec4	color;			// RGBA color in [0,1]

	float t0=0;				// time buffer for halt
	float speed = 0.01f;	// velocity of AI
	
	float collision_t0=0;
	bool collision_true = 1; // �Ѿ��� �հ� �������µ��� ��� �浹 �Ͼ�°� ������. true �϶��� �浹��.   �浹�ϴ� ���� 0���� �ٲ�� ���� �ð� �� �ٽ� 1�� �ٲ�.
	vec3 collision_direction0 = vec3(0);  //�浹 ���������� �浹����.

	mat4	model_matrix;	// modeling transformation
	
	// public functions
	void	update( float t, const vec3& tpos );	
	void collision(vec3 bullet_pos);
};

AI ai = {0.3f, vec3(7.0f,0,7.0f), vec4(1.0f,0.0f,0.0f,1.0f)};

// vertex buffer for AI 
GLuint	vertex_array_5 = 0;	// ID holder for vertex array object

//********** �𵨸� ��Ʈ *************
std::vector<vertex> create_ai_vertices( uint N )
{
	std::vector<vertex> v;
	
	for (uint j = 0; j <= N; j++)
	{
		for (uint k = 0; k <= N; k++)
		{
			float s = 1.0f,	// radius
				th = PI / 2.0f * (1.0f + float(j) / float(N)),	// theta
				p = 2.0f * PI * float(k) / float(N);	// phi
			v.push_back({ vec3(s * sin(th) * cos(p),s * sin(th) * sin(p),s * cos(th)),	//P spherical
				vec3(sin(th) * cos(p),sin(th) * sin(p),cos(th)),	// Normal vector of spherical
				vec2(p / (2.0f * PI),1.0f - th / PI) });	// Texture of sperical
		}
	}
	
	for (uint j = 0; j <= N; j++)
	{
		for (uint k = 0; k <= N; k++)
		{
			float s = 1.0f,	// radius
				th = PI / 2.0f *  float(j) / float(N),	// theta
				p = 2.0f * PI * float(k) / float(N);	// phi
			v.push_back({ vec3(s * sin(th) * cos(p),s * sin(th) * sin(p),s * cos(th)),	//P spherical
				vec3(sin(th) * cos(p),sin(th) * sin(p),cos(th)),	// Normal vector of spherical
				vec2(p / (2.0f * PI),1.0f - th / PI) });	// Texture of spherical
		}
	}

	return v;
}

void update_vertex_buffer_ai(const std::vector<vertex>& vertices, uint N)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	std::vector<uint> indices;

	for (uint j = 0; j < N; j++)
	{
		for (uint k = 0; k < N; k++)
		{
			uint l = k + j * (N + 1);
			indices.push_back(l);
			indices.push_back(l + 1);
			indices.push_back(l + N + 1);
		}
	}

	for (uint j = 1; j <= N; j++)
	{
		for (uint k = 0; k < N; k++)
		{
			uint l = k + j * (N + 1);
			indices.push_back(l - N);
			indices.push_back(l + 1);
			indices.push_back(l);
		}
	}

	for (uint j = 0; j < N; j++)
	{
		for (uint k = 0; k < N; k++)
		{
			uint l = k + j * (N + 1) + (N + 1) * (N + 1);
			indices.push_back(l);
			indices.push_back(l + 1);
			indices.push_back(l + N + 1);
		}
	}

	for (uint j = 1; j <= N; j++)
	{
		for (uint k = 0; k < N; k++)
		{
			uint l = k + j * (N + 1) + (N + 1) * (N + 1);
			indices.push_back(l - N);
			indices.push_back(l + 1);
			indices.push_back(l);
		}
	}

	// generation of vertex buffer: use vertices as it is
	glGenBuffers( 1, &vertex_buffer );
	glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers( 1, &index_buffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*indices.size(), &indices[0], GL_STATIC_DRAW );

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if(vertex_array_5) glDeleteVertexArrays(1,&vertex_array_5);
	vertex_array_5 = cg_create_vertex_array( vertex_buffer, index_buffer );
	if(!vertex_array_5){ printf("%s(): failed to create vertex aray\n",__func__); return; }
}

//********** AI ������ ��Ʈ *************
inline void AI::update( float t, const vec3& tpos )
{
	// A�� AI, B�� tank, O�� ����
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
	pos.y = cy + radius;


	//********* �浹 �˻� **********//
	collision(bullet.pos);

	



	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
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


inline void AI::collision(vec3 bullet_pos)
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
