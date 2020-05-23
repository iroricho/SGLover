#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "cgmath.h"
#include "cgut.h"		// slee's OpenGL utility

extern float t;	// 전체 시간, 단 정지 기능을 위해 buffer가 빠진 값

//********** Temp var for adjust tank height *********
float CP = 0.0f;	// 콜로세움 윗면 위치입니다 (y축) 수정해주세요

// tank 모델 구현 헤더에 구조체를 정의하고 싶어도
// camera 초기 값에 tank의 반지름이 필요해
// 구조체 정의를 여기에 했습니다. 다른 방법 있으면 수정해주세요.
struct sphere_t
{
	float	radius=0.3f;	// radius
	vec3	pos;			// position of tank
	vec4	color;			// RGBA color in [0,1]
	mat4	model_matrix;	// modeling transformation

	// public functions
	void	update( float t, const vec3& eye, const vec3& at );	
};

// tank 구조체 생성
// 결국엔 pos를 eye에 맞게 업데이트 해야되기 때문에 radius 외에는 크게 상관 없습니다
sphere_t tank = {0.3f, vec3(0), vec4(1.0f,0.0f,0.0f,1.0f)};

//*************************************
struct camera
{
	vec3	eye = vec3( 0.0f, CP+tank.radius, 0.0f );
	vec3	at = vec3( 0.0f, CP+tank.radius, -20.0f );
	vec3	up = vec3( 0.0f, 1.0f, 0.0f );
	mat4	view_matrix = mat4::look_at( eye, at, up );;

	float	fovy = PI/4.0f; // must be in radian
	float	aspect;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;
	
	bool	b_tracking = false;
	int		button;
	int		mods;
	vec2	m0;				// the last mouse position

	bool is_tracking() const { return b_tracking; }
	
	void begin( vec2 m );
	void end() { b_tracking = false; }

	void update_rot( vec2 m );
	void update_zm( vec2 m );
	void update_pn( vec2 m );

	// time buffer
	float tw=0;
	float ta=0;
	float ts=0;
	float td=0;
	float tl=0;
	float tr=0;
	float speed=0.1f;			// velocity of move of camera 기체 별 상이
	float max_speed=0.1f;			// max velocity of move of camera 기체 별 상이

	// ************ WASD  ***************//
	bool b_W = false;
	bool b_A = false;
	bool b_S = false;
	bool b_D = false;
	bool b_LEFT = false;
	bool b_RIGHT = false;

	bool is_W() const { return b_W; }
	bool is_A() const { return b_A; }
	bool is_S() const { return b_S; }
	bool is_D() const { return b_D; }
	bool is_LEFT() const { return b_LEFT; }
	bool is_RIGHT() const { return b_RIGHT; }

	//굳이 함수 call을 해서 overhead를 늘릴 필요는 사실 없음
	void begin_W() { tw=t; b_W= true; }
	void begin_A() { ta=t; b_A = true; }
	void begin_S() { ts=t; b_S= true; }
	void begin_D() { td=t; b_D = true; }
	void begin_LEFT() { tl=t; b_LEFT = true; }
	void begin_RIGHT() { tr=t; b_RIGHT= true; }

	void end_W() { b_W= false; }
	void end_A() { b_A = false; }
	void end_S() { b_S= false; }
	void end_D() { b_D = false; }
	void end_LEFT() { b_LEFT = false; }
	void end_RIGHT() { b_RIGHT= false; }

	//camera의 eye, at, up 값을 받아서 바꾸고 view matrix를 반환하는 함수임
	void update_W();
	void update_A();
	void update_S();
	void update_D();
	void update_LEFT();
	void update_RIGHT();

};

camera	cam;

inline void camera::begin( vec2 m )
{
	b_tracking = true;			// enable camera tracking
	m0 = m;						// save current mouse position
}

inline void camera::update_rot( vec2 m )
{
	vec3 p1 = vec3(m-m0,0);					// displacement
	if( !b_tracking || length(p1) < 0.00001f ) return;

	vec3 n = (eye - at).normalize();
	vec3 u = (up.cross(n)).normalize();
	vec3 v = (n.cross(u)).normalize();

	float cons = 1.5f;
	
	vec3 p0 = vec3(0,0,1.0f);
	p1 = vec3(p1.x,p1.y,sqrtf(max(0,1.0f-length2(p1)))).normalize();
	float theta = acos(p1.dot(p0)/(length(p0)*length(p1)));
	theta *= cons;
	vec3 axis = p1.cross(p0);
	

	vec3 ex = vec3(1.0f,0,0);
	vec3 ey = vec3(0,1.0f,0);
	vec3 ez = vec3(0,0,1.0f);
	
	float j = eye.x*ex.dot(u) + eye.y*ey.dot(u) + eye.z*ez.dot(u);
	float k = eye.x*ex.dot(v) + eye.y*ey.dot(v) + eye.z*ez.dot(v);
	float l = eye.x*ex.dot(n) + eye.y*ey.dot(n) + eye.z*ez.dot(n);
	
	float p = up.x*ex.dot(u) + up.y*ey.dot(u) + up.z*ez.dot(u);
	float q = up.x*ex.dot(v) + up.y*ey.dot(v) + up.z*ez.dot(v);
	float r = up.x*ex.dot(n) + up.y*ey.dot(n) + up.z*ez.dot(n);
	
	//mat4 o = mat4::rotate(vec3(1.0f,0,0),beta) * mat4::rotate(vec3(0,1.0f,0),alpha);
	mat4 o = mat4::rotate(axis.normalize(),theta);
	vec4 oeye = o*vec4(j,k,l,0);
	vec4 oup = o*vec4(p,q,r,0);
	
	eye.x = oeye.x*u.dot(ex) + oeye.y*v.dot(ex) + oeye.z*n.dot(ex);
	eye.y = oeye.x*u.dot(ey) + oeye.y*v.dot(ey) + oeye.z*n.dot(ey);
	eye.z = oeye.x*u.dot(ez) + oeye.y*v.dot(ez) + oeye.z*n.dot(ez);
	
	up.x = oup.x*u.dot(ex) + oup.y*v.dot(ex) + oup.z*n.dot(ex);
	up.y = oup.x*u.dot(ey) + oup.y*v.dot(ey) + oup.z*n.dot(ey);
	up.z = oup.x*u.dot(ez) + oup.y*v.dot(ez) + oup.z*n.dot(ez);
	
	m0 = m;
}

inline void camera::update_zm( vec2 m )
{
	// project a 2D mouse position to a unit sphere
	float p1 = m.y-m0.y;					// displacement
	if( !b_tracking || abs(p1)<0.00001f ) return;
	p1 *= 0.6f;								// apply cons
	float p = max(1.0f + p1,0.0f);
	
	vec3 n = (eye - at).normalize();
	
	vec3 o = (p-1.0f)*length(eye-at)*n;

	float x = o.dot(vec3(1.0f,0,0));
	float y = o.dot(vec3(0,1.0f,0));
	float z = o.dot(vec3(0,0,1.0f));
	
	eye.x += x;
	eye.y += y;
	eye.z += z;

	m0 = m;
}

inline void camera::update_pn( vec2 m )
{
	// project a 2D mouse position to a unit sphere
	vec3 p1 = vec3(m-m0,0);					// displacement
	if( !b_tracking || length(p1)<0.00001f ) return;

	p1 = p1 * 1.65f;							// apply cons

	vec3 n = (eye - at).normalize();
	vec3 u = (up.cross(n)).normalize();
	vec3 v = (n.cross(u)).normalize();
	
	float x = p1.x*(u.dot(vec3(1.0f,0,0))) + p1.y*(v.dot(vec3(1.0f,0,0)));
	float y = p1.x*(u.dot(vec3(0,1.0f,0))) + p1.y*(v.dot(vec3(0,1.0f,0)));
	float z = p1.x*(u.dot(vec3(0,0,1.0f))) + p1.y*(v.dot(vec3(0,0,1.0f)));
	
	eye.x -= x;
	eye.y -= y;
	eye.z -= z;

	at.x -= x;
	at.y -= y;
	at.z -= z;
	
	m0 = m;
}

// WASD는 n벡터의 시작점을 바꾸는 것과 같음
// 즉 eye와 at을 같이 바꿔주면 됨
inline void camera::update_W()
{
	vec3 u = (at- eye).normalize();
	//vec3 v = (u.cross(up)).normalize();
	float dt = t - tw;

	eye += u * min(max_speed,dt * speed);
	at += u * min(max_speed,dt * speed);
}

inline void camera::update_A()
{
	vec3 u = (at- eye).normalize();
	vec3 v = (u.cross(up)).normalize();
	float dt = t - ta;

	eye -= v * min(max_speed,dt * speed);
	at -= v * min(max_speed,dt * speed);
}


inline void camera::update_S()
{
	vec3 u = (at- eye).normalize();
	//vec3 v = (u.cross(up)).normalize();
	float dt = t - ts;

	eye -= u * min(max_speed,dt * speed);
	at -= u * min(max_speed,dt * speed);
}


inline void camera::update_D()
{
	vec3 u = (at- eye).normalize();
	vec3 v = (u.cross(up)).normalize();
	float dt = t - td;

	eye += v * min(max_speed,dt * speed);
	at += v * min(max_speed,dt * speed);
}

// n를 up을 축으로 회전시키는 것임, (축이 v일 수도 있음)
// eye를 고정하고 at을 바꾸는 것 트랙볼 반대라고 보면 됨
inline void camera::update_LEFT()
{
	vec4 at4 = vec4(at, 1);
	float dt = t - tl;

	//eye를 중심으로 이동(translate), up기준 회전(rotate), 다시 eye 원래 위치로 이동
	vec4 atb = (mat4::translate(eye) * mat4::rotate(up, min(max_speed,dt * speed)) * (mat4::translate(-eye) * at4));

	at = vec3(atb.x, atb.y, atb.z);
}

inline void camera::update_RIGHT()
{
	vec4 at4 = vec4(at, 1);
	float dt = t - tr;

	//eye를 중심으로 이동(translate), up기준 회전(rotate), 다시 eye 원래 위치로 이동
	vec4 atb = (mat4::translate(eye) * mat4::rotate(-up, min(max_speed,dt * speed)) * (mat4::translate(-eye) * at4));

	at = vec3(atb.x, atb.y, atb.z);
}

#endif // __CAMERA_H__
