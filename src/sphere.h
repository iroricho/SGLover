#ifndef __SPHERE_H__
#define __SPHERE_H__
#include "camera.h"		// slee's OpenGL utility

//tank struct는 camera에 정의되어 있습니다.

// vertex buffer for tank 
GLuint	vertex_array_1 = 0;	// ID holder for vertex array object

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
std::vector<vertex> create_sphere_vertices( uint N )
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
	if(vertex_array_1) glDeleteVertexArrays(1,&vertex_array_1);
	vertex_array_1 = cg_create_vertex_array( vertex_buffer, index_buffer );
	if(!vertex_array_1){ printf("%s(): failed to create vertex aray\n",__func__); return; }
}


#endif
