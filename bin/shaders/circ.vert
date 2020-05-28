// input attributes of vertices
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;

// outputs of vertex shader = input to fragment shader
// out vec4 gl_Position: a built-in output variable that should be written in main()
out vec3 norm;	// the second output: not used yet
out vec2 tc;	// the third output: not used yet

// uniform variables
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform int		screan_mode;		//화면 모드

void main()
{

	if(screan_mode == 0 || screan_mode == 2)		//시작화면0 설명서2
	{
		gl_Position = model_matrix*vec4(position,1);
	}
	else if(screan_mode == 1)	//메인게임
	{
		vec4 wpos = model_matrix * vec4(position,1);
		vec4 epos = view_matrix * wpos;
		gl_Position = projection_matrix * epos;
	}
	

	// pass eye-coordinate normal to fragment shader
	norm = normalize(mat3(view_matrix*model_matrix)*normal);

	tc = texcoord;
}
