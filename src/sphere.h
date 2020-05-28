#ifndef __SPHERE_H__
#define __SPHERE_H__
#include "camera.h"		// slee's OpenGL utility

//tank struct는 camera에 정의되어 있습니다.

// vertex buffer for tank 
GLuint	vertex_array_1 = 0;	// ID holder for vertex array object bullet
GLuint	vertex_array_2 = 0;	// ID holder for vertex array object sky

struct sphere_t
{
	vec2	center = vec2(0);		// 2D position for translation
	float	radius = 1.0f;		// radius
	float	theta = 0.0f;			// rotation angle
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation
	float save_t1 = 0, save_t2 = 0;

	// public functions
	void	update(float t);
};







//********** 모델링 파트 *************
std::vector<vertex> create_sphere_vertices(uint N)
{										//     position3   norm3         tex2
	std::vector<vertex> v = { { vec3(2,2,2), vec3(0,0,-1.0f), vec2(0.625f,0.75f) } }; // origin


	for (uint j = 0; j <= N; j++) {
		for (uint k = 0; k <= N; k++) {

			float theta = (PI * k) / float(N), phi = ((2 * PI * j) / float(N));
			float s_theta = sin(theta), c_theta = cos(theta), s_phi = sin(phi), c_phi = cos(phi);
			float x = s_theta * c_phi;
			float  y = s_theta * s_phi;
			float  z = c_theta;
			v.push_back({ vec3(y,z,x), vec3(y,z,x), vec2(0.5f + 0.25f * (phi / (2 * PI)) , 0.75f + 0.25f * (1 - (theta / PI))) });



		}
	}
	return v;
}

void update_vertex_buffer_sphere(const std::vector<vertex>& vertices, uint N)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer
	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	if (1)
	{
		std::vector<uint> indices;
		for (uint j = 0; j < N; j++)
		{
			for (uint k = 0; k < N; k++) {
				indices.push_back((N + 1) * j + k + 1);
				indices.push_back((N + 1) * j + k + (N + 1) + 1);
				indices.push_back((N + 1) * j + k + (N + 1) + 2); //뭐지?  +N을 해서 한칸이 밀린건가..? 

				indices.push_back((N + 1) * j + k + 1);
				indices.push_back((N + 1) * j + k + (N + 1) + 2);
				indices.push_back((N + 1) * j + k + 2);
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
	}
	else
	{
		std::vector<vertex> v; // triangle vertices
		for (uint j = 0; j < N; j++)
		{
			for (uint k = 0; k < N; k++)
			{
				v.push_back(vertices[(N + 1) * j + k + 1]);
				v.push_back(vertices[(N + 1) * j + k + (N + 1) + 1]);
				v.push_back(vertices[(N + 1) * j + k + (N + 1) + 2]);

				v.push_back(vertices[(N + 1) * j + k + 1]);
				v.push_back(vertices[(N + 1) * j + k + (N + 1) + 2]);
				v.push_back(vertices[(N + 1) * j + k + 2]);
			}
		}

		// generation of vertex buffer: use triangle_vertices instead of vertices
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * v.size(), &v[0], GL_STATIC_DRAW);
	}

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_1) glDeleteVertexArrays(1, &vertex_array_1);
	vertex_array_1 = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_1) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}


//**********하늘 모델링 파트 *************
std::vector<vertex> create_sky_vertices(uint N)
{										//     position3   norm3         tex2
	std::vector<vertex> v = { { vec3(2,2,2), vec3(0,0,-1.0f), vec2(0.575f,0.875f) } }; // origin


	for (uint j = 0; j <= N; j++) {
		for (uint k = 0; k <= N; k++) {

			float theta = (PI * k) / float(N), phi = ((2 * PI * j) / float(N));
			float s_theta = sin(theta), c_theta = cos(theta), s_phi = sin(phi), c_phi = cos(phi);
			float x = s_theta * c_phi;
			float  y = s_theta * s_phi;
			float  z = c_theta;
			//v.push_back({ vec3(y,z,x), vec3(y,z,x), vec2(0.26f + 0.23f*0.5f*(j%2) , 0.76f + 0.2f *0.5f*(k%2) ) });
			v.push_back({ vec3(y,z,x), vec3(y,z,x), vec2((phi / (2 * PI)) , (1 - (theta / PI)) )});	//텍스쳐좌표 변경



		}
	}
	return v;
}

void update_vertex_buffer_sky(const std::vector<vertex>& vertices, uint N)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer
	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	if (1)
	{
		std::vector<uint> indices;
		for (uint j = 0; j < N; j++)
		{
			for (uint k = 0; k < N; k++) {
				indices.push_back((N + 1) * j + k + 1);
				indices.push_back((N + 1) * j + k + (N + 1) + 1);
				indices.push_back((N + 1) * j + k + (N + 1) + 2); //뭐지?  +N을 해서 한칸이 밀린건가..? 

				indices.push_back((N + 1) * j + k + 1);
				indices.push_back((N + 1) * j + k + (N + 1) + 2);
				indices.push_back((N + 1) * j + k + 2);
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
	}
	else
	{
		std::vector<vertex> v; // triangle vertices
		for (uint j = 0; j < N; j++)
		{
			for (uint k = 0; k < N; k++)
			{
				v.push_back(vertices[(N + 1) * j + k + (N + 1) + 1]);
				v.push_back(vertices[(N + 1) * j + k + 1]);
				v.push_back(vertices[(N + 1) * j + k + (N + 1) + 2]);

				v.push_back(vertices[(N + 1) * j + k + (N + 1) + 2]);
				v.push_back(vertices[(N + 1) * j + k + 1]);
				v.push_back(vertices[(N + 1) * j + k + 2]);
			}
		}

		// generation of vertex buffer: use triangle_vertices instead of vertices
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * v.size(), &v[0], GL_STATIC_DRAW);
	}

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_2) glDeleteVertexArrays(1, &vertex_array_2);
	vertex_array_2 = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_2) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}


#endif
