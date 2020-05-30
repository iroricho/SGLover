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

GLuint	vertex_array_AI = 0;	// cylinder vertex_array
GLuint	vertex_array_colosseum = 0;	// cylinder vertex_array
GLuint	vertex_array_tank = 0;	// cylinder vertex_array
GLuint	vertex_array_arm = 0;	//t손

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
};


//AI 매핑하는 원기둥//
std::vector<vertex> create_cyltop_vertices_AI(uint N)
{
	std::vector<vertex> v;

	float theta = 2.0f * PI / N;

	//upside vertices
	v.push_back({ vec3(0,0.5f,0), vec3(0,0.5f,0), vec2(0.125f,0.875f) });	//origin of upside
	for (uint i = 0; i <= N; i++)
	{
		//v.push_back({ vec3(sin(i * theta),0.5f,cos(i * theta)), vec3(sin(i * theta),0.5f,cos(i * theta)), vec2(0.25f + 0.25f*(i * theta)/PI, 0.5f) });
		v.push_back({ vec3(0.7f*sin(i * theta),0.5f,0.7f * cos(i * theta)), vec3(sin(i * theta),0.5f,cos(i * theta)), vec2(theta/(2.0f*PI)*i, 1.0f) });
	}

	//downside vertices
	v.push_back({ vec3(0,-0.5f,0), vec3(0,-0.5f,0), vec2(0.5f,0.5f) });	//origin of downside
	for (uint i = 0; i <= N; i++)
	{
		//v.push_back({ vec3(sin(i * theta),-0.5f,cos(i * theta)), vec3(sin(i * theta),-0.5f,cos(i * theta)),vec2(0.25f + 0.25f*(i * theta) / PI, 0.5f) });
		v.push_back({ vec3(0.7f * sin(i * theta),-0.5f,0.7f * cos(i * theta)), vec3(sin(i * theta),-0.5f,cos(i * theta)),vec2(theta / (2.0f*PI) * i , 0.0f) });
	}
	return v;
}

void update_vertex_buffer_cyltop_AI(const std::vector<vertex>& vertices, uint N)
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
	if(vertex_array_AI) glDeleteVertexArrays(1,&vertex_array_AI);
	vertex_array_AI = cg_create_vertex_array( vertex_buffer, index_buffer );
	if(!vertex_array_AI){ printf("%s(): failed to create vertex aray\n",__func__); return; }
}


//콜로세움용 원기둥//

std::vector<vertex> create_cyltop_vertices_colosseum(uint N)
{
	std::vector<vertex> v;

	float theta = 2.0f * PI / N;

	//upside vertices
	v.push_back({ vec3(0,0.5f,0), vec3(0,0.5f,0), vec2(0.5f,0.5f) });	//origin of upside
	for (uint i = 0; i <= N; i++)
	{
		//v.push_back({ vec3(sin(i * theta),0.5f,cos(i * theta)), vec3(sin(i * theta),0.5f,cos(i * theta)), vec2(0.125f + 0.125f * cos(i * theta),0.875f + 0.125f * sin(i * theta)) });
		v.push_back({ vec3(sin(i * theta),0.5f,cos(i * theta)), vec3(sin(i * theta),0.5f,cos(i * theta)), vec2( (cos(i * theta)+1.0f)/2.0f, (sin(i * theta)+1.0f) / 2.0f) });
	}

	//downside vertices
	v.push_back({ vec3(0,-0.5f,0), vec3(0,-0.5f,0), vec2(0,0) });	//origin of downside
	for (uint i = 0; i <= N; i++)
	{
		//v.push_back({ vec3(sin(i * theta),-0.5f,cos(i * theta)), vec3(sin(i * theta),-0.5f,cos(i * theta)),vec2(0.125f + 0.125f * cos(i * theta),0.875f + 0.125f * sin(i * theta)) });
		v.push_back({ vec3(sin(i * theta),-0.5f,cos(i * theta)), vec3(sin(i * theta),-0.5f,cos(i * theta)),vec2((cos(i * theta) + 1.0f) / 2.0f, (sin(i * theta) + 1.0f) / 2.0f)  });
	}
	return v;
}

void update_vertex_buffer_cyltop_colosseum(const std::vector<vertex>& vertices, uint N)
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
	for (uint i = 0; i < N; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);

		indices.push_back(0);
		indices.push_back(i + 2);
		indices.push_back(i + 1);
	}

	// downside indices
	for (uint i = N + 2; i < 2 * N + 2; i++)
	{
		indices.push_back(N + 2);
		indices.push_back(i + 2);
		indices.push_back(i + 1);

		indices.push_back(N + 2);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}

	// side indices
	for (uint i = 1; i < N + 1; i++)
	{
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + N + 2);

		indices.push_back(i);
		indices.push_back(i + N + 2);
		indices.push_back(i + 1);

		indices.push_back(i + N + 2);
		indices.push_back(i + N + 3);
		indices.push_back(i + 1);

		indices.push_back(i + N + 2);
		indices.push_back(i + 1);
		indices.push_back(i + N + 3);
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
	if (vertex_array_colosseum) glDeleteVertexArrays(1, &vertex_array_colosseum);
	vertex_array_colosseum = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_colosseum) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}


//탱크용 원기둥//
std::vector<vertex> create_cyltop_vertices_tank(uint N)
{
	std::vector<vertex> v;

	float theta = 2.0f * PI / N;

	//upside vertices
	v.push_back({ vec3(0,0.5f,0), vec3(0,0.5f,0), vec2(0.125f,0.625f) });	//origin of upside
	for (uint i = 0; i <= N; i++)
	{
		v.push_back({vec3(0.6f * sin(i * theta),0.5f, 0.6f * cos(i * theta)), vec3(sin(i * theta),0.5f,cos(i * theta)),  vec2(theta / (2.0f * PI) * i, 1.0f) });
	}

	//downside vertices
	v.push_back({ vec3(0,-0.5f,0), vec3(0,-0.5f,0), vec2(0.125f,0.625f) });	//origin of downside
	for (uint i = 0; i <= N; i++)
	{
		v.push_back({ vec3(0.6f * sin(i * theta),-0.5f, 0.6f * cos(i * theta)), vec3(sin(i * theta),-0.5f,cos(i * theta)), vec2(theta / (2.0f * PI) * i, 0.0f) });
	}
	return v;
}

void update_vertex_buffer_cyltop_tank(const std::vector<vertex>& vertices, uint N)
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
	for (uint i = 0; i < N; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);

		indices.push_back(0);
		indices.push_back(i + 2);
		indices.push_back(i + 1);
	}

	// downside indices
	for (uint i = N + 2; i < 2 * N + 2; i++)
	{
		indices.push_back(N + 2);
		indices.push_back(i + 2);
		indices.push_back(i + 1);

		indices.push_back(N + 2);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}

	// side indices
	for (uint i = 1; i < N + 1; i++)
	{
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + N + 2);

		indices.push_back(i);
		indices.push_back(i + N + 2);
		indices.push_back(i + 1);

		indices.push_back(i + N + 2);
		indices.push_back(i + N + 3);
		indices.push_back(i + 1);

		indices.push_back(i + N + 2);
		indices.push_back(i + 1);
		indices.push_back(i + N + 3);
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
	if (vertex_array_tank) glDeleteVertexArrays(1, &vertex_array_tank);
	vertex_array_tank = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_tank) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}



//AI 팔 원기둥//
std::vector<vertex> create_cyltop_vertices_AI_arm(uint N)
{
	std::vector<vertex> v;

	float theta = 2.0f * PI / N;

	//upside vertices
	v.push_back({ vec3(0,1.0f,0), vec3(0,1.0f,0), vec2(0.125f,0.875f) });	//origin of upside
	for (uint i = 0; i <= N; i++)
	{
		//v.push_back({ vec3(sin(i * theta),0.5f,cos(i * theta)), vec3(sin(i * theta),0.5f,cos(i * theta)), vec2(0.25f + 0.25f*(i * theta)/PI, 0.5f) });
		v.push_back({ vec3(0.2f*sin(i * theta),1.0f,0.2f * cos(i * theta)), vec3(sin(i * theta),0.5f,cos(i * theta)), vec2(theta / (2.0f * PI) * i, 1.0f) });
	}

	//downside vertices
	v.push_back({ vec3(0,-1.0f,0), vec3(0,-1.0f,0), vec2(0.5f,0.5f) });	//origin of downside
	for (uint i = 0; i <= N; i++)
	{
		//v.push_back({ vec3(sin(i * theta),-0.5f,cos(i * theta)), vec3(sin(i * theta),-0.5f,cos(i * theta)),vec2(0.25f + 0.25f*(i * theta) / PI, 0.5f) });
		v.push_back({ vec3(0.2f * sin(i * theta),-1.0f,0.2f * cos(i * theta)), vec3(sin(i * theta),-0.5f,cos(i * theta)),vec2(theta / (2.0f * PI) * i , 0.0f) });
	}
	return v;
}

void update_vertex_buffer_cyltop_AI_arm(const std::vector<vertex>& vertices, uint N)
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
	for (uint i = 0; i < N; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);

		indices.push_back(0);
		indices.push_back(i + 2);
		indices.push_back(i + 1);
	}

	// downside indices
	for (uint i = N + 2; i < 2 * N + 2; i++)
	{
		indices.push_back(N + 2);
		indices.push_back(i + 2);
		indices.push_back(i + 1);

		indices.push_back(N + 2);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}

	// side indices
	for (uint i = 1; i < N + 1; i++)
	{
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + N + 2);

		indices.push_back(i);
		indices.push_back(i + N + 2);
		indices.push_back(i + 1);

		indices.push_back(i + N + 2);
		indices.push_back(i + N + 3);
		indices.push_back(i + 1);

		indices.push_back(i + N + 2);
		indices.push_back(i + 1);
		indices.push_back(i + N + 3);
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
	if (vertex_array_arm) glDeleteVertexArrays(1, &vertex_array_arm);
	vertex_array_arm = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_arm) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}


#endif
