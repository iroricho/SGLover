#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "sphere.h"		// slee's OpenGL utility
#include "trackball.h"

//*************************************
// global constants
static const char*	window_name = "2015312406_A3";
static const char*	vert_shader_path = "../bin/shaders/circ.vert";
static const char*	frag_shader_path = "../bin/shaders/circ.frag";
static const uint	MIN_TESS = 3;		// minimum tessellation factor (down to a triangle)
static const uint	MAX_TESS = 256;		// maximum tessellation factor (up to 256 triangles)
uint				NUM_TESS = 80;		// initial tessellation factor of the sphere as a polygon

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program = 0;		// ID holder for GPU program
GLuint	vertex_array = 0;	// ID holder for vertex array object

//*************************************
// global variables
int		frame = 0;						// index of rendering frames
float	t = 0.0f;						// current simulation parameter
float	time_buffer = 0;				// time buffer for resume
bool	halt = 0;
int		b_solid_color = 0;				// what sphere's color?
#ifndef GL_ES_VERSION_2_0
bool	b_wireframe = false;
#endif
auto	spheres = std::move(create_spheres());
struct { bool add=false, sub=false; operator bool() const { return add||sub; } } b; // flags of keys for smooth changes

//*************************************
// holder of vertices and indices of a unit sphere
std::vector<vertex>	unit_sphere_vertices;	// host-side vertices

//*************************************
// common structures
struct camera
{
	vec3	eye = vec3( 0.0f, 0.0f, 5.0f );
	vec3	at = vec3( 0.0f, 0.0f, 0.0f );
	vec3	up = vec3( 0.0f, 1.0f, 0.0f );
	mat4	view_matrix = mat4::look_at( eye, at, up );;

	float	fovy = PI/4.0f; // must be in radian
	float	aspect;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;
};

//*************************************
// scene objects
camera		cam;
trackball	tb;

// utility function
inline vec2 cursor_to_ndc( dvec2 cursor, ivec2 window_size )
{
	// normalize window pos to [0,1]^2
	vec2 npos = vec2( float(cursor.x)/float(window_size.x-1),
					  float(cursor.y)/float(window_size.y-1) );
	
	// normalize window pos to [-1,1]^2 with vertical flipping
	// vertical flipping: window coordinate system defines y from
	// top to bottom, while the trackball from bottom to top
	return vec2(npos.x*2.0f-1.0f,1.0f-npos.y*2.0f);
}

void update()
{
	// update global simulation parameter
	t = halt? t : float(glfwGetTime()) - time_buffer;
	
	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );

	GLint uloc;
	uloc = glGetUniformLocation( program, "b_solid_color" );	if(uloc>-1) glUniform1i( uloc, b_solid_color );
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	
	// update vertex buffer by the pressed keys
	void update_tess(); // forward declaration
	if(b) update_tess(); 
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// bind vertex array object
	glBindVertexArray( vertex_array );

	// render two spheres: trigger shader program to process vertex data
	for( auto& c : spheres )
	{
		// per-sphere update
		c.update(t);

		// update per-sphere uniforms
		GLint uloc;
		uloc = glGetUniformLocation( program, "solid_color" );		if(uloc>-1) glUniform4fv( uloc, 1, c.color );	// pointer version
		uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, c.model_matrix );

		// per-sphere draw calls
		glDrawElements( GL_TRIANGLES, 4*NUM_TESS*NUM_TESS*3, GL_UNSIGNED_INT, nullptr );
	}

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

std::vector<vertex> create_sphere_vertices( uint N )
{
	std::vector<vertex> v;
	
	for (uint j = 0; j <= N; j++)
	{
		for (uint k = 0; k <= N; k++)
		{
			float s = 1.0f,	// radius
				t = PI / 2.0f * (1.0f + float(j) / float(N)),	// theta
				p = 2.0f * PI * float(k) / float(N);	// phi
			v.push_back({ vec3(s * sin(t) * cos(p),s * sin(t) * sin(p),s * cos(t)),	//P spherical
				vec3(sin(t) * cos(p),sin(t) * sin(p),cos(t)),	// Normal vector of spherical
				vec2(p / (2.0f * PI),1.0f - t / PI) });	// Texture of sperical
		}
	}
	
	for (uint j = 0; j <= N; j++)
	{
		for (uint k = 0; k <= N; k++)
		{
			float s = 1.0f,	// radius
				t = PI / 2.0f *  float(j) / float(N),	// theta
				p = 2.0f * PI * float(k) / float(N);	// phi
			v.push_back({ vec3(s * sin(t) * cos(p),s * sin(t) * sin(p),s * cos(t)),	//P spherical
				vec3(sin(t) * cos(p),sin(t) * sin(p),cos(t)),	// Normal vector of spherical
				vec2(p / (2.0f * PI),1.0f - t / PI) });	// Texture of sperical
		}
	}

	return v;
}

void update_vertex_buffer(const std::vector<vertex>& vertices, uint N)
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
	if(vertex_array) glDeleteVertexArrays(1,&vertex_array);
	vertex_array = cg_create_vertex_array( vertex_buffer, index_buffer );
	if(!vertex_array){ printf("%s(): failed to create vertex aray\n",__func__); return; }
}

void update_tess()
{
	uint n = NUM_TESS; if(b.add) n++; if(b.sub) n--;
	if(n==NUM_TESS||n<MIN_TESS||n>MAX_TESS) return;
	
	unit_sphere_vertices = create_sphere_vertices(NUM_TESS=n);
	update_vertex_buffer( unit_sphere_vertices, NUM_TESS );
	printf( "> NUM_TESS = % -4d\r", NUM_TESS );
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
	printf( "- press 'd' to toggle between texture coordinates\n" );
	printf( "- press '+/-' to increase/decrease tessellation factor (min=%d, max=%d)\n", MIN_TESS, MAX_TESS );
#ifndef GL_ES_VERSION_2_0
	printf( "- press 'w' to toggle wireframe\n" );
	printf( "- press 'r' to toggle between halt and resume the sphere\n" );
#endif
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if(key==GLFW_KEY_KP_ADD||(key==GLFW_KEY_EQUAL&&(mods&GLFW_MOD_SHIFT)))	b.add = true;
		else if(key==GLFW_KEY_KP_SUBTRACT||key==GLFW_KEY_MINUS) b.sub = true;
		else if(key==GLFW_KEY_D)
		{
			++b_solid_color;
			b_solid_color %= 5;
			if (b_solid_color == 0) { printf("using (texcoord.xy,0,1) as color\n");  }
			else if (b_solid_color == 1) { printf("using (texcoord.xxx,1) as color\n");  }
			else if (b_solid_color == 2) { printf("using (texcoord.yyy,1) as color\n");  }
			else { printf("using solid_color\n");  }

		}
#ifndef GL_ES_VERSION_2_0
		else if (key == GLFW_KEY_R)
		{
			halt = !halt;
			if (halt) { time_buffer = t; printf("Pause\n"); }
			else { time_buffer = float(glfwGetTime()) - time_buffer; printf("Rotate\n"); }
		}
		
		//******WASD 움직임 begin 함수들*********//
		//사실 인자 아무것도 안넣어줘도 됨. 그냥 복붙한거 고치기 귀찮아서 냅둠.
		else if (key == GLFW_KEY_W) { tb.begin_W(cam.view_matrix,npos); }
		else if (key == GLFW_KEY_A) { tb.begin_A(cam.view_matrix, npos); }
		else if (key == GLFW_KEY_S) { tb.begin_S(cam.view_matrix, npos); }
		else if (key == GLFW_KEY_D) { tb.begin_D(cam.view_matrix, npos); }
		else if (key == GLFW_KEY_RIGHT) { tb.begin_RIGHT(cam.view_matrix, npos); }
		else if (key == GLFW_KEY_LEFT) { tb.begin_LEFT(cam.view_matrix, npos); }

#endif
	}
	else if(action==GLFW_RELEASE)
	{
		if(key==GLFW_KEY_KP_ADD||(key==GLFW_KEY_EQUAL&&(mods&GLFW_MOD_SHIFT)))	b.add = false;
		else if(key==GLFW_KEY_KP_SUBTRACT||key==GLFW_KEY_MINUS) b.sub = false;
		
		
		//******WASD 움직임 end 함수들*********//
		//사실 인자 아무것도 안넣어줘도 됨. 그냥 복붙한거 고치기 귀찮아서 냅둠.
		else if (key == GLFW_KEY_W) { tb.end_W(); }
		else if (key == GLFW_KEY_A) { tb.end_A(); }
		else if (key == GLFW_KEY_S) { tb.end_S(); }
		else if (key == GLFW_KEY_D) { tb.end_D(); }
		else if (key == GLFW_KEY_RIGHT) { tb.end_RIGHT(); }
		else if (key == GLFW_KEY_LEFT) { tb.end_LEFT(); }
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	tb.button = button;
	tb.mods = mods;

	dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
	vec2 npos = cursor_to_ndc( pos, window_size );
	
	if(action==GLFW_PRESS)			tb.begin( npos );
	else if(action==GLFW_RELEASE)	tb.end();
}

void motion( GLFWwindow* window, double x, double y )
{
	if (tb.is_tracking())
	{
		vec2 npos = cursor_to_ndc( dvec2(x,y), window_size );
		if (tb.button==GLFW_MOUSE_BUTTON_LEFT&&tb.mods==0)
		{
			cam.view_matrix = tb.update_rot( npos, cam.eye, cam.at, cam.up );
		}
		else if ((tb.button==GLFW_MOUSE_BUTTON_RIGHT)||((tb.button==GLFW_MOUSE_BUTTON_LEFT)&&(tb.mods&GLFW_MOD_SHIFT)))
		{
			cam.view_matrix = tb.update_zm( npos, cam.eye, cam.at, cam.up );
		}
		else if ((tb.button==GLFW_MOUSE_BUTTON_MIDDLE)||((tb.button==GLFW_MOUSE_BUTTON_LEFT)&&(tb.mods&GLFW_MOD_CONTROL)))
		{
			cam.view_matrix = tb.update_pn( npos, cam.eye, cam.at, cam.up );
		}
	}
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth( 1.0f );
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	
	// define the position of four corner vertices
	unit_sphere_vertices = std::move(create_sphere_vertices( NUM_TESS ));

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer( unit_sphere_vertices, NUM_TESS );

	glPolygonMode( GL_FRONT_AND_BACK, b_wireframe ? GL_LINE:GL_FILL );	// init wireframe
	
	return true;
}

void user_finalize()
{
}

void update_camera() {
	if (tb.is_W()) { cam.view_matrix = tb.update_W(vec2(0)); }
	if (tb.is_A()) { cam.view_matrix = tb.update_A(vec2(0)); }
	if (tb.is_S()) { cam.view_matrix = tb.update_S(vec2(0)); }
	if (tb.is_D()) { cam.view_matrix = tb.update_D(vec2(0)); }
	if (tb.is_RIGHT()) { cam.view_matrix = tb.update_RIGHT(vec2(0)); }
	if (tb.is_LEFT()) { cam.view_matrix = tb.update_LEFT(vec2(0)); }
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
		update_camera();
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
