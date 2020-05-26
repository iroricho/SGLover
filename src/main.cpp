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
#include "ai.h"			// ai 헤더

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
static const char* lena_image_path = "../bin/images/lena.jpg";
static const char* baboon_image_path = "../bin/images/baboon.jpg";

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program = 0;		// ID holder for GPU program

GLuint  vertex_array = 0;			// ID holder for vertex array object
GLuint	LENA = 0;					// texture object
GLuint	BABOON = 0;					// texture object

//*************************************
// global variables
int		frame = 0;						// index of rendering frames
float	t = 0.0f;						// t는 전체 파일에서 동일하게 쓰이길 요망
float	time_buffer = 0;				// time buffer for resume
bool	halt = 0;
uint	mode = 1;			// texture display mode: 0=texcoord, 1=lena, 2=baboon


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
	if (cam.is_W()) { cam.update_W(); }
	if (cam.is_A()) { cam.update_A(); }
	if (cam.is_S()) { cam.update_S(); }
	if (cam.is_D()) { cam.update_D(); }
	if (cam.is_RIGHT()) { cam.update_RIGHT(); }
	if (cam.is_LEFT()) { cam.update_LEFT(); }
}

void update()
{
	//change texture mode
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	
	
	// update global simulation parameter
	t = halt? t : float(glfwGetTime()) - time_buffer;
	
	// **** update camera
	update_camera();
	
	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );

	// camera의 eye, at, up 은 cameara 헤더에 정의된 함수들이 바꿔줍니다.
	// 때문에 update 때만 view_matrix를 구해주면 됩니다.
	cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	
	// **** update tank
	tank.update_tank(t, cam.eye, cam.at);
	
	// AI move update 탱크 위치를 받기 때문에 tank update 보다 밑에 있어야 함
	ai.update(t, tank.pos);
	
	// Bullet move update
	bullet.update(t, tank.pos);
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, LENA);
	glUniform1i(glGetUniformLocation(program, "TEX0"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, BABOON);
	glUniform1i(glGetUniformLocation(program, "TEX1"), 1);


	// bind vertex array object of cylinder
	glBindVertexArray( vertex_array_0 ); //여기.

	GLint uloc;
	
	// update tank uniforms and draw calls
	uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, tank.model_matrix );
	glDrawElements( GL_TRIANGLES, 4*tank.NTESS*tank.NTESS*3, GL_UNSIGNED_INT, nullptr );

	// update colosseum uniforms and draw calls
	uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, colosseum.model_matrix );
	glDrawElements( GL_TRIANGLES, 4*colosseum.NTESS*colosseum.NTESS*3, GL_UNSIGNED_INT, nullptr );

	//AI 그리기
	glBindVertexArray( vertex_array_0 );
	uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, ai.model_matrix );
	glDrawElements( GL_TRIANGLES, 4*ai.NTESS*ai.NTESS*3, GL_UNSIGNED_INT, nullptr );

	//Bullet 그리기
	glBindVertexArray(vertex_array_0);
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, bullet.model_matrix);
	glDrawElements(GL_TRIANGLES, 4 * bullet.NTESS * bullet.NTESS * 3, GL_UNSIGNED_INT, nullptr);
	
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

		//그냥 텍스쳐매핑할떄 테스트용. 나중에 지워도 됨.
		else if (key == GLFW_KEY_T)
		{
			mode = (mode + 1) % 3;
			printf("using mode %d: %s (press 'd' to toggle)\n", mode,
				mode == 0 ? "texcoord" : mode == 1 ? "lena" : "baboon");
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
	unit_tank_vertices = std::move( create_cyl_vertices( tank.NTESS ));
	unit_colsseum_vertices = std::move( create_cyl_vertices( colosseum.NTESS ));
	unit_ai_vertices = std::move( create_cyl_vertices( ai.NTESS ));
	unit_bullet_vertices = std::move( create_cyl_vertices( bullet.NTESS ));

	/*
	colosseum_bottom_vertices = std::move(bottom.create_colosseum_vertices());		//경기장하부vertice생성
	//colosseum_side_vertices = std::move(side.create_colosseum_side_vertices());		//경기장옆면vertice생성
	*/

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer_cyl( unit_tank_vertices, tank.NTESS );
	update_vertex_buffer_cyl( unit_colsseum_vertices, colosseum.NTESS );
	update_vertex_buffer_cyl( unit_ai_vertices, ai.NTESS);		// AI 버퍼 생성
	update_vertex_buffer_cyl( unit_bullet_vertices, bullet.NTESS);		// bullet 버퍼 생성
	
	/*
	bottom.update_vertex_buffer_colosseum(colosseum_bottom_vertices);	//경기장하부 버퍼 생성
	//side.update_vertex_buffer_colosseum_side(colosseum_side_vertices);	//경기장옆면 버퍼 생성
	*/

	// load the Lena image to a texture
	LENA = create_texture(lena_image_path, true);		if (LENA == -1) return false;
	BABOON = create_texture(baboon_image_path, true);	if (BABOON == -1) return false;

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
		update();			// per-frame update
		render();			// per-frame render
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
