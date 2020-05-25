#ifndef __COLOSSEUM_H__
#define __COLOSSEUM_H__
#include "cyl.h"
// 경기장 객체를 생성하고 설정하는 헤더입니다.
// user_init에서 update를 한번 실행하고 끝납니다.
inline void cyl::update_colosseum()
{
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, height, 0, 0,
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

cyl colosseum = {20.0f, 20.0f, vec3(0,-10.5f,0), vec4(0.5f,0.5f,0.5f,1.0f), 100};

#endif
