#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float exposure;

void main( ) {
	//const float gamma = 2.2f;

	vec3 hdrColor = texture( screenTexture, TexCoords ).rgb;
	// Exposure tone mapping
	vec3 result = vec3( 1.0f ) - exp( -hdrColor * exposure );
	// Gamma correction
	//result = pow( result, vec3( 1.0f / gamma ) );

	FragColor = vec4( result, 1.0f );
}
