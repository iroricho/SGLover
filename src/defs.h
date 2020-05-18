//********* trackball.h ***********//

/*
// 이렇게 쓰면 되나요?
struct trackball
{
	bool is_W() const { return b_W; }
	bool is_A() const { return b_A; }
	bool is_S() const { return b_S; }
	bool is_D() const { return b_D; }
	bool is_LEFT() const { return b_LEFT; }
	bool is_RIGHT() const { return b_RIGHT; }

	void begin_W() { t0=t; b_W= true; }
	void begin_A() { t0=t; b_A = true; }
	void begin_S() { t0=t; b_S= true; }
	void begin_D() { t0=t; b_D = true; }
	void begin_LEFT() { t0=t; b_LEFT = true; }
	void begin_RIGHT() { t0=t; b_RIGHT= true; }

	void end_W() { b_W= false; }
	void end_A() { b_A = false; }
	void end_S() { b_S= false; }
	void end_D() { b_D = false; }
	void end_LEFT() { b_LEFT = false; }
	void end_RIGHT() { b_RIGHT= false; }

	mat4 update_W(vec3& eye, vec3& at, vec3& up);
	mat4 update_A(vec3& eye, vec3& at, vec3& up);
	mat4 update_S(vec3& eye, vec3& at, vec3& up);
	mat4 update_D(vec3& eye, vec3& at, vec3& up);
	mat4 update_LEFT(vec3& eye, vec3& at, vec3& up);
	mat4 update_RIGHT(vec3& eye, vec3& at, vec3& up);
};
함수1
함수2
함수3
.
.
.
.
*/


//********* 헤더파일 이름 2 ***********//

/*
함수1
함수2
함수3
.
.
.
.
*/


//********* 헤더파일 이름 3 ***********//

/*
함수1
함수2
함수3
.
.
.
.
*/

