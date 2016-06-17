#version 330 core
out vec4 FragColor;

uniform float near;
uniform float far;

float LinearizeDepth( float depth ) {
	float z = depth * 2.0f - 1.0f; // Back to NDC
	return ( 2.0f * near * far ) / ( far + near - z * ( far - near ) );
}

void main( ) {
	float depth = LinearizeDepth( gl_FragCoord.z ) / far; // divide by far to get depth in range [0,1] for visualization purposes.
	FragColor = vec4( vec3( depth ), 1.0f );
}
