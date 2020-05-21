#ifndef __AI_H__
#define __AI_H__
#include "cgmath.h"
#include "cgut.h"

extern float t;	// 전체 시간, 단 정지 기능을 위해 buffer가 빠진 값

//********** Temp var for adjust tank height *********
float CY = 0.0f;	// 콜로세움 윗면 위치입니다 (y축) 수정해주세요

struct AI
{
	float radius = 0.3f;	// radius of AI
	vec3	pos;			// position of AI
	vec4	color;			// RGBA color in [0,1]

	float t0=0;				// time buffer for halt
	float speed = 0.01f;	// velocity of AI

	mat4	model_matrix;	// modeling transformation
	
	// public functions
	void	update( float t, const vec3& tpos );	
};

AI ai = {0.3f, vec3(7.0f,0,7.0f), vec4(1.0f,0.0f,0.0f,1.0f)};

// vertex buffer for AI 
GLuint	vertex_array_5 = 0;	// ID holder for vertex array object

//********** 모델링 파트 *************
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

//********** AI 움직임 파트 *************
inline void AI::update( float t, const vec3& tpos )
{
	// A가 AI, B가 tank, O가 원점
	vec3 AB = vec3(tpos.x - pos.x, 0, tpos.z - pos.z);
	vec3 AO = vec3(-pos.x, 0, -pos.z);
	vec3 OB = vec3(tpos.x, 0, tpos.z);
	//float dt = t - t0;
	vec3 ref = vec3(0,1.0f,0);
	vec3 h = (AB.cross(ref)).normalize();

	// 삼각형 OAB에서 B가 둔각인 경우를 제외하고는 h를 이용할 경우
	// OB를 포함하는 직선에 가까워지는 방향으로 A가 이동하게 되는데
	// OB 주변에 A가 위치하게 되면 진동을 하게 되기 때문에
	// B가 둔각이 아닐 경우엔 그냥 원점으로 A가 이동
	if ( AB.dot(OB) <= 0 )
	{
		if ( h.dot(AO) >= 0 ) pos += h * speed;
		else pos += -h * speed;
	}
	else pos += AO.normalize() * speed;
	
	printf("%f %f %f %f\n", pos.x, pos.y, pos.z, length(vec2(pos.x,pos.z)));
	printf("%f %f %f\n", tpos.x, tpos.y, tpos.z);
	printf("%f %f %f\n", h.x, h.y, h.z);
	printf("%f\n", AB.dot(AO));

	pos.y = CY + radius;

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

#endif // __AI_H__
