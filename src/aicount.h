#pragma once


#include "cgmath.h"
#include "cgut.h"
#include "gl/glad/glad.h"





GLuint	vertex_array_aicount_1 = 0;
GLuint	vertex_array_aicount_2 = 0;
GLuint	vertex_array_aicount_3 = 0;
GLuint	vertex_array_aicount_4 = 0;
GLuint	vertex_array_aicount_5 = 0;
GLuint	vertex_array_aicount_6 = 0;
GLuint	vertex_array_aicount_7 = 0;
GLuint	vertex_array_aicount_8 = 0;
GLuint	vertex_array_aicount_9 = 0;
GLuint	vertex_array_aicount_10 = 0;
GLuint	vertex_array_aicount_11 = 0;
GLuint	vertex_array_aicount_12 = 0;
GLuint	vertex_array_aicount_13 = 0;
GLuint	vertex_array_aicount_14 = 0;
GLuint	vertex_array_aicount_15 = 0;
GLuint	vertex_array_aicount_16 = 0;


struct aicount_t {

	mat4 translate_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0.6f,
		0, 0, 1, -0.1f,
		0, 0, 0, 1
	};

	mat4 scale_matrix =
	{
		0.31f, 0, 0, 0,
		0, 0.32f, 0, 0,
		0, 0, 0.31f, 0,
		0, 0, 0, 1
	};

	mat4 model_matrix = translate_matrix * scale_matrix;


} ai_count;



std::vector<vertex> create_vertices_aicount(int i , int j)
{
	std::vector<vertex> v;


	v.push_back({ vec3(-1.0f, -1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f),vec2((float)i / 8,(float)j / 8) });	//좌측하단
	v.push_back({ vec3(0.5f, -1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f),vec2((float)i / 8 + (float)3 / 8 ,(float)j / 8) });	//우측하단
	v.push_back({ vec3(0.5f, -0.5f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec2((float)i / 8 + (float)3 / 8 ,(float)j / 8 + (float)1 / 8) });	//우측상단
	v.push_back({ vec3(-1.0f, -0.5f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec2((float)i / 8 ,(float)j / 8 + (float)1 / 8) });	//좌측상단

	return v;
}



void update_vertex_buffer_aicount(const std::vector<vertex>& vertices, GLuint* vertex_array_aicount )
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


	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (*vertex_array_aicount) glDeleteVertexArrays(1, vertex_array_aicount);
	*vertex_array_aicount = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!*vertex_array_aicount) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}



std::vector<vertex>	aicount_1_vertices = create_vertices_aicount(0, 7);
std::vector<vertex>	aicount_2_vertices = create_vertices_aicount(3, 7);
std::vector<vertex>	aicount_3_vertices = create_vertices_aicount(0, 6);

std::vector<vertex>	aicount_4_vertices = create_vertices_aicount(3, 6);
std::vector<vertex>	aicount_5_vertices = create_vertices_aicount(0, 5);
std::vector<vertex>	aicount_6_vertices = create_vertices_aicount(3, 5);
std::vector<vertex>	aicount_7_vertices = create_vertices_aicount(0, 4);
std::vector<vertex>	aicount_8_vertices = create_vertices_aicount(3, 4);

std::vector<vertex>	aicount_9_vertices = create_vertices_aicount(0, 3);
std::vector<vertex>	aicount_10_vertices = create_vertices_aicount(3, 3);
std::vector<vertex>	aicount_11_vertices = create_vertices_aicount(0, 2);
std::vector<vertex>	aicount_12_vertices = create_vertices_aicount(3, 2);
std::vector<vertex>	aicount_13_vertices = create_vertices_aicount(0, 1);
std::vector<vertex>	aicount_14_vertices = create_vertices_aicount(3, 1);
std::vector<vertex>	aicount_15_vertices = create_vertices_aicount(0, 0);
std::vector<vertex>	aicount_16_vertices = create_vertices_aicount(3, 0);



void update_aicount_vertexbuffers()
{
	
	update_vertex_buffer_aicount(aicount_1_vertices, &vertex_array_aicount_1);
	update_vertex_buffer_aicount(aicount_2_vertices, &vertex_array_aicount_2);
	update_vertex_buffer_aicount(aicount_3_vertices, &vertex_array_aicount_3);
	update_vertex_buffer_aicount(aicount_4_vertices, &vertex_array_aicount_4);
	update_vertex_buffer_aicount(aicount_5_vertices, &vertex_array_aicount_5);
	update_vertex_buffer_aicount(aicount_6_vertices, &vertex_array_aicount_6);
	update_vertex_buffer_aicount(aicount_7_vertices, &vertex_array_aicount_7);
	update_vertex_buffer_aicount(aicount_8_vertices, &vertex_array_aicount_8);
	update_vertex_buffer_aicount(aicount_9_vertices, &vertex_array_aicount_9);
	update_vertex_buffer_aicount(aicount_10_vertices, &vertex_array_aicount_10);
	update_vertex_buffer_aicount(aicount_11_vertices, &vertex_array_aicount_11);
	update_vertex_buffer_aicount(aicount_12_vertices, &vertex_array_aicount_12);
	update_vertex_buffer_aicount(aicount_13_vertices, &vertex_array_aicount_13);
	update_vertex_buffer_aicount(aicount_14_vertices, &vertex_array_aicount_14);
	update_vertex_buffer_aicount(aicount_15_vertices, &vertex_array_aicount_15);
	update_vertex_buffer_aicount(aicount_16_vertices, &vertex_array_aicount_16);
	
}
