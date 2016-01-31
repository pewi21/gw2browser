#version 330 core

// Interpolated values from the vertex shaders
in vec2 TexCoords;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D texture1;

float alpha_threshold = 0.1f;

void main( ) {
	// Output color = color of the texture at the specified UV
	vec4 texColor = texture( texture1, TexCoords );
	if ( texColor.a < alpha_threshold ) {
		discard;
	}
	color = texColor;
}