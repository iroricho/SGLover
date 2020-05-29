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
static const char* pause_page_path = "../bin/images/pausepage.jpg";
static const char* clear_page_path = "../bin/images/clearpage.jpg";
static const char* fail_page_path = "../bin/images/failpage.jpg";

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
GLint		pause_page = 0;
GLint		clear_page = 0;
GLint		fail_page = 0;


//*************************************
// global variables
float	t = 0.0f;						// t는 전체 파일에서 동일하게 쓰이길 요망
float	time_buffer = 0;				// time buffer for resume
bool	halt = 0;
uint	mode = 0;			// texture display mode: 0=texcoord, 1=lena, 2=baboon
int		screan_mode = 0;		//타이틀, 게임화면, 앤딩화면 전환용
int		ai_death = 0;			//ai 사망자수
int		game_counter = 0;	//제한시간


irrklang::ISoundEngine* engine = nullptr;
irrklang::ISoundSource* sound_src_1 = nullptr;
irrklang::ISoundSource* sound_src_2 = nullptr;
irrklang::ISoundSource* sound_src_3 = nullptr;


//*************************************
// holder of vertices and indices of a unit tank
std::vector<vertex>	unit_tank_vertices;			// tank vertices
std::vector<vertex>	unit_colsseum_vertices;		// colosseum vertices
std::vector<vertex>	unit_ai_vertices;			// ai vertices
std::vector<vertex>	unit_bullet_vertices;		// bullet vertices
std::vector<vertex>	unit_sky_vertices;		// skyvertices
std::vector<vertex>	main_theme_vertices;	//메인화면vertices
std::vector<vertex>	button_vertices;				//버튼vertices
/*
std::vector<vertex>	colosseum_bottom_vertices;	//경기장 하부 vertices
std::vector<vertex>	colosseum_side_vertices;	//경기장 옆면 vertices

// objects
//colosseum_bottom bottom;		//경기장 하부 객체 생성, initial 값을 그대로 따라감
//colosseum_side side;			//경기장 옆면 객체 생성
*/

//*************************************
// utility function
inline vec2 cursor_to_ndc( dvec2 cursor, ivec2 window_size )
{
	// normalize window pos to [0,1]^2
	vec2 npos = vec2( float(cursor.x)/float(window_size.x-1),
					  float(cursor.y)/float(window_size.y-1) );
	
	// normalize window pos to [-1,1]^2 with vertical flipping
	// vertical flipping: window coordinate system defines y from
	// top to bottom, while the camera from bottom to top
	return vec2(npos.x*2.0f-1.0f,1.0f-npos.y*2.0f);
}

void update_camera() {
	cam.update_Camera();
}

void update()
{
	//change texture mode
	glUniform1i(glGetUniformLocation(program, "mode"), mode);

	if (screan_mode == 1)
	{
		// **** update camera
		update_camera();

		// update projection matrix
		cam.aspect = window_size.x / float(window_size.y);
		cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect, cam.dnear, cam.dfar);

		// camera의 eye, at, up 은 cameara 헤더에 정의된 함수들이 바꿔줍니다.
		// 때문에 update 때만 view_matrix를 구해주면 됩니다.
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

		GLint uloc;
		uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
		uloc = glGetUniformLocation(program, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);

		// **** update tank
		tank.update_tank(t, cam.eye, cam.at);
		tank.update_tank_head(t);
		num_cnt.update_counter(t, cam.eye, cam.at);

		// **** update ai
		for (int i = 0; i < anum; i++)
		{
			ai_t& ai = ais[i];

			// AI move update 탱크 위치를 받기 때문에 tank update 보다 밑에 있어야 함
			//********* 충돌 검사 **********//
			ai.collision(bullet.pos, bullet.radius, bullet.mass);
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
			}

			ai.update(t, tank.pos);
			ai.update_head(t);

			ai_death = ai.death;
		}

		// Bullet move update
		bullet.update(t, tank.pos);

		//Sky move update
		sky.update(t, tank.pos);


		if (ai_death == anum)	screan_mode = 4;	//승리조건
		if (game_counter == 61)	screan_mode = 5;	//패배조건
	}

	
	
	
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	


	// bind vertex array object of cylinder
	//glBindVertexArray( vertex_array_0 ); //여기.

	GLint uloc;


	

	if (screan_mode == 1)
	{
		
		glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 업데이트

		// bind textures
		
		glUniform1i(glGetUniformLocation(program, "TEX0"), 0);

		
		glUniform1i(glGetUniformLocation(program, "TEX1"), 1);

		// update tank uniforms and draw calls
		glBindVertexArray(vertex_array_tank); //여기.
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, tank.model_matrix);
		glDrawElements(GL_TRIANGLES, 4 * tank.NTESS * tank.NTESS * 3, GL_UNSIGNED_INT, nullptr);	//몸통
		glBindVertexArray(vertex_array_2);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 6);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, tank.model_matrix_head);
		glDrawElements(GL_TRIANGLES, 4 * bullet.NTESS * bullet.NTESS * 3, GL_UNSIGNED_INT, nullptr);	//머리

		glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
		// update colosseum uniforms and draw calls
		glBindVertexArray(vertex_array_colosseum);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, colosseum.model_matrix);
		glDrawElements(GL_TRIANGLES, 4 * colosseum.NTESS * colosseum.NTESS * 3, GL_UNSIGNED_INT, nullptr);

		for (int i = 0; i < anum; i++)
		{
			ai_t& ai = ais[i];

			//AI 그리기
			glBindVertexArray(vertex_array_AI);
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, ai.model_matrix);
			glDrawElements(GL_TRIANGLES, 4 * ai.NTESS * ai.NTESS * 3, GL_UNSIGNED_INT, nullptr);	//몸통
			glBindVertexArray(vertex_array_2);
			glUniform1i(glGetUniformLocation(program, "TEX0"), 8);
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, ai.model_matrix_head);
			glDrawElements(GL_TRIANGLES, 4 * bullet.NTESS * bullet.NTESS * 3, GL_UNSIGNED_INT, nullptr);	//머리

		}


		//Bullet 그리기
		glBindVertexArray(vertex_array_2);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 7);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, bullet.model_matrix);
		glDrawElements(GL_TRIANGLES, 4 * bullet.NTESS * bullet.NTESS * 3, GL_UNSIGNED_INT, nullptr);

		//Sky 그리기
		glBindVertexArray(vertex_array_2);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 4);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, sky.model_matrix);
		glDrawElements(GL_TRIANGLES, 4 * bullet.NTESS * bullet.NTESS * 3, GL_UNSIGNED_INT, nullptr);

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

		// swap front and back buffers, and display to screen




	}

	else if (screan_mode == 0)		//초기화면
	{
		halt = 1;
		ai_death = 0;		//데스 초기화
		game_counter = 0;		//시간 카운터 초기화
		glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 update 


		//메인 바탕화면 그리기
		glBindVertexArray(vertex_array_maintheme);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, theme.model_matrix);
		

		
		glUniform1i(glGetUniformLocation(program, "TEX0"), 2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/*
		처음에는 버튼식으로 구현해서 클릭하면 넘기는 걸로 하려 했는데 구현이 어려울거 같아서 일단은 키보드로 구현했습니다
		//시작 버튼 그리기
		glBindVertexArray(vertex_array_button);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, bt_start.model_matrix);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, button_start);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		//설명서 버튼 그리기
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, bt_help.model_matrix);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, button_help);
		glUniform1i(glGetUniformLocation(program, "TEX0"), 4);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		*/



	}

	else if (screan_mode == 2)		//설명서
	{
	halt = 1;
	glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 update 


	//설명서 그리기
	glBindVertexArray(vertex_array_maintheme);
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, theme.model_matrix);


	
	glUniform1i(glGetUniformLocation(program, "TEX0"), 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


	}

	else if (screan_mode == 3)		//일시정지
	{
	halt = 1;
	glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 update 


	//일시정지 화면 그리기
	glBindVertexArray(vertex_array_maintheme);
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, theme.model_matrix);



	glUniform1i(glGetUniformLocation(program, "TEX0"), 9);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


	}

	else if (screan_mode == 4)		//클리어
	{
	halt = 1;
	glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 update 


	//클리어 화면 그리기
	glBindVertexArray(vertex_array_maintheme);
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, theme.model_matrix);



	glUniform1i(glGetUniformLocation(program, "TEX0"), 10);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


	}

	else if (screan_mode == 5)		//실패
	{
	halt = 1;
	glUniform1i(glGetUniformLocation(program, "screan_mode"), screan_mode);		//스크린모드 uniform 최우선 update 


	//일시정지 화면 그리기
	glBindVertexArray(vertex_array_maintheme);
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, theme.model_matrix);



	glUniform1i(glGetUniformLocation(program, "TEX0"), 11);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


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
#ifndef GL_ES_VERSION_2_0
	printf( "- press 'r' to toggle between halt and resume the game\n" );
#endif
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
#ifndef GL_ES_VERSION_2_0
		else if (key == GLFW_KEY_R)
		{
			halt = !halt;
			if (halt) { time_buffer = t; printf("Pause\n"); }
			else { time_buffer = float(glfwGetTime()) - time_buffer; printf("Rotate\n"); }
		}
#endif
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
		}
		else if (key == GLFW_KEY_RIGHT) {
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera('R', flags);
		}


		else if (key == GLFW_KEY_SPACE) {
			engine->play2D(sound_src_1, false);
			bullet.launch(t, tank.pos, (cam.at - cam.eye).normalize());
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
			if (screan_mode == 0 || screan_mode == 2 || screan_mode == 3) { halt = 0;  screan_mode = 1; }
		}
		else if (key == GLFW_KEY_F2)
		{
			if (screan_mode == 0 )	screan_mode = 2;
			
			
		}
		else if (key == GLFW_KEY_I)
		{
			if (screan_mode != 1 )		screan_mode = 0;

		}
		else if (key == GLFW_KEY_P)
		{
			if (screan_mode == 1)		screan_mode = 3;
			else if (screan_mode == 3)	screan_mode = 1;

		}
	



	}
	else if(action==GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W) {
			cam.b_W = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(NULL, flags);
		}
		else if (key == GLFW_KEY_A) {
			cam.b_A = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(NULL, flags);
		}
		else if (key == GLFW_KEY_S) {
			cam.b_S = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(NULL, flags);
		}
		else if (key == GLFW_KEY_D) {
			cam.b_D = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(NULL, flags);
		}
		else if (key == GLFW_KEY_LEFT) {
			cam.b_LEFT = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(NULL, flags);
		}
		else if (key == GLFW_KEY_RIGHT) {
			cam.b_RIGHT = false;
			int flags;
			flags = cam.end_Camera();
			cam.begin_Camera(NULL, flags);
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	cam.button = button;
	cam.mods = mods;

	dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
	vec2 npos = cursor_to_ndc( pos, window_size );
	
	if(action==GLFW_PRESS)			cam.begin( npos );
	else if(action==GLFW_RELEASE)	cam.end();
}

void motion( GLFWwindow* window, double x, double y )
{
	if (cam.is_tracking())
	{
		vec2 npos = cursor_to_ndc( dvec2(x,y), window_size );
		if (cam.button==GLFW_MOUSE_BUTTON_LEFT&&cam.mods==0)
		{
			cam.update_rot( npos );
		}
		else if ((cam.button==GLFW_MOUSE_BUTTON_RIGHT)||((cam.button==GLFW_MOUSE_BUTTON_LEFT)&&(cam.mods&GLFW_MOD_SHIFT)))
		{
			cam.update_zm( npos );
		}
		else if ((cam.button==GLFW_MOUSE_BUTTON_MIDDLE)||((cam.button==GLFW_MOUSE_BUTTON_LEFT)&&(cam.mods&GLFW_MOD_CONTROL)))
		{
			cam.update_pn( npos );
		}
	}
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
	engine->play2D(sound_src_2, true);

	// log hotkeys
	print_help();

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
	unit_bullet_vertices = std::move(create_sphere_vertices(bullet.NTESS));
	unit_sky_vertices = std::move( create_sky_vertices( sky.NTESS ));
	main_theme_vertices = std::move(create_vertices_maintheme());
	button_vertices = std::move(create_vertices_button());
	

	/*
	colosseum_bottom_vertices = std::move(bottom.create_colosseum_vertices());		//경기장하부vertice생성
	//colosseum_side_vertices = std::move(side.create_colosseum_side_vertices());		//경기장옆면vertice생성
	*/

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer_cyltop_colosseum( unit_colsseum_vertices, colosseum.NTESS );
	update_vertex_buffer_cyltop_AI( unit_ai_vertices, ais[0].NTESS);		// AI 버퍼 생성
	update_vertex_buffer_cyltop_tank( unit_tank_vertices, tank.NTESS );
	update_vertex_buffer_sphere(unit_bullet_vertices, bullet.NTESS);		// bullet 버퍼 생성
	update_vertex_buffer_sky( unit_sky_vertices, sky.NTESS);		// bullet 버퍼 생성
	update_vertex_buffer_maintheme(main_theme_vertices);		//메인화면 버퍼 생성
	update_vertex_buffer_button(button_vertices);					//버튼 버퍼 생성
	update_number_vertexbuffers();		//숫자 버퍼 생성
	
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




	return true;
}

void user_finalize()
{
	engine->drop();
}

void play_sound()
{
	engine->play2D(sound_src_3, false);
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
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movements

	t = float(glfwGetTime());	// time init
	float spf = 0.008f;	// sec per frame
	float tb = t;
	
	// enters rendering/event loop
	while( !glfwWindowShouldClose(window) )
	{
		// update global simulation parameter
		t = halt? t : float(glfwGetTime()) - time_buffer;

		glfwPollEvents();	// polling and processing of events

		
		if (screan_mode != 1) { t = float(glfwGetTime());   update(); }	//시작부터 halt가 1이면 정상실행이 안되어서 수정하였습니다.
		else
		{
			if (t >= tb)	// 컴퓨터가 빨라서 time interval이 작은 경우에는 문제가 안 됨
			{
				update();
				tb += spf;
			}
		}
			render();			// per-frame render

		
		
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
