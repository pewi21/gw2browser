#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 outColor;

// Values that stay constant for the whole mesh.
uniform sampler2D myTexture;

float alpha_threshold = 0.1f;

void main( ) {
	// Output color = color of the texture at the specified UV
	vec4 texel = texture( myTexture, UV );
	if ( texel.a < alpha_threshold ) {
		discard;
	}
	outColor = texel;
}