#pragma once
#ifndef __COLOSSEUM_H__
#define __COLOSSEUM_H__

#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility

struct colosseum_bottom //경기장 밑면
{
	float radius = 10.0f;	//경기장 반지름
	vec3 pos = (0, 0, -30.0f);	//경기장 위치
	GLuint	vertex_array_3 = 0;	//경기장 vertex_array


	mat4 scale_matrix_col =
	{
		radius,0,0,0,
		0,radius,0,0,
		0,0,radius,0,
		0,0,0,1
	};
	mat4 translate_matrix_col =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,pos.z,
		0,0,0,1
	};
	mat4 model_col = translate_matrix_col * scale_matrix_col; //경기장 모델 메트릭스 - 회전, 기울기 등 활동성이 없다고 가정하고 바로 초기화

	std::vector<vertex> create_colosseum_vertices();	//경기장 밑면 지오메트리
	void update_vertex_buffer_colosseum(const std::vector<vertex>& vertices);	//경기장 밑면 버퍼생성

};

struct colosseum_side		//경기장 옆면
{
	float radius = 10.0f;	//옆면 반지름
	vec3 pos = (0, 0, -30.0f);	//옆면 위치
	GLuint	vertex_array_4 = 0;	//옆면 vertex_array


	mat4 scale_matrix_col =
	{
		radius,0,0,0,
		0,radius,0,0,
		0,0,radius,0,
		0,0,0,1
	};
	mat4 translate_matrix_col =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,pos.z,
		0,0,0,1
	};
	mat4 model_col = translate_matrix_col * scale_matrix_col;	//옆면 모델 메트릭 정의

	std::vector<vertex> create_colosseum_side_vertices();
	void update_vertex_buffer_colosseum_side(const std::vector<vertex>& vertices);

};


//밑면 지오메트리 정의
std::vector<vertex> colosseum_bottom::create_colosseum_vertices()
{
	std::vector<vertex> v;

	for (int z = 0; z <= 1; z++)
	{
		v.push_back({ vec3(0,0,0.05f * -z), vec3(0,0,0.05f * -z), vec2(0,0) });

		for (int edge = 0; edge <= 1000; edge++)
		{
			float t = 2.0f * PI * edge / 1000, ct = cos(t), st = sin(t);
			v.push_back({ vec3(ct,st,0.05f * -z), vec3(ct,st,0.05f * -z), vec2(ct,st) });

		}

	}
	return v;


}

//밑면 버퍼 생성
void colosseum_bottom::update_vertex_buffer_colosseum(const std::vector<vertex>& vertices)
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

	for (int k = 0; k <= 1000; k++)
	{
		indices.push_back(0);
		indices.push_back(k);
		indices.push_back(k + 1);

	} // 경기장 상부

	for (int k = 0; k <= 1000; k++)
	{
		indices.push_back(k + 1);
		indices.push_back(k);
		indices.push_back(k + 1004);
		indices.push_back(k + 1);
		indices.push_back(k + 1004);
		indices.push_back(k + 1005);

	} //경기장 옆통

	for (int k = 0; k <= 1000; k++)
	{
		indices.push_back(k + 1003);
		indices.push_back(1003);
		indices.push_back(k + 1004);
	} //경기장 하부



	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);


	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_3) glDeleteVertexArrays(1, &vertex_array_3);
	vertex_array_3 = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_3) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

//옆면 지오메트리
std::vector<vertex> colosseum_side::create_colosseum_side_vertices()
{
	std::vector<vertex> v;

	for (int r = 0; r <= 1; r++)
	{
		for (int z = 0; z <= 1; z++)
		{
			for (int edge = 0; edge < 10; edge++)
			{
				float t = 2.0f * PI * edge / 10, ct = cos(t), st = sin(t);
				v.push_back({ vec3((1.0f - 0.1f * r) * ct,(1.0f - 0.1f * r) * st,0.2f * z), vec3(ct,st,0.2f * z), vec2(ct,st) });
			}
		}


	}

	return v;
}


//옆면 버퍼 생성
void colosseum_side::update_vertex_buffer_colosseum_side(const std::vector<vertex>& vertices)
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

	for (int i = 0; i < 10; i += 2)
	{
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + 10);

		indices.push_back(i + 10);
		indices.push_back(i + 1);
		indices.push_back(i + 11);
	}//바깥쪽면
	for (int i = 0; i < 10; i += 2)
	{
		indices.push_back(i + 21);
		indices.push_back(i + 20);
		indices.push_back(i + 30);

		indices.push_back(i + 21);
		indices.push_back(i + 30);
		indices.push_back(i + 31);
	}//안쪽면
	for (int i = 0; i < 10; i += 2)
	{
		indices.push_back(i + 20);
		indices.push_back(i);
		indices.push_back(i + 30);

		indices.push_back(i);
		indices.push_back(i + 10);
		indices.push_back(i + 30);
	}//좌측	
	for (int i = 0; i < 10; i += 2)
	{
		indices.push_back(i + 1);
		indices.push_back(i + 21);
		indices.push_back(i + 31);

		indices.push_back(i + 11);
		indices.push_back(i + 1);
		indices.push_back(i + 31);
	}//우측
	for (int i = 0; i < 10; i += 2)
	{
		indices.push_back(i + 10);
		indices.push_back(i + 11);
		indices.push_back(i + 30);

		indices.push_back(i + 30);
		indices.push_back(i + 11);
		indices.push_back(i + 31);
	}//상부





	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);


	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_4) glDeleteVertexArrays(1, &vertex_array_4);
	vertex_array_4 = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_4) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}


#endif