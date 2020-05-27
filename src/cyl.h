#ifndef __CYL_H__
#define __CYL_H__
#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
// 원통을 모델링 해놓은 헤더 파일입니다.
// 반지름 1.0f, 높이 1.0f인 원기둥입니다.
// 이후 사용할 때 적절히 scale해서 사용하시면 됩니다.
// 경기장 처럼 큰 경우 TESS의 수를 늘려야 합니다.
// 경기장, 탱크, 탄, ai 모두 이 구조체를 사용합니다.
// tank 객체 정의는 camera.h에 되어 있습니다.

GLuint	vertex_array_0 = 0;	// cylinder vertex_array

struct cyl
{
	float	radius=0.5f;	// radius
	float	height=0.5f;	// height
	vec3	pos;			// position of tank
	vec4	color;			// RGBA color in [0,1]
	uint	NTESS=29;
	mat4	model_matrix;	// modeling transformation

	// public functions
	void	update_colosseum();	
	void	update_tank( float t, const vec3& eye, const vec3& at );	
	void	update_bullet( float t, const vec3& eye, const vec3& at );	
	void	update_ai( float t, const vec3& eye, const vec3& at );	
};


//위쪽 매핑하는 원기둥//
std::vector<vertex> create_cyltop_vertices(uint N)
{
	std::vector<vertex> v;

	float theta = 2.0f * PI / N;

	//upside vertices
	v.push_back({ vec3(0,0.5f,0), vec3(0,0.5f,0), vec2(0.125f,0.875f) });	//origin of upside
	for (uint i = 0; i <= N; i++)
	{
		v.push_back({ vec3(sin(i * theta),0.5f,cos(i * theta)), vec3(sin(i * theta),0.5f,cos(i * theta)), vec2(0.125f + 0.125f * cos(i * theta),0.875f + 0.125f * sin(i * theta)) });
	}

	//downside vertices
	v.push_back({ vec3(0,-0.5f,0), vec3(0,-0.5f,0), vec2(0.5f,0.5f) });	//origin of downside
	for (uint i = 0; i <= N; i++)
	{
		v.push_back({ vec3(sin(i * theta),-0.5f,cos(i * theta)), vec3(sin(i * theta),-0.5f,cos(i * theta)), vec2(i * theta,0) });
	}
	return v;
}

void update_vertex_buffer_cyltop(const std::vector<vertex>& vertices, uint N)
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
	
	// upside indices
	for (uint i=0; i<N; i++)
	{
		indices.push_back(0);
		indices.push_back(i+1);
		indices.push_back(i+2);
		
		indices.push_back(0);
		indices.push_back(i+2);
		indices.push_back(i+1);
	}

	// downside indices
	for (uint i=N+2; i<2*N+2; i++)
	{
		indices.push_back(N+2);
		indices.push_back(i+2);
		indices.push_back(i+1);
		
		indices.push_back(N+2);
		indices.push_back(i+1);
		indices.push_back(i+2);
	}

	// side indices
	for (uint i=1; i<N+1; i++)
	{
		indices.push_back(i);
		indices.push_back(i+1);
		indices.push_back(i+N+2);
		
		indices.push_back(i);
		indices.push_back(i+N+2);
		indices.push_back(i+1);
		
		indices.push_back(i+N+2);
		indices.push_back(i+N+3);
		indices.push_back(i+1);
		
		indices.push_back(i+N+2);
		indices.push_back(i+1);
		indices.push_back(i+N+3);
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
	if(vertex_array_0) glDeleteVertexArrays(1,&vertex_array_0);
	vertex_array_0 = cg_create_vertex_array( vertex_buffer, index_buffer );
	if(!vertex_array_0){ printf("%s(): failed to create vertex aray\n",__func__); return; }
}





//옆면 매핑하는 원기둥//
std::vector<vertex> create_cylside_vertices(uint N)
{
	std::vector<vertex> v;

	float theta = 2.0f * PI / N;

	//upside vertices
	v.push_back({ vec3(0,0.5f,0), vec3(0,0.5f,0), vec2(0.125f,0.875f) });	//origin of upside
	for (uint i = 0; i <= N; i++)
	{
		v.push_back({ vec3(sin(i * theta),0.5f,cos(i * theta)), vec3(sin(i * theta),0.5f,cos(i * theta)), vec2(0.125f + 0.125f * cos(i * theta),0.875f + 0.125f * sin(i * theta)) });
	}

	//downside vertices
	v.push_back({ vec3(0,-0.5f,0), vec3(0,-0.5f,0), vec2(0.5f,0.5f) });	//origin of downside
	for (uint i = 0; i <= N; i++)
	{
		v.push_back({ vec3(sin(i * theta),-0.5f,cos(i * theta)), vec3(sin(i * theta),-0.5f,cos(i * theta)), vec2(i * theta,0) });
	}
	return v;
}

#endif
