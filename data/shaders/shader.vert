#version 330 core

// Input vertex data, different for all executions of this shader.
layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec2 texCoords;
layout( location = 2 ) in vec3 normal;
layout( location = 3 ) in vec3 tangent;
layout( location = 4 ) in vec3 bitangent;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
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
	vs_out.FragPos = vec3( model * vec4( position, 1.0 ) );
	// UV of the vertex. No special space for this one.
	vs_out.TexCoords = texCoords;

	mat3 normalMatrix = transpose( inverse( mat3( model ) ) );
	vs_out.Normal = normalize( normalMatrix * normal );

	vec3 T = normalize( normalMatrix * tangent );
	vec3 B = normalize( normalMatrix * bitangent );
	vec3 N = normalize( normalMatrix * normal );
	mat3 TBN = transpose( mat3( T, B, N ) );
	vs_out.TBN = TBN;

	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	vs_out.Tangent = T;
	vs_out.Bitangent = B;
}
