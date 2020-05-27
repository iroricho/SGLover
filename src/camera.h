#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "cgmath.h"
#include "cgut.h"		// slee's OpenGL utility
#include "colosseum.h"
#include "cyl.h"
// camera ������, ȸ�� �׿� ���� ��ũ ��ü �̵��� ������ ����Դϴ�. tank ��ü�� �����Ǿ� �ֽ��ϴ�.

extern float t;	// ��ü �ð�, �� ���� ����� ���� buffer�� ���� ��

// tank ��ü ����
cyl tank = {0.3f, 0.3f, vec3(0), vec4(0.5f,0.5f,0.5f,1.0f), 30};

//*************************************
struct camera
{
	// �ٴ��� ���� ���� 1.0h ��ŭ �÷�����, tank �ٴ��� �״�� ���鿡 ����ְ� ��������
	vec3	eye = vec3( 0.5f, colosseum.pos.y + colosseum.height * 0.5f + 2.0f * tank.height, 0.0f );
	vec3	at = vec3( 0.0f, colosseum.pos.y + colosseum.height * 0.5f + 2.0f * tank.height, -20.0f );
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
	float speed=0.1f;			// velocity of move of camera ��ü �� ����
	float max_speed=0.1f;			// max velocity of move of camera ��ü �� ����

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

	//���� �Լ� call�� �ؼ� overhead�� �ø� �ʿ�� ��� ����
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

	//camera�� eye, at, up ���� �޾Ƽ� �ٲٰ� view matrix�� ��ȯ�ϴ� �Լ���
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
	// project a 2D mouse position to a unit tank
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
	// project a 2D mouse position to a unit tank
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

// WASD�� n������ �������� �ٲٴ� �Ͱ� ����
// �� eye�� at�� ���� �ٲ��ָ� ��
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

// n�� up�� ������ ȸ����Ű�� ����, (���� v�� ���� ����)
// eye�� �����ϰ� at�� �ٲٴ� �� Ʈ���� �ݴ��� ���� ��
inline void camera::update_LEFT()
{
	vec4 at4 = vec4(at, 1);
	float dt = t - tl;






	//eye�� �߽����� �̵�(translate), up���� ȸ��(rotate), �ٽ� eye ���� ��ġ�� �̵�
	vec4 atb = (mat4::translate(eye) * mat4::rotate(up, min(max_speed,dt * speed)) * (mat4::translate(-eye) * at4));
	
	at = vec3(atb.x, atb.y, atb.z);
}

inline void camera::update_RIGHT()
{
	vec4 at4 = vec4(at, 1);
	float dt = t - tr;

	//eye�� �߽����� �̵�(translate), up���� ȸ��(rotate), �ٽ� eye ���� ��ġ�� �̵�
	vec4 atb = (mat4::translate(eye) * mat4::rotate(-up, min(max_speed,dt * speed)) * (mat4::translate(-eye) * at4));

	at = vec3(atb.x, atb.y, atb.z);
}

//********** tank ������ ��Ʈ *************
inline void cyl::update_tank( float t, const vec3& eye, const vec3& at )
{
	vec3 n = (eye - at).normalize();
	// tank pos�� ��Ȯ�� ��ġ��Ű�� �ڱ���� �� ������ �� ������� �̴ϴ�.
	pos = eye - 7.0f * radius * n;
	pos.y = eye.y - 1.5f * height;

	// ī�޶� ȸ���� ���� ��ü�� ȸ����Ű�� ����. n,ref �Ѵ� ����������. y�� ���� ȸ��
	vec3 ref = vec3(0,0,1.0f);
	float theta = 0;
	if (n.x >= 0) theta = acos(ref.dot(n));
	else theta = -acos(ref.dot(n));
	float c = cos(theta), s = sin(theta);

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

#endif // __CAMERA_H__
