#version 330 core

out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
	mat3 TBN;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int normalMapping;
uniform int lighting;

float ambientStrength = 0.5f;
float alpha_threshold = 0.1f;

void main( ) {
	vec4 finalColor;
	//mat3 tbn;

	vec3 normal = fs_in.Normal;

	if ( normalMapping ) {
		// Obtain normal from normal map in range [0,1]
		normal = texture( normalMap, fs_in.TexCoords ).rgb;
		// Transform normal vector to range [-1,1]
		normal = normalize( normal * 2.0f - 1.0f );
		// Then transform normal in tangent space to world-space via TBN matrix
		//tbn = mat3( fs_in.Tangent, fs_in.Bitangent, fs_in.Normal ); // TBN calculated in fragment shader
		//normal = normalize( tbn * normal ); // This works!
		//normal = normalize( fs_in.TBN * normal ); // This gives incorrect results
	}

	// Get diffuse color
	vec3 color = texture( diffuseMap, fs_in.TexCoords ).rgb;
	// Ambient
	vec3 ambient = ambientStrength * color;
	// Diffuse
	vec3 lightDir = normalize( lightPos - fs_in.FragPos );
	if ( normalMapping ) {
		lightDir = normalize( fs_in.TangentLightPos - fs_in.TangentFragPos );
	}
	float diff = max( dot( lightDir, normal ), 0.0f );
	vec3 diffuse = diff * color;

	// Specular
	vec3 viewDir = normalize( fs_in.TangentViewPos - fs_in.TangentFragPos );
	vec3 reflectDir = reflect( -lightDir, normal );
	vec3 halfwayDir = normalize( lightDir + viewDir );
	float spec = pow( max( dot( normal, halfwayDir ), 0.0f ), 32.0f );
	vec3 specular = vec3( 0.2f ) * spec;

	if ( lighting ) {
		finalColor = vec4( ambient + diffuse + specular, texture( diffuseMap, fs_in.TexCoords ).a );
	} else {
		finalColor = texture( diffuseMap, fs_in.TexCoords );
	}

	// Alpha test
	if ( finalColor.a < alpha_threshold ) {
		discard;
	}

	FragColor = finalColor;
}
