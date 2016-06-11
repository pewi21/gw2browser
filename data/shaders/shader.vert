#version 330 core

// Input vertex data, different for all executions of this shader.
layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec2 texCoords;
layout( location = 3 ) in vec3 tangent;
layout( location = 4 ) in vec3 bitangent;	// not need

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
	mat3 TBN;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main( ) {
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = projection * view * model * vec4( position, 1.0f );
	// Position of the vertex, in worldspace : M * position
	vs_out.FragPos = vec3( model * vec4( position, 1.0f ) );
	// UV of the vertex. No special space for this one.
	vs_out.TexCoords = texCoords;

	mat3 normalMatrix = transpose( inverse( mat3( model ) ) );
	vs_out.Normal = normalize( normalMatrix * normal );

	vec3 T = normalize( normalMatrix * tangent );
	vec3 N = normalize( normalMatrix * normal );
	// Gram-Schmidt orthogonalize: re-orthogonalize T with respect to N
	T = normalize( T - dot( T, N ) * N );
	// Then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = cross( T, N );
	// Calculate handedness
	if ( dot( cross( T, N ), B ) < 0.0f ) {
		T = T * -1.0f;
	}
	mat3 TBN = mat3( T, B, N );

	vs_out.TBN = TBN;

	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	vs_out.Tangent = T;
	vs_out.Bitangent = B;
}
