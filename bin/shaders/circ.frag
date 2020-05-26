#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// inputs from vertex shader
in vec3 norm;
in vec2 tc;	// used for texture coordinate visualization

// output of the fragment shader
out vec4 fragColor;

uniform sampler2D	TEX0;
uniform sampler2D	TEX1;
uniform int			mode;

// shader's global variables, called the uniform variables

void main()
{
	if (mode == 1)			fragColor = texture(TEX0, tc);
	else if (mode == 2)	fragColor = texture(TEX1, tc);
	else				fragColor = vec4(tc, 0, 0);
}
