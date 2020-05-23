#pragma warning(disable: 4819)

#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "sphere.h"		// slee's OpenGL utility
#include "camera.h"
#include "colosseum.h"	// 경기장 헤더 추가
#include "bullet.h"	// bullet 헤더
#include "AI.h"			// AI 헤더

#include <../irrKlang/irrKlang.h>
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
static const char*	window_name = "2015312406_A3";
static const char*	vert_shader_path = "../bin/shaders/circ.vert";
static const char*	frag_shader_path = "../bin/shaders/circ.frag";
uint				NUM_TESS = 80;		// initial tessellation factor of the sphere as a polygon


//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program = 0;		// ID holder for GPU program

//*************************************
// global variables
int		frame = 0;						// index of rendering frames
float	t = 0.0f;						// t는 전체 파일에서 동일하게 쓰이길 요망
float	time_buffer = 0;				// time buffer for resume
bool	halt = 0;
struct { bool add=false, sub=false; operator bool() const { return add||sub; } } b; // flags of keys for smooth changes

irrklang::ISoundEngine* engine = nullptr;
irrklang::ISoundSource* sound_src_1 = nullptr;
irrklang::ISoundSource* sound_src_2 = nullptr;
irrklang::ISoundSource* sound_src_3 = nullptr;


//*************************************
// holder of vertices and indices of a unit sphere
std::vector<vertex>	unit_sphere_vertices;		// host-side vertices
std::vector<vertex>	unit_ai_vertices;			// ai vertices
std::vector<vertex>	colosseum_bottom_vertices;	//경기장 하부 vertices
std::vector<vertex>	colosseum_side_vertices;	//경기장 옆면 vertices

//*************************************
// objects
colosseum_bottom bottom;		//경기장 하부 struct 정의
colosseum_side side;				//경기장 옆면 struct 정의

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

void update()
{
	// update global simulation parameter
	t = halt? t : float(glfwGetTime()) - time_buffer;
	
	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );

	// camera의 eye, at, up 은 cameara 헤더에 정의된 함수들이 바꿔줍니다.
	// 때문에 update 때만 view_matrix를 구해주면 됩니다.
	cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );


	GLint uloc;
	
	// render spheres: trigger shader program to process vertex data
	
	// tank move update
	tank.update(t, cam.eye, cam.at);

	// AI move update 탱크 위치를 받기 때문에 tank update 보다 밑에 있어야 함
	ai.update(t, tank.pos);
	
	// Bullet move update
	bullet.update(t, tank.pos);

	// bind vertex array object of tank
	// update tank uniforms
	// tank draw calls
	glBindVertexArray( vertex_array_1 );
	uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, tank.model_matrix );
	glDrawElements( GL_TRIANGLES, 4*NUM_TESS*NUM_TESS*3, GL_UNSIGNED_INT, nullptr );

	//경기장 하부 그리기
	glBindVertexArray(bottom.vertex_array_3);
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, bottom.model_col);
	glDrawElements(GL_TRIANGLES, 20000, GL_UNSIGNED_INT, nullptr);

	//경기장 옆면 그리기
	glBindVertexArray(side.vertex_array_4);
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, side.model_col);
	glDrawElements(GL_TRIANGLES, 10000, GL_UNSIGNED_INT, nullptr);

	//AI 그리기
	glBindVertexArray( vertex_array_5 );
	uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, ai.model_matrix );
	glDrawElements( GL_TRIANGLES, 4*NUM_TESS*NUM_TESS*3, GL_UNSIGNED_INT, nullptr );

	//Bullet 그리기
	glBindVertexArray(vertex_array_6);
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, bullet.model_matrix);
	glDrawElements(GL_TRIANGLES, 4 * NUM_TESS * NUM_TESS * 3, GL_UNSIGNED_INT, nullptr);

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void update_camera() {
	if (cam.is_W()) { cam.update_W(); }
	if (cam.is_A()) { cam.update_A(); }
	if (cam.is_S()) { cam.update_S(); }
	if (cam.is_D()) { cam.update_D(); }
	if (cam.is_RIGHT()) { cam.update_RIGHT(); }
	if (cam.is_LEFT()) { cam.update_LEFT(); }
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
	printf( "- press 'r' to toggle between halt and resume the sphere\n" );
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
		
		else if (key == GLFW_KEY_W) cam.begin_W();
		else if (key == GLFW_KEY_A) cam.begin_A();
		else if (key == GLFW_KEY_S) cam.begin_S();
		else if (key == GLFW_KEY_D) cam.begin_D();
		else if (key == GLFW_KEY_LEFT) cam.begin_LEFT();
		else if (key == GLFW_KEY_RIGHT) cam.begin_RIGHT();
		else if (key == GLFW_KEY_SPACE) {
			engine->play2D(sound_src_1, false);
			bullet.launch(t, tank.pos, (cam.at - cam.eye).normalize());
			printf("space\n");
		}
	}
	else if(action==GLFW_RELEASE)
	{
		if(key==GLFW_KEY_W) cam.end_W();
		else if(key==GLFW_KEY_A) cam.end_A();
		else if(key==GLFW_KEY_S) cam.end_S();
		else if(key==GLFW_KEY_D) cam.end_D();
		else if(key==GLFW_KEY_LEFT) cam.end_LEFT();
		else if(key==GLFW_KEY_RIGHT) cam.end_RIGHT();
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

bool user_init()
{
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
	
	// define the position of four corner vertices
	unit_sphere_vertices = std::move( create_sphere_vertices( NUM_TESS ));
	unit_ai_vertices = std::move( create_ai_vertices( NUM_TESS ));
	colosseum_bottom_vertices = std::move(bottom.create_colosseum_vertices());		//경기장하부vertice생성
	colosseum_side_vertices = std::move(side.create_colosseum_side_vertices());		//경기장옆면vertice생성

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer_sphere( unit_sphere_vertices, NUM_TESS );
	update_vertex_buffer_ai(unit_sphere_vertices, NUM_TESS);		// AI 버퍼 생성
	update_vertex_buffer_bullet(unit_sphere_vertices, NUM_TESS);		// bullet 버퍼 생성
	bottom.update_vertex_buffer_colosseum(colosseum_bottom_vertices);	//경기장하부 버퍼 생성
	side.update_vertex_buffer_colosseum_side(colosseum_side_vertices);	//경기장옆면 버퍼 생성

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

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update_camera();
		update();			// per-frame update
		render();			// per-frame render
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
