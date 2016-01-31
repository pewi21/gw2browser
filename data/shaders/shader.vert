#version 330 core

// Input vertex data, different for all executions of this shader.
layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec2 texCoords;

// Output data ; will be interpolated for each fragment.
out vec2 TexCoords;

// Values that stay constant for the whole mesh.
//uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main( ) {
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = projection * view * model * vec4( position, 1.0f );

	// UV of the vertex. No special space for this one.
	TexCoords = texCoords;
}
