#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 outColor;

// Values that stay constant for the whole mesh.
uniform sampler2D myTexture;

float cutoff = 0.1;

void main( ) {
	// Output color = color of the texture at the specified UV
	vec4 texel = texture( myTexture, UV );
	if ( texel.a < cutoff ) {
		discard;
	}
	outColor = texel;
}