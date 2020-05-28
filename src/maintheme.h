#pragma once
#include "cgmath.h"
#include "cgut.h"


GLuint	vertex_array_maintheme = 0;	// maintheme vertex_array
GLuint	vertex_array_button = 0;	// buttonstart vertex_array
//GLuint	vertex_array_buttonhelp = 0;	// buttonhelp vertex_array

struct main_theme {

	mat4 translate_matrix=
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 model_matrix = translate_matrix;


} theme;

struct button_start {

	mat4 translate_matrix =
	{
		1, 0, 0, 0.8f,
		0, 1, 0, 0.5f,
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

	mat4 model_matrix = translate_matrix*scale_matrix;


} bt_start;

struct button_help {

	mat4 translate_matrix =
	{
		1, 0, 0, 0.8f,
		0, 1, 0, 0.3f,
		0, 0, 1, -0.1f,
		0, 0, 0, 1
	};

	mat4 scale_matrix =
	{
		0.3f, 0, 0, 0,
		0, 0.31f, 0, 0,
		0, 0, 0.3f, 0,
		0, 0, 0, 1
	};

	mat4 model_matrix = translate_matrix*scale_matrix;


} bt_help;


std::vector<vertex> create_vertices_maintheme()
{
	std::vector<vertex> v;


	v.push_back({ vec3(-1.0f, -1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec2(0.0f,0.0f) });	//좌측하단
	v.push_back({ vec3(1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec2(1.0f,0.0f) });	//우측하단
	v.push_back({ vec3(1.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f,1.0f) });	//우측상단
	v.push_back({ vec3(-1.0f, 1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), vec2(0.0f,1.0f) });	//좌측상단

	return v;
}

std::vector<vertex> create_vertices_button()
{
	std::vector<vertex> v;


	v.push_back({ vec3(-1.0f, -1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec2(0.0f,0.0f) });	//좌측하단
	v.push_back({ vec3(0.5f, -1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec2(1.0f,0.0f) });	//우측하단
	v.push_back({ vec3(0.5f, -0.5f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec2(1.0f,1.0f) });	//우측상단
	v.push_back({ vec3(-1.0f, -0.5f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec2(0.0f,1.0f) });	//좌측상단

	return v;
}



void update_vertex_buffer_maintheme(const std::vector<vertex>& vertices)
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
	if (vertex_array_maintheme) glDeleteVertexArrays(1, &vertex_array_maintheme);
	vertex_array_maintheme = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_maintheme) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}


void update_vertex_buffer_button(const std::vector<vertex>& vertices)
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
	if (vertex_array_button) glDeleteVertexArrays(1, &vertex_array_button);
	vertex_array_button = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_button) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}