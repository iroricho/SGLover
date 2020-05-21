#ifndef __BULLET_H__
#define __BULLET_H__
#include "cgmath.h"
#include "cgut.h"

extern float t;	// 전체 시간, 단 정지 기능을 위해 buffer가 빠진 값

//********** Temp var for adjust tank height *********

struct Bullet
{
	float radius = 0.1f;	// radius of Bullet
	vec3	pos;			// position of Bullet
	vec4	color;			// RGBA color in [0,1]

	float t0 = 0;				// 총 쏜 시점에서의 시간
	vec3 pos0 = 0;			//총 쏜 시점에서의 탱크 위치
	vec3 n0 = vec3(0);   //총 쏜 시점에서의 방향. camer의 n방향. 
	float speed = 10.0f;	// velocity of Bullet

	mat4	model_matrix;	// modeling transformation

	// public functions
	void	update(float t, const vec3& tpos);
	void launch(float t0, vec3 pos0, vec3 n0);
};

Bullet bullet = { 0.1f, vec3(7.0f,0,7.0f), vec4(1.0f,0.0f,0.0f,1.0f) };

// vertex buffer for Bullet 
GLuint	 vertex_array_6 = 0;	// ID holder for vertex array object

//********** 모델링 파트 *************
std::vector<vertex> create_bullet_vertices(uint N)
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
				th = PI / 2.0f * float(j) / float(N),	// theta
				p = 2.0f * PI * float(k) / float(N);	// phi
			v.push_back({ vec3(s * sin(th) * cos(p),s * sin(th) * sin(p),s * cos(th)),	//P spherical
				vec3(sin(th) * cos(p),sin(th) * sin(p),cos(th)),	// Normal vector of spherical
				vec2(p / (2.0f * PI),1.0f - th / PI) });	// Texture of spherical
		}
	}

	return v;
}

void update_vertex_buffer_bullet(const std::vector<vertex>& vertices, uint N)
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
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_6) glDeleteVertexArrays(1, &vertex_array_6);
	vertex_array_6 = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_6) { printf("%s(): fBulletled to create vertex aray\n", __func__); return; }
}

//********** Bullet 움직임 파트 *************
inline void Bullet::launch(float _t0, vec3 _pos0, vec3 _n0)
{
	
	t0 = _t0;
	pos0 = _pos0;
	n0 = _n0;
}



inline void Bullet::update(float t, const vec3& tpos)
{
	
	float dt = t - t0; //총 쏜 시점부터 흘러간 시간.



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
		1, 0, 0, pos0.x + n0.x * speed * dt,
		0, 1, 0, pos0.y,
		0, 0, 1, pos0.z + n0.z * speed * dt,
		0, 0, 0, 1
	};

	/*
	if (dt > 5000)
	{
		scale_matrix =
		{
			radius * 2 * sin(t), 0, 0, 0,
			0, radius * 2 * sin(t), 0, 0,
			0, 0, radius * 2 * sin(t), 0,
			0, 0, 0, 1
		};

	}
	*/
	
	model_matrix = translate_matrix * rotation_matrix * scale_matrix;


}

#endif // __BULLET_H__
