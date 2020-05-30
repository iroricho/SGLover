#pragma warning(disable: 4819)

#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "cyl.h"		// cylinder 모델링 헤더
#include "sphere.h"
#include "camera.h"		// camera, 즉 player 헤더
#include "colosseum.h"	// 경기장 헤더
#include "bullet.h"		// bullet 헤더
#include "sky.h"
#include "ai.h"			// ai 헤더
#include "maintheme.h"		//메인화면 헤더
#include "numbers.h"		//숫자 카운터 헤더
#include "aicount.h"		//ai 카운터 헤더


#include "./irrKlang/irrKlang.h"
#pragma comment(lib, "irrKlang.lib")



//소리용 헤더 파일 추가
/*
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#define 효과음추가(sound) PlaySound(TEXT(#sound), 0, SND_FILENAME | SND_ASYNC)
#define 브금추가(sound) PlaySound(TEXT(#sound), 0, SND_FILENAME | SND_ASYNC | SND_NOSTOP)
*/


//*************************************
// global constants
static const char*	window_name = "SGLover";
static const char*	vert_shader_path = "../bin/shaders/circ.vert";
static const char*	frag_shader_path = "../bin/shaders/circ.frag";
static const char* lena_image_path = "../bin/images/lena3.jpg";
static const char* baboon_image_path = "../bin/images/baboon.jpg";
static const char* main_theme_path = "../bin/images/maintheme.jpg";
static const char* button_start_path = "../bin/images/buttonstart.jpg";
static const char* button_help_path = "../bin/images/buttonhelp.jpg";
static const char* help_page_path = "../bin/images/helppage.jpg";
static const char* skymap_path = "../bin/images/skymap.jpg";
static const char* numbers_path = "../bin/images/numbers.jpg";
static const char* head_path = "../bin/images/head.jpg";
static const char* bullets_path = "../bin/images/bullet.jpg";
static const char* hh_head_path = "../bin/images/hh_head.jpg";
static const char* mg_head_path = "../bin/images/mg_head.jpg";
static const char* dy_head_path = "../bin/images/dy_head.jpg";
static const char* pause_page_path = "../bin/images/pausepage.jpg";
static const char* clear_page_path = "../bin/images/clearpage.jpg";
static const char* fail_page_path = "../bin/images/failpage.jpg";
static const char* ground_path = "../bin/images/ground.jpg";
static const char* student_path = "../bin/images/student.jpg";
static const char* student_arm_path = "../bin/images/studentarm.jpg";
static const char* prof_path = "../bin/images/prof.jpg";
static const char* prof_arm_path = "../bin/images/profarm.jpg";
static const char* aicount_path = "../bin/images/aicount.jpg";



//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program = 0;		// ID holder for GPU program

GLuint  vertex_array = 0;			// ID holder for vertex array object
GLint	LENA = 0;					// texture object
GLint	BABOON = 0;					// texture object
GLint		main_theme = 0;		//main_theme texture object
GLint		button_start = 0;	//button texture object
GLint		button_help = 0;	//button texture object
GLint		help_page = 0;		//help page texture object
GLint		skymap = 0;		//skymap texture object
GLint		numbers = 0;
GLint		head = 0;
GLint		bullets = 0;
GLint		hh_head = 0;
GLint		mg_head = 0;
GLint		dy_head = 0;
GLint		pause_page = 0;
GLint		clear_page = 0;
GLint		fail_page = 0;
GLint		ground = 0;
GLint		student = 0;
GLint		student_arm = 0;
GLint		prof = 0;
GLint		profarm = 0;
GLint		aicount = 0;



//*************************************
// global variables
float	t = 0.0f;						// t는 전체 파일에서 동일하게 쓰이길 요망
float	time_buffer = 0;				// time buffer for resume
float	tb = 0;
bool	halt = 0;
uint	mode = 0;			// texture display mode: 0=texcoord, 1=lena, 2=baboon
int		screan_mode = 0;		//타이틀, 게임화면, 앤딩화면 전환용
int		game_counter = 0;	//제한시간
int		camer_toggle = 0;
bool	init_flag = 0;
float		spacebar_timer = 0;	//연사 조절
bool	introbgm_flag = 0;
bool	gamebgm_flag = 0;
bool gameending_flag = 0;


irrklang::ISoundEngine* engine = nullptr;
irrklang::ISoundSource* sound_src_1 = nullptr;
irrklang::ISoundSource* sound_src_2 = nullptr;
irrklang::ISoundSource* sound_src_3 = nullptr;
irrklang::ISoundSource* sound_src_gamebgm = nullptr;
irrklang::ISoundSource* sound_src_introbgm = nullptr;
irrklang::ISoundSource* sound_src_ouch = nullptr;
irrklang::ISoundSource* sound_src_laser = nullptr;
irrklang::ISoundSource* sound_src_ohno = nullptr;
irrklang::ISoundSource* sound_src_gamewin = nullptr;
irrklang::ISoundSource* sound_src_gamelose = nullptr;
irrklang::ISoundSource* sound_src_beep = nullptr;


//*************************************
// holder of vertices and indices of a unit tank
std::vector<vertex>	unit_tank_vertices;			// tank vertices
std::vector<vertex>	unit_colsseum_vertices;		// colosseum vertices
std::vector<vertex>	unit_ai_vertices;			// ai vertices
std::vector<vertex>	unit_bullet_vertices;		// bullet vertices
std::vector<vertex>	unit_sky_vertices;		// skyvertices
std::vector<vertex>	main_theme_vertices;	//메인화면vertices
std::vector<vertex>	button_vertices;				//버튼vertices
std::vector<vertex>	unit_ai_arm_vertices;				//버튼vertices

/*
std::vector<vertex>	colosseum_bottom_vertices;	//경기장 하부 vertices
std::vector<vertex>	colosseum_side_vertices;	//경기장 옆면 vertices

// objects
//colosseum_bottom bottom;		//경기장 하부 객체 생성, initial 값을 그대로 따라감
//colosseum_side side;			//경기장 옆면 객체 생성
*/

//*************************************
void update_camera() {
	cam.update_Camera();
}

void Pause() {
	if (halt == 0) {
		printf("Pause\n");
		halt = 1;
		time_buffer = t;
	}
}

void Resume() {
	if (halt == 1) {
		printf("Resume\n");
		halt = 0;
		tb = time_buffer;
		glfwSetTime(time_buffer);
	}
}

void update()
{
	// **** update camera
	update_camera();

	// update projection matrix
	cam.aspect = window_size.x / float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect, cam.dnear, cam.dfar);

	// camera의 eye, at, up 은 cameara 헤더에 정의된 함수들이 바꿔줍니다.
	// 때문에 update 때만 view_matrix를 구해주면 됩니다.
	if (camer_toggle == 0)	cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
	else if (camer_toggle == 1) {
		vec3 towards = cam.at - cam.eye;
		vec3 up = cam.up;

		vec3 new_eye = cam.eye +  2*up - 0.5f* towards;
		vec3 new_at = cam.eye - (towards + vec3(0, -0.5f, 0)).normalize();

		cam.view_matrix = mat4::look_at(new_eye, new_at, cam.up);
	}
	GLint uloc;
	uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);

	// **** update tank
	tank.update_tank(t, cam.eye, cam.at);
	tank.update_tank_head(t);
	tank.update_tank_arm();
	num_cnt.update_counter(t, cam.eye, cam.at);

	// **** update ai
	for (int i = 0; i < anum; i++)
	{
		ai_t& ai = ais[i];

		// AI move update 탱크 위치를 받기 때문에 tank update 보다 밑에 있어야 함
		//********* 충돌 검사 **********//
		for (int i = 0; i < bullet_num; i++) {
			if (ai.collision_bullet(bullet_list[i].pos, bullet_list[i].radius, bullet_list[i].mass)) bullet_list[i].disappear();
		}
		ai.collision(tank.pos, tank.radius, tank.mass);

		for (int j = 0; j < anum; j++)
		{
			if (j != i)
			{
				vec3 aipos0 = ai.pos;
				float airadius0 = ai.radius;
				float aimass0 = ai.mass;
				ai.collision(ais[j].pos, ais[j].radius, ais[j].mass);
				ais[j].collision(aipos0, airadius0, aimass0);
			}

			//사망 카운트
		}

		ai.update(t, tank.pos);
		ai.update_head(t);
		ai.update_arm();
	}

	// Bullet move update
	for (int i = 0; i < bullet_num; i++) {
		bullet_list[i].update(t, tank.pos);
	}
	//Sky move update
	sky.update(t, tank.pos);

	


	if (num_death_ai == anum)	screan_mode = 4;	//승리조건
	if (game_counter == 61)	screan_mode = 5;	//패배조건

}

void render()
{
	// clear screan (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	


	// bind vertex array object of cylinder
	//glBindVertexArray( vertex_array_0 ); //여기.

	GLint uloc;


	

	if (screan_mode == 1)
	{
		Resume();

		glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 업데이트

		//sound change
		if (introbgm_flag == 1)
		{
			engine->stopAllSoundsOfSoundSource(sound_src_introbgm);
			introbgm_flag = 0;
		}
		if (gamebgm_flag == 0)
		{
			engine->play2D(sound_src_gamebgm, false);
			gamebgm_flag = 1;
			
		}

		// bind textures
		
		glUniform1i(glGetUniformLocation(program, "TEX0"), 0);

		
		glUniform1i(glGetUniformLocation(program, "TEX1"), 1);

		// update tank uniforms and draw calls
		glBindVertexArray(vertex_array_tank); //여기.
		glUniform1i(glGetUniformLocation(program, "TEX0"), 17);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, tank.model_matrix);
		glDrawElements(GL_TRIANGLES, 4 * tank.NTESS * tank.NTESS * 3, GL_UNSIGNED_INT, nullptr);	//몸통
		glBindVertexArray(vertex_array_2);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 6);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, tank.model_matrix_head);
		glDrawElements(GL_TRIANGLES, 4 * tank.NTESS * tank.NTESS * 3, GL_UNSIGNED_INT, nullptr);	//머리
		glBindVertexArray(vertex_array_arm);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 18);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, tank.model_matrix_arm);
		glDrawElements(GL_TRIANGLES, 4 * tank.NTESS * tank.NTESS * 3, GL_UNSIGNED_INT, nullptr);	//팔




		
		// update colosseum uniforms and draw calls
		glBindVertexArray(vertex_array_colosseum);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 12);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, colosseum.model_matrix);
		glDrawElements(GL_TRIANGLES, 4 * colosseum.NTESS * colosseum.NTESS * 3, GL_UNSIGNED_INT, nullptr);

		for (int i = 0; i < anum; i++)
		{
			ai_t& ai = ais[i];

			//AI 그리기
			glBindVertexArray(vertex_array_AI);
			glUniform1i(glGetUniformLocation(program, "TEX0"), 13);
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, ai.model_matrix);
			glDrawElements(GL_TRIANGLES, 4 * ai.NTESS * ai.NTESS * 3, GL_UNSIGNED_INT, nullptr);	//몸통
			glBindVertexArray(vertex_array_2);
			if(i%3==0)		glUniform1i(glGetUniformLocation(program, "TEX0"), 8);
			else if(i%3==1)	glUniform1i(glGetUniformLocation(program, "TEX0"), 14);
			else					glUniform1i(glGetUniformLocation(program, "TEX0"), 15);
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, ai.model_matrix_head);
			glDrawElements(GL_TRIANGLES, 4 * bullet_list[i].NTESS * bullet_list[i].NTESS * 3, GL_UNSIGNED_INT, nullptr);	//머리
			glBindVertexArray(vertex_array_arm);
			glUniform1i(glGetUniformLocation(program, "TEX0"), 16);
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, ai.model_matrix_arm);
			glDrawElements(GL_TRIANGLES, 4 * ai.NTESS * ai.NTESS * 3, GL_UNSIGNED_INT, nullptr);	//팔

		}


		//Bullet 그리기
		for (int i = 0; i < bullet_num; i++) {
			glBindVertexArray(vertex_array_2);
			glUniform1i(glGetUniformLocation(program, "TEX0"), 7);
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, bullet_list[i].model_matrix);
			glDrawElements(GL_TRIANGLES, 4 * bullet_list[i].NTESS * bullet_list[i].NTESS * 3, GL_UNSIGNED_INT, nullptr);
		}
		//Sky 그리기
		glBindVertexArray(vertex_array_2);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 4);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, sky.model_matrix);
		glDrawElements(GL_TRIANGLES, 4 * tank.NTESS * tank.NTESS * 3, GL_UNSIGNED_INT, nullptr);

		//숫자 카운터 그리기
	
		game_counter = (int)t;
		
		glBindVertexArray(vertex_array_numbers_60+1-game_counter);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 5);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, num_cnt.model_matrix);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


		/*
		//경기장 하부 그리기
		glBindVertexArray(vertex_array_3);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, bottom.model_col);
		glDrawElements(GL_TRIANGLES, 20*bottom.NTESS, GL_UNSIGNED_INT, nullptr);

		//경기장 옆면 그리기
		glBindVertexArray(side.vertex_array_4);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, side.model_col);
		glDrawElements(GL_TRIANGLES, 10000, GL_UNSIGNED_INT, nullptr);
		*/

		// swap front and back buffers, and display to screan

		init_flag = 0;

	}

	else if (screan_mode == 0)		//초기화면
	{
		Pause();
		glfwSetTime(0);
		time_buffer = t;

		// **** 초기화부분
		if (init_flag == 0) {
			printf("initialized\n");
			num_death_ai = 0;		//데스 초기화
			game_counter = 0;		//시간 카운터 초기화
			spacebar_timer = 0;

			for (int i = 0; i < bullet_num; i++) {
				bullet_list[i].disappear();
			}	//총알 초기화

			for (int i=ais.size(); i>0; i--) {
				ais.pop_back();
			}
			ais = std::move(create_ais(anum));
			
		}
		//sound change
		if (introbgm_flag == 0)
		{
			engine->play2D(sound_src_introbgm, true);
			introbgm_flag = 1;

		}
		if (gamebgm_flag == 1)
		{
			engine->stopAllSoundsOfSoundSource(sound_src_gamebgm);
			gamebgm_flag = 0;
		}
		gameending_flag = 0;

		glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 update 


		//메인 바탕화면 그리기
		glBindVertexArray(vertex_array_maintheme);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, theme.model_matrix);
		

		
		glUniform1i(glGetUniformLocation(program, "TEX0"), 2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


		//난이도 조절 그리기
		glBindVertexArray(vertex_array_aicount_1+anum-1);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, ai_count.model_matrix);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 19);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/*
		//설명서 버튼 그리기
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, bt_help.model_matrix);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, button_help);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 4);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		*/
		
		init_flag = 1;

	}

	else if (screan_mode == 2)		//설명서
	{
		Pause();
		glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 update 


		//설명서 그리기
		glBindVertexArray(vertex_array_maintheme);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, theme.model_matrix);



		glUniform1i(glGetUniformLocation(program, "TEX0"), 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		
		init_flag = 0;
	}

	else if (screan_mode == 3)		//일시정지
	{
		Pause();
		glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 update 

		

		//일시정지 화면 그리기
		glBindVertexArray(vertex_array_maintheme);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, theme.model_matrix);



		glUniform1i(glGetUniformLocation(program, "TEX0"), 9);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		
		init_flag = 0;
	}

	else if (screan_mode == 4)		//클리어
	{
		Pause();
		glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 update 


		if (gamebgm_flag == 1)
		{
			engine->stopAllSoundsOfSoundSource(sound_src_gamebgm);
			gamebgm_flag = 0;
		}
		if (gameending_flag == 0)
		{
			engine->play2D(sound_src_gamewin, false);
			gameending_flag = 1;
		}
		
		//클리어 화면 그리기
		glBindVertexArray(vertex_array_maintheme);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, theme.model_matrix);



		glUniform1i(glGetUniformLocation(program, "TEX0"), 10);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		
		init_flag = 0;
	}

	else if (screan_mode == 5)		//실패
	{
		Pause();
		glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 update 


		if (gamebgm_flag == 1)
		{
			engine->stopAllSoundsOfSoundSource(sound_src_gamebgm);
			gamebgm_flag = 0;
		}
		if (gameending_flag == 0)
		{
			engine->play2D(sound_src_gamelose, false);
			gameending_flag = 1;
		}
		
		//일시정지 화면 그리기
		glBindVertexArray(vertex_array_maintheme);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, theme.model_matrix);



		glUniform1i(glGetUniformLocation(program, "TEX0"), 11);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		init_flag = 0;
	}

	
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		// 동시키 문제는 자동으로 해결이 돼버림, 왜 되지?
		// 하지만 8방향일 뿐 자연스럽지는 못 함
		// 이동과 카메라 회전을 동시에 하면 문제가 생김
		
		else if (key == GLFW_KEY_W) {
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera('W', flags);
		}
		else if (key == GLFW_KEY_A) {
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera('A', flags);
		}
		else if (key == GLFW_KEY_S) {
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera('S', flags);
		}
		else if (key == GLFW_KEY_D) {
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera('D', flags);
		}
		else if (key == GLFW_KEY_LEFT) {
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera('L', flags);
			if (screan_mode == 0)
			{
				if (anum > 1)	anum--;
				init_flag = 0;
				engine->play2D(sound_src_beep, false);

			}
		}
		else if (key == GLFW_KEY_RIGHT) {
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera('R', flags);
			if (screan_mode == 0)
			{
				if (anum < 16)	anum++;
				init_flag = 0;
				engine->play2D(sound_src_beep, false);
			}
		}
		else if (key == GLFW_KEY_C) {
			camer_toggle = (camer_toggle + 1) % 2;
		}


		else if (key == GLFW_KEY_SPACE) {
			if (screan_mode == 1)
			{
				if (spacebar_timer < t)
				{
					engine->play2D(sound_src_laser, false);
					bullets = (bullets + 1) % bullet_num;
					bullet_list[bullets].launch(t, tank.pos, (cam.at - cam.eye).normalize());
					spacebar_timer = t + 0.5f;

				}
			}
			
		}

		//그냥 텍스쳐매핑할떄 테스트용. 나중에 지워도 됨.
		else if (key == GLFW_KEY_T)
		{
			mode = (mode + 1) % 3;
			printf("using mode %d: %s (press 'd' to toggle)\n", mode,
				mode == 0 ? "texcoord" : mode == 1 ? "lena" : "baboon");
		}

		else if (key == GLFW_KEY_G)
		{
			if (screan_mode == 0 || screan_mode == 2 || screan_mode == 3) { engine->play2D(sound_src_beep, false); screan_mode = 1; }
		}
		else if (key == GLFW_KEY_F2)
		{
			if (screan_mode == 0) { engine->play2D(sound_src_beep, false); screan_mode = 2; }
			
			
		}
		else if (key == GLFW_KEY_I)
		{
			if (screan_mode != 1) { engine->play2D(sound_src_beep, false); screan_mode = 0; }

		}
		else if (key == GLFW_KEY_P)
		{
			if (screan_mode == 1) { engine->play2D(sound_src_beep, false);  screan_mode = 3; }
			else if (screan_mode == 3) { engine->play2D(sound_src_beep, false);  screan_mode = 1; }
		}
	



	}
	else if(action==GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W) {
			cam.b_W = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(0, flags);
		}
		else if (key == GLFW_KEY_A) {
			cam.b_A = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(0, flags);
		}
		else if (key == GLFW_KEY_S) {
			cam.b_S = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(0, flags);
		}
		else if (key == GLFW_KEY_D) {
			cam.b_D = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(0, flags);
		}
		else if (key == GLFW_KEY_LEFT) {
			cam.b_LEFT = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(0, flags);
		}
		else if (key == GLFW_KEY_RIGHT) {
			cam.b_RIGHT = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(0, flags);
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
}

void motion( GLFWwindow* window, double x, double y )
{
}

GLuint create_texture(const char* image_path, bool b_mipmap)
{
	// load the image with vertical flipping
	image* img = cg_load_image(image_path); if (!img) return -1;
	int w = img->width, h = img->height;

	// create a src texture (lena texture)
	GLuint texture; glGenTextures(1, &texture);//여기!
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->ptr);
	if (img) delete img; // release image

	// build mipmap
	if (b_mipmap && glGenerateMipmap)
	{
		int mip_levels = 0; for (int k = max(w, h); k; k >>= 1) mip_levels++;
		for (int l = 1; l < mip_levels; l++)
			glTexImage2D(GL_TEXTURE_2D, l, GL_RGB8, max(1, w >> l), max(1, h >> l), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// set up texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, b_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	
	return texture;
}

bool user_init()
{
	// set default display mode to lena
	keyboard(window, GLFW_KEY_T, 0, GLFW_PRESS, 0);


	//sound source
	engine = irrklang::createIrrKlangDevice();
	if (!engine)	 return false;
	sound_src_1 = engine->addSoundSourceFromFile("gomonster.wav");
	sound_src_2 = engine->addSoundSourceFromFile("grandpas11month.wav");
	sound_src_3 = engine->addSoundSourceFromFile("hit.wav");
	sound_src_gamebgm = engine->addSoundSourceFromFile("../bin/sound/gamebgm.wav");
	sound_src_introbgm = engine->addSoundSourceFromFile("../bin/sound/introbgm.wav");
	sound_src_ouch = engine->addSoundSourceFromFile("../bin/sound/ouch.wav");
	sound_src_ohno = engine->addSoundSourceFromFile("../bin/sound/ohno.wav");
	sound_src_laser = engine->addSoundSourceFromFile("../bin/sound/laser.wav");
	sound_src_gamewin = engine->addSoundSourceFromFile("../bin/sound/gamewin.wav");
	sound_src_gamelose = engine->addSoundSourceFromFile("../bin/sound/gamelose.wav");
	sound_src_beep = engine->addSoundSourceFromFile("../bin/sound/beep.wav");
	//engine->play2D(sound_src_2, true);

	// log hotkeys
	print_help();

	//bullet 생성이 잘 안되서 그냥 여기다 만들어요
	for (int i = 0; i < bullet_num; i++) {
		Bullet bullet = { 0.0f,  vec3(-10,-10,-10), vec4(1.0f,0.0f,0.0f,1.0f), 30 };
		bullet_list.emplace_back(bullet);
	}


	// init GL states
	glLineWidth( 1.0f );
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable(GL_TEXTURE_2D);			// enable texturing
	glActiveTexture(GL_TEXTURE0);		// notify GL the current texture slot is 0
	
	colosseum.update_colosseum();

	// define the position of four corner vertices
	unit_tank_vertices = std::move( create_cyltop_vertices_tank( tank.NTESS ));
	unit_colsseum_vertices = std::move( create_cyltop_vertices_colosseum( colosseum.NTESS ));
	unit_ai_vertices = std::move( create_cyltop_vertices_AI( ais[0].NTESS ));
	unit_bullet_vertices = std::move(create_sphere_vertices(tank.NTESS));
	unit_sky_vertices = std::move( create_sky_vertices( sky.NTESS ));
	main_theme_vertices = std::move(create_vertices_maintheme());
	button_vertices = std::move(create_vertices_button());
	unit_ai_arm_vertices = std::move(create_cyltop_vertices_AI_arm(ais[0].NTESS));
	

	/*
	colosseum_bottom_vertices = std::move(bottom.create_colosseum_vertices());		//경기장하부vertice생성
	//colosseum_side_vertices = std::move(side.create_colosseum_side_vertices());		//경기장옆면vertice생성
	*/

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer_cyltop_colosseum( unit_colsseum_vertices, colosseum.NTESS );
	update_vertex_buffer_cyltop_AI( unit_ai_vertices, ais[0].NTESS);		// AI 버퍼 생성
	update_vertex_buffer_cyltop_tank( unit_tank_vertices, tank.NTESS );
	update_vertex_buffer_sphere(unit_bullet_vertices, tank.NTESS);		// bullet 버퍼 생성
	update_vertex_buffer_sky( unit_sky_vertices, sky.NTESS);		// bullet 버퍼 생성
	update_vertex_buffer_maintheme(main_theme_vertices);		//메인화면 버퍼 생성
	update_vertex_buffer_button(button_vertices);					//버튼 버퍼 생성
	update_number_vertexbuffers();		//숫자 버퍼 생성
	update_vertex_buffer_cyltop_AI_arm(unit_ai_arm_vertices, ais[0].NTESS);
	update_aicount_vertexbuffers();		//ai 카운터 버퍼 생성
	
	/*
	bottom.update_vertex_buffer_colosseum(colosseum_bottom_vertices);	//경기장하부 버퍼 생성
	//side.update_vertex_buffer_colosseum_side(colosseum_side_vertices);	//경기장옆면 버퍼 생성
	*/

	// load the Lena image to a texture
	LENA = create_texture(lena_image_path, true);		if (LENA == -1) return false;
	BABOON = create_texture(baboon_image_path, true);	if (BABOON == -1) return false;
	main_theme = create_texture(main_theme_path, true);	if (main_theme == -1) return false;
	button_start = create_texture(button_start_path, true);	if (button_start == -1) return false;
	button_help = create_texture(button_help_path, true);	if (button_help == -1) return false;
	help_page = create_texture(help_page_path, true);	if (help_page == -1) return false;
	skymap = create_texture(skymap_path, true);	if (skymap == -1) return false;
	numbers = create_texture(numbers_path, true);	if (numbers == -1) return false;
	head = create_texture(head_path, true);	if (head == -1) return false;
	bullets = create_texture(bullets_path, true);	if (bullets== -1) return false;
	hh_head = create_texture(hh_head_path, true);	if (hh_head == -1) return false;
	pause_page = create_texture(pause_page_path, true);	if (pause_page == -1) return false;
	clear_page = create_texture(clear_page_path, true);	if (clear_page == -1) return false;
	fail_page = create_texture(fail_page_path, true);	if (fail_page == -1) return false;
	ground = create_texture(ground_path, true);	if (ground == -1) return false;
	student = create_texture(student_path, true);	if (student== -1) return false;
	mg_head = create_texture(mg_head_path, true);	if (mg_head == -1) return false;
	dy_head = create_texture(dy_head_path, true);	if (dy_head == -1) return false;
	student_arm = create_texture(student_arm_path, true);	if (student_arm == -1) return false;
	prof = create_texture(prof_path, true);	if (prof == -1) return false;
	profarm = create_texture(prof_arm_path, true);	if (profarm == -1) return false;
	aicount = create_texture(aicount_path, true);	if (aicount == -1) return false;

	//bind texture object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, LENA);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, BABOON);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, main_theme);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, help_page);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, skymap);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, numbers);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, head);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, bullets);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, hh_head);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, pause_page);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, clear_page);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, fail_page);
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, ground);
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, student);
	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D,  mg_head);
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, dy_head);
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, student_arm);
	glActiveTexture(GL_TEXTURE17);
	glBindTexture(GL_TEXTURE_2D, prof);
	glActiveTexture(GL_TEXTURE18);
	glBindTexture(GL_TEXTURE_2D, profarm);
	glActiveTexture(GL_TEXTURE19);
	glBindTexture(GL_TEXTURE_2D, aicount);





	return true;
}

void user_finalize()
{
	engine->drop();
}

void play_sound()
{
	engine->play2D(sound_src_ouch, false);
}

void play_death_sound()
{
	engine->play2D(sound_src_ohno, false);
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// init OpenGL extensions

	// initializations and validations of GLSL program
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	//glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	//glfwSetCursorPosCallback( window, motion );		// callback for mouse movements

	t = float(glfwGetTime());	// time init
	float spf = 0.008f;	// sec per frame
	tb = t;

	// enters rendering/event loop
	while( !glfwWindowShouldClose(window) )
	{
		// update global simulation parameter
		t = float(glfwGetTime());	// time init

		glfwPollEvents();	// polling and processing of events
		glUniform1i(glGetUniformLocation(program, "mode"), mode);	// update에서 여기로 옮김

		if (t >= tb)	// 컴퓨터가 빨라서 time interval이 작은 경우에는 문제가 안 됨
		{
			if (screan_mode == 1) update();
			tb += spf;
		}
		render();			// per-frame render

	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
