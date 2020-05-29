#pragma once

#include "cgmath.h"
#include "cgut.h"
#include "gl/glad/glad.h"



GLuint	vertex_array_numbers_1 = 0;	
GLuint	vertex_array_numbers_2 = 0;
GLuint	vertex_array_numbers_3 = 0;
GLuint	vertex_array_numbers_4 = 0;
GLuint	vertex_array_numbers_5 = 0;
GLuint	vertex_array_numbers_6 = 0;
GLuint	vertex_array_numbers_7 = 0;
GLuint	vertex_array_numbers_8 = 0;
GLuint	vertex_array_numbers_9 = 0;
GLuint	vertex_array_numbers_10 = 0;
GLuint	vertex_array_numbers_11 = 0;
GLuint	vertex_array_numbers_12 = 0;
GLuint	vertex_array_numbers_13 = 0;
GLuint	vertex_array_numbers_14 = 0;
GLuint	vertex_array_numbers_15 = 0;
GLuint	vertex_array_numbers_16 = 0;
GLuint	vertex_array_numbers_17 = 0;
GLuint	vertex_array_numbers_18 = 0;
GLuint	vertex_array_numbers_19 = 0;
GLuint	vertex_array_numbers_20 = 0;
GLuint	vertex_array_numbers_21 = 0;
GLuint	vertex_array_numbers_22 = 0;
GLuint	vertex_array_numbers_23 = 0;
GLuint	vertex_array_numbers_24 = 0;
GLuint	vertex_array_numbers_25 = 0;
GLuint	vertex_array_numbers_26 = 0;
GLuint	vertex_array_numbers_27 = 0;
GLuint	vertex_array_numbers_28 = 0;
GLuint	vertex_array_numbers_29 = 0;
GLuint	vertex_array_numbers_30 = 0;
GLuint	vertex_array_numbers_31 = 0;
GLuint	vertex_array_numbers_32 = 0;
GLuint	vertex_array_numbers_33 = 0;
GLuint	vertex_array_numbers_34 = 0;
GLuint	vertex_array_numbers_35 = 0;
GLuint	vertex_array_numbers_36 = 0;
GLuint	vertex_array_numbers_37 = 0;
GLuint	vertex_array_numbers_38 = 0;
GLuint	vertex_array_numbers_39 = 0;
GLuint	vertex_array_numbers_40 = 0;
GLuint	vertex_array_numbers_41 = 0;
GLuint	vertex_array_numbers_42 = 0;
GLuint	vertex_array_numbers_43 = 0;
GLuint	vertex_array_numbers_44 = 0;
GLuint	vertex_array_numbers_45 = 0;
GLuint	vertex_array_numbers_46 = 0;
GLuint	vertex_array_numbers_47 = 0;
GLuint	vertex_array_numbers_48 = 0;
GLuint	vertex_array_numbers_49 = 0;
GLuint	vertex_array_numbers_50 = 0;
GLuint	vertex_array_numbers_51 = 0;
GLuint	vertex_array_numbers_52 = 0;
GLuint	vertex_array_numbers_53 = 0;
GLuint	vertex_array_numbers_54 = 0;
GLuint	vertex_array_numbers_55 = 0;
GLuint	vertex_array_numbers_56 = 0;
GLuint	vertex_array_numbers_57 = 0;
GLuint	vertex_array_numbers_58 = 0;
GLuint	vertex_array_numbers_59 = 0;
GLuint	vertex_array_numbers_60 = 0;



struct number_counter {


	mat4 model_matrix;
	float	radius = 0.2f;	// radius
	float	height = -0.3f;	// height
	vec3	pos;			// position of tank
	vec3 relative_pos = vec3(-5.0f, 2.0f, 0);	//상대거리
	void	update_counter(float t, const vec3& eye, const vec3& at);






} num_cnt;



inline void number_counter::update_counter(float t, const vec3& eye, const vec3& at)
{
	vec3 n = (eye - at).normalize();
	// tank pos랑 정확히 일치시키면 자기몸이 안 보여요 좀 떼어놓은 겁니다.
	pos = eye+ - 7.0f * n ;
	
	pos.y = eye.y - 1.5f * height+relative_pos.y;

	// 카메라 회전에 따라 몸체도 회전시키기 위함. n,ref 둘다 단위벡터임. y축 기준 회전
	vec3 ref = vec3(0, 0, 1.0f);
	float theta = 0;
	if (n.x >= 0) theta = acos(ref.dot(n));
	else theta = -acos(ref.dot(n));
	float c = cos(theta), s = sin(theta);

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		c, 0, s, 0,
		0, 1, 0, 0,
		-s, 0, c, 0,
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



std::vector<vertex> create_vertices_numbers(int i, int j)	
{
	std::vector<vertex> v;

	
	v.push_back({ vec3(-1.0f, -1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec2((float)i/8,(float)j/ 8) });	//좌측하단
	v.push_back({ vec3(1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec2((float)i/ 8 + (float)1/ 8 ,(float)j/ 8) });	//우측하단
	v.push_back({ vec3(1.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), vec2((float)i/ 8 + (float)1/ 8 ,(float)j/ 8 + (float)1/ 8) });	//우측상단
	v.push_back({ vec3(-1.0f, 1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), vec2((float)i/ 8 ,(float)j/ 8 +(float)1/ 8) });	//좌측상단
	
	/*
	v.push_back({ vec3(-1.0f, -1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec2(0.0f,0.0f) });	//좌측하단
	v.push_back({ vec3(1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec2(1.0f,0.0f) });	//우측하단
	v.push_back({ vec3(1.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f,1.0f) });	//우측상단
	v.push_back({ vec3(-1.0f, 1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), vec2(0.0f,1.0f) });	//좌측상단
	*/
	return v;
}



void update_vertex_buffer_numbers(const std::vector<vertex>& vertices,GLuint* vertex_array_numbers)
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
	if (*vertex_array_numbers) glDeleteVertexArrays(1, vertex_array_numbers);
	*vertex_array_numbers = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!*vertex_array_numbers) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}


std::vector<vertex>	number_1_vertices = create_vertices_numbers(0,7);
std::vector<vertex>	number_2_vertices = create_vertices_numbers(1,7);
std::vector<vertex>	number_3_vertices = create_vertices_numbers(2,7);

std::vector<vertex>	number_4_vertices = create_vertices_numbers(3,7);
std::vector<vertex>	number_5_vertices = create_vertices_numbers(4,7);
std::vector<vertex>	number_6_vertices = create_vertices_numbers(5,7);
std::vector<vertex>	number_7_vertices = create_vertices_numbers(6,7);
std::vector<vertex>	number_8_vertices = create_vertices_numbers(7,7);

std::vector<vertex>	number_9_vertices = create_vertices_numbers(0,6);
std::vector<vertex>	number_10_vertices = create_vertices_numbers(1,6);
std::vector<vertex>	number_11_vertices = create_vertices_numbers(2,6);
std::vector<vertex>	number_12_vertices = create_vertices_numbers(3,6);
std::vector<vertex>	number_13_vertices = create_vertices_numbers(4,6);
std::vector<vertex>	number_14_vertices = create_vertices_numbers(5,6);
std::vector<vertex>	number_15_vertices = create_vertices_numbers(6,6);
std::vector<vertex>	number_16_vertices = create_vertices_numbers(7,6);
std::vector<vertex>	number_17_vertices = create_vertices_numbers(0,5);
std::vector<vertex>	number_18_vertices = create_vertices_numbers(1,5);
std::vector<vertex>	number_19_vertices = create_vertices_numbers(2,5);
std::vector<vertex>	number_20_vertices = create_vertices_numbers(3,5);
std::vector<vertex>	number_21_vertices = create_vertices_numbers(4,5);
std::vector<vertex>	number_22_vertices = create_vertices_numbers(5,5);
std::vector<vertex>	number_23_vertices = create_vertices_numbers(6,5);
std::vector<vertex>	number_24_vertices = create_vertices_numbers(7,5);
std::vector<vertex>	number_25_vertices = create_vertices_numbers(0,4);
std::vector<vertex>	number_26_vertices = create_vertices_numbers(1,4);
std::vector<vertex>	number_27_vertices = create_vertices_numbers(2,4);
std::vector<vertex>	number_28_vertices = create_vertices_numbers(3,4);
std::vector<vertex>	number_29_vertices = create_vertices_numbers(4,4);
std::vector<vertex>	number_30_vertices = create_vertices_numbers(5,4);
std::vector<vertex>	number_31_vertices = create_vertices_numbers(6,4);
std::vector<vertex>	number_32_vertices = create_vertices_numbers(7,4);
std::vector<vertex>	number_33_vertices = create_vertices_numbers(0,3);
std::vector<vertex>	number_34_vertices = create_vertices_numbers(1,3);
std::vector<vertex>	number_35_vertices = create_vertices_numbers(2,3);
std::vector<vertex>	number_36_vertices = create_vertices_numbers(3,3);
std::vector<vertex>	number_37_vertices = create_vertices_numbers(4,3);
std::vector<vertex>	number_38_vertices = create_vertices_numbers(5,3);
std::vector<vertex>	number_39_vertices = create_vertices_numbers(6,3);
std::vector<vertex>	number_40_vertices = create_vertices_numbers(7,3);
std::vector<vertex>	number_41_vertices = create_vertices_numbers(0,2);
std::vector<vertex>	number_42_vertices = create_vertices_numbers(1,2);
std::vector<vertex>	number_43_vertices = create_vertices_numbers(2,2);
std::vector<vertex>	number_44_vertices = create_vertices_numbers(3,2);
std::vector<vertex>	number_45_vertices = create_vertices_numbers(4,2);
std::vector<vertex>	number_46_vertices = create_vertices_numbers(5,2);
std::vector<vertex>	number_47_vertices = create_vertices_numbers(6,2);
std::vector<vertex>	number_48_vertices = create_vertices_numbers(7,2);
std::vector<vertex>	number_49_vertices = create_vertices_numbers(0,1);
std::vector<vertex>	number_50_vertices = create_vertices_numbers(1,1);
std::vector<vertex>	number_51_vertices = create_vertices_numbers(2,1);
std::vector<vertex>	number_52_vertices = create_vertices_numbers(3,1);
std::vector<vertex>	number_53_vertices = create_vertices_numbers(4,1);
std::vector<vertex>	number_54_vertices = create_vertices_numbers(5,1);
std::vector<vertex>	number_55_vertices = create_vertices_numbers(6,1);
std::vector<vertex>	number_56_vertices = create_vertices_numbers(7,1);
std::vector<vertex>	number_57_vertices = create_vertices_numbers(0,0);
std::vector<vertex>	number_58_vertices = create_vertices_numbers(1,0);
std::vector<vertex>	number_59_vertices = create_vertices_numbers(2,0);
std::vector<vertex>	number_60_vertices = create_vertices_numbers(3,0);



void update_number_vertexbuffers()
{
	update_vertex_buffer_numbers(number_1_vertices, &vertex_array_numbers_1);
	update_vertex_buffer_numbers(number_2_vertices, &vertex_array_numbers_2);
	update_vertex_buffer_numbers(number_3_vertices, &vertex_array_numbers_3);
	
	update_vertex_buffer_numbers(number_4_vertices, &vertex_array_numbers_4);
	update_vertex_buffer_numbers(number_5_vertices, &vertex_array_numbers_5);
	update_vertex_buffer_numbers(number_6_vertices, &vertex_array_numbers_6);
	update_vertex_buffer_numbers(number_7_vertices, &vertex_array_numbers_7);
	update_vertex_buffer_numbers(number_8_vertices, &vertex_array_numbers_8);
	
	update_vertex_buffer_numbers(number_9_vertices, &vertex_array_numbers_9);
	update_vertex_buffer_numbers(number_10_vertices, &vertex_array_numbers_10);
	update_vertex_buffer_numbers(number_11_vertices, &vertex_array_numbers_11);
	update_vertex_buffer_numbers(number_12_vertices, &vertex_array_numbers_12);
	update_vertex_buffer_numbers(number_13_vertices, &vertex_array_numbers_13);
	update_vertex_buffer_numbers(number_14_vertices, &vertex_array_numbers_14);
	update_vertex_buffer_numbers(number_15_vertices, &vertex_array_numbers_15);
	update_vertex_buffer_numbers(number_16_vertices, &vertex_array_numbers_16);
	update_vertex_buffer_numbers(number_17_vertices, &vertex_array_numbers_17);
	update_vertex_buffer_numbers(number_18_vertices, &vertex_array_numbers_18);
	update_vertex_buffer_numbers(number_19_vertices, &vertex_array_numbers_19);
	update_vertex_buffer_numbers(number_20_vertices, &vertex_array_numbers_20);
	update_vertex_buffer_numbers(number_21_vertices, &vertex_array_numbers_21);
	update_vertex_buffer_numbers(number_22_vertices, &vertex_array_numbers_22);
	update_vertex_buffer_numbers(number_23_vertices, &vertex_array_numbers_23);
	update_vertex_buffer_numbers(number_24_vertices, &vertex_array_numbers_24);
	update_vertex_buffer_numbers(number_25_vertices, &vertex_array_numbers_25);
	update_vertex_buffer_numbers(number_26_vertices, &vertex_array_numbers_26);
	update_vertex_buffer_numbers(number_27_vertices, &vertex_array_numbers_27);
	update_vertex_buffer_numbers(number_28_vertices, &vertex_array_numbers_28);
	update_vertex_buffer_numbers(number_29_vertices, &vertex_array_numbers_29);
	update_vertex_buffer_numbers(number_30_vertices, &vertex_array_numbers_30);
	update_vertex_buffer_numbers(number_31_vertices, &vertex_array_numbers_31);
	update_vertex_buffer_numbers(number_32_vertices, &vertex_array_numbers_32);
	update_vertex_buffer_numbers(number_33_vertices, &vertex_array_numbers_33);
	update_vertex_buffer_numbers(number_34_vertices, &vertex_array_numbers_34);
	update_vertex_buffer_numbers(number_35_vertices, &vertex_array_numbers_35);
	update_vertex_buffer_numbers(number_36_vertices, &vertex_array_numbers_36);
	update_vertex_buffer_numbers(number_37_vertices, &vertex_array_numbers_37);
	update_vertex_buffer_numbers(number_38_vertices, &vertex_array_numbers_38);
	update_vertex_buffer_numbers(number_39_vertices, &vertex_array_numbers_39);
	update_vertex_buffer_numbers(number_40_vertices, &vertex_array_numbers_40);
	update_vertex_buffer_numbers(number_41_vertices, &vertex_array_numbers_41);
	update_vertex_buffer_numbers(number_42_vertices, &vertex_array_numbers_42);
	update_vertex_buffer_numbers(number_43_vertices, &vertex_array_numbers_43);
	update_vertex_buffer_numbers(number_44_vertices, &vertex_array_numbers_44);
	update_vertex_buffer_numbers(number_45_vertices, &vertex_array_numbers_45);
	update_vertex_buffer_numbers(number_46_vertices, &vertex_array_numbers_46);
	update_vertex_buffer_numbers(number_47_vertices, &vertex_array_numbers_47);
	update_vertex_buffer_numbers(number_48_vertices, &vertex_array_numbers_48);
	update_vertex_buffer_numbers(number_49_vertices, &vertex_array_numbers_49);
	update_vertex_buffer_numbers(number_50_vertices, &vertex_array_numbers_50);
	update_vertex_buffer_numbers(number_51_vertices, &vertex_array_numbers_51);
	update_vertex_buffer_numbers(number_52_vertices, &vertex_array_numbers_52);
	update_vertex_buffer_numbers(number_53_vertices, &vertex_array_numbers_53);
	update_vertex_buffer_numbers(number_54_vertices, &vertex_array_numbers_54);
	update_vertex_buffer_numbers(number_55_vertices, &vertex_array_numbers_55);
	update_vertex_buffer_numbers(number_56_vertices, &vertex_array_numbers_56);
	update_vertex_buffer_numbers(number_57_vertices, &vertex_array_numbers_57);
	update_vertex_buffer_numbers(number_58_vertices, &vertex_array_numbers_58);
	update_vertex_buffer_numbers(number_59_vertices, &vertex_array_numbers_59);
	update_vertex_buffer_numbers(number_60_vertices, &vertex_array_numbers_60);
	
	
}
