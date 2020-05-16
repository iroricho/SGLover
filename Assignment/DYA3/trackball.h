#ifndef __TRACKBALL_H__
#define __TRACKBALL_H__
#include "cgmath.h"

struct trackball
{
	bool	b_tracking = false;
	int		button;
	int		mods;
	vec2	m0;				// the last mouse position
	vec3	taxis;

	bool is_tracking() const { return b_tracking; }
	void begin( vec2 m );
	void end() { b_tracking = false; }
	mat4 update_rot( vec2 m, vec3& eye, vec3& at, vec3& up );
	mat4 update_zm( vec2 m, vec3& eye, vec3& at, vec3& up );
	mat4 update_pn( vec2 m, vec3& eye, vec3& at, vec3& up );
};

inline void trackball::begin( vec2 m )
{
	b_tracking = true;			// enable trackball tracking
	m0 = m;						// save current mouse position
}

inline mat4 trackball::update_rot( vec2 m, vec3& eye, vec3& at, vec3& up )
{
	/*
	// project a 2D mouse position to a unit sphere
	static const vec3 p0 = vec3(0,0,1.0f);	// reference position on sphere
	vec3 p1 = vec3(m-m0,0);					// displacement
	if( !b_tracking || length(p1)<0.0001f ) return view_matrix;		// ignore subtle movement
	p1 = vec3(p1.x,p1.y,sqrtf(max(0,1.0f-length2(p1)))).normalize();

	vec3 v = mat3(view_matrix).transpose()*p0.cross(p1);
	float theta = asin( min(v.length(),1.0f) );

	return view_matrix*mat4::rotate(v.normalize(),theta);
	*/
	
	vec3 p1 = vec3(m-m0,0);					// displacement
	if( !b_tracking || length(p1) < 0.00001f ) return mat4::look_at(eye, at, up);

	vec3 n = (eye - at).normalize();
	vec3 u = (up.cross(n)).normalize();
	vec3 v = (n.cross(u)).normalize();

	float cons = 1.5f;
	/*
	float alpha = p1.x * cons;
	float beta = p1.y * cons;
	*/
	
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
	return mat4::look_at(eye, at, up);
}

/*
inline mat4 trackball::update_zm( vec2 m, vec3& eye, vec3& at, vec3& up )
{
	// project a 2D mouse position to a unit sphere
	float p1 = m.y-m0.y;					// displacement
	if( !b_tracking || abs(p1)<0.0001f ) return mat4::look_at(eye, at, up);
	p1 *= 0.4f;								// apply cons
	float p = max(1.0f + p1,0.0f);
	
	vec3 l = eye - at;
	l *= p;
	l += at;
	eye = l;
	//printf( "%f %f %f\n", eye.x, eye.y, eye.z);

	m0 = m;
	return mat4::look_at(eye, at, up);
}
*/

inline mat4 trackball::update_zm( vec2 m, vec3& eye, vec3& at, vec3& up )
{
	// project a 2D mouse position to a unit sphere
	float p1 = m.y-m0.y;					// displacement
	if( !b_tracking || abs(p1)<0.00001f ) return mat4::look_at(eye, at, up);
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
	return mat4::look_at(eye, at, up);
}

inline mat4 trackball::update_pn( vec2 m, vec3& eye, vec3& at, vec3& up )
{
	// project a 2D mouse position to a unit sphere
	vec3 p1 = vec3(m-m0,0);					// displacement
	if( !b_tracking || length(p1)<0.00001f ) return mat4::look_at(eye, at, up);

	p1 *= 1.65;							// apply cons

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
	
	//printf( "%f %f %f\n", eye.x, eye.y, eye.z);
	
	m0 = m;
	return mat4::look_at(eye, at, up);
}

#endif // __TRACKBALL_H__
